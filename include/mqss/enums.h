/*------------------------------------------------------------------------------
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
------------------------------------------------------------------------------*/

enum MQSS_CLIENT_PARAMETER_T {
    MQSS_CLIENT_TOKEN,
    MQSS_CLIENT_URL_OR_QUEUE,
    MQSS_CLIENT_HPC,
};

typedef enum MQSS_CLIENT_PARAMETER_T MQSS_Client_Parameter;

enum MQSS_REQUEST_TYPES_T {
    MQSS_REQUEST_GET,
    MQSS_REQUEST_POST,
    MQSS_REQUEST_DELET,
};

typedef enum MQSS_REQUEST_TYPES_T MQSS_Request_Type;

