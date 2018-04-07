/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _STM32F10X_USART_H_
  #define _STM32F10X_USART_H_

#include "types.h"

/* USART Word Length */
#define USART_WordLength_8                  (0x00)
#define USART_WordLength_9                  (1 << 12)

#define IS_USART_WORD_LENGTH(LENGTH) ((LENGTH == USART_WordLength_8) || \
                                      (LENGTH == USART_WordLength_9))

/* USART Stop Bits */
#define USART_StopBits_1                     (0x00)
#define USART_StopBits_0_5                   (1 << 12)
#define USART_StopBits_2                     (2 << 12)
#define USART_StopBits_1_5                   (3 << 12)

#define IS_USART_STOPBITS(STOPBITS) ((STOPBITS == USART_StopBits_1) || \
                                     (STOPBITS == USART_StopBits_0_5) || \
                                     (STOPBITS == USART_StopBits_2) || \
                                     (STOPBITS == USART_StopBits_1_5))

/* USART Parity */
#define USART_Parity_None                    (0x0000)
#define USART_Parity_Even                    (0x02 << 9)
#define USART_Parity_Odd                     (0x03 << 9) 

#define IS_USART_PARITY(PARITY) ((PARITY == USART_Parity_None) || \
                                 (PARITY == USART_Parity_Even) || \
                                 (PARITY == USART_Parity_Odd))

/* USART Hardware Flow Control */
#define USART_HardwareFlowControl_None       (0x00)
#define USART_HardwareFlowControl_RTS        (1 << 8)
#define USART_HardwareFlowControl_CTS        (1 << 9)
#define USART_HardwareFlowControl_RTS_CTS    (0x03 << 8)

#define IS_USART_HARDWARE_FLOW_CONTROL(CONTROL)\
                              ((CONTROL == USART_HardwareFlowControl_None) || \
                               (CONTROL == USART_HardwareFlowControl_RTS) || \
                               (CONTROL == USART_HardwareFlowControl_CTS) || \
                               (CONTROL == USART_HardwareFlowControl_RTS_CTS))

/* USART Clock Polarity */
#define USART_CPOL_Low                    (0x00)
#define USART_CPOL_High                   (1 << 10)

#define IS_USART_CPOL(CPOL) ((CPOL == USART_CPOL_Low) || \
                             (CPOL == USART_CPOL_High))
                               
/* USART Clock Phase */
#define USART_CPHA_1Edge                  (0x00)
#define USART_CPHA_2Edge                  (1 << 9)
#define IS_USART_CPHA(CPHA) ((CPHA == USART_CPHA_1Edge) || \
                             (CPHA == USART_CPHA_2Edge))



/* USART Flags */
#define USART_FLAG_CTS                       (1 << 9)
#define USART_FLAG_LBD                       (1 << 8)
#define USART_FLAG_TXE                       (1 << 7)
#define USART_FLAG_TC                        (1 << 6)
#define USART_FLAG_RXNE                      (1 << 5)
#define USART_FLAG_IDLE                      (1 << 4)
#define USART_FLAG_ORE                       (1 << 3)
#define USART_FLAG_NE                        (1 << 2)
#define USART_FLAG_FE                        (1 << 1)
#define USART_FLAG_PE                        (1 << 0)

#define IS_USART_FLAG(FLAG) ((FLAG == USART_FLAG_PE) || \
                             (FLAG == USART_FLAG_TXE) || \
                             (FLAG == USART_FLAG_TC) || \
                             (FLAG == USART_FLAG_RXNE) || \
                             (FLAG == USART_FLAG_IDLE) || \
                             (FLAG == USART_FLAG_LBD) || \
                             (FLAG == USART_FLAG_CTS) || \
                             (FLAG == USART_FLAG_ORE) || \
                             (FLAG == USART_FLAG_NE) || \
                             (FLAG == USART_FLAG_FE))

/* USART Interrupt definition */
#define USART_IT_PE                          ((uint16_t)0x0001)
#define USART_IT_TXE                         ((uint16_t)0x0071)
#define USART_IT_TC                          ((uint16_t)0x0061)
#define USART_IT_RXNE                        ((uint16_t)0x0051)
#define USART_IT_IDLE                        ((uint16_t)0x0041)
#define USART_IT_LBD                         ((uint16_t)0x0062)
#define USART_IT_CTS                         ((uint16_t)0x0093)
#define USART_IT_ERR                         ((uint16_t)0x0003)


#define IS_USART_IT(IT) ((IT == USART_IT_PE) || (IT == USART_IT_TXE) || \
                         (IT == USART_IT_TC) || (IT == USART_IT_RXNE) || \
                         (IT == USART_IT_IDLE) || (IT == USART_IT_LBD) || \
                         (IT == USART_IT_CTS) || (IT == USART_IT_ERR))


/* USART WakeUp methods */
#define USART_WakeUp_IdleLine                (0x00)
#define USART_WakeUp_AddressMark             (1 << 11)

#define IS_USART_WAKEUP(WAKEUP) ((WAKEUP == USART_WakeUp_IdleLine) || \
                                 (WAKEUP == USART_WakeUp_AddressMark))

/* USART receive mode */
#define USART_RecvMode_Active                (0x00)
#define USART_RecvMode_Mute                  (1 << 1)

#define IS_USART_RECVMODE(RECV) ((RECV == USART_RecvMode_Active) || \
                                 (RECV == USART_RecvMode_Mute))



/* USART LIN Break Detection Length */
#define USART_LINBreakDetectLength_10      (0x0000)
#define USART_LINBreakDetectLength_11      (1 << 1)

#define IS_USART_LIN_BREAK_DETECT_LENGTH(LENGTH) \
                               ((LENGTH == USART_LINBreakDetectLength_10) || \
                                (LENGTH == USART_LINBreakDetectLength_11))

/* USART IrDA Low Power */
#define USART_IrDAMode_LowPower              (1 << 2)
#define USART_IrDAMode_Normal                (0x00)

#define IS_USART_IRDA_MODE(MODE) ((MODE == USART_IrDAMode_LowPower) || \
                                  (MODE == USART_IrDAMode_Normal))


/* usart group definition */
typedef enum
{
    USART1,
    USART2,
    USART3,
    UASRT_Count,
}USART_Group;


/*usart configuration */
typedef struct
{
    uint32_t baudRate;
    uint16_t wordLength;
    uint16_t parity;
    uint16_t stopBits;
    uint16_t hardwareFlowControl;
    bool txEnable;
    bool rxEnable;
    bool clkEnable;
    uint16_t clkPolarity;
    uint16_t clkPhase;
    bool lastBitClkEnable;
}USART_Config;



/* interface */
void USART_Enable(USART_Group group, bool flag);
void USART_TransEnable(USART_Group group, bool flag);
void USART_RecvEnable(USART_Group group, bool flag);
void USART_Setup(USART_Group group, const USART_Config *config);
void USART_StructInit(USART_Config *config);
void USART_SetAddress(USART_Group group, uint8_t address);
uint8_t USART_GetAddress(USART_Group group);
void USART_SetPrescaler(USART_Group group, uint8_t prescaler);
bool USART_IsFlagOn(USART_Group group, uint16_t intFlag);
void USART_ClearFlag(USART_Group group, uint16_t intFlag);
void USART_WriteData_Wait(USART_Group group, uint8_t data);
void USART_WriteData(USART_Group group, uint8_t data);
uint8_t USART_ReadData(USART_Group group);
void USART_SetWakeupMethod(USART_Group group, uint16_t method);
void USART_EnableInt(USART_Group group, uint8_t intFlag, 
                     bool flag);
bool USART_IsIntEnabled(USART_Group group, uint8_t intFlag);
void USART_SetRecvMode(USART_Group group, uint8_t mode);
void USART_SendBreakCharacter(USART_Group group);
void USART_EnableLINMode(USART_Group group, bool flag);
void USART_SetLineBreakDetectLength(USART_Group group, uint8_t length);
void USART_EnableDMATX(USART_Group group, bool flag);
void USART_EnableDMARX(USART_Group group, bool flag);
void USART_SmartcardEnable(USART_Group group, bool flag);
void USART_SmartcardNACKEnable(USART_Group group, bool flag);
void USART_HalfDuplexSelection(USART_Group group, bool flag);
void USART_IrdaEnable(USART_Group group, bool flag);
void USART_SetIrdaMode(USART_Group group, uint8_t mode);
void USART_SetGuardTime(USART_Group group, uint16_t time);


#endif /* _STM32F10X_USART_H_ */

