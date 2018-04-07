/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "stm32f10x_cfg.h"
#include "global.h"
#include "FreeRTOS.h"
#include "task.h"
#include "trace.h"

/**
 * @brief initialize system
 * @param pvParameters - task parameter
 */
static void vInitSystem(void *pvParameters)
{
    TRACE("startup application...\n");
    vTaskDelete(NULL);
}

/**
 * @brief start system
 */
void ApplicationStartup()
{
    xTaskCreate(vInitSystem, "Init", INIT_SYSTEM_STACK_SIZE, NULL, 
                INIT_SYSTEM_PRIORITY, NULL);
    
    /* Start the scheduler. */
    vTaskStartScheduler();
}
