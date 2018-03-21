/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _STM32F10X_I2C_H_
  #define _STM32F10X_I2C_H_


#include "types.h"



/* i2c address bits */
#define I2C_Address_7Bits     (0x00)
#define I2C_Address_10Bits    (1 << 15)
#define IS_I2C_Address_Param(address)  ((address == I2C_Address_7Bits) || \
                                        (address == I2C_Address_10Bits))



/* i2c interrupt bits */
#define I2C_IT_BUFFEN    (1 << 10)
#define I2C_IT_EVTEN     (1 << 9)
#define I2C_IT_ERREN     (1 << 8)
#define IS_I2C_IT_Param(it)    ((it == I2C_IT_BUFFEN) || \
                                (it == I2C_IT_EVTEN) || \
                                (it == I2C_IT_ERREN))

/* i2c status */
//SR1
#define I2C_FLAG_SMBALERT           (1 << 15)
#define I2C_FLAG_TIMEOUT            (1 << 14)
#define I2C_FLAG_PECERR             (1 << 12)
#define I2C_FLAG_OVR                (1 << 11)
#define I2C_FLAG_AF                 (1 << 10)
#define I2C_FLAG_ARLO               (1 << 9)
#define I2C_FLAG_BERR               (1 << 8)
#define I2C_FLAG_TXE                (1 << 7)
#define I2C_FLAG_RXNE               (1 << 6)
#define I2C_FLAG_STOPF              (1 << 4)
#define I2C_FLAG_ADD10              (1 << 3)
#define I2C_FLAG_BTF                (1 << 2)
#define I2C_FLAG_ADDR               (1 << 1)
#define I2C_FALG_SB                 (1 << 0)
//SR2
#define I2C_FLAG_DUALF              (0x80000080)
#define I2C_FLAG_SMBHOST            (0x80000040)
#define I2C_FLAG_SMBDEFAULT         (0x80000020)
#define I2C_FLAG_GENCALL            (0x80000010)
#define I2C_FLAG_TRA                (0x80000004)
#define I2C_FLAG_BUSY               (0x80000002)
#define I2C_FLSG_MSL                (0x80000001)

#define IS_I2C_Flag_Param(flag)      ((flag == I2C_FLAG_SMBALERT) || \
                                      (flag == I2C_FLAG_SMBALERT) || \
                                      (flag == I2C_FLAG_SMBALERT) || \
                                      (flag == I2C_FLAG_SMBALERT) || \
                                      (flag == I2C_FLAG_SMBALERT) || \
                                      (flag == I2C_FLAG_SMBALERT) || \
                                      (flag == I2C_FLAG_SMBALERT) || \
                                      (flag == I2C_FLAG_SMBALERT) || \
                                      (flag == I2C_FLAG_SMBALERT) || \
                                      (flag == I2C_FLAG_SMBALERT) || \
                                      (flag == I2C_FLAG_SMBALERT) || \
                                      (flag == I2C_FLAG_SMBALERT) || \
                                      (flag == I2C_FLAG_SMBALERT) || \
                                      (flag == I2C_FLAG_SMBALERT) || \
                                      (flag == I2C_FLAG_DUALF) || \
                                      (flag == I2C_FLAG_SMBHOST) || \
                                      (flag == I2C_FLAG_SMBDEFAULT) || \
                                      (flag == I2C_FLAG_GENCALL) || \
                                      (flag == I2C_FLAG_TRA) || \
                                      (flag == I2C_FLAG_BUSY) || \
                                      (flag == I2C_FLSG_MSL))


/* i2c error */
#define I2C_ERROR_SMBALERT          (1 << 15)
#define I2C_ERROR_TIMEOUT           (1 << 14)
#define I2C_ERROR_PECERR            (1 << 12)
#define I2C_ERR_OVR                 (1 << 11)
#define I2C_ERR_AF                  (1 << 10)
#define I2C_ERROR_ARLO              (1 << 9)
#define I2C_ERROR_BERR              (1 << 8)


/* i2c event */

/* BUSY, MSL and SB flag */
#define  I2C_EVENT_MASTER_MODE_SELECT                      (0x00030001)  
/* BUSY, MSL, ADDR, TXE and TRA flags */
#define  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED        (0x00070082)
/* BUSY, MSL and ADDR flags */
#define  I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED           (0x00030002)  
/* BUSY, MSL and ADD10 flags */
#define  I2C_EVENT_MASTER_MODE_ADDRESS10                   (0x00030008)  
/* BUSY, MSL and RXNE flags */
#define  I2C_EVENT_MASTER_BYTE_RECEIVED                    (0x00030040)  
/* TRA, BUSY, MSL, TXE flags */
#define I2C_EVENT_MASTER_BYTE_TRANSMITTING                 (0x00070080) 
/* TRA, BUSY, MSL, TXE and BTF flags */
#define  I2C_EVENT_MASTER_BYTE_TRANSMITTED                 (0x00070084)  


/* i2c group definition */
typedef enum
{
    I2C1,
    I2C2,
    I2C_Count,
}I2C_Group;



typedef struct
{
    uint32_t clock;
    uint16_t addressBits;
    uint16_t selfAddress;
    uint8_t selfAddressDual;
    bool enableDualAddress;
    bool enableStrech;
    bool enableGeneralCall;
    bool enablePEC;
    bool enableDMA;
}I2C_Config;


/* interface */
void I2C_Reset(I2C_Group group);
void I2C_Enable(I2C_Group group, bool flag);
void I2C_Setup(I2C_Group group, I2C_Config *config);
void I2C_StructInit(I2C_Config *config);
void I2C_GenerateStart(I2C_Group group);
void I2C_GenerateStop(I2C_Group group);
void I2C_EnableInt(I2C_Group group, uint16_t intFlag, bool flag);
void I2C_WriteData(I2C_Group group,  uint8_t data);
uint8_t I2C_ReadData(I2C_Group group);
bool I2C_Is_FlagSet(I2C_Group group, uint32_t flag);
uint32_t I2C_GetEvent(I2C_Group group);
uint32_t I2C_GetError(I2C_Group group);
void I2C_ClearError(I2C_Group group, uint32_t error);
void I2C_AckEnable(I2C_Group group, bool flag);


#endif /* _STM32F10X_I2C_H */

