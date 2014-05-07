#ifndef _MSI_
#define _MSI_

#define MSI_DEBUG


/*
 * mqx sound interface
 */

/*
 * default set is the sample rate 44100, 16 bits, 2 channel
 * umute
 */
int32_t msi_snd_init(void);

/*
 * write samples to device
 * must have correct set before write
 */
int32_t msi_snd_write(uint8_t * pcmStream, uint32_t pcmCnt);

/*
 * collect resources of sound device
 */
int32_t msi_snd_deinit(void);

/*
 * set pcm sound format: sample rate, bit width, channel number
 */
int32_t msi_snd_set_format(uint32_t sampleRate, uint8_t bitWidth, uint8_t chNum);

/*
 * mute sound device
 */
int32_t msi_snd_mute(void);
/*
 * umute device
 */
int32_t msi_snd_umute(void);

/*
 * turn up sound volume
 */
int32_t msi_snd_vol_up(void);

/*
 * turn down sound volume
 */
int32_t msi_snd_vol_down(void);
/*
 * flush the pcm stream out
 * 
 */
int32_t msi_snd_flush(void);

/*
 * get the buffer occupancy rate
 * return 0~100 represent 0%~ 100% 
 * 
 */
int16_t msi_snd_get_occ(void);
#endif
