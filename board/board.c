#include <string.h>
#include "board.h"
#include "types.h"
#include "stm32f10x_cfg.h"
#include "pinconfig.h"


static void clockInit(void);
static void miscInit(void);

//init function
typedef void (*initFuc)(void);

//init sequence
initFuc initSequence[] = 
{
    clockInit,
    pinInit,
    miscInit,
};

/**
 * @brief init board
 */
void board_init(void)
{
    uint32_t len = sizeof(initSequence) / sizeof(initFuc);
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
    bool flag = RCC_StartupHSE();
    
    //config flash latency
    FLASH_SetLatency(FLASH_LATENCY_TWO);
    Flash_EnablePrefetch(TRUE);
    
    //config HCLK(72MHz), PCLK1(36MHz), PCLK2(72MHz)
    RCC_HCLKPrescalerFromSYSCLK(RCC_HPRE_SYSCLK);
    RCC_PCLK1PrescalerHCLK(RCC_PPRE1_HCLK_DIV2);
    RCC_PCLK2PrescalerFromHCLK(RCC_PPRE2_HCLK);
    
    //config PLL(72MHz)
    uint32_t retVal = RCC_SetSysclkUsePLL(72000000, TRUE, 8000000);
    RCC_SystemClockSwitch(RCC_SW_PLL);
    //Wait till PLL is used as system clock source
	while( RCC_GetSystemClock() != 0x02);
    
    //config adc slock(9MHz)
    RCC_ADCPrescalerFromPCLK2(RCC_ADC_PCLK_DIV8);
    
    //setup interrupt grouping, we only use group priority
    SCB_SetPriorityGrouping(3);
}

static void miscInit(void)
{
    pinSet("power");
}




