#include "mqss/client.h"
#include <cstring>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <string>
using json = nlohmann::json;

#define MQP_DEFAULT_URL "https://portal.quantum.lrz.de:4000/v1/"

typedef struct MQSS_Rest_Response_d {
  char *response;
  size_t size;
} MQSS_Rest_Response;

static size_t WriteCallback(char *data, size_t size, size_t nmemb,
                            void *clientp) {
  size_t realsize = size * nmemb;
  MQSS_Rest_Response *mem = (MQSS_Rest_Response *)clientp;
  char *ptr = (char *)realloc(mem->response, mem->size + realsize + 1);
  if (!ptr)
    return 0;

  mem->response = ptr;
  void *dest = memcpy(&(mem->response[mem->size]), data, realsize);
  if (dest == NULL)
    fprintf(stderr, "Memory error");
  mem->size += realsize;
  mem->response[mem->size] = 0;

  return realsize;
}

class MQSS_Rest_Client : public MQSS_Base_Client {

  std::string url;
  std::string token;
  std::string header;
  struct curl_slist *headers;

public:
  MQSS_Rest_Client(std::string token, std::string url = MQP_DEFAULT_URL)
      : token(token), url(url), MQSS_Base_Client() {
    std::string auth_header = "Authorization: Bearer " + token;
    headers = curl_slist_append(headers, auth_header.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
  }

  std::string get(const std::string &path) override {
    CURL *curl = curl_easy_init();
    if (!curl)
      return "";

    MQSS_Rest_Response response = {0};

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
    curl_easy_setopt(curl, CURLOPT_URL, (url + path).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
      return "";

    std::string result(response.response, response.size);

    free(response.response);

    return result;
  }

  std::string post(const std::string &path, const std::string &data) override {
    CURL *curl = curl_easy_init();
    if (!curl)
      return "";

    MQSS_Rest_Response response = {0};

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
    curl_easy_setopt(curl, CURLOPT_URL, (url + path).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK || response.response == NULL) {
      return "";
    }

    std::string result(response.response, response.size);
    return result;
  }

  void cancel(const std::string &path) override {

    CURL *curl = curl_easy_init();
    if (!curl)
      return;
    long response_code;
    MQSS_Rest_Response response = {0};

    curl_easy_setopt(curl, CURLOPT_URL, (url + path).c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
      return;
  }
};
