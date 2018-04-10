/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _MOTORCTL_H_
  #define _MOTORCTL_H_

#include "types.h"

BEGIN_DECLS

void motor_init(void);
void motor_start(uint8_t num);
bool motor_isopen(uint8_t num);
uint16_t motor_getstatus(void);

END_DECLS

#endif /* _MOTORCTL_H_ */

