#pragma once

#include <map>
#include <memory>
#include <optional>
#include <regex>
#include <string>
#include <vector>

#include "device.h"
#include "job.h"
#include <nlohmann/json.hpp>

#define MQP_DEFAULT_URL "https://portal.quantum.lrz.de:4000/v1/"

namespace mqss::client {

class MQSSBaseClient {
public:
  virtual ~MQSSBaseClient() = default;
  virtual std::string get(const std::string &path) = 0;
  virtual std::string post(const std::string &path,
                           const nlohmann::json &data) = 0;
  virtual void cancel(const std::string &path) = 0;
};

class MQSSClient {
public:
  MQSSClient(const std::string &token = "",
             const std::string &url_or_queue = MQP_DEFAULT_URL,
             bool is_hpc = false);

  // Devices
  std::vector<Device> getAllResources() const;
  std::optional<Device> getResourceInfo(const std::string &resource) const;

  // Jobs
  std::optional<std::string> submitJob(JobRequest &job);
  void cancelJob(JobRequest &job);
  std::string getJobStatus(const JobRequest &job);
  std::unique_ptr<JobResult> getJobResult(const JobRequest &job);
  std::unique_ptr<JobResult> waitForJobResult(const JobRequest &job,
                                              size_t poll_seconds = 2);

  int getNumberPendingJobs(const std::string &resource) const;

private:
  std::unique_ptr<MQSSBaseClient> mClient;
};

} // namespace mqss::client
