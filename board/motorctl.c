/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "motorctl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "assert.h"
#include "trace.h"
#include "pinconfig.h"
#include "global.h"
#include "stm32f10x_exti.h"


#undef __TRACE_MODULE
#define __TRACE_MODULE  "[motor]"

#define MOTOR_NUM   10

const char *motor_left[] = {"CON_L1", "CON_L2", "CON_L3", "CON_L4"};
const char *motor_right[] = {"CON_R1", "CON_R2", "CON_R3", "CON_R4"};
const char *motor_dect[] = {"CH1_DET", "CH2_DET", "CH3_DET", "CH4_DET",
"CH5_DET", "CH6_DET", "CH7_DET", "CH8_DET", "CH9_DET", "CH10_DET"};


/* motor control message queue */
static xQueueHandle xMotorQueue = NULL;
#define MOTOR_MSG_NUM      (4)

static xSemaphoreHandle xMotorWorking = NULL;
#define MOTOR_UP_TIME      (3000 / portTICK_PERIOD_MS)


/**
 * motor working detect interrupt handler
 */
void EXTI3_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    uint8_t pin_num = 0;
    get_pininfo("MOT_DET", NULL, &pin_num);
    xSemaphoreGiveFromISR(xMotorWorking, &xHigherPriorityTaskWoken);
    /* check if there is any higher priority task need to wakeup */
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    EXTI_ClrPending(pin_num);
}

/**
 * @brief motor control task
 * @param pvParameter - parameters pass to task
 */
static void vMotorCtl(void *pvParameters)
{
    uint8_t num = 0;
    uint8_t left = 0, right = 0;
    for (;;)
    {
        if (xQueueReceive(xMotorQueue, &num, portMAX_DELAY))
        {
            left = (num >> 2);
            right = num - (left << 2);
            TRACE("start motor: %d\n", num);
            pin_set(motor_left[left]);
            pin_set(motor_right[right]);

            /* wait motor working */
            if (pdTRUE == xSemaphoreTake(xMotorWorking, MOTOR_UP_TIME))
            {
                TRACE("motor working...\n");
                xSemaphoreGive(xMotorWorking);
            }
            else
            {
                TRACE("motor working timeout!\n");
            }

            TRACE("stop motor: %d\n", num);
            pin_reset(motor_left[left]);
            pin_reset(motor_right[right]);
        }
    }
}

/**
 * @brief initialize motor control
 */
void motor_init(void)
{
    int i = 0;
    TRACE("initialize motor...\n");
    for (; i < sizeof(motor_left) / sizeof(char *); ++i)
    {
        pin_reset(motor_left[i]);
        pin_reset(motor_right[i]);
    }
    
    xMotorQueue = xQueueCreate(MOTOR_MSG_NUM, 1);
    xMotorWorking = xSemaphoreCreateBinary();
    xTaskCreate(vMotorCtl, "MotorCtl", MOTOR_STACK_SIZE, 
                NULL, MOTOR_PRIORITY, NULL);
}

/**
 * @brief start motor
 * @param num - motor number
 */
void motor_start(uint8_t num)
{
    assert_param(num < MOTOR_NUM);
    xQueueSend(xMotorQueue, &num, 0);
}

/**
 * @brief check is motor opend
 * @param num - motor number
 * @return open status
 */
bool motor_isopen(uint8_t num)
{
    assert_param(num < MOTOR_NUM);

    return is_pinset(motor_dect[num]);
}

/**
 * @brief get motor status
 * @return motor status
 */
uint16_t motor_getstatus(void)
{
    uint16_t status = 0xffff;
    for (int i = 0; i < MOTOR_NUM; ++i)
    {
        if (!is_pinset(motor_dect[i]))
        {
            status &= ~(1 << i);
        }
    }

    return status;
}

