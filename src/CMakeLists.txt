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

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(rabbitmq-c REQUIRED)

file(GLOB CLIENT_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/clients/*.cpp")
add_library(mqss_client SHARED ${CLIENT_SRC_FILES} client.cpp job.cpp resource.cpp)

target_include_directories(mqss_client PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(mqss_client PUBLIC curl nlohmann_json rabbitmq)

add_subdirectory(python)
include(GNUInstallDirs)

install(
  TARGETS mqss_client
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
