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

#include "max22x88.h"
#include "private/max22x88_internal.h"
#include <stdlib.h>

/**
 * @brief Allocates the context used by the IO layer implementation. If the context size is zero, the context is set to NULL.
 * 
 * @param driver driver
 * @param ctx_size context size
 * @retval true initialization succesful.
 * @retval false initialization failed.
 */
static bool initialize_context(adi_max22x88_t* driver, size_t ctx_size);

/**
 * @brief Initialize the IO layer.
 * 
 * @param driver driver
 * @param fns IO layer functions
 * @param io_init_param IO layer initialization argument
 * @return adi_max22x88_Result_e 
 */
static adi_max22x88_Result_e initialize_io_layer(adi_max22x88_t* driver, adi_max22x88_Functions_t fns, void* io_init_param);

/**
 * @brief Deinitialize the IO layer.
 * 
 * @param driver driver
 * @return adi_max22x88_Result_e 
 */
static adi_max22x88_Result_e deinitialize_io_layer(adi_max22x88_t* driver);

adi_max22x88_Result_e adi_max22x88_Init(adi_max22x88_t* driver,
    size_t rx_buffer_len,
    adi_max22x88_Functions_t fns,
    void* user_params)
{
    if (driver == NULL || fns.write_fn == NULL || fns.set_rst_state_fn == NULL) {
        return MAX22X88_ERR_BAD_PARAM;
    }

    adi_max22x88_Result_e ret;

    if (_adi_fifo_Init(&driver->rx_queue, rx_buffer_len, sizeof(uint8_t)) != FIFO_ERR_OK) {
        ret = MAX22X88_ERR_INTERNAL;
        goto err_1;
    }

    ret = initialize_io_layer(driver, fns, user_params);
    if (ret != MAX22X88_ERR_OK) {
        goto err_2;
    }

    ret = adi_max22x88_SetTxState(driver, false);
    if (ret != MAX22X88_ERR_OK) {
        ret = MAX22X88_ERR_USER_FN;
        goto err_3;
    }

    return MAX22X88_ERR_OK;

err_3:
    deinitialize_io_layer(driver);
err_2:
    _adi_fifo_Free(&driver->rx_queue);
err_1:
    return ret;
}

void* adi_max22x88_GetLowLevelCtx(adi_max22x88_t* driver)
{
    if (driver == NULL) {
        return NULL;
    }

    return driver->low_level_ctx;
}

adi_max22x88_Result_e adi_max22x88_SetTxState(adi_max22x88_t* driver, bool state)
{
    if (driver == NULL) {
        return MAX22X88_ERR_BAD_PARAM;
    }

    adi_max22x88_Result_e err = driver->fns.set_rst_state_fn(driver, state);
    if (err != MAX22X88_ERR_OK) {
        return err;
    }
    driver->tx_state = state;
    return MAX22X88_ERR_OK;
}

adi_max22x88_Result_e adi_max22x88_Transmit(adi_max22x88_t* driver, uint8_t* data, size_t len)
{
    if (driver == NULL || data == NULL || len == 0) {
        return MAX22X88_ERR_BAD_PARAM;
    }

    adi_max22x88_Result_e err;

    err = adi_max22x88_SetTxState(driver, true);
    if (err != MAX22X88_ERR_OK) {
        return err;
    }

    err = adi_max22x88_Write(driver, data, len);
    if (err != MAX22X88_ERR_OK) {
        return err;
    }

    err = adi_max22x88_SetTxState(driver, false);
    if (err != MAX22X88_ERR_OK) {
        return err;
    }
    
    return MAX22X88_ERR_OK;
}

adi_max22x88_Result_e adi_max22x88_Write(adi_max22x88_t* driver, uint8_t* data, size_t len)
{
    if (!driver->tx_state) {
        return MAX22X88_ERR_INTERNAL;
    }

    return driver->fns.write_fn(driver, data, len);
}

adi_max22x88_Result_e adi_max22x88_Read(adi_max22x88_t* driver, uint8_t* data)
{
    if (driver == NULL) {
        return MAX22X88_ERR_BAD_PARAM;
    }

    _adi_fifo_Result_e fifo_result = _adi_fifo_Pop(&driver->rx_queue, data);
    switch (fifo_result) {
        case FIFO_ERR_OK:
            return MAX22X88_ERR_OK;
            break;
        case FIFO_ERR_BUFFER_EMPTY:
            return MAX22X88_ERR_RX_BUFFER_EMPTY;
            break;
        default:
            return MAX22X88_ERR_INTERNAL;
            break;
    }
}

adi_max22x88_Result_e adi_max22x88_FlushRx(adi_max22x88_t* driver)
{
    if (driver == NULL) {
        return MAX22X88_ERR_BAD_PARAM;
    }

    _adi_fifo_Result_e fifo_result = _adi_fifo_Clear(&driver->rx_queue);
    switch (fifo_result) {
        case FIFO_ERR_OK: // fallthrough
        case FIFO_ERR_BUFFER_EMPTY:
            return MAX22X88_ERR_OK;
            break;
        default:
            return MAX22X88_ERR_INTERNAL;
            break;
    }
}

bool adi_max22x88_IsAvailable(adi_max22x88_t* driver)
{
    if (driver == NULL) {
        return false;
    }

    return !_adi_fifo_IsEmpty(&driver->rx_queue);
}

adi_max22x88_Result_e adi_max22x88_DataReceived(adi_max22x88_t* driver, uint8_t data)
{
    if (driver == NULL) {
        return MAX22X88_ERR_BAD_PARAM;
    }

    _adi_fifo_Result_e fifo_result = _adi_fifo_Push(&driver->rx_queue, &data);
    switch (fifo_result) {
        case FIFO_ERR_OK:
            return MAX22X88_ERR_OK;
            break;
        case FIFO_ERR_BUFFER_FULL:
            return MAX22X88_ERR_RX_BUFFER_FULL;
            break;
        default:
            return MAX22X88_ERR_INTERNAL;
            break;
    }
}

static bool initialize_context(adi_max22x88_t* driver, size_t ctx_size)
{
    if (ctx_size > 0) {
        driver->low_level_ctx = malloc(ctx_size);
        return driver->low_level_ctx != NULL;
    } else {
        driver->low_level_ctx = NULL;
        return true;
    }
}

static adi_max22x88_Result_e initialize_io_layer(adi_max22x88_t* driver, adi_max22x88_Functions_t fns, void* user_params)
{
    if (!fns.init_fn) {
        return MAX22X88_ERR_OK;
    }

    driver->fns = fns;
    if (!initialize_context(driver, fns.ctx_size)) {
        return MAX22X88_ERR_INTERNAL;
    }
    if (fns.init_fn(driver, adi_max22x88_GetLowLevelCtx(driver), user_params) != MAX22X88_ERR_OK) {
        return MAX22X88_ERR_USER_FN;
    }
    return MAX22X88_ERR_OK;
}

static adi_max22x88_Result_e deinitialize_io_layer(adi_max22x88_t* driver)
{
    if (driver == NULL) {
        return MAX22X88_ERR_BAD_PARAM;
    }

    free(driver->low_level_ctx);
    driver->low_level_ctx = NULL;
    return MAX22X88_ERR_OK;
}

adi_max22x88_Result_e adi_max22x88_Deinit(adi_max22x88_t* driver)
{
    if (driver == NULL) {
        return MAX22X88_ERR_BAD_PARAM;
    }

    bool success = true;

    if (_adi_fifo_Free(&driver->rx_queue) != FIFO_ERR_OK) {
        success = false;
    }

    if (deinitialize_io_layer(driver) != MAX22X88_ERR_OK) {
        success = false;
    }

    return success ? MAX22X88_ERR_OK : MAX22X88_ERR_INTERNAL;
}
