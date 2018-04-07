/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _ASSERT_H_
  #define _ASSERT_H_

#include "types.h"

BEGIN_DECLS

#ifdef __DEBUG
    /**
     * @brief the assert macro is used for function's parameters check.
     * @param file: file name
     * @param line: line number
     * @param expr: if expr is false, it calls assert_failed function which reports 
     *        the name of the source file and the source line number of the call 
     *        that failed. If expr is true, it returns no value.
     */
    /* external function */
    extern void assert_failed(const char *file, const char *line,
                              const char *exp);
    #define assert_param(expr) ((expr) ? (void)0 :       \
          assert_failed(__FILE__, STR(__LINE__), #expr))
#else
    #define assert_param(expr)
#endif 

END_DECLS

#endif /* _ASSERT_H_ */

