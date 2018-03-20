#include "dbgserial.h"
#include "FreeRTOS.h"
#include "serial.h"
#include "environment.h"


static Handle serial = NULL;

/**
 * @brief implement putchar function to use printf function
 * @param char to put
 * @param char to put;
 */
int32 putchar(__in int32 c)  
{  
    Serial_PutChar(serial, c, 0);
    return (c);  
}
/**
 * @brief init debug serial port
 */
void vDbgSerialSetup(void)
{
    //init serial port
    serial = Serial_Request(COM1);
    Serial_Open(serial);
}



#ifdef __DEBUG
void assert_failed(__in const char *file, __in const char *line, __in const char *exp)
{
    printf("Assert Failed: %s:%s (%s)", file, line, exp);
    /*Serial_PutString(serial, "Assert Failed: ", 15);
    Serial_PutString(serial, file, strlen(file));
    Serial_PutChar(serial, ':', 0);
    Serial_PutString(serial, line, strlen(line));
    Serial_PutChar(serial, '(', 0);
    Serial_PutString(serial, exp, strlen(exp));
    Serial_PutString(serial, ")\n", 2);*/
    while(1);
}
#endif

