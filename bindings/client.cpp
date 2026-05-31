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

#include "mqss/client.h"

#include "bindings.h"

void registerClientInterface(const py::module& m) {

  py::class_<mqss::client::MQSSClient>(m, "MQSSClient")
      .def(py::init<const std::string&, const std::string&, bool>(),
           py::arg("token") = "", py::arg("url_or_queue") = MQP_DEFAULT_URL,
           py::arg("is_hpc") = false)
      .def_property_readonly("resources",
                             &mqss::client::MQSSClient::getAllResources)
      .def("resource", &mqss::client::MQSSClient::getResourceInfo,
           py::arg("resource"))
      .def("submit_job", &mqss::client::MQSSClient::submitJob,
           py::arg("job_request"))
      .def("cancel_job", &mqss::client::MQSSClient::cancelJob, py::arg("job"))
      .def("job_status", &mqss::client::MQSSClient::getJobStatus,
           py::arg("job"))
      .def("job_results", &mqss::client::MQSSClient::getJobResult,
           py::arg("job"), py::arg("wait") = false, py::arg("timeout") = 100)
      .def("pending_job_count", &mqss::client::MQSSClient::getNumberPendingJobs,
           py::arg("resource"));
}
