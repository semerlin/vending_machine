/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "stm32f10x_scb.h"
#include "stm32f10x_map.h"
#include "stm32f10x_cfg.h"

/* SCB register must operate in privileged mode */


typedef struct
{
    volatile uint32_t CPUID;
    volatile uint32_t ICSR;
    volatile uint32_t VTOR;
    volatile uint32_t AIRCR;
    volatile uint32_t SCR;
    volatile uint32_t CCR;
    volatile uint32_t SHPR1;
    volatile uint32_t SHPR2;
    volatile uint32_t SHPR3;
    volatile uint32_t SHCSR;
    volatile uint32_t CFSR;
    volatile uint32_t HFSR;
    uint32_t RESERVED;
    volatile uint32_t MMFAR;
    volatile uint32_t BFAR;
}SCB_T;

/* SCB寄存器结构体定义*/
SCB_T *SCB = (SCB_T *)SCB_BASE;

//寄存器内部位置定义
#define NMIPENDSET    ((uint32_t)1 << 31)
#define PENDSVSET     ((uint32_t)1 << 28)
#define PENDSVCLR     ((uint32_t)1 << 27)
#define PENDSTSET     ((uint32_t)1 << 26)
#define PENDSTCLR     ((uint32_t)1 << 25)
#define ISRPENDING    ((uint32_t)1 << 22)
#define VECTPENDING   ((uint32_t)0x3ff << 12)
#define RETOBASE      ((uint32_t)1 << 11)
#define VECTACTIVE    ((uint32_t)0xff << 0)

#define TBLOFF        ((uint32_t)0x7ffff << 11)
#define TBLBASE       ((uint32_t)1 << 29)

#define VECTKEY       ((uint32_t)0x5FA << 16)

#define PRIGROUP      ((uint32_t)0x07 << 8)
#define SYSRESETREQ   ((uint32_t)0x01 << 2)

#define SEVEONPEND    ((uint32_t)0x01 << 4)
#define SLEEPDEEP     ((uint32_t)0x01 << 2)
#define SLEEPONEXIT   ((uint32_t)0x01 << 1)

//CCR寄存器
#define STKALIGN      ((uint32_t)0x01 << 9)
#define BFHFNMIGN     ((uint32_t)0x01 << 8)
#define DIV0TRP       ((uint32_t)0x01 << 4)
#define UNALIGNTRP    ((uint32_t)0x01 << 3)
#define USERSETMPEND  ((uint32_t)0x01 << 1)
#define NONBASETHRDENA ((uint32_t)0x01)

//SHPRx寄存器
#define MEMMANGEFAULT   ((uint32_t)0x0f << 4)  
#define BUSFAULT        ((uint32_t)0x0f << 12)
#define USAGEFAULT      ((uint32_t)0x0f << 20)
#define SVCALL          ((uint32_t)0x0f << 28)
#define PENDSV          ((uint32_t)0x0f << 20)
#define SYSTICK         ((uint32_t)0x0f << 28)


//SHCSR寄存器
#define USGFAULTENA    ((uint32_t)0x01 << 18)
#define BUSFAULTENA    ((uint32_t)0x01 << 17)
#define MEMFAULTENA    ((uint32_t)0x01 << 16)

#define SVCALLPENDED   ((uint32_t)0x01 << 15)
#define BUSFAULTPENDED ((uint32_t)0x01 << 14)
#define MEMFAULTPENDED ((uint32_t)0x01 << 13)
#define USGFAULTPENDED ((uint32_t)0x01 << 12)

#define SYSTICKACT     ((uint32_t)0x01 << 11)
#define PENDSVACT      ((uint32_t)0x01 << 10)
#define MONITORACT     ((uint32_t)0x01 << 8)
#define SVCALLACT      ((uint32_t)0x01 << 7)
#define USGFAULTACT    ((uint32_t)0x01 << 3)
#define BUSFAULTACT    ((uint32_t)0x01 << 1)
#define MEMFAULTACT    ((uint32_t)0x01)


/**
 * @brief get cpu id
 * @return cpu id
 */
uint32_t SCB_GetCPUID(void)
{
    return (SCB->CPUID);   
}

/**
 * @brief change NMI excpetion state to pending
 */
void SCB_PendNMI(void)
{
    SCB->ICSR |= NMIPENDSET;
}

/**
 * @brief check if NMI is pending
 * @retunr TREU:yes FALSE:no
 */
bool SCB_IsNMIPending(void)
{
    if(((SCB->ICSR) & NMIPENDSET) == NMIPENDSET)
        return TRUE;
    else
        return FALSE;
}

/**
 * @brief change pendsv exception state
 * @param flag TRUE:pending FALSE: not pending
 */
void SCB_PendPendSV(bool flag)
{
    if(flag)
        SCB->ICSR |= PENDSVSET;
    else
        SCB->ICSR |= PENDSVCLR;
}


/**
 * @brief check if PendSV is pending
 * @retunr TREU:yes FALSE:no
 */
bool SCB_IsPendSVPending(void)
{
    if(((SCB->ICSR) & PENDSVSET) == PENDSVSET)
        return TRUE;
    else
        return FALSE;
}

/**
 * @brief change SysTick exception state
 * @param flag TRUE:pending FALSE: not pending
 */
void SCB_PendSysTick(bool flag)
{
    if(flag)
        SCB->ICSR |= PENDSTSET;
    else
        SCB->ICSR |= PENDSTCLR;
}

/**
 * @brief check if any interrupt is pending, excluding NMI and Faults
 * @return TRUE:yes FALSE:no
 */
bool SCB_IsIntPending(void)
{
    if(((SCB->ICSR) & ISRPENDING) == ISRPENDING)
        return TRUE;
    else
        return FALSE;
}

/**
 * @brief get pending interrupt vector
 * @return interrupt vector
 */
uint32_t SCB_GetPendIntVector(void)
{
    return (((SCB->ICSR) & VECTPENDING) >> 12);
}


/**
 * @brief indicates whether there are preempted active exceptions
 * @return TRUE:yes FALSE:no
 */
bool SCB_IsIntPreempted(void)
{
    if(((SCB->ICSR) & RETOBASE) == RETOBASE)
        return FALSE;
    else
        return TRUE;
}

/**
 * @brief get active exception vector
 * @retunr active exception vector
 */
uint32_t SCB_GetActiveIntVector(void)
{
    return ((SCB->ICSR) & VECTACTIVE);
}

/**
* @brief set vector table address
* @param vector table address description
*/
void SCB_SetVectTableConfig(VectTable table)
{
    if(table.place == CODE)
        SCB->VTOR &= ~TBLBASE;
    else
        SCB->VTOR |= TBLBASE;
    
    SCB->VTOR = (((table.offsetAddr) & 0x7ffff) << 9); 
}

/**
* @brief set vector table address
* @return vector table address description
*/
VectTable SCB_GetVectTableConfig(void)
{
    VectTable table;
    table.offsetAddr = (((SCB->VTOR) & TBLOFF) >> 9);
    if(((SCB->VTOR) & TBLBASE) == TBLBASE)
        table.place = SRAM;
    else
        table.place = CODE;
    
    return table;
}

/**
 * @brief set priority group
 * @param priority group
 */
void SCB_SetPriorityGrouping(uint32_t group)
{
    assert_param(group <= 7);
	SCB->AIRCR = (VECTKEY | (group << 8));
}

/**
 * @brief get minimum preemption priority
 * @return minimum preemption priority
 */
uint8_t SCB_GetMinPreemptionPriority(void)
{
    uint8_t priorityGrouping = ((SCB->AIRCR >> 8) & 0x07);
    return (1 << (7 - priorityGrouping)) - 1;   
}

/**
 * @brief get minimum sub priority
 * @return minimum sub priority
 */
uint8_t SCB_GetMinSubPriority(void)
{
    uint8_t priorityGrouping = ((SCB->AIRCR >> 8) & 0x07);
    return (1 << (priorityGrouping - 3)) - 1;
}

/**
 * @brief get priority grouping
 * @return priority grouping
 */
uint8_t SCB_GetPriorityGrouping(void)
{
    return ((SCB->AIRCR >> 8) & 0x07);
}
/**
 * @brief generate system reset
 */
void SCB_GenSystemReset(void)
{
	SCB->AIRCR |= (VECTKEY | SYSRESETREQ);
}



/**
 * @brief config only enabled interrupts or all interrupts can wakeup processor
 * @param TRUE: all FALSE: only enabled
 */
void SCB_EnableAllIntWakeup(bool flag)
{
	SCB->SCR &= ~SEVEONPEND;
    if(flag)
        SCB->SCR |= (1 << 4);
}

/**
 * @brief controls whether the processor uses sleep or deep sleep as its low
 *        power mode
 * @param sleep mode
 */                  
void SCB_SetSleepMode(uint8_t mode)
{
    assert_param(IS_SCB_SLEEP_PARAM(mode));
    SCB->SCR &= ~SLEEPDEEP;
    SCB->SCR |= mode;
}

/**
 * @brief configures sleep-on-exit when returning from handler mode to thread 
 *        mode
 * @param TRUE: Enter sleep or deep sleep FALSE: Do not sleep
 */  
void SCB_EnableSleepOnExit(bool flag)
{
    SCB->SCR &= ~SLEEPONEXIT;

    if(flag)
        SCB->SCR |= SLEEPONEXIT;
}


/**
* @brief set stack align mode
* @param align mode
*/                   
void SCB_SetStackAlign(uint16_t align)
{
    assert_param(IS_SCB_STACK_PARAM(align));
    SCB->CCR &= ~STKALIGN;
    SCB->CCR |= align;
}


/**
 * @brief whether process bus faults or not
 * @param TRUE: yes FALSE:no
 */                   
void SCB_BusFaultIgnore(bool flag)
{
    SCB->CCR &= ~BFHFNMIGN;
    if(flag)
        SCB->CCR |= BFHFNMIGN;
}



/**
 * @brief configure trap divide by 0 
 * @param TRUE: yes FALSE:no
 */                   
void SCB_EnableDiv0Trp(bool flag)
{
    SCB->CCR &= ~DIV0TRP;
    if(flag)
        SCB->CCR |= DIV0TRP;
}


/**
 * @brief enables unaligned access traps 
 * @param TRUE: yes FALSE:no
 */ 
void SCB_EnableUnalignTrp(bool flag)
{
    SCB->CCR &= ~UNALIGNTRP;
    if(flag)
        SCB->CCR |= UNALIGNTRP;
}

/**
 * @brief enables unprivileged software access to the STIR 
 * @param TRUE: yes FALSE:no
 */                   
void SCB_EnableUserAccessSTIR(bool flag)
{
    SCB->CCR &= ~USERSETMPEND;
    if(flag)
        SCB->CCR |= USERSETMPEND;
}

/**
 * @brief set thread mode enter method
 * @param enter method
 */
void SCB_SetThreadModeEnterMethod(uint8_t mode)
{
    assert_param(IS_SCB_THREADMODE_PARAM(mode));
    SCB->CCR &= ~NONBASETHRDENA;
    SCB->CCR |= mode;
}


/**
 * @brief set exception handlers priority
 * @param exception name
 * @param exception priority, 0-15, 0 has the highest priority, 15 has the 
         lowest                   
 */                   
void SCB_SetExceptionPriorty(uint8_t exception, uint32_t priority)
{
    assert_param(IS_SCB_EXCEPTION_PARAM(exception));
    assert_param(priority <= 15);
   
    switch(exception)
    {
    case SCB_Exception_MemMangeFault:
        SCB->SHPR1 &= ~MEMMANGEFAULT;
        SCB->SHPR1 |= (priority << 4);
        break;
    case SCB_Exception_BusFault:
        SCB->SHPR1 &= ~BUSFAULT;
        SCB->SHPR1 |= (priority << 12);
        break;
    case SCB_Exception_UsageFault:
        SCB->SHPR1 &= ~USAGEFAULT;
        SCB->SHPR1 |= (priority << 20);
        break;
    case SCB_Exception_SVCall:
        SCB->SHPR2 &= ~SVCALL;
        SCB->SHPR2 |= (priority << 28);
        break;
    case SCB_Exception_PendSV:
        SCB->SHPR3 &= ~PENDSV;
        SCB->SHPR3 |= (priority << 20);
        break;
    case SCB_Exception_SysTick:
        SCB->SHPR3 &= ~SYSTICK;
        SCB->SHPR3 |= (priority << 28);
        break;
    default:
        break;
    }
}


/**
 * @brief enable exception handler
 * @param exception handler
 * @param TRUE: enable FALSE: disable
 */
void SCB_EnableException(uint8_t handle, bool flag)
{
    assert_param(IS_SCB_EXCEPTION_PARAM(handle));
    switch(handle)
    {
    case SCB_Exception_MemMangeFault:
        if(flag)
            SCB->SHCSR |= MEMFAULTENA;
        else
            SCB->SHCSR &= ~MEMFAULTENA;
        break;
    case SCB_Exception_BusFault:
        if(flag)
            SCB->SHCSR |= BUSFAULTENA;
        else
            SCB->SHCSR &= ~BUSFAULTENA;
        break;
    case SCB_Exception_UsageFault:
        if(flag)
            SCB->SHCSR |= USGFAULTENA;
        else
            SCB->SHCSR &= ~USGFAULTENA;
        break;
    default:
         break;
    }
}


/**
 * @brief get pending exception handlers
 * @return pending exception handlers
 */
uint8_t SCB_GetPendingException(void)
{
    uint8_t pendException = (SCB->SHCSR >> 12) & 0x0f;
    uint8_t ret = 0;
    
    if(pendException & 0x01)
        ret |= SCB_Exception_UsageFault;
    
    if(pendException & 0x02)
        ret |= SCB_Exception_MemMangeFault;
    
    if(pendException & 0x04)
        ret |= SCB_Exception_BusFault;
    
    if(pendException & 0x08)
        ret |= SCB_Exception_SVCall;
    
    return ret;
}


/**
 * @brief set exception pending status
 * @param TRUE:set FALSE:reset
 */
void SCB_SetPendingStatus(uint8_t exception, bool flag)
{   
    assert_param(IS_SCB_EXCEPTION_PARAM(exception));
   
    switch(exception)
    {
    case SCB_Exception_MemMangeFault:
        if(flag)
            SCB->SHCSR |= MEMFAULTPENDED;
        else
            SCB->SHCSR &= ~MEMFAULTPENDED;
        break;
    case SCB_Exception_BusFault:
        if(flag)
            SCB->SHCSR |= BUSFAULTPENDED;
        else
            SCB->SHCSR &= ~BUSFAULTPENDED;
        break;
    case SCB_Exception_UsageFault:
        if(flag)
            SCB->SHCSR |= USGFAULTPENDED;
        else
            SCB->SHCSR &= ~USGFAULTPENDED;
        break;
    case SCB_Exception_SVCall:
        if(flag)
            SCB->SHCSR |= SVCALLPENDED;
        else
            SCB->SHCSR &= ~SVCALLPENDED;
        break;
    default:
        break;
    }
}

/**
 * @param get active exception handlers
 * @return active exception handlers
 */
uint8_t SCB_GetActiveException(void)
{
    uint8_t dwActiveStatus = 0;
    if(SCB->SHCSR & MEMFAULTACT)
    {
        dwActiveStatus |= SCB_Exception_MemMangeFault;
    }

    if(SCB->SHCSR & BUSFAULTACT)
    {
        dwActiveStatus |= SCB_Exception_BusFault;
    }
    
    if(SCB->SHCSR & USGFAULTACT)
    {
        dwActiveStatus |= SCB_Exception_UsageFault;
    }

    if(SCB->SHCSR & SVCALLACT)
    {
        dwActiveStatus |= SCB_Exception_SVCall;
    }

    if(SCB->SHCSR & MONITORACT)
    {
        dwActiveStatus |= SCB_Debug_Monitor;
    }

    if(SCB->SHCSR & PENDSVACT)
    {
        dwActiveStatus |= SCB_Exception_PendSV;
    }

    if(SCB->SHCSR & SYSTICKACT)
    {
        dwActiveStatus |= SCB_Exception_SysTick;
    }

    return dwActiveStatus;
}

/**
 * @brief set exception active status
 * @param exception handler
 * @param TRUE:set FALSE:reset
 */
void SCB_SetActiveStatus(uint8_t exception, bool flag)
{   
    
    assert_param(IS_SCB_ACTIVE_PARAM(exception));
   
    switch(exception)
    {
    case SCB_Exception_MemMangeFault:
        if(flag)
            SCB->SHCSR |= MEMFAULTACT;
        else
            SCB->SHCSR &= ~MEMFAULTACT;
        break;
    case SCB_Exception_BusFault:
        if(flag)
            SCB->SHCSR |= BUSFAULTACT;
        else
            SCB->SHCSR &= ~BUSFAULTACT;
        break;
    case SCB_Exception_UsageFault:
        if(flag)
            SCB->SHCSR |= USGFAULTACT;
        else
            SCB->SHCSR &= ~USGFAULTACT;
        break;
    case SCB_Exception_SVCall:
        if(flag)
            SCB->SHCSR |= SVCALLACT;
        else
            SCB->SHCSR &= ~SVCALLACT;
        break;
    case SCB_Debug_Monitor:
        if(flag)
            SCB->SHCSR |= MONITORACT;
        else
            SCB->SHCSR &= ~MONITORACT;
        break;
    case SCB_Exception_PendSV:
        if(flag)
            SCB->SHCSR |= PENDSVACT;
        else
            SCB->SHCSR &= ~PENDSVACT;
        break;
    case SCB_Exception_SysTick:
        if(flag)
            SCB->SHCSR |= SYSTICKACT;
        else
            SCB->SHCSR &= ~SYSTICKACT;
        break;
    default:
        break;
    }
}

/**
 * @brief get usage fault reason
 * @return reason
 */
uint32_t SCB_GetUsageFaultDetail(void)
{
    return (SCB->CFSR & 0xffff0000);
}

/**
 * @brief clear usage fault
 * @param position to clear
 */
void SCB_ClrUsageFaultStatus(uint32_t reg)
{
    assert_param(IS_SCB_USAGE_FAULT_PARAM(reg));
    SCB->CFSR |= reg;
}

/**
 * @brief get bus fault reason
 * @return reason
 */
uint32_t SCB_GetBusFaultDetail(void)
{
    return (SCB->CFSR & 0x0000ff00);
}

/**
 * @brief clear bus fault
 * @param position to clear
 */
void SCB_ClrBusFaultStatus(uint32_t status)
{
    assert_param(IS_SCB_BUS_FAULT_PARAM(status));
    SCB->CFSR |= status;
}

/**
 * @brief get bus fault reason
 * @return reason
 */
uint32_t SCB_GetMemFaultDetail(void)
{
  return (SCB->CFSR & 0x000000ff);
}


/**
 * @brief clear bus fault
 * @param position to clear
 */
void SCB_ClrMemFaultStatus(uint32_t reg)
{
    assert_param(IS_SCB_MEM_FAULT_PARAM(reg));
    SCB->CFSR |= reg;
}


/**
 * @brief get hard fault reason
 * @return reason
 */
uint32_t SCB_GetHardFaultDetail(void)
{
  return SCB->HFSR;
}


/**
 * @brief clear hard fault
 * @param position to clear
 */
void SCB_ClrHardFaultStatus(uint32_t reg)
{
    assert_param(IS_SCB_HARD_FAULT_PARAM(reg));
    SCB->HFSR |= reg;
}

/**
 * @brief get memory management fault address
 * @return address
 */
uint32_t SCB_GetMemFaultAddress(void)
{
    return SCB->MMFAR;
}

/**
 * @brief get memory management fault address
 * @return address
 */
uint32_t SCB_GetBusFaultAddress(void)
{
    return SCB->BFAR;
}

/**
 * @brief reset system
 */
void SCB_SystemReset(void) 
{ 
  __ASM("MOV R0, #1"); 
  __ASM("MSR FAULTMASK, R0"); 
  SCB->AIRCR = 0x05FA0004; 
  for(;;); 
} 






