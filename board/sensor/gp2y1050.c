#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "stm32f10x_cfg.h"
#include "global.h"

/**
 * @brief process gp2y1050 data
 */
static void vGP2Y1050SProcess(void *pvParameters)
{
    UNUSED(pvParameters);
    const TickType_t xNotifyWait = 100 / portTICK_PERIOD_MS;
    const TickType_t xDelay = 500 / portTICK_PERIOD_MS; 
    uint32 value = 0;
    uint16 pmVal = 0;
    uint16 prevPmVal = 0;
    Sensor_Info sensorInfo = {GP2Y1050 , 0};
    for(;;)
    {
        xSemaphoreTake(xAdcMutex, portMAX_DELAY);
        ADC_SetRegularChannel(ADC1, 0, ADC_CHANNEL13);
        ADC_InternalTriggerConversion(ADC1, ADC_CHANNEL_GROUP_REGULAR);
        while(!ADC_IsFlagOn(ADC1, ADC_FLAG_EOC));
        value = ADC_GetRegularValue(ADC1);
        xSemaphoreGive(xAdcMutex);
        //calculate pm2.5 value, ug/m3
        if(value <= 3730)
        {
            pmVal = value * 3300 / 20475;
        }
        else
        {
            /* ax+b=y
            3-------0.6   3.15---0.70  3.3-----0.8
            3722----600   3909---700   4095----800
            a=0.54 b=-1401.6
            */
            pmVal = (uint16)(value * 0.53 - 1401.6);
        }
        sensorInfo.value = pmVal;
        if(sensorInfo.value != prevPmVal)
        {
            prevPmVal = sensorInfo.value;
            xQueueSend(xSensorValues, (const void *)&sensorInfo, xNotifyWait);
        }

        vTaskDelay(xDelay);
    }
}

/**
 * @brief setup gp2y1050 process
 */
void vGP2Y10150Setup(void)
{
    xTaskCreate(vGP2Y1050SProcess, "GP2Y1050Process", GP2Y1050_STACK_SIZE, 
                NULL, PM2_5_PRIORITY, NULL);
}