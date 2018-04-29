/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _MODESWITCH_H_
  #define _MODESWITCH_H_

#include "types.h"

BEGIN_DECLS

#define MODE_AP        1
#define MODE_SAT       2

void modeswitch_init(void);
void modeswitch_set(uint8_t mode);

END_DECLS

#endif