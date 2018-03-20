#include "delay.h"


/**
* @brief init delay system
*/
void delayInit(void)
{
}


/**
 * @brief delay us
 * @param us to delay
 */
void delay_us(__in uint16 time)
{
    uint16 i = 0;
    while(time--)
    {
        i = 10;
        while(i--);
    }
}

/**
 * @brief delay ms
 * @param ms to delay
 */
void delay_ms(__in uint16 time)
{
    uint16 i = 0;
    while(time--)
    {
        i = 12000;
        while(i--);
    }
}