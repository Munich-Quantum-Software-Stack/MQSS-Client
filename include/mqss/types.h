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
#include <stdbool.h>
#include "enums.h"

struct MQSS_Client_impl_d {
    char* token;
    char* url_or_queue_name;
    bool is_hpc;
};

typedef struct MQSS_Client_impl_d *MQSS_Client;

struct MQSS_Request_Data_impl_d {
    char* token;
    char* url_or_queue_name;
    bool is_hpc;
};

typedef struct MQSS_Request_Data_impl_d *MQSS_Request_Data;

struct MQSS_Client_Request_impl_d {
    MQSS_Request_Type request_type;
    MQSS_Request_Data data;

};

typedef struct MQSS_Request_impl_d *MQSS_Client_Request;