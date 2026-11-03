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

#include <chrono>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <string>

namespace {

constexpr char TEST_CIRCUIT[] = R"(
OPENQASM 2.0;
include "qelib1.inc";
qreg q[2];
creg c[2];
h q[0];
cx q[0], q[1];
measure q -> c;
)";

constexpr char kSubmitSuccess[] = R"({
    "uuid":"12345"
})";

constexpr char kStatusCompleted[] = R"({
    "status":"COMPLETED"
})";

constexpr char kStatusRunning[] = R"({
    "status":"RUNNING"
})";

constexpr char kStatusFailed[] = R"({
    "status":"FAILED"
})";

constexpr char kResultArrayResponse[] =
    R"({"result":"[{\"10\":100},{\"10\":200}]","timestamp_completed":"2026-05-26 11:26:07.615381","timestamp_scheduled":"2026-05-26 08:07:40.026325","timestamp_submitted":"2026-05-25 08:52:14.503964"})";

constexpr char kResultResponse[] =
    R"({"result":"{\"10\":200}","timestamp_completed":"2026-05-26 11:26:07.615381","timestamp_scheduled":"2026-05-26 08:07:40.026325","timestamp_submitted":"2026-05-25 08:52:14.503964"})";

constexpr char kPendingJobsResponse[] = R"({
    "num_pending_jobs":42
})";

} // namespace

class MQSSClientTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock_ = std::make_unique<MockMQSSBaseClient>();
    mock_ptr_ = mock_.get();
  }

  MQSSClient createClient() { return MQSSClient(std::move(mock_)); }

  CircuitJobRequest createCircuitJob() {
    return CircuitJobRequest(TEST_CIRCUIT, "qasm", "mock-resource", 100, 0, 0);
  }

  HamiltonianJobRequest createHamiltonianJob() {
    return HamiltonianJobRequest("mock-resource", "Z0 Z1\nX0 X1", "0.5\n0.3");
  }

  MockMQSSBaseClient* mock_ptr_{nullptr};
  std::unique_ptr<MockMQSSBaseClient> mock_;
};

TEST_F(MQSSClientTest, SubmitJobSuccess) {
  auto job = createCircuitJob();

  EXPECT_CALL(*mock_ptr_, post(job.getPath(), job.toJson(false)))
      .WillOnce(Return(kSubmitSuccess));

  auto client = createClient();

  auto uuid = client.submitJob(job);

  ASSERT_TRUE(uuid.has_value());
  EXPECT_EQ(*uuid, "12345");
  EXPECT_EQ(job.getUuid(), "12345");
}

TEST_F(MQSSClientTest, SubmitJobWithExplicitHpcTransport) {
  auto job = createCircuitJob();
  EXPECT_CALL(*mock_ptr_, post(job.getPath(), job.toJson(true)))
      .WillOnce(Return(kSubmitSuccess));
  MQSSClient client(std::move(mock_), true);
  EXPECT_EQ(client.submitJob(job), std::optional<std::string>("12345"));
}

TEST_F(MQSSClientTest, SubmitJobEmptyResponse) {
  auto job = createCircuitJob();

  EXPECT_CALL(*mock_ptr_, post(job.getPath(), job.toJson(false)))
      .WillOnce(Return(""));

  auto client = createClient();

  EXPECT_FALSE(client.submitJob(job).has_value());
}

TEST_F(MQSSClientTest, SubmitJobInvalidJson) {
  auto job = createCircuitJob();

  EXPECT_CALL(*mock_ptr_, post(job.getPath(), job.toJson(false)))
      .WillOnce(Return("not-json"));

  auto client = createClient();

  EXPECT_FALSE(client.submitJob(job).has_value());
}

TEST_F(MQSSClientTest, SubmitJobMissingUuid) {
  auto job = createCircuitJob();

  EXPECT_CALL(*mock_ptr_, post(job.getPath(), job.toJson(false)))
      .WillOnce(Return(R"({"status":"ok"})"));

  auto client = createClient();

  EXPECT_FALSE(client.submitJob(job).has_value());
}

TEST_F(MQSSClientTest, CancelJob) {
  auto job = createCircuitJob();
  job.setUuid("12345");

  EXPECT_CALL(*mock_ptr_, del(job.getPath() + "/12345")).Times(1);

  auto client = createClient();

  client.cancelJob(job);
}

TEST_F(MQSSClientTest, GetJobStatusCompleted) {
  auto job = createCircuitJob();
  job.setUuid("12345");

  EXPECT_CALL(*mock_ptr_, get(job.getPath() + "/12345/status"))
      .WillOnce(Return(kStatusCompleted));

  auto client = createClient();

  EXPECT_EQ(client.getJobStatus(job), "COMPLETED");
}

TEST_F(MQSSClientTest, GetJobStatusFailed) {
  auto job = createCircuitJob();
  job.setUuid("12345");

  EXPECT_CALL(*mock_ptr_, get(job.getPath() + "/12345/status"))
      .WillOnce(Return(kStatusFailed));

  auto client = createClient();

  EXPECT_EQ(client.getJobStatus(job), "FAILED");
}

TEST_F(MQSSClientTest, GetJobStatusEmptyResponse) {
  auto job = createCircuitJob();
  job.setUuid("12345");

  EXPECT_CALL(*mock_ptr_, get(_)).WillOnce(Return(""));

  auto client = createClient();

  EXPECT_EQ(client.getJobStatus(job), "");
}

TEST_F(MQSSClientTest, GetJobStatusInvalidJson) {
  auto job = createCircuitJob();
  job.setUuid("12345");

  EXPECT_CALL(*mock_ptr_, get(_)).WillOnce(Return("bad-json"));

  auto client = createClient();

  EXPECT_EQ(client.getJobStatus(job), "");
}

TEST_F(MQSSClientTest, GetJobStatusMissingField) {
  auto job = createCircuitJob();
  job.setUuid("12345");

  EXPECT_CALL(*mock_ptr_, get(_)).WillOnce(Return(R"({})"));

  auto client = createClient();

  EXPECT_EQ(client.getJobStatus(job), "");
}

TEST_F(MQSSClientTest, GetJobResultSuccess) {
  auto job = createCircuitJob();
  job.setUuid("12345");

  EXPECT_CALL(*mock_ptr_, get(job.getPath() + "/12345/status"))
      .WillOnce(Return(kStatusCompleted));

  EXPECT_CALL(*mock_ptr_, get(job.getPath() + "/12345/result"))
      .WillOnce(Return(kResultResponse));

  auto client = createClient();

  auto result = client.getJobResult(job, true);

  ASSERT_NE(result, nullptr);
}

TEST_F(MQSSClientTest, GetJobResultArraySuccess) {
  auto job = createCircuitJob();
  job.setUuid("12345");

  EXPECT_CALL(*mock_ptr_, get(job.getPath() + "/12345/status"))
      .WillOnce(Return(kStatusCompleted));

  EXPECT_CALL(*mock_ptr_, get(job.getPath() + "/12345/result"))
      .WillOnce(Return(kResultArrayResponse));

  auto client = createClient();

  auto result = client.getJobResult(job, true);

  ASSERT_NE(result, nullptr);
}

TEST_F(MQSSClientTest, GetJobResultEmptyResponse) {
  auto job = createCircuitJob();
  job.setUuid("12345");

  EXPECT_CALL(*mock_ptr_, get(_)).WillOnce(Return(""));

  auto client = createClient();

  EXPECT_EQ(client.getJobResult(job, false), nullptr);
}

TEST_F(MQSSClientTest, GetJobResultInvalidJson) {
  auto job = createCircuitJob();
  job.setUuid("12345");

  EXPECT_CALL(*mock_ptr_, get(_)).WillOnce(Return("invalid-json"));

  auto client = createClient();

  EXPECT_EQ(client.getJobResult(job, false), nullptr);
}

TEST_F(MQSSClientTest, WaitForJobResultWithZeroTimeoutDoesNotPoll) {
  auto job = createCircuitJob();
  EXPECT_CALL(*mock_ptr_, get(_)).Times(0);
  auto client = createClient();
  EXPECT_EQ(client.getJobResult(job, true, 0), nullptr);
}

class MQSSClientPollingTest : public MQSSClientTest,
                              public ::testing::WithParamInterface<size_t> {};

TEST_P(MQSSClientPollingTest, WaitForJobResultStopsAtTimeout) {
  const auto timeout = GetParam();
  auto job = createCircuitJob();
  job.setUuid("12345");
  EXPECT_CALL(*mock_ptr_, get(job.getPath() + "/12345/status"))
      .Times((timeout + 1) / 2)
      .WillRepeatedly(Return(kStatusRunning));
  EXPECT_CALL(*mock_ptr_, get(job.getPath() + "/12345/result")).Times(0);
  auto client = createClient();
  const auto start = std::chrono::steady_clock::now();
  EXPECT_EQ(client.getJobResult(job, true, timeout), nullptr);
  const auto elapsed = std::chrono::steady_clock::now() - start;
  EXPECT_GE(elapsed, std::chrono::seconds(timeout));
  EXPECT_LT(elapsed,
            std::chrono::seconds(timeout) + std::chrono::milliseconds(750));
}

INSTANTIATE_TEST_SUITE_P(Timeouts, MQSSClientPollingTest,
                         ::testing::Values(1u, 2u, 3u));

TEST_F(MQSSClientTest, WaitForJobResultStopsOnTerminalOrMissingStatus) {
  for (const std::string status :
       {kStatusFailed, R"({"status":"CANCELLED"})", ""}) {
    SCOPED_TRACE(status);
    auto mock = std::make_unique<MockMQSSBaseClient>();
    auto job = createCircuitJob();
    EXPECT_CALL(*mock, get(job.getPath() + "//status"))
        .WillOnce(Return(status));
    MQSSClient client(std::move(mock));
    EXPECT_EQ(client.getJobResult(job, true, 1), nullptr);
  }
}

TEST_F(MQSSClientTest, SubmitHamiltonianJobSuccess) {
  auto job = createHamiltonianJob();

  EXPECT_CALL(*mock_ptr_, post(job.getPath(), job.toJson(0)))
      .WillOnce(Return(kSubmitSuccess));

  auto client = createClient();

  auto uuid = client.submitJob(job);

  ASSERT_TRUE(uuid.has_value());
  EXPECT_EQ(*uuid, "12345");
  EXPECT_EQ(job.getUuid(), "12345");
}

TEST_F(MQSSClientTest, SubmitHamiltonianJobEmptyResponse) {
  auto job = createHamiltonianJob();

  EXPECT_CALL(*mock_ptr_, post(job.getPath(), job.toJson(0)))
      .WillOnce(Return(""));

  auto client = createClient();

  EXPECT_FALSE(client.submitJob(job).has_value());
}

TEST_F(MQSSClientTest, SubmitHamiltonianJobInvalidJson) {
  auto job = createHamiltonianJob();

  EXPECT_CALL(*mock_ptr_, post(job.getPath(), job.toJson(0)))
      .WillOnce(Return("not-json"));

  auto client = createClient();

  EXPECT_FALSE(client.submitJob(job).has_value());
}

TEST_F(MQSSClientTest, SubmitHamiltonianJobMissingUuid) {
  auto job = createHamiltonianJob();

  EXPECT_CALL(*mock_ptr_, post(job.getPath(), job.toJson(0)))
      .WillOnce(Return(R"({"status":"ok"})"));

  auto client = createClient();

  EXPECT_FALSE(client.submitJob(job).has_value());
}

TEST_F(MQSSClientTest, CancelHamiltonianJob) {
  auto job = createHamiltonianJob();
  job.setUuid("12345");

  EXPECT_CALL(*mock_ptr_, del(job.getPath() + "/12345")).Times(1);

  auto client = createClient();

  client.cancelJob(job);
}

TEST_F(MQSSClientTest, GetHamiltonianJobStatusCompleted) {
  auto job = createHamiltonianJob();
  job.setUuid("12345");

  EXPECT_CALL(*mock_ptr_, get(job.getPath() + "/12345/status"))
      .WillOnce(Return(kStatusCompleted));

  auto client = createClient();

  EXPECT_EQ(client.getJobStatus(job), "COMPLETED");
}

TEST_F(MQSSClientTest, GetHamiltonianJobStatusEmptyResponse) {
  auto job = createHamiltonianJob();
  job.setUuid("12345");

  EXPECT_CALL(*mock_ptr_, get(_)).WillOnce(Return(""));

  auto client = createClient();

  EXPECT_EQ(client.getJobStatus(job), "");
}

TEST_F(MQSSClientTest, GetHamiltonianJobStatusInvalidJson) {
  auto job = createHamiltonianJob();
  job.setUuid("12345");

  EXPECT_CALL(*mock_ptr_, get(_)).WillOnce(Return("bad-json"));

  auto client = createClient();

  EXPECT_EQ(client.getJobStatus(job), "");
}

TEST_F(MQSSClientTest, GetHamiltonianJobStatusMissingField) {
  auto job = createHamiltonianJob();
  job.setUuid("12345");

  EXPECT_CALL(*mock_ptr_, get(_)).WillOnce(Return(R"({})"));

  auto client = createClient();

  EXPECT_EQ(client.getJobStatus(job), "");
}

TEST_F(MQSSClientTest, GetNumberPendingJobsSuccess) {
  EXPECT_CALL(*mock_ptr_, get("resources/mock-resource/num_pending_jobs"))
      .WillOnce(Return(kPendingJobsResponse));

  auto client = createClient();

  EXPECT_EQ(client.getNumberPendingJobs("mock-resource"), 42);
}

TEST_F(MQSSClientTest, GetNumberPendingJobsEmptyResponse) {
  EXPECT_CALL(*mock_ptr_, get(_)).WillOnce(Return(""));

  auto client = createClient();

  EXPECT_EQ(client.getNumberPendingJobs("mock-resource"), -1);
}

TEST_F(MQSSClientTest, GetNumberPendingJobsInvalidJson) {
  EXPECT_CALL(*mock_ptr_, get(_)).WillOnce(Return("invalid-json"));

  auto client = createClient();

  EXPECT_EQ(client.getNumberPendingJobs("mock-resource"), -1);
}

TEST_F(MQSSClientTest, GetNumberPendingJobsMissingField) {
  EXPECT_CALL(*mock_ptr_, get(_)).WillOnce(Return(R"({})"));

  auto client = createClient();

  EXPECT_EQ(client.getNumberPendingJobs("mock-resource"), -1);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
