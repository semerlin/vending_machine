#ifndef _I2C_H_
  #define _I2C_H_

#include "FreeRTOS.h"
#include "sysdef.h"

/* serial parameter definition */
typedef enum
{ 
	I2c1,
    I2c2,
    Port_Count,
}Port;


typedef struct
{
    int8 name[16];
    Handle (*i2c_request)(__in Port port);
    BOOL (*i2c_open)(__in Handle handle);
    void (*i2c_close)(__in Handle handle);
    void (*i2c_setspeed)(__in Handle handle, __in uint32 speed);
    void (*i2c_setslaveaddress)(__in Handle handle, __in uint8 address);
    void (*i2c_setbufferlength)(__in Handle handle, __in UBaseType_t rxLen, 
                            __in UBaseType_t txLen);
    BOOL (*i2c_write)(__in Handle handle, __in const char *data, 
                      __in uint32 length);
    BOOL (*i2c_read)(__in Handle handle, __out char *data, __in uint32 length);
}I2C_Device;

typedef struct
{
    Handle i2c;
    I2C_Device *device;
}I2C;



/* interface */
void I2c_RegisterDevice(__in I2C_Device *device);
I2C_Device *I2c_GetDevice(const int8 *name);
void I2c_RemoveDevice(__in const int8 *name);


#endif

