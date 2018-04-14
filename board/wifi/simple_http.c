/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "FreeRTOS.h"
#include "task.h"
#include "simple_http.h"
#include "esp8266.h"
#include "trace.h"
#include "global.h"

#undef __TRACE_MODULE
#define __TRACE_MODULE  "[http]"

/* default timeout time */
#define DEFAULT_TIMEOUT      (3000 / portTICK_PERIOD_MS)

/* ap address */
#define AP_IP           "192.168.10.1"
#define AP_GATEWAY      "192.168.10.1"
#define AP_NETMASK      "255.255.255.0"


const char set_page[] ="<html><body><a href=\"/ON\"><input type=\"Button\" value=\"ON\" \
style=\"position:absolute;top:50%;left:40%\"></input></a><a href=\"/OFF\"><input type=\"Button\" value=\"OFF\" \
style=\"position:absolute;top:50%;left:60%\"></input></a></body></html>";

static void vHttpd(void *pvParameters)
{
    esp8266_listen(80, DEFAULT_TIMEOUT);
    for (;;)
    {
        
    }
    //esp8266_close(80, DEFAULT_TIMEOUT);
}

/**
 * @brief pms5003 data process task
 * @param serial handle
 */
int http_init(void)
{
    int err;
    TRACE("initialize http...\n");
    err = esp8266_send_ok("AT+CIPMUX=1\r\n", DEFAULT_TIMEOUT);
    if (ESP_ERR_OK == err)
    {
        err = esp8266_set_apaddr(AP_IP, AP_GATEWAY, AP_NETMASK, 
                                 DEFAULT_TIMEOUT);
        if (ESP_ERR_OK == err)
        {
           xTaskCreate(vHttpd, "httpd", HTTP_STACK_SIZE, NULL, 
                       HTTP_PRIORITY, NULL); 
        }
    }

    TRACE("http status: %d\n", -err);
    return err;
}

