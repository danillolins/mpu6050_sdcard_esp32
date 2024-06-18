#include "sdcard_spi.h"

static const char *TAG_SDCARD = "SD_CARD";

const char mount_point[] = MOUNT_POINT;
sdmmc_card_t *card;
sdmmc_host_t host = SDSPI_HOST_DEFAULT();

void mount_sd_card(void)
{
    esp_err_t ret;


    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };



    ESP_LOGI(TAG_SDCARD, "Initializing SD card");

    ESP_LOGI(TAG_SDCARD, "Using SPI peripheral");



    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG_SDCARD, "Failed to initialize bus.");
        return;
    }

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    ESP_LOGI(TAG_SDCARD, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG_SDCARD, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG_SDCARD, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return;
    }
    ESP_LOGI(TAG_SDCARD, "Filesystem mounted");

    sdmmc_card_print_info(stdout, card);

}

void unmount_sd_card(void)
{
        // All done, unmount partition and disable SPI peripheral
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    ESP_LOGI(TAG_SDCARD, "Card unmounted");

    //deinitialize the bus after all devices are removed
    spi_bus_free(host.slot);

}

esp_err_t write_file_sd_card(const char *path, char *data)
{
    //ESP_LOGI(TAG_SDCARD, "Opening file %s", path);
    FILE *f = fopen(path, "a");
    if (f == NULL) {
        ESP_LOGE(TAG_SDCARD, "Failed to open file for writing");
        return ESP_FAIL;
    }
    fprintf(f, data);
    fclose(f);
    //ESP_LOGI(TAG_SDCARD, "File written");

    return ESP_OK;
}

esp_err_t read_file_sd_card(const char *path)
{
    ESP_LOGI(TAG_SDCARD, "Reading file %s", path);
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG_SDCARD, "Failed to open file for reading");
        return ESP_FAIL;
    }
    char line[MAX_CHAR_SIZE];
    fgets(line, sizeof(line), f);
    fclose(f);

    // strip newline
    char *pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    ESP_LOGI(TAG_SDCARD, "Read from file: '%s'", line);

    return ESP_OK;
}
