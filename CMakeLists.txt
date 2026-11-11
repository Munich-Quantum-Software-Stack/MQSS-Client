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

# set required cmake version
cmake_minimum_required(VERSION 3.19...3.30)

project(
  MQSS-Client
  VERSION 0.3.0
  DESCRIPTION "MQSS Client"
  LANGUAGES CXX C)

# Generate compile_commands.json to make it easier to work with clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE BOOL "Export compile commands" FORCE)

option(MQSSCLIENT_BUILD_TESTS "Build the tests" OFF)
option(MQSSCLIENT_BUILD_UNIT_TESTS "Build the unit tests" OFF)
option(MQSSCLIENT_BUILD_INTEGRATION_TESTS "Build the unit tests" OFF)
option(MQSSCLIENT_ENABLE_COVERAGE "Enabling coverage" OFF)
option(MQSSCLIENT_BUILD_PYTHON_BINDINGS "Build Python Bindings" OFF)
option(MQSSCLIENT_BUILD_SHARED_LIBS "Build Shared Library " ON)

if(MQSSCLIENT_BUILD_PYTHON_BINDINGS)
  set(MQSSCLIENT_BUILD_SHARED_LIBS
      OFF
      CACHE BOOL "Build static libraries" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(MQSSCLIENT_ENABLE_COVERAGE)
  add_library(coverage_flags INTERFACE)

  target_compile_options(coverage_flags INTERFACE --coverage -fprofile-arcs -ftest-coverage -O0)

  target_link_options(coverage_flags INTERFACE --coverage)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/ExternalDependencies.cmake)

if(MQSSCLIENT_BUILD_TESTS OR MQSSCLIENT_BUILD_UNIT_TESTS)
  add_compile_definitions(ENABLE_UNIT_TEST)
endif()

add_subdirectory(src)

if(MQSSCLIENT_BUILD_TESTS
   OR MQSSCLIENT_BUILD_UNIT_TESTS
   OR MQSSCLIENT_BUILD_INTEGRATION_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if(BUILD_DOCUMENTATION)
  add_subdirectory(docs)
endif()

if(MQSSCLIENT_BUILD_PYTHON_BINDINGS)
  add_subdirectory(bindings)
endif()
