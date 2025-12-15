#include "mqss/client.h"
#include "rabbitmq_client.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
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
  std::string offload_listerner_queue_name;
  std::string response_queue_name;

  MQSS_RabbitMQ_Client rabbitmq_client;

public:
  MQSS_HPC_Client(std::string offload_listerner_queue_name = "",
                  std::string response_queue_name = "")
      : offload_listerner_queue_name(
            offload_listerner_queue_name.empty()
                ? "qoffload_api_request_reception_queue_" + _getHostName()
                : offload_listerner_queue_name),
        response_queue_name(response_queue_name.empty()
                                ? "response_queue_" + _getHostName() + "_" +
                                      boost::uuids::to_string(
                                          boost::uuids::random_generator()())
                                          .substr(0, 8)
                                : response_queue_name),
        rabbitmq_client(), MQSS_Base_Client() {
    rabbitmq_client.connect();
    rabbitmq_client.declare_queue(offload_listerner_queue_name);
    rabbitmq_client.declare_queue(response_queue_name);
  }

  //MQSS_HPC_Client() : MQSS_Base_Client() {}

  std::string get(const std::string &path) override {
    std::string request, response;
    rabbitmq_client.send(offload_listerner_queue_name, request);
    response = rabbitmq_client.receive(response_queue_name);
    return response;
  }

  std::string post(const std::string &path, const std::string &data) override {
    rabbitmq_client.send(offload_listerner_queue_name, data);
    std::string response = rabbitmq_client.receive(response_queue_name);
    return response;
  }

  void cancel(const std::string &path) override {
    std::string request, response;
    rabbitmq_client.send(offload_listerner_queue_name, request);
    response = rabbitmq_client.receive(response_queue_name);
  }
};
