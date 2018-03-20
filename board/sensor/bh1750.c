#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "stm32f10x_cfg.h"
#include "global.h"
#include "i2c.h"
#include "environment.h"


/* bh1750 definition */
#define BH1750_ADDRESS      (0x23)
#define BH1750_SPEED        (100000)

/* register */
#define BH1750_POWER_DOWN      (0x00)
#define BH1750_POWER_ON        (0x01)
#define BH1750_RESET           (0x07)
#define BH1750_CON_HR1         (0x10)
#define BH1750_CON_HR2         (0x11)
#define BH1750_CON_LR          (0x13)
#define BH1750_ONE_HR1         (0x20)
#define BH1750_ONE_HR2         (0x21)
#define BH1750_ONE_LR          (0x23)


/**
* @brief init bh1750
* @param i2c handle
*/
void bh1750Init(__in I2C *i2c)
{
    i2c->device->i2c_setspeed(i2c->i2c, BH1750_SPEED);
    i2c->device->i2c_setslaveaddress(i2c->i2c, BH1750_ADDRESS);
    i2c->device->i2c_open(i2c->i2c);
}

/**
 * @brief start measure light
 * @param i2c handle
 */
void bh1750Start(__in I2C *i2c)
{
    uint8 data = BH1750_POWER_ON;
    i2c->device->i2c_write(i2c->i2c, (const char *)&data, 1);
    data = BH1750_RESET;
    i2c->device->i2c_write(i2c->i2c, (const char *)&data, 1);
    data = BH1750_ONE_HR1;
    i2c->device->i2c_write(i2c->i2c, (const char *)&data, 1);
}

/**
 * @brief get light value
 * @param i2c module
 */
uint32 bh1750GetLight(__in I2C *i2c)
{
    uint16 result = 0;
    uint8 data[2] = {0, 0};
    i2c->device->i2c_read(i2c->i2c, (char *)data, 2);
    result = data[0];
    result <<= 16;
    result |= data[1];
    return (uint32)(result / 1.2);
}