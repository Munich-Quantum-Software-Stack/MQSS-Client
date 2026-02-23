#include <string>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

namespace mqss::client {
class JobRequest {

private:
  std::string mUuid;

public:
  virtual ~JobRequest() = default;
  virtual nlohmann::json toJson() const = 0;
  virtual std::string getPath() const = 0;
  std::string getUuid() const { return mUuid; }
  void setUuid(const std::string &uuid) { mUuid = uuid; }
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

  JobResult(const nlohmann::json &parsed);

  std::map<std::string, unsigned int> getResults() const { return mResults; }
  std::string getTimestampCompleted() const { return mTimestampCompleted; }
  std::string getTimestampSubmitted() const { return mTimestampSubmitted; }
  std::string getTimestampScheduled() const { return mTimestampScheduled; }
};
} // namespace mqss::client