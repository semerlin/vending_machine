/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include <string.h>
#include "pinconfig.h"
#include "stm32f10x_cfg.h"

/* pin configure structure */
typedef struct 
{
    char name[16];
    GPIO_Group group;
    GPIO_Config config;
    bool exti_enable;
    Trigger_Edge edge;
}PIN_CONFIG;


typedef enum
{
    AHB,
    APB1,
    APB2,
}PIN_BUS;

typedef struct
{
    PIN_BUS bus;
    uint32_t reset_reg;
    uint32_t enable_reg;
}PIN_CLOCK;


/* pin arrays */
PIN_CONFIG pins[] = 
{
    {"CON_L1", GPIOC, 9, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, FALSE, 0},
    {"CON_L2", GPIOC, 8, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, FALSE, 0},
    {"CON_L3", GPIOC, 7, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, FALSE, 0},
    {"CON_L4", GPIOC, 6, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, FALSE, 0},
    {"CON_R1", GPIOB, 12, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, FALSE, 0},
    {"CON_R2", GPIOB, 13, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, FALSE, 0},
    {"CON_R3", GPIOB, 14, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, FALSE, 0},
    {"CON_R4", GPIOB, 15, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, FALSE, 0},
    {"CH1_DET", GPIOA, 0, GPIO_Speed_2MHz, GPIO_Mode_IN_FLOATING, FALSE, 0},
    {"CH2_DET", GPIOA, 1, GPIO_Speed_2MHz, GPIO_Mode_IN_FLOATING, FALSE, 0},
    {"CH3_DET", GPIOA, 4, GPIO_Speed_2MHz, GPIO_Mode_IN_FLOATING, FALSE, 0},
    {"CH4_DET", GPIOA, 5, GPIO_Speed_2MHz, GPIO_Mode_IN_FLOATING, FALSE, 0},
    {"CH5_DET", GPIOA, 6, GPIO_Speed_2MHz, GPIO_Mode_IN_FLOATING, FALSE, 0},
    {"CH6_DET", GPIOA, 7, GPIO_Speed_2MHz, GPIO_Mode_IN_FLOATING, FALSE, 0},
    {"CH7_DET", GPIOC, 4, GPIO_Speed_2MHz, GPIO_Mode_IN_FLOATING, FALSE, 0},
    {"CH8_DET", GPIOC, 5, GPIO_Speed_2MHz, GPIO_Mode_IN_FLOATING, FALSE, 0},
    {"CH9_DET", GPIOB, 0, GPIO_Speed_2MHz, GPIO_Mode_IN_FLOATING, FALSE, 0},
    {"CH10_DET", GPIOB, 1, GPIO_Speed_2MHz, GPIO_Mode_IN_FLOATING, FALSE, 0},
    {"MOT_DET", GPIOC, 3, GPIO_Speed_2MHz, GPIO_Mode_IPD, TRUE, Trigger_Rising},
    {"DEBUG_TX", GPIOA, 9, GPIO_Speed_50MHz, GPIO_Mode_AF_PP, FALSE, 0},
    {"DEBUG_RX", GPIOA, 10, GPIO_Speed_2MHz, GPIO_Mode_IN_FLOATING, FALSE, 0},
    {"WIFI_TX", GPIOA, 2, GPIO_Speed_50MHz, GPIO_Mode_AF_PP, FALSE, 0},
    {"WIFI_RX", GPIOA, 3, GPIO_Speed_2MHz, GPIO_Mode_IN_FLOATING, FALSE, 0},
    {"WIFI_RST", GPIOC, 14, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, FALSE, 0},
    {"WIFI_EN", GPIOC, 15, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, FALSE, 0},
    {"GPRS_TX", GPIOB, 10, GPIO_Speed_50MHz, GPIO_Mode_AF_PP, FALSE, 0},
    {"GPRS_RX", GPIOB, 11, GPIO_Speed_2MHz, GPIO_Mode_IN_FLOATING, FALSE, 0},
    {"GPRS_PWR", GPIOC, 13, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, FALSE, 0},
    {"LED1", GPIOB, 3, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, FALSE, 0},
    {"LED2", GPIOB, 4, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, FALSE, 0},
    {"LED3", GPIOB, 5, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, FALSE, 0},
    {"IR_IN", GPIOB, 2, GPIO_Speed_2MHz, GPIO_Mode_IN_FLOATING, TRUE, Trigger_Falling},
    {"SWITCH1", GPIOB, 7, GPIO_Speed_2MHz, GPIO_Mode_IN_FLOATING, FALSE, 0},
    {"SWITCH2", GPIOB, 8, GPIO_Speed_2MHz, GPIO_Mode_IN_FLOATING, FALSE, 0},
    {"MODE_SET", GPIOB, 6, GPIO_Speed_2MHz, GPIO_Mode_IN_FLOATING, FALSE, 0},
    {"LED_DATA", GPIOC, 0, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, FALSE, 0},
    {"LED_ST", GPIOC, 1, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, FALSE, 0},
    {"LED_SH", GPIOC, 2, GPIO_Speed_2MHz, GPIO_Mode_Out_PP, FALSE, 0},
};

/* clock arrays */
PIN_CLOCK pin_clocks[] = 
{
    {AHB, RCC_AHB_ENABLE_CRC, RCC_AHB_ENABLE_CRC},
    {APB2, RCC_APB2_RESET_AFIO, RCC_APB2_ENABLE_AFIO},
    {APB2, RCC_APB2_RESET_IOPA, RCC_APB2_ENABLE_IOPA},
    {APB2, RCC_APB2_RESET_IOPB, RCC_APB2_ENABLE_IOPB},
    {APB2, RCC_APB2_RESET_IOPC, RCC_APB2_ENABLE_IOPC},
    {APB2, RCC_APB2_RESET_USART1, RCC_APB2_ENABLE_USART1},
    {APB1, RCC_APB1_RESET_USART2, RCC_APB1_ENABLE_USART2},
    {APB1, RCC_APB1_RESET_USART3, RCC_APB1_ENABLE_USART3},
};

/**
 * @brief get pin configuration by name
 * @param name - pin name
 * @return pin configuration
 */
static const PIN_CONFIG *get_pinconfig(const char *name)
{
    uint32_t len = sizeof(pins) / sizeof(PIN_CONFIG);
    for(uint32_t i = 0; i < len; ++i)
    {
        if(strcmp(name, pins[i].name) == 0)
            return &pins[i];
    }
    
    return NULL;
}

/**
 * @brief init pins
 */
void pin_init(void)
{
    /* config pin clocks */
    uint32_t len = sizeof(pin_clocks) / sizeof(PIN_CLOCK);
    for(uint32_t i = 0; i < len; ++i)
    {
        switch(pin_clocks[i].bus)
        {
        case AHB:
            RCC_AHBPeripClockEnable(pin_clocks[i].enable_reg, TRUE);
            break;
        case APB1:
            RCC_APB1PeriphReset(pin_clocks[i].reset_reg, TRUE);
            RCC_APB1PeriphReset(pin_clocks[i].reset_reg, FALSE);
            RCC_APB1PeripClockEnable(pin_clocks[i].enable_reg, TRUE);
            break;
        case APB2:
            RCC_APB2PeriphReset(pin_clocks[i].reset_reg, TRUE);
            RCC_APB2PeriphReset(pin_clocks[i].reset_reg, FALSE);
            RCC_APB2PeripClockEnable(pin_clocks[i].enable_reg, TRUE);
            break;
        default:
            break;
        }
    }
    
    /* config pins */
    len = sizeof(pins) / sizeof(PIN_CONFIG);
    for(uint32_t i = 0; i < len; ++i)
    {
        GPIO_Setup(pins[i].group, &pins[i].config);
        if (pins[i].exti_enable)
        {
            GPIO_ExtiConfig(pins[i].group, pins[i].config.pin);
            EXTI_EnableLine_INT(pins[i].config.pin, TRUE);
            EXTI_SetTrigger(pins[i].config.pin, pins[i].edge);
        }
    }
}

/**
 * @brief set pin
 * @param name - pin name
 */
void pin_set(const char *name)
{
    const PIN_CONFIG *config = get_pinconfig(name);
    assert_param(config != NULL);
    GPIO_SetPin(config->group, config->config.pin);
}

/**
 * @brief reset pin
 * @param name - pin name
 */
void pin_reset(const char *name)
{
    const PIN_CONFIG *config = get_pinconfig(name);
    assert_param(config != NULL);
    GPIO_ResetPin(config->group, config->config.pin);
}

/**
 * @brief toggle pin 
 * @param name - pin name
 */
void pin_toggle(const char *name)
{
    const PIN_CONFIG *config = get_pinconfig(name);
    assert_param(config != NULL);
    if (GPIO_ReadPin(config->group, config->config.pin) != 0)
    {
        GPIO_ResetPin(config->group, config->config.pin);
    }
    else
    {
        GPIO_SetPin(config->group, config->config.pin);
    }
}
/**
 * @brief check if pin is set
 * @param name - pin name
 */
bool is_pinset(const char *name)
{
    const PIN_CONFIG *config = get_pinconfig(name);
    assert_param(config != NULL);
    return (GPIO_ReadPin(config->group, config->config.pin) != 0);
}

/**
 * @brief get pin information
 * @param name - pin name
 * @param group - pin group
 * @param num - pin number
 */
void get_pininfo(const char *name, uint8_t *group, uint8_t *num)
{
    assert_param(name != NULL);
    const PIN_CONFIG *config = get_pinconfig(name);
    assert_param(config != NULL);
    if (NULL != group)
    {
        *group = config->group;
    }

    if (NULL != num)
    {
        *num = config->config.pin;
    }
}

