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
#include "esp8266.h"
#include "simple_http.h"
#include "motorctl.h"

#undef __TRACE_MODULE
#define __TRACE_MODULE  "[init]"

/**
 * @brief network initialize task
 * @param pvParameter - task parameter
 */
static void vInitNetwork(void *pvParameters)
{
    TRACE("initialize network...\n");
    esp8266_init();
    //esp8266_send_ok("AT+RST\r\n", 3000 / portTICK_PERIOD_MS);
    esp8266_send_ok("ATE0\r\n", 3000 / portTICK_PERIOD_MS);
    http_init();
    
    vTaskDelete(NULL);
}

/**
 * @brief initialize network
 */
static __INLINE void network_init(void)
{
    xTaskCreate(vInitNetwork, "Init", INIT_NETWORK_STACK_SIZE, NULL, 
                INIT_NETWORK_PRIORITY, NULL);
}

/**
 * @brief initialize system
 * @param pvParameters - task parameter
 */
static void vInitSystem(void *pvParameters)
{
    TRACE("startup application...\n");
    network_init();
    //motor_init();
    
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
