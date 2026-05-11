#include "mqss/job.h"

using namespace mqss::client;

nlohmann::json CircuitJobRequest::toJson() const {

  return {{"circuit", {mCircuit}},
          {"circuit_format", mCircuitFormat},
          {"resource_name", mResourceName},
          {"shots", mShots},
          {"no_modify", mNoModify},
          {"queued", mQueued}};
}

CircuitJobRequest::CircuitJobRequest(std::string circuit,
                                     std::string circuitFormat,
                                     std::string resourceName,
                                     unsigned int shots, bool noModify,
                                     bool queued)
    : mCircuit(std::move(circuit)), mCircuitFormat(std::move(circuitFormat)),
      mResourceName(std::move(resourceName)), mShots(shots),
      mNoModify(noModify), mQueued(queued) {}

HamiltonianJobRequest::HamiltonianJobRequest(std::string resourceName,
                                             std::string interactionStr,
                                             std::string coefficientsStr)
    : mResourceName(std::move(resourceName)),
      mInteractionStr(std::move(interactionStr)),
      mCoefficientsStr(std::move(coefficientsStr)) {}

nlohmann::json HamiltonianJobRequest::toJson() const {

  return {{"resource_name", mResourceName},
          {"interaction_str", mInteractionStr},
          {"coefficients_str", mCoefficientsStr}};
}

JobResult::JobResult(std::map<std::string, unsigned int> results,
                     std::string timestampCompleted,
                     std::string timestampSubmitted,
                     std::string timestampScheduled)
    : mResults(std::move(results)),
      mTimestampCompleted(std::move(timestampCompleted)),
      mTimestampSubmitted(std::move(timestampSubmitted)),
      mTimestampScheduled(std::move(timestampScheduled)) {}

JobResult::JobResult(const nlohmann::json &parsed) {

  const auto &rResultStr = parsed.at("result").get_ref<const std::string &>();

  if(!nlohmann::json::accept(rResultStr))
    return;

  nlohmann::json resultMap = nlohmann::json::parse(rResultStr);
  if(resultMap.is_array())
    resultMap = resultMap[0];

  for (auto &[key, value] : resultMap.items()) {
    resultMap[key] = value.get<unsigned int>();
  }

  mResults = std::move(resultMap);
  mTimestampCompleted = parsed.at("timestamp_completed").get<std::string>();
  mTimestampSubmitted = parsed.at("timestamp_submitted").get<std::string>();
  mTimestampScheduled = parsed.at("timestamp_scheduled").get<std::string>();
}
