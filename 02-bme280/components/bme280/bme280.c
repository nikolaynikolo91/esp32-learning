/* Compact port of the Bosch Sensortec BME280 compensation algorithm. */
#include "bme280.h"

#define R_CALIB00 0x88
#define R_CALIB26 0xE1
#define R_CTRL_HUM 0xF2
#define R_STATUS 0xF3
#define R_CTRL_MEAS 0xF4
#define R_CONFIG 0xF5
#define R_DATA 0xF7
#define R_RESET 0xE0

static uint16_t u16(const uint8_t *b) { return (uint16_t)b[0] | ((uint16_t)b[1] << 8); }
static int16_t s16(const uint8_t *b) { return (int16_t)u16(b); }
static int8_t read_reg(struct bme280_dev *d, uint8_t r, uint8_t *v, uint32_t n) { return d->read(r, v, n, d->intf_ptr); }
static int8_t write_reg(struct bme280_dev *d, uint8_t r, uint8_t v) { return d->write(r, &v, 1, d->intf_ptr); }

int8_t bme280_init(struct bme280_dev *d)
{
    uint8_t a[26], b[7], id;
    if (read_reg(d, BME280_REG_CHIP_ID, &id, 1) || id != 0x60) return BME280_E_DEV_NOT_FOUND;
    if (write_reg(d, R_RESET, 0xB6)) return BME280_E_COMM_FAIL;
    d->delay_us(3000, d->intf_ptr);
    if (read_reg(d, R_CALIB00, a, sizeof(a)) || read_reg(d, R_CALIB26, b, sizeof(b))) return BME280_E_COMM_FAIL;
    struct bme280_calib_data *c = &d->calib_data;
    c->t1=u16(a); c->t2=s16(a+2); c->t3=s16(a+4); c->p1=u16(a+6); c->p2=s16(a+8); c->p3=s16(a+10);
    c->p4=s16(a+12); c->p5=s16(a+14); c->p6=s16(a+16); c->p7=s16(a+18); c->p8=s16(a+20); c->p9=s16(a+22); c->h1=a[25];
    c->h2=s16(b); c->h3=b[2]; c->h4=(int16_t)((b[3] << 4) | (b[4] & 0x0F)); if (c->h4 & 0x0800) c->h4 |= 0xF000;
    c->h5=(int16_t)((b[5] << 4) | (b[4] >> 4)); if (c->h5 & 0x0800) c->h5 |= 0xF000; c->h6=(int8_t)b[6];
    return BME280_OK;
}
int8_t bme280_set_sensor_settings(uint8_t s, struct bme280_dev *d)
{
    uint8_t v;
    if (s & BME280_SEL_OSR_HUM) if (write_reg(d, R_CTRL_HUM, d->settings.osr_h)) return BME280_E_COMM_FAIL;
    if (s & (BME280_SEL_FILTER | BME280_SEL_STANDBY)) { v=(d->settings.standby_time<<5)|(d->settings.filter<<2); if (write_reg(d,R_CONFIG,v)) return BME280_E_COMM_FAIL; }
    if (s & (BME280_SEL_OSR_TEMP | BME280_SEL_OSR_PRESS)) { v=(d->settings.osr_t<<5)|(d->settings.osr_p<<2); if (write_reg(d,R_CTRL_MEAS,v)) return BME280_E_COMM_FAIL; }
    return BME280_OK;
}
int8_t bme280_set_sensor_mode(struct bme280_dev *d)
{ return write_reg(d, R_CTRL_MEAS, (d->settings.osr_t<<5)|(d->settings.osr_p<<2)|d->settings.pwr_mode); }

int8_t bme280_get_sensor_data(uint8_t all, struct bme280_data *out, struct bme280_dev *d)
{
    (void)all; uint8_t b[8]; struct bme280_calib_data *c=&d->calib_data;
    if (read_reg(d,R_DATA,b,8)) return BME280_E_COMM_FAIL;
    int32_t adc_p=((int32_t)b[0]<<12)|((int32_t)b[1]<<4)|(b[2]>>4), adc_t=((int32_t)b[3]<<12)|((int32_t)b[4]<<4)|(b[5]>>4), adc_h=((int32_t)b[6]<<8)|b[7];
    float v1=(((float)adc_t)/16384.0f-((float)c->t1)/1024.0f)*(float)c->t2;
    float v2=((((float)adc_t)/131072.0f-((float)c->t1)/8192.0f)*(((float)adc_t)/131072.0f-((float)c->t1)/8192.0f))*(float)c->t3;
    c->t_fine=v1+v2; out->temperature=c->t_fine/5120.0f;
    float v1p=c->t_fine/2.0f-64000.0f;
    float v2p=v1p*v1p*(float)c->p6/32768.0f + v1p*(float)c->p5*2.0f;
    v2p=v2p/4.0f+(float)c->p4*65536.0f;
    v1p=(((float)c->p3*v1p*v1p/524288.0f)+(float)c->p2*v1p)/524288.0f;
    v1p=(1.0f+v1p/32768.0f)*(float)c->p1;
    if (v1p == 0.0f) return BME280_E_COMM_FAIL;
    float p=1048576.0f-(float)adc_p;
    p=(p-v2p/4096.0f)*6250.0f/v1p;
    v1p=(float)c->p9*p*p/2147483648.0f;
    v2p=p*(float)c->p8/32768.0f;
    out->pressure=p+(v1p+v2p+(float)c->p7)/16.0f;
    float h=c->t_fine-76800.0f; h=((float)adc_h-((float)c->h4*64.0f+(float)c->h5/16384.0f*h))*((float)c->h2/65536.0f*(1.0f+(float)c->h6/67108864.0f*h*(1.0f+(float)c->h3/67108864.0f*h)));
    h=h*(1.0f-(float)c->h1*h/524288.0f); out->humidity=h<0?0:(h>100?100:h); return BME280_OK;
}
