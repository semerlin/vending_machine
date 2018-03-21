/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "stm32f10x_systick.h"
#include "stm32f10x_map.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_cfg.h"


/* systick register structure */
typedef struct
{
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
}SYSTICK_T;

SYSTICK_T *SYSTICK = (SYSTICK_T *)SYSTICK_BASE;

/* systick regiter definition */
#define CTRL_COUNTFLAG    (1 << 16)
#define CTRL_CLKSOURCE    (1 << 2)
#define CTRL_TICKINT      (1 << 1)
#define CTRL_ENABLE       (0x01)



/**
 * @brief set systick clock source
 * @param clock source
 */
void SYSTICK_SetClockSource(uint8_t source)
{
    assert_param(IS_SYSTICK_CLOCK_PARAM(source));
    SYSTICK->CTRL &= ~CTRL_CLKSOURCE;
    SYSTICK->CTRL |= source;
}

/**
 * @brief systick exception request enable
 * @param enable flag
 */
void SYSTICK_EnableInt(bool flag)
{
    SYSTICK->CTRL &= ~CTRL_TICKINT;
    if(flag)
        SYSTICK->CTRL |= CTRL_TICKINT;
}

/**
 * @brief start or stop systick counter
 * @param start flag
 */
void SYSTICK_EnableCounter(bool flag)
{
    SYSTICK->CTRL &= ~CTRL_ENABLE;
    if(flag)
        SYSTICK->CTRL |= CTRL_ENABLE;
}

/**
 * @brief check if count flag is set
 * @return TRUE:set FALSE:not set
 */
bool SYSTICK_IsCountFlagSet(void)
{
    if(SYSTICK->CTRL & CTRL_COUNTFLAG)
        return TRUE;
    
    return FALSE;
}

/**
 * @brief clear systick count flag
 */
void SYSTICK_ClrCountFlag(void)
{
    SYSTICK->CTRL &= ~CTRL_COUNTFLAG;
}

/**
 * @brief set systick tick interval
 * @param time intervel, unit is ms
 */
void SYSTICK_SetTickInterval(uint32_t time)
{
    uint32_t hclk = RCC_GetHCLK();
    uint32_t tickClock = 0;
    if(SYSTICK->CTRL & CTRL_CLKSOURCE)
        tickClock = hclk;
    else
        tickClock = (hclk >> 3);
    
#ifdef __DEBUG
    uint32_t maxInterval = (1 << 24) / (tickClock / 1000);
    assert_param(time <= maxInterval);
#endif
    
    SYSTICK->LOAD = ((tickClock / 1000 * time) & 0xffffff);
}
