/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _STM32F10X_ADC_H_
 #define _STM32F10X_ADC_H_

#include "types.h"


/* adc channels */
#define ADC_CHANNEL0      (0)
#define ADC_CHANNEL1      (1)
#define ADC_CHANNEL2      (2)
#define ADC_CHANNEL3      (3)
#define ADC_CHANNEL4      (4)
#define ADC_CHANNEL5      (5)
#define ADC_CHANNEL6      (6)
#define ADC_CHANNEL7      (7)
#define ADC_CHANNEL8      (8)
#define ADC_CHANNEL9      (9)
#define ADC_CHANNEL10     (10)
#define ADC_CHANNEL11     (11)
#define ADC_CHANNEL12     (12)
#define ADC_CHANNEL13     (13)
#define ADC_CHANNEL14     (14)
#define ADC_CHANNEL15     (15)
#define ADC_CHANNEL16     (16)
#define ADC_CHANNEL17     (17)
#define IS_ADC_CHANNEL_PARAM(CHANNEL) ((CHANNEL == ADC_CHANNEL0) || \
                                   (CHANNEL == ADC_CHANNEL1) || \
                                   (CHANNEL == ADC_CHANNEL2) || \
                                   (CHANNEL == ADC_CHANNEL3) || \
                                   (CHANNEL == ADC_CHANNEL4) || \
                                   (CHANNEL == ADC_CHANNEL5) || \
                                   (CHANNEL == ADC_CHANNEL6) || \
                                   (CHANNEL == ADC_CHANNEL7) || \
                                   (CHANNEL == ADC_CHANNEL8) || \
                                   (CHANNEL == ADC_CHANNEL9) || \
                                   (CHANNEL == ADC_CHANNEL10) || \
                                   (CHANNEL == ADC_CHANNEL11) || \
                                   (CHANNEL == ADC_CHANNEL12) || \
                                   (CHANNEL == ADC_CHANNEL13) || \
                                   (CHANNEL == ADC_CHANNEL14) || \
                                   (CHANNEL == ADC_CHANNEL15) || \
                                   (CHANNEL == ADC_CHANNEL16) || \
                                   (CHANNEL == ADC_CHANNEL17))

/* SR flags */
#define ADC_FLAG_STRT           (1 << 4)
#define ADC_FLAG_JSTRT          (1 << 3)
#define ADC_FLAG_JEOC           (1 << 2)
#define ADC_FLAG_EOC            (1 << 1)
#define ADC_FLAG_AWD            (1 << 0)
#define IS_ADC_FLAG_PARAM(FLAG)   ((FLAG == ADC_FLAG_STRT) || \
                                   (FLAG == ADC_FLAG_JSTRT) || \
                                   (FLAG == ADC_FLAG_JEOC) || \
                                   (FLAG == ADC_FLAG_EOC) || \
                                   (FLAG == ADC_FLAG_AWD))

/* dual mode */
#define ADC_DUAL_MODE_INDEPENDENT                             (0x00 << 16)
#define ADC_DUAL_MODE_CombRegular_AND_Injected                (0x01 << 16)
#define ADC_DUAL_MODE_CombRegulat_AND_AlternateTrigger        (0x02 << 16)
#define ADC_DUAL_MODE_CombInjected_AND_FaseInterleaved        (0x03 << 16)
#define ADC_DUAL_MODE_CombInjected_AND_SlowInterleaved        (0x04 << 16)
#define ADC_DUAL_MODE_Injected                                (0x05 << 16)
#define ADC_DUAL_MODE_Regular                                 (0x06 << 16)
#define ADC_DUAL_MODE_FastInterleaved                         (0x07 << 16)
#define ADC_DUAL_MODE_SlowInterleaved                         (0x08 << 16)
#define ADC_DUAL_MODE_AlternateTrigger                        (0x09 << 16)

#define IS_ADC_DUAL_MODE_PARAM(MODE)   \
           ((MODE == ADC_DUAL_MODE_INDEPENDENT) || \
            (MODE == ADC_DUAL_MODE_CombRegular_AND_Injected) || \
            (MODE == ADC_DUAL_MODE_CombRegulat_AND_AlternateTrigger) || \
            (MODE == ADC_DUAL_MODE_CombInjected_AND_FaseInterleaved) || \
            (MODE == ADC_DUAL_MODE_CombInjected_AND_SlowInterleaved) || \
            (MODE == ADC_DUAL_MODE_Injected) || \
            (MODE == ADC_DUAL_MODE_Regular) || \
            (MODE == ADC_DUAL_MODE_FastInterleaved) || \
            (MODE == ADC_DUAL_MODE_SlowInterleaved) || \
            (MODE == ADC_DUAL_MODE_AlternateTrigger))
               
               
/* adc channel group */
#define ADC_CHANNEL_GROUP_REGULAR     (0x01)
#define ADC_CHANNEL_GROUP_INJECTED    (0x02)
#define IS_ADC_CHANNEL_GROUP_PARAM(CHANNEL)   \
               ((CHANNEL == ADC_CHANNEL_GROUP_REGULAR) || \
                (CHANNEL == ADC_CHANNEL_GROUP_INJECTED))


/* adc convert mode */
#define ADC_CONVERT_MODE_SINGLE                   (0x00)
#define ADC_CONVERT_MODE_CONTINUOUS               (0x01)
#define ADC_CONVERT_MODE_SCAN                     (0x02)
#define ADC_CONVERT_MODE_DISCONTINUOUS_INJECTED   (0x03)
#define ADC_CONVERT_MODE_DISCONTINUOUS_REGULAR    (0x04)
#define IS_ADC_CONVERT_MDDE_PARAM(MODE) \
               ((MODE == ADC_CONVERT_MODE_SINGLE) || \
                (MODE == ADC_CONVERT_MODE_CONTINUOUS) || \
                (MODE == ADC_CONVERT_MODE_SCAN) || \
                (MODE == ADC_CONVERT_MODE_DISCONTINUOUS_INJECTED) || \
                (MODE == ADC_CONVERT_MODE_DISCONTINUOUS_REGULAR))


/* adc interrupt */
#define ADC_IT_JEOC   (1 << 7)
#define ADC_IT_AWD    (1 << 6)
#define ADC_IT_EOC    (1 << 5)
#define IS_ADC_IT_PARAM(IT) ((IT == ADC_IT_JEOC) || \
                             (IT == ADC_IT_AWD) || \
                             (IT == ADC_IT_EOC))


/* adc regular trigger */
#define ADC_TRIGGER_REGULAR_ADC1_2_TIM1_CC1      (0 << 17)
#define ADC_TRIGGER_REGULAR_ADC1_2_TIM1_CC2      (1 << 17)
#define ADC_TRIGGER_REGULAR_ADC1_2_TIM2_CC3      (2 << 17)
#define ADC_TRIGGER_REGULAR_ADC1_2_TIM2_CC2      (3 << 17)
#define ADC_TRIGGER_REGULAR_ADC1_2_TIM3_TRGO     (4 << 17)
#define ADC_TRIGGER_REGULAR_ADC1_2_TIM4_CC4      (5 << 17)
#define ADC_TRIGGER_REGULAR_ADC1_2_EXTI11        (6 << 17)
#define ADC_TRIGGER_REGULAR_ADC1_2_TIM8_TRGO     (6 << 17)
#define ADC_TRIGGER_REGULAR_ADC1_2_SWSTART       (7 << 17)
#define ADC_TRIGGER_REGULAR_ADC3_TIM3_CC1      (0 << 17)
#define ADC_TRIGGER_REGULAR_ADC3_TIM2_CC3      (1 << 17)
#define ADC_TRIGGER_REGULAR_ADC3_TIM1_CC3      (2 << 17)
#define ADC_TRIGGER_REGULAR_ADC3_TIM8_CC1      (3 << 17)
#define ADC_TRIGGER_REGULAR_ADC3_TIM8_TRGO     (4 << 17)
#define ADC_TRIGGER_REGULAR_ADC3_TIM5_CC1      (5 << 17)
#define ADC_TRIGGER_REGULAR_ADC3_TIM5_CC3      (6 << 17)
#define ADC_TRIGGER_REGULAR_ADC3_SWSTART       (7 << 17)
#define IS_ADC_TRIGGER_REGULAR_PARAM(TRIGGER) \
       ((TRIGGER == ADC_TRIGGER_REGULAR_ADC1_2_TIM1_CC1) || \
        (TRIGGER == ADC_TRIGGER_REGULAR_ADC1_2_TIM1_CC2) || \
        (TRIGGER == ADC_TRIGGER_REGULAR_ADC1_2_TIM2_CC3) || \
        (TRIGGER == ADC_TRIGGER_REGULAR_ADC1_2_TIM2_CC2) || \
        (TRIGGER == ADC_TRIGGER_REGULAR_ADC1_2_TIM3_TRGO) || \
        (TRIGGER == ADC_TRIGGER_REGULAR_ADC1_2_TIM4_CC4) || \
        (TRIGGER == ADC_TRIGGER_REGULAR_ADC1_2_EXTI11) || \
        (TRIGGER == ADC_TRIGGER_REGULAR_ADC1_2_TIM8_TRGO) || \
        (TRIGGER == ADC_TRIGGER_REGULAR_ADC1_2_SWSTART) || \
        (TRIGGER == ADC_TRIGGER_REGULAR_ADC3_TIM3_CC1) || \
        (TRIGGER == ADC_TRIGGER_REGULAR_ADC3_TIM2_CC3) || \
        (TRIGGER == ADC_TRIGGER_REGULAR_ADC3_TIM1_CC3) || \
        (TRIGGER == ADC_TRIGGER_REGULAR_ADC3_TIM8_CC1) || \
        (TRIGGER == ADC_TRIGGER_REGULAR_ADC3_TIM8_TRGO) || \
        (TRIGGER == ADC_TRIGGER_REGULAR_ADC3_TIM5_CC1) || \
        (TRIGGER == ADC_TRIGGER_REGULAR_ADC3_TIM5_CC3) || \
        (TRIGGER == ADC_TRIGGER_REGULAR_ADC3_SWSTART))
            
            
/* adc inject trigger */            
#define ADC_TRIGGER_INJECTED_ADC1_2_TIM1_TRGO       (0 << 17)
#define ADC_TRIGGER_INJECTED_ADC1_2_TIM1_CC4        (1 << 17)
#define ADC_TRIGGER_INJECTED_ADC1_2_TIM2_TRGO       (2 << 17)
#define ADC_TRIGGER_INJECTED_ADC1_2_TIM2_CC1        (3 << 17)
#define ADC_TRIGGER_INJECTED_ADC1_2_TIM3_CC4        (4 << 17)
#define ADC_TRIGGER_INJECTED_ADC1_2_TIM4_TRGO       (5 << 17)
#define ADC_TRIGGER_INJECTED_ADC1_2_EXTI15          (6 << 17)
#define ADC_TRIGGER_INJECTED_ADC1_2_TIM8_CC4        (6 << 17)
#define ADC_TRIGGER_INJECTED_ADC1_2_JSWSTART        (7 << 17)
#define ADC_TRIGGER_INJECTED_ADC3_TIM1_TRGO         (0 << 17)
#define ADC_TRIGGER_INJECTED_ADC3_TIM1_CC4          (1 << 17)
#define ADC_TRIGGER_INJECTED_ADC3_TIM4_CC3          (2 << 17)
#define ADC_TRIGGER_INJECTED_ADC3_TIM8_CC2          (3 << 17)
#define ADC_TRIGGER_INJECTED_ADC3_TIM8_CC4          (4 << 17)
#define ADC_TRIGGER_INJECTED_ADC3_TIM5_TRGO         (5 << 17)
#define ADC_TRIGGER_INJECTED_ADC3_TIM5_CC4          (6 << 17)
#define ADC_TRIGGER_INJECTED_ADC3_JSWSTART          (7 << 17)
#define IS_ADC_TRIGGER_INJECTED_PARAM(TRIGGER) \
       ((TRIGGER == ADC_TRIGGER_INJECTED_ADC1_2_TIM1_TRGO) || \
        (TRIGGER == ADC_TRIGGER_INJECTED_ADC1_2_TIM1_CC4) || \
        (TRIGGER == ADC_TRIGGER_INJECTED_ADC1_2_TIM2_TRGO) || \
        (TRIGGER == ADC_TRIGGER_INJECTED_ADC1_2_TIM2_CC1) || \
        (TRIGGER == ADC_TRIGGER_INJECTED_ADC1_2_TIM3_CC4) || \
        (TRIGGER == ADC_TRIGGER_INJECTED_ADC1_2_TIM4_TRGO) || \
        (TRIGGER == ADC_TRIGGER_INJECTED_ADC1_2_EXTI15) || \
        (TRIGGER == ADC_TRIGGER_INJECTED_ADC1_2_TIM8_CC4) || \
        (TRIGGER == ADC_TRIGGER_INJECTED_ADC1_2_JSWSTART) || \
        (TRIGGER == ADC_TRIGGER_INJECTED_ADC3_TIM1_TRGO) || \
        (TRIGGER == ADC_TRIGGER_INJECTED_ADC3_TIM1_CC4) || \
        (TRIGGER == ADC_TRIGGER_INJECTED_ADC3_TIM4_CC3) || \
        (TRIGGER == ADC_TRIGGER_INJECTED_ADC3_TIM8_CC2) || \
        (TRIGGER == ADC_TRIGGER_INJECTED_ADC3_TIM8_CC4) || \
        (TRIGGER == ADC_TRIGGER_INJECTED_ADC3_TIM5_TRGO) || \
        (TRIGGER == ADC_TRIGGER_INJECTED_ADC3_TIM5_CC4) || \
        (TRIGGER == ADC_TRIGGER_INJECTED_ADC3_JSWSTART))
            
 /* data alignment */
#define ADC_ALIGNMENT_RIGHT  (0x00)
#define ADC_ALIGNMENT_LEFT   (1 << 11)
#define IS_ADC_ALIGNMENT_PARAM(ALI) ((ALI == ADC_ALIGNMENT_RIGHT) || \
                                     (ALI == ADC_ALIGNMENT_LEFT))

/* sample cycle */
#define ADC_SAMPLE_CYCLE_1_5          (0x00)
#define ADC_SAMPLE_CYCLE_7_5          (0x01)
#define ADC_SAMPLE_CYCLE_13_5         (0x02)
#define ADC_SAMPLE_CYCLE_28_5         (0x03)
#define ADC_SAMPLE_CYCLE_41_5         (0x04)
#define ADC_SAMPLE_CYCLE_55_5         (0x05)
#define ADC_SAMPLE_CYCLE_71_5         (0x06)
#define ADC_SAMPLE_CYCLE_239_5        (0x07)
#define IS_ADC_SAMPLE_CYCLE_PARAM(SAMPLE) \
       ((SAMPLE == ADC_SAMPLE_CYCLE_1_5) || \
        (SAMPLE == ADC_SAMPLE_CYCLE_7_5) || \
        (SAMPLE == ADC_SAMPLE_CYCLE_13_5) || \
        (SAMPLE == ADC_SAMPLE_CYCLE_28_5) || \
        (SAMPLE == ADC_SAMPLE_CYCLE_41_5) || \
        (SAMPLE == ADC_SAMPLE_CYCLE_55_5) || \
        (SAMPLE == ADC_SAMPLE_CYCLE_71_5) || \
        (SAMPLE == ADC_SAMPLE_CYCLE_239_5))

/* injected channel */
#define ADC_INJECTED_CHANNEL1 (0x00)
#define ADC_INJECTED_CHANNEL2 (0x01)
#define ADC_INJECTED_CHANNEL3 (0x02)
#define ADC_INJECTED_CHANNEL4 (0x03)
#define IS_ADC_INJECTED_CHANNEL_PARAM(CHANNEL) \
       ((CHANNEL == ADC_INJECTED_CHANNEL1) || \
        (CHANNEL == ADC_INJECTED_CHANNEL2) || \
        (CHANNEL == ADC_INJECTED_CHANNEL3) || \
        (CHANNEL == ADC_INJECTED_CHANNEL4))
           
           
/* adc group definition */
typedef enum
{
    ADC1,
    ADC2,
    ADC3,
    ADC_Count,
}ADC_Group;



/* interface */
void ADC_PowerOn(ADC_Group group, bool flag);
void ADC_Calibration(ADC_Group group);
bool ADC_IsFlagOn(ADC_Group group, uint8_t flag);
void ADC_ClrFlag(ADC_Group group, uint8_t flag);
void ADC_SetDualMode(ADC_Group group, uint32_t mode);
void ADC_EnableAnalogWatchdogOnGroup(ADC_Group group, uint8_t channel, 
                                     bool flag);
void ADC_EnableAnalogWatchdogOnChannel(ADC_Group group, uint8_t channel, 
                                       bool flag);
void ADC_SetDiscontinuousModeChannelCount(ADC_Group group, 
                                          uint32_t count);
void ADC_EnableAutoInjected(ADC_Group group, bool flag);
void ADC_SetConvertMode(ADC_Group group, uint8_t mode);
void ADC_EnableInt(ADC_Group group, uint32_t intFlag, bool flag);
void ADC_EnableTemperensor(ADC_Group group, bool flag);
void ADC_InternalTriggerConversion(ADC_Group group, uint8_t channel);
void ADC_EnableExternalTriggerOnGroup(ADC_Group group, uint8_t channel,
                                      bool flag);
void ADC_SetTriggerMode(ADC_Group group, uint8_t channel,
                        uint32_t mode);
void ADC_SetDataAlignment(ADC_Group group, uint32_t alignment);
void ADC_EnableDMA(ADC_Group group, bool flag);
void ADC_SetSampleCycle(ADC_Group group, uint8_t channel,
                        uint8_t cycle);
void ADC_SetInjectedDataOffset(ADC_Group group, uint8_t channel,
                               uint16_t offset);
void ADC_SetWatchdogThreshold(ADC_Group group, uint16_t high, 
                              uint16_t low);
void ADC_SetRegularSequenceLength(ADC_Group group, uint32_t length);
void ADC_SetRegularChannel(ADC_Group group, uint8_t regularChannel,
                           uint8_t adcChannel);
void ADC_SetInjectedSequenceLength(ADC_Group group, uint32_t length);
void ADC_SetInjectedChannel(ADC_Group group, uint8_t injectedChannel,
                           uint8_t adcChannel);
uint16_t ADC_GetInjectedValue(ADC_Group group, uint8_t channel);
uint16_t ADC_GetRegularValue(ADC_Group group);
uint16_t ADC_GetDualModeADC2Value(void);


#endif /* _STM32F10X_ADC_H_ */
