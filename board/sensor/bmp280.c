#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "stm32f10x_cfg.h"
#include "global.h"
#include "i2c.h"
#include "bmp280.h"

/* BMP280 address definition */
#define BMP280_ADDRESS               (0x76)
  
#define BMP280_ID                    (0x58)
#define BMP280_RESET_REG_VALUE       (0xB6)  
#define BMP280_SPEED                 (100000)

/*calibration parameters */  
#define BMP280_DIG_T1_LSB_REG                0x88  
#define BMP280_DIG_T1_MSB_REG                0x89  
#define BMP280_DIG_T2_LSB_REG                0x8A  
#define BMP280_DIG_T2_MSB_REG                0x8B  
#define BMP280_DIG_T3_LSB_REG                0x8C  
#define BMP280_DIG_T3_MSB_REG                0x8D  
#define BMP280_DIG_P1_LSB_REG                0x8E  
#define BMP280_DIG_P1_MSB_REG                0x8F  
#define BMP280_DIG_P2_LSB_REG                0x90  
#define BMP280_DIG_P2_MSB_REG                0x91  
#define BMP280_DIG_P3_LSB_REG                0x92  
#define BMP280_DIG_P3_MSB_REG                0x93  
#define BMP280_DIG_P4_LSB_REG                0x94  
#define BMP280_DIG_P4_MSB_REG                0x95  
#define BMP280_DIG_P5_LSB_REG                0x96  
#define BMP280_DIG_P5_MSB_REG                0x97  
#define BMP280_DIG_P6_LSB_REG                0x98  
#define BMP280_DIG_P6_MSB_REG                0x99  
#define BMP280_DIG_P7_LSB_REG                0x9A  
#define BMP280_DIG_P7_MSB_REG                0x9B  
#define BMP280_DIG_P8_LSB_REG                0x9C  
#define BMP280_DIG_P8_MSB_REG                0x9D  
#define BMP280_DIG_P9_LSB_REG                0x9E  
#define BMP280_DIG_P9_MSB_REG                0x9F  
  
#define BMP280_CHIPID_REG                    0xD0  /*Chip ID Register */  
#define BMP280_RESET_REG                     0xE0  /*Softreset Register */  
#define BMP280_STATUS_REG                    0xF3  /*Status Register */  
#define BMP280_CTRLMEAS_REG                  0xF4  /*Ctrl Measure Register */  
#define BMP280_CONFIG_REG                    0xF5  /*Configuration Register */  
#define BMP280_PRESSURE_MSB_REG              0xF7  /*Pressure MSB Register */  
#define BMP280_PRESSURE_LSB_REG              0xF8  /*Pressure LSB Register */  
#define BMP280_PRESSURE_XLSB_REG             0xF9  /*Pressure XLSB Register */  
#define BMP280_TEMPERATURE_MSB_REG           0xFA  /*Temperature MSB Reg */  
#define BMP280_TEMPERATURE_LSB_REG           0xFB  /*Temperature LSB Reg */  
#define BMP280_TEMPERATURE_XLSB_REG          0xFC  /*Temperature XLSB Reg */  



/* interface */
static uint8 bmp280ReadReg(__in I2C *i2c, __in uint8 reg);
static void bmp280WriteReg(__in I2C *i2c, __in uint8 reg, 
                           __in uint8 value);
BOOL bmp280Init(__in BMP280_T *bmp280);
static void bmp280Reset(__in I2C *i2c);
void bmp280SetWorkMode(__in BMP280_T *bmp280, BMP280_WORK_MODE mode);
static double bmp280CompensateTemperatureDouble(__in BMP280_T *bmp280, 
                                                 __in int32 temper);
static double bmp280CompensatePressureDouble(__in BMP280_T *bmp280, 
                                               __in int32 value);
#if 0
static int32 bmp280CompensateTemperatureInt32(__in BMP280_T *bmp280, 
                                               __in int32 temper);
static uint32 bmp280CompensatePressureInt64(__in BMP280_T *bmp280, 
                                              __in int32 value);
#endif
static double bmp280GetTemperature(__in BMP280_T *bmp280);
static double bmp280GetPressure(__in BMP280_T *bmp280);
void bmp280GetTemperatureAndPressure(__in BMP280_T *bmp280, 
                                     __out double *temperature, 
                                     __out double *pressure);



/**
 * @brief read bmp280 register value
 * @param i2c handle
 * @param register address
 * @return register value
 */
static uint8 bmp280ReadReg(__in I2C *i2c, __in uint8 reg)
{
    uint8 data = 0;
    i2c->device->i2c_write(i2c->i2c, (const char *)&reg, 1);
    i2c->device->i2c_read(i2c->i2c, (char *)&data, 1);
    return data;
}

/**
 * @brief write value to bmp280 register
 * @param i2c handle
 * @param register address
 * @param register value
 */
static void bmp280WriteReg(__in I2C *i2c, __in uint8 reg, __in uint8 value)
{
    uint8 data[2] = {0, 0};
    data[0] = reg;
    data[1] = value;
    i2c->device->i2c_write(i2c->i2c, (const char *)data, 2);
}
 
  
/**
 * @brief init bmp280
 * @param bmp280 structure
 * @return init status
 */
BOOL bmp280Init(__in BMP280_T *bmp280)  
{  
    I2C *i2c = bmp280->i2c;
    i2c->device->i2c_setspeed(i2c->i2c, BMP280_SPEED);
    i2c->device->i2c_setslaveaddress(i2c->i2c, BMP280_ADDRESS);
    i2c->device->i2c_open(i2c->i2c);
    
    uint8 id = bmp280ReadReg(i2c, BMP280_CHIPID_REG);
    if(id != BMP280_ID)
        return FALSE;
    
  
    bmp280->mode = BMP280_SLEEP;  
    bmp280->t_sb = BMP280_T_SB1;  
    bmp280->p_oversampling = BMP280_P_x4;  
    bmp280->t_oversampling = BMP280_T_x1;  
    bmp280->filter_coefficient = BMP280_FILTER_x4;

    bmp280->t1 = bmp280ReadReg(i2c, BMP280_DIG_T1_MSB_REG);
    bmp280->t1 <<= 8;
    bmp280->t1 |= bmp280ReadReg(i2c, BMP280_DIG_T1_LSB_REG);
    
    bmp280->t2 = bmp280ReadReg(i2c, BMP280_DIG_T2_MSB_REG);
    bmp280->t2 <<= 8;
    bmp280->t2 |= bmp280ReadReg(i2c, BMP280_DIG_T2_LSB_REG);
    
    bmp280->t3 = bmp280ReadReg(i2c, BMP280_DIG_T3_MSB_REG);
    bmp280->t3 <<= 8;
    bmp280->t3 |= bmp280ReadReg(i2c, BMP280_DIG_T3_LSB_REG); 
  
    /* read the pressure calibration parameters */
    bmp280->p1 = bmp280ReadReg(i2c, BMP280_DIG_P1_MSB_REG);
    bmp280->p1 <<= 8;
    bmp280->p1 |= bmp280ReadReg(i2c, BMP280_DIG_P1_LSB_REG);
    
    bmp280->p2 = bmp280ReadReg(i2c, BMP280_DIG_P2_MSB_REG);
    bmp280->p2 <<= 8;
    bmp280->p2 |= bmp280ReadReg(i2c, BMP280_DIG_P2_LSB_REG);
    
    bmp280->p3 = bmp280ReadReg(i2c, BMP280_DIG_P3_MSB_REG);
    bmp280->p3 <<= 8;
    bmp280->p3 |= bmp280ReadReg(i2c, BMP280_DIG_P3_LSB_REG);
    
    bmp280->p4 = bmp280ReadReg(i2c, BMP280_DIG_P4_MSB_REG);
    bmp280->p4 <<= 8;
    bmp280->p4 |= bmp280ReadReg(i2c, BMP280_DIG_P1_LSB_REG);
    
    bmp280->p5 = bmp280ReadReg(i2c, BMP280_DIG_P5_MSB_REG);
    bmp280->p5 <<= 8;
    bmp280->p5 |= bmp280ReadReg(i2c, BMP280_DIG_P5_LSB_REG);
    
    bmp280->p6 = bmp280ReadReg(i2c, BMP280_DIG_P6_MSB_REG);
    bmp280->p6 <<= 8;
    bmp280->p6 |= bmp280ReadReg(i2c, BMP280_DIG_P6_LSB_REG);
    
    bmp280->p7 = bmp280ReadReg(i2c, BMP280_DIG_P7_MSB_REG);
    bmp280->p7 <<= 8;
    bmp280->p7 |= bmp280ReadReg(i2c, BMP280_DIG_P7_LSB_REG);
    
    bmp280->p8 = bmp280ReadReg(i2c, BMP280_DIG_P8_MSB_REG);
    bmp280->p8 <<= 8;
    bmp280->p8 |= bmp280ReadReg(i2c, BMP280_DIG_P8_LSB_REG);
    
    bmp280->p9 = bmp280ReadReg(i2c, BMP280_DIG_P9_MSB_REG);
    bmp280->p9 <<= 8;
    bmp280->p9 |= bmp280ReadReg(i2c, BMP280_DIG_P9_LSB_REG);
    
    //reset bmp280
    bmp280Reset(i2c);  
  
    //set work mode
    uint8 ctrlmeas, config;   
    ctrlmeas = (bmp280->t_oversampling << 5) | (bmp280->p_oversampling << 2) | 
                bmp280->mode;  
    config = (bmp280->t_sb << 5) | (bmp280->filter_coefficient << 2);  
  
    bmp280WriteReg(i2c, BMP280_CTRLMEAS_REG, ctrlmeas);  
    bmp280WriteReg(i2c, BMP280_CONFIG_REG, config);
  
    return TRUE; 
}  

/**
 * @brief reset bmp280
 * @param bmp280 structure
 */
static void bmp280Reset(__in I2C *i2c)
{  
    bmp280WriteReg(i2c, BMP280_RESET_REG, BMP280_RESET_REG_VALUE);  
}  


/**
 * @brief set bmp280 work mode
 * @param bmp280 structure
 * @param bmp280 work mode
 */
void bmp280SetWorkMode(__in BMP280_T *bmp280, BMP280_WORK_MODE mode)
{  
    uint8 ctrlmeas;  
  
    bmp280->mode = mode;  
    ctrlmeas = (bmp280->t_oversampling << 5) | (bmp280->p_oversampling << 2) | 
                bmp280->mode;  
  
    bmp280WriteReg(bmp280->i2c, BMP280_CTRLMEAS_REG, ctrlmeas);  
}  
  
/**
 * @brief compensate bmp280 temperature value
 * @param bmp280 structure
 * @param temperature value
 */
static double bmp280CompensateTemperatureDouble(__in BMP280_T *bmp280, 
                                                 __in int32 temper)
{  
    double var1, var2;  
  
    var1 = (((double)temper) / 16384.0 - ((double)bmp280->t1) / 1024.0)  
            * ((double)bmp280->t2);  
    var2 = ((((double)temper) / 131072.0 - ((double)bmp280->t1) / 8192.0)  
            * (((double)temper) / 131072.0 - ((double)bmp280->t1) / 8192.0))  
            * ((double)bmp280->t3);  
    bmp280->t_fine = (int32)(var1 + var2);
  
    return (var1 + var2) / 5120.0;  
}  
  
/**
 * @brief compensate bmp280 pressure value
 * @param bmp280 structure
 * @param pressure value
 */
static double bmp280CompensatePressureDouble(__in BMP280_T *bmp280, 
                                               __in int32 value)
{  
    double var1, var2, pressure;  

    var1 = ((double)bmp280->t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)bmp280->p6) / 32768.0;
    var2 = var2 + var1 * ((double)bmp280->p5) * 2.0;
    var2 = (var2 / 4.0) + (((double)bmp280->p4) * 65536.0);
    var1 = (((double)bmp280->p3) * var1 * var1 / 524288.0
            + ((double)bmp280->p2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double)bmp280->p1);

    if (var1 == 0.0) 
        return 0;

    pressure = 1048576.0 - (double)value;
    pressure = (pressure - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double) bmp280->p9) * pressure * pressure / 2147483648.0;
    var2 = pressure * ((double) bmp280->p8) / 32768.0;
    pressure = pressure + (var1 + var2 + ((double) bmp280->p7)) / 16.0;

    return pressure;
}  

#if 0
/**
 * @brief compensate bmp280 temperature value
 * @param bmp280 structure
 * @param temperature value
 */
static int32 bmp280CompensateTemperatureInt32(__in BMP280_T *bmp280, 
                                               __in int32 temper)
{  
    int32 var1, var2;  
  
    var1 = ((((temper >> 3) - ((int32)bmp280->t1 << 1))) * 
            ((int32_t)bmp280->t2)) >> 11;
    var2 = (((((temper >> 4) - ((int32_t)bmp280->t1)) * 
              ((temper >> 4) - ((int32_t)bmp280->t1))) >> 12) * 
            ((int32_t)bmp280->t3)) >> 14;
    bmp280->t_fine = var1 + var2;
  
    return (bmp280->t_fine * 5 + 128) >> 8;  
}  

/**
 * @brief compensate bmp280 pressure value
 * @param bmp280 structure
 * @param pressure value
 */
static uint32 bmp280CompensatePressureInt64(__in BMP280_T *bmp280, 
                                              __in int32 value)
{  
    int64 var1, var2, pressure;  
  
    var1 = ((int64)bmp280->t_fine) - 128000;
    var2 = var1 * var1 * (int64)bmp280->p6;
    var2 = var2 + ((var1*(int64)bmp280->p5) << 17);
    var2 = var2 + (((int64)bmp280->p4) << 35);
    var1 = ((var1 * var1 * (int64)bmp280->p3) >> 8) + 
           ((var1 * (int64)bmp280->p2) << 12);
    var1 = (((((int64)1) << 47) + var1)) * ((int64)bmp280->p1) >> 33;
    if(var1 == 0) 
        return 0;
  
    pressure = 1048576 - value;
    pressure = (((pressure<<31)-var2)*3125)/var1;
    var1 = (((int64_t)bmp280->p9) * (pressure >> 13) * (pressure >> 13)) >> 25;
    var2 = (((int64_t)bmp280->p8) * pressure) >> 19;
    pressure = ((pressure + var1 + var2) >> 8) + (((int64_t)bmp280->p7) << 4);
  
    return (uint32)pressure;
}  
#endif
/**
 * @brief get bmp280 temperature value
 * @param bmp280 structure
 * @return temperature value
 */
static double bmp280GetTemperature(__in BMP280_T *bmp280)
{  
    uint32 lsb, msb, xlsb;
    int32 temper;
  
    xlsb = bmp280ReadReg(bmp280->i2c, BMP280_TEMPERATURE_XLSB_REG);
    lsb = bmp280ReadReg(bmp280->i2c, BMP280_TEMPERATURE_LSB_REG);
    msb = bmp280ReadReg(bmp280->i2c, BMP280_TEMPERATURE_MSB_REG);
  
    temper = (msb << 12) | (lsb << 4) | (xlsb >> 4);
    return bmp280CompensateTemperatureDouble(bmp280, temper);
}  

/**
 * @brief get bmp280 pressure value
 * @param bmp280 structure
 * @return pressure value
 */
static double bmp280GetPressure(__in BMP280_T *bmp280)
{  
    uint32 lsb, msb, xlsb;
    int32 pressure;
  
  
    xlsb = bmp280ReadReg(bmp280->i2c, BMP280_PRESSURE_XLSB_REG);
    lsb = bmp280ReadReg(bmp280->i2c, BMP280_PRESSURE_LSB_REG);
    msb = bmp280ReadReg(bmp280->i2c, BMP280_PRESSURE_MSB_REG);
  
    pressure = (msb << 12) | (lsb << 4) | (xlsb >> 4);

    return bmp280CompensatePressureDouble(bmp280, pressure);
}  
  
/**
 * @brief get bmp280 temperature and pressure value
 * @param bmp280 structure
 * @param temperature value
 * @param pressure value
 */
void bmp280GetTemperatureAndPressure(__in BMP280_T *bmp280, 
                                     __out double *temperature, 
                                     __out double *pressure)
{  
    *temperature = bmp280GetTemperature(bmp280);
    *pressure = bmp280GetPressure(bmp280);
}  
  


