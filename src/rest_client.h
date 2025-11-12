#include "mqss/client.h"
#include <cstring>
#include <curl/curl.h>
#include <iostream>
#include <string>

#define MQP_DEFAULT_URL "https://portal.quantum.lrz.de:4000/v1"

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
      : MQSS_Base_Client() {
    token = token;
    url = url;

    std::string auth_header = "Authorization: Bearer " + token;
    headers = curl_slist_append(headers, auth_header.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
  }

  std::string get(const std::string &path) override {
    return "";
  }
};
