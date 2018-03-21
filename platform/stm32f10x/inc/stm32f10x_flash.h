/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _STM32F10X_FLASH_H_
  #define _STM32F10X_FLASH_H_

#include "types.h"


/* flash latency definition */
#define FLASH_LATENCY_ZERO    0x00
#define FLASH_LATENCY_ONE     0x01
#define FLASH_LATENCY_TWO     0x02

#define IS_FLASH_LATENCY_PARAM(param) ((param == FLASH_LATENCY_ZERO) || \
                                       (param == FLASH_LATENCY_ONE) || \
                                       (param == FLASH_LATENCY_TWO))


/* interface */
void Flash_EnablePrefetch(bool flag);
bool FLASH_IsPrefetchEnabled(void);
void FLASH_EnableHalfCycleAccess(bool flag);
bool FLASH_IsHalfCycleAccessEnabled(void);
void FLASH_SetLatency(uint8_t latency);



#endif /* _STM32F10X_FLASH_H_ */


