/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "FreeRTOS.h"
#include "simple_http.h"
#include "esp8266.h"
#include "trace.h"

#undef __TRACE_MODULE
#define __TRACE_MODULE  "[http]"

/* default timeout time */
#define DEFAULT_TIMEOUT      (3000 / portTICK_PERIOD_MS)

const char set_page[] ="<html><body><a href=\"/ON\"><input type=\"Button\" value=\"ON\" \
style=\"position:absolute;top:50%;left:40%\"></input></a><a href=\"/OFF\"><input type=\"Button\" value=\"OFF\" \
style=\"position:absolute;top:50%;left:60%\"></input></a></body></html>";

/**
 * @brief pms5003 data process task
 * @param serial handle
 */
int http_init(void)
{
    TRACE("initialize http...\n");
    int err = ESP_ERR_OK;
    err = esp8266_setmode(AP, DEFAULT_TIMEOUT);
    err = esp8266_send_ok("AT+RST\r\n", 3000 / portTICK_PERIOD_MS);
    if (ESP_ERR_OK == err)
    {
        err = esp8266_set_softap("vendor", "0123456789", 11, 
                0, DEFAULT_TIMEOUT);
    }

    TRACE("http status: %d\n", -err);
    return err;
}

