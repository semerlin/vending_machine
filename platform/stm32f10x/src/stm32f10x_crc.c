/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "stm32f10x_crc.h"
#include "stm32f10x_map.h"
#include "stm32f10x_cfg.h"


/* crc register structure */
typedef struct 
{
	volatile uint32_t DR;
	volatile uint32_t IDR;
	volatile uint32_t CR;
}CRC_T;

/* init crc struct base address */
CRC_T *CRC = (CRC_T *)CRC_BASE;


/* flash register's bit band area */
#define CRC_OFFSET (CRC_BASE - PERIPH_BASE)
/*  CR bit band */
#define CR_OFFSET (CRC_OFFSET + 0x08)
#define CR_RESET (PERIPH_BB_BASE + CR_OFFSET * 32 + 0x00 * 4)


/** 
 * @brief   reset DR value to 0xffffffff
 */
void CRC_ResetDR(void)
{
    *((volatile uint32_t *)CR_RESET) = 0x01;
}


/**
 * @brief calculate one data crc value
 * *return 32-bit crc value
 */
uint32_t CRC_Cal(uint32_t data)
{
	CRC->DR = data;

	return CRC->DR;
}

/**
 * @brief calculate block data crc value
 * @param [in] inbuf: buffer hold data
 * @param [in] len: buffer length
 * @return 32-bit crc value
 */
uint32_t CRC_CalBlock(uint32_t *buf, uint32_t len)
{
    assert_param(buf != NULL);

	while(len--)
	{
		CRC->DR = *buf++;	
	}

	return CRC->DR;
}


/**
 * @brief get previous crc value
 * @return previour crc value
 */
uint32_t CRC_GetDR(void)
{
	return CRC->DR;
}


/**
 * @brief temporary store one byte
 * @param data: data need to hold
 */
void CRC_SetIDR(uint8_t data)
{
	CRC->IDR = data;
}

/**
 * @brief get temporary store data
 * @return data value
 */
uint8_t CRC_GetIDR(void)
{
	return CRC->IDR;
}




