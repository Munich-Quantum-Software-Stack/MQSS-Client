#include "hpc_client.h"

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

MQSSHPCClient::MQSSHPCClient(std::string token,
                             std::string offloadListenerQueueName)
    : mOffloadListenerQueueName(offloadListenerQueueName.empty()
                                    ? "qoffload_api_request_reception_queue_" +
                                          getHostName()
                                    : offloadListenerQueueName) {
  mResponseQueueName =
      "response_queue_" + getHostName() + "_" +
      boost::uuids::to_string(boost::uuids::random_generator()()).substr(0, 8);
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

  mRabbitmqClient.send(mOffloadListenerQueueName, request_json.dump());
  std::string response = mRabbitmqClient.receive(mResponseQueueName);

  return response;
}

void MQSSHPCClient::cancel(const std::string &path) {
  std::string request, response;
  mRabbitmqClient.send(mOffloadListenerQueueName, request);
  response = mRabbitmqClient.receive(mResponseQueueName);
}