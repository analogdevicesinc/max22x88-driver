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
 * @file hal_config.h
 * Configuration file for Max32670 HAL.
 */

#ifndef HAL_CONFIG_H
#define HAL_CONFIG_H

#include "max32670.h"
#include "gpio.h"
#include "tmr.h"

#define MAX32670_GPIO_DOUT_PORT MXC_GPIO0
#define MAX32670_GPIO_DIN_PORT MXC_GPIO0
#define MAX32670_GPIO_RST_PORT MXC_GPIO0

#define MAX32670_GPIO_DOUT_MASK MXC_GPIO_PIN_14
#define MAX32670_GPIO_DIN_MASK MXC_GPIO_PIN_15
#define MAX32670_GPIO_RST_MASK MXC_GPIO_PIN_26

#define MAX32670_TIMER_SIGNAL MXC_TMR0
#define MAX32670_TIMER_CLOCK_SIGNAL MXC_TMR_32M_CLK

#define MAX32670_TIMER_CLOCK_PRESCALE_SIGNAL MXC_TMR_PRES_1
#define MAX32670_TIMER_CLOCK_PRESCALE_VALUE_SIGNAL 1

#define MAX32670_TIMER_IRQn_SIGNAL MXC_TMR_GET_IRQ(MXC_TMR_GET_IDX(MAX32670_TIMER_SIGNAL))

#endif
