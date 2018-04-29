/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include <string.h>
#include "stm32f10x_flash.h"
#include "stm32f10x_map.h"
#include "stm32f10x_cfg.h"

/* flash register structure */
typedef struct 
{
	volatile uint32_t ACR;
	volatile uint32_t KEYR;
	volatile uint32_t OPTKEYR;
	volatile uint32_t SR;
	volatile uint32_t CR;
	volatile uint32_t AR;
	volatile uint32_t OBR;
	volatile uint32_t WRPR;
}FLASH_T;

FLASH_T *FLASH = (FLASH_T *)FLASH_BASE;


/* flash register's bit band area */

#define FLASH_OFFSET (FLASH_BASE - PERIPH_BASE)
/*  ACR bit band */
#define ACR_OFFSET (FLASH_OFFSET + 0x00)
#define ACR_PRFTBS (PERIPH_BB_BASE + ACR_OFFSET * 32 + 0x05 * 4)
#define ACR_PRFTBE (PERIPH_BB_BASE + ACR_OFFSET * 32 + 0x04 * 4)
#define ACR_HLFCYA (PERIPH_BB_BASE + ACR_OFFSET * 32 + 0x03 * 4)
/* CR bit band */
#define CR_OFFSET (FLASH_OFFSET + 0x10)
#define CR_PG     (PERIPH_BB_BASE + CR_OFFSET * 32 + 0x00 * 4)
#define CR_PER    (PERIPH_BB_BASE + CR_OFFSET * 32 + 0x01 * 4)
#define CR_STRT   (PERIPH_BB_BASE + CR_OFFSET * 32 + 0x06 * 4)
#define CR_LOCK   (PERIPH_BB_BASE + CR_OFFSET * 32 + 0x07 * 4)


/* key values */
#define RDPRT       0x00A5
#define KEY1        0x45670123
#define KEY2        0xCDEF89AB

/******************************************************/
//ACR latency 
#define ACR_LATENCY   0x03



/**
 * @brief unlock flash
 */
static void FLASH_Unlock(void)
{
    FLASH->KEYR = KEY1;
    FLASH->KEYR = KEY2;
}

/**
 * @brief lock flash
 */
static void FLASH_Lock(void)
{
    *((volatile uint32_t*)CR_LOCK) = 0x01;
}

/**
 * @brief enable flash prefetch
 * @param flag: TRUE: enable, FALSE: disable
 */
void FLASH_EnablePrefetch(bool flag)
{
    if(flag)
        *((volatile uint32_t *)ACR_PRFTBE) = 0x01;
    else
        *((volatile uint32_t *)ACR_PRFTBE) = 0x00;
}


/**
 * @brief get flash prefetch status
 * @return flash prefetch is enabled if true, otherwise false
 */
bool FLASH_IsPrefetchEnabled(void)
{
    if(*((volatile uint32_t*)ACR_PRFTBS))
    {
        return TRUE;
    }

    return FALSE;
}


/**
 * @brief enable flash half cycle access, only use this when sysclk is lower
 *        than 8MHz
 * @param flag: TRUE: enable, FALSE: disable
 */
void FLASH_EnableHalfCycleAccess(bool flag)
{
    if(flag)
        *((volatile uint32_t*)ACR_HLFCYA) = 0x01;
    else
        *((volatile uint32_t*)ACR_HLFCYA) = 0x00;
}

/**
 * @brief get flash half cycle access status
 * @return flash half cycle access is enabled if true, otherwise false
 */
bool FLASH_IsHalfCycleAccessEnabled(void)
{
    if(*((volatile uint32_t*)ACR_HLFCYA))
    {
        return TRUE;
    }

    return FALSE;
}


/**
 * @brief set flash read latency
 * @param latency: latency value
 */
void FLASH_SetLatency(uint8_t latency)
{
    assert_param(IS_FLASH_LATENCY_PARAM(latency));

    FLASH->ACR &= ~ACR_LATENCY;

    FLASH->ACR |= latency;
}

/**
 * @brief get flag status
 * @pram - flag
 * @return flag status
 */
bool FLASH_Is_FlagSet(uint8_t flag)
{
    assert_param(IS_FLASH_FLAG_PARAM(flag)); 
    return (0 != (FLASH->SR & flag));
}

/**
 * @brief clear flash flag
 * @param flag - flash flag
 */
void FLASH_ClrFlag(uint8_t flag)
{
    FLASH->SR &= ~(flag);
}

/**
 * @brief erase flash page
 * @param addr - erase address
 */
void FLASH_ErasePage(uint32_t addr)
{
    FLASH_Unlock();
    FLASH_ClrFlag(FLAH_FLAG_EOP | FLAH_FLAG_WRPRTERR | FLAH_FLAG_PGERR);
    
    *((volatile uint32_t*)CR_PER) = 0x01;
    FLASH->AR = addr;
    *((volatile uint32_t*)CR_STRT) = 0x01;
    while (FLASH->SR & FLAH_FLAG_BSY); 
    *((volatile uint32_t*)CR_PER) = 0x00;
    
    FLASH_Lock();
}
/**
 * @brief write data to flash
 * @param addr - start address
 * @param data - data to write
 * @param len - data length
 */
uint32_t FLASH_Write(uint32_t addr, uint8_t *data, uint32_t len)
{
    FLASH_Unlock();
    FLASH_ClrFlag(FLAH_FLAG_EOP | FLAH_FLAG_WRPRTERR | FLAH_FLAG_PGERR);
    
    *((volatile uint32_t*)CR_PG) = 0x01;
    
    for (uint32_t i = 0; i < len; i += 2)
    {
        *(uint16_t*)addr = *(uint16_t *)data;
        addr += 2;
        data += 2;
        while (FLASH->SR & FLAH_FLAG_BSY);
    }

    *((volatile uint32_t*)CR_PG) = 0x00;
    
    FLASH_Lock();
    
    return len;
}

/**
 * @brief write data to flash
 * @param addr - start address
 * @param data - data to read out
 * @param len - data length
 */
uint32_t FLASH_Read(uint32_t addr, uint8_t *data, uint32_t len)
{
    memcpy(data, (void *)addr, len);
    return len;
}
