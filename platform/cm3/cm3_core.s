;**
; This file is part of the vendoring machine project.
;
; Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
;
; See the COPYING file for the terms of usage and distribution.
;/
  SECTION .text:CODE(2)

  ;Exported functions
  EXPORT __NOP
  EXPORT __WFI
  EXPORT __WFE
  EXPORT __SEV
  EXPORT __ISB
  EXPORT __DSB
  EXPORT __DMB
  EXPORT __SVC
  EXPORT __get_PSR
  EXPORT __get_IPSR
  EXPORT __get_CONTROL
  EXPORT __set_CONTROL
  EXPORT __get_PSP
  EXPORT __set_PSP
  EXPORT __get_MSP
  EXPORT __set_MSP
  EXPORT __set_PRIMASK
  EXPORT __get_PRIMASK
  EXPORT __reset_PRIMASK
  EXPORT __set_FAULTMASK
  EXPORT __get_FAULTMASK
  EXPORT __reset_FAULTMASK
  EXPORT __set_BASEPRI
  EXPORT __get_BASEPRI
  EXPORT __REV
  EXPORT __REV16
  EXPORT __REVSH
  EXPORT __RBIT
  EXPORT __CLZ


;*******************************************************************************
; @brief nop instruction
;*******************************************************************************
__NOP
    nop
    bx lr
    
;*******************************************************************************
; @brief wait for interrupt instruction, use this to enter low power mode
;*******************************************************************************
__WFI
    wfi
    bx lr

;*******************************************************************************
; @brief wait for event instruction, use this to enter low power mode
;*******************************************************************************
__WFE
    wfe
    bx lr

;*******************************************************************************
; @brief set event instruction, can wakeup processor from WFE
;*******************************************************************************
__SEV
    sev
    bx lr

;*******************************************************************************
; @brief instruction synchronization
;*******************************************************************************
__ISB
    isb
    bx r14

;*******************************************************************************
; @brief data synchronization
;*******************************************************************************
__DSB
    dsb
    bx r14

;*******************************************************************************
; @brief data memory
;*******************************************************************************
__DMB
    dmb
    bx r14

;*******************************************************************************
; @brief svc instruction, user mode can call this function to enter privilege
;        mode
;*******************************************************************************
__SVC
    svc #0x01 
    bx lr

;*******************************************************************************
; @brief get psr register value
;*******************************************************************************
__get_PSR
    mrs r0, psr
    bx lr

;*******************************************************************************
; @brief get ipsr value, ipsr is current exception number
;*******************************************************************************
__get_IPSR
    mrs r0, ipsr
    bx lr
;*******************************************************************************
; @brief get contorl register value
; @note CONTROL[1] stack pointer choose
;                  0: choose MSP(reset default value)
;                  1: choose PSP
;       in thread mode both MSP or PSP can be used, but in exception mode, only 
;       MSP can be used
;       CONTROL[0] 
;                  0: thread mode is privilege mode
;                  1: thread mode is user mode
; @return control register value
;*******************************************************************************
__get_CONTROL
    mrs r0, control
    bx lr

;*******************************************************************************
; @brief set contorl register value
;*******************************************************************************
__set_CONTROL
  msr control, r0
  isb
  bx lr

;*******************************************************************************
; @brief get process stack value
;*******************************************************************************
__get_PSP
    mrs r0, psp
    bx lr

;*******************************************************************************
; @brief set process stack value
;*******************************************************************************
__set_PSP
    msr psp, r0
    bx lr

;*******************************************************************************
; @brief get master stack value
;*******************************************************************************
__get_MSP
  mrs r0, msp
  bx lr

;*******************************************************************************
; @brief set master stack value
;*******************************************************************************
__set_MSP
    msr msp, r0
    bx lr

;*******************************************************************************
; @brief turn off all maskable exception except NMI and hard fault
;*******************************************************************************
__set_PRIMASK
    cpsid i
    bx lr

;*******************************************************************************
; @brief get primask value
;*******************************************************************************
__get_PRIMASK
    mrs r0, primask
    bx lr
    
;*******************************************************************************
; @brief turn on all maskable exceptions
;*******************************************************************************
__reset_PRIMASK
    cpsie i
    bx lr

;*******************************************************************************
; @brief turn off all maskable exception include hard fault, except NMI
;*******************************************************************************
__set_FAULTMASK
    cpsid f
    bx lr

;*******************************************************************************
; @brief get faultmask value
;*******************************************************************************
__get_FAULTMASK
    mrs r0, faultmask
    bx lr
    
;*******************************************************************************
; @brief turn on all maskable exceptions
;*******************************************************************************
__reset_FAULTMASK
    cpsie f
    bx lr

;*******************************************************************************
; @brief basepri defined maskable exception threshold value. when it has been
;        setted to some value than exceptions that irq num bigger than that 
;        value will be turn off.it turn off no exception when is's value is 0
;*******************************************************************************
__set_BASEPRI
    msr basepri, r0
    bx lr

;*******************************************************************************
; @brief get basepri value
; @return basepri value
;*******************************************************************************
__get_BASEPRI
    mrs r0, basepri
    bx lr

;*******************************************************************************
; @brief reverse byte order in a word
; @note for example, R0=0x12345678
;       REV R1 R0
;       R1=0x78563412
;*******************************************************************************
__REV
    rev r0, r0
    bx lr

;*******************************************************************************
; @brief reverse byte order in each halfword independently
; @note for example, R0=0x12345678
;       REV16 R1 R0
;       R1=0x34127856
;*******************************************************************************
__REV16
    rev16 r0, r0
    bx lr
 
;*******************************************************************************
; @brief reverse byte order in the bottom halfword, and sign extend to 32 bits
; @note for example, R0=0x12345678
;       REVSH R1 R0
;       R1=0xFFFF7856
;******************************************************************************* 
__REVSH
    revsh r0, r0
    bx lr

;*******************************************************************************
; @brief reverse the bit order in a 32-bit word
; @note for example, R0=0xB4E10C23(1011,0100,1110,0001,0000,1100,0010,0011)
;       RBIT R1 R0
;       R1=0xC430872D(1100,0100,0011,0000,1000,0111,0010,1101)
;*******************************************************************************
__RBIT
    rbit r0, r0
    bx lr

;*******************************************************************************
; @brief count leading zeros
; @note for example, R0=0x0000ffff
;       RBIT R1 R0
;       R1=16
;*******************************************************************************
__CLZ
    clz r0, r0
    bx lr

  END
  
