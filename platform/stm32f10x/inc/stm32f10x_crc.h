/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _STM32F10X_CRC_H_
  #define _STM32F10X_CRC_H_

#include "types.h"

/* interface */
void CRC_ResetDR(void);
uint32_t CRC_Cal(uint32_t data);
uint32_t CRC_CalBlock(uint32_t *buf, uint32_t len);
uint32_t CRC_GetDR(void);
void CRC_SetIDR(uint8_t data);
uint8_t CRC_GetIDR(void);

#endif /* _STM32F10X_CRC_H */

