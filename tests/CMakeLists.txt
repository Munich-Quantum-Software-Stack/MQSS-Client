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

enable_testing()
include(GoogleTest)
find_package(GTest REQUIRED)

function(add_mqss_test test_name)
  add_executable(${test_name} ${test_name}.cpp)

  target_link_libraries(${test_name} PRIVATE GTest::gmock GTest::gtest_main mqss_client)

  if(ENABLE_COVERAGE)
    target_link_libraries(${test_name} PRIVATE coverage_flags)
  endif()

  gtest_discover_tests(${test_name})
endfunction()

if(BUILD_TESTS OR BUILD_UNIT_TESTS)
  add_subdirectory(unit/c++)
endif()

if(BUILD_TESTS OR BUILD_INTEGRATION_TESTS)
  add_subdirectory(integration/c++)
endif()
