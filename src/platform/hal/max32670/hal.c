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

#include "hal_config.h"
#include "bitbang_hal.h"
#include "gpio.h"
#include "tmr.h"
#include "nvic_table.h"
#include "max32670_inline_macros.h"

static const mxc_gpio_cfg_t gpio_din_cfg = {
    .drvstr = MXC_GPIO_DRVSTR_0,
    .func = MXC_GPIO_FUNC_OUT,
    .mask = MAX32670_GPIO_DIN_MASK,
    .pad = MXC_GPIO_PAD_PULL_UP,
    .port = MAX32670_GPIO_DIN_PORT,
    .vssel = MXC_GPIO_VSSEL_VDDIOH
};

static const mxc_gpio_cfg_t gpio_rst_cfg = {
    .drvstr = MXC_GPIO_DRVSTR_0,
    .func = MXC_GPIO_FUNC_OUT,
    .mask = MAX32670_GPIO_RST_MASK,
    .pad = MXC_GPIO_PAD_PULL_UP,
    .port = MAX32670_GPIO_RST_PORT,
    .vssel = MXC_GPIO_VSSEL_VDDIOH
};

static const mxc_gpio_cfg_t gpio_dout_cfg = {
    .drvstr = MXC_GPIO_DRVSTR_0,
    .func = MXC_GPIO_FUNC_IN,
    .mask = MAX32670_GPIO_DOUT_MASK,
    .pad = MXC_GPIO_PAD_PULL_UP,
    .port = MAX32670_GPIO_DOUT_PORT,
    .vssel = MXC_GPIO_VSSEL_VDDIOH
};

static const mxc_tmr_cfg_t timer_signal_cfg = {
    .bitMode = MXC_TMR_BIT_MODE_32,
    .clock = MAX32670_TIMER_CLOCK_SIGNAL,
    .mode = MXC_TMR_MODE_CONTINUOUS,
    .pol = 0,
    .pres = MAX32670_TIMER_CLOCK_PRESCALE_SIGNAL
};

void adi_max22x88_hal_GpioConfigureDin(void)
{
    MXC_GPIO_Config(&gpio_din_cfg);
}

void adi_max22x88_hal_GpioSetDin(void)
{
    MXC_GPIO_OutSet(MAX32670_GPIO_DIN_PORT, MAX32670_GPIO_DIN_MASK);
}

void adi_max22x88_halGpioClearDin(void)
{
    MXC_GPIO_OutClr(MAX32670_GPIO_DIN_PORT, MAX32670_GPIO_DIN_MASK);
}

void adi_max22x88_hal_GpioConfigureRst(void)
{
    MXC_GPIO_Config(&gpio_rst_cfg);
}

void adi_max22x88_hal_GpioSetRst(void)
{
    MXC_GPIO_OutSet(MAX32670_GPIO_RST_PORT, MAX32670_GPIO_RST_MASK);
}

void adi_max22x88_hal_GpioClearRst(void)
{
    MXC_GPIO_OutClr(MAX32670_GPIO_RST_PORT, MAX32670_GPIO_RST_MASK);
}

void adi_max22x88_hal_GpioConfigureDout(void)
{
    MXC_GPIO_Config(&gpio_dout_cfg);
}

int adi_max22x88_hal_GpioReadDout(void)
{
    return MXC_GPIO_InGet(MAX32670_GPIO_DOUT_PORT, MAX32670_GPIO_DOUT_MASK);
}

void adi_max22x88_hal_GpioIntConfigureDoutFallingEdge(void)
{
    MXC_GPIO_IntConfig(&gpio_dout_cfg, MXC_GPIO_INT_FALLING);
}

void adi_max22x88_hal_GpioIntEnableDout(void)
{
    MXC_GPIO_EnableInt(MAX32670_GPIO_DOUT_PORT, MAX32670_GPIO_DOUT_MASK);
}

void adi_max22x88_hal_GpioIntDisableDout(void)
{
    MXC_GPIO_DisableInt(MAX32670_GPIO_DOUT_PORT, MAX32670_GPIO_DOUT_MASK);
}

void adi_max22x88_hal_TimerInitSignal(uint32_t cmp)
{
    mxc_tmr_cfg_t cfg = timer_signal_cfg;
    cfg.cmp_cnt = cmp;
    MXC_TMR_Init(MAX32670_TIMER_SIGNAL, &cfg, false);
}

void adi_max22x88_hal_TimerStartSignal(void)
{
    MXC_TMR_Start(MAX32670_TIMER_SIGNAL);
}

void adi_max22x88_hal_TimerStopSignal(void)
{
    MXC_TMR_Stop(MAX32670_TIMER_SIGNAL);
}

void adi_max22x88_hal_TimerShutdowSignal(void)
{
    MXC_TMR_Shutdown(MAX32670_TIMER_SIGNAL);
}

void adi_max22x88_hal_TimerSetCountSignal(uint32_t cnt)
{
    MXC_TMR_SetCount(MAX32670_TIMER_SIGNAL, cnt);
}

void adi_max22x88_hal_TimerIntEnableSignal(void)
{
    MXC_TMR_EnableInt(MAX32670_TIMER_SIGNAL);
}

void adi_max22x88_hal_TimerClearFlagsSignalInterrupt(void)
{
    MXC_TMR_ClearFlags(MAX32670_TIMER_SIGNAL);
}

uint32_t adi_max22x88_hal_TimerCalcPeriodSignal(uint32_t baud_rate)
{
    return MXC_TMR_GetPeriod(MAX32670_TIMER_SIGNAL, MAX32670_TIMER_CLOCK_SIGNAL, MAX32670_TIMER_CLOCK_PRESCALE_VALUE_SIGNAL, baud_rate);
}

void adi_max22x88_hal_NvicSetVectorSignal(void (*fn)(void))
{
    MXC_NVIC_SetVector(MAX32670_TIMER_IRQn_SIGNAL, fn);
}

void adi_max22x88_hal_NvicEnableSignal(void)
{
    NVIC_EnableIRQ(MAX32670_TIMER_IRQn_SIGNAL);
}
