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
#include"adapter.h"

#include"dai_link.h"

#include"adapter_prv.h"
#include"../dac/dac.h"


#include "fio.h"  

#define USE_I2S
#ifndef BY_PASS_ADAPTER
 
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
#define CUR_BUF_FULL 	1
#define CUR_BUF_NOT_FULL 			0

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
int32_t g_sampleConsumed;
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
 * \brief Write pcm data to audio output
 * \param[in]	buffer	pcm data buffer
 * \param[in]	bufLen  data length
 * \return		0
 */
int8_t audio_write(uint16_t * buffer, uint32_t pcmCnt)
{
	int32_t res = 0;
	int32_t i = 0;   
	static uint8_t s_writeReadyFlag = CUR_BUF_NOT_FULL;
	static int32_t s_sampleDelta = 0;
	
	int isAdapter = 0;
	
	 
	 
#if 0
	return audio_try_add_buffer((uint16_t*)buffer,bufLen);
#endif
	
#if 0 
	g_sampleProduced += bufLen/4;
	

	if(g_sampleConsumed >= 44100)
	{
		s_sampleDelta += g_sampleProduced - g_sampleConsumed;
		
#ifdef	AUDIO_OUTPUT_DEBUG
		
		printf("%d\n",s_sampleDelta);
		fflush(stdout);
#endif

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

		g_sampleProduced = 0;
		g_sampleConsumed = 0;
	}

	if(BLOCK_BUF_SIZE - g_blockBufferPos <= (int32_t)bufLen/4 )
	{
		for(i = 0; g_blockBufferPos < BLOCK_BUF_SIZE; g_blockBufferPos++,i += 4) g_blockBuffer[g_blockBufferPos] = *(int_32_ptr)(buffer + i);
		if(CUR_BUF_FULL == s_writeReadyFlag) 
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
	
		
	if (CUR_BUF_FULL == s_writeReadyFlag ) 
	{    	
		//0 succeed
		 res = audio_try_add_buffer(g_audioBuffer + g_curBufferNum * PINGPONGBUFSIZE, PINGPONGBUFSIZE);   
		 if (!res) {
			 s_writeReadyFlag = CUR_BUF_NOT_FULL;  	
			g_curBufferNum++;
			if (g_curBufferNum >= AUDIO_BUF_CNT)	g_curBufferNum = 0;
			g_audioBufferPos = 0;
		 }
		 else
		 {
				
		 }

	}
#endif
	
	 audio_try_add_buffer(buffer , pcmCnt);   
	
	
	return 0;
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
		 
		 
#ifndef AUIDO_PHY_SAI_CODEC
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
	
	
#ifndef AUIDO_PHY_SAI_CODEC
	g_audioBufferPos += len;
#else
		 g_audioBufferPos += 2*len;
#endif
		 
	if(g_audioBufferPos == PINGPONGBUFSIZE)
	{
		return CUR_BUF_FULL;
	}
	else
	{
		return CUR_BUF_NOT_FULL;
	}
}
/*
 * 
 * 
 */
static int32_t audio_try_add_buffer(uint16_t* samples, uint32_t  numSamples)
{

	return dai_link_write(samples, numSamples);
   
}
	
#endif
