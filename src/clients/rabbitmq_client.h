/*
 * Copyright (c) 2024 - 2026 MQSS Project
 * All rights reserved.
 *
 * Licensed under the Apache License v2.0 with LLVM Exceptions (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://llvm.org/LICENSE.txt
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#pragma once

#include <algorithm>
#include <amqp.h>
#include <amqp_tcp_socket.h>
#include <string>
#include <vector>

class MQSSRabbitMQClient {

private:
  amqp_connection_state_t mConnection;
  amqp_socket_t* mpSocket;

  std::string mHostname;
  std::string mUser;
  std::string mPassword;
  int mPort;
  std::vector<std::string> mQueues;

public:
  MQSSRabbitMQClient(std::string user = "guest", std::string password = "guest",
                     std::string hostname = "localhost", int port = 5672)
      : mUser(user), mPassword(password), mHostname(hostname), mPort(port) {};

  ~MQSSRabbitMQClient() { disconnect(); }

  int connect();

  void disconnect();

  int send(const std::string& queue, const std::string& data);

  std::string receive(const std::string& queue);

  int declareQueue(const std::string& queueName);
};
