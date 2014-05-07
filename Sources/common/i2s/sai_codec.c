/**HEADER********************************************************************
 *
 * Copyright (c) 2013 Freescale Semiconductor;
 * All Rights Reserved
 *
 ***************************************************************************
 *
 * THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************
 *
 * $FileName: sd_player_task.c$
 * $Version : 3.8.3.0$
 * $Date    : Sep-12-2012$
 *
 *END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include"dai_link.h"
#include"../HMI/hmi.h"
#include"./Sai_dma/dma/edma.h"
#include"./Sai_dma/sai/sai_dma.h"
#include"./Sai_dma/sai/init_sai_dma.h"
 
#include "../common/hwcodec/codec.h"

#define SAI_CODEC_DEBUG
#undef SAI_CODEC_DEBUG

#ifdef AUIDO_PHY_SAI_CODEC

#ifndef CLK_MULT
#define CLK_MULT 384
#endif

#define AUDIO_DIVECE_NAME_STR	"sai0:"
#define INIT_SAMPLE_RATE 44100u
#define INIT_BIT_WIDTH  16u
#define INIT_CH_NUM  2u

struct Snd_Format {
	uint32_t sampleRate;
	uint8_t  bitWidth;
	uint8_t ch;
}g_snd_format = {INIT_SAMPLE_RATE, INIT_BIT_WIDTH, INIT_CH_NUM};


static FILE_PTR g_sai_device_ptr = NULL;
static AUDIO_DATA_FORMAT_DMA s_audio_format;

/*
 * open sai and set master mode
 * default init parameter is present in init_sai_dma.h _bsp_sai_dma_init
 */
static int32_t saiMasterInit(void)
{
	uint_8 mode = (I2S_TX_MASTER | I2S_RX_SLAVE);
	uint_32 fs_freq = INIT_SAMPLE_RATE, bit_width = INIT_BIT_WIDTH,  mclk_freq = fs_freq * CLK_MULT;
		
	if(g_sai_device_ptr == NULL)
	{
		/*
		 * open sai
		 */
		if ((g_sai_device_ptr = fopen(AUDIO_DIVECE_NAME_STR, "w")) == NULL) 
		{
			#ifdef SAI_CODEC_DEBUG
			printf("In file %s line %d  function :%s \n", __FILE__  , __LINE__, __FUNCTION__);
			printf("  Error: Unable to open the device \"%s\".\n",AUDIO_DIVECE_NAME_STR);
			#endif
			return -1;
		}
#if 0
		/*
		 * has inited in fopen sai
		 */
		// Setup audio data format in device 
		if (ioctl(g_sai_device_ptr, IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT, &s_audio_format) != I2S_OK) {
			#ifdef SAI_CODEC_DEBUG
				printf("In file %s line %d  function :%s \n", __FILE__  , __LINE__, __FUNCTION__);
				printf("  Init SAI data format failed.\n");
			#endif
				return -1;
				
			}
#endif
		/*
		 * set tx master and rx slave
		 */
		if(ioctl(g_sai_device_ptr, IO_IOCTL_I2S_SET_MODE_MASTER, &mode) != 0)
		{
			#ifdef SAI_CODEC_DEBUG
			printf("In file %s line %d  function :%s \n", __FILE__  , __LINE__, __FUNCTION__);
			printf("  Error: failed to set mode of  \"%s\".\n",AUDIO_DIVECE_NAME_STR);
			#endif
			return -1;
		}
		/*
		 * default set in driver is 48k*16*2  mclk = 8*bclk
		 */
		// Setup rest of parameters - master clock, valid data bits and sampling frequency 
		if ((ioctl(g_sai_device_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &mclk_freq) != I2S_OK)
				|| (ioctl(g_sai_device_ptr, IO_IOCTL_I2S_SET_DATA_BITS,
						&bit_width) != I2S_OK)
				|| (ioctl(g_sai_device_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &fs_freq)
						!= I2S_OK))
		{
		#ifdef SAI_CODEC_DEBUG
			printf("In file %s line %d  function :%s \n", __FILE__  , __LINE__, __FUNCTION__);
			printf("  i2s_init Error: Unable to setup \"%s\" device driver.\n",AUDIO_DIVECE_NAME_STR);
		#endif
			 return -1;
		}
	     
	}
	else
	{
	#ifdef SAI_CODEC_DEBUG
		printf("In file %s line %d  function :%s \n", __FILE__  , __LINE__, __FUNCTION__);
		printf("Can not re-init sai resource\n");
	#endif
	}
	
	return 0;
}

/*
 * set sai fs bit width channel
 */
static int saiSetFormat(uint32_t fs, uint8_t bitWidth, uint8_t ch)
{
	uint32_t mclk_freq = fs*CLK_MULT;
	g_snd_format.sampleRate = fs;
	g_snd_format.bitWidth = bitWidth;
	g_snd_format.ch = ch;
	
	if ((ioctl(g_sai_device_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &mclk_freq) != I2S_OK)
					|| (ioctl(g_sai_device_ptr, IO_IOCTL_I2S_SET_DATA_BITS,
							&g_snd_format.bitWidth) != I2S_OK)
					|| (ioctl(g_sai_device_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &g_snd_format.sampleRate)
							!= I2S_OK))
	{
	#ifdef SAI_CODEC_DEBUG
		printf("In file %s line %d  function :%s \n", __FILE__  , __LINE__, __FUNCTION__);
		printf("  sai set format Error.\n",AUDIO_DIVECE_NAME_STR);
	#endif
		 return -1;
	}
	
	if (ioctl(g_sai_device_ptr, IO_IOCTL_I2S_SET_CHANNEL, &ch) != I2S_OK)
	{
	#ifdef SAI_CODEC_DEBUG
		printf("In file %s line %d  function :%s \n", __FILE__  , __LINE__, __FUNCTION__);
		printf("  sai set channel Error.\n",AUDIO_DIVECE_NAME_STR);
	#endif
		 return -1;
	}
	
	return 0;
}



static LWSEM_STRUCT hwcodec_sem;
int8_t sai_codec_init(uint32_t pcmSampleRate, uint8_t bitWidth, uint8_t chNum) {
	_mqx_int return_code;
	
	uint_32 mclk_freq, fs_freq, bit_width = 0;
	I2S_STATISTICS_STRUCT_DMA stats;
	static int8_t ctrl = 0;
#if 0
	g_snd_format.sampleRate = pcmSampleRate;
	g_snd_format.bitWidth = bitWidth;
	g_snd_format.ch = chNum;
#endif	
	 
	if(ctrl == 0)
	{
		dma_init();
		_io_sai_dma_int_install(&_bsp_sai_dma_init);
		ctrl = 1;
		
		if (MQX_OK != _lwsem_create(&hwcodec_sem, 1)) /* create a mutex lock, avoid the conflict */
		 {
	    	printf("\_lwsem_create hwcodec_sem failed.\n");
		 }
	}
	
	printf("hwcodec_sem lock in hwinit \n");
	_lwsem_wait(&hwcodec_sem);       /* get the lock, avoid open twice and conflict */
	
	if(0 != codec_slave_init())
	{
	#ifdef SAI_CODEC_DEBUG
		printf("In file %s line %d  function :%s \n", __FILE__  , __LINE__, __FUNCTION__);
		printf("codec slave init failed\n");
	#endif
	}
	 
	/*
	 * sai master mode init
	 */
	if(0 != saiMasterInit())
	{
	#ifdef SAI_CODEC_DEBUG
		printf("In file %s line %d  function :%s \n", __FILE__  , __LINE__, __FUNCTION__);
		printf("sai master init failed\n");
	#endif
	}
	
#ifdef SAI_CODEC_DEBUG
	ioctl(g_sai_device_ptr, IO_IOCTL_I2S_GET_FS_FREQ, &fs_freq);
	ioctl(g_sai_device_ptr, IO_IOCTL_I2S_GET_DATA_BITS, &bit_width);
	printf("  sound information\n");
	printf("  Sampling frequency:     %d Hz\n", fs_freq);
	printf("  Bit depth:              %d bits\n", (uint_8)bit_width);
	printf("  Channels:              %d\n", chNum);
#if 0	
	/* Print transfer statistics */
	if (ioctl(g_sai_device_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats) != I2S_OK) {
		printf("  Error: Cannot read I2S statistics.\n");
		return -6;
	} else {
	
	}
#endif
	
#endif
	
	return 0;
}

int16_t sai_codec_get_occ(void)
{
	uint16_t occ = 0;
	if (ioctl(g_sai_device_ptr, IO_IOCTL_I2S_GET_BUFFER_OCC, &occ) != I2S_OK) {
	#ifdef SAI_CODEC_DEBUG
			printf("error: unable to flush the device of \"%s\"",AUDIO_DIVECE_NAME_STR);
	#endif
				occ = 50;
				return occ;
				}
		return occ;
}

int8_t sai_codec_flush(void)
{
	if (ioctl(g_sai_device_ptr, IO_IOCTL_FLUSH_OUTPUT, NULL) != I2S_OK) {
#ifdef SAI_CODEC_DEBUG
		printf("error: unable to flush the device of \"%s\"",AUDIO_DIVECE_NAME_STR);
#endif
			
				return -1;
			}
	return 0;
}

int32_t sai_codec_mute(void)
{
	codec_mute();
	return 0;
}
int32_t sai_codec_umute(void)
{
	codec_umute();
	return 0;
}

int32_t sai_codec_vol_up(void)
{
	codec_vol_up();
	return 0;
}
int32_t sai_codec_vol_down(void)
{
	codec_vol_down();
	return 0;
}

int32_t sai_codec_set_format(uint32_t sampleRate, uint8_t bitWidth, uint8_t chNum)
{
#ifdef SAI_CODEC_DEBUG
	printf("In file %s line %d  function :%s \n", __FILE__  , __LINE__, __FUNCTION__);
	printf("@@setting sai & codec samplerate %d bitwidth %d channel %d\n", sampleRate, bitWidth, chNum);
#endif
	if(codec_slave_set_bitwidth(bitWidth) != 0) return -1;
	if(saiSetFormat(sampleRate, bitWidth, chNum) != 0) return -1;
	return 0;
	
}
int32_t sai_codec_write(uint16_t *sample, uint16_t n)
{
	int32_t res = 0;

	if(NULL != g_sai_device_ptr && sample != NULL)
	res = fwrite((uint8_t *) sample, g_snd_format.bitWidth/8, n,(FILE_PTR) g_sai_device_ptr);
	if(res  != n) 
	{
#ifdef SAI_CODEC_DEBUG
		printf("drop %d samples\n",n - res);
#endif
		
	}
	return res;
}

void sai_codec_deinit()
{
	codec_slave_deinit();
	if (NULL != g_sai_device_ptr) {
		 
		if (fclose(g_sai_device_ptr) != MQX_OK) {
			
		}
		g_sai_device_ptr = NULL;
	}
	else {
		printf("sai_codec_deinit at not init\n");
		return;
	}
	
	/* put the lock, enable re-open */
	printf("hwcodec_sem unlock in hwdeinit\n");
	_lwsem_post(&hwcodec_sem);
}
#endif
