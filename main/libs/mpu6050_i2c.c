/*------------------------------------------------------------------------------

    MP6050 accelerometer and gyroscope sensor driver for ESP32 IDF
    May 2024: Danillo Lins

---------------------------------------------------------------------------------*/
#include "mpu6050_i2c.h"


static const char *TAG_MP6050 = MPU6050_TAG;

esp_err_t mpu6050_register_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_SENSOR_ADDR, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

esp_err_t mpu6050_raw_data_read(mpu6050_data_t *data)
{
    
    uint8_t sensor_data[14];

    esp_err_t err = mpu6050_register_read(MPU6050_DATA_ADDR, sensor_data, 14);

    data->AccX = (int16_t)(sensor_data[0] << 8 | sensor_data[1]);
    data->AccY = (int16_t)(sensor_data[2] << 8 | sensor_data[3]);
    data->AccZ = (int16_t)(sensor_data[4] << 8 | sensor_data[5]);
    data->Temp = (int16_t)(sensor_data[6] << 8 | sensor_data[7]);
    data->GyrX = (int16_t)(sensor_data[8] << 8 | sensor_data[9]);
    data->GyrY = (int16_t)(sensor_data[10] << 8 | sensor_data[11]);
    data->GyrZ = (int16_t)(sensor_data[12] << 8 | sensor_data[13]);

    return err;
}

esp_err_t mpu6050_register_write_byte(uint8_t reg_addr, uint8_t data)
{
    int ret;
    uint8_t write_buf[2] = {reg_addr, data};

    ret = i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_SENSOR_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

    return ret;
}

esp_err_t i2c_mpu6050_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}


esp_err_t mpu6050_define_acc_resolution(uint8_t resolution)
{
    return mpu6050_register_write_byte(0x1C, resolution);
}

esp_err_t mpu6050_define_gyr_resolution(uint8_t resolution)
{
    return mpu6050_register_write_byte(0x1B, resolution);
}


esp_err_t mpu6050_init_read_sensor(void)
{
    return mpu6050_register_write_byte(0x6B, 0x00);
}



esp_err_t mpu6050_test_communication(void)
{
    uint8_t data[2];
    int err = mpu6050_register_read(MPU6050_WHO_AM_I_REG_ADDR, data, 1);
    ESP_LOGI(TAG_MP6050, "WHO_AM_I = %X", data[0]);  
    return err;
}