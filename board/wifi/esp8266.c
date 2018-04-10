/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "esp8266.h"
#include "serial.h"
#include "global.h"
#include "trace.h"
#include "pinconfig.h"

#undef __TRACE_MODULE
#define __TRACE_MODULE "[esp8266]"

/* esp8266 work in block mode */

/* serial handle */
serial *g_serial = NULL;

/* recive queue */
static xQueueHandle xRecvQueue = NULL;
#define ESP_MAX_NODE_NUM              (2)
#define ESP_MAX_MSG_LINE              (4)
#define ESP_MAX_MSG_SIZE_PER_LINE     (16)

typedef struct
{
    uint8_t size;
    uint8_t data[ESP_MAX_MSG_LINE][ESP_MAX_MSG_SIZE_PER_LINE];
}at_node;

/* timeout time(ms) */
#define DEFAULT_TIMEOUT      (3000 / portTICK_PERIOD_MS)

/**
 * @brief pms5003 data process task
 * @param serial handle
 */
static void vESP8266Response(void *pvParameters)
{
    const TickType_t xDelay = 3 / portTICK_PERIOD_MS;
    serial *pserial = pvParameters;
    at_node node;
    uint8_t line = 0;
    uint8_t *pData = node.data[line];
    char data;
    uint8_t cnt = 0;
    for (;;)
    {
        if (serial_getchar(pserial, &data, portMAX_DELAY))
        {
            *pData++ = data;
            cnt++;
            while (serial_getchar(pserial, &data, xDelay))
            {
                *pData++ = data;
                cnt++;
                if (cnt >= ESP_MAX_MSG_SIZE_PER_LINE)
                {

                    /* TODO: error happened, process it like get '\r\n', 
                       this is not good, fix it later */
                    line++;
                    pData = node.data[line];
                    continue;
                }

                *pData = 0x00;
                if (0 == strcmp((const char *)(pData - 2), "\r\n"))
                {
                    /* get line data */
                    line++;
                    if (0 == strcmp((const char *)(pData - 4), "OK\r\n"))
                    {
                        /* get done */
                        break;
                    }
                    pData = node.data[line];
                }
            }

            /* get done, process data */
            node.size = line;
            xQueueSend(xRecvQueue, &node, portMAX_DELAY);
            pData = node.data[0];
            cnt = 0;
            line = 0;
        }
    }
}

/**
 * @brief initialize esp8266
 * @return 0 means success, otherwise error code
 */
bool esp8266_init(void)
{
    TRACE("initialize esp8266...\n");
    pin_reset("WIFI_EN");
    g_serial = serial_request(COM2);
    if (NULL == g_serial)
    {
        TRACE("initialize failed, can't open serial \'COM2\'\n");
        return FALSE;
    }
    serial_open(g_serial);

    xRecvQueue = xQueueCreate(ESP_MAX_NODE_NUM, 
                              sizeof(at_node) / sizeof(uint8_t));
    if (NULL == xRecvQueue)
    {
        TRACE("initialize failed, can't create queue\'COM2\'\n");
        serial_release(g_serial);
        g_serial = NULL;
        return FALSE;
    }

    xTaskCreate(vESP8266Response, "ESP8266Response", ESP8266_STACK_SIZE, 
                g_serial, ESP8266_PRIORITY, NULL);
    //pin_set("WIFI_EN");
     
    return TRUE;
}

/**
 * @brief test esp8266 module
 * @param cmd - cmd to send
 * @param time - timeout time
 * @return 0 means connect success, otherwise failed
 */
int esp8266_send_ok(const char *cmd, TickType_t time)
{
    assert_param(NULL != g_serial);

    at_node node;
    serial_putstring(g_serial, cmd, strlen(cmd));
    int ret = ESP_ERR_OK;

    if (pdPASS == xQueueReceive(xRecvQueue, &node, time))
    {
        ret = strcmp((const char *)node.data[node.size - 1], "OK") ? -ESP_ERR_FAIL: ESP_ERR_OK;
    }
    else
    {
        ret = -ESP_ERR_TIMEOUT;
    }

    TRACE("send cmd: %s", cmd);
    TRACE("status: %d\n", -ret);
    return ret;
}

/**
 * @brief write data
 * @param data - data
 * @param length - data length
 */
void esp8266_send(const char *data, uint32_t length)
{
    serial_putstring(g_serial, data, length);
}

/**
 * @brief set esp8266 work mode
 * @param mode - work mode
 * @param time - timeout time
 * @return 0 means connect success, otherwise failed
 */
int esp8266_setmode(esp8266_mode mode, TickType_t time)
{
    assert_param(NULL != g_serial);

    char str_mode[16];
    sprintf(str_mode, "AT+CWMODE_CUR=%d\r\n", mode);
    return esp8266_send_ok(str_mode, time);
}

/**
 * @brief get esp8266 mode
 * @param time - timeout time
 * @return esp8266 current work mode
 */
esp8266_mode esp8266_getmode(TickType_t time)
{
    assert_param(NULL != g_serial);

    at_node node;
    serial_putstring(g_serial, "AT+CWMODE_CUR?\r\n", 16);
    esp8266_mode mode = UNKNOWN;

    if (pdPASS == xQueueReceive(xRecvQueue, &node, time))
    {
        if (0 == strcmp((char *)node.data[node.size - 1], "OK"))
        {
            uint8_t len = strlen((char const *)node.data[0]);
            mode = (esp8266_mode)(node.data[0][len - 1] - '0');
        }
    }

    TRACE("send cmd: AT+CWMODE_CUR?\n");
    TRACE("mode: %d\n", mode);
    return mode;
}

/**
 * @brief connect ap
 * @param ssid - ap ssid
 * @param pwd - ap password
 * @param time - connect timeout time
 * @return 0 means connect success, otherwise failed
 */
int esp8266_connect_ap(const char *ssid, const char *pwd, TickType_t time)
{
    assert_param(NULL != g_serial);

    at_node node;
    char str_mode[64];
    sprintf(str_mode, "AT+CWJAP_DEF=%s,%s\r\n", ssid, pwd);
    serial_putstring(g_serial, str_mode, strlen(str_mode));
    int ret = -ESP_ERR_FAIL;

    if (pdPASS == xQueueReceive(xRecvQueue, &node, time))
    {
        if (0 == strcmp((char *)node.data[node.size - 1], "OK"))
        {
            ret = ESP_ERR_OK;
        }
        else if (0 == strcmp((char *)node.data[node.size - 1], "FAIL"))
        {
            uint8_t len = strlen((char const *)node.data[0]);
            ret = -(node.data[0][len - 1] - '0');
        }
    }
    else
    {
        ret = -ESP_ERR_TIMEOUT;
    }

    TRACE("send cmd: %s", str_mode);
    TRACE("status: %d\n", -ret);
    return ret;
}

/**
 * @brief disconnect ap
 * @param time - disconnect timeout time
 */
void esp8266_disconnect_ap(TickType_t time)
{
    assert_param(NULL != g_serial);

    esp8266_send_ok("AT+CWQAP\r\n", time);
}

/**
 * @brief set software ap parameter
 * @param ssid - ap ssid
 * @param pwd - ap password
 * @param chl - ap channel
 * @param ecn - password encode type
 * @return 0 means connect success, otherwise failed
 */
int esp8266_set_softap(const char *ssid, const char *pwd, uint8_t chl, uint8_t ecn, TickType_t time)
{
    assert_param(NULL != g_serial);

    at_node node;
    char str_mode[128];
    sprintf(str_mode, "AT+CWSAP_CUR=%s,%s,%d,%d\r\n", ssid, pwd, chl, ecn);
    serial_putstring(g_serial, str_mode, strlen(str_mode));
    int ret = -ESP_ERR_FAIL;

    if (pdPASS == xQueueReceive(xRecvQueue, &node, time))
    {
        if (0 == strcmp((char *)node.data[node.size - 1], "OK"))
        {
            ret = ESP_ERR_OK;
        }
    }
    else
    {
        ret = -ESP_ERR_TIMEOUT;
    }

    TRACE("send cmd: %s", str_mode);
    TRACE("status: %d\n", -ret);
    return ret;
}

/**
 * @brief connect remote server
 * @param mode - connect mode
 * @param ip - remote ip address
 * @param port - remote port
 * @param time - timeout time
 */
int esp8266_connect(esp8266_connectmode mode, const char *ip, uint16_t port,
                    TickType_t time)
{
    assert_param(NULL != g_serial);

    at_node node;
    char str_mode[128];
    sprintf(str_mode, "AT+CIPSTART=%d,%s,%d\r\n", mode, ip, port);
    serial_putstring(g_serial, str_mode, strlen(str_mode));
    int ret = -ESP_ERR_FAIL;

    if (pdPASS == xQueueReceive(xRecvQueue, &node, time))
    {
        if (0 == strcmp((char *)node.data[node.size - 1], "OK"))
        {
            ret = ESP_ERR_OK;
        }
        else if (0 == strcmp((char *)node.data[node.size - 1], "ALREADY CONNECTED"))
        {
            ret = -ESP_ERR_ALREADY;
        }
    }
    else
    {
        ret = -ESP_ERR_TIMEOUT;
    }

    TRACE("send cmd: %s", str_mode);
    TRACE("status: %d\n", -ret);
    return ret;
}

/**
 * @brief set esp8266 transport mode
 * @param mode - transport mode
 * @param time - timeout time
 */
int esp8266_set_transmode(esp8266_transmode mode, TickType_t time)
{
    assert_param(NULL != g_serial);
    
    char str_mode[32];
    sprintf(str_mode, "AT+CIPMODE=%d\r\n", mode);

    return esp8266_send_ok(str_mode, time);
}



