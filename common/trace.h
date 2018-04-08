/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _TRACE_H_
  #define _TRACE_H_

#include "types.h"

BEGIN_DECLS

#ifdef __ENABLE_TRACE
    #define __TRACE_MODULE   "[trace]"
    /** 
     * @brief extern function, used to output message.
     *        if you want to use log system, you need to implement this function 
     * @param file: file name
     * @param line: line number
     * @param fmt: trace message
     */
    /* external function */
    /*
      extern void trace(const char *file, long line, const char *fmt, ...);
      #define TRECE(fmt, ...) trace(__FILE__, STR(__LINE__), fmt, ##__VA_ARGS__)
    */
    extern void trace(const char *module, const char *fmt, ...);
    #define TRACE(fmt, ...) trace(__TRACE_MODULE, fmt, ##__VA_ARGS__)
#else
    #define TRACE(fmt, ...)
#endif

END_DECLS

#endif /* _TRACE_H_ */


