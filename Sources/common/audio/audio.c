/*
 * audio_output.c
 * Version : 2.0
 * Created on: Jul 17, 2013
 * Author: B46407
 * Comments :
 * 
 * 		This file contains audio output operation
 * 		
 * 		including init / deinit audio and write PCM package
 *  
 */
 
#include<mqx.h>
#include"audio.h"

#include"../i2s/i2s.h"

#include"audio_prv.h"
#include"../dac/dac.h"

#include"codec.h"
#include "fio.h"  

 
/*--------------------------------------------------------------------------*/
/*                        CONSTANT DEFINITIONS                              */
/* Debug switch */
//#define AUDIO_OUTPUT_DEBUG
/*
 * Buffer count and size
 */
#define AUDIO_BUF_CNT    	3             
#define BLOCK_BUF_SIZE 	   (44*8)
/*
 * Current USB buffer status
 */
#define BUF_READY_WRITE 	1
#define BUF_FULL 			0

/*
 * MACROS
 */

static int32_t audio_try_add_buffer(uint16_t* samples, uint32_t  numSamples);
static int32_t write_block_to_buf(uint16_t * src, uint16_t * des, uint32_t len);

/*--------------------------------------------------------------------------*/
/*                        GLOBAL VARIABLES DEFINITIONS                      */

/*
 * This variable is define outside, reference here
 * To record samples count consumeb by DMA near every second
 */
extern int32_t g_sampleConsumed;
/*Use to record samples count produced by USB near every second */
static int32_t 	g_sampleProduced = 0;
/*
 * This is the buffer used for audio
 * two of which are occupied by DMA and the other by USB.
 * This three buffer are polling by USB and DMA
 * 
 * 		DMA use 1		DMA use2		USB use
 * 		--------			---------		---------
 * T1	-	----			---------		---		-
 *		--------			---------		---------
 *		
 *	     USB use            DMA use 1		DMA use2		
 * 		--------			---------		---------
 * T2	---	   -			-	-----		---------
 *		--------			---------		---------
 *				3 buffers' polling
 */
static uint16_t g_audioBuffer[PINGPONGBUFSIZE * AUDIO_BUF_CNT] = {0};
/*
 * A 16-sample aligned block used for secondary buffering
 */
static uint32_t g_blockBuffer[BLOCK_BUF_SIZE] = {0};
/*
 * Block buffer current position
 */
static int32_t  g_blockBufferPos = 0;
/*
 * USB buffer current position
 */
static uint32_t g_audioBufferPos = 0;
/*
 * Current USB buffer's number, obtained by polling
 */
static uint32_t g_curBufferNum = 0;

/*
 * \brief Init DAC and DAC_DMA as well as PDB
 * \param[in]	pcmSampleRate	the DAC output rate
 * \return		0 success, nagative failure
 */
int8_t audio_init(uint32_t pcmSampleRate, uint8_t bitWidth, uint8_t chNum)
{

	
#ifdef USE_I2S
	printf("Audio device init \n");
	return i2s_init(pcmSampleRate, bitWidth, chNum);
#else
	dac_init();
#endif

	return 0;
}

/*
 * \brief Reset DAC and PDB
 * \return		0
 */
int8_t audio_deinit(void)
{
#ifdef USE_I2S
#if 1
	while(audio_flush())	
	{
		printf("Audio flush now\n");
	}
#endif
	i2s_deinit();
	printf("Audio device closed\n");
#else
	DACC_RESET();
#endif
	g_audioBufferPos = 0;
	g_curBufferNum = 0;


	g_sampleConsumed = 0;
	g_sampleProduced = 0;
	
	return 0;
}


/*
 * \brief Write pcm data to audio output
 * \param[in]	buffer	pcm data buffer
 * \param[in]	bufLen  data length
 * \return		0
 */
int8_t audio_write(uint8_t * buffer, uint32_t bufLen)
{
	int32_t res = 0;
	int32_t i = 0;   
	static uint8_t s_writeReadyFlag = BUF_FULL;
	static int32_t s_sampleDelta = 0;
	
	
	 
	     
#ifdef USE_I2S
	return audio_try_add_buffer((uint16_t*)buffer,bufLen);
#endif
	g_sampleProduced += bufLen/4;

	if(g_sampleConsumed >= 44100)
	{
		
#ifdef	AUDIO_OUTPUT_DEBUG
		s_sampleDelta += g_sampleProduced - g_sampleConsumed;
		printf("%d\n",s_sampleDelta);
		fflush(stdout);
#endif
#ifndef USE_I2S
		if(s_sampleDelta < PINGPONGBUFSIZE/2*3 && PDB0_DACINT0 != PDB_DACINT_UP)
		{
			
			PDB0_DACINT0 =  PDB_DACINT_UP; 
			PDB0_MOD = PDB0_DACINT0;
			PDB0_SC |= PDB_SC_LDOK_MASK; 
		}
		else if(s_sampleDelta > PINGPONGBUFSIZE*2 &&  PDB0_DACINT0 != PDB_DACINT_DOWN)
		{
			PDB0_DACINT0 =  PDB_DACINT_DOWN;
			PDB0_MOD = PDB0_DACINT0;
			PDB0_SC |= PDB_SC_LDOK_MASK; 
		}
#endif
		g_sampleProduced = 0;
		g_sampleConsumed = 0;
	}

	if(BLOCK_BUF_SIZE - g_blockBufferPos <= (int32_t)bufLen/4 )
	{
		for(i = 0; g_blockBufferPos < BLOCK_BUF_SIZE; g_blockBufferPos++,i += 4) g_blockBuffer[g_blockBufferPos] = *(int_32_ptr)(buffer + i);
		if(BUF_READY_WRITE == s_writeReadyFlag) 
		{
			#ifdef AUDIO_OUTPUT_DEBUG
			light_led(0x08);
			#endif
		}
		else
		{
			#ifdef	AUDIO_OUTPUT_DEBUG
			light_down_led(0x08);
			#endif
			s_writeReadyFlag = write_block_to_buf((uint16_t *)g_blockBuffer, g_audioBuffer + g_curBufferNum * PINGPONGBUFSIZE + g_audioBufferPos, BLOCK_BUF_SIZE);		 
			for(g_blockBufferPos = 0; i < bufLen ; g_blockBufferPos++,i += 4) g_blockBuffer[g_blockBufferPos] = *(int_32_ptr)(buffer + i);
		}
	}
	else
	{
		for(i = 0; i < bufLen; i += 4) g_blockBuffer[g_blockBufferPos + i/4] = *(uint_32_ptr)(buffer + i);
		g_blockBufferPos += bufLen/4;
	}
	
		
	if (BUF_READY_WRITE == s_writeReadyFlag ) 
	{    	
		//0 failed
		 res = audio_try_add_buffer(g_audioBuffer + g_curBufferNum * PINGPONGBUFSIZE, PINGPONGBUFSIZE);   
		 if (!res) {
			 
		 }
		 else
		 {
				s_writeReadyFlag = BUF_FULL;  
				
				g_curBufferNum++;
				if (g_curBufferNum >= AUDIO_BUF_CNT)	g_curBufferNum = 0;
				g_audioBufferPos = 0;
		 }

	}
	return 0;
}
/*
 * \brief set the SAI sample rate, bit width and channel number
 * param[in] cmd is the enum type, one of setSamplerate, setBitWidth and setChNum
 * param[in] value the desired value to be set
 * return 0 success, negative failure
 */
int8_t audio_ioctl(audio_cmd_t cmd, uint32_t value)
{
	switch(cmd)
	{
	case setSamplerate:
	case setBitWidth:
	case setChNum:
		
	case volUp:
	case volDown:
		return i2s_ioctl(cmd, value);
	
		break;
	default:
		return -128;
		break;
	}
	
}
int32_t audio_vol_up(void)
{
	return codec_vol_up();
}

int32_t audio_vol_down(void)
{
	return codec_vol_down();
}
int_8 audio_flush(void)
{
	g_blockBufferPos = 0;
	g_audioBufferPos = 0;
	g_curBufferNum = 0;
	printf("@@flush audio\n");
	return i2s_flush();
}
/*
 * \brief Write the block buffer to current USB buffer when 
 * 			block is full
 * \param[in]	src
 * \param[in]	des
 * \param[in]	len
 * \return		1(current USB buffer is full)
 * \return		0
 */

static int32_t write_block_to_buf(uint16_t * src, uint16_t * des, uint32_t len)
{
	int32_t i = 0, temp = 0;
	
	for(i = 0; i < 2*len; i++)
	{
		 temp =  (int_16)src[i];
		 
		 
#ifndef USE_I2S
		 temp += 32768;
		 temp *= 255;//get_volume()
		 temp = temp >> (5 + 8);
		 temp += 2048;
		 temp &= 0xfff;
		 des[i/2] = temp;
#else
		 des[i] = temp;
#endif
	}
	
	
#ifndef USE_I2S
	g_audioBufferPos += len;
#else
		 g_audioBufferPos += 2*len;
#endif
		 
	if(g_audioBufferPos == PINGPONGBUFSIZE)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
/*
 * 
 * 
 */
static int32_t audio_try_add_buffer(uint16_t* samples, uint32_t  numSamples)
{
#ifdef USE_I2S
	return i2s_write(samples, numSamples);
#else 
    return dac_write(samples, numSamples);
#endif
   
}
	
