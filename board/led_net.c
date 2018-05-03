/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include <string.h>
#include "led_net.h"
#include "FreeRTOS.h"
#include "task.h"
#include "assert.h"
#include "trace.h"
#include "pinconfig.h"
#include "global.h"

#undef __TRACE_MODULE
#define __TRACE_MODULE  "[led_net]"

typedef struct
{
    const char *name;
    led_action action;
}led_status;

static led_status leds[] = 
{
    {"LED_ERROR", off},
    {"LED_NET", off},
    {"LED_MQTT", off}
};

/**
 * @brief led control task
 * @param pvParameters - task parameters
 */
static void vLed(void *pvParameters)
{
    for (;;)
    {
        for (int i = 0; i < sizeof(leds) / sizeof(leds[0]); ++i)
        {
            switch (leds[i].action)
            {
            case on:
                pin_set(leds[i].name);
                break;
            case off:
                pin_reset(leds[i].name);
                break;
            case flash:
                pin_toggle(leds[i].name);
                break;
            default:
                break;
            }
            
        }
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}


/**
 * @brief initialize net led
 */
void led_net_init(void)
{
    TRACE("initialize net led...\r\n");
    for (int i = 0; i < sizeof(leds) / sizeof(leds[0]); ++i)
    {
        pin_reset(leds[i].name);
    }
    
    xTaskCreate(vLed, "led", LED_STACK_SIZE, NULL, LED_PRIORITY, NULL);
}

/**
 * @brief set led action
 * @param name - led name
 * @param action - led action
 */
void led_net_set_action(const char *name, led_action action)
{
    assert_param(NULL != name);
    assert_param(action <= flash);
    for (int i = 0; i < sizeof(leds) / sizeof(leds[0]); ++i)
    {
        if (0 == strcmp(leds[i].name, name))
        {
            TRACE("set led(%s) action: %d\r\n", name, action);
            leds[i].action = action;
        }
    }
}
