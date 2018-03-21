/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _STM32F10X_NVIC_H_
  #define _STM32F10X_NVIC_H_

#include "types.h"

/* NVIC Init Structure definition */
typedef struct
{
    uint8_t channel;
    uint8_t preemptionPriority;
    uint8_t subPriority;
    bool enable;
} NVIC_Config;

/* IRQ Channels */
#define WWDG_IRQChannel              (0x00)  /* Window WatchDog Interrupt */
#define PVD_IRQChannel               (0x01)  /* PVD through EXTI Line detection Interrupt */
#define TAMPER_IRQChannel            (0x02)  /* Tamper Interrupt */
#define RTC_IRQChannel               (0x03)  /* RTC global Interrupt */
#define FLASH_IRQChannel             (0x04)  /* FLASH global Interrupt */
#define RCC_IRQChannel               (0x05)  /* RCC global Interrupt */
#define EXTI0_IRQChannel             (0x06)  /* EXTI Line0 Interrupt */
#define EXTI1_IRQChannel             (0x07)  /* EXTI Line1 Interrupt */
#define EXTI2_IRQChannel             (0x08)  /* EXTI Line2 Interrupt */
#define EXTI3_IRQChannel             (0x09)  /* EXTI Line3 Interrupt */
#define EXTI4_IRQChannel             (0x0A)  /* EXTI Line4 Interrupt */
#define DMAChannel1_IRQChannel       (0x0B)  /* DMA Channel 1 global Interrupt */
#define DMAChannel2_IRQChannel       (0x0C)  /* DMA Channel 2 global Interrupt */
#define DMAChannel3_IRQChannel       (0x0D)  /* DMA Channel 3 global Interrupt */
#define DMAChannel4_IRQChannel       (0x0E)  /* DMA Channel 4 global Interrupt */
#define DMAChannel5_IRQChannel       (0x0F)  /* DMA Channel 5 global Interrupt */
#define DMAChannel6_IRQChannel       (0x10)  /* DMA Channel 6 global Interrupt */
#define DMAChannel7_IRQChannel       (0x11)  /* DMA Channel 7 global Interrupt */
#define ADC_IRQChannel               (0x12)  /* ADC global Interrupt */
#define USB_HP_CAN_TX_IRQChannel     (0x13)  /* USB High Priority or CAN TX Interrupts */
#define USB_LP_CAN_RX0_IRQChannel    (0x14)  /* USB Low Priority or CAN RX0 Interrupts */
#define CAN_RX1_IRQChannel           (0x15)  /* CAN RX1 Interrupt */
#define CAN_SCE_IRQChannel           (0x16)  /* CAN SCE Interrupt */
#define EXTI9_5_IRQChannel           (0x17)  /* External Line[9:5] Interrupts */
#define TIM1_BRK_IRQChannel          (0x18)  /* TIM1 Break Interrupt */
#define TIM1_UP_IRQChannel           (0x19)  /* TIM1 Update Interrupt */
#define TIM1_TRG_COM_IRQChannel      (0x1A)  /* TIM1 Trigger and Commutation Interrupt */
#define TIM1_CC_IRQChannel           (0x1B)  /* TIM1 Capture Compare Interrupt */
#define TIM2_IRQChannel              (0x1C)  /* TIM2 global Interrupt */
#define TIM3_IRQChannel              (0x1D)  /* TIM3 global Interrupt */
#define TIM4_IRQChannel              (0x1E)  /* TIM4 global Interrupt */
#define I2C1_EV_IRQChannel           (0x1F)  /* I2C1 Event Interrupt */
#define I2C1_ER_IRQChannel           (0x20)  /* I2C1 Error Interrupt */
#define I2C2_EV_IRQChannel           (0x21)  /* I2C2 Event Interrupt */
#define I2C2_ER_IRQChannel           (0x22)  /* I2C2 Error Interrupt */
#define SPI1_IRQChannel              (0x23)  /* SPI1 global Interrupt */
#define SPI2_IRQChannel              (0x24)  /* SPI2 global Interrupt */
#define USART1_IRQChannel            (0x25)  /* USART1 global Interrupt */
#define USART2_IRQChannel            (0x26)  /* USART2 global Interrupt */
#define USART3_IRQChannel            (0x27)  /* USART3 global Interrupt */
#define EXTI15_10_IRQChannel         (0x28)  /* External Line[15:10] Interrupts */
#define RTCAlarm_IRQChannel          (0x29)  /* RTC Alarm through EXTI Line Interrupt */
#define USBWakeUp_IRQChannel         (0x2A)  /* USB WakeUp from suspend through EXTI Line Interrupt */

#define IS_NVIC_IRQ_CHANNEL(CHANNEL) ((CHANNEL == WWDG_IRQChannel) || \
                                      (CHANNEL == PVD_IRQChannel) || \
                                      (CHANNEL == TAMPER_IRQChannel) || \
                                      (CHANNEL == RTC_IRQChannel) || \
                                      (CHANNEL == FLASH_IRQChannel) || \
                                      (CHANNEL == RCC_IRQChannel) || \
                                      (CHANNEL == EXTI0_IRQChannel) || \
                                      (CHANNEL == EXTI1_IRQChannel) || \
                                      (CHANNEL == EXTI2_IRQChannel) || \
                                      (CHANNEL == EXTI3_IRQChannel) || \
                                      (CHANNEL == EXTI4_IRQChannel) || \
                                      (CHANNEL == DMAChannel1_IRQChannel) || \
                                      (CHANNEL == DMAChannel2_IRQChannel) || \
                                      (CHANNEL == DMAChannel3_IRQChannel) || \
                                      (CHANNEL == DMAChannel4_IRQChannel) || \
                                      (CHANNEL == DMAChannel5_IRQChannel) || \
                                      (CHANNEL == DMAChannel6_IRQChannel) || \
                                      (CHANNEL == DMAChannel7_IRQChannel) || \
                                      (CHANNEL == ADC_IRQChannel) || \
                                      (CHANNEL == USB_HP_CAN_TX_IRQChannel) || \
                                      (CHANNEL == USB_LP_CAN_RX0_IRQChannel) || \
                                      (CHANNEL == CAN_RX1_IRQChannel) || \
                                      (CHANNEL == CAN_SCE_IRQChannel) || \
                                      (CHANNEL == EXTI9_5_IRQChannel) || \
                                      (CHANNEL == TIM1_BRK_IRQChannel) || \
                                      (CHANNEL == TIM1_UP_IRQChannel) || \
                                      (CHANNEL == TIM1_TRG_COM_IRQChannel) || \
                                      (CHANNEL == TIM1_CC_IRQChannel) || \
                                      (CHANNEL == TIM2_IRQChannel) || \
                                      (CHANNEL == TIM3_IRQChannel) || \
                                      (CHANNEL == TIM4_IRQChannel) || \
                                      (CHANNEL == I2C1_EV_IRQChannel) || \
                                      (CHANNEL == I2C1_ER_IRQChannel) || \
                                      (CHANNEL == I2C2_EV_IRQChannel) || \
                                      (CHANNEL == I2C2_ER_IRQChannel) || \
                                      (CHANNEL == SPI1_IRQChannel) || \
                                      (CHANNEL == SPI2_IRQChannel) || \
                                      (CHANNEL == USART1_IRQChannel) || \
                                      (CHANNEL == USART2_IRQChannel) || \
                                      (CHANNEL == USART3_IRQChannel) || \
                                      (CHANNEL == EXTI15_10_IRQChannel) || \
                                      (CHANNEL == RTCAlarm_IRQChannel) || \
                                      (CHANNEL == USBWakeUp_IRQChannel))



void NVIC_Init(const NVIC_Config *config);
void NVIC_InitStruct(NVIC_Config *config);
void NVIC_EnableIRQ(uint8_t channel, bool flag);
void NVIC_SetIRQPending(uint8_t channel, bool flag);
bool NVIC_IsIRQPending(uint8_t channel);
bool NVIC_IsIRQActive(uint8_t channel);
void NVIC_SetIRQPriority(uint8_t channel, uint8_t preemptionPriority,
                         uint8_t subPriority);
void NVIC_GetIRQPriority(uint8_t channel, uint8_t *preemptionPriority,
                         uint8_t *subPriority);

#endif /* _STM32F10X_NVIC_H_ */

