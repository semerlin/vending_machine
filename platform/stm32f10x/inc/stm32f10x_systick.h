/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _STM32F10X_SYSTICK_H_
  #define _STM32F10X_SYSTICK_H_

#include "types.h"

/* systick clock resource */
#define SYSTICK_CLOCK_AHB_DIV_EIGHT    (0x00)
#define SYSTICK_CLOCK_AHB              (1 << 2)
#define IS_SYSTICK_CLOCK_PARAM(CLOCK) \
                       ((CLOCK == SYSTICK_CLOCK_AHB_DIV_EIGHT) || \
                        (CLOCK == SYSTICK_CLOCK_AHB))

/* interface */
void SYSTICK_SetClockSource(uint8_t source);
void SYSTICK_EnableInt(bool flag);
void SYSTICK_EnableCounter(bool flag);
bool SYSTICK_IsCountFlagSet(void);
void SYSTICK_ClrCountFlag(void);
void SYSTICK_SetTickInterval(uint32_t time);


#endif /* _STM32F10X_SYSTICK_H_ */

