/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "modeswitch.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "assert.h"
#include "trace.h"
#include "global.h"
#include "pinconfig.h"
#include "mqtt.h"
#include "wifi.h"
#include "flash.h"
#include "simple_http.h"

#undef __TRACE_MODULE
#define __TRACE_MODULE  "[mode]"


#define MODE_AP        1
#define MODE_SAT       2

uint8_t g_cur_mode = MODE_SAT;

#define SWITCH_COUNT    (5)


/**
 * @brief monitor button
 */
static void vModeMonitor(void *pvParameters)
{
    uint8_t count = 0;
    for (;;)
    {
        if (is_pinset("MODE_SET"))
        {
            count = 0;
        }
        else
        {
            count ++;
        }
        
        if (count > SWITCH_COUNT)
        {
            if (MODE_AP != g_cur_mode)
            {
                flash_restore();
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                SCB_SystemReset();
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief initilise mode switch module
 */
void modeswitch_init(void)
{
    if (flash_first_start())
    {
        g_cur_mode = MODE_AP;
    }
    else
    {
        g_cur_mode = MODE_SAT;
    }
    xTaskCreate(vModeMonitor, "ModeMonitor", MODESWITCH_STACK_SIZE, 
                NULL, MODESWITCH_PRIORITY, NULL);
}





