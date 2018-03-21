/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _CM3_CORE_H
  #define _CM3_CORE_H

#include "types.h"

void __NOP(void);
void __WFI(void);
void __WFE(void);
void __SEV(void);
void __ISB(void);
void __DSB(void);
void __DMB(void);
void __SVC(void);
uint32_t __get_PSR(void);
uint32_t __get_IPSR(void);
uint32_t __get_CONTROL(void);
void __set_CONTROL(uint32_t val);
uint32_t __get_PSP(void);
void __set_PSP(uint32_t val);
uint32_t __get_MSP(void);
void __set_MSP(uint32_t val);
void __set_PRIMASK();
uint32_t __get_PRIMASK(void);
void __reset_PRIMASK(void);
void __set_FAULTMASK(void);
uint32_t __get_FAULTMASK(void);
void __reset_FAULTMASK(void);
void __set_BASEPRI(uint32_t val);
uint32_t __get_BASEPRI(void);
uint32_t __REV(uint32_t value);
uint32_t __REV16(uint16_t value);
int32_t __REVSH(int16_t value);
uint32_t __RBIT(uint32_t value);
uint32_t __CLZ(uint32_t value);


#endif

