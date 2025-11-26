#include "mqss/client.h"
#include <curl/curl.h>
#include <iostream>
#include <string>

#define MQP_DEFAULT_URL "https://portal.quantum.lrz.de:4000/v1"

class MQSS_HPC_Client : public MQSS_Base_Client {

  std::string url;
  std::string token;
  std::string header;

public:
  MQSS_HPC_Client(std::string token, std::string url = MQP_DEFAULT_URL)
      : MQSS_Base_Client() {
    token = token;
    url = url;
  }

  MQSS_HPC_Client() : MQSS_Base_Client() {}

  std::string get(const std::string &path) override {
    return "RETURNS FROM HPC";
  }

  std::string post(const std::string &path, const std::string &data) override {
    return "";
  }
};
