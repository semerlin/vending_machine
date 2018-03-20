#include "cm3_core.h"
#include "stm32f10x_scb.h"
#include "stm32f10x_systick.h"
#include "stm32f10x_nvic.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "port.h"


/* check syscall priority definition */
#if configMAX_SYSCALL_INTERRUPT_PRIORITY == 0
	#error configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to 0.
#endif


/* systick and pendsv priority */
#define portNVIC_PENDSV_PRI					(configKERNEL_INTERRUPT_PRIORITY)
#define portNVIC_SYSTICK_PRI				(configKERNEL_INTERRUPT_PRIORITY)

/* irq start number */
#define portIRQ_START_NUMBER        		(16)

/* xPSR reset value */
#define portINITIAL_XPSR					(0x01000000)

/* For strict compliance with the Cortex-M spec the task start address should
have bit-0 clear, as it is loaded into the PC on exit from an ISR. */
#define portSTART_ADDRESS_MASK				(( StackType_t )0xfffffffeUL)


/* Each task maintains its own interrupt status in the critical nesting
variable. */
static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;


/* interface */
void vPortSetupTimerInterrupt( void );
void SysTickHandler( void );
extern void vPortStartFirstTask( void );
static void prvTaskExitError( void );


/*
 * @brief initialize new created task's stack, so when task switch happened, 
          the task function can run for the first time
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	/* Simulate the stack frame as it would be created by a context switch
	interrupt. */
	pxTopOfStack--;
	*pxTopOfStack = portINITIAL_XPSR;	/* xPSR */
	pxTopOfStack--;
	*pxTopOfStack = (( StackType_t)pxCode)&portSTART_ADDRESS_MASK;	/* PC */
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t )prvTaskExitError;	/* LR */
	pxTopOfStack -= 5;	/* R12, R3, R2 and R1. */
	*pxTopOfStack = (StackType_t)pvParameters;	/* R0 */
	pxTopOfStack -= 8;	/* R11, R10, R9, R8, R7, R6, R5 and R4. */

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

static void prvTaskExitError( void )
{
	/* A function that implements a task must not exit or attempt to return to
	its caller as there is nothing to return to.  If a task wants to exit it
	should instead call vTaskDelete( NULL ).

	Artificially force an assert() to be triggered if configASSERT() is
	defined, then stop here so application writers can catch the error. */
	configASSERT( uxCriticalNesting == ~0UL );
	portDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

/**
 * @brief setup scheduler functions
 */
BaseType_t xPortStartScheduler( void )
{
	/* make PendSV and SysTick the lowest priority interrupts. */
    SCB_SetExceptionPriorty(SCB_Exception_SysTick, portNVIC_PENDSV_PRI);
    SCB_SetExceptionPriorty(SCB_Exception_PendSV, portNVIC_SYSTICK_PRI);

	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	here already. */
	vPortSetupTimerInterrupt();

	/* Initialise the critical nesting count ready for the first task. */
	uxCriticalNesting = 0;

	/* Start the first task. */
	vPortStartFirstTask();

	/* Should not get here! */
	return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* Not implemented in ports where there is nothing to return to.
	Artificially force an assert. */
	configASSERT( uxCriticalNesting == 1000UL );
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
	portDISABLE_INTERRUPTS();
	uxCriticalNesting++;

	/* This is not the interrupt safe version of the enter critical function so
	assert() if it is being called from an interrupt context.  Only API
	functions that end in "FromISR" can be used in an interrupt.  Only assert if
	the critical nesting count is 1 to protect against recursive calls if the
	assert function also uses a critical section. */
	if(uxCriticalNesting == 1)
	{
        configASSERT(SCB_GetActiveIntVector() == 0);
	}
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	configASSERT( uxCriticalNesting );
	uxCriticalNesting--;
	if( uxCriticalNesting == 0 )
	{
		portENABLE_INTERRUPTS();
	}
}
/*-----------------------------------------------------------*/

void SysTickHandler( void )
{
	/* The SysTick runs at the lowest interrupt priority, so when this interrupt
	executes all interrupts must be unmasked.  There is therefore no need to
	save and then restore the interrupt mask value as its value is already
	known. */
	portDISABLE_INTERRUPTS();
	{
		/* Increment the RTOS tick. */
		if( xTaskIncrementTick() != pdFALSE )
		{
			/* use PendSV to start a context switching. */
            SCB_PendPendSV(TRUE);
		}
	}
	portENABLE_INTERRUPTS();
}
/*-----------------------------------------------------------*/

/**
 * @brief Setup the systick timer to generate the tick interrupts at the 
 *        required frequency.
 */
void vPortSetupTimerInterrupt( void )
{
	/* Configure SysTick to interrupt at the requested rate. */
    SYSTICK_SetClockSource(SYSTICK_CLOCK_AHB_DIV_EIGHT);
    SYSTICK_SetTickInterval(1000 / configTICK_RATE_HZ);
    SYSTICK_EnableInt(TRUE);
    SYSTICK_ClrCountFlag();
    SYSTICK_EnableCounter(TRUE);
}
/*-----------------------------------------------------------*/

#if (configASSERT_DEFINED == 1)
/**
 * @brief validate current running exception priority  
 */
void vPortValidateInterruptPriority( void )
{
    uint32 currentInterrupt;
    uint8 groupPriority;
    uint8 subPriority;

    /* obtain the number of the currently executing interrupt. */
    currentInterrupt = __get_IPSR();

    /* is the interrupt number a irq */
    if(currentInterrupt >= portIRQ_START_NUMBER)
    {
        /* get irq priority */
        NVIC_GetIRQPriority(currentInterrupt - portIRQ_START_NUMBER, 
                            &groupPriority, &subPriority);
        
        /* as we only use group priority, 
        we only need to judge group priority */
        configASSERT(groupPriority >= configMAX_SYSCALL_INTERRUPT_PRIORITY);
    }
}

#endif /* configASSERT_DEFINED */





















