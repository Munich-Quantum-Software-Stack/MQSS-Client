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

#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <vector>
namespace mqss::client {

class Gate {

public:
  Gate(std::string name, unsigned int arity,
       std::vector<std::vector<unsigned int>> supportedQubits)
      : mName(std::move(name)), mArity(arity),
        mSupportedQubits(std::move(supportedQubits)) {}

  const std::string& getName() const noexcept { return mName; }

  const unsigned int& getArity() const noexcept { return mArity; }

  const std::vector<std::vector<unsigned int>>&
  getSupportedQubits() const noexcept {
    return mSupportedQubits;
  }

private:
  std::string mName;
  unsigned int mArity;
  std::vector<std::vector<unsigned int>> mSupportedQubits;
};

class Resource {
public:
  Resource(std::string name, unsigned qubitCount, bool online,
           std::vector<std::vector<int>> couplingMap,
           std::vector<Gate> nativeGateset);

  Resource(const nlohmann::json& json);

  const std::string& getName() const noexcept { return mName; }
  unsigned getQubitCount() const noexcept { return mQubitCount; }
  bool isOnline() const noexcept { return mOnline; }

  const std::vector<std::vector<int>>& getCouplingMap() const noexcept {
    return mCouplingMap;
  }

  const std::vector<Gate>& getNativeGateset() const noexcept {
    return mNativeGateset;
  }

private:
  std::string mName;
  unsigned mQubitCount;
  bool mOnline;
  std::vector<std::vector<int>> mCouplingMap;
  std::vector<Gate> mNativeGateset;
};

} // namespace mqss::client
