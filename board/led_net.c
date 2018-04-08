/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "led_motor.h"
#include "FreeRTOS.h"
#include "timers.h"

/**
 * @brief turn on led
 * @param num - led number(0-2)
 */
void led_net_turn_on(uint8_t num)
{
}

/**
 * @brief turn off led
 * @param num - led number(0-2)
 */
void led_net_turn_off(uint8_t num)
{
}

/**
 * @brief flashing led
 * num - led number(0-2)
 * interval - flashing interval
 */
void led_net_flashing(uint8_t num, TickType_t interval)
{
}

/**
 * @brief stop led flashing
 * num - led number(0-2)
 */
void led_net_stop_flashing(uint8_t num)
{
}
