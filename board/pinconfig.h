#ifndef _PINCONFIG_H_
#define _PINCONFIG_H_

#include "types.h"


void pin_init(void);
void pin_set(const char *name);
void pin_reset(const char *name);
bool is_pinset(const char *name);
void get_pininfo(const char *name, uint8_t *group, uint8_t *num);

#endif
