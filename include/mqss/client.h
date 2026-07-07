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

#include "job.h"
#include "resource.h"

#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <regex>
#include <string>
#include <vector>

#define MQP_DEFAULT_URL "https://portal.quantum.lrz.de:4000/v1/"

namespace mqss::client {

class MQSSBaseClient {
public:
  virtual ~MQSSBaseClient() = default;
  virtual std::string get(const std::string& path) = 0;
  virtual std::string post(const std::string& path,
                           const nlohmann::json& data) = 0;
  virtual void del(const std::string& path) = 0;
};

class MQSSClient {

private:
  std::unique_ptr<JobResult> waitForJobResult(const JobRequest& job,
                                              size_t poll_seconds);

  std::unique_ptr<MQSSBaseClient> mClient;

public:
  MQSSClient(const std::string& token = "",
             const std::string& url_or_queue = MQP_DEFAULT_URL,
             bool is_hpc = false);

  explicit MQSSClient(std::unique_ptr<MQSSBaseClient> client)
      : mClient(std::move(client)) {}

  // Resources
  std::vector<Resource> getAllResources() const;
  std::optional<Resource> getResourceInfo(const std::string& resource) const;

  // Jobs
  std::optional<std::string> submitJob(JobRequest& job);
  void cancelJob(JobRequest& job);
  std::string getJobStatus(const JobRequest& job);
  std::unique_ptr<JobResult>
  getJobResult(const JobRequest& job, bool wait = false, size_t timeout = 100);
  int getNumberPendingJobs(const std::string& resource) const;
};

} // namespace mqss::client
