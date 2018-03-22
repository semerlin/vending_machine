/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "mqtt.h"

/* send and recive queue */
xQueueHandle xSendQueue = NULL;
xQueueHandle xRecvQueue = NULL;

#define MQTT_MAX_MSG_NUM     (10)
#define MQTT_MAX_MSG_SIZE    (128)

typedef struct
{
    uint8_t size;
    uint8_t data[MQTT_MAX_BUF_SIZE];
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

typedef struct
{
    mqtt_type type;
    uint8_t length[4];
}mqtt_fix_header;

/* connect */
const uint8_t protocol_name[6] = {0x00, 0x04, 'M', 'Q', 'T', 'T'};
#define PROTOCOL_LEVEL    (0x04)
connect_flag default_connect_flag = {0x00};

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
    }while (data > 0)

    return size;
}

/**
 * @brief decode data length
 * @param decode - data to decode
 * @return data length 
 */
static uint32_t decode_length(uint8_t *decode)
{
    uint32_t multiplier = 0;
    uint32_t value = 0;
    uint8_t pos = 0;
    do
    {
        decode ++;
        value += (*decode & 0x7f) * multiplier;
        multipiler *= 128;
        if (multiplier > 128 * 128 *128)
        {
            /* error happened */
            return 0xffffffff;
        }
    }while (0 != (*decode & 0x80))

    return value;
}

/**
 * @brief initialize mqtt protocol
 */
void mqtt_init(void)
{
    xSendQueue = xQueueCreate(MQTT_MAX_MSG_NUM, sizeof(mqtt_msg) / sizeof(uint8_t));
    xRecvQueue = xQueueCreate(MQTT_MAX_MSG_NUM, sizeof(mqtt_msg) / sizeof(uint8_t));
}

/**
 * @brief validation connect parameter
 * @param param - connect parameter
 */
void check_connect_param(const connect_param *param)
{
    if (NULL == param)
    {
        return FALSE;
    }

    if (NULL == param->client_id)
    {
        assert_param(0x01 == param->flag.clear_session);
    }

    if (0x00 == flag->will_flag)
    {
        assert_param(0 == flag->will_qos);
        assert_param(0 == flag->will_retain);
    }
    else
    {
        assert_param(flag->will_qos < 0x03);
        assert_param(NULL != param->will_topic);
        assert_param(NULL != param->will_msg);
    }

    if (0x01 == flag->username_flag)
    {
        assert_param(NULL != param->username);
    }

    if (0x01 == flag->password_flag)
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

    if (0x01 == flag->will_flag)
    {
        /* add will topic and will message */
        payload_len += (strlen(param->will_topic) + 2);
        payload_len += (strlen(param->will_msg) + 2);
    }

    if (0x01 == flag->username_flag)
    {
        /* add username */
        payload_len += (strlen(param->username) + 2);
    }

    if (0x01 == flag->password_flag)
    {
        /* add password */
        payload_len += (strlen(param->password) + 2);
    }

    return payload_len;
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
    *pdata ++ = param->flag;
    *pdata ++ = (uint8_t)(alive_time >> 8);
    *pdata ++ = (uint8_t)(alive_time & 0xff);
    if (NULL != param->client_id)
    {
        str_len = strlen(param->client_id);
        *pdata ++ = (uint8_t)(str_len >> 8);
        *pdata ++ = (uint8_t)(str_len & 0xff);
        strcpy(pdata, param->client_id);
        pdata += str_len;
    }
    else
    {
        *pdata ++ = 0x00;
        *pdata ++ = 0x00;
    }

    if (0x01 == flag->will_flag)
    {
        /* add will topic and will message */
        str_len = strlen(param->will_topic);
        *pdata ++ = (uint8_t)(str_len >> 8);
        *pdata ++ = (uint8_t)(str_len & 0xff);
        strcpy(pdata, param->will_topic);
        pdata += str_len;

        str_len = strlen(param->will_msg);
        *pdata ++ = (uint8_t)(str_len >> 8);
        *pdata ++ = (uint8_t)(str_len & 0xff);
        strcpy(pdata, param->will_msg);
        pdata += str_len;
    }

    if (0x01 == flag->username_flag)
    {
        /* add username */
        str_len = strlen(param->username);
        *pdata ++ = (uint8_t)(str_len >> 8);
        *pdata ++ = (uint8_t)(str_len & 0xff);
        strcpy(pdata, param->username);
        pdata += str_len;
    }

    if (0x01 == flag->password_flag)
    {
        /* add password */
        str_len = strlen(param->password);
        *pdata ++ = (uint8_t)(str_len >> 8);
        *pdata ++ = (uint8_t)(str_len & 0xff);
        strcpy(pdata, param->password);
        pdata += str_len;
    }

    msg.size = payload_len + encode_len + 1;

    if (0x01 == flag->clear_session)
    {
        /* clear unack qos1 and qos2 message*/
    }

    /* send message to queue */
    xQueueSend(xSendQueue, &msg, 0);
}



