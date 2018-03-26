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
#include "semphr.h"
#include "esp8266.h"
#include "serial.h"
#include "global.h"

/* esp8266 work in block mode */

/* serial handle */
serial *g_serial = NULL;

/* recive queue */
static xQueueHandle xRecvQueue = NULL;
#define ESP_MAX_MSG_NUM      (1)
#define ESP_MAX_MSG_SIZE     (32)

/* timeout time(ms) */
#define DEFAULT_TIMEOUT      (3000 / portTICK_PERIOD_MS)

typedef enum
{
    AT,
    SAT,
}esp8266_state;

/**
 * @brief pms5003 data process task
 * @param serial handle
 */
static void vESP8266Response(void *pvParameters)
{
    const TickType_t xDelay = 3 / portTICK_PERIOD_MS;
    serial *pserial = pvParameters;
    uint8_t data_package[36];
    uint8_t *pData = data_package;
    char data;
    uint8_t cnt = 0;
    for (;;)
    {
        if(serial_getchar(pserial, &data, portMAX_DELAY))
        {
            *pData++ = data;
            cnt++;
            while(serial_getchar(pserial, &data, xDelay))
            {
                *pData++ = data;
                cnt++;
                if(cnt >= ESP_MAX_MSG_SIZE)
                {
                    break;
                }
            }

            /* get done, process data */
            data_package[cnt - 1] = 0;
            xQueueSend(xRecvQueue, data_package, portMAX_DELAY);
            pData = data_package;
            cnt = 0;
        }
    }                                                                                                                                                                                                            
}

/**
 * @brief get serial handle
 * @return serial handle
 */
static __INLINE serial *get_serial(void)
{
    return g_serial;
}

/**
 * @brief initialize esp8266
 * @return 0 means success, otherwise error code
 */
bool esp8266_init(void)
{
    g_serial = serial_request(COM1);
    if (NULL == g_serial)
    {
        return FALSE;
    }

    xRecvQueue = xQueueCreate(ESP_MAX_MSG_NUM, ESP_MAX_MSG_SIZE);
    if (NULL == xRecvQueue)
    {
        vPortFree(g_serial);
        g_serial = NULL;
        return FALSE;
    }

    xTaskCreate(vESP8266Response, "ESP8266Response", ESP8266_STACK_SIZE, 
                g_serial, ESP8266_PRIORITY, NULL);  

    return 0;
}

/**
 * @brief test esp8266 module
 * @param cmd - cmd to send
 * @param time - timeout time
 * @return TRUE: send success
 *         FALSE: send failed
 */
static bool esp8266_send_ok(const char *cmd, TickType_t time)
{
    assert_param(NULL != g_serial);

    uint8_t recv_buf[ESP_MAX_MSG_SIZE];
    serial_putstring(g_serial, cmd, strlen(cmd));

    if (pdPASS == xQueueReceive(xRecvQueue, recv_buf, time))
    {
        recv_buf[ESP_MAX_MSG_SIZE - 1] = 0;
        return strcmp((char *)recv_buf, "OK") ? FALSE : TRUE;
    }

    return FALSE;
}

/**
 * @brief write data
 * @param data - data
 * @param length - data length
 */
void esp8266_send(const char *data, uint32_t length)
{
    serial_putstring(get_serial(), data, length);
}

