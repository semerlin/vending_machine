/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "FreeRTOS.h"
#include "task.h"
#include "global.h"
#include "trace.h"
#include "serial.h"
#include "esp8266.h"
#include "m26.h"


#undef __TRACE_MODULE
#define __TRACE_MODULE  "[license]"

uint32_t g_count = 0;

/**
 * @brief license check task
 * @param pvParameter - task parameter
 */
static void vLicense(void *pvParameters)
{
    for (;;)
    {
        g_count++;
        if (g_count >= 3600 * 24)
        {
            /* license experid */
            /* shutdown network task */
            esp8266_shutdown();
            m26_shutdown();
            TRACE("license expired!\r\n");
            break;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

/**
 * @brief init license check
 */
void license_init(void)
{
    TRACE("initialise license system...\r\n");
    xTaskCreate(vLicense, "license", LICENSE_STACK_SIZE, NULL, 
                LICENSE_PRIORITY, NULL);
}






