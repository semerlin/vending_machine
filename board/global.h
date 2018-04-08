/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _GLOBAL_H_
  #define _GLOBAL_H_

#include "FreeRTOS.h"

#define VERSION  ("v1.0.1.0_rc")

/* task priority definition */
#define INIT_SYSTEM_PRIORITY         (tskIDLE_PRIORITY + 1)
#define ESP8266_PRIORITY             (tskIDLE_PRIORITY + 1)

/* task stack definition */
#define INIT_SYSTEM_STACK_SIZE       (configMINIMAL_STACK_SIZE)
#define ESP8266_STACK_SIZE           (configMINIMAL_STACK_SIZE)


/* interrupt priority */
#define USART1_PRIORITY        (14)


#endif /* _GLOBAL_H_ */

