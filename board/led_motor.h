/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _LED_MOTOR_H_
  #define _LED_MOTOR_H_

#include "types.h"

BEGIN_DECLS

void led_motor_init(void);
void led_motor_turn_on(uint8_t num);
void led_motor_turn_off(uint8_t num);
void led_motor_all_on(void);
void led_motor_all_off(void);

END_DECLS


#endif /* _LED_MOTOR_H_ */

