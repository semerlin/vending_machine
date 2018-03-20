#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "stm32f10x_cfg.h"
#include "global.h"

/**
 * @brief process sound data
 */
         
static void vSoundProcess(void *pvParameters)
{
    UNUSED(pvParameters);
    const TickType_t xNotifyWait = 100 / portTICK_PERIOD_MS;
    const TickType_t xDelay = 100 / portTICK_PERIOD_MS;
    
    Sensor_Info sensorInfo = {Sound , 0};
    uint32 value = 0;
    uint8 count = 0;
    uint32 prevValue = 0;
    for(;;)
    {
        xSemaphoreTake(xAdcMutex, portMAX_DELAY);
        ADC_SetRegularChannel(ADC1, 0, ADC_CHANNEL11);
        ADC_InternalTriggerConversion(ADC1, ADC_CHANNEL_GROUP_REGULAR);
        while(!ADC_IsFlagOn(ADC1, ADC_FLAG_EOC));
        value += ADC_GetRegularValue(ADC1);
        xSemaphoreGive(xAdcMutex);
        count ++;
        if(count >= 8)
        {
            count = 0;
            value >>= 3;
            sensorInfo.value = value / SOUND_FACTOR;
            if(sensorInfo.value != prevValue)
            {
                prevValue = sensorInfo.value;
                xQueueSend(xSensorValues, (const void *)&sensorInfo, 
                           xNotifyWait);  
            }
            value = 0;
        }
        
        vTaskDelay(xDelay);
    }
}

/**
 * @brief ssetup sound process
 */
void vSoundSetup(void)
{
    xTaskCreate(vSoundProcess, "SoundProcess", SOUND_STACK_SIZE, 
                NULL, SOUND_PRIORITY, NULL);
}