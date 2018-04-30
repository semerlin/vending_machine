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
#include "mode.h"
#include "flash.h"

#undef __TRACE_MODULE
#define __TRACE_MODULE  "[wifi]"

#define DEFAULT_TIMEOUT      (3000 / portTICK_PERIOD_MS)

static char g_ssid[32];
static char g_pwd[32];

/* mqtt topic */
#define TOPIC_CONTROL     "controller/"
#define TOPIC_STATE       "state/"
#define TOPIC_REGISTER    "register"

uint8_t g_id[25];

/* mqtt information */
#define MQTT_ID        2
#define MQTT_ADDRESS   "139.196.137.228"
#define MQTT_PORT      1883

/* mqtt status */
uint8_t mqtt_status = 0x00;

bool ap_connected = FALSE;

static uint16_t g_motor_num = 0;

#define LED_AP            (1)
#define LED_MQTT          (2)
#define FLASH_INTERVAL    (500 / portTICK_PERIOD_MS)

static TaskHandle_t xConnectMqttTask = NULL;
static TaskHandle_t xHeartTask = NULL; 
static TaskHandle_t xMotorStateTask = NULL; 
static TaskHandle_t xConnectApTask = NULL; 


/**
 * @brief connedted default process function
 */
static void esp8266_ap_connect(void)
{
    led_net_stop_flashing(LED_AP);
    led_net_turn_on(LED_AP);
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
    led_net_turn_off(LED_MQTT);
    ap_connected = FALSE;
    mqtt_notify_disconnect();
    mqtt_status = 0x00;
}

/**
 * @brief connedted default process function
 */
static void esp8266_server_connect(uint8_t id)
{
    if (MQTT_ID == id)
    {
        mqtt_notify_connect(id);
        mqtt_status = 0x01;
    }
}

/**
 * @brief connedted default process function
 */
static void esp8266_server_disconnect(uint8_t id)
{
    if (MQTT_ID == id)
    {
        mqtt_notify_disconnect();
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
    driver.server_connect = m26_server_connect;
    driver.server_disconnect = m26_server_disconnect;
    m26_attach(&driver);
}

/**
 * @brief ap process task
 */
static void vConnectAp(void *pvParameters)
{
    led_net_flashing(LED_AP, FLASH_INTERVAL);
    for (;;)
    {
        if (FALSE == ap_connected)
        {
            TRACE("connect ap:%s, %s\r\n", g_ssid, g_pwd);
            if(ESP_ERR_OK == esp8266_connect_ap(g_ssid, g_pwd, 
                                                20000 / portTICK_PERIOD_MS))
            {
                ap_connected = TRUE;
            }
        }
        else
        {
            vTaskDelay(3000 / portTICK_PERIOD_MS);
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
            vTaskDelay(3000 / portTICK_PERIOD_MS);
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
        else
        {
            vTaskDelay(3000 / portTICK_PERIOD_MS);
        }
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
        vTaskDelay(9000 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief connack default process function
 */
static void mqtt_connack_cb(uint8_t status)
{
    if (MQTT_ERR_OK == status)
    {
        led_net_stop_flashing(LED_MQTT);
        mqtt_status |= 0x02;
        /* register sn */
        mqtt_publish(TOPIC_REGISTER, (const char *)g_id, 0, 1, 0);

        /* subscribe topic */
        mqtt_subscribe(TOPIC_CONTROL, 2);
    }
}

/**
 * @brief publish callback
 */
static void mqtt_publish_cb(const char *topic, uint8_t *data, uint32_t len)
{
    assert_param(len >= 1);
    g_motor_num = *data;
}

/**
 * @brief connack default process function
 */
static void mqtt_puback_cb(uint16_t id)
{
    /* register ack */
    TRACE("register sn \'%s\' success\r\n", g_id);
}

/**
 * @brief connack default process function
 */
static void mqtt_pubrel_cb(uint16_t id)
{
    assert_param(g_motor_num < 10);
    motor_start(g_motor_num);
}

/**
 * @brief connack default process function
 */
static void mqtt_suback_cb(uint8_t status, uint16_t id)
{
    if (MQTT_SUB_ERR == status)
    {
        if (0x03 == mqtt_status)
        {
            /* try to subscribe again */
            mqtt_subscribe(TOPIC_CONTROL, 2);
        }
    }
}

/**
 * @brief initialize mqtt driver
 */
static void init_mqtt_driver(void)
{
    mqtt_driver driver;
    driver.connack = mqtt_connack_cb;
    driver.publish = mqtt_publish_cb;
    driver.puback = mqtt_puback_cb;
    driver.pubrel = mqtt_pubrel_cb;
    driver.suback = mqtt_suback_cb;
    driver.pubrec = NULL;
    driver.pubcomp = NULL;
    driver.unsuback = NULL;
    driver.pingresp = NULL;
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
 * @brief init parameter
 */
static void init_param(void)
{
    mqtt_status = 0x00;
    ap_connected = FALSE;
    g_motor_num = 0;
}

/**
 * @brief init wifi
 * @return init status
 */
bool wifi_init(void)
{
    TRACE("initialize wifi...\r\n");
    init_param();
    flash_get_ssid_pwd(g_ssid, g_pwd);
    if (ESP_ERR_OK != esp8266_setmode(SAT))
    {
        return FALSE;
    }
    
    init_mqtt_driver();
    if (MODE_NET_WIFI == mode_net())
    {
        init_esp8266_driver();
    }
    else
    {
        init_m26_driver();
    }
    
    xTaskCreate(vConnectMqtt, "connectmqtt", AP_STACK_SIZE, NULL, 
                       AP_PRIORITY, &xConnectMqttTask);
    xTaskCreate(vHeart, "heart", AP_STACK_SIZE, NULL, 
                        AP_PRIORITY, &xHeartTask);
    xTaskCreate(vMotorState, "motorstate", AP_STACK_SIZE, NULL, 
                       AP_PRIORITY, &xMotorStateTask);
    if ((NULL == xConnectMqttTask) ||
        (NULL == xHeartTask) ||
        (NULL == xMotorStateTask))
    {
        return FALSE;
    }
    convert_chipid();

    if (MODE_NET_WIFI == mode_net())
    {
        xTaskCreate(vConnectAp, "connectap", AP_STACK_SIZE, NULL, 
                        AP_PRIORITY, &xConnectApTask); 
        if (NULL == xConnectApTask)
        {
            return FALSE;
        }
    }
    
    return TRUE;
}

/**
 * @brief deinit wifi module
 */
void wifi_deinit(void)
{
    TRACE("deinit wifi module...\r\n");
    
    if (NULL != xConnectMqttTask)
    {
        vTaskDelete(xConnectMqttTask);
        xConnectMqttTask = NULL;
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    if (NULL != xConnectApTask)
    {
        vTaskDelete(xConnectApTask);
        xConnectApTask = NULL;
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    if (NULL != xMotorStateTask)
    {
        vTaskDelete(xMotorStateTask);
        xMotorStateTask = NULL;
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    if (NULL != xHeartTask)
    {
        vTaskDelete(xHeartTask);
        xHeartTask = NULL;
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    esp8266_send_ok("AT+CWQAP\r\n");
}

