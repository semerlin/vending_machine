#ifndef _BH1750_H_
#define _BH1750_H_

#include "sysdef.h"
#include "i2c.h"

void bh1750Init(__in I2C *i2c);
void bh1750Start(__in I2C *i2c);
uint32 bh1750GetLight(__in I2C *i2c);

#endif

