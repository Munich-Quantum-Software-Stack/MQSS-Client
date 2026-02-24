#pragma once

#include "mqss/client.h"
#include <curl/curl.h>
#include <iostream>

using namespace mqss::client;

class MQSSRestClient : public MQSSBaseClient {
private:
  std::string mUrl;
  std::string mToken;
  struct curl_slist *pHeaders;

public:
  MQSSRestClient(std::string token, std::string url = MQP_DEFAULT_URL);

  std::string get(const std::string &path) override;

  std::string post(const std::string &path,
                   const nlohmann::json &data) override;

  void del(const std::string &path) override;
};
