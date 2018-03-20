#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "pinconfig.h"
#include "serial.h"
#include "global.h"
#include "cm3_core.h"
#include "environment.h"


/* pms5003 header definition */
#define PMS5003_HEAD1    (0x42)
#define PMS5003_HEAD2    (0x4d)


/* command */
static uint8 cmdData[7] = {0x42, 0x4d, 0x00, 0x00, 0x00, 0x00, 0x00};

typedef struct
{
    uint16 PM1_0_CF;
    uint16 PM2_5_CF;
    uint16 PM10_CF;
    uint16 PM1_0_Air;
    uint16 PM2_5_Air;
    uint16 PM10_Air;
    uint16 PM0_3_Num;
    uint16 PM0_5_Num;
    uint16 PM1_0_Num;
    uint16 PM2_5_Num;
    uint16 PM5_0_Num;
    uint16 PM10_Num;
    uint16 Formaldehyde;
}PM_Data;

/* interface */
static void vPackageData(__in uint8 cmd, __in uint16 status);
static BOOL vProcessData(__in uint8 *data, __out PM_Data *pm);

/**
 * @brief pms5003 data request task
 * @param serial handle
 */
static void vPMS5003SRequest(void *pvParameters)
{
    const TickType_t xDelay = 1500 / portTICK_PERIOD_MS;    
    Handle serial = pvParameters;
    
    vPackageData(0xe2, 0x00);
    for(;;)
    {
        //request pm2.5 data
        Serial_PutString(serial, (const char *)cmdData, 7);
        vTaskDelay(xDelay);
    }
}

/**
 * @brief pms5003 data process task
 * @param serial handle
 */
static void vPMS5003SProcess(void *pvParameters)
{
    const TickType_t xDelay = 3 / portTICK_PERIOD_MS;
    const TickType_t xNotifyWait = 100 / portTICK_PERIOD_MS; 
    Handle serial = pvParameters;
    uint8 dataPackage[36];
    uint8 *pData = dataPackage;
    char data;
    uint8 cnt = 0;
    PM_Data pmData;
    Sensor_Info sensorInfo = {PMS5003S , 0};
    uint32 prevValue = 0;
    for(;;)
    {
        if(Serial_GetChar(serial, &data, portMAX_DELAY))
        {
            *pData++ = data;
            cnt++;
            while(Serial_GetChar(serial, &data, xDelay))
            {
                *pData++ = data;
                cnt++;
                if(cnt >= 32)
                    break;
            }
            
            //get done, process data
            if(vProcessData(dataPackage, &pmData))
            {
                sensorInfo.value = pmData.PM2_5_CF;
                if(sensorInfo.value != prevValue)
                {
                    prevValue = sensorInfo.value;
                    //unpackage success, notify ui
                    xQueueSend(xSensorValues, (const void *)&sensorInfo, 
                           xNotifyWait);
                }
            }
            
            pData = dataPackage;
            cnt = 0;
        }
    }
}


/**
 * @brief init pms5003s
 */
void vPMS5003SInit(void *pvParameters)
{
    //reset pms5003
    pinReset("pms_reset");
    vTaskDelay(10 / portTICK_PERIOD_MS);
    pinSet("pms_reset");
    
    //active pms5003
    pinSet("pms_set");
    
    //init serial port
    Handle serial = Serial_Request(COM1);
    Serial_Open(serial);
    
    //set pms5003 work mode
    vPackageData(0xe4, 0x01);
    Serial_PutString(serial, (const char *)cmdData, 7);
    vPackageData(0xe1, 0x00);
    Serial_PutString(serial, (const char *)cmdData, 7);

    xTaskCreate(vPMS5003SRequest, "PMS5003SRequest", PMS5003_STACK_SIZE, 
                serial, PM2_5_PRIORITY, NULL);
    xTaskCreate(vPMS5003SProcess, "PMS5003SProcess", PMS5003_STACK_SIZE, 
                serial, PM2_5_PRIORITY, NULL);
    
    vTaskDelete(NULL);
}

/**
 * @brief setup pms5003s
 */
void vPMS5003Setup(void)
{
    
    xTaskCreate(vPMS5003SInit, "PMS5003SInit", PMS5003_STACK_SIZE, 
                NULL, PM2_5_PRIORITY, NULL);
}

/**
 * @brief package command
 * @param protocol command
 * @param command data
 */
static void vPackageData(__in uint8 cmd, __in uint16 status)
{
    uint16 cal = 0;
    cmdData[2] = cmd;
    cmdData[3] = ((status >> 8) & 0xff);
    cmdData[4] = (status & 0x0f);
    for(uint8 i = 0; i < 5; ++i)
    {
        cal += cmdData[i];
    }
    cmdData[5] = ((cal >> 8) & 0xff);
    cmdData[6] = (cal & 0xff);
}

/**
 * @brief unpackage pm data
 * @param pm data in
 * @param pm data return
 * @return TRUE:unpackge success FALSE:error happened
 */
static BOOL vProcessData(__in uint8 *data, __out PM_Data *pm)
{
    assert_param(data != NULL);
    assert_param(pm != NULL);
    
    //check header
    if((data[0] != 0x42) || (data[1] != 0x4d))
        return FALSE;
    
    //check parity
    uint16 parity = 0;
    for(uint8 i = 0; i < 30; ++i)
    {
        parity += data[i];
    }
    
    if(((parity - data[31]) >> 8) != data[30])
        return FALSE;
    
    /* because arm endian is small and the protocol endian is big,
       we can not do the transform as below */
    //*pm = *((PM_Data *)(data + 4));
    
    /* as the reason above, we use revsh to resolve it */
    uint16 *pDataOut = (uint16 *)pm;
    uint16 *pDataIn = (uint16 *)(data + 4);
    for(uint8 i = 0; i < 13; ++i)
        *pDataOut++ = __REVSH(*pDataIn++);
    
    
    return TRUE;
}