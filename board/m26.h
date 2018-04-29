/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _M26_H_
  #define _M26_H_

#include "types.h"

BEGIN_DECLS

/* m26 error message */
#define M26_ERR_OK                0
#define M26_ERR_TIMEOUT           1
#define M26_ERR_NOT_FOUND         3
#define M26_ERR_FAIL              4
#define M26_ERR_ALREADY           5

/* pin code status definition */
#define M26_PIN_READY           0
#define M26_SIM_PIN             1
#define M26_SIM_PUK             2
#define M26_PH_SIM_PIN          3
#define M26_PH_SIM_PUK          4
#define M26_SIM_PIN2            5
#define M26_SIM_PUK2            6
#define M26_SIM_UNKNOWN         7

typedef struct
{
    void (*net_register)(uint8_t code);
    void (*gprs_attach)(uint8_t code);
    void (*server_connect)(void);
    void (*server_disconnect)(void);
}m26_driver;

/* m26 interface */
bool m26_init(void);
int m26_send_ok(const char *cmd, TickType_t time);
uint8_t m26_pin_status(TickType_t time);
void m26_attach(const m26_driver *driver);
void m26_detach(void);
int m26_connect(const char *mode, const char *ip, const char *port,
        TickType_t time);
int m26_disconnect(TickType_t time);
int m26_prepare_send(uint16_t length, TickType_t time);
int m26_write(const char *data, uint32_t length, TickType_t time);
int m26_recv(char *data, uint16_t *len, TickType_t xBlockTime);
int m26_sync(void);
void m26_shutdown(void);

END_DECLS

#endif /* _ESP8266_H_ */

