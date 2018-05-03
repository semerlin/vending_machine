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
#include "led_motor.h"
#include "led_net.h"
#include "ir.h"
#include "m26.h"
#include "mode.h"
#include "license.h"
#include "modeswitch.h"
#include "flash.h"

#undef __TRACE_MODULE
#define __TRACE_MODULE  "[init]"


#define DEFAULT_TIMEOUT      (3000 / portTICK_PERIOD_MS)
#define VERSION  ("v1.0.2.3_alpha")

/**
 * @brief initialize esp8266 module
 * @return initialize status
 */
static bool init_esp8266(void)
{
    if (!esp8266_init())
    {
        return FALSE;
    }
    
    if (ESP_ERR_OK != esp8266_send_ok("ATE0\r\n"))
    {
        return FALSE;
    }
    
    if (ESP_ERR_OK != esp8266_send_ok("AT+CIPMUX=1\r\n"))
    {
        return FALSE;
    }
    
    if (flash_first_start())
    {
        TRACE("first start\r\n");
        return http_init();
    }
    else
    {
        if (mqtt_init())
        {
            return wifi_init();
        }
        else
        {
            return FALSE;
        }
    }
}

/**
 * @brief initialize m26 module
 * @return initialize status
 */
static bool init_m26()
{
    if (!m26_init())
    {
        return FALSE;
    }

    if (M26_ERR_OK != m26_sync())
    {
       return FALSE; 
    }
  
    //vTaskDelay(5000 / portTICK_PERIOD_MS);
#if 1
    if (M26_ERR_OK != m26_send_ok("AT+IPR=115200&W\r\n", DEFAULT_TIMEOUT))
    {
        return FALSE;
    }
#endif

    if (M26_ERR_OK != m26_send_ok("ATE0\r\n", DEFAULT_TIMEOUT))
    {
        return FALSE;
    }
    
    if (M26_ERR_OK != m26_send_ok("AT+QIHEAD=1\r\n", DEFAULT_TIMEOUT))
    {
        return FALSE;
    }

    if (M26_ERR_OK != m26_send_ok("AT+CREG=1\r\n", DEFAULT_TIMEOUT))
    {
        return FALSE;
    }

    if (M26_ERR_OK != m26_send_ok("AT+CGREG=1\r\n", DEFAULT_TIMEOUT))
    {
        return FALSE;
    }
    
    mqtt_init();
    wifi_init();

    return TRUE;
}

/**
 * @brief network initialize task
 * @param pvParameter - task parameter
 */
static void vInitNetwork(void *pvParameters)
{
    TRACE("initialize network...\r\n");
    
    if (MODE_NET_WIFI == mode_net())
    {
        if (!init_esp8266())
        {
            TRACE("initialize network failed\r\n");
            led_net_set_action("LED_ERROR", on);
        }
    }
    else
    {
        if (!init_m26())
        {
            TRACE("initialize network failed\r\n");
            led_net_set_action("LED_ERROR", on);
        }
    }
    
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
    TRACE("version = %s\r\n", VERSION);
    led_motor_init();
    led_net_init();
    ir_init();
    motor_init();
    network_init();
    modeswitch_init();
    
    vTaskDelete(NULL);
}

/**
 * @brief initialize system
 * @param pvParameters - task parameter
 */
static void vTestSystem(void *pvParameters)
{
    TRACE("startup test...\r\n");

    vTaskDelete(NULL);
}

/**
 * @brief start system
 */
void ApplicationStartup()
{
    mode_init();
    license_init();
    if (MODE_WORK_NORMAL == mode_work())
    {
        xTaskCreate(vInitSystem, "Init", INIT_SYSTEM_STACK_SIZE, NULL, 
                    INIT_SYSTEM_PRIORITY, NULL);
    }
    else
    {
        xTaskCreate(vTestSystem, "Test", INIT_SYSTEM_STACK_SIZE, NULL, 
                    INIT_SYSTEM_PRIORITY, NULL);
    }
    
    /* Start the scheduler. */
    vTaskStartScheduler();
}
