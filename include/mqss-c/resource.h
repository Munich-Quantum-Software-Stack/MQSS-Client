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
#include <stdbool.h>

typedef struct MQSSOpaqueResource* MQSSResourceRef;

typedef struct MQSSOpaqueGate* MQSSGateRef;

int mqssClientResourceGetInfo(MQSSResourceRef resource, char** name,
                              unsigned* qubitCount, bool* online,
                              int** couplingMap, MQSSGateRef** nativeGateset,
                              unsigned int* gateCount);

int mqssClientResourceGetGateInfo(MQSSGateRef gate, unsigned int qubitNumber,
                                  unsigned int parameterNumber,
                                  int* supportedQubits);

#ifdef __cplusplus
}
#endif
