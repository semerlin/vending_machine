#ifndef _TYPES_H_
#define _TYPES_H_

#include "macros.h"

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


#endif
