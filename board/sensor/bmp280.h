#ifndef _BMP280_H_
  #define _BMP280_H_

#include "sysdef.h"
#include "i2c.h"

/* bmp280 structure */
typedef struct 
{  
    I2C *i2c;
    uint16 t1;
    int16 t2;
    int16 t3;
    uint16 p1;
    int16 p2;
    int16 p3;
    int16 p4;
    int16 p5;
    int16 p6;
    int16 p7;
    int16 p8;
    int16 p9;
    int32 t_fine;
    uint8 t_sb;  
    uint8 mode;  
    uint8 t_oversampling;  
    uint8 p_oversampling;  
    uint8 filter_coefficient;  
}BMP280_T;

typedef enum {  
    BMP280_T_SKIP = 0x0, 
    BMP280_T_x1,
    BMP280_T_x2,
    BMP280_T_x4,
    BMP280_T_x8,
    BMP280_T_x16
} BMP280_T_OVERSAMPLING;  
  
typedef enum {  
    BMP280_SLEEP = 0x0,  
    BMP280_FORCED = 0x01,  
    BMP280_NORMAL = 0x03,  
} BMP280_WORK_MODE;  
  
typedef enum {  
    BMP280_P_SKIP = 0x0,
    BMP280_P_x1,
    BMP280_P_x2, 
    BMP280_P_x4,
    BMP280_P_x8, 
    BMP280_P_x16
} BMP280_P_OVERSAMPLING;  
  
typedef enum {  
    BMP280_FILTER_OFF = 0x0,     /*filter off*/  
    BMP280_FILTER_x2,            /*0.223*ODR*/  
    BMP280_FILTER_x4,            /*0.092*ODR*/  
    BMP280_FILTER_x8,            /*0.042*ODR*/  
    BMP280_FILTER_x16            /*0.021*ODR*/  
} BMP280_FILTER_COEFFICIENT;  
  
typedef enum {  
    BMP280_T_SB0 = 0x0,     /*0.5ms*/  
    BMP280_T_SB1,           /*62.5ms*/  
    BMP280_T_SB2,           /*125ms*/  
    BMP280_T_SB3,           /*250ms*/  
    BMP280_T_SB4,           /*500ms*/  
    BMP280_T_SB5,           /*1000ms*/  
    BMP280_T_SB6,           /*2000ms*/  
    BMP280_T_SB7,           /*4000ms*/  
} BMP280_T_SB;  


BOOL bmp280Init(__in BMP280_T *bmp280);
void bmp280SetWorkMode(__in BMP280_T *bmp280, BMP280_WORK_MODE mode);
void bmp280GetTemperatureAndPressure(__in BMP280_T *bmp280, 
                                     __out double *temperature, 
                                     __out double *pressure);


#endif