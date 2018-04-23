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
#include "wifi.h"
#include "mqtt.h"

#undef __TRACE_MODULE
#define __TRACE_MODULE  "[init]"


#define DEFAULT_TIMEOUT      (3000 / portTICK_PERIOD_MS)

/* ap configueation */
#define AP_NAME              "vendor"
#define AP_PWD               "12345678"
#define AP_CHL               5
#define AP_ENC               OPEN

/**
 * @brief network initialize task
 * @param pvParameter - task parameter
 */
static void vInitNetwork(void *pvParameters)
{
    TRACE("initialize network...\r\n");
    esp8266_init();
    esp8266_send_ok("ATE0\r\n", DEFAULT_TIMEOUT);
    esp8266_setmode(BOTH, DEFAULT_TIMEOUT);
    /* need restart after set esp8266 mode */
    //esp8266_send_ok("AT+RST\r\n", DEFAULT_TIMEOUT);
    esp8266_set_softap(AP_NAME, AP_PWD, AP_CHL, AP_ENC, DEFAULT_TIMEOUT);

    http_init();
    wifi_init();
    mqtt_init();
    
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
    TRACE("startup application...\r\n");
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
