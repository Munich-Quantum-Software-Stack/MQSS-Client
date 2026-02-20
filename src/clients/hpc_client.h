#pragma once

#include "mqss/client.h"
#include "rabbitmq_client.h"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>

using namespace mqss::client;

class MQSSHPCClient : public MQSSBaseClient {
private:
  std::string mOffloadListenerQueueName;
  std::string mResponseQueueName;

  MQSSRabbitMQClient mRabbitmqClient;

public:

  MQSSHPCClient(std::string offloadListenerQueueName = "",
                std::string responseQueueName = "");

  // MQSS_HPC_Client() : MQSSBaseClient() {}

  std::string get(const std::string &path) override;

  std::string post(const std::string &path,
                   const nlohmann::json &data) override;

  void cancel(const std::string &path) override;
};
