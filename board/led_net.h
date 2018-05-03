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

typedef enum
{
    on,
    off,
    flash
}led_action;

void led_net_init(void);
void led_net_set_action(const char *name, led_action action);

END_DECLS


#endif /* _LED_NET_H_ */
