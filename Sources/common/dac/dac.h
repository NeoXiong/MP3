
#ifndef __DAC_H__
#define __DAC_H__
#include<cstdint>

#include"../audio/adapter_prv.h"
/*
 * Macros to change PDB value
 */
#if (MQX_CPU == PSP_CPU_MK70F120M)  

#define PDB_DACINT_UP   1360
#define PDB_DACINT_DOWN 1359
#else
#if (MQX_CPU == PSP_CPU_MK60D100M)
#define PDB_DACINT_UP   1088
#define PDB_DACINT_DOWN 1087
#else
#error "Can not recognize chip type"
#endif
#endif

#define PDB_INIT_VAL PDB_DACINT_DOWN


int8_t dac_init(/*uint32_t pcmSampleRate, uint8_t bitWidth, uint8_t chNum*/);
void dac_deinit(void);
int8_t dac_flush();
int8_t dac_ioctl(uint32_t cmd, uint32_t value);
int32_t dac_write( uint16_t* samples, uint16_t numSamples);	//0 if failed, positive number of existing queued samples
int32_t dac_set_format(uint32_t sampleRate, uint8_t bitWidth, uint8_t chNum);
int32_t dac_vol_up(void);
int32_t dac_vol_down(void);
int32_t dac_mute(void);
int32_t dac_umute(void);
#endif  // __DAC_H__


