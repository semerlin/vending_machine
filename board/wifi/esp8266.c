/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include <errno.h>
#include "esp8266.h"
#include "serial.h"

/* esp8266 work in block mode */

/* serial handle */
serial *g_serial = NULL;

/* recive queue */
static xQueueHandle xRecvQueue = NULL;
#define ESP_MAX_MSG_NUM      (1)
#define ESP_MAX_MSG_SIZE     (32)

/* timeout time(ms) */
#define DEFAULT_TIMEOUT      (3000)

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
    Handle serial = pvParameters;
    uint8_t data_package[36];
    uint8_t *pData = dataPackage;
    char data;
    uint8 cnt = 0;
    PM_Data pmData;
    Sensor_Info sensorInfo = {PMS5003S , 0};
    uint32 prevValue = 0;
    for (;;)
    {
        if(serial_getchar(serial, &data, portMAX_DELAY))
        {
            *pData++ = data;
            cnt++;
            while(serial_getchar(serial, &data, xDelay))
            {
                *pData++ = data;
                cnt++;
                if(cnt >= 32)
                    break;
            }

            /* get done, process data */

            pData = dataPackage;
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
int esp8266_init(void)
{
    g_serial = serial_request(COM1);
    if (NULL == g_serial)
    {
        return -ENOMEM;
    }

    xRecvQueue = xQueueCreate(ESP_MAX_MSG_NUM, ESP_MAX_MSG_SIZE);
    if (NULL == xRecvQueue)
    {
        vPortFree(g_serial);
        g_serial = NULL;
        return -ENOMEM;
    }

    xTaskCreate(vESP8266Response, "ESP8266Response", ESP8266_STACK_SIZE, g_serial, ESP8266_PRIORITY, NULL);  

    return 0;
}


/**
 * @brief test esp8266 module
 * @return TRUE: test passed
 *         FALSE: test failed
 */
bool esp8266_test(void)
{
    assert_param(NULL != g_serial);

    uint8_t recv_buf[ESP_MAX_MSG_SIZE];
    serial_putstring(g_serial, "AT", 2);

    if (pdPASS == xQueueReceive(xRecvQueue, recv_buf, DEFAULT_TIMEOUT / portTICK_PERIOD_MS))
    {
        recv_buf[ESP_MAX_MSG_SIZE - 1] = 0;
        return strcpy(recv_buf, "OK") ? FALSE : TRUE;
    }

    return FALSE;
}


