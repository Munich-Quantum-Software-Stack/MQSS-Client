#pragma once

#include <amqp.h>
#include <amqp_tcp_socket.h>

#include <algorithm>
#include <string>
#include <vector>

class MQSSRabbitMQClient {

private:
  amqp_connection_state_t mConnection;
  amqp_socket_t *mpSocket;

  std::string mHostname;
  std::string mUser;
  std::string mPassword;
  int mPort;
  std::vector<std::string> mQueues;

public:
  MQSSRabbitMQClient(std::string user = "guest",
                       std::string password = "guest",
                       std::string hostname = "host.docker.internal",
                       int port = 5672)
      : mUser(user), mPassword(password), mHostname(hostname), mPort(port){};

  ~MQSSRabbitMQClient() { disconnect(); }

  int connect();

  void disconnect();

  int send(const std::string& queue, const std::string& data);

  std::string receive(const std::string& queue);

  int declareQueue(const std::string& queueName);
};
