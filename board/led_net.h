/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _LED_NET_H_
  #define _LED_NET_H_

#include "types.h"

BEGIN_DECLS

void led_net_init(void);
void led_net_turn_on(uint8_t num);
void led_net_turn_off(uint8_t num);
void led_net_flashing(uint8_t num, TickType_t interval);
void led_net_stop_flashing(uint8_t num);

END_DECLS


#endif /* _LED_NET_H_ */
