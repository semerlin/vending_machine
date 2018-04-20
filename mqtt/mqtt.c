/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include <string.h>
#include "mqtt.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "esp8266.h"
#include "trace.h"
#include "global.h"
#include "assert.h"


#undef __TRACE_MODULE
#define __TRACE_MODULE  "[mqtt]"

/* mqtt driver */
mqtt_driver g_driver;

/* send and recive queue */
static xQueueHandle xSendQueue = NULL;
static xQueueHandle xRecvQueue = NULL;

#define MQTT_MAX_MSG_NUM     (6)
#define MQTT_MAX_MSG_SIZE    (128)

typedef struct
{
    uint8_t size;
    uint8_t data[MQTT_MAX_MSG_SIZE];
}mqtt_msg;

 
/* message type definition */
#define TYPE_CONNECT        (0x10)
#define TYPE_CONNACK        (0x20)
#define TYPE_PUBLISH        (0x30)
#define TYPE_PUBACK         (0x40)
#define TYPE_PUBREC         (0x50)
#define TYPE_PUBREL         (0x62)
#define TYPE_PUBCOMP        (0x70)
#define TYPE_SUBSCRIBE      (0x82)
#define TYPE_SUBACK         (0x90)
#define TYPE_UNSUBSCRIBE    (0xa2)
#define TYPE_UNSUBACK       (0xb0)
#define TYPE_PINGREQ        (0xc0)
#define TYPE_PINGRESP       (0xd0)
#define TYPE_DISCONNECT     (0xe0)

/* uuid */
static uint16_t g_uuid = 0;

/* connect */
const uint8_t protocol_name[6] = {0x00, 0x04, 'M', 'Q', 'T', 'T'};
#define PROTOCOL_LEVEL    (0x04)
connect_flag default_connect_flag = {0x00};

/* process function */
typedef void (*process_func)(const char *data, uint8_t len);

/* mqtt connect status */
static bool g_is_connected = FALSE;


/**
 * @brief connack default process function
 */
static void mqtt_connack(uint8_t status)
{
    UNUSED(status);
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
    UNUSED(status);
    UNUSED(id);
}

/**
 * @brief initialize mqtt default driver
 */
static void init_mqtt_driver(void)
{
    g_driver.connack = mqtt_connack;
    g_driver.puback = mqtt_puback;
    g_driver.pubrec = mqtt_pubrec;
    g_driver.pubrel = mqtt_pubrel;
    g_driver.pubcomp = mqtt_pubcomp;
    g_driver.suback = mqtt_suback;
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
 * @brief encode data length
 * @param data - data to encode
 * @param encode - encoded length
 * @return valid size
 */
static uint8_t encode_length(uint32_t data, uint8_t *encode)
{
    uint8_t encode_byte = 0;
    uint8_t size = 0;
    do
    {
        encode_byte = data % 128;
        data /= 128;
        if (data > 0)
        {
            encode_byte |= 0x80;
        }
        else
        {
            encode[size] = encode_byte;
            size++;
        }
    }while (data > 0);

    return size;
}

/**
 * @brief decode data length
 * @param decode - data to decode
 * @return data length 
 */
static uint32_t decode_length(uint8_t *decode)
{
    uint32_t multipiler = 0;
    uint32_t value = 0;
    do
    {
        decode ++;
        value += (*decode & 0x7f) * multipiler;
        multipiler *= 128;
        if (multipiler > 128 * 128 *128)
        {
            /* error happened */
            return 0xffffffff;
        }
    }while (0 != (*decode & 0x80));

    return value;
}

/**
 * @brief process connack information
 * @param data - data to process
 * @param len - data length
 */
void process_connack(const char *data, uint8_t len)
{
    if (len >= 4)
    {
        assert_param(decode_length((uint8_t *)(data + 1)) == 2);
        if (MQTT_ERR_OK == data[3])
        {
            g_is_connected = TRUE;
        }
        else
        {
            g_is_connected = FALSE;
        }
        g_driver.connack(data[3]);
    }
}

/**
 * @brief process puback information
 * @param data - data to process
 * @param len - data length
 */
void process_puback(const char *data, uint8_t len)
{
    if (len >= 4)
    {
        assert_param(decode_length((uint8_t *)(data + 1)) == 2);
        uint16_t uuid = data[2];
        uuid <<= 8;
        uuid += data[3];
        g_driver.puback(uuid);
        TRACE("PUBACK: %d\n", uuid);
    }
}

/**
 * @brief process pubrec information
 * @param data - data to process
 * @param len - data length
 */
void process_pubrec(const char *data, uint8_t len)
{
    if (len >= 4)
    {
        assert_param(decode_length((uint8_t *)(data + 1)) == 2);
        uint16_t uuid = data[2];
        uuid <<= 8;
        uuid += data[3];
        g_driver.pubrec(uuid);
        TRACE("PUBREC: %d\n", uuid);
    }
}

/**
 * @brief process pubrel information
 * @param data - data to process
 * @param len - data length
 */
void process_pubrel(const char *data, uint8_t len)
{
    if (len >= 4)
    {
        assert_param(decode_length((uint8_t *)(data + 1)) == 2);
        uint16_t uuid = data[2];
        uuid <<= 8;
        uuid += data[3];
        g_driver.pubrel(uuid);
        TRACE("PUBREL: %d\n", uuid);
    }
}

/**
 * @brief process pubcomp information
 * @param data - data to process
 * @param len - data length
 */
void process_pubcomp(const char *data, uint8_t len)
{
    if (len >= 4)
    {
        assert_param(decode_length((uint8_t *)(data + 1)) == 2);
        uint16_t uuid = data[2];
        uuid <<= 8;
        uuid += data[3];
        g_driver.pubcomp(uuid);
        TRACE("PUBCOMP: %d\n", uuid);
    }
}

/**
 * @brief process pubcomp information
 * @param data - data to process
 * @param len - data length
 */
void process_suback(const char *data, uint8_t len)
{
    if (len >= 4)
    {
        assert_param(decode_length((uint8_t *)(data + 1)) == 3);
        uint16_t uuid = data[2];
        uuid <<= 8;
        uuid += data[3];
        uint8_t status = data[4];
        g_driver.suback(status, uuid);
        TRACE("SUBACK: %d, %d\n", status, uuid);
    }
}

/**
 * @brief process unsuback information
 * @param data - data to process
 * @param len - data length
 */
void process_unsuback(const char *data, uint8_t len)
{
    if (len >= 4)
    {
        assert_param(decode_length((uint8_t *)(data + 1)) == 3);
        uint16_t uuid = data[2];
        uuid <<= 8;
        uuid += data[3];
        g_driver.unsuback(uuid);
        TRACE("UNSUBACK: %d\n", uuid);
    }
}

/**
 * @brief process pingresp information
 * @param data - data to process
 * @param len - data length
 */
void process_pingresp(const char *data, uint8_t len)
{
    g_driver.pingresp();
    TRACE("PINGRESP: 0\n");
}

/* function node */
typedef struct
{
    uint8_t type;
    process_func process;
}func_node;

func_node funcs[] = 
{
    {TYPE_CONNACK, process_connack},
    {TYPE_PUBACK, process_puback},
    {TYPE_PUBREC, process_pubrec},
    {TYPE_PUBREL, process_pubrel},
    {TYPE_PUBCOMP, process_pubcomp},
    {TYPE_SUBACK, process_suback},
    {TYPE_PINGRESP, process_pingresp},
};

/**
 * @brief mqtt send task
 * @param pvParameters - task parameters
 */
void vMqttSend(void *pvParameters)
{
    mqtt_msg msg;
    for (;;)
    {
        xQueueReceive(xSendQueue, &msg, portMAX_DELAY);
        esp8266_write((const char *)msg.data, msg.size, 
                      1000 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief mqtt receive task
 * @param pvParameters - task parameters
 */
void vMqttRecv(void *pvParameters)
{
    char data[65];
    uint16_t len;
    int count = sizeof(funcs) / sizeof(funcs[0]);
    for (;;)
    {
        if (ESP_ERR_OK == esp8266_recv(out, data, &len, portMAX_DELAY))
        {
            for (int i = 0; i < count; ++i)
            {
                if (funcs[i].type == data[0])
                {
                    funcs[i].process(data, len);
                    break;
                }
            }
        }
         
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief initialize mqtt protocol
 */
bool mqtt_init(void)
{
    g_is_connected = FALSE;
    xSendQueue = xQueueCreate(MQTT_MAX_MSG_NUM, sizeof(mqtt_msg) / sizeof(uint8_t));
    if (NULL == xSendQueue)
    {
        return FALSE;
    }

    xRecvQueue = xQueueCreate(MQTT_MAX_MSG_NUM, sizeof(mqtt_msg) / sizeof(uint8_t));
    if (NULL == xRecvQueue)
    {
        vPortFree(xSendQueue);
        return FALSE;
    }
    
    xTaskCreate(vMqttSend, "MqttSend", MQTT_STACK_SIZE, 
            NULL, MQTT_PRIORITY, NULL);
    xTaskCreate(vMqttRecv, "MqttRecv", MQTT_STACK_SIZE, 
            NULL, MQTT_PRIORITY, NULL);

    init_mqtt_driver();

    return TRUE;
}

/**
 * @brief refresh driver
 */
static void refresh_driver(void)
{
    if (NULL == g_driver.connack)
    {
        g_driver.connack = mqtt_connack;
    }

    if (NULL == g_driver.puback)
    {
        g_driver.puback = mqtt_puback;
    }

    if (NULL == g_driver.pubrec)
    {
        g_driver.pubrec = mqtt_pubrec;
    }

    if (NULL == g_driver.pubrel)
    {
        g_driver.pubrel = mqtt_pubrel;
    }

    if (NULL == g_driver.pubcomp)
    {
        g_driver.pubcomp = mqtt_pubcomp;
    }

    if (NULL == g_driver.suback)
    {
        g_driver.suback = mqtt_suback;
    }
    
    if (NULL == g_driver.unsuback)
    {
        g_driver.unsuback = mqtt_unsuback;
    }
    
    if (NULL == g_driver.pingresp)
    {
        g_driver.pingresp = mqtt_pingresp;
    }
}

/**
 * @brief attach mqtt driver
 * @param driver - mqtt driver handle
 */
void mqtt_attach(const mqtt_driver *driver)
{
    assert_param(NULL != driver);
    g_driver.connack = driver->connack;
    g_driver.puback = driver->puback;
    g_driver.pubrec = driver->pubrec;
    g_driver.pubrel = driver->pubrel;
    g_driver.pubcomp = driver->pubcomp;
    g_driver.suback = driver->suback;
    g_driver.unsuback = driver->unsuback;
    g_driver.pingresp = driver->pingresp;
    refresh_driver();
}

/**
 * @brief detach mqtt driver
 */
void mqtt_detach(void)
{
    init_mqtt_driver();
}

/**
 * @brief validation connect parameter
 * @param param - connect parameter
 */
static void check_connect_param(const connect_param *param)
{
    assert_param(NULL != param);

    if (NULL == param->client_id)
    {
        assert_param(0x01 == param->flag._flag.clear_session);
    }

    if (0x00 == param->flag._flag.will_flag)
    {
        assert_param(0 == param->flag._flag.will_qos);
        assert_param(0 == param->flag._flag.will_retain);
    }
    else
    {
        assert_param(param->flag._flag.will_qos < 0x03);
        assert_param(NULL != param->will_topic);
        assert_param(NULL != param->will_msg);
    }

    if (0x01 == param->flag._flag.username_flag)
    {
        assert_param(NULL != param->username);
    }

    if (0x01 == param->flag._flag.password_flag)
    {
        assert_param(NULL != param->password);
    }
}

/**
 * @brief calculate connect parameter length
 * @param param - connect parameter
 */
static uint32_t calculate_connect_payload_len(const connect_param *param)
{
    uint32_t payload_len = 10;
    if (NULL != param->client_id)
    {
        payload_len += (strlen(param->client_id) + 2);
    }

    if (0x01 == param->flag._flag.will_flag)
    {
        /* add will topic and will message */
        payload_len += (strlen(param->will_topic) + 2);
        payload_len += (strlen(param->will_msg) + 2);
    }

    if (0x01 == param->flag._flag.username_flag)
    {
        /* add username */
        payload_len += (strlen(param->username) + 2);
    }

    if (0x01 == param->flag._flag.password_flag)
    {
        /* add password */
        payload_len += (strlen(param->password) + 2);
    }

    return payload_len;
}

/**
 * @brief connect to mqtt server
 * @param ip - server ip address
 * @param port - server port number
 */
int mqtt_connect_server(uint16_t id, const char *ip, uint16_t port)
{
    return esp8266_connect(id, "TCP", ip, port, 3000 / portTICK_PERIOD_MS);
}

/**
 * @brief get mqtt connect status
 * @return connect status
 */
bool mqtt_is_connected(void)
{
    return g_is_connected;
}

/**
 * @brief connect mqtt server
 * @param param - connect parameter
 */
void mqtt_connect(const connect_param *param)
{
    check_connect_param(param);

    mqtt_msg msg;
    uint8_t *pdata = msg.data;
    uint16_t str_len = 0;
    uint32_t payload_len = 0;

    /* calculate payload length */
    payload_len = calculate_connect_payload_len(param);

    /* packet data */
    *pdata ++ = TYPE_CONNECT;
    uint8_t encode_len = encode_length(payload_len, pdata);
    assert_param((payload_len + encode_len + 1) <= MQTT_MAX_MSG_SIZE);
    pdata += encode_len;
    for (uint8_t i = 0; i < sizeof(protocol_name) / sizeof(uint8_t); ++i)
    {
        *pdata ++ = protocol_name[i];
    }
    *pdata ++ = PROTOCOL_LEVEL;
    *pdata ++ = param->flag.flag;
    *pdata ++ = (uint8_t)(param->alive_time >> 8);
    *pdata ++ = (uint8_t)(param->alive_time & 0xff);
    if (NULL != param->client_id)
    {
        str_len = strlen(param->client_id);
        *pdata ++ = (uint8_t)(str_len >> 8);
        *pdata ++ = (uint8_t)(str_len & 0xff);
        strcpy((char *)pdata, param->client_id);
        pdata += str_len;
    }
    else
    {
        *pdata ++ = 0x00;
        *pdata ++ = 0x00;
    }

    if (0x01 == param->flag._flag.will_flag)
    {
        /* add will topic and will message */
        str_len = strlen(param->will_topic);
        *pdata ++ = (uint8_t)(str_len >> 8);
        *pdata ++ = (uint8_t)(str_len & 0xff);
        strcpy((char *)pdata, param->will_topic);
        pdata += str_len;

        str_len = strlen(param->will_msg);
        *pdata ++ = (uint8_t)(str_len >> 8);
        *pdata ++ = (uint8_t)(str_len & 0xff);
        strcpy((char *)pdata, param->will_msg);
        pdata += str_len;
    }

    if (0x01 == param->flag._flag.username_flag)
    {
        /* add username */
        str_len = strlen(param->username);
        *pdata ++ = (uint8_t)(str_len >> 8);
        *pdata ++ = (uint8_t)(str_len & 0xff);
        strcpy((char *)pdata, param->username);
        pdata += str_len;
    }

    if (0x01 == param->flag._flag.password_flag)
    {
        /* add password */
        str_len = strlen(param->password);
        *pdata ++ = (uint8_t)(str_len >> 8);
        *pdata ++ = (uint8_t)(str_len & 0xff);
        strcpy((char *)pdata, param->password);
        pdata += str_len;
    }

    msg.size = payload_len + encode_len + 1;

    if (0x01 == param->flag._flag.clear_session)
    {
        /* clear unack qos1 and qos2 message*/
    }

    /* send message to queue */
    xQueueSend(xSendQueue, &msg, 0);
}

/**
 * @brief public content to topic
 * @param topic - topic to publish
 * @param content - content to publish
 */
void mqtt_publish(const char *topic, const char *content, uint8_t dup,
                  uint8_t qos, uint8_t retain)
{
    assert_param(NULL != topic);
    assert_param(NULL != content);
    mqtt_msg msg;
    uint8_t *pdata = msg.data;
    uint16_t str_len = 0;
    uint32_t payload_len = 0;

    /* calculate payload length */
    payload_len = strlen(topic) + 2 + strlen(content);
    if (0 != qos)
    {
        payload_len += 2;
    }

    /* packet data */
    *pdata = TYPE_PUBLISH;
    *pdata |= ((dup & 0x01) << 3);
    *pdata |= ((qos & 0x03) << 1);
    *pdata |= (retain & 0x01);
    
    uint8_t encode_len = encode_length(payload_len, pdata);
    assert_param((payload_len + encode_len + 1) <= MQTT_MAX_MSG_SIZE);
    pdata += encode_len;
    
    /* add topic */
    str_len = strlen(topic);
    *pdata ++ = (uint8_t)(str_len >> 8);
    *pdata ++ = (uint8_t)(str_len & 0xff);
    strcpy((char *)pdata, topic);
    pdata += str_len;
    if (0 != qos)
    {
        *pdata ++ = (uint8_t)(g_uuid >> 8);
        *pdata ++ = (uint8_t)(g_uuid & 0xff);
        g_uuid ++;
    }
    strcpy((char *)pdata, content);
    
    msg.size = payload_len + encode_len + 1;

    /* send message to queue */
    xQueueSend(xSendQueue, &msg, 0);
}

/**
 * @brief subscribe topic from server
 * @param topic - topic to subscribe
 * @param qos - topic qos
 */
void mqtt_subscribe(const char *topic, uint8_t qos)
{
    assert_param(NULL != topic);
    mqtt_msg msg;
    uint8_t *pdata = msg.data;
    uint16_t str_len = 0;
    uint32_t payload_len = 0;

    /* calculate payload length */
    payload_len = strlen(topic) + 5;

    /* packet data */
    *pdata = TYPE_SUBSCRIBE;
    
    uint8_t encode_len = encode_length(payload_len, pdata);
    assert_param((payload_len + encode_len + 1) <= MQTT_MAX_MSG_SIZE);
    pdata += encode_len;
    
    /* add uuid */
    *pdata ++ = (uint8_t)(g_uuid >> 8);
    *pdata ++ = (uint8_t)(g_uuid & 0xff);
    g_uuid ++;

    /* add topic */
    str_len = strlen(topic);
    *pdata ++ = (uint8_t)(str_len >> 8);
    *pdata ++ = (uint8_t)(str_len & 0xff);
    strcpy((char *)pdata, topic);
    pdata += str_len;

    *pdata = (qos & 0x03);
    
    msg.size = payload_len + encode_len + 1;

    /* send message to queue */
    xQueueSend(xSendQueue, &msg, 0);
}

/**
 * @brief unsubscribe topic from server
 * @param topic - topic to subscribe
 * @param qos - topic qos
 */
void mqtt_unsubscribe(const char *topic)
{
    assert_param(NULL != topic);
    mqtt_msg msg;
    uint8_t *pdata = msg.data;
    uint16_t str_len = 0;
    uint32_t payload_len = 0;

    /* calculate payload length */
    payload_len = strlen(topic) + 4;

    /* packet data */
    *pdata = TYPE_SUBSCRIBE;
    
    uint8_t encode_len = encode_length(payload_len, pdata);
    assert_param((payload_len + encode_len + 1) <= MQTT_MAX_MSG_SIZE);
    pdata += encode_len;
    
    /* add uuid */
    *pdata ++ = (uint8_t)(g_uuid >> 8);
    *pdata ++ = (uint8_t)(g_uuid & 0xff);
    g_uuid ++;

    /* add topic */
    str_len = strlen(topic);
    *pdata ++ = (uint8_t)(str_len >> 8);
    *pdata ++ = (uint8_t)(str_len & 0xff);
    strcpy((char *)pdata, topic);
    pdata += str_len;
    
    msg.size = payload_len + encode_len + 1;

    /* send message to queue */
    xQueueSend(xSendQueue, &msg, 0);
}

/**
 * @brief pingreq topic from server
 * @param topic - topic to subscribe
 * @param qos - topic qos
 */
void mqtt_pingreq(void)
{
    mqtt_msg msg;
    uint8_t *pdata = msg.data;

    /* packet data */
    *pdata ++ = TYPE_PINGREQ;
    *pdata ++ = 0x00;
    
    msg.size = 2;

    /* send message to queue */
    xQueueSend(xSendQueue, &msg, 0);
}


