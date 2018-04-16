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
#include "simple_http.h"
#include "esp8266.h"
#include "trace.h"
#include "global.h"
#include "dbgserial.h"

#undef __TRACE_MODULE
#define __TRACE_MODULE  "[http]"

/* default timeout time */
#define DEFAULT_TIMEOUT      (3000 / portTICK_PERIOD_MS)

/* ap address */
#define AP_IP           "192.168.10.1"
#define AP_GATEWAY      "192.168.10.1"
#define AP_NETMASK      "255.255.255.0"

/* set page */
const char set_page[] = "\
<html>\r\n \
<body>\r\n \
<p>set AP name and password</p>\r\n \
<form action=\"/setting\" method=\"get\" target=\"_self\">\r\n \
  AP Name:<br>\r\n \
  <input type=\"text\" name=\"apname\">\r\n \
  <br>\r\n \
  AP Password:<br>\r\n \
  <input type=\"text\" name=\"appwd\">\r\n \
  <br><br>\r\n \
  <input type=\"submit\" value=\"set\">\r\n \
</form>\r\n \
</body>\r\n \
</html>";

/**
 * @brief http process task
 */
static void vHttpd(void *pvParameters)
{
    esp8266_listen(80, DEFAULT_TIMEOUT);
    esp8266_set_tcp_timeout(10, DEFAULT_TIMEOUT);
    const TickType_t xDelay = 3 / portTICK_PERIOD_MS;
    char data;
    int ret = 0;
    for (;;)
    {
        //esp8266_wait_connect(portMAX_DELAY);
        if (esp8266_getchar(&data, portMAX_DELAY))
        {
            while (esp8266_getchar(&data, xDelay));
        }
        if (ESP_ERR_OK == esp8266_prepare_send(esp8266_tcp_id(), 
                                               strlen(set_page), DEFAULT_TIMEOUT))
        {
            ret = esp8266_send(set_page, strlen(set_page), DEFAULT_TIMEOUT);
            TRACE("send tcp data status: %d\n", ret);
            esp8266_disconnect(esp8266_tcp_id(), DEFAULT_TIMEOUT);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
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

