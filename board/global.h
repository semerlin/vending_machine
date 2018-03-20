#ifndef _GLOBAL_H_
#define _GLOBAL_H_


#include "sysdef.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define VERSION  ("v1.0.1.0_rc")

typedef enum
{
    PMS5003S,
    GP2Y1050,
    Sound,
    Voc,
    AM2302,
    BMP280,
    BH1750,
}Sensor_Type;

typedef struct
{
    Sensor_Type type;
    uint32 value;
}Sensor_Info;

extern xQueueHandle xSensorValues;
extern xSemaphoreHandle xAdcMutex;

/* task priority definition */
#define LCD_PRIORITY             (tskIDLE_PRIORITY + 2)
#define VOC_PRIORITY             (tskIDLE_PRIORITY + 1)
#define SOUND_PRIORITY           (tskIDLE_PRIORITY + 1)
#define PM2_5_PRIORITY           (tskIDLE_PRIORITY + 1)
#define AMS_PRIORITY             (tskIDLE_PRIORITY + 1)
#define I2C_PRIORITY             (tskIDLE_PRIORITY + 3)

/* task stack definition */
#define LCD_STACK_SIZE           (configMINIMAL_STACK_SIZE * 5)
#define AMS_STACK_SIZE           (configMINIMAL_STACK_SIZE)
#define VOC_STACK_SIZE           (configMINIMAL_STACK_SIZE)
#define SOUND_STACK_SIZE         (configMINIMAL_STACK_SIZE)
#define GP2Y1050_STACK_SIZE      (configMINIMAL_STACK_SIZE)
#define PMS5003_STACK_SIZE       (configMINIMAL_STACK_SIZE * 2)
#define I2C_STACK_SIZE           (configMINIMAL_STACK_SIZE * 4)

/* sensor control */
#define SOUND_FACTOR (20)

/* interrupt priority */
#define USART1_PRIORITY        (14)
#define I2C_EV_PRIORITY        (13)
#define I2C_ER_PRIORITY        (13)




#endif