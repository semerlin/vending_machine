/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _TYPES_H_
  #define _TYPES_H_

#include "macros.h"
#include <stdint.h>

/* boolean definition */
typedef int bool;

#ifndef FALSE
#define FALSE    (0)
#endif

#ifndef TRUE
#define TRUE     (!FALSE)
#endif

#ifndef NULL
#ifdef __cplusplus
    #define NULL    (0L)
#else
    #define NULL    ((void *)0)
#endif
#endif


#endif /* _TYPES_H_ */
