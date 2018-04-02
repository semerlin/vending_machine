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
    int err = ESP_ERR_OK;
    err = esp8266_setmode(AP, DEFAULT_TIMEOUT);
    if (ESP_ERR_OK != err)
    {
        return err;
    }

    return esp8266_set_softap("vendoring_machine", "123456", 5, 
            3, DEFAULT_TIMEOUT);
}
