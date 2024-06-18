/*------------------------------------------------------------------------------

    MP6050 accelerometer and gyroscope sensor driver for ESP32 IDF
    May 2024: Danillo Lins

---------------------------------------------------------------------------------*/

#ifndef MPU6050_I2C_H
#define MPU6050_I2C_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "esp_err.h"

#define I2C_MASTER_SCL_IO           19                         /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           18                         /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#define MPU6050_SENSOR_ADDR         0x68                        /*!< Slave address of the MPU6050 sensor */
#define MPU6050_WHO_AM_I_REG_ADDR   0x75                        /*!< Register addresses of the "who am I" register */
#define MPU6050_DATA_ADDR           0x3B                        /*!< Register addresses of the DATA */

#define MPU6050_TAG                 "MPU6050"


//Acelerometer Constants
#define ACEL_2G                     16384
#define ACEL_4G                     8192
#define ACEL_8G                     4096
#define ACEL_16G                    2048

//Gyroscope Constants

#define GYR_250                     131
#define GYR_500                     65.6
#define GYR_1000                    32.8
#define GYR_2000                    16.4

// Accelerometer configuration
/*
    0x00 //  +/-2g
    0x08 //  +/-4g
    0x10 //  +/-8g
    0x18 //  +/-16g
*/

#define CONFIG_ACEL_2G                     0x00
#define CONFIG_ACEL_4G                     0x08
#define CONFIG_ACEL_8G                     0x10
#define CONFIG_ACEL_16G                    0x18

    // Gyroscope configuration
  /*
    0x00 //  +/-250°/s
    0x08 //  +/-500°/s
    0x10 //  +/-1000°/s
    0x18 //  +/-2000°/s
  */

#define CONFIG_GYR_250                     0x00
#define CONFIG_GYR_500                     0x08
#define CONFIG_GYR_1000                    0x10
#define CONFIG_GYR_2000                    0x18


typedef struct 
{
    int16_t AccX; 
    int16_t AccY; 
    int16_t AccZ; 
    int16_t Temp; 
    int16_t GyrX; 
    int16_t GyrY; 
    int16_t GyrZ; 
}mpu6050_data_t;



esp_err_t mpu6050_register_read(uint8_t reg_addr, uint8_t *data, size_t len);

esp_err_t mpu6050_raw_data_read(mpu6050_data_t *data);

esp_err_t mpu6050_register_write_byte(uint8_t reg_addr, uint8_t data);

esp_err_t i2c_mpu6050_init(void);

esp_err_t mpu6050_define_acc_resolution(uint8_t resolution);

esp_err_t mpu6050_define_gyr_resolution(uint8_t resolution);

esp_err_t mpu6050_init_read_sensor(void);

esp_err_t mpu6050_test_communication(void);


#endif /*MPU6050_I2C_H*/