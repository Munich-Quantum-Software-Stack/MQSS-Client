
#include "hpc_client.h"
#include "rabbitmq_client.h"
#include "rest_client.h"

#include <stdexcept>

namespace {

typedef struct MQSSRestResponse_d {
  char *pResponse;
  size_t mSize;
} MQSSRestResponse;

static size_t writeCallback(char *data, size_t size, size_t nmemb,
                            void *clientp) {
  size_t realsize = size * nmemb;
  MQSSRestResponse *pResponse = (MQSSRestResponse *)clientp;
  char *pTempResponse =
      (char *)realloc(pResponse->pResponse, pResponse->mSize + realsize + 1);
  if (!pTempResponse)
    return 0;

  pResponse->pResponse = pTempResponse;
  void *dest =
      memcpy(&(pResponse->pResponse[pResponse->mSize]), data, realsize);
  if (dest == NULL)
    fprintf(stderr, "Memory error");
  pResponse->mSize += realsize;
  pResponse->pResponse[pResponse->mSize] = 0;

  return realsize;
}

inline std::string getHostName() {
  size_t max_hostname_size = sysconf(_SC_HOST_NAME_MAX);
  if (max_hostname_size == -1) {
    max_hostname_size = 256;
  }
  char *hostname = new char[max_hostname_size];
  if (gethostname(hostname, max_hostname_size) == 0) {
    std::string result(hostname);
    delete[] hostname;
    return result;
  } else {
    delete[] hostname;
    return "";
  }
}

} // namespace

int MQSSRabbitMQClient::connect() {

  mConnection = amqp_new_connection();
  mpSocket = amqp_tcp_socket_new(mConnection);
  if (!mpSocket)
    return 1;

  if (amqp_socket_open(mpSocket, mHostname.c_str(), mPort))
    return 1;

  amqp_rpc_reply_t reply =
      amqp_login(mConnection, "/", 0, AMQP_DEFAULT_FRAME_SIZE, 60,
                 AMQP_SASL_METHOD_PLAIN, mUser.c_str(), mPassword.c_str());

  if (reply.reply_type != AMQP_RESPONSE_NORMAL)
    return 1;

  amqp_channel_open(mConnection, 1);
  reply = amqp_get_rpc_reply(mConnection);

  if (reply.reply_type != AMQP_RESPONSE_NORMAL)
    return 1;

  return 0;
}

void MQSSRabbitMQClient::disconnect() {
  amqp_channel_close(mConnection, 1, AMQP_REPLY_SUCCESS);
  amqp_connection_close(mConnection, AMQP_REPLY_SUCCESS);
  amqp_destroy_connection(mConnection);
}

int MQSSRabbitMQClient::send(const std::string &queue,
                             const std::string &data) {
  amqp_basic_properties_t props;
  props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
  props.content_type = amqp_cstring_bytes("text/plain");
  props.delivery_mode = 2;
  return amqp_basic_publish(mConnection, 1, amqp_empty_bytes,
                            amqp_cstring_bytes(queue.c_str()), 0, 0, &props,
                            amqp_cstring_bytes(data.c_str()));
}

std::string MQSSRabbitMQClient::receive(const std::string &queue) {
  declareQueue(queue);

  amqp_basic_consume(mConnection, 1, amqp_cstring_bytes(queue.c_str()),
                     amqp_empty_bytes, 0, 0, 0, amqp_empty_table);

  amqp_envelope_t envelope;
  amqp_rpc_reply_t reply =
      amqp_consume_message(mConnection, &envelope, NULL, 0);

  if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
    return amqp_error_string2(reply.library_error);
  }

  std::string body(static_cast<char *>(envelope.message.body.bytes),
                   envelope.message.body.len);

  amqp_basic_ack(mConnection, 1, envelope.delivery_tag, false);
  amqp_destroy_envelope(&envelope);

  return body;
}

int MQSSRabbitMQClient::declareQueue(const std::string &queueName) {
  if (std::find(mQueues.begin(), mQueues.end(), queueName) != mQueues.end())
    return 0;

  amqp_queue_declare(mConnection, 1, amqp_cstring_bytes(queueName.c_str()), 0,
                     0, 0, 0, amqp_empty_table);
  mQueues.push_back(queueName);
  return 0;
}

MQSSHPCClient::MQSSHPCClient(std::string offloadListenerQueueName,
                             std::string responseQueueName)
    : mOffloadListenerQueueName(offloadListenerQueueName.empty()
                                    ? "qoffload_api_request_reception_queue_" +
                                          getHostName()
                                    : offloadListenerQueueName),
      mResponseQueueName(
          responseQueueName.empty()
              ? "response_queue_" + getHostName() + "_" +
                    boost::uuids::to_string(boost::uuids::random_generator()())
                        .substr(0, 8)
              : responseQueueName),
      mRabbitmqClient(), MQSSBaseClient() {

  int isErr = mRabbitmqClient.connect();
  if (isErr) {
    throw std::runtime_error("Error: Failed to connect RabbiitMQ");
  }
  mRabbitmqClient.declareQueue(mOffloadListenerQueueName);
  mRabbitmqClient.declareQueue(mResponseQueueName);
}

std::string MQSSHPCClient::get(const std::string &path) {
  std::string request, response;
  nlohmann::json request_json = {
      {"authorization", ""},
      {"method", "GET"},
      {"request", path},
      {"data", ""},
      {"response_queue", mResponseQueueName},
  };
  int err =
      mRabbitmqClient.send(mOffloadListenerQueueName, request_json.dump());
  response = mRabbitmqClient.receive(mResponseQueueName);
  return response;
}

std::string MQSSHPCClient::post(const std::string &path,
                                const nlohmann::json &data) {
  nlohmann::json request_json = {
      {"authorization", ""},
      {"method", "POST"},
      {"request", path},
      {"data", data},
      {"response_queue", mResponseQueueName},
  };

  mRabbitmqClient.send(mResponseQueueName, request_json.dump());
  std::string response = mRabbitmqClient.receive(mResponseQueueName);

  return response;
}

void MQSSHPCClient::cancel(const std::string &path) {
  std::string request, response;
  mRabbitmqClient.send(mOffloadListenerQueueName, request);
  response = mRabbitmqClient.receive(mResponseQueueName);
}

MQSSRestClient::MQSSRestClient(std::string token, std::string url)
    : mToken(token), mUrl(url), MQSSBaseClient() {
  pHeaders = nullptr;
  std::string auth_header = "Authorization: Bearer " + token;
  pHeaders = curl_slist_append(pHeaders, auth_header.c_str());
  pHeaders = curl_slist_append(pHeaders, "Content-Type: application/json");
}

std::string MQSSRestClient::get(const std::string &path) {
  CURL *curl = curl_easy_init();
  if (!curl)
    return "";

  MQSSRestResponse response = {0};

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
  curl_easy_setopt(curl, CURLOPT_URL, (mUrl + path).c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, pHeaders);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK)
    return "";

  std::string result(response.pResponse, response.mSize);
  if (response.pResponse)
    free(response.pResponse);

  return result;
}

std::string MQSSRestClient::post(const std::string &path,
                                 const nlohmann::json &data) {

  CURL *curl = curl_easy_init();
  if (!curl)
    return "";

  MQSSRestResponse response = {0};

  std::string payload = data.dump();

  curl_easy_setopt(curl, CURLOPT_URL, (mUrl + path).c_str());
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, pHeaders);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

  CURLcode res = curl_easy_perform(curl);
  curl_easy_cleanup(curl);
  if (res != CURLE_OK || response.pResponse == NULL) {
    return "";
  }

  std::string result(response.pResponse, response.mSize);
  return result;
}

void MQSSRestClient::cancel(const std::string &path) {

  CURL *curl = curl_easy_init();
  if (!curl)
    return;
  long response_code;
  MQSSRestResponse response = {0};

  curl_easy_setopt(curl, CURLOPT_URL, (mUrl + path).c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, pHeaders);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK)
    return;
}