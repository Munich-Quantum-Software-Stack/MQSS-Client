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

#include "mqss-c/resource.h"
#include "mqss/client.h"

#include <regex>

using namespace mqss::client;

namespace {

std::vector<std::vector<int>> extractCouplingMap(const nlohmann::json& response,
                                                 const std::string& key) {
  std::vector<std::vector<int>> result;

  const auto it = response.find(key);
  if (it == response.end() || !it->is_string())
    return result;

  const std::string& text = it->get_ref<const std::string&>();
  if (text == "None")
    return result;
  std::regex re(R"(\[(\d+),\s*(\d+)\])");

  auto begin = std::sregex_iterator(text.begin(), text.end(), re);
  auto end = std::sregex_iterator();

  for (auto it = begin; it != end; ++it) {
    int a = std::stoi((*it)[1].str());
    int b = std::stoi((*it)[2].str());
    std::vector<int> pairs = {a, b};
    result.push_back(pairs);
  }

  return result;
}

std::pair<unsigned int, unsigned int>
getGateProterties(const std::string& gateName) {

  if (gateName == "cz") {
    return {2, 0};
  }
  if (gateName == "measure") {
    return {1, 0};
  }
  if (gateName == "id") {
    return {1, 0};
  }
  if (gateName == "r") {
    return {1, 2};
  }
  if (gateName == "rx") {
    return {1, 1};
  }
  if (gateName == "rz") {
    return {1, 1};
  }
  if (gateName == "rxx") {
    return {2, 1};
  }
  if (gateName == "if_else") {
    return {1, 0};
  }
  if (gateName == "reset") {
    return {1, 0};
  }
  if (gateName == "swap") {
    return {2, 0};
  }

  throw std::invalid_argument("Unknown Gate: " + gateName);
}

std::vector<Gate> extractGates(const nlohmann::json& response,
                               const std::string& key) {

  std::vector<Gate> result;

  const auto it = response.find(key);
  if (it == response.end() || !it->is_string())
    return result;

  const std::string& text = it->get_ref<const std::string&>();
  if (text == "None")
    return result;
  std::regex gateRegex(R"(\(\s*['"]([^'"]+)['"]\s*,\s*(\{[^{}]*\})\s*\))");
  auto gateBegin = std::sregex_iterator(text.begin(), text.end(), gateRegex);
  auto gateEnd = std::sregex_iterator();
  std::string gateName;
  unsigned int qubitCount;
  std::vector<std::vector<unsigned int>> supportedQubits;

  for (auto it = gateBegin; it != gateEnd; ++it) {

    gateName = (*it)[1];

    std::string body = (*it)[2];
    std::regex tupleRegex(R"(([^)]*)\)\s*:\s*None)");

    auto tupleBegin =
        std::sregex_iterator(body.begin(), body.end(), tupleRegex);

    auto tupleEnd = std::sregex_iterator();

    for (auto jt = tupleBegin; jt != tupleEnd; ++jt) {
      std::string tupleText = (*jt)[1];

      std::vector<unsigned> qubits;

      std::regex numberRegex(R"(\d+)");

      auto numBegin =
          std::sregex_iterator(tupleText.begin(), tupleText.end(), numberRegex);

      for (auto kt = numBegin; kt != std::sregex_iterator(); ++kt) {
        qubits.push_back(static_cast<unsigned>(std::stoi((*kt).str())));
      }

      supportedQubits.push_back(qubits);
    }

    auto [qubitCount, parameterNumber] = getGateProterties(gateName);
    result.emplace_back(gateName, qubitCount, parameterNumber,
                        std::move(supportedQubits));
  }

  return result;
}

} // namespace

Resource::Resource(std::string name, unsigned qubitCount, bool online,
                   std::vector<std::vector<int>> couplingMap,
                   std::vector<Gate> nativeGateset)
    : mName(std::move(name)), mQubitCount(qubitCount), mOnline(online),
      mCouplingMap(std::move(couplingMap)),
      mNativeGateset(std::move(nativeGateset)) {}

Resource::Resource(const nlohmann::json& json) {

  std::string name = json.value("name", "");
  unsigned qubitCount = json.value("qubits", 0);
  bool online = json.value("online", false);

  std::vector<std::vector<int>> couplingMap =
      extractCouplingMap(json, "connectivity");

  std::vector<Gate> nativeGateset = extractGates(json, "instructions");

  mName = std::move(name);
  mQubitCount = qubitCount;
  mOnline = online;
  mCouplingMap = std::move(couplingMap);
  mNativeGateset = std::move(nativeGateset);
}
template <typename T>
int* flatten2D(const std::vector<std::vector<T>>& input, unsigned int* count) {
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
  auto* r = unwrap<Resource>(resource);
  if (!r)
    return -2;

  if (asprintf(name, "%s", r->getName().c_str()) < 0)
    return -3;

  *qubitCount = r->getQubitCount();
  *online = r->isOnline();

  *couplingMap = flatten2D(r->getCouplingMap(), couplingMapSize);
  if (!*couplingMap)
    return -4;

  const auto& gates = r->getNativeGateset();
  *gateCount = static_cast<unsigned>(gates.size());

  *nativeGateset =
      static_cast<MQSSGateRef*>(malloc(gates.size() * sizeof(MQSSGateRef)));

  if (!*nativeGateset)
    return -5;

  for (size_t i = 0; i < gates.size(); ++i)
    (*nativeGateset)[i] = wrap<MQSSGateRef>(new Gate(gates[i]));

  return 0;
}

int MQSSClientResourceGetGateInfo(MQSSGateRef gate, char** name,
                                  unsigned* qubitNumber,
                                  unsigned* parameterNumber,
                                  int** supportedQubits,
                                  unsigned int* supportedQubitCount) {
  auto* g = unwrap<Gate>(gate);
  if (!g)
    return -1;
  auto gate_name = g->getName();
  *name = (char*)malloc(gate_name.size() * sizeof(char));
  strcpy(*name, gate_name.data());

  *qubitNumber = g->getQubitNumber();
  *parameterNumber = g->getParameterNumber();

  *supportedQubits = flatten2D(g->getSupportedQubits(), supportedQubitCount);
  if (!*supportedQubits)
    return -2;

  return 0;
}
