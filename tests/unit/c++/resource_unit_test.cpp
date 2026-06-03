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

#include "mock_server.h"
#include "mqss/client.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <string>

namespace {

constexpr char kResourceResponse[] = R"({
  "connectivity":"[[0, 2], [2, 0], [1, 2], [2, 1], [2, 3], [3, 2], [2, 4], [4, 2]]",
  "instructions":"[(\"r\",{(0,):None, (1,):None, (2,):None, (3,):None,(4,):None})]",
  "name":"Fake-Resource2",
  "online":false,
  "qubits":5
})";

constexpr char kAllResourcesResponse[] = R"({
  "Fake-Resource1":{
    "connectivity":"None",
    "instructions":"None",
    "name":"Fake-Resource1",
    "online":false,
    "qubits":20
  },
  "Fake-Resource2":{
    "connectivity":"[[0, 2], [2, 0], [1, 2], [2, 1], [2, 3], [3, 2], [2, 4], [4, 2]]",
    "instructions":"[(\"r\",{(0,):None})]",
    "name":"Fake-Resource2",
    "online":false,
    "qubits":5
  }
})";

constexpr char kNotFoundResponse[] = R"({"RESOURCE NOT FOUND"})";

constexpr char kErrorResponse[] = R"({"ERROR" : "Not Found"})";

constexpr char kNotAcceptedResponse[] = R"()";

} // namespace

class MQSSClientTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock_ = std::make_unique<MockMQSSBaseClient>();
    mock_ptr_ = mock_.get();
  }

  MQSSClient createClient() { return MQSSClient(std::move(mock_)); }

  MockMQSSBaseClient* mock_ptr_{nullptr};
  std::unique_ptr<MockMQSSBaseClient> mock_;
};

TEST_F(MQSSClientTest, GetAllResources) {
  EXPECT_CALL(*mock_ptr_, get("resources"))
      .WillOnce(Return(kAllResourcesResponse));

  auto client = createClient();

  auto resources = client.getAllResources();

  ASSERT_EQ(resources.size(), 2u);
  EXPECT_EQ(resources[0].getName(), "Fake-Resource1");
  EXPECT_EQ(resources[1].getName(), "Fake-Resource2");
}

TEST_F(MQSSClientTest, GetResourceInfoReturnsResource) {
  EXPECT_CALL(*mock_ptr_, get("resources/Fake-Resource2"))
      .WillOnce(Return(kResourceResponse));

  auto client = createClient();

  auto resource = client.getResourceInfo("Fake-Resource2");

  ASSERT_TRUE(resource.has_value());
  EXPECT_EQ(resource->getName(), "Fake-Resource2");
}

TEST_F(MQSSClientTest, GetResourceInfoReturnsNulloptForUnknownResource) {
  EXPECT_CALL(*mock_ptr_, get("resources/mock-resource"))
      .WillOnce(Return(kNotFoundResponse));

  auto client = createClient();

  auto resource = client.getResourceInfo("mock-resource");

  EXPECT_FALSE(resource.has_value());
}

TEST_F(MQSSClientTest, GetResourceInfoReturnsNotAccepted) {
  EXPECT_CALL(*mock_ptr_, get("resources/mock-resource"))
      .WillOnce(Return(kNotAcceptedResponse));

  auto client = createClient();

  auto resource = client.getResourceInfo("mock-resource");

  EXPECT_FALSE(resource.has_value());
}

TEST_F(MQSSClientTest, GetResourceInfoReturnsError) {
  EXPECT_CALL(*mock_ptr_, get("resources/mock-resource"))
      .WillOnce(Return(kErrorResponse));

  auto client = createClient();

  auto resource = client.getResourceInfo("mock-resource");

  EXPECT_FALSE(resource.has_value());
}

TEST_F(MQSSClientTest, ResourceFieldsAreParsedCorrectly) {
  constexpr char kResourceName[] = "Fake-Resource2";

  EXPECT_CALL(*mock_ptr_, get("resources/Fake-Resource2"))
      .WillOnce(Return(kResourceResponse));

  auto client = createClient();

  auto resource = client.getResourceInfo(kResourceName);

  ASSERT_TRUE(resource.has_value());

  EXPECT_EQ(resource->getName(), kResourceName);
  EXPECT_EQ(resource->getQubitCount(), 5);
  EXPECT_FALSE(resource->isOnline());

  EXPECT_GT(resource->getCouplingMap().size(), 0);
  EXPECT_GT(resource->getNativeGateset().size(), 0);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
