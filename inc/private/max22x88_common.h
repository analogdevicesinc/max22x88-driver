/* 
 * Copyright 2024 Analog Devices, Inc.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     https://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file max22x88_common.h
 * Common functions that can be shared by IO layer implementations.
 */

#ifndef PRIVATE_MAX22X88_COMMON_H
#define PRIVATE_MAX22X88_COMMON_H

#include "max22x88.h"

/**
 * @brief Sets the transceivers's Tx state.
 * 
 * @param[in] driver the driver.
 * @param[in] state `true` to enabled, `false` for disabled.
 * @return adi_max22x88_Result_e 
 */
adi_max22x88_Result_e adi_max22x88_SetTxStateGpio(adi_max22x88_t* driver, bool state);

#endif
