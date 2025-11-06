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

#include "types.h"


int MQSS_client_set_parameter(MQSS_Client client, MQSS_Client_Parameter parameter, void* value);

int MQSS_client_check_connection(MQSS_Client client);

int MQSS_client_connect(MQSS_Client client);

int MQSS_client_disconnect(MQSS_Client client);

int MQSS_client_submit_request(MQSS_Client client, MQSS_Client_Request request);



