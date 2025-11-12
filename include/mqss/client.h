/*------------------------------------------------------------------------------
Copyright 2024 Munich Quantum Software Stack Project

Licensed under the Apache License, Version 2.0 with LLVM Exceptions (the
"License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at

https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
License for the specific language governing permissions and limitations under
the License.

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
------------------------------------------------------------------------------*/
#pragma once
#include <curl/curl.h>
#include <iostream>
#include <memory>
#include <stdbool.h>
#include <string>

class MQSS_Base_Client {
public:
  virtual ~MQSS_Base_Client() = default;
  virtual std::string get(const std::string &path) = 0;
};

class MQSS_Client {

public:

  MQSS_Client(); //HPC with default queue_name

  MQSS_Client(std::string token_or_queue_name, bool is_hpc); // if is_hpc, it is queue_name, else token;
  
  MQSS_Client(std::string token, std::string url);

  std::string getToken() { return token; }

  void setToken(std::string token) { token = token; }

  std::string getUrlOrQueueName() { return url_or_queue_name; }

  void setUrlOrQueueName(std::string url_or_queue_name) {
    url_or_queue_name = url_or_queue_name;
  }

  bool getIsHPC() { return is_hpc; }

  void setIsHPC(bool is_hpc) { is_hpc = is_hpc; }

  std::string getAllResources();

  auto getResourceInfo(std::string resource);

  auto submitJob(std::string jobRequest);

  auto cancelJob(std::string jobRequest);

  auto getJobStatus(std::string jobRequest);

  auto getJobResult(std::string jobRequest);

  auto waitForJobResult(std::string jobRequest);

  auto getNumberPendingJobs(std::string resource);

private:
  std::string token;
  std::string url_or_queue_name;
  bool is_hpc;

  std::unique_ptr<MQSS_Base_Client> client;

  struct curl_slist *headers;
};
