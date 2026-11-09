#include <stdio.h>
#include "bme280.h"
#include "driver/i2c.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_PORT I2C_NUM_0
#define I2C_SDA GPIO_NUM_21
#define I2C_SCL GPIO_NUM_22
#define BME280_ADDR 0x76

/* These callbacks connect Bosch's driver to ESP-IDF's I2C driver. */
static int8_t i2c_read(uint8_t reg, uint8_t *data, uint32_t len, void *ptr)
{
    esp_err_t err = i2c_master_write_read_device(I2C_PORT, *(uint8_t *)ptr, &reg, 1, data, len, pdMS_TO_TICKS(100));
    return err == ESP_OK ? BME280_INTF_RET_SUCCESS : BME280_E_COMM_FAIL;
}
static int8_t i2c_write(uint8_t reg, const uint8_t *data, uint32_t len, void *ptr)
{
    uint8_t tx[16] = { reg };
    if (len + 1 > sizeof(tx)) return BME280_E_COMM_FAIL;
    for (uint32_t i = 0; i < len; i++) tx[i + 1] = data[i];
    esp_err_t err = i2c_master_write_to_device(I2C_PORT, *(uint8_t *)ptr, tx, len + 1, pdMS_TO_TICKS(100));
    return err == ESP_OK ? BME280_INTF_RET_SUCCESS : BME280_E_COMM_FAIL;
}
static void delay_us(uint32_t us, void *ptr)
{
    (void)ptr;
    esp_rom_delay_us(us);
}

static esp_err_t init_i2c(void)
{
    i2c_config_t cfg = { .mode = I2C_MODE_MASTER, .sda_io_num = I2C_SDA, .scl_io_num = I2C_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE, .scl_pullup_en = GPIO_PULLUP_ENABLE, .master.clk_speed = 100000 };
    ESP_RETURN_ON_ERROR(i2c_param_config(I2C_PORT, &cfg), "BME280", "I2C configuration failed");
    return i2c_driver_install(I2C_PORT, cfg.mode, 0, 0, 0);
}

void app_main(void)
{
    if (init_i2c() != ESP_OK) { printf("I2C initialization failed.\n"); return; }
    uint8_t address = BME280_ADDR, id = 0;
    struct bme280_dev sensor = { .intf = BME280_I2C_INTF, .intf_ptr = &address,
        .read = i2c_read, .write = i2c_write, .delay_us = delay_us };

    /* Verify register 0xD0 before loading the calibration coefficients. */
    if (i2c_read(BME280_REG_CHIP_ID, &id, 1, &address) != BME280_OK || id != 0x60) {
        printf("BME280 chip ID check failed (got 0x%02X, expected 0x60).\n", id); return;
    }
    if (bme280_init(&sensor) != BME280_OK) { printf("BME280 initialization failed.\n"); return; }

    sensor.settings.osr_t = BME280_OVERSAMPLING_2X;
    sensor.settings.osr_p = BME280_OVERSAMPLING_16X;
    sensor.settings.osr_h = BME280_OVERSAMPLING_1X;
    sensor.settings.filter = BME280_FILTER_COEFF_16;
    sensor.settings.standby_time = BME280_STANDBY_TIME_0_5_MS;
    sensor.settings.pwr_mode = BME280_NORMAL_MODE;
    if (bme280_set_sensor_settings(BME280_SEL_OSR_TEMP | BME280_SEL_OSR_PRESS | BME280_SEL_OSR_HUM |
            BME280_SEL_FILTER | BME280_SEL_STANDBY, &sensor) != BME280_OK ||
        bme280_set_sensor_mode(&sensor) != BME280_OK) { printf("BME280 configuration failed.\n"); return; }

    printf("BME280 initialized successfully!\n\n");
    while (true) {
        struct bme280_data data = { 0 };
        if (bme280_get_sensor_data(BME280_ALL, &data, &sensor) == BME280_OK) {
            printf("Temperature: %.2f °C\nHumidity: %.2f %%\nPressure: %.2f hPa\n\n----------------------------\n\n",
                   (double)data.temperature, (double)data.humidity, (double)(data.pressure / 100.0f));
        } else printf("BME280 measurement read failed.\n");
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
