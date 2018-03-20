#include "sysdef.h"
#include "i2c.h"
#include "delay.h"
#include "stm32f10x_cfg.h"
#include <string.h>

/* i2c handle definition */
typedef struct
{
    Port port;
    uint8 slaveAddress;
    GPIO_Group sdaPinGroup;
    GPIO_Group sclPinGroup;
    uint8 sdaPinPort;
    uint8 sclPinPort;
}I2C_T;

/* pin operation */
static __INLINE void SDA_H(__in Handle i2c)
{
    GPIO_SetPin(((I2C_T *)i2c)->sdaPinGroup, ((I2C_T *)i2c)->sdaPinPort);
}

static __INLINE void SDA_L(__in Handle i2c)
{
    GPIO_ResetPin(((I2C_T *)i2c)->sdaPinGroup, ((I2C_T *)i2c)->sdaPinPort);
}

static __INLINE void SCL_H(__in Handle i2c)
{
    GPIO_SetPin(((I2C_T *)i2c)->sclPinGroup, ((I2C_T *)i2c)->sclPinPort);
}

static __INLINE void SCL_L(__in Handle i2c)
{
    GPIO_ResetPin(((I2C_T *)i2c)->sclPinGroup, ((I2C_T *)i2c)->sclPinPort);
}

static __INLINE uint8 SDA_VAL(__in Handle i2c)
{
    return GPIO_ReadPin(((I2C_T *)i2c)->sdaPinGroup, 
                        ((I2C_T *)i2c)->sdaPinPort);
}


static void i2c_start(__in Handle i2c)
{
	SDA_H(i2c);
	SCL_H(i2c);
	delay_us(4);
	SDA_L(i2c);
	delay_us(4);
	SCL_L(i2c);
}

static void i2c_stop(__in Handle i2c)
{
	SCL_L(i2c);
	SDA_L(i2c);
	delay_us(4);
	SCL_H(i2c);
	delay_us(4);
	SDA_H(i2c);
	delay_us(4);
}

static void i2c_ack(__in Handle i2c)
{
	SCL_L(i2c);
	SDA_L(i2c);
    delay_us(2);
	SCL_H(i2c);
    delay_us(2);
	SCL_L(i2c);
}

static void i2c_nack(__in Handle i2c)
{
	SCL_L(i2c);
	SDA_H(i2c);
    delay_us(2);
	SCL_H(i2c);
	delay_us(2);
	SCL_L(i2c);
}

static uint8 i2c_waitack(__in Handle i2c)
{
	uint8 errCnt = 0;
	SDA_H(i2c);
	delay_us(1);
	SCL_H(i2c);
	delay_us(1);
    

	while(SDA_VAL(i2c))
	{
		errCnt++;
		if(errCnt > 200)
		{
			i2c_stop(i2c);
			return 1;
		}
	}

	SCL_L(i2c);
	return 0;
}

static void i2c_write(__in Handle i2c, __in uint8 data)
{
	SCL_L(i2c);
	for(uint8 i = 0; i < 8; ++i)
	{
		if(data & 0x80)
			SDA_H(i2c);
		else
			SDA_L(i2c);
		data <<= 1;
        delay_us(2);
		SCL_H(i2c);
        delay_us(2);
		SCL_L(i2c);
        delay_us(2);
	}
}

static uint8 i2c_read(__in Handle i2c, __in BOOL ack)
{
	uint8 data = 0;
	for(uint8 i = 0; i < 8; ++i)
	{
		SCL_L(i2c);
        SDA_H(i2c);
        delay_us(2);
		SCL_H(i2c);
		data <<= 1;
		if(SDA_VAL(i2c))
			data++;
        delay_us(2);
	}

	if(ack)
		i2c_ack(i2c);
	else
		i2c_nack(i2c);
	
	return data;
}

/**
 * @brief request i2c device
 * @param i2c port
 */
static Handle I2c_Request(__in Port port)
{
    assert_param(port < Port_Count);
    I2C_T *i2c = pvPortMalloc(sizeof(I2C_T) / sizeof(char));
    if(i2c == NULL)
        return NULL;
    i2c->port = port;
    i2c->slaveAddress = 0xff;
    if(port == I2c1)
    {
        i2c->sclPinGroup = GPIOB;
        i2c->sdaPinGroup = GPIOB;
        i2c->sclPinPort = 6;
        i2c->sdaPinPort = 7;
    }
    else
    {
        i2c->sclPinGroup = GPIOB;
        i2c->sdaPinGroup = GPIOB;
        i2c->sclPinPort = 10;
        i2c->sdaPinPort = 11;
    }
    return i2c;
}

/**
 * @brief open i2c port
 * @param i2c handle
 */
static BOOL I2c_Open(__in Handle handle)
{
    UNUSED(handle);
    return TRUE;
}

/**
 * @brief close serial port
 * @param serial port handle
 */
static void I2c_Close(__in Handle handle)
{
    assert_param(handle != NULL);  
    vPortFree(handle);
}

/**
 * @brief set serial port stop bits
 * @param handle: serial handle
 * @param stopBits: stop bits
 */
static void I2c_SetSpeed(__in Handle handle, __in uint32 speed)
{
    UNUSED(handle);
    UNUSED(speed);
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
    UNUSED(handle);
    UNUSED(rxLen);
    UNUSED(txLen);
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
    I2C_T *i2c = (I2C_T *)handle;
    //start transfer
    i2c_start(i2c);
    //send address
    i2c_write(i2c, i2c->slaveAddress << 1);
    if(i2c_waitack(i2c) != 0)
        return FALSE;
    //send data
    const char *pData = data;
    while(length--)
    {
        i2c_write(i2c, *pData);
        if(i2c_waitack(i2c) != 0)
            return FALSE;
        pData++;
    }
    //stop transfer
    i2c_stop(i2c);
    
    return TRUE;
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
    //start transfer
    i2c_start(i2c);
    //send address
    i2c_write(i2c, (i2c->slaveAddress << 1) | 0x01);
    if(i2c_waitack(i2c) != 0)
        return FALSE;
    //read data
    char *pData = data;
    if(length == 0)
        i2c_stop(i2c);
    else
    {
        length -= 1;
        while(length--)
        {
            *pData++ = i2c_read(i2c, TRUE);
        }
        *pData++ = i2c_read(i2c, FALSE);
        //stop transfer
        i2c_stop(i2c);
    }
    
    return TRUE;
}

/**
 * @brief init software i2c module
 */
void softwareI2CInit(void)
{
    I2C_Device device;
    strcpy((char *)device.name, "software");
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

