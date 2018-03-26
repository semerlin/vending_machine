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

bool esp8266_init(void);
bool esp8266_test(void);

END_DECLS

#endif /* _ESP8266_H_ */
