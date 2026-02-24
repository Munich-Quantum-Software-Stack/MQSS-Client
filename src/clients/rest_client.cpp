
#include "rest_client.h"

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
    return "1";

  MQSSRestResponse response = {0};
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
  curl_easy_setopt(curl, CURLOPT_URL, (mUrl + path).c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, pHeaders);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK)
    return "2";

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

void MQSSRestClient::del(const std::string &path) {

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
