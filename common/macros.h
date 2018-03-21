/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _MACROS_H_
  #define _MACROS_H_

#include <stddef.h>

/* inline definition */
#ifndef __INLINE
  #define __INLINE inline
#endif

/* asm definition */
#ifndef __ASM
  #define __ASM __asm
#endif

#undef  MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#undef  MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

#undef  ABS
#define ABS(a)     (((a) < 0) ? -(a) : (a))

#undef  CLAMP
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

/* array length */
#define N_ELEMENTS(arr) (sizeof(arr) / sizeof(arr[0]))

#define OFFSET_OF(struct_type, member)         \
    ((unsigned long)&((struct_type *)0)->member)

/* get struct address */
#define CONTAINER_OF(member_ptr, struct_type, member)                     \
    ({                                                                      \
        const typeof(((struct_type *)0)->member) *__mptr = (member_ptr);    \
        (struct_type *)((char *)__mptr - T_OFFSET_OF(struct_type, member)); \
    })

#ifdef __cplusplus
    #define BEGIN_DECLS    extern "C" {
    #define END_DECLS      }
#else
    #define BEGIN_DECLS
    #define END_DECLS
#endif

#define STR(x) VAL(x)
#define VAL(x) #x

/* unused parameter declare */
#ifndef UNUSED
 #define UNUSED(x) ((void)x)
#endif


#endif /* _MACROS_H_ */

