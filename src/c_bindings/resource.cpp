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

#include "mqss/resource.h"

#include "common.h"
#include "mqss-c/resource.h"

using namespace mqss::client;

template <typename T>
int* flatten2D(const std::vector<std::vector<T>>& input, unsigned int* count) {
  if (!count)
    return nullptr;
  size_t total = 0;
  for (const auto& row : input)
    total += row.size();

  int* result = static_cast<int*>(malloc(total * sizeof(int)));
  if (!result)
    return nullptr;

  size_t index = 0;
  for (const auto& row : input) {
    for (T value : row) {
      result[index++] = static_cast<int>(value);
    }
  }
  *count = total;
  return result;
}

int MQSSClientResourceGetInfo(MQSSResourceRef resource, char** name,
                              unsigned* qubitCount, bool* online,
                              int** couplingMap, unsigned int* couplingMapSize,
                              MQSSGateRef** nativeGateset,
                              unsigned* gateCount) {
  if (!name || !qubitCount || !online || !couplingMap || !couplingMapSize ||
      !nativeGateset || !gateCount) {
    return -1;
  }

  auto* r = unwrap<Resource>(resource);
  if (!r)
    return -2;

  if (asprintf(name, "%s", r->getName().c_str()) < 0)
    return -3;

  *qubitCount = r->getQubitCount();
  *online = r->isOnline();

  *couplingMap = flatten2D(r->getCouplingMap(), couplingMapSize);
  if (!*couplingMap) {
    free(*name);
    *name = nullptr;
    return -4;
  }

  const auto& gates = r->getNativeGateset();
  *gateCount = static_cast<unsigned>(gates.size());

  *nativeGateset =
      static_cast<MQSSGateRef*>(malloc(gates.size() * sizeof(MQSSGateRef)));

  if (!*nativeGateset) {
    free(*couplingMap);
    *couplingMap = nullptr;
    free(*name);
    *name = nullptr;
    return -5;
  }

  for (size_t i = 0; i < gates.size(); ++i)
    (*nativeGateset)[i] = wrap<MQSSGateRef>(new Gate(gates[i]));

  return 0;
}

int MQSSClientResourceGetGateInfo(MQSSGateRef gate, char** name,
                                  unsigned* qubitNumber,
                                  unsigned* parameterNumber,
                                  int** supportedQubits,
                                  unsigned int* supportedQubitCount) {
  if (!gate || !name || !qubitNumber || !parameterNumber || !supportedQubits ||
      !supportedQubitCount) {
    return -1;
  }

  auto* g = unwrap<Gate>(gate);
  if (!g)
    return -1;
  const auto& gateName = g->getName();
  *name = static_cast<char*>(malloc(gateName.size() + 1));
  if (!*name)
    return -2;
  std::memcpy(*name, gateName.c_str(), gateName.size() + 1);

  *qubitNumber = g->getQubitNumber();
  *parameterNumber = g->getParameterNumber();

  *supportedQubits = flatten2D(g->getSupportedQubits(), supportedQubitCount);
  if (!*supportedQubits) {
    free(*name);
    *name = nullptr;
    return -2;
  }

  return 0;
}
