#ifndef SDCARD_SPI_H
#define SDCARD_SPI_H

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"
#include "esp_err.h"

#define MAX_CHAR_SIZE    64

#define PIN_NUM_MISO    2
#define PIN_NUM_MOSI    15
#define PIN_NUM_CLK     14
#define PIN_NUM_CS      13

#define MOUNT_POINT "/sdcard"


void mount_sd_card(void);

void unmount_sd_card(void);

esp_err_t write_file_sd_card(const char *path, char *data);

esp_err_t read_file_sd_card(const char *path);


#endif //SDCARD_SPI_H