#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "stm32f10x_cfg.h"
#include "global.h"
#include "pinconfig.h"
#include "delay.h"
#include "sysdef.h"


/* static functions */
static BOOL getTemperAndHumidityValue(__out int8 *temper, 
                                      __out uint8 *humidity);

/* ams2302 pin definition */
static uint8 am_group = 0;
static uint8 am_pin = 0;

/* pin value definition */
#define PIN_SET     (GPIO_SetPin((GPIO_Group)am_group, am_pin))
#define PIN_RESET   (GPIO_ResetPin((GPIO_Group)am_group, am_pin))


/**
 * @brief get pin value
 * @return pin value
 */
__INLINE static uint8 pinValue(void)
{
    return GPIO_ReadPin((GPIO_Group)am_group, am_pin);
}


/**
 * @brief change pin to output mode
 */
static void changePinToOutput(void)
{
    //set output push-pull
    GPIO_Config config = {am_pin, GPIO_Speed_2MHz, GPIO_Mode_Out_PP};
    GPIO_Setup((GPIO_Group)am_group, &config);
    GPIO_SetPin((GPIO_Group)am_group, am_pin);
}

/**
 * @brief change pin to input mode
 */
static void changePinToInput(void)
{
    //set input pull up
    GPIO_Config config = {am_pin, GPIO_Speed_2MHz, GPIO_Mode_IPU};
    GPIO_Setup((GPIO_Group)am_group, &config);
}



/**
 * @brief process voc data
 */
static void vAM2302Process(void *pvParameters)
{
    UNUSED(pvParameters);
    const TickType_t xNotifyWait = 100 / portTICK_PERIOD_MS;
    const TickType_t xDelay = 2000 / portTICK_PERIOD_MS;
    getPinInfo("am2302", &am_group, &am_pin);
    Sensor_Info sensorInfo = {AM2302 , 0};
    uint32 prevValue = 0;
    int8 temperature = 0; 
    uint8 humidity = 0;
    BOOL flag = FALSE;
    for(;;)
    {
        portENTER_CRITICAL();
        flag = getTemperAndHumidityValue(&temperature, &humidity);
        portEXIT_CRITICAL();
        
        if(flag)
        {
            sensorInfo.value = temperature;
            sensorInfo.value <<= 8;
            sensorInfo.value |= humidity;
            
            if(sensorInfo.value != prevValue)
            {
                prevValue = sensorInfo.value;
                xQueueSend(xSensorValues, (const void *)&sensorInfo, 
                           xNotifyWait);
            }
        }        
        vTaskDelay(xDelay);
    }
}

/**
 * @brief setup voc process
 */
void vAM2302Setup(void)
{
    xTaskCreate(vAM2302Process, "AM2302Process", AMS_STACK_SIZE, 
                NULL, AMS_PRIORITY, NULL);
}

/**
 * @brief read one byte from ams2302
 */
static uint8 am2302ReadByte(void)
{
	uint8 value = 0;
    uint16 count = 0;
	
	for(uint8 i = 0; i < 8; i++)    
	{	 
		while(pinValue() == 0)
        {
            if(count++ >= 400)
                return 0;
        }

		delay_us(40); 	  

		if(pinValue() == 1)
		{
            count = 0;
            while(pinValue() == 1)
            {
                if(count++ >= 400)
                    return 0;
            }
            
			value |= (uint8)(0x01 << (7 - i));
		}
		else
		{			   
			value &= (uint8)~(0x01 << (7 - i));
		}
	}
	return value;
}


/**
 * @brief get temperature and humidity value
 * @param temperature value
 * @param humidity value
 */
static BOOL getTemperAndHumidityValue(__out int8 *temper, __out uint8 *humidity)
{
    changePinToOutput();
    //start
    PIN_RESET;
    delay_ms(12);
    PIN_SET;
    changePinToInput();
    delay_us(30);
    uint8 temperHigh, temperLow, humiHigh, humiLow, calc, readCalc;
    uint16 count = 0;
#ifndef __DHT11
    uint16 temp = 0;
    int16 temp1 = 0;
#endif
    //ack
    if(pinValue() == 0)
    {
        while(pinValue() == 0)
        {
            if(count++ >= 400)
                return FALSE;
        }
        
        while(pinValue() == 1)
        {
            if(count++ >= 400)
                return FALSE;
        }
        
        //read valid data
        humiHigh = am2302ReadByte();
        humiLow = am2302ReadByte();
        temperHigh = am2302ReadByte();
        temperLow = am2302ReadByte();
        readCalc = am2302ReadByte();
        calc = humiHigh + humiLow + temperHigh + temperLow;
        if(readCalc == calc)
        {
            //check success
#ifdef __DHT11
            *humidity = humiHigh / 10;
            *temper = temperHigh / 10;
#else  
            temp = humiHigh;
            temp <<= 8;
            temp |= humiLow;
            *humidity = temp / 10;
            
            temp1 = temperHigh;
            temp1 <<= 8;
            temp1 |= temperLow;
            *temper = temp1 / 10;
#endif
            return TRUE;
        }
        else
            return FALSE;
    }
    else
        return FALSE;
}