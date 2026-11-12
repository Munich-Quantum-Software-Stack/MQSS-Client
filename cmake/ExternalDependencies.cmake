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

include(FetchContent)
set(FETCH_PACKAGES "")

find_package(Boost CONFIG QUIET OPTIONAL_COMPONENTS uuid)
if(TARGET Boost::uuid)
  set(MQSS_BOOST_UUID_TARGET Boost::uuid)
elseif(Boost_FOUND)
  # Older Boost packages expose header-only UUID through the headers target.
  find_path(
    MQSS_BOOST_UUID_INCLUDE_DIR boost/uuid/uuid_generators.hpp
    HINTS ${Boost_INCLUDE_DIRS}
    NO_DEFAULT_PATH)
  if(MQSS_BOOST_UUID_INCLUDE_DIR)
    set(MQSS_BOOST_UUID_TARGET Boost::headers)
  endif()
endif()

if(NOT MQSS_BOOST_UUID_TARGET)
  set(BOOST_VERSION
      1.86.0
      CACHE STRING "Boost fallback release")
  set(BOOST_SHA256
      2c5ec5edcdff47ff55e27ed9560b0a0b94b07bd07ed9928b476150e16b0efc57
      CACHE STRING "SHA256 of the Boost fallback archive")
  # UUID is header-only; configure just UUID and its required Boost dependencies.
  set(BOOST_INCLUDE_LIBRARIES uuid)
  set(BOOST_SKIP_INSTALL_RULES OFF)
  FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-${BOOST_VERSION}/boost-${BOOST_VERSION}-cmake.tar.xz
    URL_HASH SHA256=${BOOST_SHA256})
  list(APPEND FETCH_PACKAGES Boost)
  set(MQSS_BOOST_UUID_TARGET Boost::uuid)
endif()

set(NLOHMANN_JSON_VERSION
    3.12.0
    CACHE STRING "Minimum nlohmann_json version and fallback release")
find_package(nlohmann_json ${NLOHMANN_JSON_VERSION} CONFIG QUIET)
if(NOT nlohmann_json_FOUND)
  # The public dependency must be exported alongside the installed MQSS targets.
  set(JSON_Install ON)
  FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v${NLOHMANN_JSON_VERSION}
    GIT_SHALLOW TRUE)
  list(APPEND FETCH_PACKAGES nlohmann_json)
endif()

find_package(rabbitmq-c CONFIG QUIET)
if(NOT rabbitmq-c_FOUND)
  set(RABBITMQ_C_VERSION
      0.13.0
      CACHE STRING "rabbitmq-c fallback release")

  set(BUILD_SHARED_LIBS
      OFF
      CACHE BOOL "" FORCE)

  FetchContent_Declare(
    rabbitmq-c
    GIT_REPOSITORY https://github.com/alanxz/rabbitmq-c.git
    GIT_TAG v${RABBITMQ_C_VERSION}
    GIT_SHALLOW TRUE)
  list(APPEND FETCH_PACKAGES rabbitmq-c)
endif()

if(BUILD_DOCUMENTATION)
  set(CMAKE_POLICY_DEFAULT_CMP0116
      NEW
      CACHE STRING "Set the default CMP0116 policy to NEW for documentation builds")
  set(DOXYGEN_VERSION
      1.15.0
      CACHE STRING "Doxygen version")
  set(DOXYGEN_REV
      "7cca38ba5185457e6d9495bf963d4cdeacebc25a"
      CACHE STRING "Doxygen identifier (tag, branch or commit hash)")
  FetchContent_Declare(
    Doxygen
    GIT_REPOSITORY https://github.com/doxygen/doxygen.git
    GIT_TAG ${DOXYGEN_REV}
    FIND_PACKAGE_ARGS ${DOXYGEN_VERSION})
  list(APPEND FETCH_PACKAGES Doxygen)

  set(DOXYGEN_AWESOME_VERSION
      2.4.1
      CACHE STRING "Doxygen Awesome version")
  set(DOXYGEN_AWESOME_REV
      "1f3620084ff75734ed192101acf40e9dff01d848"
      CACHE STRING "Doxygen Awesome identifier (tag, branch or commit hash)")
  FetchContent_Declare(
    doxygen-awesome-css
    GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
    GIT_TAG ${DOXYGEN_AWESOME_REV}
    FIND_PACKAGE_ARGS ${DOXYGEN_AWESOME_VERSION})
  list(APPEND FETCH_PACKAGES doxygen-awesome-css)
endif()

if(FETCH_PACKAGES)
  FetchContent_MakeAvailable(${FETCH_PACKAGES})
endif()
