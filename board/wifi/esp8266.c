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
static xQueueHandle xStatusQueue = NULL;
static xQueueHandle xTcpQueue = NULL;
static xQueueHandle xAtQueue = NULL;

#define ESP_MAX_NODE_NUM              (4)
#define ESP_MAX_MSG_SIZE_PER_LINE     (64)

typedef struct
{
    uint8_t size;
    uint8_t data[ESP_MAX_MSG_SIZE_PER_LINE];
}at_node;

/* timeout time(ms) */
#define DEFAULT_TIMEOUT      (3000 / portTICK_PERIOD_MS)

/**
 * @brief send at command
 * @param cmd - at command
 * @param length - command length
 */
static void send_at_cmd(const char *cmd, uint32_t length)
{
    xQueueReset(xStatusQueue);
    xQueueReset(xAtQueue);
    TRACE("send: %s", cmd);
    serial_putstring(g_serial, cmd, length);
}

/**
 * @brief process line data
 * @param data - line data
 */
static void process_line(const at_node *node)
{
    
    uint8_t status;
    if (node->size > 0)
    {
        if (0 == strcmp((const char *)node->data, "OK"))
        {
            /* at command status */
            status = ESP_ERR_OK;
            xQueueSend(xStatusQueue, &status, portMAX_DELAY);
        }
        else if (0 == strcmp((const char *)node->data, "FAIL"))
        {
            /* at command status */
            TRACE("data = FAIL\n");
            status = ESP_ERR_FAIL;
            xQueueSend(xStatusQueue, &status, portMAX_DELAY);
        }
        else if (0 == strcmp((const char *)node->data, "ERROR"))
        {
            /* at command status */
            status = ESP_ERR_FAIL;
            xQueueSend(xStatusQueue, &status, portMAX_DELAY);
        }
        else if (0 == strcmp((const char *)node->data, "ALREADY CONNECTED"))
        {
            /* at command status */
            status = ESP_ERR_ALREADY;
            xQueueSend(xStatusQueue, &status, portMAX_DELAY);
        }
        else
        {
            /* at command parameter */
            xQueueSend(xAtQueue, node->data, portMAX_DELAY);
        }
    }
}

/**
 * @brief pms5003 data process task
 * @param serial handle
 */
static void vESP8266Response(void *pvParameters)
{
    const TickType_t xDelay = 3 / portTICK_PERIOD_MS;
    serial *pserial = pvParameters;
    at_node node;
    node.size = 0;
    uint8_t *pData = node.data;
    char data;
    /* skip startup data */
    for (;;)
    {
        if (serial_getchar(pserial, &data, portMAX_DELAY))
        {
            *pData++ = data;
            node.size ++; 
#if 0
            if (0)
            //if ('+' == data)
            {
                /* tcp data */
                while (serial_getchar(pserial, &data, xDelay))
                {
                    *pData++ = data;
                    node.size ++;
                    assert_param(node.size < ESP_MAX_MSG_SIZE_PER_LINE);

                    *pData = 0x00;
                    if (0 == strcmp((const char *)(pData - 2), "\r\n"))
                    {
                        /* get line data */
                        if (0 == strcmp((const char *)(pData - 4), "OK\r\n"))
                        {
                            /* get done */
                            break;
                        }
                        pData = node.data;
                    }
                }
            }
            else
#endif
            {
                /* at command data */
                while (serial_getchar(pserial, &data, xDelay))
                {
                    *pData++ = data;
                    node.size ++;
                    assert_param(node.size < ESP_MAX_MSG_SIZE_PER_LINE);

                    if (0 == strncmp((const char *)(pData - 2), "\r\n", 2))
                    {
                        /* get line data */
                        node.data[node.size - 2] = '\0';
                        node.size -= 2;
                        process_line(&node);
                        pData = node.data;
                        node.size = 0;
                    }
                }
            }
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
    vTaskDelay(100 / portTICK_PERIOD_MS);
    pin_set("WIFI_EN");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    g_serial = serial_request(COM2);
    if (NULL == g_serial)
    {
        TRACE("initialize failed, can't open serial \'COM2\'\n");
        return FALSE;
    }
    serial_open(g_serial);

    xStatusQueue = xQueueCreate(ESP_MAX_NODE_NUM, ESP_MAX_NODE_NUM);
    xAtQueue = xQueueCreate(ESP_MAX_NODE_NUM, ESP_MAX_MSG_SIZE_PER_LINE);
    xTcpQueue = xQueueCreate(ESP_MAX_NODE_NUM, sizeof(at_node) / sizeof(uint8_t));
    if ((NULL == xStatusQueue) || (NULL == xAtQueue) || (NULL == xTcpQueue))
    {
        TRACE("initialize failed, can't create queue\'COM2\'\n");
        serial_release(g_serial);
        g_serial = NULL;
        return FALSE;
    }
    
    xTaskCreate(vESP8266Response, "ESP8266Response", ESP8266_STACK_SIZE, 
            g_serial, ESP8266_PRIORITY, NULL);
     
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

    uint8_t status;
    send_at_cmd(cmd, strlen(cmd));
    int ret = ESP_ERR_OK;

    if (pdPASS == xQueueReceive(xStatusQueue, &status, time))
    {
        ret = -status;
    }
    else
    {
        ret = -ESP_ERR_TIMEOUT;
    }

    if (0 != ret)
    {
        TRACE("status: %d\n", -ret);
    }
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

    uint8_t status;
    uint8_t buf[ESP_MAX_MSG_SIZE_PER_LINE];
    buf[0] = UNKNOWN + '0';
    send_at_cmd("AT+CWMODE_CUR?\r\n", 16);
    esp8266_mode mode = UNKNOWN;

    if (pdPASS == xQueueReceive(xStatusQueue, &status, time))
    {
        if (ESP_ERR_OK == status)
        {
            xQueueReceive(xAtQueue, buf, 0);
            mode = (esp8266_mode)(buf[0] - '0');
        }
    }

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

    char str_mode[64];
    sprintf(str_mode, "AT+CWJAP_CUR=%s,%s\r\n", ssid, pwd);
    send_at_cmd(str_mode, strlen(str_mode));
    int ret = -ESP_ERR_FAIL;

    uint8_t status;
    uint8_t buf[ESP_MAX_MSG_SIZE_PER_LINE];
    buf[0] = ESP_ERR_FAIL + '0';
    if (pdPASS == xQueueReceive(xStatusQueue, &status, time))
    {
        if (ESP_ERR_OK != status)
        {
            xQueueReceive(xAtQueue, buf, 0);
            ret = -(buf[0] - '0');
        }
    }
    else
    {
        ret = -ESP_ERR_TIMEOUT;
    }

    if (0 != ret)
    {
        TRACE("status: %d\n", -ret);
    }
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
int esp8266_set_softap(const char *ssid, const char *pwd, uint8_t chl, 
                       esp8266_ecn ecn, TickType_t time)
{
    assert_param(NULL != g_serial);

    char str_mode[64];
    sprintf(str_mode, "AT+CWSAP_CUR=\"%s\",\"%s\",%d,%d\r\n", 
            ssid, pwd, chl, ecn);
    
    return esp8266_send_ok(str_mode, time);
}

/**
 * @brief set software ap address
 * @param ssid - ap ssid
 * @param pwd - ap password
 * @param chl - ap channel
 * @param ecn - password encode type
 * @return 0 means connect success, otherwise failed
 */
int esp8266_set_apaddr(const char *ip, const char *gateway, const char *netmask,
                       TickType_t time)
{
    assert_param(NULL != g_serial);

    char str_mode[64];
    sprintf(str_mode, "AT+CIPAP_CUR=\"%s\",\"%s\",\"%s\"\r\n", 
            ip, gateway, netmask);
    
    return esp8266_send_ok(str_mode, time);
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

    char str_mode[128];
    sprintf(str_mode, "AT+CIPSTART=%d,%s,%d\r\n", mode, ip, port);
    
    return esp8266_send_ok(str_mode, time);
}

/**
 * @brief set esp8266 transport mode
 * @param mode - transport mode
 * @param time - timeout time
 * @return error code
 */
int esp8266_set_transmode(esp8266_transmode mode, TickType_t time)
{
    assert_param(NULL != g_serial);
    
    char str_mode[16];
    sprintf(str_mode, "AT+CIPMODE=%d\r\n", mode);

    return esp8266_send_ok(str_mode, time);
}

/**
 * @brief create server
 * @param port - listen port
 * @param time - timeout time
 * @return error code
 */
int esp8266_listen(uint16_t port, TickType_t time)
{
    assert_param(NULL != g_serial);
    
    char str_mode[24];
    sprintf(str_mode, "AT+CIPSERVER=1,%d\r\n", port);

    return esp8266_send_ok(str_mode, time);
}

/* @brief close server
 * @param time - timeout time
 * @return error code
 */
int esp8266_close(uint16_t port, TickType_t time)
{
    assert_param(NULL != g_serial);
    
    char str_mode[24];
    sprintf(str_mode, "AT+CIPSERVER=0,%d\r\n", port);

    return esp8266_send_ok(str_mode, time);
}


