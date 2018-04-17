/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _SIMPLE_HTTP_H_
  #define _SIMPLE_HHTP_H_

#include "types.h"

BEGIN_DECLS

int http_init(void);
void http_start(void);
void http_stop(void);

END_DECLS


#endif /* _SIMPLE_HTTP_H_ */
