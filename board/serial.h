/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _SERIAL_H_
  #define _SERIAL_H_

#include "types.h"
#include "FreeRTOS.h"

BEGIN_DECLS

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
serial *serial_request(Port port);
void serial_release(serial *pserial);
bool serial_open(serial *handle);
void serial_close(serial *handle);
void serial_set_baudrate(serial *handle, Baudrate baudrate);
void serial_set_parity(serial *handle, Parity parity);
void serial_set_stopbits(serial *handle, StopBits stopBits);
void serial_set_databits(serial *handle, DataBits dataBits);
void serial_set_bufferlength(serial *handle, UBaseType_t rxLen, 
                            UBaseType_t txLen);

bool serial_getchar(serial *handle, char *data, 
                    portTickType xBlockTime);
bool serial_putchar(serial *handle, char data,
                    portTickType xBlockTime);
void serial_putstring(serial *handle, const char *string,
                      uint32_t length);

END_DECLS

#endif /* _SERIAL_H_ */

