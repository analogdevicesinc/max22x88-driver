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
 * @file bitbang_hal.h
 * HAL API for the bitbang implementation.
 */

#ifndef BITBANG_HAL_H
#define BITBANG_HAL_H

#include "common_hal.h"
#include <stdint.h>

/**
 * @brief Configures the GPIO connected to DIN as an output.
 * 
 */
void adi_max22x88_hal_GpioConfigureDin(void);

/**
 * @brief Sets the GPIO connected to DIN.
 * 
 */
void adi_max22x88_hal_GpioSetDin(void);

/**
 * @brief Clears the GPIO connected to DIN.
 * 
 */
void adi_max22x88_halGpioClearDin(void);

/**
 * @brief Configures the GPIO connected to DOUT as an input.
 * 
 */
void adi_max22x88_hal_GpioConfigureDout(void);

/**
 * @brief Reads the state of the GPIO connected to DOUT
 * 
 * @retval 0 GPIO is low
 * @retval otherwise GPIO is high
 */
int adi_max22x88_hal_GpioReadDout(void);

/**
 * @brief Configures the interrupt of the GPIO connected to DOUT to be triggered at falling edges.
 * 
 */
void adi_max22x88_hal_GpioIntConfigureDoutFallingEdge(void);

/**
 * @brief Enables the interrupts of the GPIO connected to DOUT.
 * 
 */
void adi_max22x88_hal_GpioIntEnableDout(void);

/**
 * @brief Disables the interrupts of the GPIO connected to DOUT.
 * 
 */
void adi_max22x88_hal_GpioIntDisableDout(void);

/**
 * @brief Initializes the signal timer in continuous mode and sets a specific compare value.
 * 
 * @param cmp compare value
 */
void adi_max22x88_hal_TimerInitSignal(uint32_t cmp);

/**
 * @brief Starts the signal timer.
 * 
 */
void adi_max22x88_hal_TimerStartSignal(void);

/**
 * @brief Stops the signal timer.
 * 
 */
void adi_max22x88_hal_TimerStopSignal(void);

/**
 * @brief Shuts down the signal timer.
 * 
 */
void adi_max22x88_hal_TimerShutdowSignal(void);

/**
 * @brief Sets the count of the signal timer.
 * 
 * @param cnt count value
 */
void adi_max22x88_hal_TimerSetCountSignal(uint32_t cnt);

/**
 * @brief Enables interrupts of the signal timer.
 * 
 */
void adi_max22x88_hal_TimerIntEnableSignal(void);

/**
 * @brief Clears the interrupt flags of the signal timer.
 * 
 */
void adi_max22x88_hal_TimerClearFlagsSignalInterrupt(void);

/**
 * @brief Calculates the number of clock ticks that correspond to a specific baud rate.
 * 
 * @param baud_rate the baud rate in bits per second.
 * @return uint32_t the clock ticks
 */
uint32_t adi_max22x88_hal_TimerCalcPeriodSignal(uint32_t baud_rate);

/**
 * @brief Sets the callback function of the signal timer.
 * 
 * @param fn the callback function
 */
void adi_max22x88_hal_NvicSetVectorSignal(void (*fn)(void));

/**
 * @brief Enables interrupts for the signal timer.
 * 
 */
void adi_max22x88_hal_NvicEnableSignal(void);

#endif
