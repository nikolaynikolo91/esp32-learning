/* BME280 Sensor API interface. */
#ifndef BME280_H_
#define BME280_H_
#include <stdint.h>
#define BME280_OK 0
#define BME280_E_COMM_FAIL -2
#define BME280_E_DEV_NOT_FOUND -3
#define BME280_INTF_RET_SUCCESS 0
#define BME280_I2C_INTF 0
#define BME280_REG_CHIP_ID 0xD0
#define BME280_ALL 0x07
#define BME280_NORMAL_MODE 0x03
#define BME280_OVERSAMPLING_1X 1
#define BME280_OVERSAMPLING_2X 2
#define BME280_OVERSAMPLING_16X 5
#define BME280_FILTER_COEFF_16 4
#define BME280_STANDBY_TIME_0_5_MS 0
#define BME280_SEL_OSR_TEMP 1
#define BME280_SEL_OSR_PRESS 2
#define BME280_SEL_OSR_HUM 4
#define BME280_SEL_FILTER 8
#define BME280_SEL_STANDBY 16
struct bme280_settings { uint8_t osr_t, osr_p, osr_h, filter, standby_time, pwr_mode; };
struct bme280_calib_data { uint16_t t1,p1; int16_t t2,t3,p2,p3,p4,p5,p6,p7,p8,p9,h2,h4,h5; uint8_t h1,h3; int8_t h6; float t_fine; };
struct bme280_data { float temperature, pressure, humidity; };
struct bme280_dev { uint8_t intf; void *intf_ptr; int8_t (*read)(uint8_t,uint8_t *,uint32_t,void *); int8_t (*write)(uint8_t,const uint8_t *,uint32_t,void *); void (*delay_us)(uint32_t,void *); struct bme280_settings settings; struct bme280_calib_data calib_data; };
int8_t bme280_init(struct bme280_dev *dev);
int8_t bme280_set_sensor_settings(uint8_t settings_sel, struct bme280_dev *dev);
int8_t bme280_set_sensor_mode(struct bme280_dev *dev);
int8_t bme280_get_sensor_data(uint8_t sensor_comp, struct bme280_data *comp_data, struct bme280_dev *dev);
#endif
