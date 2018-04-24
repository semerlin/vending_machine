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
#define INIT_NETWORK_PRIORITY        (tskIDLE_PRIORITY + 1)
#define HTTP_PRIORITY                (tskIDLE_PRIORITY + 1)
#define AP_PRIORITY                  (tskIDLE_PRIORITY + 1)
#define ESP8266_PRIORITY             (tskIDLE_PRIORITY + 3)
#define MOTOR_PRIORITY               (tskIDLE_PRIORITY + 2)
#define MQTT_PRIORITY                (tskIDLE_PRIORITY + 2)
#define IR_PRIORITY                  (tskIDLE_PRIORITY + 1)

/* task stack definition */
#define INIT_SYSTEM_STACK_SIZE       (configMINIMAL_STACK_SIZE)
#define INIT_NETWORK_STACK_SIZE      (configMINIMAL_STACK_SIZE)
#define HTTP_STACK_SIZE              (configMINIMAL_STACK_SIZE)
#define AP_STACK_SIZE                (configMINIMAL_STACK_SIZE)
#define ESP8266_STACK_SIZE           (configMINIMAL_STACK_SIZE)
#define MOTOR_STACK_SIZE             (configMINIMAL_STACK_SIZE)
#define MQTT_STACK_SIZE              (configMINIMAL_STACK_SIZE)
#define IR_STACK_SIZE                (configMINIMAL_STACK_SIZE)

/* interrupt priority */
#define USART1_PRIORITY        (13)
#define EXTI3_PRIORITY         (14)


#endif /* _GLOBAL_H_ */

