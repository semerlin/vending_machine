/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _STM32F10X_GPIO_H_
  #define _STM32F10X_GPIO_H_

#include "types.h"

/* gpio group definition */
typedef enum
{
    GPIOA,
    GPIOB,
    GPIOC,
    GPIOD,
    GPIOE,
    GPIOF,
    GPIOG,
    GPIO_Count
}GPIO_Group;

/* gpio configure definition */
typedef enum
{ 
    GPIO_Speed_10MHz = 1,
    GPIO_Speed_2MHz, 
    GPIO_Speed_50MHz
}GPIO_Speed;

typedef enum
{ 
    GPIO_Mode_AIN = 0x0,
    GPIO_Mode_IN_FLOATING = 0x01,
    GPIO_Mode_IPD = 0x02,
    GPIO_Mode_IPU = 0x22,
    GPIO_Mode_Out_OD = 0x11,
    GPIO_Mode_Out_PP = 0x10,
    GPIO_Mode_AF_OD = 0x13,
    GPIO_Mode_AF_PP = 0x12
}GPIO_Mode;

     
typedef struct
{
    uint8_t pin;
    GPIO_Speed speed;
    GPIO_Mode mode;  
}GPIO_Config;


#define SWJ_JTAG_DISABLE     (2 << 24)

/* interface */
void GPIO_Setup(GPIO_Group group, const GPIO_Config *config);
uint16_t GPIO_ReadDataGroup(GPIO_Group group);
void GPIO_WriteDataGroup(GPIO_Group group, uint16_t data);
uint8_t GPIO_ReadPin(GPIO_Group group, uint8_t pin);
void GPIO_SetPin(GPIO_Group group, uint8_t pin);
void GPIO_ResetPin(GPIO_Group group, uint8_t pin);
void GPIO_LockPin(GPIO_Group group, uint8_t pin);
void GPIO_EXTIConfig(GPIO_Group group, uint8_t pin);
void GPIO_PinRemap(uint32_t pin, bool flag);


#endif /* _STM32F10X_GPIO_H_ */

