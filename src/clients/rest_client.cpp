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

#include "rest_client.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>

std::string MQSSRestClient::normalizeUrl(const std::string& url) {
  const auto invalid = [] {
    return std::invalid_argument(
        "REST URL must be an absolute HTTP(S) URL without credentials, "
        "a query, or a fragment");
  };
  const auto separator = url.find("://");
  if (url.empty() ||
      std::any_of(url.begin(), url.end(),
                  [](unsigned char c) { return c <= 0x20 || c == 0x7f; }) ||
      separator == std::string::npos || separator + 3 == url.size() ||
      url[separator + 3] == '/' ||
      url.find_first_of("?#") != std::string::npos) {
    throw invalid();
  }

  std::unique_ptr<CURLU, decltype(&curl_url_cleanup)> parsed(curl_url(),
                                                             curl_url_cleanup);
  if (!parsed)
    throw std::runtime_error("Failed to initialize URL parser");
  if (curl_url_set(parsed.get(), CURLUPART_URL, url.c_str(),
                   CURLU_DISALLOW_USER) != CURLUE_OK)
    throw invalid();

  char* scheme = nullptr;
  const auto result = curl_url_get(parsed.get(), CURLUPART_SCHEME, &scheme, 0);
  std::unique_ptr<char, decltype(&curl_free)> schemeGuard(scheme, curl_free);
  if (result != CURLUE_OK ||
      (std::string(scheme) != "http" && std::string(scheme) != "https"))
    throw invalid();

  const auto pathStart = url.find('/', separator + 3);
  auto origin = url.substr(0, pathStart);
  std::transform(origin.begin(), origin.end(), origin.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  const auto path = pathStart == std::string::npos ? "" : url.substr(pathStart);
  if (std::string(scheme) == "https" &&
      (path.empty() || path == "/" || path == "/v1" || path == "/v1/")) {
    auto host = origin.substr(separator + 3);
    const std::string port = ":4000";
    if (host.size() > port.size() &&
        host.compare(host.size() - port.size(), port.size(), port) == 0)
      host.erase(host.size() - port.size());

    for (const std::string domain : {".quantum.lrz.de", ".quantumpathway.eu"}) {
      if (host.size() > domain.size() &&
          host.compare(host.size() - domain.size(), domain.size(), domain) == 0)
        return "https://" + host + port + "/v1/";
    }
  }

  return url.back() == '/' ? url : url + '/';
}

namespace {

size_t writeResponse(char* data, size_t size, size_t nmemb, void* output) {
  const auto bytes = size * nmemb;
  try {
    static_cast<std::string*>(output)->append(data, bytes);
  } catch (...) {
    // Exceptions must not cross libcurl's C callback boundary.
    return 0;
  }
  return bytes;
}

} // namespace

MQSSRestClient::MQSSRestClient(std::string token, std::string url)
    : mUrl(normalizeUrl(url)) {
  const auto appendHeader = [this](const char* header) {
    auto* headers = curl_slist_append(pHeaders.get(), header);
    if (!headers)
      throw std::bad_alloc();
    pHeaders.release();
    pHeaders.reset(headers);
  };
  std::string auth_header = "Authorization: Bearer " + token;
  appendHeader(auth_header.c_str());
  appendHeader("Content-Type: application/json");
  request("GET", "resources");
}

std::string MQSSRestClient::request(const char* method, const std::string& path,
                                    const nlohmann::json* data) const {
  std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(),
                                                           curl_easy_cleanup);
  if (!curl)
    throw std::runtime_error("Failed to initialize REST connection");

  const std::string url = mUrl + path;
  const std::string payload = data ? data->dump() : std::string{};
  std::string response;
  curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, pHeaders.get());
  curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, writeResponse);
  curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(curl.get(), CURLOPT_CONNECTTIMEOUT_MS, 3000L);
  curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT_MS, 5000L);
  curl_easy_setopt(curl.get(), CURLOPT_NOSIGNAL, 1L);
  curl_easy_setopt(curl.get(), CURLOPT_CAINFO, std::getenv("CURL_CA_BUNDLE"));
  curl_easy_setopt(curl.get(), CURLOPT_CUSTOMREQUEST, method);
  if (data) {
    curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDSIZE_LARGE,
                     static_cast<curl_off_t>(payload.size()));
  }

  const std::string errorPrefix = std::string("REST ") + method + " failed: ";
  const CURLcode result = curl_easy_perform(curl.get());
  if (result != CURLE_OK)
    throw std::runtime_error(errorPrefix + curl_easy_strerror(result));

  long status = 0;
  curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &status);
  if (status < 200 || status >= 300)
    throw std::runtime_error(errorPrefix + "HTTP " + std::to_string(status));
  return response;
}

std::string MQSSRestClient::get(const std::string& path) {
  try {
    return request("GET", path);
  } catch (const std::runtime_error&) {
    return "";
  }
}

std::string MQSSRestClient::post(const std::string& path,
                                 const nlohmann::json& data) {
  try {
    return request("POST", path, &data);
  } catch (const std::runtime_error&) {
    return "";
  }
}

void MQSSRestClient::del(const std::string& path) { request("DELETE", path); }
