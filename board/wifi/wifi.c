/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "wifi.h"
#include "esp8266.h"
#include "global.h"
#include "trace.h"

#undef __TRACE_MODULE
#define __TRACE_MODULE  "[wifi]"

#define DEFAULT_TIMEOUT      (3000 / portTICK_PERIOD_MS)
#define TRY_COUNT             3

static xQueueHandle xApInfoQueue = NULL;
typedef struct
{
    char name[32];
    char password[32];
}ap_info;

/**
 * @brief ap process task
 */
static void vConnectAp(void *pvParameters)
{
    ap_info info;
    uint8_t count = 0;
    for (;;)
    {
        count = 0;
        xQueueReceive(xApInfoQueue, &info, portMAX_DELAY);
        TRACE("connect ap:%s, %s\n", info.name, info.password);
        while(ESP_ERR_OK != esp8266_connect_ap(info.name, info.password, 
                                               20000 / portTICK_PERIOD_MS))
        {
            count++;
            if (count >= TRY_COUNT)
            {
                TRACE("connect to ap \"%s\" failed\n", info.name);
                break;
            }
        }
        
        if (count < TRY_COUNT)
        {
            TRACE("connect to ap \"%s\" success\n", info.name);
        }
    }
}

/**
 * @brief init wifi
 * @return init status
 */
int wifi_init(void)
{
    TRACE("initialize wifi...\n");
    xApInfoQueue = xQueueCreate(1, sizeof(ap_info) / sizeof(char));
    xTaskCreate(vConnectAp, "connectap", AP_STACK_SIZE, NULL, 
                       AP_PRIORITY, NULL);
    return 0;
}

/**
 * @brief connect ap
 * @param ssid - ap name
 * @param pwd - ap password
 */
void wifi_connect_ap(const char *ssid, const char *pwd)
{
    assert_param(NULL != xApInfoQueue);
    ap_info info;
    strcpy(info.name, ssid);
    strcpy(info.password, pwd);
    xQueueOverwrite(xApInfoQueue, &info);
}

