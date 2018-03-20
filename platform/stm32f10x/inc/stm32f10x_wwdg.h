#ifndef _STM32F10X_WWDG_H_
  #define _STM32F10X_WWDG_H_

#include "types.h"


/* wwdg timer base definition */
#define WWDG_TIMEBASE_DIV1       (0x00)
#define WWDG_TIMEBASE_DIV2       (0x01 << 7)   
#define WWDG_TIMEBASE_DIV4       (0x02 << 7)  
#define WWDG_TIMEBASE_DIV8       (0x03 << 7)   


#define IS_WWDG_TIMEBASE_PARAM(param)  ((param == WWDG_TIMEBASE_DIV1) || \
			                            (param == WWDG_TIMEBASE_DIV2) || \
                                        (param == WWDG_TIMEBASE_DIV4) || \
			                            (param == WWDG_TIMEBASE_DIV8))




/* interface */
void WWDG_Startup(void);
void WWDG_Feed(void);
void WWDG_SetCounter(uint8_t cnt);
void WWDG_SetTimerBase(uint16_t base);
void WWDG_SetWindowCounter(uint8_t cnt);
void WWDG_EnableEWI(void);
void WWDG_ClrEWIFlag(void);
BOOL WWDG_IsEWIFFlagSet(void);






#endif



