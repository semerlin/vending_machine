#include "trace.h"
#include <stdio.h>
#include <stdarg.h>

void Trace_Msg(const char *format, ...)
{
#ifdef __ENABLE_TRACE
    char buffer[32];
    va_list msg;
    va_start(msg, format);

    vsprintf(buffer, format, msg);
    printf("MSG: %s", buffer);

    va_end(msg);
#else
    UNUSED(foramt);
#endif
}

void Trace_Warn(const char *format, ...)
{
#ifdef __ENABLE_TRACE
    char buffer[32];
    va_list msg;
    va_start(msg, format);

    vsprintf(buffer, format, msg);
    printf("WARN: %s", buffer);

    va_end(msg);
#else
    UNUSED(foramt);
#endif
}

void Trace_Error(const char *format, ...)
{
#ifdef __ENABLE_TRACE
    char buffer[32];
    va_list msg;
    va_start(msg, format);

    vsprintf(buffer, format, msg);
    printf("ERR: %s", buffer);

    va_end(msg);
#else
    UNUSED(foramt);
#endif
}