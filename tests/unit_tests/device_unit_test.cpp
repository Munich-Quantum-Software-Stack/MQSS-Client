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
#include <cstddef>
#include <cstdlib>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

#define MQSS_HPC_QUEUENAME std::getenv("MQSS_HPC_QUEUENAME")
#define MQSS_API_TOKEN std::getenv("MQSS_API_TOKEN")
#define MQSS_API_URL "https://portal.quantum.lrz.de:4000/v1/"

enum class CtorKind {
  Empty,
  StringBool,
  StringString,
};

struct ClientCtorParam {
  CtorKind kind;
  std::string token_or_queue;
  std::string url;
  bool is_hpc = false;
};

class MQSS_Client_Device_Test
    : public ::testing::TestWithParam<ClientCtorParam> {
protected:
  void SetUp() override {
    const auto &p = GetParam();

    switch (p.kind) {
    case CtorKind::Empty:
      client = MQSS_Client{};
      break;

    case CtorKind::StringBool:
      client = MQSS_Client{p.token_or_queue, p.is_hpc};
      break;

    case CtorKind::StringString:
      client = MQSS_Client{p.token_or_queue, p.url};
      break;
    }
  }

  MQSS_Client client;
};

INSTANTIATE_TEST_SUITE_P(
    MQSS_Test_Instantiation, MQSS_Client_Device_Test,
    ::testing::Values(
        //ClientCtorParam{CtorKind::Empty},
        ClientCtorParam{CtorKind::StringBool, MQSS_HPC_QUEUENAME, "", true},
        ClientCtorParam{CtorKind::StringBool, MQSS_API_TOKEN, "", false},
        ClientCtorParam{CtorKind::StringString, MQSS_API_TOKEN, MQSS_API_URL}
        ));

TEST_P(MQSS_Client_Device_Test, ClientGetAllResources) {
  std::vector<Device> devices = client.getAllResources();
  ASSERT_GE(devices.size(), 0);
}


TEST_P(MQSS_Client_Device_Test, ClientGetAResource) {
  std::vector<std::string> device_names = {"QLM", "Q5", "Q20", "AQT20",
                                           "QExa20"};
  for (auto device_name : device_names) {
    std::optional<Device> device = client.getResourceInfo(device_name);
    ASSERT_TRUE(device.has_value());
  }
}

TEST_P(MQSS_Client_Device_Test, ClientGetAResourceFalse) {
  std::vector<std::string> device_names = {"Eviden", "IQM5", "IQM20", "AQT",
                                           "QExa120"};
  for (auto device_name : device_names) {
    std::optional<Device> device = client.getResourceInfo(device_name);
    ASSERT_FALSE(device.has_value());
  }
}

TEST_P(MQSS_Client_Device_Test, ClientCheckResourceName) {
  std::string golden_device_name = "QLM";
  std::optional<Device> device = client.getResourceInfo(golden_device_name);
  ASSERT_TRUE(device.has_value());
  ASSERT_EQ((*device).name(), golden_device_name);
}

TEST_P(MQSS_Client_Device_Test, ClientCheckQubitCount) {
  std::string device_name = "Q5";
  std::optional<Device> device = client.getResourceInfo(device_name);
  ASSERT_TRUE(device.has_value());
  ASSERT_GE((*device).qubit_count(), 0);
}

TEST_P(MQSS_Client_Device_Test, ClientCheckIfOnline) {
  /* If the device is under maintenance, this test might fail.*/
  std::string device_name = "AQT20";
  std::optional<Device> device = client.getResourceInfo(device_name);
  ASSERT_TRUE(device.has_value());
  ASSERT_EQ((*device).online(), true);
}

TEST_P(MQSS_Client_Device_Test, ClientCheckCouplingMap) {

  std::string device_name = "AQT20";
  std::optional<Device> device = client.getResourceInfo(device_name);
  ASSERT_TRUE(device.has_value());
  ASSERT_GE((*device).coupling_map().size(), 0);
}

TEST_P(MQSS_Client_Device_Test, ClientCheckNativeGateset) {
  std::string device_name = "Q20";
  std::optional<Device> device = client.getResourceInfo(device_name);
  ASSERT_TRUE(device.has_value());
  ASSERT_GE((*device).native_gateset().size(), 0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
