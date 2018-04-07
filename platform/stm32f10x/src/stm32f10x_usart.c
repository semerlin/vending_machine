/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "stm32f10x_usart.h"
#include "stm32f10x_map.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_cfg.h"


/* flash register structure */
typedef struct 
{
    volatile uint16_t SR;
    uint16_t RESERVED0;
    volatile uint16_t DR;
    uint16_t RESERVED1;
    volatile uint16_t BRR;
    uint16_t RESERVED2;
    volatile uint16_t CR1;
    uint16_t RESERVED3;
    volatile uint16_t CR2;
    uint16_t RESERVED4;
    volatile uint16_t CR3;
    uint16_t RESERVED5;
    volatile uint16_t GTPR;
    uint16_t RESERVED6;
}USART_T;

/* usart definition */
#define UE               (1 << 13)
#define TE               (1 << 3)
#define RE               (1 << 2)
#define SR_TXE           (1 << 7)

#define ADD              (0x0f)
#define PSC              (0xff)

#define M                (1 << 12)
#define PARITY           (0x03 << 9)

#define STOP             (0x03 << 12)
#define CLKEN            (0x01 << 11)
#define CPOL             (0x01 << 10)
#define CPHA             (0x01 << 9)
#define LBCL             (1 << 8)

#define CRTS             (0x03 << 8)

#define WAKE             (1 << 11)

#define RWU              (1 << 1)


/* GPIO group array */
static USART_T * const USARTx[] = {(USART_T *)USART1_BASE, 
                                   (USART_T *)USART2_BASE,
                                   (USART_T *)USART3_BASE};


/**
 * @brief enable or disable usart
 * @param group: usart group
 * @param flag: TRUE: enable FALSE:disable
 */
void USART_Enable(USART_Group group, bool flag)
{
    assert_param(group < UASRT_Count);
    
    USART_T * const UsartX = USARTx[group]; 
    if(flag)
        UsartX->CR1 |= UE;
    else
        UsartX->CR1 &= ~UE;
}

/**
 * @brief enable or disable usart transfer
 * @param group: usart group
 * @param flag: TRUE: enable FALSE:disable
 */
void USART_TransEnable(USART_Group group, bool flag)
{
    assert_param(group < UASRT_Count);
    
    USART_T * const UsartX = USARTx[group];
    if(flag)
        UsartX->CR1 |= TE;
    else
        UsartX->CR1 &= ~TE;
}

/**
 * @brief enable or disable usart recive
 * @param group: usart group
 * @param flag: TRUE: enable FALSE:disable
 */
void USART_RecvEnable(USART_Group group, bool flag)
{
    assert_param(group < UASRT_Count);
    
    USART_T * const UsartX = USARTx[group];
    if(flag)
        UsartX->CR1 |= RE;
    else
        UsartX->CR1 &= ~RE;
}

void USART_Setup(USART_Group group, const USART_Config *config)
{
    assert_param(group < UASRT_Count);
    assert_param(config != NULL);
    assert_param((config->baudRate <= 4500000) && (config->baudRate != 0));
    assert_param(IS_USART_WORD_LENGTH(config->wordLength));
    assert_param(IS_USART_STOPBITS(config->stopBits));
    assert_param(IS_USART_PARITY(config->parity));
    assert_param(IS_USART_HARDWARE_FLOW_CONTROL(config->hardwareFlowControl));
    assert_param(IS_USART_CPOL(config->clkPolarity));
    assert_param(IS_USART_CPHA(config->clkPhase));
   
    USART_T * const UsartX = USARTx[group];
    
    uint32_t pclk = 0;
    if(group == USART1)
        pclk = RCC_GetPCLK2();
    else
        pclk = RCC_GetPCLK1();
    
    //config baudrate
    uint16_t divFraction = 0;
    uint16_t divMantissa = 0;
    float divVal = pclk / (float)(config->baudRate);
    divMantissa = (uint16_t)(divVal / 16);
    divFraction = (uint8_t)((divVal - divMantissa * 16));
    if(((divVal - divMantissa * 16) - divFraction) >= 0.5)
        divFraction += 1;
    
    UsartX->BRR = (divMantissa << 4) + divFraction;
    
    //config word lenght, tx/rx enable, parity
    UsartX->CR1 &= ~(M | TE | RE | PARITY);
    UsartX->CR1 |= (config->wordLength | config->parity);
    if(config->txEnable)
        UsartX->CR1 |= TE;
    if(config->rxEnable)
        UsartX->CR1 |= RE;
    
    //config stop bits, CK
    UsartX->CR2 &= ~(CLKEN | CPOL | CPHA | STOP | LBCL);
    UsartX->CR2 |= (config->stopBits | config->clkPolarity | config->clkPhase);
    if(config->clkEnable)
        UsartX->CR2 |= CLKEN;
    if(config->lastBitClkEnable)
        UsartX->CR2 |= LBCL;
    
    UsartX->CR3 &= ~CRTS;
    UsartX->CR3 |= config->hardwareFlowControl;

}

void USART_StructInit(USART_Config *config)
{   
    config->baudRate = 115200;
    config->wordLength = USART_WordLength_8;
    config->parity = USART_Parity_None;
    config->stopBits = USART_StopBits_1;
    config->hardwareFlowControl = USART_HardwareFlowControl_None;
    config->txEnable = TRUE;
    config->rxEnable = TRUE;
    config->clkEnable = FALSE;
    config->clkPolarity = USART_CPOL_Low;
    config->clkPhase = USART_CPHA_1Edge;
    config->lastBitClkEnable = FALSE;
}

/**
 * @brief set usart address
 * @param group: usart group
 * @param address
 */
void USART_SetAddress(USART_Group group, uint8_t address)
{
    assert_param(group < UASRT_Count);
    assert_param(address < 16);
    
    USART_T * const UsartX = USARTx[group];
    UsartX->CR2 &= ~ADD;
    UsartX->CR2 |= address;
}

/**
 * @brief get usart address
 * @param group: usart group
 * @return address
 */
uint8_t USART_GetAddress(USART_Group group)
{
    assert_param(group < UASRT_Count);
    
    USART_T * const UsartX = USARTx[group];
    return ((uint8_t)(UsartX->CR2 & ADD));
}

/**
 * @param set usart clock prescaler
 * @param group: usart group
 * @param prescaler
 */
void USART_SetPrescaler(USART_Group group, uint8_t prescaler)
{
    assert_param(group < UASRT_Count);
    
    USART_T * const UsartX = USARTx[group];
    UsartX->GTPR &= ~PSC;
    UsartX->GTPR |= prescaler;
}

/**
 * @param check uart flag status
 * @param group: usart group
 * @param flag: flag position
 * @return TRUE: flag is set FALSE: flag is not set
 */
bool USART_IsFlagOn(USART_Group group, uint16_t flag)
{
    assert_param(group < UASRT_Count);
    assert_param(IS_USART_FLAG(flag));
    
    USART_T * const UsartX = USARTx[group];
    if(UsartX->SR & flag)
        return TRUE;
    else
        return FALSE;
}

/**
 * @param clear uart flag status
 * @param group: usart group
 * @param flag: flag position
 */
void USART_ClearFlag(USART_Group group, uint16_t flag)
{
    assert_param(group < UASRT_Count);
    assert_param(IS_USART_FLAG(flag));
    
    USART_T * const UsartX = USARTx[group];
    
    UsartX->SR &= ~flag;
}

/**
 * @param write data and wait written done
 * @param group: usart group
 * @param data
 */
void USART_WriteData_Wait(USART_Group group, uint8_t data)
{
    assert_param(group < UASRT_Count);
    
    USART_T * const UsartX = USARTx[group];
    
    UsartX->DR = data;
    
    /* wait data written */
    while(!(UsartX->SR & SR_TXE));
}

/**
 * @param write data
 * @param group: usart group
 * @param data
 */
void USART_WriteData(USART_Group group, uint8_t data)
{
    assert_param(group < UASRT_Count);
    
    USART_T * const UsartX = USARTx[group];
    
    UsartX->DR = data;
}

/**
 * @param get data
 * @param group: usart group
 * @return data value
 */
uint8_t USART_ReadData(USART_Group group)
{
    assert_param(group < UASRT_Count);
    
    USART_T * const UsartX = USARTx[group];
    
    return UsartX->DR;
}

/**
 * @param set usart wakeup mode
 * @param group: usart group
 * @return wakeup mode
 */
void USART_SetWakeupMethod(USART_Group group, uint16_t method)
{
    assert_param(group < UASRT_Count);
    assert_param(IS_USART_WAKEUP(method));
    
    USART_T * const UsartX = USARTx[group];
    
    UsartX->CR1 &= ~WAKE;
    UsartX->CR1 |= method;
}

/**
 * @param enable usart interrupt
 * @param interrupt flag
 * @param enable or disable flag
 */
void USART_EnableInt(USART_Group group, uint8_t intFlag, bool flag)
{
    assert_param(group < UASRT_Count);
    assert_param(IS_USART_IT(intFlag));
    
    USART_T * const UsartX = USARTx[group];
    
    switch(intFlag & 0x01)
    {
    case 0x01:
        if(flag)
            UsartX->CR1 |= (1 << (intFlag >> 4));
        else
            UsartX->CR1 &= ~(1 << (intFlag >> 4));
        break;
    case 0x02:
        if(flag)
            UsartX->CR2 |= (1 << (intFlag >> 4));
        else
            UsartX->CR2 &= ~(1 << (intFlag >> 4));
        break;
    case 0x03:
        if(flag)
            UsartX->CR3 |= (1 << (intFlag >> 4));
        else
            UsartX->CR3 &= ~(1 << (intFlag >> 4));
        break;
    default:
        break;
    }
}

/**
 * @brief check if sepcific interrupt is enabled
 * @param usart group
 * @param interrupt flag
 * @return enable flag
 */
bool USART_IsIntEnabled(USART_Group group, uint8_t intFlag)
{
    assert_param(group < UASRT_Count);
    assert_param(IS_USART_IT(intFlag));
    
    USART_T * const UsartX = USARTx[group];
    bool ret = FALSE;
    switch(intFlag & 0x01)
    {
    case 0x01:
        if(UsartX->CR1 & (1 << (intFlag >> 4)))
            ret = TRUE;
        break;
    case 0x02:
        if(UsartX->CR2 & (1 << (intFlag >> 4)))
            ret = TRUE;
        break;
    case 0x03:
        if(UsartX->CR3 & (1 << (intFlag >> 4)))
            ret = TRUE;
        break;
    default:
        break;
    }
    
    return ret;
}

/**
 * @param set usart receive mode
 * @param group: usart group
 * @param mode
 */
void USART_SetRecvMode(USART_Group group, uint8_t mode)
{
    assert_param(group < UASRT_Count);
    assert_param(IS_USART_RECVMODE(mode));
    
    USART_T * const UsartX = USARTx[group];
    
    UsartX->CR1 &= ~RWU;
    UsartX->CR1 |= mode;
}

/**
 * @param set usart break character
 * @param group: usart group
 */
void USART_SendBreakCharacter(USART_Group group)
{
    assert_param(group < UASRT_Count);
    
    USART_T * const UsartX = USARTx[group];
    
    UsartX->CR1 |= 0x01;
}

/**
 * @param enable or disable usart LIN mode
 * @param group: usart group
 */
void USART_EnableLINMode(USART_Group group, bool flag)
{
    assert_param(group < UASRT_Count);
    
    USART_T * const UsartX = USARTx[group];
    
    if(flag)
        UsartX->CR2 |= (1 << 14);
    else
        UsartX->CR2 &= ~(1 << 14);
}

/**
 * @param set line break detect length
 * @param group: usart group
 * @param delect length
 */
void USART_SetLineBreakDetectLength(USART_Group group, uint8_t length)
{
    assert_param(group < UASRT_Count);
    assert_param(IS_USART_LIN_BREAK_DETECT_LENGTH(length));
    
    USART_T * const UsartX = USARTx[group];
    UsartX->CR2 &= ~(1 << 1);
    UsartX->CR2 |= length;
}

/**
 * @param enable or disable dma tx
 * @param group: usart group
 * @param enable or disable flag 
 */
void USART_EnableDMATX(USART_Group group, bool flag)
{
    assert_param(group < UASRT_Count);
    
    USART_T * const UsartX = USARTx[group];
    
    if(flag)
        UsartX->CR3 &= ~(1 << 7);
    else
        UsartX->CR3 |= (1 << 7);
}

/**
 * @param enable or disable dma rx
 * @param group: usart group
 * @param enable or disable flag 
 */
void USART_EnableDMARX(USART_Group group, bool flag)
{
    assert_param(group < UASRT_Count);
    
    USART_T * const UsartX = USARTx[group];
    
    if(flag)
        UsartX->CR3 &= ~(1 << 6);
    else
        UsartX->CR3 |= (1 << 6);
}

/**
 * @param enable or disable smartcard
 * @param group: usart group
 * @param enable or disable flag 
 */
void USART_SmartcardEnable(USART_Group group, bool flag)
{
    assert_param(group < UASRT_Count);
    
    USART_T * const UsartX = USARTx[group];
    if(flag)
        UsartX->CR3 &= ~(1 << 5);
    else
        UsartX->CR3 |= (1 << 5);
}

/**
 * @param enable or disable smartcard NACK
 * @param group: usart group
 * @param enable or disable flag 
 */
void USART_SmartcardNACKEnable(USART_Group group, bool flag)
{
    assert_param(group < UASRT_Count);
    
    USART_T * const UsartX = USARTx[group];
    if(flag)
        UsartX->CR3 &= ~(1 << 4);
    else
        UsartX->CR3 |= (1 << 5);
}

/**
 * @param enable or disable half duplex selection
 * @param group: usart group
 * @param enable or disable flag 
 */
void USART_HalfDuplexSelection(USART_Group group, bool flag)
{
    assert_param(group < UASRT_Count);
    
    USART_T * const UsartX = USARTx[group];
    if(flag)
        UsartX->CR3 &= ~(1 << 3);
    else
        UsartX->CR3 |= (1 << 5);
}

/**
 * @param enable or disable irda
 * @param group: usart group
 * @param enable or disable flag 
 */
void USART_IrdaEnable(USART_Group group, bool flag)
{
    assert_param(group < UASRT_Count);
    
    USART_T * const UsartX = USARTx[group];
    if(flag)
        UsartX->CR3 &= ~(1 << 1);
    else
        UsartX->CR3 |= (1 << 1);
}

/**
 * @param set irda mode
 * @param group: usart group
 * @param work mode 
 */
void USART_SetIrdaMode(USART_Group group, uint8_t mode)
{
    assert_param(group < UASRT_Count);
    assert_param(IS_USART_IRDA_MODE(mode));
    
    USART_T * const UsartX = USARTx[group];
    UsartX->CR3 &= ~(1 << 2);
    UsartX->CR3 |= mode;
}

void USART_SetGuardTime(USART_Group group, uint16_t time)
{
    assert_param(group < UASRT_Count);
    
    USART_T * const UsartX = USARTx[group];
    UsartX->GTPR &= ~(0x0ff << 8);
    UsartX->GTPR |= (time << 8);
}
