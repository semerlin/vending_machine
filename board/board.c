#include "board.h"
#include "sysdef.h"
#include "stm32f10x_cfg.h"
#include "pinconfig.h"
#include "string.h"
#include "delay.h"


static void clockInit(void);
static void miscInit(void);
static void adcInit(void);

//init function
typedef void (*initFuc)(void);

//init sequence
initFuc initSequence[] = 
{
    clockInit,
    pinInit,
    adcInit,
    miscInit,
};

/**
 * @brief init board
 */
void board_init(void)
{
    uint32 len = sizeof(initSequence) / sizeof(initFuc);
    for(int i = 0; i < len; ++i)
    {
        assert_param(initSequence[i] != NULL);
        initSequence[i]();
        //TODO put some log information here
    }

    return;
}

/**
 * @brief board clock init
 */
static void clockInit(void)
{
    //config rcc
    RCC_DeInit();
    BOOL flag = RCC_StartupHSE();
    
    //config flash latency
    FLASH_SetLatency(FLASH_LATENCY_TWO);
    Flash_EnablePrefetch(TRUE);
    
    //config HCLK(72MHz), PCLK1(36MHz), PCLK2(72MHz)
    RCC_HCLKPrescalerFromSYSCLK(RCC_HPRE_SYSCLK);
    RCC_PCLK1PrescalerHCLK(RCC_PPRE1_HCLK_DIV2);
    RCC_PCLK2PrescalerFromHCLK(RCC_PPRE2_HCLK);
    
    //config PLL(72MHz)
    uint32 retVal = RCC_SetSysclkUsePLL(72000000, TRUE, 8000000);
    RCC_SystemClockSwitch(RCC_SW_PLL);
    //Wait till PLL is used as system clock source
	while( RCC_GetSystemClock() != 0x02);
    
    //config adc slock(9MHz)
    RCC_ADCPrescalerFromPCLK2(RCC_ADC_PCLK_DIV8);
    
    //setup interrupt grouping, we only use group priority
    SCB_SetPriorityGrouping(3);
}

/**
* @brief board adc init
*/
static void adcInit(void)
{
    ADC_SetRegularSequenceLength(ADC1, 1);
    ADC_SetConvertMode(ADC1, ADC_CONVERT_MODE_SINGLE);
    ADC_SetSampleCycle(ADC1, ADC_CHANNEL11, ADC_SAMPLE_CYCLE_55_5);
    ADC_SetSampleCycle(ADC1, ADC_CHANNEL13, ADC_SAMPLE_CYCLE_55_5);
    ADC_EnableExternalTriggerOnGroup(ADC1, ADC_CHANNEL_GROUP_REGULAR, TRUE);
    ADC_SetTriggerMode(ADC1, ADC_CHANNEL_GROUP_REGULAR, ADC_TRIGGER_REGULAR_ADC1_2_SWSTART);
    ADC_PowerOn(ADC1, TRUE);
    ADC_Calibration(ADC1);
}

/**
 * @brief board misc devices init
 */
extern void hardwareI2CInit(void);
extern void softwareI2CInit(void);

static void miscInit(void)
{
    pinSet("power");
    delayInit();
#ifdef __I2C_HARDWARE
    hardwareI2CInit();
#else
    softwareI2CInit();
#endif
}




