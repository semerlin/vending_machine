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
#include "queue.h"
#include "wifi.h"
#include "esp8266.h"
#include "m26.h"
#include "global.h"
#include "trace.h"
#include "mqtt.h"
#include "stm32f10x_cfg.h"
#include "motorctl.h"
#include "led_net.h"

#undef __TRACE_MODULE
#define __TRACE_MODULE  "[wifi]"

#define DEFAULT_TIMEOUT      (3000 / portTICK_PERIOD_MS)
#define TRY_COUNT             3

/* mqtt topic */
#define TOPIC_CONTROL     "controller"
#define TOPIC_STATE       "state"

static xQueueHandle xApInfoQueue = NULL;
typedef struct
{
    char name[32];
    char password[32];
}ap_info;

uint8_t g_id[25];

/* mqtt information */
#define MQTT_ID        2
#define MQTT_ADDRESS   "139.196.137.228"
#define MQTT_PORT      1883

/* mqtt status */
uint8_t mqtt_status = 0x00;

bool ap_connected = FALSE;

#define LED_AP            (1)
#define LED_MQTT          (2)
#define FLASH_INTERVAL    (500 / portTICK_PERIOD_MS)

/**
 * @brief connedted default process function
 */
static void esp8266_ap_connect(void)
{
    led_net_stop_flashing(LED_AP);
    led_net_flashing(LED_MQTT, FLASH_INTERVAL);
    ap_connected = TRUE;
}

/**
 * @brief connedted default process function
 */
static void esp8266_ap_disconnect(void)
{
    led_net_flashing(LED_AP, FLASH_INTERVAL);
    led_net_stop_flashing(LED_MQTT);
    ap_connected = FALSE;
    mqtt_status = 0x00;
}

/**
 * @brief connedted default process function
 */
static void esp8266_server_connect(uint16_t id)
{
    if (MQTT_ID == id)
    {
        mqtt_status = 0x01;
    }
}

/**
 * @brief connedted default process function
 */
static void esp8266_server_disconnect(uint16_t id)
{
    if (MQTT_ID == id)
    {
        mqtt_status = 0x00;
    }
}

/**
 * @brief initialize esp8266 default driver
 */
static void init_esp8266_driver(void)
{
    esp8266_driver driver;
    driver.ap_connect = esp8266_ap_connect;
    driver.ap_disconnect = esp8266_ap_disconnect;
    driver.server_connect = esp8266_server_connect;
    driver.server_disconnect = esp8266_server_disconnect;
    esp8266_attach(&driver);
}

/**
 * @brief m26 network register process function
 */
static void m26_net_register(uint8_t code)
{
}

/**
 * @brief m26 gprs process function
 */
static void m26_gprs_attach(uint8_t code)
{
}

/**
 * @brief m26 server connect process function
 */
static void m26_server_connect(void)
{
}

/**
 * @brief m26 server disconnect process function
 */
static void m26_server_disconnect(void)
{
}

/**
 * @brief initialize esp8266 default driver
 */
static void init_m26_driver(void)
{
    m26_driver driver;
    driver.net_register = m26_net_register;
    driver.gprs_attach = m26_gprs_attach;
    driver.server_connect = m26_server_connect();
    driver.server_disconnect = m26_server_disconnect();
    m26_attach(&driver);
}


/**
 * @brief ap process task
 */
static void vConnectAp(void *pvParameters)
{
    ap_info info;
    uint8_t count = 0;
    led_net_flashing(LED_AP, FLASH_INTERVAL);
    for (;;)
    {
        count = 0;
        xQueueReceive(xApInfoQueue, &info, portMAX_DELAY);
        TRACE("connect ap:%s, %s\r\n", info.name, info.password);
        while(ESP_ERR_OK != esp8266_connect_ap(info.name, info.password, 
                                               20000 / portTICK_PERIOD_MS))
        {
            count++;
            if (count >= TRY_COUNT)
            {
                ap_connected = FALSE;
                TRACE("connect to ap \"%s\" failed\r\n", info.name);
                break;
            }
        }
        
        if (count < TRY_COUNT)
        {
            ap_connected = TRUE;
            TRACE("connect to ap \"%s\" success\r\n", info.name);
        }
    }
}

/**
 * @brief heart beat process task
 */
static void vHeart(void *pvParameters)
{
    for (;;)
    {
        if (0x03 == mqtt_status)
        {
            mqtt_pingreq();
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief ap process task
 */
static void vConnectMqtt(void *pvParameters)
{
    for (;;)
    {
        if (ap_connected)
        {
            switch (mqtt_status)
            {
            case 0x00:
                mqtt_connect_server(MQTT_ID, MQTT_ADDRESS, MQTT_PORT);
                break;
            case 0x01:
                {
                /* connect mqtt */
                connect_param param;
                param.flag.flag = 0x02;
                param.client_id = (const char *)g_id;
                param.alive_time = 8;
                mqtt_connect(&param);
                }
                break;
            default:
                break;
            }
        }
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief motor state process task
 */
static void vMotorState(void *pvParameters)
{
    uint16_t status = 0;
    uint8_t status_str[3] = {0x00, 0x00, 0x00};
    for (;;)
    {
        if (0x03 == mqtt_status)
        {
            status = motor_getstatus();
            status_str[0] = (status >> 8);
            status_str[1] = (status & 0xff);
            mqtt_publish(TOPIC_STATE, (const char *)status_str, 0, 0, 0);
        }
        
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief connack default process function
 */
static void mqtt_connack(uint8_t status)
{
    if (MQTT_ERR_OK == status)
    {
        mqtt_status |= 0x02;
        /* subscribe topic */
        mqtt_subscribe(TOPIC_CONTROL, 2);
    }
}

/**
 * @brief connack default process function
 */
static void mqtt_puback(uint16_t id)
{
    UNUSED(id);
}

/**
 * @brief connack default process function
 */
static void mqtt_pubrec(uint16_t id)
{
    UNUSED(id);
}

/**
 * @brief connack default process function
 */
static void mqtt_pubrel(uint16_t id)
{
    UNUSED(id);
}

/**
 * @brief connack default process function
 */
static void mqtt_pubcomp(uint16_t id)
{
    UNUSED(id);
}

/**
 * @brief connack default process function
 */
static void mqtt_suback(uint8_t status, uint16_t id)
{
    if (MQTT_SUB_ERR == status)
    {
    }
}

/**
 * @brief connack default process function
 */
static void mqtt_unsuback(uint16_t id)
{
    UNUSED(id);
}

/**
 * @brief connack default process function
 */
static void mqtt_pingresp(void)
{
}

/**
 * @brief initialize mqtt driver
 */
static void init_mqtt_driver(void)
{
    mqtt_driver driver;
    driver.connack = mqtt_connack;
    driver.puback = mqtt_puback;
    driver.pubrec = mqtt_pubrec;
    driver.pubrel = mqtt_pubrel;
    driver.pubcomp = mqtt_pubcomp;
    driver.suback = mqtt_suback;
    driver.unsuback = mqtt_unsuback;
    driver.pingresp= mqtt_pingresp;
    mqtt_attach(&driver);
}


/**
 * @brief convert chip id to string format
 */
static void convert_chipid(void)
{
    uint32_t id[3];
    uint8_t len = 0;
    uint8_t data = 0;
    uint8_t up = 0, low = 0;
    Get_ChipID(id, &len);

    for (int i = 0; i < len; ++i)
    {
        for(int j = 0; j < 4; j++)
        {
            data = ((id[i] & 0xff000000) >> 24);
            up = data / 16;
            low = data % 16;
            if (up < 10)
            {
                g_id[i * 8 + j * 2] = up + '0';
            }
            else
            {
                g_id[i * 8 + j * 2] = up + 55;
            }
            
            if (low < 10)
            {
                g_id[i * 8 + j * 2 + 1] = low + '0';
            }
            else
            {
                g_id[i * 8 + j * 2 + 1] = low + 55;
            }
           
            id[i] <<= 8;
        }
    }

    g_id[24] = 0x00;
}

/**
 * @brief init wifi
 * @return init status
 */
int wifi_init(void)
{
    TRACE("initialize wifi...\r\n");
    init_mqtt_driver();
    init_esp8266_driver();
    init_m26_driver();
    xApInfoQueue = xQueueCreate(1, sizeof(ap_info) / sizeof(char));
    xTaskCreate(vConnectAp, "connectap", AP_STACK_SIZE, NULL, 
                       AP_PRIORITY, NULL);
    xTaskCreate(vHeart, "heart", AP_STACK_SIZE, NULL, 
                       AP_PRIORITY, NULL);
    xTaskCreate(vConnectMqtt, "connectmqtt", AP_STACK_SIZE, NULL, 
                       AP_PRIORITY, NULL);
    xTaskCreate(vMotorState, "motorstate", AP_STACK_SIZE, NULL, 
                       AP_PRIORITY, NULL);
    convert_chipid();
    return 0;
}

/**
 * @brief connect ap
 * @param ssid - ap name
 * @param pwd - ap password
 */
void wifi_connect_ap(const char *ssid, const char *pwd)
{
    assert_param(NULL != xApInfoQueue);
    ap_info info;
    strcpy(info.name, ssid);
    strcpy(info.password, pwd);
    xQueueOverwrite(xApInfoQueue, &info);
}



