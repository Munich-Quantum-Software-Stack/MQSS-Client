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

# URL validation and normalization use the curl_url API
find_package(CURL 7.62 REQUIRED)

file(GLOB_RECURSE MQSS_CLIENT_SOURCES **.cpp)

if(MQSSCLIENT_BUILD_SHARED_LIBS)
  add_library(mqss_client SHARED ${MQSS_CLIENT_SOURCES})
  if(APPLE)
    set_target_properties(
      mqss_client
      PROPERTIES MACOSX_RPATH ON
                 INSTALL_NAME_DIR "@rpath"
                 INSTALL_RPATH "@loader_path")
  elseif(UNIX)
    set_target_properties(mqss_client PROPERTIES INSTALL_RPATH "$ORIGIN")
  endif()
else()
  add_library(mqss_client STATIC ${MQSS_CLIENT_SOURCES})
  set_target_properties(mqss_client PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

target_include_directories(
  mqss_client PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
                     $<INSTALL_INTERFACE:include>)

if(TARGET rabbitmq::rabbitmq-static)
  set(MQSS_RABBITMQ_TARGET rabbitmq::rabbitmq-static)
elseif(TARGET rabbitmq-static)
  set(MQSS_RABBITMQ_TARGET rabbitmq-static)
elseif(TARGET rabbitmq)
  set(MQSS_RABBITMQ_TARGET rabbitmq)
else()
  message(FATAL_ERROR "Static RabbitMQ library not found. "
                      "MQSS Python extension requires static RabbitMQ.")
endif()

target_link_libraries(mqss_client PUBLIC CURL::libcurl nlohmann_json::nlohmann_json
                                         ${MQSS_RABBITMQ_TARGET} ${MQSS_BOOST_UUID_TARGET})

include(GNUInstallDirs)

install(
  TARGETS mqss_client
  EXPORT mqss_client_targets
  LIBRARY DESTINATION . COMPONENT mqss_client_bindings
  ARCHIVE DESTINATION . COMPONENT mqss_client_bindings
  RUNTIME DESTINATION . COMPONENT mqss_client_bindings)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
  EXPORT mqss_client_targets
  FILE MQSSClientTargets.cmake
  NAMESPACE MQSS::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mqss_client)
