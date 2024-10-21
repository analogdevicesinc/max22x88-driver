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
 * @file max22x88_internal.h
 * 
 * Internal functionality of the MAX22088/MAX22288 driver.
 */

#ifndef PRIVATE_MAX22X88_H
#define PRIVATE_MAX22X88_H

#include "max22x88.h"

/**
 * @brief Write data to the bus.
 * 
 * @param[in] driver the driver.
 * @param[in] data data to be written.
 * @param[in] len length of the data to be written.
 * @return MAX22X88_ERR_RX_BUFFER_FULL
 * @return MAX22X88_ERR_OK 
 */
adi_max22x88_Result_e adi_max22x88_Write(adi_max22x88_t* driver, uint8_t* data, size_t len);

/**
 * @brief Getter for the IO layer context.
 * 
 * @param[in] driver the driver.
 * @return The context of the IO layer. Can be cast to the which `user_params` pointed to during the `adi_max22x88_Init` call.
 */
void* adi_max22x88_GetLowLevelCtx(adi_max22x88_t* driver);

/**
 * @brief Sets the transceivers's Tx state.
 * 
 * @param[in] driver the driver.
 * @param[in] state `true` to enabled, `false` for disabled.
 * @return adi_max22x88_Result_e 
 */
adi_max22x88_Result_e adi_max22x88_SetTxState(adi_max22x88_t* driver, bool state);

#endif
