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

#include "private/max22x88_common.h"
#include "common_hal.h"

adi_max22x88_Result_e adi_max22x88_SetTxStateGpio(adi_max22x88_t* driver, bool state)
{
    if (state) {
        // Set RST GPIO to LOW (enabled)
        adi_max22x88_hal_GpioClearRst();
    } else {
        // Set RST GPIO to HIGH (disabled)
        adi_max22x88_hal_GpioSetRst();
    }
    return MAX22X88_ERR_OK;
}
