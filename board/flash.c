/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include <string.h>
#include "flash.h"
#include "assert.h"
#include "trace.h"
#include "stm32f10x_cfg.h"

/* 0x800F400, 1K */
#define FLASH_ADDR   0x800F400
#define SSID_OFFSET   8
#define PWD_OFFSET    40

#undef __TRACE_MODULE
#define __TRACE_MODULE  "[flash]"

/**
 * @brief check system init status
 */
bool flash_first_start(void)
{
    uint8_t buf[8];
    FLASH_Read(FLASH_ADDR, buf, 8);
    return (0 != strncmp((const char *)buf, "INIT", 4));
}

/**
 * @brief get ssid and password
 * @param ssid - ap ssid
 * @param pwd - ap password
 */
void flash_get_ssid_pwd(char *ssid, char *pwd)
{
    FLASH_Read(FLASH_ADDR + SSID_OFFSET, (uint8_t *)ssid, 32);
    FLASH_Read(FLASH_ADDR + PWD_OFFSET, (uint8_t *)pwd, 32);
    TRACE("get ssid(%s), pwd(%s)\r\n", ssid, pwd);
}

/**
 * @brief set ap ssid and password
 * @param ssid - ap ssid
 * @param pwd - ap password
 */
void flash_set_ssid_pwd(const char *ssid, const char *pwd)
{
    FLASH_ErasePage(FLASH_ADDR);
    FLASH_Write(FLASH_ADDR, "INIT", 4);
    FLASH_Write(FLASH_ADDR + SSID_OFFSET, (uint8_t *)ssid, strlen(ssid) + 1);
    FLASH_Write(FLASH_ADDR + PWD_OFFSET, (uint8_t *)pwd, strlen(pwd) + 1);
    TRACE("update ssid(%s), pwd(%s)\r\n", ssid, pwd);
}

/**
 * @brief restore to configure
 */
void flash_restore(void)
{
    FLASH_ErasePage(FLASH_ADDR);
}