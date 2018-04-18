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
#include "wifi.h"

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
 * @brief parse ap name and password from data
 * @param data - data to parse
 * @param name - ap name
 * @param pwd - ap password
 */
static void parse_name_and_pwd(const char *data, char *name, char *pwd)
{
    const char *pdata = data;
    while('=' != *pdata++);
    while('&' != *pdata)
    {
        *name++ = *pdata++;
    }
    *name = '\0';
    while('=' != *pdata++);
    while(' ' != *pdata)
    {
        *pwd++ = *pdata++;
    }
    *pwd = '\0';
}

/**
 * @brief http process task
 */
static void vHttpd(void *pvParameters)
{
    //esp8266_send_ok("AT+CIPSERVERMAXCONN=1\r\n", DEFAULT_TIMEOUT);
    esp8266_listen(80, DEFAULT_TIMEOUT);
    esp8266_set_tcp_timeout(10, DEFAULT_TIMEOUT);
    const TickType_t xDelay = 300 / portTICK_PERIOD_MS;
    char data[65];
    uint16_t len;
    char apname[32];
    char password[32];
    uint16_t id = 0xffff;
    for (;;)
    {
        if (ESP_ERR_OK == esp8266_recv(in, data, &len, portMAX_DELAY))
        {
            if (0 == strncmp(data, "GET / HTTP/1.1", 14))
            {
                while (ESP_ERR_OK == esp8266_recv(in, data, &len, xDelay));
                id = esp8266_tcp_id(in);
                if (0xffff != id)
                {
                    if (ESP_ERR_OK == esp8266_prepare_send(id, 
                                                   strlen(set_page), 
                                                   DEFAULT_TIMEOUT))
                    {
                        esp8266_write(set_page, strlen(set_page), DEFAULT_TIMEOUT);
                        esp8266_disconnect(id, DEFAULT_TIMEOUT);
                    }
                }
            }
            else if (0 == strncmp(data, "GET /setting?", 13))
            {
                apname[0] = 0;
                password[0] = 0;
                parse_name_and_pwd(data, apname, password);
                wifi_connect_ap(apname, password);
                while (ESP_ERR_OK == esp8266_recv(in, data, &len, xDelay));
                id = esp8266_tcp_id(in);
                if (0xffff != id)
                {
                    esp8266_disconnect(id, DEFAULT_TIMEOUT);
                }
            }
            else
            {
                while (ESP_ERR_OK == esp8266_recv(in, data, &len, xDelay));
                id = esp8266_tcp_id(in);
                if (0xffff != id)
                {
                    esp8266_disconnect(id, DEFAULT_TIMEOUT);
                }
            }
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

