/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "stm32f10x_exti.h"
#include "stm32f10x_map.h"
#include "stm32f10x_cfg.h"

#if 0
/* wwdg structure */
typedef struct 
{
    volatile uint16_t CR;
	uint16_t RESERVE0;
    volatile uint16_t CFR;
	uint16_t RESERVE1;
	volatile uint16_t SR;
	uint16_t RESERVE2;
}WWDG_T;

WWDG_T *WWDG = (WWDG_T *)WWDG_BASE;


/* wwdg bit band */
#define WWDG_OFFSET (WWDG_BASE - PERIPH_BASE)
/* CR register bit band */
#define CR_OFFSET (WWDG_OFFSET + 0x00)
#define CR_WDGA (PERIPH_BB_BASE + CR_OFFSET * 32 + 0x07 * 4)

/* CFR register bit band */
#define CFR_OFFSET (WWDG_OFFSET + 0x04)
#define CFR_EWI (PERIPH_BB_BASE + CFR_OFFSET * 32 + 0x09 * 4)

/* SR register bit band */
#define SR_OFFSET (WWDG_OFFSET + 0x08)
#define SR_EWIF (PERIPH_BB_BASE + SR_OFFSET * 32 + 0x01 * 4)

/* register operation bits definition */
#define CFR_WDGTB   (0x03 << 7)
#endif
