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

std::vector<Gate> extractGates(const nlohmann::json& response,
                               const std::string& key) {

  std::vector<Gate> result;

  const auto it = response.find(key);
  if (it == response.end() || !it->is_string())
    return result;

  const std::string& text = it->get_ref<const std::string&>();
  if (text == "None")
    return result;
  std::regex gateRegex(R"(\(\s*'([^']+)'\s*,\s*\{([^}]*)\}\s*\))");

  auto gateBegin = std::sregex_iterator(text.begin(), text.end(), gateRegex);
  auto gateEnd = std::sregex_iterator();
  std::string GateName;
  unsigned int arity;
  std::vector<std::vector<unsigned int>> supportedQubits;

  for (auto it = gateBegin; it != gateEnd; ++it) {

    GateName = (*it)[1];

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

    arity = !supportedQubits.empty() ? supportedQubits.at(0).size() : 0;

    result.push_back(Gate(GateName, arity, std::move(supportedQubits)));
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

  std::vector<Gate> nativeGateset = extractGates(json, "Gates");

  mName = std::move(name);
  mQubitCount = qubitCount;
  mOnline = online;
  mCouplingMap = std::move(couplingMap);
  mNativeGateset = std::move(nativeGateset);
}
