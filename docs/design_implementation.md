# Design

<!-- IMPORTANT: Keep the line above as the first line. -->

<!----------------------------------------------------------------------------
Copyright 2024 Munich Quantum Software Stack Project

Licensed under the Apache License, Version 2.0 with LLVM Exceptions (the
"License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at

https://github.com/Munich-Quantum-Software-Stack/QDMI/blob/develop/LICENSE

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
License for the specific language governing permissions and limitations under
the License.

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-------------------------------------------------------------------------- -->

<!-- This file is a static page and included in the CMakeLists.txt file. -->

<img class="mqss-client" alt="MQSS Client Structure" src="mqss_client_structure.png">

As depicted in Figure above, the **MQSS Client** is a unifying, context-aware access layer and
programming library that decouples the programming interfaces and the underlying compilation and
runtime stack. We implement the _MQSS Client_ using C++17 with
[_Niels Lohmann's JSON_](https://github.com/nlohmann/json) to handle the JSON data format,
[_libcurl_](https://github.com/curl/curl) to establish the remote connection,
[_rabbitmq-c_](https://github.com/alanxz/rabbitmq-c) to handle the HPC connection.

<img class="mqss-client" alt="MQSS Client Class Diagram" src="mqss-client-classdiagram.svg">
