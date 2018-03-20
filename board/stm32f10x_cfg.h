#ifndef _STM32F10x_CFG_H
#define _STM32F10x_CFG_H

#include "environment.h"



/**********************************************************
* library module inclue configure
***********************************************************/
/*********************************************************/
#define _MODULE_RCC
#define _MODULE_FLASH
#define _MODULE_GPIO
#define _MODULE_USART
#define _MODULE_SCB
#define _MODULE_NVIC
#define _MODULE_SYSTICK
#define _MODULE_ADC
#define _MODULE_SPI
#define _MODULE_I2C

/**********************************************************/
#ifdef _MODULE_CRC
  #include "stm32f10x_crc.h" 
#endif

#ifdef _MODULE_FLASH
  #include "stm32f10x_flash.h" 
#endif

#ifdef _MODULE_RCC
  #include "stm32f10x_rcc.h" 
#endif

#ifdef _MODULE_GPIO
  #include "stm32f10x_gpio.h" 
#endif

#ifdef _MODULE_USART
  #include "stm32f10x_usart.h" 
#endif

#ifdef _MODULE_SCB
  #include "stm32f10x_scb.h" 
#endif

#ifdef _MODULE_NVIC
  #include "stm32f10x_nvic.h" 
#endif

#ifdef _MODULE_SYSTICK
  #include "stm32f10x_systick.h"
#endif

#ifdef _MODULE_ADC
  #include "stm32f10x_adc.h"
#endif

#ifdef _MODULE_SPI
  #include "stm32f10x_spi.h"
#endif

#ifdef _MODULE_I2C
  #include "stm32f10x_i2c.h"
#endif

#endif

