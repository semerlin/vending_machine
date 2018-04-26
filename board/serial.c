/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "stm32f10x_cfg.h"
#include "serial.h"
#include "global.h"
#include "dbgserial.h"

/* serial handle definition */
struct _serial_t
{
    Port port;
    UBaseType_t rxBufLen;
    USART_Config config;
};

/* The queue used to hold received characters. */
static xQueueHandle xRxedChars[Port_Count];

#define SERIAL_NO_BLOCK						((portTickType)0)
#define SERIAL_TX_BLOCK_TIME				(10 / portTICK_RATE_MS)

/**
 * @brief get system serial resource
 * @return serial handle
 */
serial *serial_request(Port port)
{
    assert_param(port < Port_Count);
    serial *pserial = pvPortMalloc(sizeof(serial) / sizeof(char));
    if (NULL == pserial)
    {
        return NULL;
    }
    pserial->port = port;
    pserial->rxBufLen = 128;
    USART_StructInit(&pserial->config);

    return pserial;
}

/**
 * @brief release serial
 * @param pserial - serial handle
 */
void serial_release(serial *pserial)
{
    vPortFree(pserial);
}

/**
 * @brief open serial port
 * @param serial handle
 */
bool serial_open(serial *handle)
{
    assert_param(handle != NULL);
    assert_param(handle->port < Port_Count);
    
    NVIC_Config nvicConfig = {USART1_IRQChannel, USART1_PRIORITY, 0, TRUE};
    
    /* Create the queues used to hold Rx/Tx characters */
	xRxedChars[handle->port] = xQueueCreate(handle->rxBufLen, 
                                            (UBaseType_t)sizeof(portCHAR));
                                             
    if (NULL != xRxedChars[handle->port])
    {
        switch(handle->port)
        {
        case COM1:
            USART_Setup(USART1, &handle->config);
            USART_EnableInt(USART1, USART_IT_RXNE, TRUE);
            nvicConfig.channel = USART1_IRQChannel;
            NVIC_Init(&nvicConfig);
            USART_Enable(USART1, TRUE);
            break;
        case COM2:
            USART_Setup(USART2, &handle->config);
            USART_EnableInt(USART2, USART_IT_RXNE, TRUE);
            nvicConfig.channel = USART2_IRQChannel;
            NVIC_Init(&nvicConfig);
            USART_Enable(USART2, TRUE);
            break;
        case COM3:
            USART_Setup(USART3, &handle->config);
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
void serial_close(serial *handle)
{
    assert_param(handle != NULL);
    serial *pserial = (serial *)handle;
    switch(pserial->port)
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
    vQueueDelete(xRxedChars[pserial->port]);
    vPortFree(handle);
}

/**
 * @brief set serial port baudrate
 * @param handle: serial handle
 * @param baudrate: baudrate
 */
void serial_set_baudrate(serial *handle, Baudrate baudrate)
{
    assert_param(handle != NULL);
    serial *pserial = (serial *)handle;
    pserial->config.baudRate = baudrate;
}

/**
 * @brief set serial port parity
 * @param handle: serial handle
 * @param parity: parity
 */
void serial_set_parity(serial *handle, Parity parity)
{
    assert_param(handle != NULL);
    serial *pserial = (serial *)handle;
    switch(parity)
    {
    case No:
        pserial->config.parity = USART_Parity_None;
        break;
    case Even:
        pserial->config.parity = USART_Parity_Even;
        break;
    case Odd:
        pserial->config.parity = USART_Parity_Odd;
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
void serial_set_stopbits(serial *handle, StopBits stopBits)
{
    assert_param(handle != NULL);
    serial *pserial = (serial *)handle;
    switch(stopBits)
    {
    case STOP_1:
        pserial->config.stopBits = USART_StopBits_1;
        break;
    case STOP_1_5:
        pserial->config.stopBits = USART_StopBits_1_5;
        break;
	case STOP_2:
        pserial->config.stopBits = USART_StopBits_2;
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
void serial_set_databits(serial *handle, DataBits dataBits)
{
    assert_param(handle != NULL);
    serial *pserial = (serial *)handle;
    switch(dataBits)
    {
    case BITS_8:
        pserial->config.wordLength = USART_WordLength_8;
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
void serial_set_bufferlength(serial *handle, UBaseType_t rxLen, 
                            UBaseType_t txLen)
{
    assert_param(handle != NULL);
    serial *pserial = (serial *)handle;
    pserial->rxBufLen = rxLen;
}

/**
 * @brief get a char from serial port
 * @return TRUE: success FALSE: timeout
 */
bool serial_getchar(serial *handle, char *data, 
                    portTickType xBlockTime)
{
    assert_param(handle != NULL);
    serial *pserial = (serial *)handle;
    if(xQueueReceive(xRxedChars[pserial->port], data, xBlockTime))
		return TRUE;
	else
		return FALSE;
}

/**
 * @brief put a char from serial port
 * @return TRUE: success FALSE: timeout
 */
bool serial_putchar(serial *handle, char data,
                    portTickType xBlockTime)
{
    assert_param(handle != NULL);
    serial *pserial = (serial *)handle;
    
    switch(pserial->port)
    {
    case COM1:
        USART_WriteData_Wait(USART1, data);
        break;
    case COM2:
        USART_WriteData_Wait(USART2, data);
        break;
    case COM3:
        USART_WriteData_Wait(USART3, data);
        break;
    default:
        return FALSE;
        break;
    }
    return TRUE;
}

/**
 * @brief put string to serial port
 * @param string to put
 * @param string length
 */
void serial_putstring(serial *handle, const char *string,
                      uint32_t length)
{
    const char *pNext = string;
    while(length--)
        serial_putchar(handle, *pNext++, SERIAL_NO_BLOCK);
}

/**
 * @brief usart interrupt handler
 */
void USART1_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    portCHAR cChar;
	
    /* The interrupt was caused by the RX not empty. */
	if(USART_IsFlagOn(USART1, USART_FLAG_RXNE))
	{
		cChar = USART_ReadData(USART1);
		xQueueSendFromISR(xRxedChars[0], &cChar, &xHigherPriorityTaskWoken);
	}	
	
    /* check if there is any higher priority task need to wakeup */
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief usart interrupt handler
 */
void USART2_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    portCHAR cChar;

    /* The interrupt was caused by the RX not empty. */
	if(USART_IsFlagOn(USART2, USART_FLAG_RXNE))
	{
		cChar = USART_ReadData(USART2);
		xQueueSendFromISR(xRxedChars[1], &cChar, &xHigherPriorityTaskWoken);
	}	
	
    /* check if there is any higher priority task need to wakeup */
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief usart interrupt handler
 */
void USART3_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    portCHAR cChar;
	
    /* The interrupt was caused by the RX not empty. */
	if(USART_IsFlagOn(USART3, USART_FLAG_RXNE))
	{
		cChar = USART_ReadData(USART3);
		xQueueSendFromISR(xRxedChars[2], &cChar, &xHigherPriorityTaskWoken);
	}	
	
    /* check if there is any higher priority task need to wakeup */
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}






	
