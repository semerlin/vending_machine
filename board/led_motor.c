/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "led_motor.h"
#include "assert.h"
#include "trace.h"
#include "cm3_core.h"
#include "pinconfig.h"

#undef __TRACE_MODULE
#define __TRACE_MODULE  "[led_motor]"

#define LED_NUM 10

/* led status */
static uint16_t led_status = 0;


/**
 * @brief shift register transition
 */
static __INLINE void sh_transition(void)
{
    pin_reset("LED_SH");
    __NOP();
    __NOP();
    pin_set("LED_SH");
    __NOP();
    __NOP();
}

/**
 * @brief storage register transition
 */
static __INLINE void st_transition(void)
{
    pin_reset("LED_ST");
    __NOP();
    __NOP();
    pin_set("LED_ST");
    __NOP();
    __NOP();
}

/**
 * @brief send data to 74hc595
 * @param data - data to send
 */
static void hc595_senddata(uint16_t data)
{
    for (int i = 0; i < 16; ++i)
    {
        if (0 != (data & 0x8000))
        {
            pin_reset("LED_DATA");
        }
        else
        {
            pin_set("LED_DATA");
        }
        data <<= 1;
        sh_transition();
    }
    st_transition();
}

/**
 * @brief initialize motor led
 */
void led_motor_init(void)
{
    TRACE("initialieze motor led...\r\n");
    led_status = 0;
    hc595_senddata(led_status);
}

/**
 * @brief turn on led
 * @param num - led number(0-15)
 */
void led_motor_turn_on(uint8_t num)
{
    assert_param(num < LED_NUM);
    TRACE("turn on led: %d\r\n", num);
    led_status |= (1 << num);
    hc595_senddata(led_status);
}

/**
 * @brief turn off led
 * @param num - led number(0-15)
 */
void led_motor_turn_off(uint8_t num)
{
    assert_param(num < LED_NUM);
    TRACE("turn off led: %d\r\n", num);
    led_status &= ~(1 << num);
    hc595_senddata(led_status);
}

/**
 * @brief turn on all led
 */
void led_motor_all_on(void)
{
    TRACE("turn on all led\r\n");
    led_status = 0xffff;
    hc595_senddata(led_status);
}

/**
 * @brief turn off all led
 */
void led_motor_all_off(void)
{
    TRACE("turn off all led\r\n");
    led_status = 0;
    hc595_senddata(led_status);
}
