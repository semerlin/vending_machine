#include "i2c.h"
#include <string.h>
#include "FreeRTOS.h"

#define MAX_LIST_NUM   (5)

I2C_Device *deviceList[MAX_LIST_NUM] = {NULL, NULL, NULL, NULL, NULL};


/**
 * @brief register i2c device
 * @param device description
 */
void I2c_RegisterDevice(__in I2C_Device *device)
{
    for(uint8 i = 0; i < MAX_LIST_NUM; ++i)
    {
        if(deviceList[i] == NULL)
        {
            deviceList[i] = pvPortMalloc(sizeof(I2C_Device) / sizeof(int8));
            strcpy((char *)(deviceList[i]->name), (const char *)(device->name));
            deviceList[i]->i2c_request = device->i2c_request;
            deviceList[i]->i2c_open = device->i2c_open;
            deviceList[i]->i2c_close = device->i2c_close;
            deviceList[i]->i2c_setspeed = device->i2c_setspeed;
            deviceList[i]->i2c_setslaveaddress = device->i2c_setslaveaddress;
            deviceList[i]->i2c_setbufferlength = device->i2c_setbufferlength;
            deviceList[i]->i2c_write = device->i2c_write;
            deviceList[i]->i2c_read = device->i2c_read;
            break;
        }
    }
}

/**
 * @brief get i2c device
 * @param device name
 */
I2C_Device *I2c_GetDevice(const int8 *name)
{
    for(uint8 i = 0; i < MAX_LIST_NUM; ++i)
    {
        if(strcmp((char *)(deviceList[i]->name), (const char *)name) == 0)
        {
            return deviceList[i];
        }
    }
    
    return NULL;
}

/**
 * @brief remove i2c device
 * @param device name
 */
void I2c_RemoveDevice(__in const int8 *name)
{
    for(uint8 i = 0; i < MAX_LIST_NUM; ++i)
    {
        if(strcmp((char *)(deviceList[i]->name), (const char *)name) == 0)
        {
            vPortFree(deviceList[i]);
            deviceList[i] = NULL;
        }
    }
}
