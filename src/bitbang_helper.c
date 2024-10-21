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
#include "private/bitbang_helper.h"

#define HBS_32_BIT_PATTERN (0b10101010101010101010101010101010)

uint32_t _stuff_byte_u32(uint16_t value)
{
    uint32_t result =
        ((value & (1 << 0)) << 0) |
        ((value & (1 << 1)) << 1) |
        ((value & (1 << 2)) << 2) |
        ((value & (1 << 3)) << 3) |
        ((value & (1 << 4)) << 4) |
        ((value & (1 << 5)) << 5) |
        ((value & (1 << 6)) << 6) |
        ((value & (1 << 7)) << 7) |
        ((value & (1 << 8)) << 8) |
        ((value & (1 << 9)) << 9) |
        ((value & (1 << 10)) << 10) |
        ((value & (1 << 11)) << 11) |
        ((value & (1 << 12)) << 12) |
        ((value & (1 << 13)) << 13) |
        ((value & (1 << 14)) << 14) |
        ((value & (1 << 15)) << 15) |
        HBS_32_BIT_PATTERN;
    return result;
}

static const bool parity_lookup_table_256[] = {
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0
};

bool _calc_even_parity_u8(uint8_t value)
{
    return parity_lookup_table_256[value];
}
