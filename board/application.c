#include "stm32f10x_cfg.h"
#include "global.h"
#include "FreeRTOS.h"
#include "task.h"

void ApplicationStartup()
{
    /* Start the scheduler. */
    vTaskStartScheduler();
}
