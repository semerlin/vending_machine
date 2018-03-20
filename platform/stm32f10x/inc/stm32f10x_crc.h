#ifndef _STM32F10X_CRC_H_
  #define _STM32F10X_CRC_H_

#include "types.h"

/* interface */
void CRC_ResetDR(void);
uint32 CRC_Cal(uint32_t data);
uint32 CRC_CalBlock(uint32_t *buf, uint32_t len);
uint32 CRC_GetDR(void);
void CRC_SetIDR(uint8_t data);
uint8 CRC_GetIDR(void);

#endif

