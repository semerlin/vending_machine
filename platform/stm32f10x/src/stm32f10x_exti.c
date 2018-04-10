/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "stm32f10x_exti.h"
#include "stm32f10x_map.h"
#include "stm32f10x_cfg.h"
#include "assert.h"

/* exti structure */
typedef struct 
{
    volatile uint32_t IMR;
    volatile uint32_t EMR;
    volatile uint32_t RTSR;
    volatile uint32_t FTSR;
    volatile uint32_t SWIER;
    volatile uint32_t PR;
}EXTI_T;

EXTI_T *EXTI = (EXTI_T *)EXTI_BASE;

#define MAX_LINE   (20)


/**
 * @brief enable line interrupt
 * @param line - interrupt line
 * @param flag - TRUE: enable
 *               FALSE: disable
 */
void EXTI_EnableLine_INT(uint8_t line, bool flag)
{
    assert_param(line < MAX_LINE);

    if (flag)
    {
        EXTI->IMR |= (1 << line);
    }
    else
    {
        EXTI->IMR &= ~(1 << line);
    }
}

/**
 * @brief enable line event 
 * @param line - event line
 * @param flag - TRUE: enable
 *               FALSE: disable
 */
void EXTI_EnableLine_EVENT(uint8_t line, bool flag)
{
    assert_param(line < MAX_LINE);

    if (flag)
    {
        EXTI->EMR |= (1 << line);
    }
    else
    {
        EXTI->EMR &= ~(1 << line);
    }
}

/**
 * @brief set line trigger mode
 * @param line - event line
 * @param edge - trigger mode
 */
void EXTI_SetTrigger(uint8_t line, Trigger_Edge edge)
{
    assert_param(line < MAX_LINE);

    if (0 != (edge & Trigger_Rising))
    {
        EXTI->RTSR |= (1 << line);
    }

    if (0 != (edge & Trigger_Falling))
    {
        EXTI->FTSR |= (1 << line);
    }
}

/**
 * @brief clear line trigger mode
 * @param line - event line
 * @param edge - trigger mode
 */
void EXTI_ClrTrigger(uint8_t line, Trigger_Edge edge)
{
    assert_param(line < MAX_LINE);

    if (0 != (edge & Trigger_Rising))
    {
        EXTI->RTSR &= ~(1 << line);
    }

    if (0 != (edge & Trigger_Falling))
    {
        EXTI->FTSR &= ~(1 << line);
    }
}

/**
 * @brief generate software interrupt event
 * @param line - event line
 */
void EXTI_SetSoftInt(uint8_t line)
{
    assert_param(line < MAX_LINE);

    EXTI->SWIER |= (1 << line);
}

/**
 * @brief check interrupt pending status
 * @param line - interrupt line
 * @return pending status
 */
bool EXTI_IsPending(uint8_t line)
{
    assert_param(line < MAX_LINE);

    return (0 != (EXTI->PR & (1 << line)));
}

/**
 * @brief clear interrupt pending status
 * @param line - interrupt line
 */
void EXTI_ClrPending(uint8_t line)
{
    assert_param(line < MAX_LINE);

    EXTI->PR |= (1 << line);
}


