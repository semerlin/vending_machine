/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _STM32F10X_RCC_H_
  #define _STM32F10X_RCC_H_

#include "types.h"



/************************************************************/
#define RCC_MCO_NONE    (0x00)
#define RCC_MCO_SYSCLK  (0x04 << 24)
#define RCC_MCO_HSI     (0x05 << 24)
#define RCC_MCO_HSE     (0x06 << 24)
#define RCC_MCO_PLL     (0x07 << 24)

#define IS_RCC_MCO_PARAM(param)  ((param == RCC_MCO_NONE) || (param == RCC_MCO_SYSCLK) || \
                                  (param == RCC_MCO_HSI) || (param == RCC_MCO_HSE) || \
                                  (param == RCC_MCO_PLL))



/**************************************************************/
#define RCC_USBPRE_1POINT5    (0x00)
#define RCC_USBPRE_DIRECT     (0x01)

#define IS_RCC_USBPRE_PARAM(param)  ((param == RCC_USBPRE_1POINT5) || (param == RCC_USBPRE_DIRECT))


/******************************************************/
#define RCC_SW_HSI      (0x00)
#define RCC_SW_HSE      (0x01)
#define RCC_SW_PLL      (0x02)
#define RCC_SW_INVALID  (0x03)

#define IS_RCC_SW_PARAM(param)  ((param == RCC_SW_HSI) || (param == RCC_SW_HSE) || \
                                  (param == RCC_SW_PLL) || (param == RCC_SW_INVALID))

/******************************************************/
#define RCC_HPRE_SYSCLK        (0x00)
#define RCC_HPRE_SYSCLK_DIV2       (0x08 << 4)
#define RCC_HPRE_SYSCLK_DIV4       (0x09 << 4)
#define RCC_HPRE_SYSCLK_DIV8       (0x0a << 4)
#define RCC_HPRE_SYSCLK_DIV16      (0x0b << 4)
#define RCC_HPRE_SYSCLK_DIV64      (0x0c << 4)
#define RCC_HPRE_SYSCLK_DIV128     (0x0d << 4)
#define RCC_HPRE_SYSCLK_DIV256     (0x0e << 4)
#define RCC_HPRE_SYSCLK_DIV512     (0x0f << 4)

#define IS_RCC_HPRE_PARAM(param) ((param == RCC_HPRE_SYSCLK) || (param == RCC_HPRE_SYSCLK_DIV2) || \
                                  (param == RCC_HPRE_SYSCLK_DIV4) || (param == RCC_HPRE_SYSCLK_DIV8) || \
                                  (param == RCC_HPRE_SYSCLK_DIV16) || (param == RCC_HPRE_SYSCLK_DIV64) || \
                                  (param == RCC_HPRE_SYSCLK_DIV128) || (param == RCC_HPRE_SYSCLK_DIV256) || \
                                  (param == RCC_HPRE_SYSCLK_DIV512))
                                  

/******************************************************/
#define RCC_PPRE1_HCLK        (0x00)
#define RCC_PPRE1_HCLK_DIV2       (0x04 << 8)
#define RCC_PPRE1_HCLK_DIV4       (0x05 << 8)
#define RCC_PPRE1_HCLK_DIV8       (0x06 << 8)
#define RCC_PPRE1_HCLK_DIV16      (0x07 << 8)


#define IS_RCC_PPRE1_PARAM(param) ((param == RCC_PPRE1_HCLK) || (param == RCC_PPRE1_HCLK_DIV2) || \
                                  (param == RCC_PPRE1_HCLK_DIV4) || (param == RCC_PPRE1_HCLK_DIV8) || \
                                  (param == RCC_PPRE1_HCLK_DIV16))


/******************************************************/
#define RCC_PPRE2_HCLK        (0x00)
#define RCC_PPRE2_HCLK_DIV2       (0x04 << 11)
#define RCC_PPRE2_HCLK_DIV4       (0x05 << 11)
#define RCC_PPRE2_HCLK_DIV8       (0x06 << 11)
#define RCC_PPRE2_HCLK_DIV16      (0x07 << 11)


#define IS_RCC_PPRE2_PARAM(param) ((param == RCC_PPRE2_HCLK) || (param == RCC_PPRE2_HCLK_DIV2) || \
                                  (param == RCC_PPRE2_HCLK_DIV4) || (param == RCC_PPRE2_HCLK_DIV8) || \
                                  (param == RCC_PPRE2_HCLK_DIV16))



/******************************************************/
#define RCC_ADC_PCLK_DIV2    (0x00)
#define RCC_ADC_PCLK_DIV4    (0x01 << 14)
#define RCC_ADC_PCLK_DIV6    (0x02 << 14)
#define RCC_ADC_PCLK_DIV8    (0x03 << 14)

#define IS_RCC_ADC_PARAM(param) ((param == RCC_ADC_PCLK_DIV2) || (param == RCC_ADC_PCLK_DIV4) || \
                                  (param == RCC_ADC_PCLK_DIV6) || (param == RCC_ADC_PCLK_DIV8))



/******************************************************/
#define RCC_INT_ClockSecuty      0x01
#define RCC_INT_PLLReady         0x02
#define RCC_INT_HSEReady         0x04
#define RCC_INT_HSIReady         0x08
#define RCC_INT_LSEReady         0x10
#define RCC_INT_LSIReady         0x20

/******************************************************/
#define RCC_APB2_RESET_TIM11        (1 << 21)
#define RCC_APB2_RESET_TIM10        (1 << 20)
#define RCC_APB2_RESET_TIM19        (1 << 19)
#define RCC_APB2_RESET_ADC3         (1 << 15)
#define RCC_APB2_RESET_USART1       (1 << 14)
#define RCC_APB2_RESET_TIM8         (1 << 13)
#define RCC_APB2_RESET_SPI1         (1 << 12)
#define RCC_APB2_RESET_TIM1         (1 << 11)
#define RCC_APB2_RESET_ADC2         (1 << 10)
#define RCC_APB2_RESET_ADC1         (1 << 9)
#define RCC_APB2_RESET_IOPG         (1 << 8)
#define RCC_APB2_RESET_IOPF         (1 << 7)
#define RCC_APB2_RESET_IOPE         (1 << 6)
#define RCC_APB2_RESET_IOPD         (1 << 5)
#define RCC_APB2_RESET_IOPC         (1 << 4)
#define RCC_APB2_RESET_IOPB         (1 << 3)
#define RCC_APB2_RESET_IOPA         (1 << 2)
#define RCC_APB2_RESET_AFIO         (1 << 0)

#define RCC_APB2_RESET_ALL          0x0031fffd



/******************************************************/
#define RCC_APB1_RESET_DAC              (1 << 29)
#define RCC_APB1_RESET_PWR              (1 << 28)
#define RCC_APB1_RESET_BKP              (1 << 27)
#define RCC_APB1_RESET_CAN              (1 << 25)
#define RCC_APB1_RESET_USB              (1 << 23)
#define RCC_APB1_RESET_I2C2             (1 << 22)
#define RCC_APB1_RESET_I2C1             (1 << 21)
#define RCC_APB1_RESET_UART5            (1 << 20)
#define RCC_APB1_RESET_UART4            (1 << 19)
#define RCC_APB1_RESET_USART3           (1 << 18)
#define RCC_APB1_RESET_USART2           (1 << 17)
#define RCC_APB1_RESET_SPI3             (1 << 15)
#define RCC_APB1_RESET_SPI2             (1 << 14)
#define RCC_APB1_RESET_WWDG             (1 << 11)
#define RCC_APB1_RESET_TIM14            (1 << 8)
#define RCC_APB1_RESET_TIM13            (1 << 7)
#define RCC_APB1_RESET_TIM12            (1 << 6)
#define RCC_APB1_RESET_TIM7             (1 << 5)
#define RCC_APB1_RESET_TIM6             (1 << 4)
#define RCC_APB1_RESET_TIM5             (1 << 3)
#define RCC_APB1_RESET_TIM4             (1 << 2)
#define RCC_APB1_RESET_TIM3             (1 << 1)
#define RCC_APB1_RESET_TIM2             (1 << 0)

#define RCC_APB1_RESET_ALL              0x3afecaff



/******************************************************/
#define RCC_AHB_ENABLE_DMA1           0x0001
#define RCC_AHB_ENABLE_DMA2           0x0002
#define RCC_AHB_ENABLE_SRAM           0x0004
#define RCC_AHB_ENABLE_FLITF          0x0010
#define RCC_AHB_ENABLE_CRC            0x0040
#define RCC_AHB_ENABLE_FSMC           0x0100
#define RCC_AHB_ENABLE_SDIO           0x0400

#define RCC_AHB_ENABLE_ALL            0x0557


/******************************************************/
#define RCC_APB2_ENABLE_AFIO         0x0001
#define RCC_APB2_ENABLE_IOPA         0x0004
#define RCC_APB2_ENABLE_IOPB         0x0008
#define RCC_APB2_ENABLE_IOPC         0x0010
#define RCC_APB2_ENABLE_IOPD         0x0020
#define RCC_APB2_ENABLE_IOPE         0x0040
#define RCC_APB2_ENABLE_IOPF         0x0080
#define RCC_APB2_ENABLE_IOPG         0x0100
#define RCC_APB2_ENABLE_ADC1         0x0200
#define RCC_APB2_ENABLE_ADC2         0x0400
#define RCC_APB2_ENABLE_TIM1         0x0800
#define RCC_APB2_ENABLE_SPI1         0x1000
#define RCC_APB2_ENABLE_TIM8         0x2000
#define RCC_APB2_ENABLE_USART1       0x4000
#define RCC_APB2_ENABLE_ADC3         0x8000

#define RCC_APB2_ENABLE_ALL          0xfffd



/******************************************************/
#define RCC_APB1_ENABLE_TIM2        0x00000001
#define RCC_APB1_ENABLE_TIM3        0x00000002
#define RCC_APB1_ENABLE_TIM4        0x00000004
#define RCC_APB1_ENABLE_TIM5        0x00000008
#define RCC_APB1_ENABLE_TIM6        0x00000010
#define RCC_APB1_ENABLE_TIM7        0x00000020
#define RCC_APB1_ENABLE_WWDG        0x00000800
#define RCC_APB1_ENABLE_SPI2        0x00004000
#define RCC_APB1_ENABLE_SPI3        0x00008000
#define RCC_APB1_ENABLE_USART2      0x00020000
#define RCC_APB1_ENABLE_USART3      0x00040000
#define RCC_APB1_ENABLE_UART4       0x00080000
#define RCC_APB1_ENABLE_UART5       0x00100000
#define RCC_APB1_ENABLE_I2C1        0x00200000
#define RCC_APB1_ENABLE_I2C2        0x00400000
#define RCC_APB1_ENABLE_USB         0x00800000
#define RCC_APB1_ENABLE_CAN         0x02000000
#define RCC_APB1_ENABLE_BKP         0x08000000
#define RCC_APB1_ENABLE_PWR         0x10000000
#define RCC_APB1_ENABLE_DAC         0x20000000

#define RCC_APB1_ENABLE_ALL         0x3afec83f

/******************************************************/
#define RTC_CLOCK_NONE    (0x00)
#define RTC_CLOCK_LSE     (1 << 8)
#define RTC_CLOCK_LSI     (2 << 8)
#define RTC_CLOCK_HSE     (3 << 8)

#define IS_RTC_CLOCK_PARAM(param) ((param == RTC_CLOCK_NONE) || \
                                   (param == RTC_CLOCK_LSE) || \
                                   (param == RTC_CLOCK_LSI) || \
                                   (param == RTC_CLOCK_HSE))





/* interface */
void RCC_DeInit(void);
bool RCC_StartupHSI(void);
void RCC_StopHSI(void);
uint8_t RCC_GetHSICalValue(void);
void RCC_SetHSITrimValue(uint8_t value);
uint8_t RCC_GetHSITrimValue(void);
bool RCC_IsHSIOn(void);
bool RCC_StartupHSE(void);
void RCC_StopHSE(void);
bool RCC_BypassHSE(bool flag);
bool RCC_IsHSEOn(void);
bool RCC_IsHSEBypassed(void);
void RCC_EnableClockSecurityConfig(bool flag);
bool RCC_StartupPLL(void);
void RCC_StopPLL(void);
bool RCC_GetPLLONFlag(void);
void RCC_MCOConfig(uint32_t method);
void RCC_USBPrescalerFromPLL(uint8_t config);
uint32_t RCC_SetSysclkUsePLL(uint32_t clock, bool useHSE, 
                             uint32_t hseClock);
void RCC_HCLKPrescalerFromSYSCLK(uint8_t config);
void RCC_PCLK1PrescalerHCLK(uint32_t config);
void RCC_PCLK2PrescalerFromHCLK(uint32_t config);
uint32_t RCC_GetSysclk(void);
uint32_t RCC_GetHCLK(void);
uint32_t RCC_GetPCLK1(void);
uint32_t RCC_GetPCLK2(void);
void RCC_SystemClockSwitch(uint8_t clock);
uint8_t RCC_GetSystemClock(void);
void RCC_ADCPrescalerFromPCLK2(uint32_t config);
void RCC_ClrClockIntFlag(uint8_t intSrc);
void RCC_EnableClockInt(uint8_t intSrc, bool flag);
uint8_t RCC_GetClockIntFlag(uint8_t intSrc);
void RCC_APB2PeriphReset(uint32_t reg, bool flag);
void RCC_APB1PeriphReset(uint32_t reg, bool flag);
void RCC_AHBPeripClockEnable(uint32_t reg, bool flag);
void RCC_APB2PeripClockEnable(uint16_t reg, bool flag);
void RCC_APB1PeripClockEnable(uint32_t reg, bool flag);
void RCC_BackUpRegisterReset(bool flag);
bool RCC_IsRTCEnabled(void);
void RCC_SetRTCClockSource(uint32_t source);
uint8_t RCC_GetRTCClockSource(void);
bool RCC_StartupLSE(void);
bool RCC_BypassLSE(bool flag);
void RCC_StopLSE(void);
bool RCC_StartupLSI(void);
void RCC_CloseLSI(void);
uint8_t RCC_GetResetFlag(void);
void RCC_ClrResetFlag(void);
    

#endif /* _STM32F10X_RCC_H_ */


