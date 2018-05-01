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
#include "flash.h"
#include "modeswitch.h"

#undef __TRACE_MODULE
#define __TRACE_MODULE  "[http]"

/* default timeout time */
#define DEFAULT_TIMEOUT      (3000 / portTICK_PERIOD_MS)

/* ap configueation */
#define AP_NAME              "vendor"
#define AP_PWD               "12345678"
#define AP_CHL               5
#define AP_ENC               OPEN

/* ap address */
#define AP_IP           "192.168.10.1"
#define AP_GATEWAY      "192.168.10.1"
#define AP_NETMASK      "255.255.255.0"

TaskHandle_t xHttpHandle = NULL;

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
    uint8_t spec = 0;
    while('=' != *pdata++);
    while('&' != *pdata)
    {
        *name++ = *pdata++;
    }
    *name = '\0';
    while('=' != *pdata++);
    while(' ' != *pdata)
    {
        if ('%' == *pdata)
        {
            pdata++;
            spec = *pdata - '0';
            spec <<= 4;
            pdata++;
            spec += (*pdata - '0');
            pdata ++;
            *pwd++ = spec;
            continue;
        }
        *pwd++ = *pdata++;
    }
    *pwd = '\0';
}

/**
 * @brief http process task
 */
static void vHttpd(void *pvParameters)
{
    const TickType_t xDelay = 300 / portTICK_PERIOD_MS;
    uint8_t data[65];
    uint16_t len;
    char ssid[32];
    char pwd[32];
    uint8_t id = 0;
    for (;;)
    {
        if (ESP_ERR_OK == esp8266_recv(&id, data, &len, portMAX_DELAY))
        {
            if (0 == strncmp((const char *)data, "GET /setting?", 13))
            {
                parse_name_and_pwd((const char *)data, ssid, pwd);
                while (ESP_ERR_OK == esp8266_recv(&id, data, &len, xDelay));
                TRACE("get setting:%s(%s)\r\n", ssid, pwd);
                esp8266_disconnect_server(id);
                esp8266_close(80);
                break;
            }
            else if (0 == strncmp((const char *)data, "GET /", 5))
            {
                while (ESP_ERR_OK == esp8266_recv(&id, data, &len, xDelay));
                if (ESP_ERR_OK == esp8266_prepare_send(id, strlen(set_page)))
                {
                    esp8266_write(set_page, strlen(set_page));
                    esp8266_disconnect_server(id);
                }
            }
            else
            {
                while (ESP_ERR_OK == esp8266_recv(&id, data, &len, xDelay));
            }
        }
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    flash_set_ssid_pwd(ssid, pwd);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    SCB_SystemReset();
    
    vTaskDelete(NULL);
}
 
/**
 * @brief pms5003 data process task
 * @param serial handle
 */
bool http_init(void)
{
    int err;
    TRACE("initialize http...\r\n");
    err = esp8266_setmode(AP);
    if (ESP_ERR_OK != err)
    {
        return FALSE;
    }
    
    err = esp8266_set_softap(AP_NAME, AP_PWD, AP_CHL, AP_ENC);
    if (ESP_ERR_OK != err)
    {
        return FALSE;
    }
    
    err = esp8266_set_apaddr(AP_IP, AP_GATEWAY, AP_NETMASK);
    if (ESP_ERR_OK != err)
    {
        return FALSE;
    }
    
    err = esp8266_listen(80);
    if (ESP_ERR_OK != err)
    {
        return FALSE;
    }
    
    xTaskCreate(vHttpd, "httpd", HTTP_STACK_SIZE, NULL, 
                       HTTP_PRIORITY, &xHttpHandle);
    if (NULL == xHttpHandle)
    {
        return FALSE;
    }
    
    return TRUE;
}


