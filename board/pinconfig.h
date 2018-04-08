/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _PINCONFIG_H_
  #define _PINCONFIG_H_

#include "types.h"

BEGIN_DECLS

void pin_init(void);
void pin_set(const char *name);
void pin_reset(const char *name);
void pin_toggle(const char *name);
bool is_pinset(const char *name);
void get_pininfo(const char *name, uint8_t *group, uint8_t *num);

END_DECLS

#endif /* _PINCONFIG_H_ */
