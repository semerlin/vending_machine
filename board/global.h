#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "FreeRTOS.h"

#define VERSION  ("v1.0.1.0_rc")

/* task priority definition */
#define AMS_PRIORITY             (tskIDLE_PRIORITY + 1)

/* task stack definition */
#define AMS_STACK_SIZE           (configMINIMAL_STACK_SIZE)


/* interrupt priority */
#define USART1_PRIORITY        (14)




#endif