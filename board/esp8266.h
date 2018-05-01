/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _ESP8266_H_
  #define _ESP8266_H_

#include "types.h"

BEGIN_DECLS

/* esp8266 error message */
#define ESP_ERR_OK                0
#define ESP_ERR_TIMEOUT           1
#define ESP_ERR_PWD               2
#define ESP_ERR_NOT_FOUND         3
#define ESP_ERR_FAIL              4
#define ESP_ERR_ALREADY           5

typedef enum
{
    SAT = 0x01,
    AP = 0x02,
    BOTH = SAT | AP,
    UNKNOWN = 0x04,
}esp8266_mode;

typedef enum
{
    Normal,
    Transparent,
}esp8266_transmode;

typedef enum
{
    OPEN,
    WPA_PSK,
    WPA2_PSK,
    WPA_WPA2_PSK,
}esp8266_ecn;


typedef struct
{
    void (*ap_connect)(void);
    void (*ap_disconnect)(void);
    void (*server_connect)(uint8_t id);
    void (*server_disconnect)(uint8_t id);
}esp8266_driver;

/* esp8266 interface */
bool esp8266_init(void);
int esp8266_send_ok(const char *cmd);
int esp8266_setmode(esp8266_mode mode);
int esp8266_create_server(void);
int esp8266_close_server(void);
esp8266_mode esp8266_getmode(void);
int esp8266_connect_ap(const char *ssid, const char *pwd, TickType_t time);
int esp8266_set_softap(const char *ssid, const char *pwd, uint8_t chl, 
                       esp8266_ecn ecn);
int esp8266_set_apaddr(const char *ip, const char *gateway, const char *netmask);
int esp8266_connect_server(uint8_t id, const char *mode, const char *ip, 
                           uint16_t port);
int esp8266_disconnect_server(uint8_t id);
int esp8266_listen(uint16_t port);
int esp8266_close(uint16_t port);
int esp8266_prepare_send(uint8_t id, uint16_t length);
int esp8266_set_tcp_timeout(uint16_t timeout);
int esp8266_recv(uint8_t *id, uint8_t *data, uint16_t *len, TickType_t xBlockTime);
int esp8266_write(const char *data, uint32_t length);
void esp8266_attach(const esp8266_driver *driver);
void esp8266_detach(void);
void esp8266_shutdown(void);

END_DECLS

#endif /* _ESP8266_H_ */

