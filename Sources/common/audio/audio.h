/*
 * audio_output.h
 *
 *  Created on: Jul 17, 2013
 *      Author: B46407
 */

#ifndef AUDIO_OUTPUT_H_
#define AUDIO_OUTPUT_H_

#include<cstdint>

//#define USE_I2S

#ifndef  USE_I2S
void DACC_RESET(void);
#endif

#define AUDIO_SET_SAMPLERATE	0x01
#define AUDIO_SET_BITWIDTH		0x02
#define AUDIO_SET_CH_NUM		0x04
typedef enum _audio_cmd
{
	setSamplerate = 1,
	setBitWidth	= 2,
	setChNum = 3,
	/*add volUp volDown*/
	volUp ,
	volDown
	
}audio_cmd_t;
/*
 * \brief Init DAC and DAC_DMA as well as PDB
 * \param[in]	pcmSampleRate	the DAC output rate
 * 				that is the sample rate of the music
 * \param[in]	bitWidth sample bit width, such as 8,16,24..
 * \param[in]	chNum channel number, 1 is single and 2 is stereo
 * \return		0 success, failure negative 
 */
int8_t audio_init(uint32_t pcmSampleRate, uint8_t bitWidth, uint8_t chNum);
/*
 * \brief Reset DAC and PDB
 * \return		0
 */
int8_t audio_deinit(void);
/*
 * \brief Write pcm data to audio output
 * \param[in]	buffer	pcm data buffer
 * \param[in]	bufLen  data length,unit 32-bit
 * \return		0
 */
int8_t audio_write(uint8_t * buffer, uint32_t bufLen);
/*
 * \brief Flush the audio buffer
 * reutrn 0 success, negative failure
 */
int_8 audio_flush(void);

/*
 * \brief config the sample rate, bitwidth and channel of the PCM
 * added the volUp and VolDown, at the point, the value is ignored
 * the volume will change by fixed step 
 */
int8_t audio_ioctl(audio_cmd_t cmd, uint32_t value);

int32_t audio_vol_up(void);

int32_t audio_vol_down(void);
#endif /* AUDIO_OUTPUT_H_ */
