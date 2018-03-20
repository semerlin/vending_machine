#ifndef _STM32F10X_IWDG_H_
  #define _STM32F10X_IWDG_H_

#include "types.h"


/* iwdg definition */
#define IWDG_PR_DIV4        (0x00)
#define IWDG_PR_DIV8        (0x01)
#define IWDG_PR_DIV16       (0x02)
#define IWDG_PR_DIV32       (0x03)
#define IWDG_PR_DIV64       (0x04)
#define IWDG_PR_DIV128      (0x05)
#define IWDG_PR_DIV256      (0x06)

#define IS_IWDG_PR_PARAM(param)  ((param == IWDG_PR_DIV4) || \
                                  (param == IWDG_PR_DIV8) || \
                                  (param == IWDG_PR_DIV16) || \
                                  (param == IWDG_PR_DIV32) || \
                                  (param == IWDG_PR_DIV64) || \
                                  (param == IWDG_PR_DIV128) || \
                                  (param == IWDG_PR_DIV256))






/* interface */
void IWDG_Startup(void);
void IWDG_Feed(void);
void IWDG_SetClockPrescaler(uint8_t div);
uint8_t IWDG_GetClockPrescaler(void);
void IWDG_SetReloadValue(uint8_t value);
uint16 IWDG_GetReloadValue(void);











































#endif


