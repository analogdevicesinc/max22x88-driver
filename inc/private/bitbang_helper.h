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
 * @file bitbang_helper.h
 * Helper functions for the bitbang implementation.
 */

#ifndef PRIVATE_BITBANG_HELPER_H
#define PRIVATE_BITBANG_HELPER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Formats a logic value to be transmitted, LSB-first.
 * This adds a bit with value 1 at every second bit position, representing the off-duty period between the actual data bits.
 * 
 * @param[in] value the logic value
 * @return uint32_t the bits to be encoded, LSB-first.
 */
uint32_t _stuff_byte_u32(uint16_t value);

/**
 * @brief Returns a bit value such that the 8 input bits concatenated to the returned value has an even amount of zeroes.
 * 
 * @param[in] value 
 * @return true 1 bit value.
 * @return false 0 bit value.
 */
bool _calc_even_parity_u8(uint8_t value);

#endif
