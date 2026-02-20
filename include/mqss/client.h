#pragma once

#include <map>
#include <memory>
#include <optional>
#include <regex>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace mqss::client {

class MQSSBaseClient {
public:
  virtual ~MQSSBaseClient() = default;
  virtual std::string get(const std::string &path) = 0;
  virtual std::string post(const std::string &path,
                           const nlohmann::json &data) = 0;
  virtual void cancel(const std::string &path) = 0;
};

class Device {
public:
  Device(std::string name, unsigned qubitCount, bool online,
         std::vector<std::pair<int, int>> couplingMap,
         std::vector<std::string> nativeGateset);

  const std::string &getName() const noexcept { return mName; }
  unsigned getQubitCount() const noexcept { return mQubitCount; }
  bool isOnline() const noexcept { return mOnline; }

  const std::vector<std::pair<int, int>> &getCouplingMap() const noexcept {
    return mCouplingMap;
  }

  const std::vector<std::string> &getNativeGateset() const noexcept {
    return mNativeGateset;
  }

  static Device fromJson(const nlohmann::json &response);

private:
  std::string mName;
  unsigned mQubitCount;
  bool mOnline;
  std::vector<std::pair<int, int>> mCouplingMap;
  std::vector<std::string> mNativeGateset;
};

class JobRequest {

private:
  std::string mUuid;

public:
  virtual ~JobRequest() = default;
  virtual nlohmann::json toJson() const = 0;
  virtual std::string getPath() const = 0;
  std::string getUuid() const { return mUuid; }
  void setUuid(const std::string& uuid) { mUuid = uuid; }
};

class CircuitJobRequest : public JobRequest {
private:
  std::string mCircuit;
  std::string mCircuitFormat;
  std::string mResourceName;
  unsigned int mShots;
  bool mNoModify;
  bool mQueued;

public:
  CircuitJobRequest(){};

  CircuitJobRequest(std::string circuit, std::string circuitFormat,
                    std::string resourceName, unsigned int shots, bool noModify,
                    bool queued);

  void setCircuit(std::string circuit) { mCircuit = circuit; }
  std::string getCircuit() const { return mCircuit; }
  void setCircuitFormat(std::string circuitFormat) {
    mCircuitFormat = circuitFormat;
  }
  std::string getCircuitFormat() const { return mCircuitFormat; }
  void setResourceName(const std::string &resourceName) {
    mResourceName = resourceName;
  }
  std::string getResourceName() const { return mResourceName; }
  void setShots(unsigned int shots) { mShots = shots; }
  unsigned int getShots() const { return mShots; }
  void setNoModify(bool noModify) { mNoModify = noModify; }
  bool isNoModify() const { return mNoModify; }
  void setQueued(bool queued) { mQueued = queued; }
  bool isQueued() const { return mQueued; }

  nlohmann::json toJson() const;

  std::string getPath() const { return "job"; }
};

class HamiltonianJobRequest : public JobRequest {
private:
  std::string mResourceName;
  std::string mInteractionStr;
  std::string mCoefficientsStr;

public:
  HamiltonianJobRequest(std::string resourceName, std::string interactionStr,
                        std::string coefficientsStr);

  HamiltonianJobRequest(){};

  void setResourceName(std::string resourceName) {
    mResourceName = resourceName;
  }
  std::string getResourceName() const { return mResourceName; }

  void setInteractionString(std::string interactionStr) {
    mInteractionStr = interactionStr;
  }
  std::string getInteractionString() const { return mInteractionStr; }

  void setCoefficientsString(std::string coefficientsStr) {
    mCoefficientsStr = coefficientsStr;
  }
  std::string getCoefficientsString() const { return mCoefficientsStr; }

  nlohmann::json toJson() const;

  std::string getPath() const { return "hamiltonian_job"; }
};

class JobResult {

  std::map<std::string, unsigned int> mResults;
  std::string mTimestampCompleted;
  std::string mTimestampSubmitted;
  std::string mTimestampScheduled;

public:
  JobResult(std::map<std::string, unsigned int> results,
            std::string timestampCompleted, std::string timestampSubmitted,
            std::string timestampScheduled);

  std::map<std::string, unsigned int> getResults() const { return mResults; }
  std::string getTimestampCompleted() const { return mTimestampCompleted; }
  std::string getTimestampSubmitted() const { return mTimestampSubmitted; }
  std::string getTimestampScheduled() const { return mTimestampScheduled; }

  static JobResult fromJson(const nlohmann::json &parsed);
};

class MQSSClient {
public:
  MQSSClient();
  MQSSClient(const std::string &token_or_queue, bool is_hpc);
  MQSSClient(const std::string &token, const std::string &url);

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
