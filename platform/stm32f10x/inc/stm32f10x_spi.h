/**
* This file is part of the vendoring machine project.
*
* Copyright 2018, Huang Yang <elious.huang@gmail.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _STM32F10X_SPI_H_
  #define _STM32F10X_SPI_H_

#include "types.h"

/* spi group definition */
typedef enum
{
    SPI1,
    SPI2,
    SPI3,
    SPI_Count,
}SPI_Group;

/* main mode */
#define SPI_MainMode_Slave          (0x00)
#define SPI_MainMode_Master         (1 << 2)
#define IS_SPI_MainMode_Param(mode) ((mode == SPI_MainMode_Slave) || \
                                     (mode == SPI_MainMode_Master))

/* sub mode */
#define SPI_SubMode_Full_Duplex                (0x00)
#define SPI_SubMode_BiDirection_Write          (3 << 14)
#define SPI_SubMode_BiDIrection_Recv           (1 << 15)
#define SPI_SubMode_UniDirection_RecvOnly      (1 << 10)
#define IS_SPI_SubMode_Param(mode) ((mode == SPI_SubMode_Full_Duplex) || \
                                    (mode == SPI_SubMode_BiDirection_Write) || \
                                    (mode == SPI_SubMode_BiDIrection_Recv) || \
                                    (mode == SPI_SubMode_UniDirection_RecvOnly))


/* clock division */
#define SPI_Clk_Divided_2               (0x00)
#define SPI_Clk_Divided_4               (1 << 3)
#define SPI_Clk_Divided_8               (2 << 3)
#define SPI_Clk_Divided_16              (3 << 3)
#define SPI_Clk_Divided_32              (4 << 3)
#define SPI_Clk_Divided_64              (5 << 3)
#define SPI_Clk_Divided_128             (6 << 3)
#define SPI_Clk_Divided_256             (7 << 3)
#define IS_SPI_Clk_Param(clk)  ((clk == SPI_Clk_Divided_2) || \
                                (clk == SPI_Clk_Divided_4) || \
                                (clk == SPI_Clk_Divided_8) || \
                                (clk == SPI_Clk_Divided_16) || \
                                (clk == SPI_Clk_Divided_32) || \
                                (clk == SPI_Clk_Divided_64) || \
                                (clk == SPI_Clk_Divided_128) || \
                                (clk == SPI_Clk_Divided_256))

/* clock polarity */
#define SPI_Polarity_Low           (0x00)
#define SPI_Polarity_High          (1 << 1)
#define IS_SPI_Polority_Param(polarity)   ((polarity == SPI_Polarity_Low) || \
                                           (polarity == SPI_Polarity_High))

/* clock phase */
#define SPI_Phase_FirstClk           (0x00)
#define SPI_Phase_SecondClk          (0x01)
#define IS_SPI_Phase_Param(phase)   ((phase == SPI_Phase_FirstClk) || \
                                     (phase == SPI_Phase_SecondClk))

/* data length */
#define SPI_DataLength_8Bits       (0x00)
#define SPI_DataLength_16Bits      (1 << 11)
#define IS_SPI_DataLength_Param(length)   \
                        ((length == SPI_DataLength_8Bits) || \
                         (length == SPI_DataLength_16Bits))


/* data format */
#define SPI_DataFormat_MSB   (0x00)
#define SPI_DataFormat_LSB   (1 << 7)                    
#define IS_SPI_DataFormat_Param(format)   \
                        ((format == SPI_DataFormat_MSB) || \
                         (format == SPI_DataFormat_LSB))


/* nss mode*/
#define SPI_NSS_HARDWARE  (0x00)
#define SPI_NSS_SOFTWARE  (1 << 9)  
#define IS_SPI_NSS_Param(NSS) ((NSS == SPI_NSS_HARDWARE) || \
                               (NSS == SPI_NSS_SOFTWARE))

/* interrupt */
#define SPI_IT_TXE     (1 << 7)
#define SPI_IT_RXNE    (1 << 6)
#define SPI_IT_ERRIE   (1 << 5)
#define IS_SPI_IT_Param(IT) ((IT == SPI_IT_TXE) || \
                             (IT == SPI_IT_RXNE) || \
                             (IT == SPI_IT_ERRIE))

/* flag */
#define SPI_Flag_BSY           (1 << 7)
#define SPI_Flag_OVR           (1 << 6)
#define SPI_Flag_MODF          (1 << 5)
#define SPI_Flag_CRCERR        (1 << 4)
#define SPI_Flag_UDR           (1 << 3)
#define SPI_Flag_CHSIDE        (1 << 2)
#define SPI_Flag_TXE           (1 << 1)
#define SPI_Flag_RXNE          (1 << 0)
#define IS_SPI_Flag_Param(flag) ((flag == SPI_Flag_BSY) || \
                                 (flag == SPI_Flag_OVR) || \
                                 (flag == SPI_Flag_MODF) || \
                                 (flag == SPI_Flag_CRCERR) || \
                                 (flag == SPI_Flag_UDR) || \
                                 (flag == SPI_Flag_CHSIDE) || \
                                 (flag == SPI_Flag_TXE) || \
                                 (flag == SPI_Flag_RXNE))

/* spi config structure */
typedef struct
{
    uint32_t mainMode;
    uint32_t subMode;
    uint32_t clock;
    uint32_t polarity;
    uint32_t phase;
    uint32_t length;
    uint32_t format;
    uint32_t nssMode;
    bool useCrc;
}SPI_Config;


/* interface */
void SPI_Enable(SPI_Group group, bool flag);
void SPI_Setup(SPI_Group group, const SPI_Config *config);
void SPI_StructInit(SPI_Config *config);
void SPI_EnableInt(SPI_Group group, uint8_t intflag, bool flag);
bool SPI_IsFlagOn(SPI_Group group, uint8_t flag);
void SPI_SetCRCNext(SPI_Group group, bool flag);
void SPI_EnableTxDMA(SPI_Group group, bool flag);
void SPI_EnableRxDMA(SPI_Group group, bool flag);
void SPI_WriteData(SPI_Group group, uint16_t data);
uint16_t SPI_ReadData(SPI_Group group);
uint16_t SPI_WriteReadDataSync(SPI_Group group, uint16_t data);
void SPI_WriteDataSync(SPI_Group group, uint16_t data);
uint16_t SPI_ReadDataSync_BiDirection(SPI_Group group);
void SPI_SetCRCPolynomial(SPI_Group group, uint16_t data);
uint16_t SPI_GetRxCRC(SPI_Group group);
uint16_t SPI_GetTxCRC(SPI_Group group);


#endif /* _STM32F10X_SPI_H_ */

