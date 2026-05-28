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

#include "mqss/job.h"

using namespace mqss::client;

nlohmann::json CircuitJobRequest::toJson() const {

  return {{"circuit", mCircuit},
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

JobResult::JobResult(std::vector<std::map<std::string, unsigned int>> results,
                     std::string timestampCompleted,
                     std::string timestampSubmitted,
                     std::string timestampScheduled)
    : mResults(std::move(results)),
      mTimestampCompleted(std::move(timestampCompleted)),
      mTimestampSubmitted(std::move(timestampSubmitted)),
      mTimestampScheduled(std::move(timestampScheduled)) {}

JobResult::JobResult(const nlohmann::json& parsed) {

  std::vector<std::map<std::string, unsigned int>> results;
  const auto& rResultStr = parsed.at("result").get_ref<const std::string&>();

  if (!nlohmann::json::accept(rResultStr))
    return;

  nlohmann::json resultMaps = nlohmann::json::parse(rResultStr);
  if (resultMaps.is_array()) {
    for (auto resultMap : resultMaps) {
      std::map<std::string, unsigned int> result;
      for (auto& [key, value] : resultMap.items()) {
        result[key] = value.get<unsigned int>();
      }
      results.push_back(result);
    }
  } else {
    std::map<std::string, unsigned int> result;
    for (auto& [key, value] : resultMaps.items()) {
      result[key] = value.get<unsigned int>();
    }
    results.push_back(result);
  }

  mResults = std::move(results);
  mTimestampCompleted = parsed.at("timestamp_completed").get<std::string>();
  mTimestampSubmitted = parsed.at("timestamp_submitted").get<std::string>();
  mTimestampScheduled = parsed.at("timestamp_scheduled").get<std::string>();
}
