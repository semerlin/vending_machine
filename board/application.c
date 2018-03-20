#include "stm32f10x_cfg.h"
#include "global.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "serial.h"
#include <string.h>
#include "pms5003s.h"
#include "gp2y1050.h"
#include "sound.h"
#include "voc.h"
#include "display.h"
#include "am2302.h"
#include "iicsensor.h"
#include "environment.h"
#include "dbgserial.h"
#include "display_simple.h"

xQueueHandle xSensorValues = NULL;
xSemaphoreHandle xAdcMutex = NULL;

void ApplicationStartup()
{
    xSensorValues = xQueueCreate(10, 
                         (UBaseType_t)(sizeof(Sensor_Info) / sizeof(char)));
    xAdcMutex = xSemaphoreCreateMutex();

#ifdef __SIMPLEUI
    vDisplaySimpleSetup();
#else
    vDisplaySetup();
#endif
    
#ifndef __DEMO
    vIICSensorSetup();
#ifdef __PMS5003S
    vPMS5003Setup();
#else
    vGP2Y10150Setup();
#endif
    
#ifdef __DBGSERIAL
    vDbgSerialSetup();
#endif
    vSoundSetup();
    vVocSetup();
    vAM2302Setup();
#endif
    
	/* Start the scheduler. */
	vTaskStartScheduler();
}