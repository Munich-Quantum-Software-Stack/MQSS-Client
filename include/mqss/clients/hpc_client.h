#include "mqss/client.h"
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <unistd.h>

std::string _getHostName() {
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

class MQSS_HPC_Client : public MQSS_Base_Client {
  std::string queue_name;
  std::string token;

public:
  // Constructor with default queue_name based on hostname
  MQSS_HPC_Client(std::string token, std::string queue_name = "")
      : token(token),
        queue_name(queue_name.empty() ? _getHostName() : queue_name),
        MQSS_Base_Client() {}

  MQSS_HPC_Client() : MQSS_Base_Client() {}

  std::string get(const std::string &path) override {
    // Implement the actual get logic
    return "";
  }

  std::string post(const std::string &path, const std::string &data) override {
    // Implement the actual post logic
    return "";
  }

  void cancel(const std::string &path) override {
    // Implement the cancel logic
  }
};
