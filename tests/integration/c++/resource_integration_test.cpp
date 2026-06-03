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

#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <vector>

#define MQSS_HPC_QUEUENAME std::getenv("MQSS_HPC_QUEUENAME")
#define MQSS_API_TOKEN std::getenv("MQSS_API_TOKEN")
#define MQSS_API_URL "https://portal.quantum.lrz.de:4000/v1/"

struct ClientCtorParam {
  std::string token;
  std::string url_or_queue;
  bool isHPC = false;
};

class MQSSClientResourceTest
    : public ::testing::TestWithParam<ClientCtorParam> {
protected:
  void SetUp() override {
    const auto& p = GetParam();
    client = mqss::client::MQSSClient{p.token, p.url_or_queue, p.isHPC};
  }

  mqss::client::MQSSClient client;
};

INSTANTIATE_TEST_SUITE_P(MQSS_Test_Instantiation, MQSSClientResourceTest,
                         ::testing::Values(
                             // ClientCtorParam{CtorKind::Empty},
                             ClientCtorParam{"", MQSS_HPC_QUEUENAME, true},
                             ClientCtorParam{MQSS_API_TOKEN, MQSS_API_URL,
                                             false}));

TEST_P(MQSSClientResourceTest, ClientGetAllResources) {
  std::vector<mqss::client::Resource> resources = client.getAllResources();
  ASSERT_GE(resources.size(), 0);
}

TEST_P(MQSSClientResourceTest, ClientGetAResource) {
  std::vector<std::string> resourceNames = {"QLM", "Q5", "Q20", "AQT20",
                                            "QExa20"};
  for (auto resourceName : resourceNames) {
    std::optional<mqss::client::Resource> resource =
        client.getResourceInfo(resourceName);
    ASSERT_TRUE(resource.has_value());
  }
}

TEST_P(MQSSClientResourceTest, ClientGetAResourceFalse) {
  std::vector<std::string> resourceNames = {"Eviden", "IQM5", "IQM20", "AQT",
                                            "QExa120"};
  for (auto resourceName : resourceNames) {
    std::optional<mqss::client::Resource> resource =
        client.getResourceInfo(resourceName);
    ASSERT_FALSE(resource.has_value());
  }
}

TEST_P(MQSSClientResourceTest, ClientCheckResourceName) {
  std::string goldenResourceName = "QLM";
  std::optional<mqss::client::Resource> resource =
      client.getResourceInfo(goldenResourceName);
  ASSERT_TRUE(resource.has_value());
  ASSERT_EQ((*resource).getName(), goldenResourceName);
}

TEST_P(MQSSClientResourceTest, ClientCheckQubitCount) {
  std::string resourceName = "Q5";
  std::optional<mqss::client::Resource> resource =
      client.getResourceInfo(resourceName);
  ASSERT_TRUE(resource.has_value());
  ASSERT_GE((*resource).getQubitCount(), 0);
}

TEST_P(MQSSClientResourceTest, ClientCheckIfOnline) {
  /* If the resource is under maintenance, this test might fail.*/
  std::string resourceName = "AQT20";
  std::optional<mqss::client::Resource> resource =
      client.getResourceInfo(resourceName);
  ASSERT_TRUE(resource.has_value());
  ASSERT_EQ((*resource).isOnline(), true);
}

TEST_P(MQSSClientResourceTest, ClientCheckCouplingMap) {

  std::string resourceName = "AQT20";
  std::optional<mqss::client::Resource> resource =
      client.getResourceInfo(resourceName);
  ASSERT_TRUE(resource.has_value());
  ASSERT_GE((*resource).getCouplingMap().size(), 0);
}

TEST_P(MQSSClientResourceTest, ClientCheckNativeGateset) {
  std::string resourceName = "Q20";
  std::optional<mqss::client::Resource> resource =
      client.getResourceInfo(resourceName);
  ASSERT_TRUE(resource.has_value());
  ASSERT_GE((*resource).getNativeGateset().size(), 0);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
