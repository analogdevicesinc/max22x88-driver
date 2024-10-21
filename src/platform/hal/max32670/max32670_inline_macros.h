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
 * @file max32670_inline_macros.h
 * This file provides `#define` macros for the MAX32670 functionality whose execution time is critical for the bitbang drivers.
 * It provides the same functionality as the MSDK implementation, except parameter error checking.
 * For ease of integration, the macro names match those of the MSDK.
 */

#ifndef MAX32670_INLINE_MACROS_H
#define MAX32670_INLINE_MACROS_H

#include "tmr_revb_regs.h"

#define MXC_TMR_ClearFlags(tmr) do { tmr->intfl |= (MXC_F_TMR_REVB_INTFL_IRQ_A | MXC_F_TMR_REVB_INTFL_IRQ_B); } while (0)
#define MXC_TMR_SetCount(tmr, _cnt) do { tmr->cnt = _cnt; while (!(tmr->intfl & MXC_F_TMR_REVB_INTFL_WRDONE_A)) {} } while (0)
#define MXC_TMR_GetCount(tmr) (tmr->cnt, tmr->cnt)
#define MXC_TMR_Start(tmr) do { tmr->ctrl0 |= MXC_F_TMR_REVB_CTRL0_EN_A; while (!(tmr->ctrl1 & MXC_F_TMR_REVB_CTRL1_CLKEN_A)) {} } while (0)
#define MXC_TMR_Stop(tmr) do { tmr->ctrl0 &= ~MXC_F_TMR_REVB_CTRL0_EN_A; } while (0)
#define MXC_TMR_EnableInt(tmr) do { tmr->ctrl1 |= MXC_F_TMR_REVB_CTRL1_IE_A | MXC_F_TMR_REVB_CTRL1_IE_B; } while (0)
#define MXC_TMR_SetCompare(tmr, _cnt) do { tmr->cmp = _cnt; } while (0)

#define MXC_GPIO_DisableInt(port, mask) do { port->inten_clr = mask; } while (0)
#define MXC_GPIO_EnableInt(port, mask) do { port->inten_set = mask; } while (0)
#define MXC_GPIO_OutSet(port, mask) do { port->out_set = mask; } while (0)
#define MXC_GPIO_OutClr(port, mask) do { port->out_clr = mask; } while (0)
#define MXC_GPIO_InGet(port, mask) (port->in & mask)
#define MXC_GPIO_OutToggle(port, mask) do { port->out ^= mask; } while (0)

#define MXC_GPIO_GetFlags(port) (port->intfl)
#define MXC_GPIO_ClearFlags(port, flags) do { port->intfl_clr = flags; } while (0)

#endif
