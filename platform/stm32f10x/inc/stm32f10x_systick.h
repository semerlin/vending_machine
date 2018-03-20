#ifndef _STM32F10X_SYSTICK_H_
  #define _STM32F10X_SYSTICK_H_

#include "types.h"

/* systick clock resource */
#define SYSTICK_CLOCK_AHB_DIV_EIGHT    (0x00)
#define SYSTICK_CLOCK_AHB              (1 << 2)
#define IS_SYSTICK_CLOCK_PARAM(CLOCK) \
                       ((CLOCK == SYSTICK_CLOCK_AHB_DIV_EIGHT) || \
                        (CLOCK == SYSTICK_CLOCK_AHB))





/* interface */
void SYSTICK_SetClockSource(uint8 source);
void SYSTICK_EnableInt(BOOL flag);
void SYSTICK_EnableCounter(BOOL flag);
BOOL SYSTICK_IsCountFlagSet(void);
void SYSTICK_ClrCountFlag(void);
void SYSTICK_SetTickInterval(uint32_t time);






#endif
