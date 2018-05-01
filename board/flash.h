/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _FLASH_H_
  #define _FLASH_H_

#include "types.h"

bool flash_first_start(void);
void flash_restore(void);
void flash_get_ssid_pwd(char *ssid, char *pwd);
void flash_set_ssid_pwd(const char *ssid, const char *pwd);


#endif

