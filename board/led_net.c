/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "led_motor.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "assert.h"
#include "trace.h"
#include "pinconfig.h"

#undef __TRACE_MODULE
#define __TRACE_MODULE  "[led_net]"

#define LED_NUM  3

/* led name */
const char *led_names[LED_NUM] = {"LED1", "LED2", "LED3"};
/* timer handler */
TimerHandle_t led_timers[LED_NUM] = {NULL, NULL, NULL};

/**
 * @brief timer callback
 * @param pxTimer - timer handler
 */
static void vTimerCallback(TimerHandle_t pxTimer)
{
    int32_t led_num;
    
    /* Which timer expired? */
    led_num = (int32_t)pvTimerGetTimerID(pxTimer);

    pin_toggle(led_names[led_num]);
}

/**
 * @brief turn on led
 * @param num - led number(0-2)
 */
void led_net_turn_on(uint8_t num)
{
    assert_param(num < LED_NUM);
    pin_set(led_names[num]);
}

/**
 * @brief turn off led
 * @param num - led number(0-2)
 */
void led_net_turn_off(uint8_t num)
{
    assert_param(num < LED_NUM);
    pin_reset(led_names[num]);
}

/**
 * @brief flashing led
 * num - led number(0-2)
 * interval - flashing interval
 */
void led_net_flashing(uint8_t num, TickType_t interval)
{
    assert_param(num < LED_NUM);
    if (NULL != led_timers[num])
    {
        if (pdPASS != xTimerStart(led_timers[num], 0))
        {
            TRACE("start timer failed\n");
        }
    }
    else
    {
        led_timers[num] = xTimerCreate("Timer", interval, pdTRUE, (void *)num,
                                    vTimerCallback);
        if (NULL == led_timers[num])
        {
            TRACE("create timer failed\n");
        }
        else
        {
            if (pdPASS != xTimerStart(led_timers[num], 0))
            {
                TRACE("start timer failed\n");
            }
        }
    }
}

/**
 * @brief stop led flashing
 * num - led number(0-2)
 */
void led_net_stop_flashing(uint8_t num)
{
    assert_param(num < LED_NUM);
    assert_param(NULL != led_timers[num]);
    xTimerStop(led_timers[num], 0);
}
