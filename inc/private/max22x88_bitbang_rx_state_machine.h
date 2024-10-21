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
 * @file max22x88_bitbang_rx_state_machine.h
 * State machine that controls the Rx of packets for the bitbang implementation.
 */

#ifndef PRIVATE_MAX22X88_BITBANG_RX_STATE_MACHINE_H
#define PRIVATE_MAX22X88_BITBANG_RX_STATE_MACHINE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/** State machine status codes. */
typedef enum {
    RX_SM_ERROR_NO_ERROR = 0,
    RX_SM_ERROR_START_BIT_SAMPLE = (1 << 0),
    RX_SM_ERROR_STOP_BIT_SAMPLE = (1 << 1),
    RX_SM_ERROR_OFFDUTY_SAMPLE = (1 << 2),
    RX_SM_ERROR_PARITY_ERROR = (1 << 3),
} _adi_bitbang_sm_FrameStatus_e;

/** The result of an Rx operation. */
typedef struct {
    uint8_t data; /*!< The data received. */
    _adi_bitbang_sm_FrameStatus_e error_flags; /*!< Status flags associated with the data. */
} _adi_bitbang_sm_Result_t;

/** The state machine object. */
typedef struct {
    _adi_bitbang_sm_Result_t result;
    size_t sampled_data_bit_cnt;
    size_t sampled_total_bit_cnt;
    bool expecting_edge;
    bool ongoing;
    bool expecting_offduty_bit;
} _adi_bitbang_sm_t;

/**
 * @brief Initialize the Rx state machine.
 * The state machine responds to the events `max22x88_bitbang_rx_sm_event_*`.
 * @param[in] sm the state machine.
 */
void _adi_bitbang_sm_Init(volatile _adi_bitbang_sm_t* sm);

/**
 * @brief Signals that the start bit edge was detected.
 * 
 * @param[in] sm the state machine.
 * @retval true the event is valid for the current state and has been processed.
 * @retval false otherwise.
 */
bool _adi_bitbang_sm_EventStartBitEdge(volatile _adi_bitbang_sm_t* sm);

/**
 * @brief Signals that a bit edge was detected.
 * 
 * @param[in] sm the state machine.
 * @retval true the event is valid for the current state and has been processed.
 * @retval false otherwise
 */
bool _adi_bitbang_sm_EventEdge(volatile _adi_bitbang_sm_t* sm);

/**
 * @brief Signals that a bit was sampled.
 * 
 * @param[in] sm the state machine.
 * @param[in] bit the bit sampled.
 * @param[out] finished Set to `true` if the if the bit sampled was the last bit in the frame.
 * @param[out] result contains the result if `finished` is set to `true`.
 * @retval true 
 * @retval false 
 */
bool _adi_bitbang_sm_EventSample(volatile _adi_bitbang_sm_t* sm, bool bit, bool* finished, _adi_bitbang_sm_Result_t* result);

/**
 * @brief Checks if the state machine expects a sample event.
 * 
 * @param[in] sm the state machine
 * @retval true sample is expected.
 * @retval false sample is not expected.
 */
bool _adi_bitbang_sm_WantSample(volatile _adi_bitbang_sm_t* sm);

#endif
