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
#include"i2s.h"
#include"../HMI/hmi.h"
#include"./Sai_dma/dma/edma.h"
#include"./Sai_dma/sai/sai_dma.h"
#include"./Sai_dma/sai/init_sai_dma.h"
 
#include "../../../common/hwcodec/codec.h"



#ifndef CLK_MULT
#define CLK_MULT 384
#endif

#define AUDIO_DIVECE_NAME_STR	"sai0:"

FILE_PTR device_ptr = NULL;
static AUDIO_DATA_FORMAT_DMA audio_format;
static void i2s_testBtn1(void *p)
{
	i2s_ioctl(5,0);
}
static void i2s_testBtn2(void *p)
{
	i2s_ioctl(4,0);
}

int8_t i2s_init(uint32_t pcmSampleRate, uint8_t bitWidth, uint8_t chNum) {
	_mqx_int return_code;
	
	uint_32 mclk_freq, fs_freq, bit_width = 0;
	I2S_STATISTICS_STRUCT_DMA stats;
	static int8_t ctrl = 0;
	if(ctrl == 0)
	{
		dma_init();
		_io_sai_dma_int_install(&_bsp_sai_dma_init);

		ctrl = 1;
	}
	fs_freq = pcmSampleRate;
	audio_format.BITS = bitWidth;
	audio_format.CHANNELS = chNum;
	audio_format.ALIGNMENT = AUDIO_ALIGNMENT_LEFT;
	audio_format.ENDIAN = AUDIO_LITTLE_ENDIAN;
	audio_format.SIZE = (audio_format.BITS + 7)/8;
	mclk_freq = fs_freq * CLK_MULT;
	

	if(0 != codec_init()) return -1;

	
	if ((device_ptr = fopen(AUDIO_DIVECE_NAME_STR, "w")) == NULL) {
		printf("  Error: Unable to open the device \"%s\".\n",
				AUDIO_DIVECE_NAME_STR);
		return -2;
	}
	 
	// Setup audio data format in device 
	if (ioctl(device_ptr, IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT,
				&audio_format) != I2S_OK) {
			printf("  Error: Input data format not supported.\n");
			return -3;
			
		}
		// Setup rest of parameters - master clock, valid data bits and sampling frequency 
	if ((ioctl(device_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &mclk_freq) != I2S_OK)
			|| (ioctl(device_ptr, IO_IOCTL_I2S_SET_DATA_BITS,
					&audio_format.BITS) != I2S_OK)
			|| (ioctl(device_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &fs_freq)
					!= I2S_OK)) {
		printf("  i2s_init Error: Unable to setup \"%s\" device driver.\n",
				AUDIO_DIVECE_NAME_STR);
		
		 return -4;
	}
     
	if (return_code != 0) {
		printf("  Audio codec configuration failed. Error 0x%X.\n",
				return_code);
		return -5;
		
	}

	ioctl(device_ptr, IO_IOCTL_I2S_GET_FS_FREQ, &fs_freq);
	ioctl(device_ptr, IO_IOCTL_I2S_GET_DATA_BITS, &bit_width);
	printf("  Playback information\n");
	printf("  Sampling frequency:     %d Hz\n", fs_freq);
	printf("  Bit depth:              %d bits\n", (uint_8)bit_width);
	printf("  Channels:              %d\n", chNum);
	
	/* Print transfer statistics */
	if (ioctl(device_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats) != I2S_OK) {
		printf("  Error: Cannot read I2S statistics.\n");
		return -6;
	} else {
	
	}
	
	return 0;
}

int8_t i2s_flush(void)
{
	if (ioctl(device_ptr, IO_IOCTL_FLUSH_OUTPUT, NULL) != I2S_OK) {
			printf("error: unable to flush the device of \"%s\"",AUDIO_DIVECE_NAME_STR);
				return -1;
			}
	return 0;
}
int8_t i2s_ioctl(uint32_t cmd, uint32_t value)
{
	uint32_t mclk_freq = 0;
	uint16_t vol = 0;
	switch(cmd)
	{
	case 1:
		mclk_freq = value*CLK_MULT;
		if ((ioctl(device_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &mclk_freq) != I2S_OK)
							|| (ioctl(device_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &value)
									!= I2S_OK)) {
						printf("  Error: Unable to config sample rate of \"%s\" device driver.\n",
								AUDIO_DIVECE_NAME_STR);
						 return -1;
					}
		break;
	case 2:
		audio_format.BITS = value;
		audio_format.SIZE = (audio_format.BITS + 7)/8; 
		if (ioctl(device_ptr, IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT,
												&audio_format) != I2S_OK)
		{
		printf("  Error: Unable to config channel number of \"%s\" device driver.\n",
				AUDIO_DIVECE_NAME_STR);
		
		 return -1;
		}
		 
		break;
	case 3:
		audio_format.CHANNELS = value;
		if (ioctl(device_ptr, IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT,
										&audio_format) != I2S_OK)
							{
							printf("  Error: Unable to config channel number of \"%s\" device driver.\n",
									AUDIO_DIVECE_NAME_STR);
							
							 return -1;
						}
		 
		break;
	case 4:
		codec_vol_up();
		break;
	case 5:
		codec_vol_down();
		
		break;

	default:
		return -128;
		break;
	}
	
	 return 0;
}
int32_t i2s_write(uint16_t *sample, uint16_t n)
{
	int32_t res = 0;

	if(NULL != device_ptr && sample != NULL)
	res = fwrite((uint8_t *) sample, audio_format.SIZE, n,(FILE_PTR) device_ptr);
	if(res  != n) printf("drop %d samples\n",n - res);
	return 1;
}

void i2s_deinit()
{
	I2S_STATISTICS_STRUCT_DMA stats;
	if (ioctl(device_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats) != I2S_OK) {
		printf("  Error: Cannot read I2S statistics.\n");
		return;
	} else {
		printf("BUF\tFIFO\tINTS\tPACKETS_PRO\tPACKETS_Q\tPACKETS_R\tRX_PACKETS\tTXs\n");
		printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",stats.BUFFER_ERROR,stats.FIFO_ERROR,stats.INTERRUPTS,stats.PACKETS_PROCESSED,stats.PACKETS_QUEUED,stats.PACKETS_REQUESTED,stats.RX_PACKETS,stats.TX_PACKETS);
	}
	memset((void *)&audio_format, 0, sizeof(audio_format));
	if (NULL != device_ptr) {
		 
		if (fclose(device_ptr) != MQX_OK) {
			
		}
		device_ptr = NULL;
	}
}
