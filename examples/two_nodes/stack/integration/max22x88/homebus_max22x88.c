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

#include "homebus_max22x88.h"

hbs_err_e adi_hbs_TxCbMax22x88(uint8_t* data, size_t sz, void* param)
{
    adi_max22x88_t* p_driver = param;
    adi_max22x88_Result_e err = adi_max22x88_Transmit(p_driver, data, sz);
    if (err == MAX22X88_ERR_OK) {
        return HBS_ERR_OK;
    } else {
        return HBS_ERR_CB_FAILED;
    }
}

hbs_err_e adi_hbs_InitMax22x88(adi_hbs_t* hbs, uint8_t address, adi_max22x88_t* driver)
{
    return adi_hbs_Init(hbs, address, adi_hbs_TxCbMax22x88, driver);
}
