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

include(GoogleTest)

add_executable(resource_unit_test resource_unit_test.cpp)

target_link_libraries(resource_unit_test PRIVATE gtest gtest_main mqss_client)
gtest_discover_tests(resource_unit_test)

add_executable(job_unit_test job_unit_test.cpp)

target_link_libraries(job_unit_test PRIVATE gtest gtest_main mqss_client)
gtest_discover_tests(job_unit_test)

if(ENABLE_COVERAGE)
  target_compile_options(resource_unit_test INTERFACE --coverage -fprofile-arcs -ftest-coverage -O0)
  target_link_libraries(resource_unit_test INTERFACE gcov --coverage)

  target_compile_options(job_unit_test INTERFACE --coverage -fprofile-arcs -ftest-coverage -O0)
  target_link_libraries(job_unit_test INTERFACE gcov --coverage)
endif()
