#ifndef _I2S_H
#define _I2S_H
#include<cstdint>


int8_t i2s_init(uint32_t pcmSampleRate, uint8_t bitWidth, uint8_t chNum);
void i2s_deinit(void);
int32_t i2s_write(uint16_t *sample, uint16_t n);
int8_t i2s_flush();
int8_t i2s_ioctl(uint32_t cmd, uint32_t value);
#endif
