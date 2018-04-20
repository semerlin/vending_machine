/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _MQTT_H_
  #define _MQTT_H_

#include "types.h"

BEGIN_DECLS

/* status */
#define MQTT_ERR_OK             (0x00)
#define MQTT_ERR_VERSION        (0x01)
#define MQTT_ERR_CLIENTID       (0x02)
#define MQTT_ERR_SERVICE        (0x03)
#define MQTT_ERR_NAME_PWD       (0x04)
#define MQTT_ERR_AUTHORIZE      (0x05)

typedef union
{
    struct
    {
        uint8_t reserved:1;
        uint8_t clear_session:1;
        uint8_t will_flag:1;
        uint8_t will_qos:2;
        uint8_t will_retain:1;
        uint8_t password_flag:1;
        uint8_t username_flag:1;
    }_flag;
    uint8_t flag;
}connect_flag;

typedef struct
{
    connect_flag flag;
    const char *client_id;
    const char *will_topic;
    const char *will_msg;
    const char *username;
    const char *password;
    uint16_t alive_time;
}connect_param;

typedef struct
{
    void (*connack)(uint8_t status);
    void (*puback)(uint16_t id);
    void (*pubrec)(uint16_t id);
    void (*pubrel)(uint16_t id);
    void (*pubcomp)(uint16_t id);
    void (*suback)(uint8_t status, uint16_t id);
    void (*unsuback)(uint16_t id);
    void (*pingresp)(void);
}mqtt_driver;

bool mqtt_init(void);
void mqtt_attach(const mqtt_driver *driver);
void mqtt_detach(void);
int mqtt_connect_server(uint16_t id, const char *ip, uint16_t port);
bool mqtt_is_connected(void);
void mqtt_connect(const connect_param *param);
void mqtt_publish(const char *topic, const char *content, uint8_t dup,
                  uint8_t qos, uint8_t retain);
void mqtt_subscribe(const char *topic, uint8_t qos);
void mqtt_unsubscribe(const char *topic);
void mqtt_pingreq(void);



END_DECLS


#endif /* _MQTT_H_ */

