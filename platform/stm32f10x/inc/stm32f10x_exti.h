/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _STM32F10X_EXTI_H_
  #define _STM32F10X_EXTI_H_

#include "types.h"

typedef enum 
{
    Trigger_None = 0,
    Trigger_Rising = 0x01,
    Trigger_Falling = 0x02,
}Trigger_Edge;


void EXTI_EnableLine_INT(uint8_t line, bool flag);
void EXTI_EnableLine_EVENT(uint8_t line, bool flag);
void EXTI_SetTrigger(uint8_t line, Trigger_Edge edge);
void EXTI_ClrTrigger(uint8_t line, Trigger_Edge edge);
void EXTI_SetSoftInt(uint8_t line);
bool EXTI_IsPending(uint8_t line);
void EXTI_ClrPending(uint8_t line);


#endif /* _STM32F10X_EXTI_H_ */



