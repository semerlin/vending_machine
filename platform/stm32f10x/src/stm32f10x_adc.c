/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "stm32f10x_adc.h"
#include "stm32f10x_map.h"
#include "stm32f10x_cfg.h"


/* adc structure */
typedef struct 
{
    volatile uint32_t SR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMPR1;
    volatile uint32_t SMPR2;
    volatile uint32_t JOFR[4];
    volatile uint32_t HTR;
    volatile uint32_t LTR;
    volatile uint32_t SQR[3];
    volatile uint32_t JSQR;
    volatile uint32_t JDR[4];
    volatile uint32_t DR;
}ADC_T;

/* ADC group array */
static ADC_T * const ADCx[] = {(ADC_T *)ADC1_BASE, 
                               (ADC_T *)ADC2_BASE,
                               (ADC_T *)ADC3_BASE};


/* register bit definition */
#define CR1_DUALMOD        (0x0f << 16)
#define CR1_AWDEN          (1 << 23)
#define CR1_JAWDEN         (1 << 22)
#define CR1_AWDSGL         (1 << 9)
#define CR1_AWDCH          (0x1f)
#define CR1_JDISCEN        (1 << 12)
#define CR1_DISCEN         (1 << 11)
#define CR1_DISCNUM        (0x07 << 13)
#define CR1_JAUTO          (1 << 10)
#define CR1_SCAN           (1 << 8)

#define CR2_ADON           (1 << 0)
#define CR2_CAL            (1 << 2)
#define CR2_CONT           (1 << 1)
#define CR2_TSVREFE        (1 << 23)
#define CR2_SWSTART        (1 << 22)
#define CR2_JSWSTART       (1 << 21)
#define CR2_EXTTRIG        (1 << 20)
#define CR2_JEXTTRIG       (1 << 15)
#define CR2_EXTSEL         (0x07 << 17)
#define CR2_JEXTSEL        (0x07 << 12)
#define CR2_ALIGN          (1 << 11)
#define CR2_DMA            (1 << 8)
#define CR2_RSTCAL         (1 << 3)

#define SQR1_L             (0x0f << 20)
#define JSQR_JL            (0x03 << 20)

/**
 * @brief power on adc module
 * @param adc group
 * @param power on flag
 */
void ADC_PowerOn(ADC_Group group, bool flag)
{
    assert_param(group < ADC_Count);
    ADC_T * const AdcX = ADCx[group];
    
    if(flag)
        AdcX->CR2 |= CR2_ADON;
    else
        AdcX->CR2 &= ~CR2_ADON;
}

/**
 * @brief run adc build-in calibration mode
 * @note it is recommended to perform a calibration after each power-up
 * @param adc group
 */
void ADC_Calibration(ADC_Group group)
{
    assert_param(group < ADC_Count);
    ADC_T * const AdcX = ADCx[group];
    
    //store origin value
    uint8_t old = (AdcX->CR2 & CR2_ADON);
    if(old == 0)
        AdcX->CR2 |= CR2_ADON;
    //wait at least two adc clock cyles
    for(volatile uint8_t i = 0; i < 16; ++i);
    //start calibration
    AdcX->CR2 |= CR2_RSTCAL;
    while(AdcX->CR2 & CR2_RSTCAL);
    AdcX->CR2 |= CR2_CAL;
    //wait calibration done
    while(AdcX->CR2 & CR2_CAL);
    
    //restore origin value
    if(old == 0)
        AdcX->CR2 &= ~CR2_ADON;
}

/**
 * @brief check if specified flag is set
 * @param adc group
 * @param flag need to check
 * @return TRUE:set FALSE:reset
 */
bool ADC_IsFlagOn(ADC_Group group, uint8_t flag)
{
    assert_param(group < ADC_Count);
    assert_param(IS_ADC_FLAG_PARAM(flag));
    ADC_T * const AdcX = ADCx[group];
    
    if(AdcX->SR & flag)
        return TRUE;
    else
        return FALSE;
    
}

/**
 * @brief clear specified flag
 * @param adc group
 * @param flag need to clear
 */
void ADC_ClrFlag(ADC_Group group, uint8_t flag)
{
    assert_param(group < ADC_Count);
    assert_param(IS_ADC_FLAG_PARAM(flag));
    ADC_T * const AdcX = ADCx[group];
    
    AdcX->SR &= ~flag;
}

/**
 * @brief set adc dual mode, only adc1 has this function
 * @param adc group
 * @param adc dual mode
 */
void ADC_SetDualMode(ADC_Group group, uint32_t mode)
{
    assert_param(group == ADC1);
    assert_param(IS_ADC_DUAL_MODE_PARAM(mode));
    
    ADC_T * const AdcX = ADCx[group];
    AdcX->CR1 &= ~CR1_DUALMOD;
    AdcX->CR1 |= mode;
}

/**
 * @brief enable or disable analog watchdog on specified channel group
 * @param adc group
 * @param channel group
 * @param enable or disable flag
 */
void ADC_EnableAnalogWatchdogOnGroup(ADC_Group group, uint8_t channel, 
                                     bool flag)
{
    assert_param(group == ADC1);
    assert_param(IS_ADC_CHANNEL_GROUP_PARAM(channel));
    
    ADC_T * const AdcX = ADCx[group];
    if(channel & ADC_CHANNEL_GROUP_REGULAR)
    {
        if(flag)
            AdcX->CR1 |= CR1_AWDEN;
        else
            AdcX->CR1 &= ~CR1_AWDEN;
    }
    
    if(channel & ADC_CHANNEL_GROUP_INJECTED)
    {
        if(flag)
            AdcX->CR1 |= CR1_JAWDEN;
        else
            AdcX->CR1 &= ~CR1_JAWDEN;
    }
}


/**
 * @brief enable or disable analog watchdog on specified channel
 * @param adc group
 * @param adc channel
 * @param enable or disable flag
 */
void ADC_EnableAnalogWatchdogOnChannel(ADC_Group group, uint8_t channel, 
                                       bool flag)
{
    assert_param(group == ADC1);
    assert_param(IS_ADC_CHANNEL_PARAM(channel));
    
    ADC_T * const AdcX = ADCx[group];
    if(flag)
    {
        AdcX->CR1 &= ~CR1_AWDCH;
        AdcX->CR1 |= channel;
        AdcX->CR1 |= CR1_AWDSGL;
    }
    else
        AdcX->CR1 &= ~CR1_AWDSGL; 
}


/**
 * @brief set discontinuous mode channel count
 * @note the count define the number of regular channels to be converted
 *       in discontinuous mode, after receiving an external trigger
 * @param adc group
 * @param channel count
 */
void ADC_SetDiscontinuousModeChannelCount(ADC_Group group, 
                                          uint32_t count)
{
    assert_param(group == ADC1);
    assert_param((count > 0) && (count <= 8));
    
    ADC_T * const AdcX = ADCx[group];
    AdcX->CR1 &= ~CR1_DISCNUM;
    AdcX->CR1 |= ((count - 1) << 13);
}

/**
 * @brief enable or disable automatic injected group conversion after regular 
 *        group conversion.
 * @param adc group
 * @param enable or diable flag
 */
void ADC_EnableAutoInjected(ADC_Group group, bool flag)
{
    assert_param(group == ADC1);
    
    ADC_T * const AdcX = ADCx[group];
    if(flag)
        AdcX->CR1 |= CR1_JAUTO;
    else
        AdcX->CR1 &= ~CR1_JAUTO;
}

/**
 * @brief set adc convert mode
 * @param adc group
 * @param adc convert mode
 */
void ADC_SetConvertMode(ADC_Group group, uint8_t mode)
{
    assert_param(group < ADC_Count);
    assert_param(IS_ADC_CONVERT_MDDE_PARAM(mode));
    
    ADC_T * const AdcX = ADCx[group];
    AdcX->CR1 &= ~CR1_JDISCEN;
    AdcX->CR1 &= ~CR1_DISCEN;
    AdcX->CR1 &= ~CR1_SCAN;
    AdcX->CR2 &= ~CR2_CONT;

    switch(mode)
    {
    case ADC_CONVERT_MODE_SINGLE:
        AdcX->CR2 &= ~CR2_CONT;
        break;
    case ADC_CONVERT_MODE_CONTINUOUS:
        AdcX->CR2 |= CR2_CONT;
        break;
    case ADC_CONVERT_MODE_SCAN:
        AdcX->CR1 |= CR1_SCAN;
        break;
    case ADC_CONVERT_MODE_DISCONTINUOUS_INJECTED:
        AdcX->CR1 |= CR1_JDISCEN;
        break;
    case ADC_CONVERT_MODE_DISCONTINUOUS_REGULAR:
        AdcX->CR1 |= CR1_DISCEN;
        break;
    default:
        break;
    }
}

/**
 * @brief enable or disable adc interrupt
 * @param adc group
 * @param interrupt flag
 * @param enable or disable flag
 */
void ADC_EnableInt(ADC_Group group, uint32_t intFlag, bool flag)
{
    assert_param(group < ADC_Count);
    assert_param(IS_ADC_IT_PARAM(intFlag));
    
    ADC_T * const AdcX = ADCx[group];
    
    if(flag)
        AdcX->CR1 &= ~intFlag;
    else
        AdcX->CR1 |= intFlag;
}

/**
 * @brief enable or disable internal temperature sensor
 * @param enable or disable flag
 */
void ADC_EnableTemperensor(ADC_Group group, bool flag)
{
    assert_param(group == ADC1);
    
    ADC_T * const AdcX = ADCx[group];
    
    if(flag)
        AdcX->CR2 |= CR2_TSVREFE;
    else
        AdcX->CR2 &= ~CR2_TSVREFE;
}

/**
 * @brief trigger conversion on specified channel group
 * @param channel group
 * @param trigger or not
 */
void ADC_InternalTriggerConversion(ADC_Group group, uint8_t channel)
{
    assert_param(group < ADC_Count);
    assert_param(IS_ADC_CHANNEL_GROUP_PARAM(channel));
    
    ADC_T * const AdcX = ADCx[group];
    if(channel & ADC_CHANNEL_GROUP_REGULAR)
        AdcX->CR2 |= CR2_SWSTART;
    
    if(channel & ADC_CHANNEL_GROUP_INJECTED)
        AdcX->CR2 |= CR2_JSWSTART;
}

/**
 * @brief enable or disable trigger conversion mode on specified channel group
 * @param channel group
 * @param trigger or not
 */
void ADC_EnableExternalTriggerOnGroup(ADC_Group group, uint8_t channel,
                                      bool flag)
{
    assert_param(group < ADC_Count);
    assert_param(IS_ADC_CHANNEL_GROUP_PARAM(channel));
    
    ADC_T * const AdcX = ADCx[group];
    if(channel & ADC_CHANNEL_GROUP_REGULAR)
    {
        if(flag)
            AdcX->CR2 |= CR2_EXTTRIG;
        else
            AdcX->CR2 &= ~CR2_EXTTRIG;
    }
    
    if(channel & ADC_CHANNEL_GROUP_INJECTED)
    {
        if(flag)
            AdcX->CR2 |= CR2_JEXTTRIG;
        else
            AdcX->CR2 &= ~CR2_JEXTTRIG;
    }
}

/**
 * @brief set adc trigger mode
 * @param channel group
 * @param tigger mode
 */
void ADC_SetTriggerMode(ADC_Group group, uint8_t channel,
                        uint32_t mode)
{
    assert_param(group < ADC_Count);
    assert_param(IS_ADC_CHANNEL_GROUP_PARAM(channel));
    assert_param(IS_ADC_TRIGGER_REGULAR_PARAM(mode));
    
    ADC_T * const AdcX = ADCx[group];
    if(channel & ADC_CHANNEL_GROUP_REGULAR)
    {
        AdcX->CR2 &= ~CR2_EXTSEL;
        AdcX->CR2 |= mode;
    }
    else if(channel & ADC_CHANNEL_GROUP_INJECTED)
    {
        AdcX->CR2 &= ~CR2_JEXTSEL;
        AdcX->CR2 |= mode;
    }
}

/**
 * @brief set adc data alignment mode
 * @param adc group
 * @param alignment mode
 */
void ADC_SetDataAlignment(ADC_Group group, uint32_t alignment)
{
    assert_param(group < ADC_Count);
    assert_param(IS_ADC_ALIGNMENT_PARAM(alignment));
    
    ADC_T * const AdcX = ADCx[group];
    AdcX->CR2 &= ~CR2_ALIGN;
    AdcX->CR2 |= alignment;
}


/**
 * @brief enable or disable dma mode
 * @param adc group
 * @param enable or disable flag
 */
void ADC_EnableDMA(ADC_Group group, bool flag)
{
    assert_param(group < ADC_Count);
    
    ADC_T * const AdcX = ADCx[group]; 
    if(flag)
        AdcX->CR2 |= CR2_DMA;
    else
        AdcX->CR2 &= ~CR2_DMA;
}

/**
 * @brief set adc channle sample cycles
 * @param adc group
 * @param adc channel
 * @param sample cycles
 */
void ADC_SetSampleCycle(ADC_Group group, uint8_t channel,
                        uint8_t cycle)
{
    assert_param(group < ADC_Count);
    assert_param(IS_ADC_CHANNEL_PARAM(channel));
    assert_param(IS_ADC_SAMPLE_CYCLE_PARAM(cycle));
    
    ADC_T * const AdcX = ADCx[group];
    uint32_t tmpCycle = cycle;
    if(channel >= 10)
    {
        AdcX->SMPR1 &= ~(0x07 << ((channel - 10) * 3));
        AdcX->SMPR1 |= (tmpCycle << ((channel - 10) * 3));
    }
    else
    {
        AdcX->SMPR2 &= ~(0x07 << (channel * 3));
        AdcX->SMPR2 |= (tmpCycle << (channel * 3));
    }
}

/**
 * @brief set user-defined injected data offset value 
 * @note The injected group channels converted data value is decreased by the 
         user-defined offset written in the ADC_JOFRx registers so the result 
         can be a negative value
 *
 *
 */
void ADC_SetInjectedDataOffset(ADC_Group group, uint8_t channel,
                               uint16_t offset)
{
    assert_param(group < ADC_Count);
    assert_param(IS_ADC_INJECTED_CHANNEL_PARAM(channel));
    assert_param(offset < (1 << 12));
 
    ADC_T * const AdcX = ADCx[group];
    AdcX->JOFR[channel] = offset;
}

/**
 * @brief set analog watchdog threshold
 * @param adc group 
 * @param high threshold
 * @param low threshold
 */
void ADC_SetWatchdogThreshold(ADC_Group group, uint16_t high, 
                              uint16_t low)
{
    assert_param(group < ADC_Count);
    assert_param(high < (1 << 12));
    assert_param(low < (1 << 12));
    
    ADC_T * const AdcX = ADCx[group];
    AdcX->HTR = high;
    AdcX->LTR = low;
}

/**
 * @brief set regular sequence length
 * @param adc group
 * @param regular sequence length
 */
void ADC_SetRegularSequenceLength(ADC_Group group, uint32_t length)
{
    assert_param(group < ADC_Count);
    assert_param((length > 0) && (length < 17));
    
    ADC_T * const AdcX = ADCx[group];
    AdcX->SQR[0] &= ~SQR1_L;
    AdcX->SQR[0] |= ((length - 1) << 20);
}

/**
 * @brief set regular channel map
 * @param adc group
 * @param regular channel
 * @param adc channel
 */
void ADC_SetRegularChannel(ADC_Group group, uint8_t regularChannel,
                           uint8_t adcChannel)
{
    assert_param(group < ADC_Count);
    assert_param(regularChannel < 16);
    assert_param(IS_ADC_CHANNEL_PARAM(adcChannel));
    
    ADC_T * const AdcX = ADCx[group];
    AdcX->SQR[2 - regularChannel / 6] &= ~(0x1f << ((regularChannel % 6) * 5));
    AdcX->SQR[2 - regularChannel / 6] |= (adcChannel << ((regularChannel % 6) * 5));
}


/**
 * @brief set injected sequence length
 * @param adc group
 * @param regular sequence length
 */
void ADC_SetInjectedSequenceLength(ADC_Group group, uint32_t length)
{
    assert_param(group < ADC_Count);
    assert_param((length > 0) && (length < 5));
    
    ADC_T * const AdcX = ADCx[group];
    AdcX->JSQR &= ~JSQR_JL;
    AdcX->JSQR |= ((length - 1) << 20);
}

/**
 * @brief set injected channel map
 * @param adc group
 * @param injected channel
 * @param adc channel
 */
void ADC_SetInjectedChannel(ADC_Group group, uint8_t injectedChannel,
                           uint8_t adcChannel)
{
    assert_param(group < ADC_Count);
    assert_param(injectedChannel < 4);
    assert_param(IS_ADC_CHANNEL_PARAM(adcChannel));
    
    ADC_T * const AdcX = ADCx[group];
    AdcX->JSQR &= ~(0x1f << (injectedChannel * 5));
    AdcX->JSQR |= ~(adcChannel << (injectedChannel * 5));
}

/**
 * @brief get injected channel convert value
 * @param adc group
 * @param injected channel
 * @return converted value
 */
uint16_t ADC_GetInjectedValue(ADC_Group group, uint8_t channel)
{
    assert_param(group < ADC_Count);
    assert_param(IS_ADC_INJECTED_CHANNEL_PARAM(channel));
    
    ADC_T * const AdcX = ADCx[group];
    
    return (AdcX->JDR[channel] & 0xffff);
}


/**
 * @brief get regular channel convert value
 * @param adc group
 * @return converted value
 */
uint16_t ADC_GetRegularValue(ADC_Group group)
{
    assert_param(group < ADC_Count);
    
    ADC_T * const AdcX = ADCx[group];
    return (AdcX->DR & 0xffff);
}

/**
 * @brief get dual mode adc2 convert value
 * @return converted value
 */
uint16_t ADC_GetDualModeADC2Value(void)
{    
    ADC_T * const AdcX = ADCx[ADC1];
    return ((AdcX->DR & 0xffff0000) >> 16);
}
