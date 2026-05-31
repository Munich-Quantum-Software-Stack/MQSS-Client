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

#include "mqss/job.h"

#include "bindings.h"

void registerJobInterface(const py::module& m) {
  py::class_<mqss::client::JobRequest>(m, "JobRequest").doc();

  py::class_<mqss::client::CircuitJobRequest, mqss::client::JobRequest>(
      m, "CircuitJobRequest")
      .def(py::init<>())
      .def(py::init<std::string&, std::string&, std::string&, unsigned int,
                    bool, bool>(),
           py::arg("circuit"), py::arg("circuit_format"),
           py::arg("resource_name"), py::arg("shots"), py::arg("no_modify"),
           py::arg("queued"))
      .def_property("circuit", &mqss::client::CircuitJobRequest::getCircuit,
                    &mqss::client::CircuitJobRequest::setCircuit)
      .def_property("circuit_format",
                    &mqss::client::CircuitJobRequest::getCircuitFormat,
                    &mqss::client::CircuitJobRequest::setCircuitFormat)
      .def_property("shots", &mqss::client::CircuitJobRequest::getShots,
                    &mqss::client::CircuitJobRequest::setShots)
      .def_property("resource_name",
                    &mqss::client::CircuitJobRequest::getResourceName,
                    &mqss::client::CircuitJobRequest::setResourceName)
      .def_property("no_modify", &mqss::client::CircuitJobRequest::isNoModify,
                    &mqss::client::CircuitJobRequest::setNoModify)
      .def_property("queued", &mqss::client::CircuitJobRequest::isQueued,
                    &mqss::client::CircuitJobRequest::setQueued);

  py::class_<mqss::client::HamiltonianJobRequest, mqss::client::JobRequest>(
      m, "HamiltonianJobRequest")
      .def(py::init<>())
      .def(py::init<std::string&, std::string&, std::string&>(),
           py::arg("resource_name"), py::arg("interaction"),
           py::arg("coefficients"))
      .def_property("resource_name",
                    &mqss::client::HamiltonianJobRequest::getResourceName,
                    &mqss::client::HamiltonianJobRequest::setResourceName)
      .def_property("interaction",
                    &mqss::client::HamiltonianJobRequest::getInteractionString,
                    &mqss::client::HamiltonianJobRequest::setInteractionString)
      .def_property(
          "coefficients",
          &mqss::client::HamiltonianJobRequest::getCoefficientsString,
          &mqss::client::HamiltonianJobRequest::setCoefficientsString);

  py::class_<mqss::client::JobResult>(m, "JobResult")
      .def_property_readonly("results", &mqss::client::JobResult::getResults)
      .def_property_readonly("completion_timestamp",
                             &mqss::client::JobResult::getTimestampCompleted)
      .def_property_readonly("submission_timestamp",
                             &mqss::client::JobResult::getTimestampSubmitted)
      .def_property_readonly("scheduled_timestamp",
                             &mqss::client::JobResult::getTimestampScheduled);
}
