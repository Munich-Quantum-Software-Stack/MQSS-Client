/*
 * Copyright (c) 2024 - 2026 MQSS Project
 * All rights reserved.
 *
 * Licensed under the Apache License v2.0 with LLVM Exceptions (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://llvm.org/LICENSE.txt
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#ifdef __cplusplus
extern "C" {
#endif
#include "job.h"
#include "resource.h"

#include <stdbool.h>

typedef struct MQSSOpaqueClient* MQSSClientRef;

MQSSClientRef MQSSClientCreateClient(char* token, char* urlOrQueue, bool isHpc);

MQSSResourceRef* MQSSClientGetAllResources(MQSSClientRef client, int* size);

MQSSResourceRef MQSSClientGetResourceInfo(MQSSClientRef client,
                                           char* resourceName);

int MQSSClientSubmitJob(MQSSClientRef client, MQSSJobRef job);

void MQSSClientCancelJob(MQSSClientRef client, MQSSJobRef job);

MQSSJobResultRef MQSSClientGetJobResult(MQSSClientRef client, MQSSJobRef job,
                                        bool wait, unsigned int timeout);

int MQSSClientGetNumberPendingJobs(MQSSClientRef client, char* resourceName);
#ifdef __cplusplus
}
#endif
