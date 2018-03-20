/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "stm32f10x_cfg.h"
#include "serial.h"
#include "global.h"

/* serial handle definition */
typedef struct
{
    Port port;
    UBaseType_t rxBufLen;
    UBaseType_t txBufLen;
    USART_Config config;
}SERIAL_T;

/* The queue used to hold received characters. */
static xQueueHandle xRxedChars[Port_Count];
static xQueueHandle xCharsForTx[Port_Count];

#define SERIAL_NO_BLOCK						((portTickType)0)
#define SERIAL_TX_BLOCK_TIME				(10 / portTICK_RATE_MS)

/**
 * @brief get system serial resource
 * @return serial handle
 */
Handle Serial_Request(__in Port port)
{
    assert_param(port < Port_Count);
    SERIAL_T *serial = pvPortMalloc(sizeof(SERIAL_T) / sizeof(char));
    if(serial == NULL)
        return NULL;
    serial->port = port;
    serial->txBufLen = 64;
    serial->rxBufLen = 64;
    USART_StructInit(&serial->config);

    return serial;
}

/**
 * @brief open serial port
 * @param serial handle
 */
BOOL Serial_Open(__in Handle handle)
{
    assert_param(handle != NULL);
    SERIAL_T *serial = (SERIAL_T *)handle;
    assert_param(serial->port < Port_Count);
    
    NVIC_Config nvicConfig = {USART1_IRQChannel, USART1_PRIORITY, 0, TRUE};
    
    /* Create the queues used to hold Rx/Tx characters */
	xRxedChars[serial->port] = xQueueCreate(serial->rxBufLen, 
                                            (UBaseType_t)sizeof(portCHAR));
	xCharsForTx[serial->port] = xQueueCreate(serial->txBufLen, 
                                             (UBaseType_t)sizeof(portCHAR));
                                             
    if((xRxedChars[serial->port] != NULL) && 
       (xCharsForTx[serial->port] != NULL))
    {
        switch(serial->port)
        {
        case COM1:
            USART_Setup(USART1, &serial->config);
            USART_EnableInt(USART1, USART_IT_RXNE, TRUE);
            nvicConfig.channel = USART1_IRQChannel;
            NVIC_Init(&nvicConfig);
            USART_Enable(USART1, TRUE);
            break;
        case COM2:
            USART_Setup(USART2, &serial->config);
            USART_EnableInt(USART2, USART_IT_RXNE, TRUE);
            nvicConfig.channel = USART2_IRQChannel;
            NVIC_Init(&nvicConfig);
            USART_Enable(USART2, TRUE);
            break;
        case COM3:
            USART_Setup(USART3, &serial->config);
            USART_EnableInt(USART3, USART_IT_RXNE, TRUE);
            nvicConfig.channel = USART3_IRQChannel;
            NVIC_Init(&nvicConfig);
            USART_Enable(USART3, TRUE);
            break;
        default:
            return FALSE;
            break;
        }
    }    
    
    return TRUE;
}

/**
 * @brief close serial port
 * @param serial port handle
 */
void Serial_Close(__in Handle handle)
{
    assert_param(handle != NULL);
    SERIAL_T *serial = (SERIAL_T *)handle;
    switch(serial->port)
    {
    case COM1:
        USART_EnableInt(USART1, USART_IT_TXE, FALSE);
        USART_EnableInt(USART1, USART_IT_RXNE, FALSE);
        USART_Enable(USART1, FALSE);
        break;
    case COM2:
        USART_EnableInt(USART2, USART_IT_TXE, FALSE);
        USART_EnableInt(USART2, USART_IT_RXNE, FALSE);
        USART_Enable(USART2, FALSE);
        break;
    case COM3:
        USART_EnableInt(USART3, USART_IT_TXE, FALSE);
        USART_EnableInt(USART3, USART_IT_RXNE, FALSE);
        USART_Enable(USART3, FALSE);
        break;
    default:
        break;
    }
    vQueueDelete(xRxedChars[serial->port]);
    vQueueDelete(xCharsForTx[serial->port]);
    vPortFree(handle);
}

/**
 * @brief set serial port baudrate
 * @param handle: serial handle
 * @param baudrate: baudrate
 */
void Serial_SetBaudrate(__in Handle handle, __in Baudrate baudrate)
{
    assert_param(handle != NULL);
    SERIAL_T *serial = (SERIAL_T *)handle;
    serial->config.baudRate = baudrate;
}

/**
 * @brief set serial port parity
 * @param handle: serial handle
 * @param parity: parity
 */
void Serial_SetParity(__in Handle handle, __in Parity parity)
{
    assert_param(handle != NULL);
    SERIAL_T *serial = (SERIAL_T *)handle;
    switch(parity)
    {
    case No:
        serial->config.parity = USART_Parity_None;
        break;
    case Even:
        serial->config.parity = USART_Parity_Even;
        break;
    case Odd:
        serial->config.parity = USART_Parity_Odd;
        break;
    default:
        break;
    }
}

/**
 * @brief set serial port stop bits
 * @param handle: serial handle
 * @param stopBits: stop bits
 */
void Serial_SetStopBits(__in Handle handle, __in StopBits stopBits)
{
    assert_param(handle != NULL);
    SERIAL_T *serial = (SERIAL_T *)handle;
    switch(stopBits)
    {
    case STOP_1:
        serial->config.stopBits = USART_StopBits_1;
        break;
    case STOP_1_5:
        serial->config.stopBits = USART_StopBits_1_5;
        break;
	case STOP_2:
        serial->config.stopBits = USART_StopBits_2;
        break;
    default:
        break;
    }
}

/**
 * @brief set serial port data bits
 * @param handle: serial handle
 * @param dataBits: data bits
 */
void Serial_setDataBits(__in Handle handle, __in DataBits dataBits)
{
    assert_param(handle != NULL);
    SERIAL_T *serial = (SERIAL_T *)handle;
    switch(dataBits)
    {
    case BITS_8:
        serial->config.wordLength = USART_WordLength_8;
        break;
    default:
        break;
    }
}

/**
 * @brief set rx and tx buffer length
 * @param handle: serial handle
 * @param rxLen: rx buffer length
 * @param txLen: tx buffer length
 */
void Serial_SetBufferLength(__in Handle handle, __in UBaseType_t rxLen, 
                            __in UBaseType_t txLen)
{
    assert_param(handle != NULL);
    SERIAL_T *serial = (SERIAL_T *)handle;
    serial->rxBufLen = rxLen;
    serial->txBufLen = txLen;
}

/**
 * @brief get a char from serial port
 * @return TRUE: success FALSE: timeout
 */
BOOL Serial_GetChar(__in Handle handle, __out char *data, 
                    __in portTickType xBlockTime)
{
    assert_param(handle != NULL);
    SERIAL_T *serial = (SERIAL_T *)handle;
    if(xQueueReceive(xRxedChars[serial->port], data, xBlockTime))
		return TRUE;
	else
		return FALSE;
}

/**
 * @brief put a char from serial port
 * @return TRUE: success FALSE: timeout
 */
BOOL Serial_PutChar(__in Handle handle, __in char data,
                    __in portTickType xBlockTime)
{
    assert_param(handle != NULL);
    SERIAL_T *serial = (SERIAL_T *)handle;
    //send char to queue
	if(xQueueSend(xCharsForTx[serial->port], &data, xBlockTime ) == pdPASS)
	{
        /* enable txe interrupt, so when transmit data register is empty,
           an interrupt occured */
        switch(serial->port)
        {
        case COM1:
            USART_EnableInt(USART1, USART_IT_TXE, TRUE);
            break;
        case COM2:
            USART_EnableInt(USART2, USART_IT_TXE, TRUE);
            break;
        case COM3:
            USART_EnableInt(USART3, USART_IT_TXE, TRUE);
            break;
        default:
            return FALSE;
            break;
        }
        return TRUE;
	}
	else
		return FALSE;
}

/**
 * @brief put string to serial port
 * @param string to put
 * @param string length
 */
void Serial_PutString(__in Handle handle, __in const char *string,
                      __in uint32 length)
{
    UNUSED(length);
    
    const char *pNext = string;
    while(length--)
        Serial_PutChar(handle, *pNext++, SERIAL_NO_BLOCK);
}

/**
 * @brief usart interrupt handler
 */
void USART1_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    portCHAR cChar;

    if(USART_IsFlagOn(USART1, USART_FLAG_TXE))
    {
        /* The interrupt was caused by the THR becoming empty.  Are there any
        more characters to transmit? */
        if(xQueueReceiveFromISR(xCharsForTx[0], &cChar, 
                                &xHigherPriorityTaskWoken) == pdTRUE)
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			USART_WriteData(USART1, cChar);
		}
		else
			USART_EnableInt(USART1, USART_IT_TXE, FALSE);
    }
	
    /* The interrupt was caused by the RX not empty. */
	if(USART_IsFlagOn( USART1, USART_FLAG_RXNE))
	{
		cChar = USART_ReadData(USART1);
		xQueueSendFromISR(xRxedChars[0], &cChar, &xHigherPriorityTaskWoken);
	}	
	
    //check if there is any higher priority task need to wakeup
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief usart interrupt handler
 */
void USART2_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    portCHAR cChar;

    if(USART_IsFlagOn(USART2, USART_IT_TXE))
    {
        /* The interrupt was caused by the THR becoming empty.  Are there any
        more characters to transmit? */
        if(xQueueReceiveFromISR(xCharsForTx[1], &cChar, 
                                &xHigherPriorityTaskWoken) == pdTRUE)
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			USART_WriteData(USART2, cChar);
		}
		else
			USART_EnableInt(USART2, USART_IT_TXE, FALSE);
    }
	
    /* The interrupt was caused by the RX not empty. */
	if(USART_IsFlagOn(USART2, USART_IT_RXNE))
	{
		cChar = USART_ReadData(USART2);
		xQueueSendFromISR(xRxedChars[1], &cChar, &xHigherPriorityTaskWoken);
	}	
	
    //check if there is any higher priority task need to wakeup
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief usart interrupt handler
 */
void USART3_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    portCHAR cChar;

    if(USART_IsFlagOn(USART3, USART_IT_TXE))
    {
        /* The interrupt was caused by the THR becoming empty.  Are there any
        more characters to transmit? */
        if(xQueueReceiveFromISR(xCharsForTx[2], &cChar, 
                                &xHigherPriorityTaskWoken) == pdTRUE)
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			USART_WriteData(USART3, cChar);
		}
		else
			USART_EnableInt(USART3, USART_IT_TXE, FALSE);
    }
	
    /* The interrupt was caused by the RX not empty. */
	if(USART_IsFlagOn(USART1, USART_IT_RXNE))
	{
		cChar = USART_ReadData(USART3);
		xQueueSendFromISR(xRxedChars[2], &cChar, &xHigherPriorityTaskWoken);
	}	
	
    //check if there is any higher priority task need to wakeup
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}






	
