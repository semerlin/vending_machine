/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "stm32f10x_iwdg.h"
#include "stm32f10x_map.h"
#include "stm32f10x_cfg.h"

/* iwdg strcture */
typedef struct 
{
    volatile uint16_t KR;
	uint16_t RESERVED0;
    volatile uint16_t PR;
	uint16_t RESERVED1;
    volatile uint16_t RLR;
	uint16_t RESERVED2;
    volatile uint16_t SR;
	uint16_t RESERVED3;
}IWDG_T;

IWDG_T *IWDG = (IWDG_T *)IWDG_BASE;

/********************************************************/
#define MAX_WAITCOUNT 4

/**************************************************
*  IWDG bit band
***************************************************/
#define IWDG_OFFSET (IWDG_BASE - PERIPH_BASE)
/*  SR register bit band */
#define SR_OFFSET (IWDG_OFFSET + 0x0c)
#define SR_RVU (PERIPH_BB_BASE + SR_OFFSET * 32 + 0x01 * 4)
#define SR_PVU (PERIPH_BB_BASE + SR_OFFSET * 32 + 0x00 * 4)



/**
 * @brief start idwg
 */
void IWDG_Startup(void)
{
    IWDG->KR = 0xcccc;
}

/**
 * @brief feed iwdg
 */
void IWDG_Feed(void)
{
    IWDG->KR = 0xaaaa;
}


/**
 * @brief set iwdg clock prescaler, iwdg clock is 40KHz
 * @param div: prescaler value
 */
void IWDG_SetClockPrescaler(uint8_t div)
{
    uint8_t waitCount = 0;
	volatile uint8_t i = 0;
	
    assert_param(IS_IWDG_PR_PARAM(div));

    //check if a write operation to this register is ongoing
    while((*((volatile uint32_t *)SR_PVU)) && (waitCount < MAX_WAITCOUNT))
    {
        //wait for complete
        for(i = 0; i < 128; i++);
        waitCount++;
    }

    if(waitCount >= MAX_WAITCOUNT)
    {
        return ;
    }

    IWDG->KR = 0x5555;
    IWDG->PR = div;
    IWDG->KR = 0x0000;
}


/**
 * @brief set iwdg clock prescaler, iwdg clock is 40KHz
 * @return prescaler value, failed if value is 0xff
 */
uint8_t IWDG_GetClockPrescaler(void)
{
    uint8_t waitCount = 0;
	volatile uint8_t i = 0;
	
    //check if a write operation to this register is ongoing
    while((*((volatile uint32_t *)SR_PVU)) && (waitCount < MAX_WAITCOUNT))
    {
        for(i = 0; i < 128; i++);
        waitCount++;
    }

    if(waitCount >= MAX_WAITCOUNT)
    {
        return 0xff;
    }

    return (IWDG->PR & 0x03);
}

/**
 * @brief set iwdg count reload value
 * @param value: count reload value
 */
void IWDG_SetReloadValue(uint8_t value)
{
    uint8_t waitCount = 0;
	volatile uint8_t i = 0;
	
	value &= 0x0fff;
	
    //check if a write operation to this register is ongoing
    while((*((volatile uint32_t*)SR_RVU)) && (waitCount < MAX_WAITCOUNT))
    {
        for(i = 0; i < 128; i++);
        waitCount++;
    }

    if(waitCount >= MAX_WAITCOUNT)
    {
        return ;
    }

    IWDG->KR = 0x5555;
    IWDG->RLR = value;
    IWDG->KR = 0x0000;
}

/**
 * @brief set iwdg count reload value
 * @return count reload value, failed is value is 0xffff
 */
uint16_t IWDG_GetReloadValue(void)
{
    uint8_t waitCount = 0;
	volatile uint8_t i = 0;

    //check if a write operation to this register is ongoing
    while((*((volatile uint32_t*)SR_RVU)) && (waitCount < MAX_WAITCOUNT))
    {
        for(i = 0; i < 128; i++);
        waitCount++;
    }

    if(waitCount >= MAX_WAITCOUNT)
    {
        return 0xffff;
    }

    return (IWDG->RLR & 0x0fff);
}















