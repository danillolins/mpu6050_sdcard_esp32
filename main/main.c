#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "libs/sdcard_spi.h"
#include "libs/mpu6050_i2c.h"
#include "driver/gpio.h"
#include "driver/timer.h"


#define TAG                 "TEST"
#define BTN                 27

#define TIMER_DIVIDER         (16)  //  Hardware timer clock divider
#define TIMER_SCALE           (80000000 / TIMER_DIVIDER)  // convert counter value to seconds


static void inline print_timer_counter(uint64_t counter_value)
{
    printf("Time   : %.8f s\r\n", (double) counter_value / TIMER_SCALE);
}

void app_main(void)
{

    timer_config_t config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = false,
    }; // default clock source is APB
    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    


    //gpio_pad_select_gpio(BTN);
    gpio_set_direction(BTN, GPIO_MODE_INPUT);

    float data_acc_x;
    float data_acc_y;
    float data_acc_z;

    mpu6050_data_t data_output;

    ESP_ERROR_CHECK(i2c_mpu6050_init());
    ESP_LOGI(MPU6050_TAG, "I2C initialized successfully");

    // Start the sensor

    ESP_ERROR_CHECK(mpu6050_init_read_sensor());
    ESP_LOGI(MPU6050_TAG, "INITIALIZING SENSOR READING");


    ESP_ERROR_CHECK(mpu6050_define_acc_resolution(CONFIG_ACEL_2G));
    ESP_LOGI(MPU6050_TAG, "ACCELEROMETER CONFIGURED");

    ESP_ERROR_CHECK(mpu6050_define_gyr_resolution(CONFIG_GYR_250));
    ESP_LOGI(MPU6050_TAG, "GYROSCOPE CONFIGURED");

    ESP_ERROR_CHECK(mpu6050_test_communication());
    
    
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "Start Reading BTN");

    while(!gpio_get_level(BTN))
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    ESP_LOGI(TAG, "Start Reading");

    mount_sd_card();

    esp_err_t ret;

    const char *file_data = MOUNT_POINT"/accdata3.txt";
    char data[MAX_CHAR_SIZE];

    snprintf(data, MAX_CHAR_SIZE, "sensor;accX;accY;accZ\n");
    ret = write_file_sd_card(file_data, data);
    if (ret != ESP_OK) {
        return;
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    uint64_t task_counter_value;

    for(int i = 0; i < 10 ; i++)
    {
        timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
        timer_start(TIMER_GROUP_0, TIMER_0);

        ESP_ERROR_CHECK(mpu6050_raw_data_read(&data_output));

        // data_acc_x = ((float)data_output.AccX/ACEL_2G);
        // data_acc_y = ((float)data_output.AccY/ACEL_2G);
        // data_acc_z = ((float)data_output.AccZ/ACEL_2G);

        // ESP_LOGI(MPU6050_TAG, "Acc_Axis X = %.2f    Acc_AxisY = %.2f    Acc_AxisZ = %.2f", 
        //                        data_acc_x, data_acc_y, data_acc_z);

        // snprintf(data, MAX_CHAR_SIZE, "s1;%.2f;%.2f;%.2f\n", data_acc_x, data_acc_y, data_acc_z);
        snprintf(data, MAX_CHAR_SIZE, "%d;%d;%d\n", data_output.AccX, data_output.AccY, data_output.AccZ);

        ret = write_file_sd_card(file_data, data);
        if (ret != ESP_OK) {
            return;
        }
        
        timer_pause(TIMER_GROUP_0, TIMER_0);
        timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &task_counter_value);
        print_timer_counter(task_counter_value);

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    
    ESP_LOGI(TAG, "Reading finished");

    // unmount_sd_card();

}