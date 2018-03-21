#include "stm32f10x_cfg.h"
#include "global.h"
#include "FreeRTOS.h"
#include "task.h"
#include "serial.h"
#include <string.h>
#include "environment.h"
#include "dbgserial.h"

void ApplicationStartup()
{
    /* Start the scheduler. */
    vTaskStartScheduler();
}