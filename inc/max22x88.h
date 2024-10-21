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
 * @file max22x88.h
 * Core MAX22088/MAX22288 driver API.
 */

#ifndef MAX22X88_H
#define MAX22X88_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "private/fifo.h"

/**
 * Driver status codes.
 * 
 */
typedef enum {
    MAX22X88_ERR_OK = 0, /*!< Success. */
    MAX22X88_ERR_RX_BUFFER_EMPTY, /*!<  Rx buffer is empty. */
    MAX22X88_ERR_RX_BUFFER_FULL, /*!< Rx buffer is full. */
    MAX22X88_ERR_BAD_PARAM, /*!< Parameter invalid. */
    MAX22X88_ERR_USER_FN, /*!< Error in user integration or IO layer implementation. */
    MAX22X88_ERR_INTERNAL, /*!< Internal error. */
} adi_max22x88_Result_e;

/**
 * Typedef for Max22x88 driver context.
 * 
 */
typedef struct adi_max22x88_t adi_max22x88_t;

/** IO layer initialization function. */
typedef adi_max22x88_Result_e (*adi_max22x88_LowLevelInit_fn)(adi_max22x88_t* driver, void* state, void* init_params);

/** IO layer RST enable/disable function. */
typedef adi_max22x88_Result_e (*adi_max22x88_LowLevelSetRst_fn)(adi_max22x88_t* driver, bool state);

/** IO layer write function. */
typedef adi_max22x88_Result_e (*adi_max22x88_LowLevelWrite_fn)(adi_max22x88_t* driver, uint8_t* data, size_t count);

/**
 * IO layer function arguments.
 * 
 */
typedef struct {
    adi_max22x88_LowLevelInit_fn init_fn; /*!< IO layer initialization function */
    size_t ctx_size; /*!< Context data size required by the IO layer implementation */
    adi_max22x88_LowLevelSetRst_fn set_rst_state_fn; /*!< IO layer RST enable/disable function */
    adi_max22x88_LowLevelWrite_fn write_fn; /*!< IO layer write function */
} adi_max22x88_Functions_t;

/**
 * Max22x88 driver context.
 * 
 */
struct adi_max22x88_t {
    volatile _adi_fifo_t rx_queue;
    void* low_level_ctx;
    adi_max22x88_Functions_t fns;
    bool tx_state;
};

/**
 * @brief Initializes the max22x88 driver.
 * 
 * @param[in] driver The driver to initialize.
 * @param[in] rx_buffer_len Length of the software buffer for incoming data.
 * @param[in] fns The functions used by the IO layer.
 * @param[in] init_params Initialization parameters for the IO layer. Can be NULL.
 * @return adi_max22x88_Result_e 
 */
adi_max22x88_Result_e adi_max22x88_Init(adi_max22x88_t* driver, size_t rx_buffer_len, adi_max22x88_Functions_t fns, void* init_params);

/**
 * @brief Transmits data. The procedure performed is: enables the transmitter, writes the data, then disables the transmitter.
 * 
 * @param[in] driver 
 * @param[in] data data array to be transmitted
 * @param[in] len length of the data to be transmitted
 * @return adi_max22x88_Result_e 
 */
adi_max22x88_Result_e adi_max22x88_Transmit(adi_max22x88_t* driver, uint8_t* data, size_t len);

/**
 * @brief Reads one uint8_t of data from the software buffer.
 * 
 * @param[in] driver 
 * @param[out] data data read
 * @return adi_max22x88_Result_e 
 */
adi_max22x88_Result_e adi_max22x88_Read(adi_max22x88_t* driver, uint8_t* data);

/**
 * @brief Checks if data is available in the software buffer.
 * 
 * @param[in] driver 
 * @retval true Data is available and can be read with adi_max22x88_Read.
 * @retval false No data is available.
 */
bool adi_max22x88_IsAvailable(adi_max22x88_t* driver);

/**
 * @brief Discard data stored in the software buffer.
 * 
 * @param[in] driver
 * @return adi_max22x88_Result_e 
 */
adi_max22x88_Result_e adi_max22x88_FlushRx(adi_max22x88_t* driver);

/**
 * @brief Deinitialize the driver. Frees any dynamically allocated resources that have
 * been allocated.
 * 
 * @param driver driver
 * @return adi_max22x88_Result_e 
 */
adi_max22x88_Result_e adi_max22x88_Deinit(adi_max22x88_t* driver);

#endif
