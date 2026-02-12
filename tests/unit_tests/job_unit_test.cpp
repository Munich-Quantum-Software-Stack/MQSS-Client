/*------------------------------------------------------------------------------
Copyright 2024 Munich Quantum Software Stack Project

Licensed under the Apache License, Version 2.0 with LLVM Exceptions (the
"License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at

https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT>
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
        // ClientCtorParam{CtorKind::Empty},
        ClientCtorParam{CtorKind::StringBool, MQSS_HPC_QUEUENAME, "", true},
        ClientCtorParam{CtorKind::StringBool, MQSS_API_TOKEN, "", false},
        ClientCtorParam{CtorKind::StringString, MQSS_API_TOKEN, MQSS_API_URL}));

static const std::string TEST_CIRCUIT = R"(
OPENQASM 2.0;
include "qelib1.inc";
qreg q[2];
creg c[2];
h q[0];
cx q[0], q[1];
measure q -> c;)";

TEST_P(MQSS_Client_Device_Test, ClientSubmitJob) {
  Circuit_Job_Request job =
      Circuit_Job_Request(TEST_CIRCUIT, "qasm", "QLM", 100, 0, 0);
  auto uuid_or_null = client.submitJob(job);
  ASSERT_TRUE(uuid_or_null.has_value());
}

TEST_P(MQSS_Client_Device_Test, ClientCancelJob) {
  if (GetParam().is_hpc)
    GTEST_SKIP();
  Circuit_Job_Request job =
      Circuit_Job_Request(TEST_CIRCUIT, "qasm", "QLM", 100, 0, 0);
  auto uuid_or_null = client.submitJob(job);
  ASSERT_TRUE(uuid_or_null.has_value());
  std::cout << *uuid_or_null << "\n";
  client.cancelJob(job);
  ASSERT_STREQ(client.getJobStatus(job).c_str(), "CANCELLED");
}

TEST_P(MQSS_Client_Device_Test, ClientSubmitHamiltonianJob) {
  if (GetParam().is_hpc)
    GTEST_SKIP();
  Hamiltonian_Job_Request job =
      Hamiltonian_Job_Request("QLM", "0 1; 1 2; 0 2; 0 3;", "0.5 0.1 0.8 1;");
  auto uuid_or_null = client.submitJob(job);
  ASSERT_TRUE(uuid_or_null.has_value());
}

TEST_P(MQSS_Client_Device_Test, ClientCheckJobStatus) {
  Circuit_Job_Request job =
      Circuit_Job_Request(TEST_CIRCUIT, "qasm", "QLM", 100, 0, 0);
  auto uuid_or_null = client.submitJob(job);
  ASSERT_TRUE(uuid_or_null.has_value());
  std::string status = client.getJobStatus(job);
  ASSERT_STRNE(status.c_str(), "");
}

TEST_P(MQSS_Client_Device_Test, ClientCheckJobSetterAndGetter) {
  Circuit_Job_Request job = Circuit_Job_Request();
  std::string CircuitFormat("qasm");
  std::string ResourceName("AQT20");
  unsigned int shots = 10;
  bool isQueued = false;
  bool isNoModify = false;

  job.setCircuit(TEST_CIRCUIT);
  ASSERT_STREQ(job.getCircuit().c_str(), TEST_CIRCUIT.c_str());

  job.setCircuitFormat(CircuitFormat);
  ASSERT_STREQ(job.getCircuitFormat().c_str(), CircuitFormat.c_str());

  job.setResourceName(ResourceName);
  ASSERT_STREQ(job.getResourceName().c_str(), ResourceName.c_str());

  job.setShots(shots);
  ASSERT_EQ(job.getShots(), shots);

  job.setNoModify(isNoModify);
  ASSERT_EQ(job.isNoModify(), isNoModify);

  job.setQueued(isQueued);
  ASSERT_EQ(job.isQueued(), isQueued);
}

TEST_P(MQSS_Client_Device_Test, ClientCheckHamiltonianJobSetterAndGetter) {
  Hamiltonian_Job_Request job = Hamiltonian_Job_Request();
  std::string CoefficientsString("0.5 0.1 0.8 1;");
  std::string Interaction_str("0 1; 1 2; 0 2; 0 3;");

  job.setCoefficientsString(CoefficientsString);
  ASSERT_STREQ(job.getCoefficientsString().c_str(), CoefficientsString.c_str());

  job.setInteractionString(Interaction_str);
  ASSERT_STREQ(job.getInteractionString().c_str(), Interaction_str.c_str());
}

TEST_P(MQSS_Client_Device_Test, ClientWaitForResult) {
  if (GetParam().is_hpc)
    GTEST_SKIP();
  Circuit_Job_Request job =
      Circuit_Job_Request(TEST_CIRCUIT, "qasm", "QLM", 100, 0, 0);
  auto uuid_or_null = client.submitJob(job);
  ASSERT_TRUE(uuid_or_null.has_value());
  auto result = client.waitForJobResult(job);
  ASSERT_NE(result, nullptr);
  ASSERT_NE(result->results.size(), 0);
}

TEST_P(MQSS_Client_Device_Test, ClientGetNumPendingJobs) {
  Circuit_Job_Request job =
      Circuit_Job_Request(TEST_CIRCUIT, "qasm", "QLM", 100, 0, 0);
  auto uuid_or_null = client.submitJob(job);
  ASSERT_TRUE(uuid_or_null.has_value());
  int n_job = client.getNumberPendingJobs("QLM");
  ASSERT_GE(n_job, 0);
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}