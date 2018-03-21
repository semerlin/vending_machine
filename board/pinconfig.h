#ifndef _PINCONFIG_H_
#define _PINCONFIG_H_

#include "types.h"


void pinInit(void);
void pinSet(const char *name);
void pinReset(const char *name);
bool isPinSet(const char *name);
void getPinInfo(const char *name, uint8_t *group, uint8_t *num);

#endif
