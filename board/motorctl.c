/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "motorctl.h"
#include "assert.h"
#include "trace.h"
#include "pinconfig.h"


#undef __TRACE_MODULE
#define __TRACE_MODULE  "[motor]"

#define MOTOR_NUM   10

const char *motor_left[] = {"CON_L1", "CON_L2", "CON_L3", "CON_L4"};
const char *motor_right[] = {"CON_R1", "CON_R2", "CON_R3", "CON_R4"};

/**
 * @brief initialize motor control
 */
void motor_init(void)
{
    int i = 0;
    for (; i < sizeof(motor_left) / sizeof(char *); ++i)
    {
        pin_reset(motor_left[i]);
        pin_reset(motor_right[i]);
    }
}

/**
 * @brief start motor
 * @param num - motor number
 */
void motor_start(uint8_t num)
{
}

/**
 * @brief stop motor
 * @param num - motor number
 */
void motor_stop(uint8_t num)
{
}


