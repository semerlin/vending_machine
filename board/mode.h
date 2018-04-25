/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _MODE_H_
  #define _MODE_H_

#include "types.h"

BEGIN_DECLS

#define MODE_NET_WIFI        0
#define MODE_NET_GPRS        1
#define MODE_WORK_NORMAL     0
#define MODE_WORK_TEST       1

void mode_init(void);
uint8_t mode_net(void);
uint8_t mode_work(void);

END_DECLS

#endif