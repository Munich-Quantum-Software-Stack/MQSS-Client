# Copyright 2024 Munich Quantum Software Stack Project
#
# Licensed under the Apache License, Version 2.0 with LLVM Exceptions (the
# "License"); you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# https://github.com/Munich-Quantum-Software-Stack/QDMI-Devices/blob/develop/LICENSE
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# ------------------------------------------------------------------------------

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(rabbitmq-c REQUIRED)

add_library(mqss_client SHARED client.cpp)

target_include_directories(
  mqss_client PUBLIC ${CMAKE_SOURCE_DIR}/include)

  target_link_libraries(mqss_client PUBLIC  curl nlohmann_json rabbitmq
                                        )
