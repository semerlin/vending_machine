/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "stm32f10x_cfg.h"
#include "dbgserial.h"
#include "global.h"
#include "FreeRTOS.h"
#include "semphr.h"

/* serial output mutex */
SemaphoreHandle_t xSerialMutex = NULL;

/**
 * @brief init debug serial port
 */
void dbg_serial_setup(void)
{
    USART_Config config;
    USART_StructInit(&config);

    config.rxEnable = FALSE;
    USART_Setup(USART1, &config);
    USART_EnableInt(USART1, USART_IT_RXNE, FALSE);
    USART_EnableInt(USART1, USART_IT_TXE, FALSE);
    USART_Enable(USART1, TRUE);
    
    xSerialMutex = xSemaphoreCreateMutex();
}

/**
 * @brief put char
 * @param data - data to put
 */
void dbg_putchar(char data)
{
    USART_WriteData_Wait(USART1, data);
}

/**
 * @brief put string
 * @param string - string to put
 * @param length - string length
 */
void dbg_putstring(const char *string, uint32_t length)
{
    const char *pNext = string;
    while(length--)
        dbg_putchar(*pNext++);
}


#ifdef __DEBUG
void assert_failed(const char *file, const char *line, const char *exp)
{
    dbg_putstring("assert failed: ", 15);
    dbg_putstring(file, strlen(file));
    dbg_putstring(":", 1);
    dbg_putstring(line, strlen(line));
    dbg_putstring("(", 1);
    dbg_putstring(exp, strlen(exp));
    dbg_putstring(")\n", 2);
    while(1);
}
#endif

#ifdef __ENABLE_TRACE
void trace(const char *module, const char *fmt, ...)
{
    char buf[80];
    va_list argptr;
    int cnt;
    va_start(argptr, fmt);
    cnt = vsprintf(buf, fmt, argptr);
    va_end(argptr);
    xSemaphoreTake(xSerialMutex, portMAX_DELAY);
    dbg_putstring(module, strlen(module));
    dbg_putchar(' ');
    dbg_putstring(buf, cnt);
    xSemaphoreGive(xSerialMutex);
}
#endif



