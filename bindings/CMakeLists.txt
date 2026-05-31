# Copyright (c) 2024 - 2026 MQSS Project
# All rights reserved.
#
# Licensed under the Apache License v2.0 with LLVM Exceptions (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# https://llvm.org/LICENSE.txt
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

find_package(pybind11 REQUIRED)

set(MQSS_PYTHON_MODULE_NAME PyMQSSClient)

file(GLOB_RECURSE MQSS_CLIENT_SOURCES **.cpp)

pybind11_add_module(${MQSS_PYTHON_MODULE_NAME} MODULE ${MQSS_CLIENT_SOURCES})

target_include_directories(${MQSS_PYTHON_MODULE_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include
                                                              ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${MQSS_PYTHON_MODULE_NAME} PRIVATE mqss_client)

set_target_properties(${MQSS_PYTHON_MODULE_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN")

install(
  TARGETS ${MQSS_PYTHON_MODULE_NAME}
  DESTINATION .
  COMPONENT mqss_client_bindings)
