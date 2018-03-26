#include "dbgserial.h"
#include "FreeRTOS.h"
#include "serial.h"
#include "environment.h"


static serial *g_serial = NULL;

/**
 * @brief init debug serial port
 */
void vDbgSerialSetup(void)
{
    //init serial port
    g_serial = serial_request(COM2);
    serial_open(g_serial);
}



#ifdef __DEBUG
void assert_failed(const char *file, const char *line, const char *exp)
{
    Serial_PutString(g_serial, "Assert Failed: ", 15);
    Serial_PutString(g_serial, file, strlen(file));
    Serial_PutChar(g_serial, ':', 0);
    Serial_PutString(g_serial, line, strlen(line));
    Serial_PutChar(g_serial, '(', 0);
    Serial_PutString(g_serial, exp, strlen(exp));
    Serial_PutString(g_serial, ")\n", 2);
    while(1);
}
#endif

