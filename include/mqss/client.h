#pragma once

#include <map>
#include <memory>
#include <optional>
#include <regex>
#include <string>
#include <vector>

#include "job.h"
#include "resource.h"
#include <nlohmann/json.hpp>

#define MQP_DEFAULT_URL "https://portal.quantum.lrz.de:4000/v1/"

namespace mqss::client {

class MQSSBaseClient {
public:
  virtual ~MQSSBaseClient() = default;
  virtual std::string get(const std::string &path) = 0;
  virtual std::string post(const std::string &path,
                           const nlohmann::json &data) = 0;
  virtual void del(const std::string &path) = 0;
};

class MQSSClient {

private:
  std::unique_ptr<JobResult> waitForJobResult(const JobRequest &job, size_t poll_seconds);

public:
  MQSSClient(const std::string &token = "",
             const std::string &url_or_queue = MQP_DEFAULT_URL,
             bool is_hpc = false);

  // Resources
  std::vector<Resource> getAllResources() const;
  std::optional<Resource> getResourceInfo(const std::string &resource) const;

  // Jobs
  std::optional<std::string> submitJob(JobRequest &job);
  void cancelJob(JobRequest &job);
  std::string getJobStatus(const JobRequest &job);
  std::unique_ptr<JobResult>
  getJobResult(const JobRequest &job, bool wait = false, size_t timeout = 100);

  int getNumberPendingJobs(const std::string &resource) const;

private:
  std::unique_ptr<MQSSBaseClient> mClient;
};

} // namespace mqss::client
