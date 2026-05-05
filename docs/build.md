# Build Guide

<!-- IMPORTANT: Keep the line above as the first line. -->
<!----------------------------------------------------------------------------
Copyright 2024 Munich Quantum Software Stack Project

Licensed under the Apache License, Version 2.0 with LLVM Exceptions (the
"License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at

https://github.com/Munich-Quantum-Software-Stack/QDMI-Devices/blob/develop/LICENSE

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
License for the specific language governing permissions and limitations under
the License.

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-------------------------------------------------------------------------- -->

<!-- This file is a static page and included in the CMakeLists.txt file. -->

Ready to use and develop the _MQSS Client_? This guide will help you get started.

## Initial Setup

1. Fork the [MQSS Client](https://github.com/Munich-Quantum-Software-Stack/MQSS-Client) repository
   on GitHub (see [Fork a Repo](https://docs.github.com/en/get-started/quickstart/fork-a-repo).

2. Clone your fork locally

```sh
  git clone git@github.com:your_name_here/MQSS-Client.git
```

3. Change into the project directory

```sh
  cd MQSS-Client
```

4. Create a branch for local development

```sh
  git checkout -b name-of-your-bugfix-or-feature
```

Now you can make your changes locally.

5. (Optional, **highly recommended**) Install [pre-commit](https://pre-commit.com/) to automatically
   Run a set of checks before each commit.

## Prerequisites

To build the _MQSS Client_, the following components are required.

1. CMake, From Version 3.19 to 3.30.
2. Compiler that supports C++-17 (Every version after Clang 5 and GCC 8).
3. Doxygen 1.15 to build documentation.
4. CURL to communicate with the REST API.
5. RabbitMQ-C to communicate with the RabbitMQ Queues.
6. Niels Lohmann's JSON to parse the JSON data.
7. Pybind11 to create Python bindings.

> [!NOTE] We strongly recommend using Dev Containers with Visual Studio Code. We provided the
> required configurations.

## Build

As stated earlier, building the project requires a C++ compiler supporting C++17 and a minimum CMake
version version 3.19.

After cloning the repository and changing the directory, run the command below to configure the
project:

```sh
    cmake -S . -B build
```

After the configuration, run the command below to build the project:

```sh
    cmake --build build
```
