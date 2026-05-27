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

find_package(pybind11 REQUIRED)
pybind11_add_module(mqss THIN_LTO OPT_SIZE bindings.cpp)
target_include_directories(mqss PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(mqss PRIVATE mqss_client)
set_target_properties(mqss PROPERTIES POSITION_INDEPENDENT_CODE ON)
