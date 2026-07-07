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

#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
typedef struct MQSSOpaqueJob* MQSSJobRef;

typedef struct MQSSOpaqueJobResult* MQSSJobResultRef;

MQSSJobRef MQSSClientCreateCircuitJob(char* circuit, char* circuitFormat,
                                      char* resourceName, unsigned int shots,
                                      bool noModify, bool queued);

MQSSJobRef MQSSClientCreateHamiltonianJob(char* resourceName,
                                          char* interactionStr,
                                          char* coefficientsStr);

int MQSSClientGetJobResultCounts(MQSSJobResultRef jobResult, char*** bitstreams,
                                 int** counts, int* size);

int MQSSClientGetJobResultCompletedTimestamp(MQSSJobResultRef jobResult,
                                             uint64_t* completedTimestamp);

int MQSSClientGetJobResultSubmittedTimestamp(MQSSJobResultRef jobResult,
                                             uint64_t* submittedTimestamp);

int MQSSClientGetJobResultScheduledTimestamp(MQSSJobResultRef jobResult,
                                             uint64_t* scheduledTimestamp);

#ifdef __cplusplus
}
#endif
