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

#include "bindings.h"

void registerResourceInterface(const py::module& m) {
  py::class_<mqss::client::Resource>(m, "Resource")
      .def_property_readonly("name", &mqss::client::Resource::getName)
      .def_property_readonly("qubit_count",
                             &mqss::client::Resource::getQubitCount)
      .def_property_readonly("online", &mqss::client::Resource::isOnline)
      .def_property_readonly("coupling_map",
                             &mqss::client::Resource::getCouplingMap)
      .def_property_readonly("native_gateset",
                             &mqss::client::Resource::getNativeGateset);

  py::class_<mqss::client::Gate>(m, "Gate")
      .def_property_readonly("name", &mqss::client::Gate::getName)
      .def_property_readonly("qubit_number", &mqss::client::Gate::getQubitNumber)
      .def_property_readonly("parameter_number", &mqss::client::Gate::getParameterNumber)
      .def_property_readonly("supported_qubits",
                             &mqss::client::Gate::getSupportedQubits);
}
