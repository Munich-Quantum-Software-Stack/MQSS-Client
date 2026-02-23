/*------------------------------------------------------------------------------
Copyright 2024 Munich Quantum Software Stack Project

Licensed under the Apache License, Version 2.0 with LLVM Exceptions (the
"License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at

https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
License for the specific language governing permissions and limitations under
the License.

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/

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

class MQSSClientDeviceTest : public ::testing::TestWithParam<ClientCtorParam> {
protected:
  void SetUp() override {
    const auto &p = GetParam();
    client = mqss::client::MQSSClient{p.token, p.url_or_queue, p.isHPC};
  }

  mqss::client::MQSSClient client;
};

INSTANTIATE_TEST_SUITE_P(MQSS_Test_Instantiation, MQSSClientDeviceTest,
                         ::testing::Values(
                             // ClientCtorParam{CtorKind::Empty},
                             ClientCtorParam{"", MQSS_HPC_QUEUENAME, true},
                             ClientCtorParam{MQSS_API_TOKEN, MQSS_API_URL,
                                             false}));

TEST_P(MQSSClientDeviceTest, ClientGetAllResources) {
  std::vector<mqss::client::Device> devices = client.getAllResources();
  ASSERT_GE(devices.size(), 0);
}

TEST_P(MQSSClientDeviceTest, ClientGetAResource) {
  std::vector<std::string> deviceNames = {"QLM", "Q5", "Q20", "AQT20",
                                          "QExa20"};
  for (auto deviceName : deviceNames) {
    std::optional<mqss::client::Device> device =
        client.getResourceInfo(deviceName);
    ASSERT_TRUE(device.has_value());
  }
}

TEST_P(MQSSClientDeviceTest, ClientGetAResourceFalse) {
  std::vector<std::string> deviceNames = {"Eviden", "IQM5", "IQM20", "AQT",
                                          "QExa120"};
  for (auto deviceName : deviceNames) {
    std::optional<mqss::client::Device> device =
        client.getResourceInfo(deviceName);
    ASSERT_FALSE(device.has_value());
  }
}

TEST_P(MQSSClientDeviceTest, ClientCheckResourceName) {
  std::string goldenDeviceName = "QLM";
  std::optional<mqss::client::Device> device =
      client.getResourceInfo(goldenDeviceName);
  ASSERT_TRUE(device.has_value());
  ASSERT_EQ((*device).getName(), goldenDeviceName);
}

TEST_P(MQSSClientDeviceTest, ClientCheckQubitCount) {
  std::string deviceName = "Q5";
  std::optional<mqss::client::Device> device =
      client.getResourceInfo(deviceName);
  ASSERT_TRUE(device.has_value());
  ASSERT_GE((*device).getQubitCount(), 0);
}

TEST_P(MQSSClientDeviceTest, ClientCheckIfOnline) {
  /* If the device is under maintenance, this test might fail.*/
  std::string deviceName = "AQT20";
  std::optional<mqss::client::Device> device =
      client.getResourceInfo(deviceName);
  ASSERT_TRUE(device.has_value());
  ASSERT_EQ((*device).isOnline(), true);
}

TEST_P(MQSSClientDeviceTest, ClientCheckCouplingMap) {

  std::string deviceName = "AQT20";
  std::optional<mqss::client::Device> device =
      client.getResourceInfo(deviceName);
  ASSERT_TRUE(device.has_value());
  ASSERT_GE((*device).getCouplingMap().size(), 0);
}

TEST_P(MQSSClientDeviceTest, ClientCheckNativeGateset) {
  std::string deviceName = "Q20";
  std::optional<mqss::client::Device> device =
      client.getResourceInfo(deviceName);
  ASSERT_TRUE(device.has_value());
  ASSERT_GE((*device).getNativeGateset().size(), 0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
