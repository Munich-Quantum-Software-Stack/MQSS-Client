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

#include "mqss-c/client.h"

#include "common.h"
#include "mqss/client.h"

using namespace mqss::client;

MQSSClientRef MQSSClientCreateClient(char* token, char* urlOrQueue,
                                     bool isHpc) {
  if (!token || !urlOrQueue)
    return nullptr;

  return wrap<MQSSClientRef>(new MQSSClient(token, urlOrQueue, isHpc));
}
MQSSResourceRef* MQSSClientGetAllResources(MQSSClientRef client, int* size) {
  if (!client || !size)
    return nullptr;

  auto resources_ = unwrap<MQSSClient>(client)->getAllResources();
  *size = static_cast<int>(resources_.size());

  auto resources = static_cast<MQSSResourceRef*>(
      malloc(resources_.size() * sizeof(MQSSResourceRef)));
  if (!resources && !resources_.empty())
    return nullptr;

  for (size_t i = 0; i < resources_.size(); ++i) {
    resources[i] = wrap<MQSSResourceRef>(new Resource(resources_[i]));
  }

  return resources;
}

MQSSResourceRef MQSSClientGetResourceInfo(MQSSClientRef client,
                                          const char* resourceName) {
  if (!client || !resourceName)
    return nullptr;

  auto resource = unwrap<MQSSClient>(client)->getResourceInfo(resourceName);
  if (!resource)
    return nullptr;

  return wrap<MQSSResourceRef>(new Resource(*resource));
}
int MQSSClientSubmitJob(MQSSClientRef client, MQSSJobRef job) {
  if (!client || !job)
    return -1;

  auto uuid =
      unwrap<MQSSClient>(client)->submitJob(*unwrap<CircuitJobRequest>(job));
  if (!uuid.has_value())
    return -1;

  try {
    return std::stoi(*uuid);
  } catch (...) {
    return -2;
  }
}

void MQSSClientCancelJob(MQSSClientRef client, MQSSJobRef job) {
  if (!client || !job)
    return;
  unwrap<MQSSClient>(client)->cancelJob(*unwrap<CircuitJobRequest>(job));
}

MQSSJobResultRef MQSSClientGetJobResult(MQSSClientRef client, MQSSJobRef job,
                                        bool wait, unsigned int timeout) {
  if (!client || !job)
    return nullptr;

  std::unique_ptr<JobResult> jobResult =
      unwrap<MQSSClient>(client)->getJobResult(*unwrap<CircuitJobRequest>(job),
                                               wait, timeout);
  return wrap<MQSSJobResultRef>(jobResult.release());
}

int MQSSClientGetNumberPendingJobs(MQSSClientRef client, char* resourceName) {
  if (!client || !resourceName)
    return -1;

  return unwrap<MQSSClient>(client)->getNumberPendingJobs(resourceName);
}
