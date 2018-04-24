/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "stm32f10x_gpio.h"
#include "stm32f10x_map.h"
#include "stm32f10x_cfg.h"

/* flash register structure */
typedef struct 
{
	volatile uint32_t CRL;
	volatile uint32_t CRH;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t BRR;
	volatile uint32_t LCKR;
}GPIO_T;

typedef struct 
{
	volatile uint32_t EVCR;
	volatile uint32_t MAPR;
	volatile uint32_t EXTICR[4];
    uint32_t RESERVED0;
	volatile uint32_t MAPR2;
}AFIO_T;



/* GPIO group array */
static GPIO_T * const GPIOx[] = {(GPIO_T *)GPIOA_BASE, 
                                 (GPIO_T *)GPIOB_BASE,
                                 (GPIO_T *)GPIOC_BASE,
                                 (GPIO_T *)GPIOD_BASE,
                                 (GPIO_T *)GPIOE_BASE,
                                 (GPIO_T *)GPIOF_BASE,
                                 (GPIO_T *)GPIOG_BASE};

static AFIO_T * const AFIO = (AFIO_T *)AFIO_BASE;



/**
 * @beirf configure one pin function
 * @param group: group name
 * @param config: pin configuration
 */
void GPIO_Setup(GPIO_Group group, const GPIO_Config *config)
{
    assert_param(config->pin < 16);
    assert_param(group < GPIO_Count);
    
    GPIO_T * const GpioX = GPIOx[group];
 
    /* config pin mode */
    if((config->mode & 0x10) == 0x10)
    {
        /* output */
        if(config->pin < 8)
        {
            GpioX->CRL &= ~(0x0f << (config->pin << 2));
            GpioX->CRL |= (config->speed << (config->pin << 2));
            GpioX->CRL |= ((config->mode & 0x0f) << ((config->pin << 2) + 2));
        }
        else
        {
            GpioX->CRH &= ~(0x0f << ((config->pin - 8) << 2));
            GpioX->CRH |= (config->speed << ((config->pin - 8) << 2));
            GpioX->CRH |= ((config->mode & 0x0f) << 
                           (((config->pin - 8) << 2) + 2));
        }
    }
    else
    {
        /* input */
        if(config->pin < 8)
        {
            GpioX->CRL &= ~(0x0f << (config->pin << 2));
            GpioX->CRL |= ((config->mode & 0x0f) << ((config->pin << 2) + 2));
        }
        else
        {
            GpioX->CRH &= ~(0x0f << ((config->pin - 8) << 2));
            GpioX->CRH |= ((config->mode & 0x0f) << 
                           (((config->pin - 8) << 2) + 2));
        }
        
        if(config->mode & 0x20)
            GpioX->ODR |= (1 << (config->pin));
        else
            GpioX->ODR &= ~(1 << (config->pin));
    }
}


/**
 * @brief read a group all pin data
 * @param group: port group
 * @return data
 */
uint16_t GPIO_ReadDataGroup(GPIO_Group group)
{
    assert_param(group < GPIO_Count);
    
    return GPIOx[group]->IDR;
}

/**
 * @brief write a group all pin data
 * @param group: port group
 * @param data: write value
 * @return data
 */
void GPIO_WriteDataGroup(GPIO_Group group, uint16_t data)
{
    assert_param(group < GPIO_Count);

    GPIOx[group]->ODR = data;
}


/**
 * @brief read pin data
 * @param group: port group
 * @param pin: pin position
 * @return pin data
 */
uint8_t GPIO_ReadPin(GPIO_Group group, uint8_t pin)
{
    assert_param(group < GPIO_Count);
    assert_param(pin < 16);
    
    return (GPIOx[group]->IDR >> pin) & 0x01;
}

/**
 * @brief set pin data
 * @param group: port group
 * @param pin: pin position
 */
void GPIO_SetPin(GPIO_Group group, uint8_t pin)
{
    assert_param(group < GPIO_Count);
    assert_param(pin < 16);

    GPIOx[group]->BSRR = (1 << pin);
}

/**
 * @brief reset pin data
 * @param group: port group
 * @param pin: pin position
 */
void GPIO_ResetPin(GPIO_Group group, uint8_t pin)
{
    assert_param(group < GPIO_Count);
    assert_param(pin < 16);

    GPIOx[group]->BRR = (1 << pin);
}
 
/**
 * @brief lock pin
 * @param group: port group
 * @param pin: pin position
 */
void GPIO_LockPin(GPIO_Group group, uint8_t pin)
{
    assert_param(group < GPIO_Count);
    assert_param(pin < 16);
    
    GPIO_T * const GpioX = GPIOx[group];
    uint32_t tmp = 0x00010000;
    tmp |= pin;
    
    /* lock sequence */
    GpioX->LCKR = tmp;
    GpioX->LCKR =  pin;
    GpioX->LCKR = tmp;
    tmp = GpioX->LCKR;
    tmp = GpioX->LCKR;
}

/**
 * @brief config gpio exti source
 * @param group - port group
 * @param pin - pin number
 */
void GPIO_EXTIConfig(GPIO_Group group, uint8_t pin)
{
    assert_param(group < GPIO_Count);
    assert_param(pin < 16);

    uint8_t pos = (pin >> 2);
    uint8_t offset = pin - (pos << 2);
    offset <<= 2;
    AFIO->EXTICR[pos] |= (((uint32_t)group) << offset);
}

/**
 * @brief config gpio remap
 * @param pin - pin name
 * @param flag - remap status
 */
void GPIO_PinRemap(uint32_t pin, bool flag)
{
    if (flag)
    {
        AFIO->MAPR |= pin;
    }
    else
    {
        AFIO->MAPR &= ~pin;
    }
}

