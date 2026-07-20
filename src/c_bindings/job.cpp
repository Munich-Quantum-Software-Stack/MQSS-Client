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

#include "mqss-c/job.h"

#include "common.h"
#include "mqss/job.h"

#include <sstream>

using namespace mqss::client;

MQSSJobRef MQSSClientCreateCircuitJob(char* circuit, char* circuitFormat,
                                      char* resourceName, unsigned int shots,
                                      bool noModify, bool queued) {
  if (!circuit || !circuitFormat || !resourceName)
    return nullptr;
  return wrap<MQSSJobRef>(new CircuitJobRequest(
      circuit, circuitFormat, resourceName, shots, noModify, queued));
}

MQSSJobRef MQSSClientCreateHamiltonianJob(char* resourceName,
                                          char* interactionStr,
                                          char* coefficientsStr) {
  if (!resourceName || !interactionStr || !coefficientsStr)
    return nullptr;
  return wrap<MQSSJobRef>(
      new HamiltonianJobRequest(resourceName, interactionStr, coefficientsStr));
}

int MQSSClientGetJobResultCounts(MQSSJobResultRef jobResult, char*** bitstreams,
                                 int** counts, int* size) {
  if (!jobResult || !bitstreams || !counts || !size)
    return -1;

  auto results = unwrap<JobResult>(jobResult)->getResults();

  // Count total entries across all result maps.
  size_t totalEntries = 0;
  for (const auto& result : results)
    totalEntries += result.size();

  *size = static_cast<int>(totalEntries);

  if (totalEntries == 0) {
    *bitstreams = nullptr;
    *counts = nullptr;
    return 0;
  }

  *counts = static_cast<int*>(malloc(sizeof(int) * totalEntries));
  *bitstreams = static_cast<char**>(malloc(sizeof(char*) * totalEntries));

  if (!*counts || !*bitstreams) {
    free(*counts);
    free(*bitstreams);
    return -1;
  }

  size_t index = 0;

  for (const auto& result : results) {
    for (const auto& [bitstream, count] : result) {

      (*bitstreams)[index] = static_cast<char*>(malloc(bitstream.size() + 1));

      if (!(*bitstreams)[index]) {
        for (size_t i = 0; i < index; ++i)
          free((*bitstreams)[i]);
        free(*bitstreams);
        free(*counts);
        return -1;
      }

      std::memcpy((*bitstreams)[index], bitstream.c_str(),
                  bitstream.size() + 1);

      (*counts)[index] = static_cast<int>(count);

      ++index;
    }
  }

  return 0;
}

uint64_t parseTimestamp(const std::string& s) {

  std::tm tm = {};
  std::istringstream ss(s.substr(0, 19)); // YYYY-MM-DD HH:MM:SS

  ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

  auto tt = std::mktime(&tm);

  auto tp = std::chrono::system_clock::from_time_t(tt);

  // Parse microseconds
  auto dot = s.find('.');
  if (dot != std::string::npos) {
    int micros = std::stoi(s.substr(dot + 1));
    tp += std::chrono::microseconds(micros);
  }
  return std::chrono::duration_cast<std::chrono::microseconds>(
             tp.time_since_epoch())
      .count();
}
int MQSSClientGetJobResultCompletedTimestamp(MQSSJobResultRef jobResult,
                                             uint64_t* completedTimestamp) {
  if (!jobResult || !completedTimestamp)
    return -1;

  try {
    *completedTimestamp =
        parseTimestamp(unwrap<JobResult>(jobResult)->getTimestampCompleted());
  } catch (...) {
    return -2;
  }
  return 0;
}

int MQSSClientGetJobResultSubmittedTimestamp(MQSSJobResultRef jobResult,
                                             uint64_t* submittedTimestamp) {
  if (!jobResult || !submittedTimestamp)
    return -1;

  try {
    *submittedTimestamp =
        parseTimestamp(unwrap<JobResult>(jobResult)->getTimestampSubmitted());
  } catch (...) {
    return -2;
  }
  return 0;
}

int MQSSClientGetJobResultScheduledTimestamp(MQSSJobResultRef jobResult,
                                             uint64_t* scheduledTimestamp) {
  if (!jobResult || !scheduledTimestamp)
    return -1;

  try {
    *scheduledTimestamp =
        parseTimestamp(unwrap<JobResult>(jobResult)->getTimestampScheduled());
  } catch (...) {
    return -2;
  }
  return 0;
}
