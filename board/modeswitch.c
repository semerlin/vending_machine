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


static xQueueHandle xModeQueue = NULL;
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
                modeswitch_set(MODE_AP);
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief switch mode
 */
static void vModeSwitch(void *pvParameters)
{
    uint8_t mode = 0;
    for (;;)
    {
        xQueueReceive(xModeQueue, &mode, portMAX_DELAY);
        if (g_cur_mode != mode)
        {
            g_cur_mode = mode;
            TRACE("enter mode: %d\r\n", mode);
            switch(mode)
            {
            case MODE_AP:
                wifi_deinit();
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                mqtt_deinit();
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                http_init();
                break;
            case MODE_SAT:
                SCB_SystemReset();
                mqtt_init();
                wifi_init();
                break;
            default:
                break;
            }
        }
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
    xModeQueue = xQueueCreate(1, 1);
    xTaskCreate(vModeMonitor, "ModeMonitor", MODESWITCH_STACK_SIZE, 
                NULL, MODESWITCH_PRIORITY, NULL);
    xTaskCreate(vModeSwitch, "ModeSwitch", MODECHANGE_STACK_SIZE, 
                NULL, MODECHANGE_PRIORITY, NULL);
}

/**
 * @brief set mode
 * @param mode - mode to set
 */
void modeswitch_set(uint8_t mode)
{
    xQueueOverwrite(xModeQueue, &mode);
}




