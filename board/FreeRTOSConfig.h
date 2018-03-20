#ifndef FREERTOS_CONFIG_H
  #define FREERTOS_CONFIG_H

#include "environment.h"

/* application specific definitions */
#define configUSE_PREEMPTION		1
#define configUSE_IDLE_HOOK			0
#define configUSE_TICK_HOOK			0
#define configCPU_CLOCK_HZ			((unsigned long)72000000)	
#define configTICK_RATE_HZ			((TickType_t)1000)
#define configMAX_PRIORITIES		(5)
#define configMINIMAL_STACK_SIZE	((unsigned short)128)
#define configTOTAL_HEAP_SIZE		((size_t)(15 * 1024))
#define configMAX_TASK_NAME_LEN		(16)
#define configUSE_TRACE_FACILITY	0
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		1
#define configUSE_MUTEXES           1


/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES (2)

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		        0
#define INCLUDE_uxTaskPriorityGet		        0
#define INCLUDE_vTaskDelete				        1
#define INCLUDE_vTaskCleanUpResources	        0
#define INCLUDE_vTaskSuspend			        0
#define INCLUDE_vTaskDelayUntil			        0
#define INCLUDE_vTaskDelay				        1
#define INCLUDE_uxTaskGetStackHighWaterMark     1

/* value can be 0(highest) to 15(lowest)*/
#define configKERNEL_INTERRUPT_PRIORITY 		(15)


#ifdef __DEBUG
extern void assert_failed(const char *file, const char *line, const char *exp);
#define STR(x) VAL(x)
#define VAL(x) #x
#define configASSERT(expr) ((expr) ? (void)0 :       \
          assert_failed(__FILE__, STR(__LINE__), #expr))
#endif


#endif /* FREERTOS_CONFIG_H */

