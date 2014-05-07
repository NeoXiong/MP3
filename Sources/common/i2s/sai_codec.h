#ifndef _I2S_H
#define _I2S_H
#include<cstdint>


int8_t sai_codec_init(uint32_t pcmSampleRate, uint8_t bitWidth, uint8_t chNum);
void   sai_codec_deinit(void);
int32_t sai_codec_write(uint16_t *sample, uint16_t n);
int8_t sai_codec_flush();
int8_t sai_codec_ioctl(uint32_t cmd, uint32_t value);
int32_t sai_codec_set_format(uint32_t sampleRate, uint8_t bitWidth, uint8_t chNum);
int32_t sai_codec_vol_up(void);
int32_t sai_codec_vol_down(void);
int32_t sai_codec_mute(void);
int32_t sai_codec_umute(void);
#endif
