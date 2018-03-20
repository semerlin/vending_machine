#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "stm32f10x_cfg.h"
#include "global.h"
#include "environment.h"
#include "i2c.h"
#include "bmp280.h"
#include "bh1750.h"

#ifdef __I2C_HARDWARE
  #define I2C_NAME  "hardware"
#else
  #define I2C_NAME  "software"
#endif


/**
 * @brief process iic sensor data
 */
static void vI2CSensorProcess(void *pvParameters)
{
    UNUSED(pvParameters);
    const TickType_t xDelay = 700 / portTICK_PERIOD_MS;
    const TickType_t xNotifyWait = 100 / portTICK_PERIOD_MS;
    
    I2C bmpI2C, bhI2C;
    bmpI2C.device = I2c_GetDevice(I2C_NAME);
    bmpI2C.i2c = bmpI2C.device->i2c_request(I2c1);
    bhI2C.device = I2c_GetDevice(I2C_NAME);
    bhI2C.i2c = bhI2C.device->i2c_request(I2c2);
    //init bmp280
    Sensor_Info bmp280Info = {BMP280 , 0};
    BMP280_T bmp280;
    bmp280.i2c = &bmpI2C;
#ifdef __I2C_HARDWARE
    portENTER_CRITICAL();
#endif
    bmp280Init(&bmp280);
#ifdef __I2C_HARDWARE
    portEXIT_CRITICAL();
#endif
    vTaskDelay(100 / portTICK_PERIOD_MS);
    double temperature = 0, pressure = 0;
    uint32 prevPressureValue = 0;
    
    //init bh1750
    Sensor_Info bh1750Info = {BH1750 , 0};
#ifdef __I2C_HARDWARE
    portENTER_CRITICAL();
#endif
    bh1750Init(&bhI2C);
#ifdef __I2C_HARDWARE
    portEXIT_CRITICAL();
#endif
    uint32 prevLight = 0;
    
    
    for(;;)
    {
        //start sample
#ifdef __I2C_HARDWARE
        portENTER_CRITICAL();
#endif
        bmp280SetWorkMode(&bmp280, BMP280_FORCED);
        bh1750Start(&bhI2C);
#ifdef __I2C_HARDWARE
        portEXIT_CRITICAL();
#endif
        vTaskDelay(xDelay);
        
        //get sensor data
#ifdef __I2C_HARDWARE
        portENTER_CRITICAL();
#endif
        bmp280GetTemperatureAndPressure(&bmp280, &temperature, &pressure);
        bh1750Info.value = bh1750GetLight(&bhI2C);
#ifdef __I2C_HARDWARE
        portEXIT_CRITICAL();
#endif
         
        bmp280Info.value = (uint32)pressure;
        if(bmp280Info.value != prevPressureValue)
        {
            prevPressureValue = bmp280Info.value;
            xQueueSend(xSensorValues, (const void *)&bmp280Info, 
                           xNotifyWait);
        }
        
        if(bh1750Info.value != prevLight)
        {
            prevLight = bh1750Info.value;
            xQueueSend(xSensorValues, (const void *)&bh1750Info, 
                           xNotifyWait);
        }
        vTaskDelay(400 / portTICK_PERIOD_MS);
    }
}



void vIICSensorSetup(void)
{
    xTaskCreate(vI2CSensorProcess, "I2CSensorProcess", I2C_STACK_SIZE, 
                NULL, I2C_PRIORITY, NULL);
}