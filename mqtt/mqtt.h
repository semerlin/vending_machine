#ifndef _MQTT_H_
  #define _MQTT_H_

#include "types.h"

BEGIN_DECLS

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


int mqtt_connect_server(uint16_t id, const char *ip, uint16_t port);

END_DECLS


#endif /* _MQTT_H_ */

