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

/* flash flags */
#define FLAH_FLAG_BSY         (1 << 0)
#define FLAH_FLAG_PGERR       (1 << 2)
#define FLAH_FLAG_WRPRTERR    (1 << 4)
#define FLAH_FLAG_EOP         (1 << 5)

#define IS_FLASH_FLAG_PARAM(param) ((param == FLAH_FLAG_BSY) || \
                                    (param == FLAH_FLAG_PGERR) || \
                                    (param == FLAH_FLAG_WRPRTERR) || \
                                    (param == FLAH_FLAG_EOP))




/* interface */
void FLASH_EnablePrefetch(bool flag);
bool FLASH_IsPrefetchEnabled(void);
void FLASH_EnableHalfCycleAccess(bool flag);
bool FLASH_IsHalfCycleAccessEnabled(void);
void FLASH_SetLatency(uint8_t latency);
bool FLASH_Is_FlagSet(uint8_t flag);
void FLASH_ClrFlag(uint8_t flag);
void FLASH_ErasePage(uint32_t addr);
uint32_t FLASH_Write(uint32_t addr, uint8_t *data, uint32_t len);
uint32_t FLASH_Read(uint32_t addr, uint8_t *data, uint32_t len);

#endif /* _STM32F10X_FLASH_H_ */


