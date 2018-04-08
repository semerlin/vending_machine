/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _DBGSERIAL_H_
  #define _DBGSERIAL_H_

#include "types.h"

BEGIN_DECLS

void dbg_serial_setup(void);
void dbg_putchar(char data);
void dbg_putstring(const char *string, uint32_t length);

END_DECLS

#endif /* _DBGSERIAL_H_ */
