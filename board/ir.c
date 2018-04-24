/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "ir.h"
#include "FreeRTOS.h"
#include "task.h"
#include "trace.h"
#include "pinconfig.h"
#include "global.h"
#include "stm32f10x_cfg.h"
#include "led_motor.h"

#undef __TRACE_MODULE
#define __TRACE_MODULE  "[ir]"

#define MAX_OFF_COUNT    (10)

/**
 * @brief motor control task
 * @param pvParameter - parameters pass to task
 */
static void vIRCtl(void *pvParameters)
{
    uint8_t count = 0;
    bool led_off = TRUE;
    for (;;)
    {
        if (is_pinset("IR_IN"))
        {
            /* no human */
            count ++;
            if (count > MAX_OFF_COUNT)
            {
                count = MAX_OFF_COUNT;
                if (!led_off)
                {
                    led_off = TRUE;
                    led_motor_all_off();
                }
            }
        }
        else
        {
            count = 0;
            if (led_off)
            {
                led_off = FALSE;
                led_motor_all_on();
            }
        }
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief initialize motor control
 */
void ir_init(void)
{
    TRACE("initialize ir...\r\n");

    xTaskCreate(vIRCtl, "IRCtl", IR_STACK_SIZE, 
                NULL, IR_PRIORITY, NULL);
}