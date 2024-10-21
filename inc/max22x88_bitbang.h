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
 * @file max22x88_bitbang.h
 * API for the bitbang implementation.
 */

#ifndef MAX22X88_BITBANG_H
#define MAX22X88_BITBANG_H

#include "max22x88.h"
#include "io_layer_interface.h"

/**
 * Status codes logged by the bitbang implementation.
 * 
 */
typedef enum {
    BITBANG_LOG_FRAME_VALID, /*!< Frame is valid*/
    BITBANG_LOG_FRAME_BAD, /*!< Frame contains error(s) */
    BITBANG_LOG_FRAME_BAD_START, /*!< Frame error: the start bit was sampled "high" */
    BITBANG_LOG_FRAME_BAD_OFFDUTY, /*!< Frame error: some off-duty bit-time was sample "low" */
    BITBANG_LOG_FRAME_BAD_PARITY, /*!< Frame error: partiy bit is incorrect */
    BITBANG_LOG_FRAME_BAD_STOP, /*!< Frame error: the stop bit was sampled "low" */
    BITBANG_LOG_RX_OVF, /*!< Rx buffer is full */
    BITBANG_LOG_INTERNAL_ERROR, /*!< Internal error */
    BITBANG_LOG_MAX  // Keep BITBANG_LOG_MAX as the last entry
} adi_max22x88_bitbang_LogCode_e;

/**
 * Initialization parameters for bitbang IO layer.
 * 
 */
typedef struct {
    uint32_t hbs_baud; /*!< Home Bus System baud rate. The effective bitrate will be twice this value due to the 50% duty cycle. */
} adi_max22x88_bitbang_InitParams_t;

/**
 * Arguments for initializing driver with bitbang implementation.
 * 
 */
extern const adi_max22x88_Functions_t max22x88_bitbang_functions;

/**
 * @brief Initializes the max22x88 driver with the bitbang implementation.
 * 
 * @param[in] driver the driver to initialize
 * @param[in] params initialization parameters
 * @param[in] rx_buffer_len the length of the rx buffer to be allocated
 * @return adi_max22x88_Result_e 
 */
adi_max22x88_Result_e adi_max22x88_InitBitbang(adi_max22x88_t* driver, adi_max22x88_bitbang_InitParams_t* params, size_t rx_buffer_len);

/**
 * @brief This function must be called when a falling edge interrupt is triggered for the pin connected to DOUT.
 * 
 * @return adi_max22x88_Result_e 
 */
adi_max22x88_Result_e adi_max22x88_FallingEdgeIntCallback(void);

#endif
