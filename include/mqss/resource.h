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
class Resource {
public:
  Resource(std::string name, unsigned qubitCount, bool online,
           std::vector<std::pair<int, int>> couplingMap,
           std::vector<std::string> nativeGateset);

  Resource(const nlohmann::json& json);

  const std::string& getName() const noexcept { return mName; }
  unsigned getQubitCount() const noexcept { return mQubitCount; }
  bool isOnline() const noexcept { return mOnline; }

  const std::vector<std::pair<int, int>>& getCouplingMap() const noexcept {
    return mCouplingMap;
  }

  const std::vector<std::string>& getNativeGateset() const noexcept {
    return mNativeGateset;
  }

private:
  std::string mName;
  unsigned mQubitCount;
  bool mOnline;
  std::vector<std::pair<int, int>> mCouplingMap;
  std::vector<std::string> mNativeGateset;
};
} // namespace mqss::client
