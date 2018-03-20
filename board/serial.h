#ifndef _SERIAL_H_
  #define _SERIAL_H_

#include "sysdef.h"

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


/* interface */
Handle Serial_Request(__in Port port);
BOOL Serial_Open(__in Handle handle);
void Serial_Close(__in Handle handle);
void Serial_SetBaudrate(__in Handle handle, __in Baudrate baudrate);
void Serial_SetParity(__in Handle handle, __in Parity parity);
void Serial_SetStopBits(__in Handle handle, __in StopBits stopBits);
void Serial_SetDataBits(__in Handle handle, __in DataBits dataBits);
void Serial_SetBufferLength(__in Handle handle, __in UBaseType_t rxLen, 
                            __in UBaseType_t txLen);

BOOL Serial_GetChar(__in Handle handle, __out char *data, 
                    __in portTickType xBlockTime);
BOOL Serial_PutChar(__in Handle handle, __in char data,
                    __in portTickType xBlockTime);
void Serial_PutString(__in Handle handle, __in const char *string,
                      __in uint32 length);

#endif

