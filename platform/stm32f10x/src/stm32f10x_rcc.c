/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "stm32f10x_rcc.h"
#include "stm32f10x_map.h"
#include "stm32f10x_cfg.h"


/* RCC structure definition */
typedef struct
{
    volatile uint32_t CR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t APB1RSTR;
    volatile uint16_t AHBENR;
    uint16_t RESERVED0;
    volatile uint32_t APB2ENR;
    volatile uint32_t APB1ENR;
    volatile uint32_t BDCR;
    volatile uint32_t CSR;  
}RCC_T;

RCC_T *RCC = (RCC_T *)RCC_BASE;



/*  RCC register bit band */
#define RCC_OFFSET (RCC_BASE - PERIPH_BASE)
/*  CR register bit band */
#define CR_OFFSET (RCC_OFFSET + 0x00)
#define CR_HSION (PERIPH_BB_BASE + CR_OFFSET * 32 + 0x00 * 4)
#define CR_HSIRDY (PERIPH_BB_BASE + CR_OFFSET * 32 + 0x01 * 4)
#define CR_HSEON  (PERIPH_BB_BASE + CR_OFFSET * 32 + 16 * 4)
#define CR_HSERDY  (PERIPH_BB_BASE + CR_OFFSET * 32 + 17 * 4)
#define CR_HSEBYP  (PERIPH_BB_BASE + CR_OFFSET * 32 + 18 * 4)
#define CR_CSSON  (PERIPH_BB_BASE + CR_OFFSET * 32 + 19 * 4)
#define CR_PLLON (PERIPH_BB_BASE + CR_OFFSET * 32 + 24 * 4)
#define CR_PLLRDY  (PERIPH_BB_BASE + CR_OFFSET * 32 + 25 * 4)


/*  CFGRR寄存器位带别名区定义 */
#define CFGR_OFFSET (RCC_OFFSET + 0x04)
#define CFGR_USBPRE (PERIPH_BB_BASE + CFGR_OFFSET * 32 + 22 * 4)
#define CFGR_PLLSRC (PERIPH_BB_BASE + CFGR_OFFSET * 32 + 16 * 4)
#define CFGR_PLLXTPRE (PERIPH_BB_BASE + CFGR_OFFSET * 32 + 17 * 4)


/*  CIR寄存器位带别名区定义 */
#define CIR_OFFSET (RCC_OFFSET + 0x08)
#define CIR_CSSC (PERIPH_BB_BASE + CIR_OFFSET * 32 + 23 * 4)
#define CIR_PLLRDYC (PERIPH_BB_BASE + CIR_OFFSET * 32 + 20 * 4)
#define CIR_HSERDYC (PERIPH_BB_BASE + CIR_OFFSET * 32 + 19 * 4)
#define CIR_HSIRDYC (PERIPH_BB_BASE + CIR_OFFSET * 32 + 18 * 4)
#define CIR_LSERDYC (PERIPH_BB_BASE + CIR_OFFSET * 32 + 17 * 4)
#define CIR_LSIRDYC (PERIPH_BB_BASE + CIR_OFFSET * 32 + 16 * 4)
#define CIR_PLLRDYIE (PERIPH_BB_BASE + CIR_OFFSET * 32 + 12 * 4)
#define CIR_HSERDYIE (PERIPH_BB_BASE + CIR_OFFSET * 32 + 11 * 4)
#define CIR_HSIRDYIE (PERIPH_BB_BASE + CIR_OFFSET * 32 + 10 * 4)
#define CIR_LSERDYIE (PERIPH_BB_BASE + CIR_OFFSET * 32 + 9 * 4)
#define CIR_LSIRDYIE (PERIPH_BB_BASE + CIR_OFFSET * 32 + 8 * 4)
#define CIR_CSSF (PERIPH_BB_BASE + CIR_OFFSET * 32 + 7 * 4)
#define CIR_PLLRDYF (PERIPH_BB_BASE + CIR_OFFSET * 32 + 4 * 4)
#define CIR_HSERDYF (PERIPH_BB_BASE + CIR_OFFSET * 32 + 3 * 4)
#define CIR_HSIRDYF (PERIPH_BB_BASE + CIR_OFFSET * 32 + 2 * 4)
#define CIR_LSERDYF (PERIPH_BB_BASE + CIR_OFFSET * 32 + 1 * 4)
#define CIR_LSIRDYF (PERIPH_BB_BASE + CIR_OFFSET * 32 + 0 * 4)

/*  BDCR寄存器位带别名区定义 */
#define BDCR_OFFSET (RCC_OFFSET + 0x20)
#define BDCR_BDRST (PERIPH_BB_BASE + BDCR_OFFSET * 32 + 16 * 4)
#define BDCR_RTCEN (PERIPH_BB_BASE + BDCR_OFFSET * 32 + 15 * 4)
#define BDCR_LSEBYP (PERIPH_BB_BASE + BDCR_OFFSET * 32 + 2 * 4)
#define BDCR_LSERDY (PERIPH_BB_BASE + BDCR_OFFSET * 32 + 1 * 4)
#define BDCR_LSEON (PERIPH_BB_BASE + BDCR_OFFSET * 32 + 0 * 4)


/*  CSR寄存器位带别名区定义 */
#define CSR_OFFSET (RCC_OFFSET + 0x24)
#define CSR_RMVF (PERIPH_BB_BASE + CSR_OFFSET * 32 + 24 * 4)
#define CSR_LSIRDY (PERIPH_BB_BASE + CSR_OFFSET * 32 + 1 * 4)
#define CSR_LSION (PERIPH_BB_BASE + CSR_OFFSET * 32 + 0 * 4)





/* osc ready cycle */
#define OSC_StableCycle    20

//CR register
#define CR_HSITRIM        (0x1f << 3)
#define CR_HSICAL         (0xff << 8)


//CFGR寄存器
#define CFGR_MCO     (0x07 << 24)
#define CFGR_SW      (0x03)
#define CFGR_SWS     (0x03 << 2)
#define CFGR_HPRE    (0x0f << 4)
#define CFGR_PPRE1   (0x07 << 8)
#define CFGR_PPRE2   (0x07 << 11)
#define CFGR_ADCPRE  (0x03 << 14)
#define CFGR_PLLMUL  (0x0f << 18)

//BDCR寄存器
#define BDCR_RTCSEL  (0x03 << 8)

//CSR寄存器
#define CSR_RESET    (0x3f << 26)

#define HSI_CLOCK   (8000000)


static uint32_t g_sysclk = 72000000;
static uint32_t g_hclk = 72000000;
static uint32_t g_pclk1 = 36000000;
static uint32_t g_pclk2 = 72000000;

/**
* @brief deinit rcc module
*/
void RCC_DeInit(void)
{
    /* Disable APB2 Peripheral Reset */
    RCC->APB2RSTR = 0x00000000;

    /* Disable APB1 Peripheral Reset */
    RCC->APB1RSTR = 0x00000000;

    /* FLITF and SRAM Clock ON */
    RCC->AHBENR = 0x00000014;

    /* Disable APB2 Peripheral Clock */
    RCC->APB2ENR = 0x00000000;

    /* Disable APB1 Peripheral Clock */
    RCC->APB1ENR = 0x00000000;

    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001;

    /* Reset SW[1:0], HPRE[3:0], PPRE1[2:0], PPRE2[2:0], ADCPRE[1:0] and MCO[2:0] bits*/
    RCC->CFGR &= 0xF8FF0000;

    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= 0xFEF6FFFF;

    /* Reset HSEBYP bit */
    RCC->CR &= 0xFFFBFFFF;

    /* Reset PLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE bits */
    RCC->CFGR &= 0xFF80FFFF;

    /* Disable all interrupts */
    RCC->CIR = 0x00000000;
}




/**
 * @brief startup internal high speed clock
 * @return TRUE: success FALSE: failed
 */
bool RCC_StartupHSI(void)
{
    uint32_t waitCount = 0;
	volatile uint32_t i = 0;
	
    //check if already enabled
    if((*((volatile uint32_t*)CR_HSION) == 0x01) && 
       (*((volatile uint32_t*)CR_HSIRDY) == 0x01))
    {
        return TRUE;
    }
    
    //start hsi
    *((volatile uint32_t*)CR_HSION) = 0x01;
    while((!(*((volatile uint32_t*)CR_HSIRDY))) && 
          (waitCount < OSC_StableCycle))
    {
        //wait for ready
        for(i = 0; i < 128; i++);
        waitCount ++;
    }

    if(waitCount >= OSC_StableCycle)
    {
        //start failed
        return FALSE;
    }


    return TRUE;  
}


/**
 * @brief stop hsi
 * @note this bit cannot be reset if the internal 8MHz RC is used directy or 
 *        indirecty as system clock or is selected to become the system clock
 */
void RCC_StopHSI(void)
{
    //stop hsi
    *((volatile uint32_t*)CR_HSION) = 0x00;
    
    //wait for stop
    volatile uint8_t i = 0;
    for(i = 0; i < 128; i++);
}

/**
 * @brief get hsi calibration value
 * @return hsi calibration value
 */
uint8_t RCC_GetHSICalValue(void)
{
    return ((RCC->CR & CR_HSICAL) >> 8);
}

/**
 * @brief set hsi trim value, this is a user-programmable trimming value that
 *        added to the CAL value. It can be programmed to adjust variations in 
 *        voltage and temperature that influence the frequency the HSI RC
 * @param value: trim value, this value cannot be bigger than 16
 */
void RCC_SetHSITrimValue(uint8_t value)
{
    assert_param(value <= 16);
    uint16_t tempVal = value;
    tempVal <<= 3;

    RCC->CR &= ~CR_HSITRIM;
    RCC->CR |= tempVal;
}

/**
 * @brief get HSI trim value
 * @return HSI trim value
 */
uint8_t RCC_GetHSITrimValue(void)
{
    return ((RCC->CR >> 3) & 0x1f);
}


/**
 * @brief check if HSI is on
 * @return TRUE:on FALSE:off
 */
bool RCC_IsHSIOn(void)
{
    if(*((volatile uint32_t*)CR_HSION))
        return TRUE;

    return FALSE;
}


/**
 * @brief start up HSE
 * @return TRUE: success FALSE: failed
 */
bool RCC_StartupHSE(void)
{
    uint32_t waitCount = 0;
	volatile uint16_t i = 0;
	
    //check if already enabled
    if((*((volatile uint32_t*)CR_HSERDY) == 0x01) 
       && (*((volatile uint32_t*)CR_HSEON) == 0x01))
    {
        return TRUE;
    }
    
    //start up HSE
    *((volatile uint32_t*)CR_HSEON) = 0x01;
    while((!(*((volatile uint32_t*)CR_HSERDY))) && 
          (waitCount < OSC_StableCycle))
    {
        //wait for ready
        for(i = 0; i < 128; i++);
        waitCount ++;
    }

    if(waitCount >= OSC_StableCycle)
    {
        //startup failed
        return FALSE;
    }

    return TRUE;
}


/**
 * @brief stop HSE
 */
void RCC_StopHSE(void)
{
    //stop HSE
    *((volatile uint32_t*)CR_HSEON) = 0x00;
    
    //wait for stop
    volatile uint8_t i = 0;
    for(i = 0; i < 128; i++);
}

/**
 * @brief bypass the oscillator with an external clock
 * @param flag TRUE:bypass FALSE: not bypass
 */
bool RCC_BypassHSE(bool flag)
{
    *((volatile uint32_t*)CR_HSEON) = 0x00;
    *((volatile uint32_t*)CR_HSEBYP) = 0x00;
    
    if(flag)
        *((volatile uint32_t*)CR_HSEBYP) = 0x01;

    //start HSE
    return RCC_StartupHSE();
}

/**
 * @brief check if HSE is on
 * @return TRUE:on FALSE:off
 */
bool RCC_IsHSEOn(void)
{
    if(*((volatile uint32_t*)CR_HSEON))
        return TRUE;

    return FALSE;
}

/**
 * @brief check if HSE is bypassed
 * @return TRUE: yes FALSE: no
 */
bool RCC_IsHSEBypassed(void)
{
    if(*((volatile uint32_t*)CR_HSEBYP))
        return TRUE;

    return FALSE;
}


/**
 * @brief enable or disable ccs. When CSSON is set, the clock detector is 
 *        enabled by hardware when the HSE oscillator is ready, and disabled by
 *        hardware if a HSE clock failure is detected
 * @param flag: TRUE:enable FALSE:disable
*/
void RCC_EnableClockSecurityConfig(bool flag)
{
    if(flag)
        *((volatile uint32_t*)CR_CSSON) = 0x01;
    else
        *((volatile uint32_t*)CR_CSSON) = 0x00;
}


/**
 * @brief startup pll
 * @return TRUE: success FALSE:failed
 */
bool RCC_StartupPLL(void)
{
    uint32_t waitCount = 0;
	volatile uint16_t i = 0;
    
    //check if pll has already been started
    if((*((volatile uint32_t*)CR_PLLRDY) == 0x01) && 
       (*(volatile uint32_t*)CR_PLLON == 0x01))
    {
        return TRUE;
    }

    //start pll
    *((volatile uint32_t*)CR_PLLON) = 0x01;

    while((!*((volatile uint32_t*)CR_PLLRDY)) && 
          (waitCount < OSC_StableCycle))
    {
        //wait for stable
        for(i = 0; i < 128; i++);
        waitCount ++;
    }

    if(waitCount >= OSC_StableCycle)
    {
        return FALSE;
    }

    return TRUE;
}

/**
 * @brief stop pll
 */
void RCC_StopPLL(void)
{
    *((volatile uint32_t*)CR_PLLON) = 0x00;
    
    //wait for stop
    volatile uint8_t i = 0;
    for(i = 0; i < 128; i++); 
}


/**
 * @brief check if pll is on
 * @return TRUE: on FALSE: off
 */
bool RCC_GetPLLONFlag(void)
{
    if(*((volatile uint32_t*)CR_PLLON))
        return TRUE;

    return FALSE;
}



/**
 * @brief config mco output source
 * @param source
*/
void RCC_MCOConfig(uint32_t method)
{
    assert_param(IS_RCC_MCO_PARAM(method));

    RCC->CFGR &= ~CFGR_MCO;

    RCC->CFGR |= method;
    
}

/**
 * @brief usb clock prescale config, usb clock is 48MHz
 * @param prescale value
 */
void RCC_USBPrescalerFromPLL(uint8_t config)
{
    assert_param(IS_RCC_USBPRE_PARAM(config));

    *((volatile uint32_t*)CFGR_USBPRE) = config;
}

/**
 * @brief calculate pll factor
 * @param clockIn: input clock
 * @param clockOut: output clock
 * @param div: div factor
 * @param needDiv2: return whether need div by 2
 * @return acturally output clock 
 */
static uint32_t calcPllFactor(uint32_t clockIn, uint32_t clockOut,
                               uint32_t *div, uint8_t *needDiv2)
{
    uint8_t tempDiv = 0;
    if(clockOut % clockIn == 0)
    {
        tempDiv = clockOut / clockIn;
        *div = (tempDiv & 0xff);
        *needDiv2 = 0;
    }
    else
    {
        tempDiv = clockOut / clockIn;
        if(tempDiv > 16)
        {
            *div = (tempDiv & 0xff);
            *needDiv2 = 0;
        }
        else
        {
            uint32_t delta1 = clockOut - tempDiv * clockIn;
            tempDiv = clockOut * 2 / clockIn;
            tempDiv &= 0xff;
            uint32_t delta2 = clockOut - tempDiv * clockIn;
            if(delta1 > delta2)
            {
                *div = tempDiv;
                *needDiv2 = 1;
            }
            else
            {
                *div = clockOut / clockIn;
                *needDiv2 = 0;
            }
        }
    }
    
    return *div * clockIn / (*needDiv2 + 1);
}

/**
* @brief set sysclk use pll
* @param clock: output clock
* @param useHse: whether use hse or hsi
* @param hse clock
* @return acturally output clock 
*/
uint32_t RCC_SetSysclkUsePLL(uint32_t clock, bool useHSE, 
                             uint32_t hseClock)
{
    assert_param(clock > hseClock);
    
    uint32_t div = 0;
    uint8_t needDiv2 = 0;
    uint32_t ret;
    RCC->CFGR &= ~CFGR_PLLMUL;
    RCC->CFGR &= ~CFGR_SW;

    if(useHSE)
    {
        *((volatile uint32_t *)CFGR_PLLSRC) = 0x01;
        ret = calcPllFactor(hseClock, clock, &div, &needDiv2);
    }
    else
    {
        *((volatile uint32_t *)CFGR_PLLSRC) = 0x00;
       ret = calcPllFactor(HSI_CLOCK / 2, clock, &div, &needDiv2); 
    }
    
    *((volatile uint32_t *)CFGR_PLLXTPRE) = needDiv2;
    
    assert_param(div >= 2);
    RCC->CFGR |= ((div - 2) << 18);
    
    RCC_StartupPLL();
    
    g_sysclk = ret;

    return ret;

}


/**
 * @brief set hclk prescaler
 * @param config: prescaler value 
 */
void RCC_HCLKPrescalerFromSYSCLK(uint8_t config)
{
    assert_param(IS_RCC_HPRE_PARAM(config));

    RCC->CFGR &= ~CFGR_HPRE;
    RCC->CFGR |= config;
    
    uint8_t div = (config == 0) ? 0 : ((config >> 4) - 7);
    g_hclk = (g_sysclk >> div);
}

/**
 * @brief set pclk1 prescaler
 * @param config: prescaler value 
 */
void RCC_PCLK1PrescalerHCLK(uint32_t config)
{
    assert_param(IS_RCC_PPRE1_PARAM(config));

    RCC->CFGR &= ~CFGR_PPRE1;
    RCC->CFGR |= config;
    
    uint8_t div = (config == 0) ? 0 : ((config >> 8) - 3);
    g_pclk1 = (g_hclk >> div);
}

/**
 * @brief set pclk2 prescaler
 * @param config: prescaler value 
 */
void RCC_PCLK2PrescalerFromHCLK(uint32_t config)
{
    assert_param(IS_RCC_PPRE2_PARAM(config));

    RCC->CFGR &= ~CFGR_PPRE2;
    RCC->CFGR |= config;
    
    uint8_t div = (config == 0) ? 0 : ((config >> 11) - 3);
    g_pclk2 = (g_hclk >> div);
}

/**
 * @brief get system clock
 * @return system clock 
 */
uint32_t RCC_GetSysclk(void)
{
    return g_sysclk;
}

/**
 * @brief get hclk
 * @return hclk value 
 */
uint32_t RCC_GetHCLK(void)
{
    return g_hclk;
}

/**
 * @brief get pclk1
 * @return pclk1 value 
 */
uint32_t RCC_GetPCLK1(void)
{
    return g_pclk1;
}

/**
 * @brief get pclk2
 * @return pclk2 value 
 */
uint32_t RCC_GetPCLK2(void)
{
    return g_pclk2;
}

/**
 * @brief switch system clock source
 * @param source value
 */
void RCC_SystemClockSwitch(uint8_t clock)
{
    assert_param(IS_RCC_SW_PARAM(clock));

    RCC->CFGR &= ~CFGR_SW;
    RCC->CFGR |= clock;
}
/**
 * @brief get system clock source prescaler
 * @return 0: HSI 1: HSE 2: PLL other: invalid 
 */
uint8_t RCC_GetSystemClock(void)
{
    return ((RCC->CFGR & CFGR_SWS)>>2);
}

/**
 * @brief set adc prescaler
 * @param config: prescaler value 
 */
void RCC_ADCPrescalerFromPCLK2(uint32_t config)
{
    assert_param(IS_RCC_ADC_PARAM(config));
    
    RCC->CFGR &= ~CFGR_ADCPRE;
    RCC->CFGR |= config;
}


/**
 * @brief clear clock ready interrupt flag
 * @param clock source
 */
void RCC_ClrClockIntFlag(uint8_t intSrc)
{
    if(intSrc == RCC_INT_ClockSecuty)
    {
        *(volatile uint32_t*)CIR_CSSC = 0x01;
    }
    else if(intSrc == RCC_INT_PLLReady)
    {
        *(volatile uint32_t*)CIR_PLLRDYC = 0x01;
    }
    else if(intSrc == RCC_INT_HSEReady)
    {
        *(volatile uint32_t*)CIR_HSERDYC = 0x01;
    }
    else if(intSrc == RCC_INT_HSIReady)
    {
        *(volatile uint32_t*)CIR_HSIRDYC = 0x01;
    }
    else if(intSrc == RCC_INT_LSEReady)
    {
        *(volatile uint32_t*)CIR_LSERDYC = 0x01;
    }
    else if(intSrc == RCC_INT_LSIReady)
    {
        *(volatile uint32_t*)CIR_LSIRDYC = 0x01;
    }
}

/**
* @brief enable or disable clock ready interrupt
* @param intSrc clock source
* @param flag enable flag
*/
void RCC_EnableClockInt(uint8_t intSrc, bool flag)
{
    if(intSrc == RCC_INT_PLLReady)
    {
        *(volatile uint32_t*)CIR_PLLRDYIE = (uint8_t)flag;
    }
    else if(intSrc == RCC_INT_HSEReady)
    {
        *(volatile uint32_t*)CIR_HSERDYIE = (uint8_t)flag;
    }
    else if(intSrc == RCC_INT_HSIReady)
    {
        *(volatile uint32_t*)CIR_HSIRDYIE = (uint8_t)flag;
    }
    else if(intSrc == RCC_INT_LSEReady)
    {
        *(volatile uint32_t*)CIR_LSERDYIE = (uint8_t)flag;
    }
    else if(intSrc == RCC_INT_LSIReady)
    {
        *(volatile uint32_t*)CIR_LSIRDYIE = (uint8_t)flag;
    }

}

/**
 * @brief get clock ready interrupt flag
 * @return flag status
 */
uint8_t RCC_GetClockIntFlag(uint8_t intSrc)
{
    uint8_t flag = 0;
    
    if(*(volatile uint32_t*)CIR_CSSF)
    {
        flag |= RCC_INT_ClockSecuty;
    }

    if(*(volatile uint32_t*)CIR_PLLRDYF)
    {
        flag |= RCC_INT_PLLReady;
    }

    if(*(volatile uint32_t*)CIR_HSERDYF)
    {
        flag |= RCC_INT_HSEReady;
    }

    if(*(volatile uint32_t*)CIR_HSIRDYF)
    {
        flag |= RCC_INT_HSIReady;
    }

    if(*(volatile uint32_t*)CIR_LSERDYF)
    {
        flag |= RCC_INT_LSEReady;
    }

    if(*(volatile uint32_t*)CIR_LSIRDYF)
    {
        flag |= RCC_INT_LSIReady;
    }
	
	return flag;
}

/**
 * @brief reset APB2 periphearl clock
 * @param reg: periphearl name
 * @param TRUE: set  FALSE: reset
 */
void RCC_APB2PeriphReset(uint32_t reg, bool flag)
{
    if(flag)
        RCC->APB2RSTR |= reg;
    else
        RCC->APB2RSTR &= ~reg;
}

/**
 * @brief reset APB1 periphearl clock
 * @param reg: periphearl name
 * @param TRUE: set  FALSE: reset
 */
void RCC_APB1PeriphReset(uint32_t reg, bool flag)
{
    if(flag)
        RCC->APB1RSTR |= reg;
    else
        RCC->APB1RSTR &= ~reg;
}


/**
 * @brief enable AHB periphearl clock
 * @param reg: periphearl name
 * @param flag: enable flag
 */
void RCC_AHBPeripClockEnable(uint32_t reg, bool flag)
{
    if(flag)
        RCC->AHBENR |= reg;
    else
        RCC->AHBENR &= ~reg;
}

/**
 * @brief enable APB2 periphearl clock
 * @param reg: periphearl name
 * @param flag: enable flag
 */
void RCC_APB2PeripClockEnable(uint16_t reg, bool flag)
{
    if(flag)
        RCC->APB2ENR |= reg;
    else
        RCC->APB2ENR &= ~reg;
}


/**
 * @brief enable APB1 periphearl clock
 * @param reg: periphearl name
 * @param flag: enable flag
 */
void RCC_APB1PeripClockEnable(uint32_t reg, bool flag)
{
    if(flag)
        RCC->APB1ENR |= reg;
    else
        RCC->APB1ENR &= ~reg; 
}


/**
 * @brief reset backup domain 
 * @param reset flag
 */
void RCC_BackUpRegisterReset(bool flag)
{
    if(flag)
        *(volatile uint32_t*)BDCR_BDRST = 0x01;
    else
        *(volatile uint32_t*)BDCR_BDRST = 0x00;  
}

/**
 * @brief check if rtc clock is enabled
 * @return enable status
 */
bool RCC_IsRTCEnabled(void)
{
    if(*(volatile uint32_t*)BDCR_RTCEN)
        return TRUE;

    return FALSE;
}

/**
 * @brief set rtc clock source
 * @param clock source
 */
void RCC_SetRTCClockSource(uint32_t source)
{
    assert_param(IS_RTC_CLOCK_PARAM(source));
    RCC->BDCR &= ~BDCR_RTCSEL;
    RCC->BDCR |= source;
}

/**
 * @brief get rtc clock source
 * @return clock source
 */
uint8_t RCC_GetRTCClockSource(void)
{
    return ((RCC->BDCR & BDCR_RTCSEL) >> 8);
}

/**
 * @brief startup rtc use lse
 * @return success flag
 */
bool RCC_StartupLSE(void)
{
    uint32_t waitCount = 0;
	volatile uint32_t i = 0;
	
    if((*(volatile uint32_t*)BDCR_LSERDY == 0x01) && 
       (*(volatile uint32_t*)BDCR_LSEON== 0x01))
    {
        return TRUE;
    }
    
    *(volatile uint32_t*)BDCR_LSEON = 0x01;
    while((!(*(volatile uint32_t*)BDCR_LSERDY)) && (waitCount < OSC_StableCycle))
    {
        for(i = 0; i < 128; i++);
        waitCount ++;
    }

    if(waitCount >= OSC_StableCycle)
    {
        return FALSE;
    }

    return TRUE;  

}

/**
 * @brief bypass lse
 * @param bypass flag
 */
bool RCC_BypassLSE(bool flag)
{
    *(volatile uint32_t*)BDCR_LSEON = 0x00;
    *(volatile uint32_t*)BDCR_LSEBYP = 0x00;
    
    if(flag)  //旁路HSE
        *(volatile uint32_t*)BDCR_LSEBYP = 0x01;

    return RCC_StartupHSE();
}


/**
 * @brief stop lse
 */
void RCC_StopLSE(void)
{
	volatile uint32_t i = 0;
    
    *(volatile uint32_t*)BDCR_LSEON = 0x00;
    for(i = 0; i < 128; i++);
}


/**
 * @brief startup rtc use lsi
 * @return success flag
 */
bool RCC_StartupLSI(void)
{
    uint32_t waitCount = 0;
	volatile uint32_t i = 0;
	
    if((*(volatile uint32_t*)CSR_LSIRDY == 0x01) && 
       (*(volatile uint32_t*)CSR_LSION== 0x01))
    {
        return TRUE;
    }
    
    *(volatile uint32_t*)CSR_LSION = 0x01;
    while((!(*(volatile uint32_t*)CSR_LSIRDY)) && (waitCount < OSC_StableCycle))
    {
        for(i = 0; i < 128; i++);
        waitCount ++;
    }

    if(waitCount >= OSC_StableCycle)
    {
        return FALSE;
    }

    return TRUE;  

}
    
  
/**
 * @brief stop lsi
 */
void RCC_CloseLSI(void)
{
	volatile uint32_t i = 0;
    
    *(volatile uint32_t*)CSR_LSION = 0x00;
    for(i = 0; i < 128; i++);
}


/**
 * @brief get reset flag
 * @return reset flag
 */
uint8_t RCC_GetResetFlag(void)
{
    return ((RCC->CSR & (uint32_t)CSR_RESET) >> 26) & 0xff;
}

/**
 * @brief clear reset flag
 */
void RCC_ClrResetFlag(void)
{
    *(volatile uint32_t*)CSR_RMVF = 0x01;
}


















































