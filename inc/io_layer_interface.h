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
 * @file io_layer_interface.h
 * API used by IO layer implementations.
 */

#ifndef IO_LAYER_INTERFACE_H
#define IO_LAYER_INTERFACE_H

#include "max22x88.h"

/**
 * @brief Stores received data in driver's rx buffer.
 * @note This function is called by the IO layer when incoming data is detected.
 * 
 * @param[in] driver the driver that received data
 * @param[in] data data received
 * @return adi_max22x88_Result_e 
 */
adi_max22x88_Result_e adi_max22x88_DataReceived(adi_max22x88_t* driver, uint8_t data);

#endif
