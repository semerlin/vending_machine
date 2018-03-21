/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "stm32f10x_nvic.h"
#include "stm32f10x_scb.h"
#include "stm32f10x_map.h"
#include "stm32f10x_cfg.h"

/* NVIC register must operate in privileged mode */

typedef struct
{
    volatile uint32_t ISER[3];
    uint32_t RESERVED0[29];
    volatile uint32_t ICER[3];
    uint32_t RESERVED1[29];
    volatile uint32_t ISPR[3];
    uint32_t RESERVED2[29];
    volatile uint32_t ICPR[3];
    uint32_t RESERVED3[29];
    volatile uint32_t IABR[3];
    uint32_t RESERVED4[61];
    volatile uint32_t IPR[21];
    uint32_t RESERVED5[683];
    volatile uint32_t STIR;
}NVIC_T;
       
/* NVIC register map definition */
NVIC_T *NVIC = (NVIC_T *)NVIC_BASE;


void NVIC_Init(const NVIC_Config *config)
{
    assert_param(config != NULL);
    assert_param(IS_NVIC_IRQ_CHANNEL(config->channel));
    assert_param((config->preemptionPriority + 1) * 
                 (config->subPriority + 1) <= 16);
    
    uint8_t minPreempPriority = SCB_GetMinPreemptionPriority();
    assert_param(config->preemptionPriority <= minPreempPriority);
    uint8_t subPriority = SCB_GetMinSubPriority();
    assert_param(config->subPriority <= subPriority);
    
    //clear interrupt flags
    NVIC->ICPR[config->channel >> 5] |= (1 << (config->channel % 32));
        
    if(config->enable)
    {
        //set interrupt priority
        uint8_t groupingPriority = SCB_GetPriorityGrouping();
        uint32_t priority = config->preemptionPriority;
        priority <<= (groupingPriority - 3);
        priority += config->subPriority;
        priority &= 0x0f;
        priority <<= 4;
        NVIC->IPR[config->channel >> 2] |= (priority << 
                                            ((config->channel % 4) * 8));
        //enable interrupt
        NVIC->ISER[config->channel >> 5] |= (1 << (config->channel % 32));
    }
    else
    {
        //disable interrupt
        NVIC->ICER[config->channel >> 5] |= (1 << (config->channel % 32));
    }
}

void NVIC_InitStruct(NVIC_Config *config)
{
    assert_param(config != NULL);
    config->channel = 0;
    config->preemptionPriority = 0;
    config->subPriority = 0;
    config->enable = FALSE;
}
/**
* @brief enable or disable irq channel
* @param irq channel
* @param enable or disable flag
*/
void NVIC_EnableIRQ(uint8_t channel, bool flag)
{
    assert_param(IS_NVIC_IRQ_CHANNEL(channel));
    if(flag)
        NVIC->ISER[channel >> 5] |= (1 << (channel % 32));
    else
        NVIC->ICER[channel >> 5] |= (1 << (channel % 32));
}

/**
* @brief set or reset irq pending
* @param irq channel
* @param set or reset flag
*/
void NVIC_SetIRQPending (uint8_t channel, bool flag)
{
    assert_param(IS_NVIC_IRQ_CHANNEL(channel));
    if(flag)
        NVIC->ISPR[channel >> 5] |= (1 << (channel % 32));
    else
        NVIC->ICPR[channel >> 5] |= (1 << (channel % 32));
}

/**
* @brief check whether irq is pending
* @param irq channel
* @return pending flag
*/
bool NVIC_IsIRQPending(uint8_t channel)
{
    assert_param(IS_NVIC_IRQ_CHANNEL(channel));
    if((NVIC->ISPR[channel >> 5] & (1 << (channel % 32))) != 0)
        return TRUE;
    else
        return FALSE;
}

/**
* @brief check whether irq is active
* @param irq channel
* @return active flag
*/
bool NVIC_IsIRQActive(uint8_t channel)
{
    assert_param(IS_NVIC_IRQ_CHANNEL(channel));
    if((NVIC->IABR[channel >> 5] & (1 << (channel % 32))) != 0)
        return TRUE;
    else
        return FALSE;
}

/**
* @brief set irq priority
* @param irq channel
* @param irq priority
*/
void NVIC_SetIRQPriority(uint8_t channel, uint8_t preemptionPriority,
                         uint8_t subPriority)
{
    assert_param(IS_NVIC_IRQ_CHANNEL(channel));
    assert_param((preemptionPriority + 1) * 
                 (subPriority + 1) <= 16);
    
    uint8_t minPreempPriority = SCB_GetMinPreemptionPriority();
    assert_param(preemptionPriority <= minPreempPriority);
    uint8_t minSubPriority = SCB_GetMinSubPriority();
    assert_param(subPriority <= minSubPriority);
    
    //clear interrupt flags
    NVIC->ICPR[channel >> 5] |= (1 << (channel % 32));
        
    //set interrupt priority
    uint8_t groupingPriority = SCB_GetPriorityGrouping();
    uint32_t priority = preemptionPriority;
    priority <<= (groupingPriority - 3);
    priority += subPriority;
    priority &= 0x0f;
    priority <<= 4;
    NVIC->IPR[channel >> 2] |= (priority << ((channel % 4) * 8));
}

/**
* @brief cget irq priority
* @param irq channel
* @return irq priority
*/
void NVIC_GetIRQPriority(uint8_t channel, uint8_t *preemptionPriority,
                         uint8_t *subPriority)
{
    assert_param(IS_NVIC_IRQ_CHANNEL(channel));
    assert_param(preemptionPriority != NULL);
    assert_param(subPriority != NULL);
    
    uint8_t priority = ((NVIC->IPR[channel >> 2] >> ((channel % 4) * 8)) & 0xf0);
    priority >>= 4;
    uint8_t groupingPriority = SCB_GetPriorityGrouping();
    *preemptionPriority = (priority >> (groupingPriority - 3));
    *subPriority = priority - (*preemptionPriority << (groupingPriority - 3));
}

/**
* @brief generate software irq
* @param irq num
*/
void NVIC_GenerateIRQ(uint8_t num)
{
    NVIC->STIR = num;
}
