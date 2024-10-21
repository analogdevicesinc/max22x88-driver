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

#include "max22x88_bitbang.h"
#include "bitbang_hal.h"
#include "private/bitbang_helper.h"
#include "private/max22x88_bitbang_rx_state_machine.h"
#include "private/max22x88_common.h"
#include "private/max22x88_internal.h"
#include <stdlib.h>
#include <string.h>

#define HOMEBUS_DATA_BITS (8)
#define BITS_IN_HOMEBUS_FRAME (HOMEBUS_DATA_BITS + 3)  // + 3 for start, parity, stop bits
#define START_BIT_OFFSET_TICKS (126)

typedef enum {
    MAX22X88_BUS_STATE_IDLE,
    MAX22X88_BUS_STATE_WAIT,
    MAX22X88_BUS_STATE_RX,
    MAX22X88_BUS_STATE_TX,
    MAX22X88_BUS_STATE_UNKNOWN,
} max22x88_bus_state_e;

/**
 * Context used for bitbang implementation.
 * 
 */
typedef struct
{
    volatile _adi_bitbang_sm_t rx_sm;
    uint32_t baud_rate;
    uint32_t half_bit_initial_cnt;
    uint32_t cnt_for_start_bit_sample;
    uint32_t half_bit_cmp;
    volatile int error_log[BITBANG_LOG_MAX];
    volatile uint32_t* data_to_tx;
    volatile size_t tx_len;
    volatile size_t tx_current_byte;
    volatile size_t tx_current_bit;
    volatile max22x88_bus_state_e bus_state;
    bool perform_bit_collation;
    bool last_bit_tx;
} max22x88_bitbang_ctx_t;

static adi_max22x88_t* _driver = NULL;

/**
 * @brief Max32670 implementation (GPIO bitbang) for Max22x88 init callback.
 * 
 * @param driver 
 * @param ctx 
 * @param user_params
 * @return adi_max22x88_Result_e 
 */
static adi_max22x88_Result_e max22x88_gpio_bitbang_init(adi_max22x88_t* driver, void* ctx, void* user_params);

/**
 * @brief 
 * 
 * @param driver 
 * @param data 
 * @param count 
 * @return adi_max22x88_Result_e 
 */
static adi_max22x88_Result_e max22x88_write_bitbang(adi_max22x88_t *driver, uint8_t* data, size_t count);

/**
 * @brief The timer interrupt indicating that the next bit should be written (during Tx) or the next bit should be read (during Rx).
 * 
 */
static void signal_timer_isr(void);

static void max22x88_bitbang_init_ctx(max22x88_bitbang_ctx_t* ctx, adi_max22x88_bitbang_InitParams_t* user_params);

/**
 * @brief Starts an asynchronous transmission.
 * 
 * @param ctx 
 */
static void start_transmission(max22x88_bitbang_ctx_t* ctx);

/**
 * @brief Configures the timer interrupt according to the desired Home Bus baud rate.
 * 
 * @param ctx 
 * @return int status code.
 */
static int configure_homebus_signal_timer(max22x88_bitbang_ctx_t* ctx);

/**
 * @brief Enables the hbs timer interrupt. Used by rxing and txing routines.
 * 
 * @param ctx 
 * @param initial_cnt Timer will be loaded with this CNT value.
 */
static void begin_hbs_timing(max22x88_bitbang_ctx_t* ctx, uint32_t initial_cnt);

/**
 * @brief Stops the hbs timer interrupt.
 * 
 * @param ctx 
 */
static void stop_hbs_timing(max22x88_bitbang_ctx_t* ctx);

/**
 * @brief Reconfigures device to listen for another Home Bus packet.
 * 
 * @param ctx 
 */
static void restart_rxing(max22x88_bitbang_ctx_t* ctx);

/**
 * @brief Converts 8 logic values into Home Bus values, by adding start, parity, stop bits and stuffing them with 1s.
 * 
 * @param value 
 * @return uint32_t 
 */
static uint32_t format_byte_for_hbs_tx(uint8_t value);

static void max22x88_handle_interrupt_tx(adi_max22x88_t* driver, max22x88_bitbang_ctx_t* ctx, int sample);

/**
 * @brief Process the incoming bit from Home Bus.
 * 
 * @param ctx 
 */
static void max22x88_handle_interrupt_rx(adi_max22x88_t* driver, max22x88_bitbang_ctx_t* ctx, int sample);

static bool max22x88_bitbang_log(adi_max22x88_t* driver, adi_max22x88_bitbang_LogCode_e code);

/**
 * @brief Handle a collision during transmission. Collisions are detected by a transmitting device when
 * it tries to transmit a "1" but it reads back a "0" from the Home Bus line.
 * 
 * @note This function is not yet implemented.
 * 
 * @param driver 
 * @param ctx 
 */
static void handle_collision(adi_max22x88_t* driver, max22x88_bitbang_ctx_t* ctx);

adi_max22x88_Result_e adi_max22x88_FallingEdgeIntCallback(void)
{
    adi_max22x88_hal_TimerStartSignal();
    if (_driver == NULL) {
        adi_max22x88_hal_TimerStopSignal();
        return MAX22X88_ERR_USER_FN;
    }
    max22x88_bitbang_ctx_t* ctx = adi_max22x88_GetLowLevelCtx(_driver);
    bool sm_result = _adi_bitbang_sm_EventStartBitEdge(&ctx->rx_sm);
    if (!sm_result) {
        adi_max22x88_hal_TimerStopSignal();
        adi_max22x88_hal_TimerSetCountSignal(ctx->cnt_for_start_bit_sample);  // Configure for reading
        return MAX22X88_ERR_INTERNAL;
    }

    // Disable the falling edge interrupt while the frame is being received
    // It is reenabled afterwards
    adi_max22x88_hal_GpioIntDisableDout();

    ctx->bus_state = MAX22X88_BUS_STATE_RX;
    return MAX22X88_ERR_OK;
}

static void start_transmission(max22x88_bitbang_ctx_t* ctx)
{
    ctx->bus_state = MAX22X88_BUS_STATE_TX;
    ctx->tx_current_bit = 0;
    ctx->tx_current_byte = 0;
    begin_hbs_timing(ctx, ctx->half_bit_initial_cnt);
}

static int configure_homebus_signal_timer(max22x88_bitbang_ctx_t* ctx)
{
    adi_max22x88_hal_TimerShutdowSignal();
    adi_max22x88_hal_NvicSetVectorSignal(signal_timer_isr);
    adi_max22x88_hal_NvicEnableSignal();
    adi_max22x88_hal_TimerInitSignal(ctx->half_bit_cmp);
    adi_max22x88_hal_TimerSetCountSignal(ctx->cnt_for_start_bit_sample);  // Configure for reading
    adi_max22x88_hal_TimerIntEnableSignal();
    return 0;
}

static void handle_collision(adi_max22x88_t* driver, max22x88_bitbang_ctx_t* ctx)
{
    // Not implemented
}

static void max22x88_handle_interrupt_tx(adi_max22x88_t* driver, max22x88_bitbang_ctx_t* ctx, int sample)
{
    if (ctx->tx_current_byte == ctx->tx_len) {
        ctx->bus_state = MAX22X88_BUS_STATE_IDLE;
        return;
    }

    if (ctx->perform_bit_collation) {
        bool bit = sample;
        ctx->perform_bit_collation = false;
        if (bit != ctx->last_bit_tx) {
            handle_collision(driver, ctx);
        }
    } else {
        uint32_t data_to_tx = ctx->data_to_tx[ctx->tx_current_byte];
        bool bit_to_tx = data_to_tx & (1 << ctx->tx_current_bit);
        if (bit_to_tx) {
            adi_max22x88_hal_GpioSetDin();
        } else {
            adi_max22x88_halGpioClearDin();
        }
        ctx->perform_bit_collation = true;
        ctx->last_bit_tx = bit_to_tx;
        ctx->tx_current_bit++;
        size_t bits_in_frame = BITS_IN_HOMEBUS_FRAME * 2;
        if (ctx->tx_current_bit == bits_in_frame) {
            ctx->tx_current_bit = 0;
            ctx->tx_current_byte++;
        }
    }
}

static void max22x88_handle_interrupt_rx(adi_max22x88_t* driver, max22x88_bitbang_ctx_t* ctx, int sample)
{
    bool sm_status = false;
    bool finished = false;
    if (_adi_bitbang_sm_WantSample(&ctx->rx_sm)) {
        uint32_t reading = sample;
        _adi_bitbang_sm_Result_t result = { 0 };
        sm_status = _adi_bitbang_sm_EventSample(&ctx->rx_sm, reading, &finished, &result);
        if (sm_status && finished) {
            if (result.error_flags == RX_SM_ERROR_NO_ERROR) {
                adi_max22x88_Result_e err = adi_max22x88_DataReceived(driver, result.data);
                switch (err) {
                    case MAX22X88_ERR_OK:
                        max22x88_bitbang_log(driver, BITBANG_LOG_FRAME_VALID);
                        break;
                    case MAX22X88_ERR_RX_BUFFER_FULL:
                        max22x88_bitbang_log(driver, BITBANG_LOG_RX_OVF);
                        break;
                    default:
                        max22x88_bitbang_log(driver, BITBANG_LOG_INTERNAL_ERROR);
                }
            } else {
                max22x88_bitbang_log(driver, BITBANG_LOG_FRAME_BAD);
                if (result.error_flags & RX_SM_ERROR_OFFDUTY_SAMPLE) {
                    max22x88_bitbang_log(driver, BITBANG_LOG_FRAME_BAD_OFFDUTY);
                }
                if (result.error_flags & RX_SM_ERROR_PARITY_ERROR) {
                    max22x88_bitbang_log(driver, BITBANG_LOG_FRAME_BAD_PARITY);
                }
                if (result.error_flags & RX_SM_ERROR_START_BIT_SAMPLE) {
                    max22x88_bitbang_log(driver, BITBANG_LOG_FRAME_BAD_START);
                }
                if (result.error_flags & RX_SM_ERROR_STOP_BIT_SAMPLE) {
                    max22x88_bitbang_log(driver, BITBANG_LOG_FRAME_BAD_STOP);
                }
            }
            restart_rxing(ctx);
        }
    } else {
        sm_status = _adi_bitbang_sm_EventEdge(&ctx->rx_sm);
    }
    if (!sm_status) {
        max22x88_bitbang_log(_driver, BITBANG_LOG_INTERNAL_ERROR);
    }
}

static void signal_timer_isr(void)
{
    // Sample DOUT preemptively.
    // Depending on why this isr was triggered, the value may be unused.
    // However, if it is used, the reading has to happen at this point in time.
    int sample = adi_max22x88_hal_GpioReadDout();

    max22x88_bitbang_ctx_t* ctx = adi_max22x88_GetLowLevelCtx(_driver);
    adi_max22x88_hal_TimerClearFlagsSignalInterrupt();
    switch (ctx->bus_state) {
        case MAX22X88_BUS_STATE_TX:
            max22x88_handle_interrupt_tx(_driver, ctx, sample);
            break;
        case MAX22X88_BUS_STATE_RX:
            max22x88_handle_interrupt_rx(_driver, ctx, sample);
            break;
        case MAX22X88_BUS_STATE_IDLE:  // fallthrough
        case MAX22X88_BUS_STATE_WAIT:  // fallthrough
        case MAX22X88_BUS_STATE_UNKNOWN:
            // signal_timer_isr is not supposed be called in these bus states
            max22x88_bitbang_log(_driver, BITBANG_LOG_INTERNAL_ERROR);
            break;
    }
}

static void begin_hbs_timing(max22x88_bitbang_ctx_t* ctx, uint32_t initial_cnt)
{
    adi_max22x88_hal_TimerSetCountSignal(initial_cnt);
    adi_max22x88_hal_TimerStartSignal();
}

static void restart_rxing(max22x88_bitbang_ctx_t* ctx)
{
    stop_hbs_timing(ctx);
    adi_max22x88_hal_GpioIntEnableDout();
}

static void stop_hbs_timing(max22x88_bitbang_ctx_t* ctx)
{
    ctx->bus_state = MAX22X88_BUS_STATE_IDLE;
    adi_max22x88_hal_TimerStopSignal();
    adi_max22x88_hal_TimerSetCountSignal(ctx->cnt_for_start_bit_sample);  // Configure for readings
    adi_max22x88_hal_TimerClearFlagsSignalInterrupt();
}

const adi_max22x88_Functions_t max22x88_bitbang_functions = {
    .init_fn = max22x88_gpio_bitbang_init,
    .ctx_size = sizeof(max22x88_bitbang_ctx_t),
    .set_rst_state_fn = adi_max22x88_SetTxStateGpio,
    .write_fn = max22x88_write_bitbang
};

adi_max22x88_Result_e adi_max22x88_InitBitbang(adi_max22x88_t* driver, adi_max22x88_bitbang_InitParams_t* params, size_t rx_buffer_len)
{
    return adi_max22x88_Init(
        driver,
        rx_buffer_len,
        max22x88_bitbang_functions,
        params
    );
}

static void max22x88_bitbang_init_ctx(max22x88_bitbang_ctx_t* ctx, adi_max22x88_bitbang_InitParams_t* user_params)
{
    ctx->baud_rate = user_params->hbs_baud * 2;
    ctx->half_bit_cmp =  adi_max22x88_hal_TimerCalcPeriodSignal(ctx->baud_rate * 2);
    ctx->half_bit_initial_cnt = 1;
    ctx->cnt_for_start_bit_sample = ctx->half_bit_initial_cnt + START_BIT_OFFSET_TICKS;
    if (ctx->cnt_for_start_bit_sample > ctx->half_bit_cmp) {
        ctx->cnt_for_start_bit_sample = ctx->half_bit_cmp;
    }

    ctx->perform_bit_collation = false;
    ctx->last_bit_tx = false;
    _adi_bitbang_sm_Init(&ctx->rx_sm);

    ctx->bus_state = MAX22X88_BUS_STATE_UNKNOWN;
    memset((void *)ctx->error_log, 0, sizeof ctx->error_log);
}

static adi_max22x88_Result_e max22x88_gpio_bitbang_init(adi_max22x88_t* driver, void* ctx, void* user_params)
{
    if (_driver != NULL || user_params == NULL ) {
        // The driver has already been initialized. Only one instance is supported.
        return MAX22X88_ERR_BAD_PARAM;
    }

    max22x88_bitbang_init_ctx(ctx, user_params);

    configure_homebus_signal_timer(ctx);

    adi_max22x88_hal_GpioSetRst();
    adi_max22x88_hal_GpioConfigureRst();

    adi_max22x88_hal_GpioSetDin();
    adi_max22x88_hal_GpioConfigureDin();

    adi_max22x88_hal_GpioConfigureDout();

    adi_max22x88_hal_GpioIntConfigureDoutFallingEdge();
    _driver = driver;
    adi_max22x88_hal_GpioIntEnableDout();

    return MAX22X88_ERR_OK;
}

/**
 * @brief Returns formatted bits for HBS transmission. LSB is transmitted first.
 * 
 * @param[in] value 
 * @return uint32_t 
 */
static uint32_t format_byte_for_hbs_tx(uint8_t value) {
    bool parity_bit = _calc_even_parity_u8(value);
    
    // Add start, parity and stop bits
    uint16_t prepared_data = value; // requires using a a bigger data type
    prepared_data |= (parity_bit << 8);  // Add the parity bit after the 8 data bits.
    prepared_data |= (1 << 9);  // Add stop bit after the parity bit. The stop bit has value 1.
    prepared_data <<= 1;  // Add start bit before all other bits. The start bit has value 0.
    uint32_t stuffed_data = _stuff_byte_u32(prepared_data);
    return stuffed_data;
}

static adi_max22x88_Result_e max22x88_write_bitbang(adi_max22x88_t *driver, uint8_t* data, size_t count)
{
    max22x88_bitbang_ctx_t* ctx = adi_max22x88_GetLowLevelCtx(driver);
    
    // Prepare data to Tx
    size_t num_packets_to_tx = sizeof(*data) * count;
    ctx->data_to_tx = malloc(num_packets_to_tx * sizeof(*(ctx->data_to_tx)));
    ctx->tx_len = num_packets_to_tx;
    if (ctx->data_to_tx == NULL) {
        return MAX22X88_ERR_INTERNAL;
    }
    for (size_t i = 0; i < count; i++) {
        ctx->data_to_tx[i] = format_byte_for_hbs_tx(data[i]);
    }

    adi_max22x88_hal_GpioIntDisableDout();
    start_transmission(ctx);
    while (ctx->bus_state == MAX22X88_BUS_STATE_TX)
        ;
    stop_hbs_timing(ctx);
    free((void *)ctx->data_to_tx);  // casting to void* to discard `volatile` qualifier
    adi_max22x88_hal_GpioIntEnableDout();
    return MAX22X88_ERR_OK;
}

static bool max22x88_bitbang_log(adi_max22x88_t* driver, adi_max22x88_bitbang_LogCode_e code)
{
    max22x88_bitbang_ctx_t* ctx = adi_max22x88_GetLowLevelCtx(driver);
    if (code > sizeof ctx->error_log / sizeof *(ctx->error_log)) {
        return false;
    }
    ctx->error_log[code]++;
    return true;
}
