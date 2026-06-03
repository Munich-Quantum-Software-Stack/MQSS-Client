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

#include "mqss/client.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace mqss::client;
using ::testing::_;
using ::testing::HasSubstr;
using ::testing::Return;

class MockMQSSBaseClient : public MQSSBaseClient {
public:
  MOCK_METHOD(std::string, get, (const std::string& path), (override));

  MOCK_METHOD(std::string, post,
              (const std::string& path, const nlohmann::json& data),
              (override));

  MOCK_METHOD(void, del, (const std::string& path), (override));
};
