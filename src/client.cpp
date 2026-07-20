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

#include "clients/hpc_client.h"
#include "clients/rest_client.h"
#include "mqss-c/client.h"

#include <chrono>
#include <optional>
#include <thread>

using namespace mqss::client;
// GCOVR_EXCL_START
MQSSClient::MQSSClient(const std::string& token,
                       const std::string& url_or_queue, bool is_hpc) {
  if (is_hpc) {
    mClient = std::make_unique<MQSSHPCClient>(token, url_or_queue);
  } else {
    mClient = std::make_unique<MQSSRestClient>(token, url_or_queue);
  }
}
// GCOVR_EXCL_STOP

std::vector<Resource> MQSSClient::getAllResources() const {
  std::vector<Resource> Resources;
  std::string resp = mClient->get("resources");
  if (!nlohmann::json::accept(resp))
    return {};
  nlohmann::json parsed = nlohmann::json::parse(resp);
  for (auto& item : parsed) {
    Resources.push_back(Resource(item));
  }
  return Resources;
}

std::optional<Resource>
MQSSClient::getResourceInfo(const std::string& resource) const {
  std::string resp = mClient->get("resources/" + resource);
  if (resp.find("RESOURCE NOT FOUND") != std::string::npos)
    return std::nullopt;

  if (!nlohmann::json::accept(resp))
    return std::nullopt;

  nlohmann::json parsed = nlohmann::json::parse(resp);
  if (parsed.contains("ERROR"))
    return std::nullopt;
  return Resource(parsed);
}
std::optional<std::string> MQSSClient::submitJob(JobRequest& job) {
  std::string path = job.getPath();
  std::string result = mClient->post(path, job.toJson());
  if (result.empty() || !nlohmann::json::accept(result))
    return std::nullopt;

  nlohmann::json parsed = nlohmann::json::parse(result);
  if (!parsed.contains("uuid"))
    return std::nullopt;

  std::string uuid = parsed["uuid"].get<std::string>();
  job.setUuid(uuid);
  return uuid;
}

void MQSSClient::cancelJob(JobRequest& job) {
  std::string path = job.getPath() + "/" + job.getUuid();
  mClient->del(path);
}

std::string MQSSClient::getJobStatus(const JobRequest& job) {
  std::string path = job.getPath() + "/" + job.getUuid() + "/status";
  std::string resp = mClient->get(path);
  if (resp.empty())
    return "";

  if (!nlohmann::json::accept(resp))
    return "";

  nlohmann::json parsed = nlohmann::json::parse(resp);

  if (parsed.contains("status") && parsed["status"].is_string()) {
    return parsed["status"].get<std::string>();
  }

  return "";
}

std::unique_ptr<JobResult> MQSSClient::getJobResult(const JobRequest& job,
                                                    bool wait, size_t timeout) {

  if (wait) {
    return waitForJobResult(job, timeout);
  }

  std::string path = job.getPath() + "/" + job.getUuid() + "/result";
  std::string resp = mClient->get(path);
  if (resp.empty() || !nlohmann::json::accept(resp))
    return nullptr;

  nlohmann::json parsed = nlohmann::json::parse(resp);
  return std::make_unique<JobResult>(JobResult(parsed));
}

std::unique_ptr<JobResult> MQSSClient::waitForJobResult(const JobRequest& job,
                                                        size_t timeout) {
  size_t poll_seconds = 2;

  while (timeout > 0) {
    std::string status = getJobStatus(job);
    if (status == "COMPLETED" || status == "CANCELLED")
      break;
    if (status == "FAILED" || status.empty())
      return nullptr;
    std::this_thread::sleep_for(std::chrono::seconds(poll_seconds));
    timeout -= poll_seconds;
  }
  return timeout <= 0 ? nullptr : getJobResult(job);
}

int MQSSClient::getNumberPendingJobs(const std::string& resource) const {
  std::string resp =
      mClient->get("resources/" + resource + "/num_pending_jobs");
  if (resp.empty() || !nlohmann::json::accept(resp))
    return -1;

  nlohmann::json parsed = nlohmann::json::parse(resp);

  return parsed.value("num_pending_jobs", -1);
}
