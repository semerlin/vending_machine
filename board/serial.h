#ifndef _SERIAL_H_
  #define _SERIAL_H_

#include "types.h"
#include "FreeRTOS.h"

/* serial parameter definition */
typedef enum
{ 
	COM1,
    COM2,
    COM3,
    Port_Count,
}Port;

typedef enum 
{ 
    No,
    Odd,
    Even,
}Parity;

typedef enum 
{ 
	STOP_1, 
    STOP_1_5,
	STOP_2 
}StopBits;

typedef enum 
{  
	BITS_8 
}DataBits;

typedef enum 
{ 
	Baudrate_50 = 50,		
	Baudrate_75 = 75,		
	Baudrate_110 = 110,		
	Baudrate_134 = 134,		
	Baudrate_150 = 150,    
	Baudrate_200 = 200,
	Baudrate_300 = 300,		
	Baudrate_600 = 600,		
	Baudrate_1200 = 1200,	
	Baudrate_1800 = 1800,	
	Baudrate_2400 = 2400,   
	Baudrate_4800 = 4800,
	Baudrate_9600 = 9600,		
	Baudrate_19200 = 19200,	
	Baudrate_38400 = 38400,	
	Baudrate_57600 = 57600,	
	Baudrate_115200 = 115200
}Baudrate;

typedef struct _serial_t serial;

/* interface */
serial *Serial_Request(Port port);
bool Serial_Open(serial *handle);
void Serial_Close(serial *handle);
void Serial_SetBaudrate(serial *handle, Baudrate baudrate);
void Serial_SetParity(serial *handle, Parity parity);
void Serial_SetStopBits(serial *handle, StopBits stopBits);
void Serial_SetDataBits(serial *handle, DataBits dataBits);
void Serial_SetBufferLength(serial *handle, UBaseType_t rxLen, 
                            UBaseType_t txLen);

bool Serial_GetChar(serial *handle, char *data, 
                    portTickType xBlockTime);
bool Serial_PutChar(serial *handle, char data,
                    portTickType xBlockTime);
void Serial_PutString(serial *handle, const char *string,
                      uint32_t length);

#endif

