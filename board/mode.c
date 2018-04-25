/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "mode.h"
#include "pinconfig.h"

static uint8_t g_mode_net = MODE_NET_WIFI;
static uint8_t g_mode_work = MODE_WORK_NORMAL;

/**
 * @brief initizlize mode configuration
 */
void mode_init(void)
{
    g_mode_net = (uint8_t)is_pinset("SWITCH1");
    g_mode_work = (uint8_t)is_pinset("SWITCH2");
}

/**
 * @brief get network work mode
 * @return network work mode
 */
uint8_t mode_net(void)
{
    return g_mode_net;
}

/**
 * @brief get board work mode
 * @return board work mode
 */
uint8_t mode_work(void)
{
    return g_mode_work;
}