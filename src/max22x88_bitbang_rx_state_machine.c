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

#include "private/max22x88_bitbang_rx_state_machine.h"
#include "private/bitbang_helper.h"

#define BITS_SAMPLED_IN_PACKET (22)

#define START_BIT_POS 0
#define PARITY_BIT_POS 19
#define STOP_BIT_POS 21

void _adi_bitbang_sm_Init(volatile _adi_bitbang_sm_t* sm)
{
    sm->result.data = 0;
    sm->result.error_flags = RX_SM_ERROR_NO_ERROR;
    sm->sampled_data_bit_cnt = 0;
    sm->sampled_total_bit_cnt = 0;
    sm->expecting_edge = true;
    sm->ongoing = false;
    sm->expecting_offduty_bit = false;
}

bool _adi_bitbang_sm_EventStartBitEdge(volatile _adi_bitbang_sm_t* sm)
{
    if (sm->ongoing) {
        return false;
    }
    sm->ongoing = true;
    sm->expecting_edge = false;
    return true;
}

bool _adi_bitbang_sm_WantSample(volatile _adi_bitbang_sm_t* sm)
{
    return !sm->expecting_edge;
}

bool _adi_bitbang_sm_EventEdge(volatile _adi_bitbang_sm_t* sm)
{
    if (!sm->ongoing || !sm->expecting_edge) {
        return false;
    }
    sm->expecting_edge = false;
    return true;
}

bool _adi_bitbang_sm_EventSample(volatile _adi_bitbang_sm_t* sm, bool bit, bool* finished, _adi_bitbang_sm_Result_t* result)
{
    if (!sm->ongoing || sm->expecting_edge) {
        return false;
    }

    if (sm->expecting_offduty_bit) {
        if (!bit) {
            sm->result.error_flags |= RX_SM_ERROR_OFFDUTY_SAMPLE;
        }
        sm->expecting_offduty_bit = false;
    } else {
        if (sm->sampled_total_bit_cnt == START_BIT_POS) {
            if (bit) {
                sm->result.error_flags |= RX_SM_ERROR_START_BIT_SAMPLE;
                sm->sampled_data_bit_cnt = 0;
            }
        } else if (sm->sampled_total_bit_cnt == PARITY_BIT_POS) {
            bool expected_parity_bit = _calc_even_parity_u8(sm->result.data);
            if (bit != expected_parity_bit) {
                sm->result.error_flags |= RX_SM_ERROR_PARITY_ERROR;
            }
        } else if (sm->sampled_total_bit_cnt == STOP_BIT_POS) {
            if (!bit) {
                sm->result.error_flags |= RX_SM_ERROR_STOP_BIT_SAMPLE;
            }
        } else if (sm->sampled_total_bit_cnt > START_BIT_POS && sm->sampled_total_bit_cnt < PARITY_BIT_POS) {
            if (bit) {
                sm->result.data |= (1 << sm->sampled_data_bit_cnt);
            }
            sm->sampled_data_bit_cnt++;
        }
        sm->expecting_offduty_bit = true;
    }
    sm->sampled_total_bit_cnt++;
    if (sm->sampled_total_bit_cnt == BITS_SAMPLED_IN_PACKET) {
        *finished = true;
        *result = sm->result;
        _adi_bitbang_sm_Init(sm);
    } else {
        *finished = false;
        sm->expecting_edge = true;
    }
    return true;
}
