#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "stm32f10x_cfg.h"
#include "i2c.h"
#include "global.h"
#include <string.h>

/* i2c handle definition */
typedef struct
{
    Port port;
    UBaseType_t rxBufLen;
    UBaseType_t txBufLen;
    I2C_Config config;
    uint8 slaveAddress;
    uint32 error;
    uint32 txLen;
    uint32 rxLen;
}I2C_T;



/* The queue used to hold received characters. */
static xQueueHandle xRxedChars[Port_Count];
static xQueueHandle xCharsForTx[Port_Count];

static xSemaphoreHandle xFinishSemphr[Port_Count];

static I2C_T *i2c1_current = NULL;
static I2C_T *i2c2_current = NULL;

#define I2C_NO_BLOCK						((portTickType)0)
#define I2C_TX_BLOCK_TIME				(10 / portTICK_RATE_MS)
#define I2C_RX_BLOCK_TIME               (10 / portTICK_RATE_MS)

/**
 * @brief get system iic resource
 * @return iic handle
 */
static Handle I2c_Request(__in Port port)
{
    assert_param(port < Port_Count);
    I2C_T *i2c = pvPortMalloc(sizeof(I2C_T) / sizeof(char));
    if(i2c == NULL)
        return NULL;
    i2c->port = port;
    i2c->txBufLen = 64;
    i2c->rxBufLen = 64;
    I2C_StructInit(&i2c->config);
    i2c->slaveAddress = 0xff;
    i2c->error = 0;
    i2c->txLen = 0;
    i2c->rxLen = 0;
    if(port == I2c1)
        i2c1_current = i2c;
    else
        i2c2_current = i2c;
        
    return i2c;
}

/**
 * @brief open i2c port
 * @param i2c handle
 */
static BOOL I2c_Open(__in Handle handle)
{
    assert_param(handle != NULL);
    I2C_T *i2c = (I2C_T *)handle;
    assert_param(i2c->port < Port_Count);
    
    NVIC_Config evNvicConfig = {I2C1_EV_IRQChannel, I2C_EV_PRIORITY, 0, TRUE};
    NVIC_Config erNvicConfig = {I2C1_ER_IRQChannel, I2C_ER_PRIORITY, 0, TRUE};
    /* Create the queues used to hold Rx/Tx characters */
	xRxedChars[i2c->port] = xQueueCreate(i2c->rxBufLen, 
                                            (UBaseType_t)sizeof(portCHAR));
	xCharsForTx[i2c->port] = xQueueCreate(i2c->txBufLen, 
                                             (UBaseType_t)sizeof(portCHAR));
    xFinishSemphr[i2c->port] = xSemaphoreCreateBinary();
                      
    if((xRxedChars[i2c->port] != NULL) && 
       (xCharsForTx[i2c->port] != NULL) &&
       (xFinishSemphr[i2c->port] != NULL))
    {
        i2c->error = 0;
        i2c->txLen = 0;
        i2c->rxLen = 0;
        switch(i2c->port)
        {
        case I2c1:
            I2C_Setup(I2C1, &i2c->config);
            I2C_EnableInt(I2C1, I2C_IT_BUFFEN, TRUE);
            I2C_EnableInt(I2C1, I2C_IT_EVTEN, TRUE);
            I2C_EnableInt(I2C1, I2C_IT_ERREN, TRUE);
            evNvicConfig.channel = I2C1_EV_IRQChannel;
            erNvicConfig.channel = I2C1_ER_IRQChannel;
            NVIC_Init(&evNvicConfig);
            NVIC_Init(&erNvicConfig);
            I2C_Enable(I2C1, TRUE);
            break;
        case I2c2:
            I2C_Setup(I2C2, &i2c->config);
            I2C_EnableInt(I2C2, I2C_IT_BUFFEN, TRUE);
            I2C_EnableInt(I2C2, I2C_IT_EVTEN, TRUE);
            I2C_EnableInt(I2C2, I2C_IT_ERREN, TRUE);
            evNvicConfig.channel = I2C2_EV_IRQChannel;
            erNvicConfig.channel = I2C2_ER_IRQChannel;
            NVIC_Init(&evNvicConfig);
            NVIC_Init(&erNvicConfig);
            I2C_Enable(I2C2, TRUE);
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
static void I2c_Close(__in Handle handle)
{
    assert_param(handle != NULL);
    I2C_T *i2c = (I2C_T *)handle;
    switch(i2c->port)
    {
    case I2c1:
        I2C_EnableInt(I2C1, I2C_IT_BUFFEN, FALSE);
        I2C_EnableInt(I2C1, I2C_IT_EVTEN, FALSE);
        I2C_EnableInt(I2C1, I2C_IT_ERREN, FALSE);
        I2C_Enable(I2C1, FALSE);
        i2c1_current = NULL;
        break;
    case I2c2:
        I2C_EnableInt(I2C2, I2C_IT_BUFFEN, FALSE);
        I2C_EnableInt(I2C2, I2C_IT_EVTEN, FALSE);
        I2C_EnableInt(I2C2, I2C_IT_ERREN, FALSE);
        I2C_Enable(I2C2, FALSE);
        i2c2_current = NULL;
        break;
    default:
        return;
        break;
    }
    vQueueDelete(xRxedChars[i2c->port]);
    vQueueDelete(xCharsForTx[i2c->port]);
    vPortFree(handle);
}

/**
 * @brief set serial port stop bits
 * @param handle: serial handle
 * @param stopBits: stop bits
 */
static void I2c_SetSpeed(__in Handle handle, __in uint32 speed)
{
    assert_param(handle != NULL);
    I2C_T *i2c = (I2C_T *)handle;
    i2c->config.clock = speed;
}

/**
* @brief set slave address
* @param i2c handle
* @param slave address
*/
static void I2c_SetSlaveAddress(__in Handle handle, __in uint8 address)
{
    assert_param(handle != NULL);
    I2C_T *i2c = (I2C_T *)handle;
    i2c->slaveAddress = address;
}

/**
 * @brief set rx and tx buffer length
 * @param handle: serial handle
 * @param rxLen: rx buffer length
 * @param txLen: tx buffer length
 */
static void I2c_SetBufferLength(__in Handle handle, __in UBaseType_t rxLen, 
                            __in UBaseType_t txLen)
{
    assert_param(handle != NULL);
    I2C_T *i2c = (I2C_T *)handle;
    i2c->rxBufLen = rxLen;
    i2c->txBufLen = txLen;
}


/**
* @brief write data to iic bus
* @param i2c handler
* @param data to write
* @param data length
* @warnig length cann't be zero, if length is zero, I2C1_EV_IRQHandler will be
*         stuck in I2C_EVENT_MASTER_BYTE_TRANSMITTING event and 
*         I2C_EVENT_MASTER_BYTE_TRANSMITTED will never reached
*/
static BOOL I2c_Write(__in Handle handle, __in const char *data, __in uint32 length)
{
    assert_param(handle != NULL);
    
    if((data == NULL) || (length < 1))
        return TRUE;
    
    I2C_T *i2c = (I2C_T *)handle;
    i2c->error = 0;
    xQueueReset(xCharsForTx[i2c->port]);
    switch(i2c->port)
    {
    case I2c1:
        //generate start
        I2C_AckEnable(I2C1, FALSE);
        I2C_GenerateStart(I2C1);
        break;
    case I2c2:
        I2C_AckEnable(I2C2, FALSE);
        I2C_GenerateStart(I2C2);
        break;
    default:
        return FALSE;
        break;
    }
    
    //send address
    uint8 address = i2c->slaveAddress;
    address <<= 1;
    xQueueSend(xCharsForTx[i2c->port], &address, I2C_NO_BLOCK);
    //send data to queue
    for(uint32 i = 0; i < length; ++i)
        xQueueSend(xCharsForTx[i2c->port], &data[i], I2C_NO_BLOCK);
    
    BOOL ret = FALSE;
    //wait write finished
    if(xSemaphoreTake(xFinishSemphr[i2c->port], 1000 / portTICK_PERIOD_MS))
    {
        if(i2c->error == 0)
            ret = TRUE;
    }
    else
    {
        if(i2c->port == I2c1)
            I2C_GenerateStop(I2C1);
        else
            I2C_GenerateStop(I2C2);
    }
    
    return ret;
}

/**
 * @brief read data from i2c bus
 * @param i2c handle
 * @param data buffer
 * @param data length
 */
static BOOL I2c_Read(__in Handle handle, __out char *data, __in uint32 length)
{
    assert_param(handle != NULL);
    I2C_T *i2c = (I2C_T *)handle;
    i2c->error = 0;
    xQueueReset(xRxedChars[i2c->port]);
    switch(i2c->port)
    {
    case I2c1:
        //generate start
        I2C_AckEnable(I2C1, TRUE);
        I2C_GenerateStart(I2C1);
        break;
    case I2c2:
        I2C_AckEnable(I2C2, TRUE);
        I2C_GenerateStart(I2C2);
        break;
    default:
        return FALSE;
        break;
    }
    
    i2c->rxLen = length;
    //send address
    uint8 address = i2c->slaveAddress;
    address <<= 1;
    address |= 1;
    xQueueSend(xCharsForTx[i2c->port], &address, I2C_NO_BLOCK);
     
    BOOL ret = FALSE;
    //wait write finished
    if(xSemaphoreTake(xFinishSemphr[i2c->port], 
                      1000 / portTICK_PERIOD_MS))
    {
        if(i2c->error == 0)
            ret = TRUE;
    }
    else
    {
        if(i2c->port == I2c1)
            I2C_GenerateStop(I2C1);
        else
            I2C_GenerateStop(I2C2);
        
        return ret;
    }
    
    //start read
    portCHAR cChar;
    char *pData = data;
    while(length--)
    {
        if(xQueueReceive(xRxedChars[i2c->port], &cChar, 
                         1000 / portTICK_PERIOD_MS))
        {
            *pData++ = cChar;
        }
        else
        {
            if(i2c->port == I2c1)
                I2C_GenerateStop(I2C1);
            else
                I2C_GenerateStop(I2C2);
        }
    }
    
    if(length == 0)
        ret = TRUE;
    
    return ret;
}

/**
 * @brief init hardware i2c module
 */
void hardwareI2CInit(void)
{
    I2C_Device device;
    strcpy((char *)device.name, "hardware");
    device.i2c_open = I2c_Open;
    device.i2c_close = I2c_Close;
    device.i2c_read = I2c_Read;
    device.i2c_request = I2c_Request;
    device.i2c_setbufferlength = I2c_SetBufferLength;
    device.i2c_setslaveaddress = I2c_SetSlaveAddress;
    device.i2c_setspeed = I2c_SetSpeed;
    device.i2c_write = I2c_Write;
    I2c_RegisterDevice(&device);
}

/**
 * @brief i2c1 event handler
 */
void I2C1_EV_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    portCHAR cChar;
    
    switch(I2C_GetEvent(I2C1))
    {
    case I2C_EVENT_MASTER_MODE_SELECT:
        if(xQueueReceiveFromISR(xCharsForTx[0], &cChar, 
                                &xHigherPriorityTaskWoken) == pdTRUE)
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			I2C_WriteData(I2C1, cChar);
		}
        break;
    case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
        if(xQueueReceiveFromISR(xCharsForTx[0], &cChar, 
                                &xHigherPriorityTaskWoken) == pdTRUE)
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			I2C_WriteData(I2C1, cChar);
		}
        break;
    case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
        if(i2c1_current->rxLen == 1)
        {
            I2C_AckEnable(I2C1, FALSE);
            I2C_GenerateStop(I2C1);
        }
        xSemaphoreGiveFromISR(xFinishSemphr[0], &xHigherPriorityTaskWoken);
        break;
    case I2C_EVENT_MASTER_MODE_ADDRESS10:
        break;
    case I2C_EVENT_MASTER_BYTE_RECEIVED:
        i2c1_current->rxLen--;
        cChar = I2C_ReadData(I2C1);
        if(i2c1_current->rxLen == 0)
        {
            xQueueSendFromISR(xRxedChars[0], &cChar, &xHigherPriorityTaskWoken);
        }
        else if(i2c1_current->rxLen == 1)
        {
            I2C_AckEnable(I2C1, FALSE);
            I2C_GenerateStop(I2C1);
            xQueueSendFromISR(xRxedChars[0], &cChar, &xHigherPriorityTaskWoken);
        }
        else
        {
            xQueueSendFromISR(xRxedChars[0], &cChar, &xHigherPriorityTaskWoken);
        }
        break;
    case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
        if(xQueueReceiveFromISR(xCharsForTx[0], &cChar, 
                                &xHigherPriorityTaskWoken) == pdTRUE)
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			I2C_WriteData(I2C1, cChar);
		}
        break;
    case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
        I2C_GenerateStop(I2C1);
        xSemaphoreGiveFromISR(xFinishSemphr[0], &xHigherPriorityTaskWoken);
        break;
    default:
        I2C_WriteData(I2C2, 0xff);
        //I2C_GenerateStop(I2C1);
        break;
    }

    //check if there is any higher priority task need to wakeup
	//portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

/**
* @brief i2c1 error handler
*/
void I2C1_ER_IRQHandler(void)
{
    uint32 error = I2C_GetError(I2C1);
    if(i2c1_current != NULL)
        i2c1_current->error = I2C_GetError(I2C1);
    I2C_GenerateStop(I2C1);
    I2C_ClearError(I2C1, error);
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xFinishSemphr[0], &xHigherPriorityTaskWoken);
    
    //portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}


/**
 * @brief i2c2 event handler
 */
void I2C2_EV_IRQHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    portCHAR cChar;
    
    switch(I2C_GetEvent(I2C2))
    {
    case I2C_EVENT_MASTER_MODE_SELECT:
        if(xQueueReceiveFromISR(xCharsForTx[1], &cChar, 
                                &xHigherPriorityTaskWoken) == pdTRUE)
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			I2C_WriteData(I2C2, cChar);
		}
        break;
    case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
        if(xQueueReceiveFromISR(xCharsForTx[1], &cChar, 
                                &xHigherPriorityTaskWoken) == pdTRUE)
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			I2C_WriteData(I2C2, cChar);
		}
        break;
    case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
        if(i2c2_current->rxLen == 1)
        {
            I2C_AckEnable(I2C2, FALSE);
            I2C_GenerateStop(I2C2);
        }
        xSemaphoreGiveFromISR(xFinishSemphr[1], &xHigherPriorityTaskWoken);
        break;
    case I2C_EVENT_MASTER_MODE_ADDRESS10:
        break;
    case I2C_EVENT_MASTER_BYTE_RECEIVED:
        i2c2_current->rxLen--;
        cChar = I2C_ReadData(I2C2);
        if(i2c2_current->rxLen == 0)
        {
            xQueueSendFromISR(xRxedChars[1], &cChar, &xHigherPriorityTaskWoken);
        }
        else if(i2c2_current->rxLen == 1)
        {
            I2C_AckEnable(I2C2, FALSE);
            I2C_GenerateStop(I2C2);
            xQueueSendFromISR(xRxedChars[1], &cChar, &xHigherPriorityTaskWoken);
        }
        else
        {
            xQueueSendFromISR(xRxedChars[1], &cChar, &xHigherPriorityTaskWoken);
        }
        break;
    case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
        if(xQueueReceiveFromISR(xCharsForTx[1], &cChar, 
                                &xHigherPriorityTaskWoken) == pdTRUE)
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			I2C_WriteData(I2C2, cChar);
		}
        break;
    case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
        I2C_GenerateStop(I2C2);
        xSemaphoreGiveFromISR(xFinishSemphr[1], &xHigherPriorityTaskWoken);
        break;
    default:
        //当没有条件符合时，给DR寄存器写数据清除SB位
        //之前运行发现，有时会只有SB位置位，导致中断一直进入，造成程序假死现象
        I2C_WriteData(I2C2, 0xff);
        //I2C_GenerateStop(I2C2);
        break;
    }

    //check if there is any higher priority task need to wakeup
	//portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

/**
* @brief i2c2 error handler
*/
void I2C2_ER_IRQHandler(void)
{
    uint32 error = I2C_GetError(I2C2);
    if(i2c2_current != NULL)
        i2c2_current->error = I2C_GetError(I2C2);
    I2C_GenerateStop(I2C2);
    I2C_ClearError(I2C2, error);
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xFinishSemphr[1], &xHigherPriorityTaskWoken);
    
    //portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}







	
