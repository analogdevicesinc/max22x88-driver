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
 * @file homebus_max22x88.h
 * Protocol stack <-> max22x88 integration.
 */

#ifndef HOMEBUS_MAX22X88_H
#define HOMEBUS_MAX22X88_H

#include "homebus.h"
#include "max22x88.h"

/**
 * @brief Max22x88 driver Tx callback for protocol stack.
 * 
 * @param data data to transmit
 * @param sz length of data to transmit
 * @param param pointer to driver object
 * @return hbs_err_e 
 */
hbs_err_e adi_hbs_TxCbMax22x88(uint8_t* data, size_t sz, void* param);

/**
 * @brief Initialize the protocol stack with Max22x88 driver integration.
 * 
 * @param hbs protocol stack
 * @param address node address
 * @param driver driver
 * @return hbs_err_e 
 */
hbs_err_e adi_hbs_InitMax22x88(adi_hbs_t* hbs, uint8_t address, adi_max22x88_t* driver);

#endif
