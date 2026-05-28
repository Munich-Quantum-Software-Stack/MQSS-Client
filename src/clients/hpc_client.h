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

#include "mqss/client.h"
#include "rabbitmq_client.h"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>

using namespace mqss::client;

class MQSSHPCClient : public MQSSBaseClient {
private:
  std::string token;
  std::string mOffloadListenerQueueName;
  std::string mResponseQueueName;

  MQSSRabbitMQClient mRabbitmqClient;

public:
  MQSSHPCClient(std::string token, std::string offloadListenerQueueName);

  std::string get(const std::string& path) override;

  std::string post(const std::string& path,
                   const nlohmann::json& data) override;

  void del(const std::string& path) override;
};
