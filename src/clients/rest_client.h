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

#pragma once

#include "mqss/client.h"

#include <curl/curl.h>

using namespace mqss::client;

class MQSSRestClient : public MQSSBaseClient {
private:
  std::string mUrl;
  std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)> pHeaders{
      nullptr, curl_slist_free_all};

  std::string request(const char* method, const std::string& path,
                      const nlohmann::json* data = nullptr) const;

public:
  static std::string normalizeUrl(const std::string& url);

  MQSSRestClient(std::string token, std::string url = MQP_DEFAULT_URL);

  std::string get(const std::string& path) override;

  std::string post(const std::string& path,
                   const nlohmann::json& data) override;

  void del(const std::string& path) override;
};
