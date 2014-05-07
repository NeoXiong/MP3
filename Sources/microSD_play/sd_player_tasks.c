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
 **********************************************5****************************
 *
 * $FileName: sd_player_task.c$
 * $Version : 3.8.3.0$
 * $Date    : Sep-12-2012$
 *
 *END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <mfs.h>
#include <sh_mfs.h>
#include <shell.h>
#include <sdcard.h>
#include <part_mgr.h>
#include "lp_main.h"
#include "main.h"
#include "log_messages.h"
#include "sh_audio.h"
#include "fs_walker.h"

//#include "codec_interface.h"
#include "codec_interface_public_api.h"
#include "cci_metadata.h"
#include "codec_support.h"
#include "codec_interface_mqx_wrapper.h"
#include "msi.h"

#ifndef LOCALPLAY_DEBUG
#define LOCALPLAY_DEBUG       0
#else 
#undef LOCALPLAY_DEBUG
#define LOCALPLAY_DEBUG       0
#endif

#if LOCALPLAY_DEBUG
#define LOCALPLAY_LOG(...)  \
        {   \
            printf("\nI2S_LOG: %s: %d", __FILE__, __LINE__);  \
            printf(__VA_ARGS__);    \
        }
#else
#define LOCALPLAY_LOG(...) {}
#endif



extern volatile boolean decoding;
extern LWGPIO_STRUCT sd_detect, btn_next, btn_prev;
extern LWEVENT_STRUCT player_event;

const char *ext_wantted[] = { "MP3",
		NULL };
char full_path[255];

/*  local variable for decoder */
static int32_t *g_memory[3];
static int32_t *g_callbackFunctionArray[3];
static int32_t g_userData[3];
uint8_t dffi_t_buf[140]; // MP3 and AAC Metadata lib need this array


static void print_metadata(file_meta_data_t *metaData) {
	printf("\n ************** Metadata Information ******************* \n");
	if (metaData->stream_type == STREAM_TYPE_MP3)
		printf("\n MP3 File");
	else if (metaData->stream_type == STREAM_TYPE_WMA)
		printf("\n WMA File");
	else if (metaData->stream_type == STREAM_TYPE_OGG)
		printf("\n OGG File");
	else if (metaData->stream_type == STREAM_TYPE_AAC)
		printf("\n AAC File");
	else if (metaData->stream_type == STREAM_TYPE_FLAC)
		printf("\n FLAC File");
	else if (metaData->stream_type == STREAM_TYPE_APE)
		printf("\n APE File");
	else if (metaData->stream_type == STREAM_TYPE_ALAC)
		printf("\n ALAC File");
	else if (metaData->stream_type == STREAM_TYPE_OPUS)
		printf("\n OPUS File");
	else if (metaData->stream_type == STREAM_TYPE_PCM)
		printf("\n WINDOWS PCM File");
	else if (metaData->stream_type == STREAM_TYPE_IMAADPCM)
		printf("\n IMA ADPCM File");
	else if (metaData->stream_type == STREAM_TYPE_MSADPCM)
		printf("\n MS ADPCM File");
	else
		printf("\n Unknown File");

	printf("\n bits_per_sample = %d", metaData->bits_per_sample);
	printf("\n Sample Rate = %d", metaData->sample_rate);
	printf("\n Bit Rate = %d", metaData->bit_rate);
	printf("\n Channel = %d", metaData->num_channels);
	printf("\n Total Time in second = %d", metaData->total_time);
	printf("\n ******************************************************* \n");
}

void sd_file_search_task(uint_32 para) {
	
	lp_param_t *  lpp_param =  (lp_param_t *)para;
	char * path = (char *)lpp_param->path;
	set_file_filter((char **) ext_wantted);
	
	while (1) {
		_lwevent_wait_ticks(
				&player_event,
				PLAYER_EVENT_MSK_NEXT_BTN_PRESSED
						| PLAYER_EVENT_MSK_PREV_BTN_PRESSED
						| PLAYER_EVENT_MSK_SD_FS_MOUNTED |PLAYER_EVENT_MSK_SEARCH_TASK_QUIT, FALSE, 0);

		full_path[0] = '\0';

		if (player_event.VALUE & PLAYER_EVENT_MSK_SEARCH_TASK_QUIT) {
			printf("search task %d exit\n",lpp_param->lp_type);
			_lwevent_clear(&player_event, PLAYER_EVENT_MSK_SEARCH_TASK_QUIT);
			goto exit_search;
		}

		printf("search lock umount at %d\n",lpp_param->lp_type);
		//_lwsem_wait(lpp_param->mfs_io_sem);
		if (player_event.VALUE & PLAYER_EVENT_MSK_NEXT_BTN_PRESSED) {
			get_next_file_name("a:\\", full_path);
			//get_next_file_name(path, full_path);
			_lwevent_clear(&player_event, PLAYER_EVENT_MSK_NEXT_BTN_PRESSED);
			
			//printf("search id1\n");
		} else if (player_event.VALUE & PLAYER_EVENT_MSK_PREV_BTN_PRESSED) {
			get_prev_file_name("a:\\", full_path);
			//get_prev_file_name(path, full_path);
			_lwevent_clear(&player_event, PLAYER_EVENT_MSK_PREV_BTN_PRESSED);
			
			//printf("search id2\n");
		} else if (player_event.VALUE & PLAYER_EVENT_MSK_SD_FS_MOUNTED) {
			get_next_file_name("a:\\", full_path);
			//get_next_file_name(path, full_path);
			_lwevent_clear(&player_event, PLAYER_EVENT_MSK_SD_FS_MOUNTED);
			//printf("search id3\n");
		}		

		printf("search unlock umount at %d\n",lpp_param->lp_type);
		//_lwsem_post(lpp_param->mfs_io_sem);
		/* If found*/
		if (full_path[0] != '\0')
			{_lwevent_set(&player_event, PLAYER_EVENT_MSK_SONG_READY);
			 printf("search y\n");
			}
		else
		     printf("search n\n");
	}

  //LOCALPLAY_LOG("__guoyifang__: sd_file_search_task  destroyed.\n");
exit_search:
   printf("__guoyifang__: sd_file_search_task  %d set SERAR_TASK_KILLED.\n",lpp_param->lp_type);
  _lwevent_set(&player_event, PLAYER_EVENT_MSK_SEARCH_TASK_KILLED);
 _task_block(); //wait for being destroyed

	
}

/* Allocate a buffer for cached decoded pcm,  and flush this buffer to sai driver. */
static uchar_ptr g_audio_buf_ptr;  
static volatile uint32_t g_buf_bytes_to_flush = 0;
/* static */ LWSEM_STRUCT   pcm_flush_sem, pcm_decoded_sem;

void pcm_flush_task(uint_32 para) 
{

   while(1)
   	{ 
	   
	   _lwevent_wait_ticks(&player_event, PLAYER_EVENT_MSK_SONG_RESUME, FALSE, 0);
	   if (_lwsem_wait(&pcm_decoded_sem) != MQX_OK)
	   {
		 LOCALPLAY_LOG("\n  pcm_flush : Error - Unable to wait pcm_decoded_sem.");
	   }
	     
#if 0
	   if (g_device_ptr == NULL) {
	   			LOCALPLAY_LOG(
	   					"The audio device pointer is NULL. audio_fulsh_task exits. \n");
	   			break;
	   		} 
#endif  	 	    
       put_file_data(g_audio_buf_ptr, g_buf_bytes_to_flush, NULL);
    	 
	   if (_lwsem_post(&pcm_flush_sem) != MQX_OK)
		{
		  //LOCALPLAY_LOG("\n  pcm_flush : Error - Unable to post pcmFlush_sem.");
		}
    }

   //LOCALPLAY_LOG("__guoyifang__: pcm_flush_task  destroyed.\n");
   _task_block(); //wait for being destroyed
   
}

//static FILE_PTR device_ptr = NULL;
//extern FILE_PTR device_ptr;

#define AUDIO_DIVECE_NAME_STR	"sai0:"
void local_player_task(uint_32 para) 
{
	_mqx_int return_code, res;
	my_audio_format_t format;
	FILE_PTR stream_ptr = NULL, stream_ptr1 = NULL;      //device_ptr = NULL, 
	uint_32 mclk_freq, fs_freq, bit_width = 0;
	I2S_STATISTICS_STRUCT stats;
	int32_t numberOfSamples =0, sampleProduced, bufOut;
	file_meta_data_t * metadata = NULL;
	audio_stream_type_t stream_type;
	char_ptr mem_ptr = NULL;
	boolean shell_cmd = FALSE;
	
    CCI_Ctx ctx;
	int32_t strLen, i;
	uint32_t file_extension=0;

	
	_task_id pcm_flush_id = MQX_NULL_TASK_ID;
	uint_32 cnt = 0;
	int32_t max_audio_buf_size = 0;
	lp_param_t *  lpp_param =  (lp_param_t *)para;

	/* initialize the audio subsystem(sai and codec) here */
#if 0
	if ((device_ptr = fopen(AUDIO_DIVECE_NAME_STR, "w")) == NULL)
	{
		LOCALPLAY_LOG("  Error: Unable to open the device \"%s\".\n",
				AUDIO_DIVECE_NAME_STR);
		return;
	}
#else
	
	printf("local_player_task.. Enter 1\n");
	
	cur_musicPlayMS = mp_sMicroSDplay;
	
	if (msi_snd_init() != 0)
	{
		LOCALPLAY_LOG("  Error: Unable to open the device \"%s\".\n",
				AUDIO_DIVECE_NAME_STR);
		return;
	}
#endif		
	
	
	/* create semaphore must before pcm_flush_task */
	if(MQX_OK != _lwsem_create(&pcm_decoded_sem, 0))
	{
		LOCALPLAY_LOG("\n Error - Unable to creat lwsem: pcm_decoded_sem\n");
	}
	
	
    LOCALPLAY_LOG("Creating pcm flush task.........\n");
	pcm_flush_id = _task_create(0, PCM_FLUSH_TASK, 0);
	if (pcm_flush_id == MQX_NULL_TASK_ID)
	 {
		LOCALPLAY_LOG("Creat pcm flush task failed\n");
		goto clean_up;
		
	 }
	else
	 _lwevent_set(&player_event, PLAYER_EVENT_MSK_SONG_RESUME);

	metadata = (file_meta_data_t *) _mem_alloc_zero(sizeof(file_meta_data_t));
	if (NULL == metadata) 
	{
		LOCALPLAY_LOG("\n Failed to allocate memory for metadata.\n");
		return;
	}
	
	while (1) {

#ifndef  USB_ACCESSORY_PLAY		
		_lwevent_wait_ticks(
				&player_event,
				PLAYER_EVENT_MSK_SONG_READY | PLAYER_EVENT_MSK_SD_FS_UNMOUNTED
						| PLAYER_EVENT_MSK_SHELL_COMMAND, FALSE, 0);
#else
_lwevent_wait_ticks(
				&player_event,
				PLAYER_EVENT_MSK_SONG_READY | PLAYER_EVENT_MSK_SD_FS_UNMOUNTED
						| PLAYER_EVENT_MSK_SHELL_COMMAND | PLAYER_EVENT_MSK_USB_ATTACHED, FALSE, 0);		
#endif
		shell_cmd = FALSE;


#ifdef USB_ACCESSORY_PLAY
		if (player_event.VALUE & PLAYER_EVENT_MSK_USB_ATTACHED) {
					_lwevent_clear(&player_event, PLAYER_EVENT_MSK_USB_ATTACHED);
					//LOCALPLAY_LOG("__guoyifang__: sd_player_task PLAYER_EVENT_MSK_USB_ACC_ATTACHED \n");
					break;
				}
#endif

		if (player_event.VALUE & PLAYER_EVENT_MSK_SD_FS_UNMOUNTED) {
			_lwevent_clear(&player_event, PLAYER_EVENT_MSK_SD_FS_UNMOUNTED);
			//LOCALPLAY_LOG("__guoyifang__: sd_player_task PLAYER_EVENT_MSK_SD_FS_UNMOUNTED \n");
			break;
		}	

		if (player_event.VALUE & PLAYER_EVENT_MSK_SONG_READY) {
			_lwevent_clear(&player_event, PLAYER_EVENT_MSK_SONG_READY);
		}

		if (player_event.VALUE & PLAYER_EVENT_MSK_SHELL_COMMAND) {
			_lwevent_clear(&player_event, PLAYER_EVENT_MSK_SHELL_COMMAND);
			shell_cmd = TRUE;
		}

		LOCALPLAY_LOG(" --------------------------------------------------------------\n");

	    printf("play lock umount at %d\n",lpp_param->lp_type);
	    _lwsem_wait(lpp_param->mfs_io_sem);
	    /******************decoding is a critical value, assume decoding is TRUE every time.*******************/    
	    printf("SET decoding.\n");
	    decoding = TRUE; // next/prev btn ISR may clear decoding to 0
	    /*******************************************/
		/*config the audio subsystem according metadata*/
		        		printf(
		        				"  Open stream file %s\n", full_path);
		        		stream_ptr = fopen(full_path, "r");
		        		if (stream_ptr == NULL) {
		        			printf("  Unable to open the file: %s\n", full_path);
		        			goto clean_up;
		        		}

		        		stream_ptr1 = fopen(full_path, "r");
		        		if (stream_ptr1 == NULL) {
		        			printf("  Unable to open the file: %s\n", full_path);
		        			goto clean_up;
		        		}
		        		
		                /* Determine the extension of the file */ 
		                i=0;
		                strLen= strlen((const char *)full_path);

		                if(strLen > 4){
		                    /* find the '.' */
		                    while(strLen--){
		                        if(full_path[i]=='.')
		                            break;
		                        i++;
		                    }
		                    if(strLen){
		                        char *p;
		                        /* Copy out the extension : 8.3 filename */
		                        memcpy(&file_extension, full_path+i+1, 3);
		                        p = (char *) (&file_extension);
		                        for(i = 0; i < 4; i++, p++){
		                            if((*p >= 'a') && (*p <= 'z')){
		                                *p -= ('a' - 'A');
		                            }
		                        }
		                    } 

		                    ctx.user_data = (void*) stream_ptr;
		                    ctx.cci_dec_read = get_file_data;;
		                    ctx.cci_dec_seek = seek_file_data;
		                    ctx.cci_dec_tell = get_file_position;

		                    /* Check if metadata was found. */
		                    return_code = cci_extract_meta_data(file_extension, metadata, &ctx);        
		        		    if (return_code != 0) {
		        			    printf("\n Metadata not found\n");
		        			    goto clean_up;
		        		    }

		        		    print_metadata(metadata); //todo

		                } else {
		        			printf("\n Metadata not found\n");
		        			goto clean_up;
		                }
		                if ( metadata->stream_type == STREAM_TYPE_MP3 ) {
		                	/* Seek from the beginning of the file */ 
		                    seek_file_data(0, metadata->start_pos, 0, stream_ptr);
		                } else {
		                	/* Seek from the beginning of the file */
		                	seek_file_data(0, 0, 0, stream_ptr);
		        		}
		        
		        
		        stream_type = metadata->stream_type;

#if 0	
		format.audio_format.ENDIAN = AUDIO_LITTLE_ENDIAN;
		format.audio_format.ALIGNMENT = AUDIO_ALIGNMENT_LEFT;
		if((streamType == kCodecStreamTypePcm) ||	// bitsPerSample Value is 8/16/24
				/*
				 * The demo not support kCodecStreamTypeImaAdpcm/kCodecStreamTypeMsAdpcm currently.
				 */
				(streamType == kCodecStreamTypeImaAdpcm) || // bitsPerSample Value is 4
				(streamType == kCodecStreamTypeMsAdpcm))	// bitsPerSample Value is 4
		{	
			format.audio_format.BITS = metadata->i32BitsPerSample;
		}else{
			format.audio_format.BITS = 16; 
		}
		
		// Currently, the wave decoder output 16bits only for kCodecStreamTypePcm.
		format.audio_format.BITS = 16;
		
		format.audio_format.SIZE = (format.audio_format.BITS + 7)/8;

		
		format.audio_format.CHANNELS = metadata->i32NumChannels;
		format.fs_freq = metadata->u32SampleRate;

		fs_freq = format.fs_freq;
		mclk_freq = fs_freq * CLK_MULT;
		// Setup audio data format in device 
		if (ioctl(device_ptr, IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT,
				&format.audio_format) != I2S_OK) {
			LOCALPLAY_LOG("  Error: Input data format not supported.\n");
			goto clean_up;
		}
		// Setup rest of parameters - master clock, valid data bits and sampling frequency 
		if ((ioctl(device_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &mclk_freq) != I2S_OK)
				|| (ioctl(device_ptr, IO_IOCTL_I2S_SET_DATA_BITS,
						&format.audio_format.BITS) != I2S_OK)
				|| (ioctl(device_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &fs_freq)
						!= I2S_OK)) {
			LOCALPLAY_LOG("  Error: Unable to setup \"%s\" device driver.\n",
					AUDIO_DIVECE_NAME_STR);
			goto clean_up;
		}

		// Setup audio codec 
		return_code = SetupCodec(device_ptr);
		if (return_code != 0) {
			LOCALPLAY_LOG("  Audio codec configuration failed. Error 0x%X.\n",
					return_code);
			goto clean_up;
		}

		ioctl(device_ptr, IO_IOCTL_I2S_GET_FS_FREQ, &fs_freq);
		ioctl(device_ptr, IO_IOCTL_I2S_GET_DATA_BITS, &bit_width);
		LOCALPLAY_LOG("  Playback information\n");
		LOCALPLAY_LOG("  Sampling frequency:     %d Hz\n", fs_freq);
		LOCALPLAY_LOG("  Bit depth:              %d bits\n", (uint_8)bit_width);
		LOCALPLAY_LOG("  Channels:               ");
		
		if (format.audio_format.CHANNELS == 1) {
			LOCALPLAY_LOG("mono\n");
		} else {
			LOCALPLAY_LOG("stereo\n");
		}
#else
		if((stream_type == STREAM_TYPE_PCM) ||	// bitsPerSample Value is 8/16/24
		/*
		 * The demo not support kCodecStreamTypeImaAdpcm/kCodecStreamTypeMsAdpcm currently.
		 */
		(stream_type == STREAM_TYPE_IMAADPCM) || // bitsPerSample Value is 4
		(stream_type == STREAM_TYPE_MSADPCM))	// bitsPerSample Value is 4
		{	
			format.audio_format.BITS = metadata->bits_per_sample;
		}else{
			format.audio_format.BITS = 16; 
		}
		
		format.audio_format.CHANNELS = metadata->num_channels;
		// Currently, the wave decoder output 16bits only for kCodecStreamTypePcm.
		format.audio_format.BITS = 16;
		
#if 0
		if(audio_ioctl(setChNum, format.audio_format.CHANNELS)!= I2S_OK)
        {
		  LOCALPLAY_LOG("  Error: audio_ioctl setChNum failed.\n");
		  goto clean_up;
        }
		if(audio_ioctl(setBitWidth, format.audio_format.BITS)!= I2S_OK)
		{

		  LOCALPLAY_LOG("  Error: audio_ioctl setBitWidth failed.\n");
		  goto clean_up;
		}
#endif
		
		format.fs_freq = metadata->sample_rate;
		
#if 0
		if(audio_ioctl(setSamplerate, format.fs_freq)!= I2S_OK)
	    {
		  LOCALPLAY_LOG("  Error: audio_ioctl setSamplerate failed.\n");
		  goto clean_up;
	    }
#endif
		msi_snd_set_format(format.fs_freq, format.audio_format.BITS, format.audio_format.CHANNELS);
	
		// Setup audio codec 
	    //return_code = SetupCodec(device_ptr);
#endif

		mem_ptr = (char_ptr) _mem_alloc_zero(codec_get_mem_info(stream_type));
		if (NULL == mem_ptr) {
			LOCALPLAY_LOG("Failed to allocate memory for the decoder.\n");
			goto clean_up;
		}

		// MP4 decoder need two fd
		g_userData[0] = (int) stream_ptr;
		g_userData[1] = (int) stream_ptr1;
		if (metadata->audio_sub_type == MEDIA_SUBTYPE_ADTS)
			g_userData[2] = 1;
		else if (metadata->audio_sub_type == MEDIA_SUBTYPE_M4A)
			g_userData[2] = 2;
		else if ( metadata->stream_type == STREAM_TYPE_OPUS )
		{
			g_userData[2] = metadata->sample_rate;
			g_userData[3] = metadata->num_channels;
	    }
		else
			g_userData[2] = 0; 

		g_callbackFunctionArray[0] = (int32_t *) &get_file_data;
		g_callbackFunctionArray[1] = (int32_t *) &seek_file_data;
		g_callbackFunctionArray[2] = (int32_t *) &get_file_position;

		while (1) {
			res = codec_init(stream_type, (long **)&mem_ptr,g_callbackFunctionArray,
								&g_userData[0]);
			if (res == CODEC_INIT_ERROR) {
				LOCALPLAY_LOG("\n  Codec Init Failed with error code %d\n", res);
				decoding = FALSE; 
				goto clean_up;
			}
			if (res == CODEC_MORE_DATA_REQUIRED) {
				LOCALPLAY_LOG("\n  More Data Processing Required for Init \n");
			}
			if (res == CODEC_SUCCESS) {
				LOCALPLAY_LOG("\n  Codec Init Done Successfully \n\n");
				break;
			}
			else {
				printf("codec init other err\n");
				decoding = FALSE;
				goto clean_up;
			}
		}

		if (res == CODEC_SUCCESS) {
			printf("  Playing %s...\n\n", full_path);
		//	ioctl(device_ptr, IO_IOCTL_I2S_CLEAR_STATISTICS, NULL);
			/* Reset variables before every song's playbacking */
			
			//printf("SET decoding.\n");
			//decoding = TRUE; // next/prev btn ISR may clear decoding to 0
			cnt = 0;

			max_audio_buf_size = 0;
			g_audio_buf_ptr = NULL;
			//_lwevent_clear(&player_event, PLAYER_EVENT_MSK_AUDIO_BUF_FILLED);
			/* Clear pcm_decoded_sem for play next song */
			_lwsem_poll(&pcm_decoded_sem);
			_lwsem_poll(&pcm_decoded_sem);  
					
			if(MQX_OK != _lwsem_create(&pcm_flush_sem, AUDIO_BUF_CNT))
			{
				LOCALPLAY_LOG("\n Error - Unable to create lwsem: pcm_flush_sem\n");
			}
			
			/*
			 * umute
			 */
			msi_snd_umute();
			while (decoding) {
				/*
				 * For FLAC decoder, it produced more than 18K bytes per frame, and takes about 30ms. 
				 * While take 44.1K/16bit/2ch/4Kbytes DMA buffer as example, the margin time is 4K/4/2/44100~=10ms. 
				 * We need bigger DMA buffer, or decode in a ping-pong way.
				 */		
				res = codec_decode(stream_type, (long **)&mem_ptr, &sampleProduced,
										&bufOut);
              
				if (res == CODEC_END_OF_DECODE) {					
					LOCALPLAY_LOG("\n  End of Decode \n");
					break;
				}
				else if (res == CODEC_DECODE_ERROR) {
					LOCALPLAY_LOG("\n  Codec Decode Failed \n");
					break;
				}
				else if(res != CODEC_SUCCESS){
					printf("codec_decode else err %d\n",res);
						break;
				}
				
				cnt += sampleProduced;
				
				while (decoding) 
				 {
					
					numberOfSamples = codec_get_pcm_samples(stream_type,
											(long **)&mem_ptr, &sampleProduced, &bufOut);
					if (numberOfSamples == 0)
						break;
					
					if (max_audio_buf_size < sampleProduced) { //More bigger buffer needed
						max_audio_buf_size = sampleProduced;
						if (NULL != g_audio_buf_ptr) {
							LOCALPLAY_LOG(" Bigger buffer needed.\n");
							_mem_free(g_audio_buf_ptr);
						}
						g_audio_buf_ptr = (uchar_ptr) _mem_alloc(max_audio_buf_size);
						if (NULL == g_audio_buf_ptr) {
							LOCALPLAY_LOG(" Failed to allocate g_audio_buf_ptr.\n");
							decoding = 0;
							break;
						}
					}
					
				    if (MQX_OK != _lwsem_wait(&pcm_flush_sem))
				    {
					  LOCALPLAY_LOG("\n Error: Wait for pcm_flush_sem failed.\n");
					 // _task_set_error(res);
				    }
				    
					_mem_copy((void *)bufOut, g_audio_buf_ptr, sampleProduced);
					g_buf_bytes_to_flush = sampleProduced;
		
					if (_lwsem_post(&pcm_decoded_sem) != MQX_OK)
					 {
					   LOCALPLAY_LOG("\n  pcm_flush : Error - Unable to set pcm_decoded_sem.");
					 }

					if (numberOfSamples == sampleProduced)
						sampleProduced = 0;
				 }//end while decoding
	 	  }//end while decoding
		}//end if res==kCodeSuccess
		
		#if 0
		fflush(device_ptr);
		#else
		msi_snd_mute();
		msi_snd_flush();
		#endif

#if 0
		/* Print transfer statistics */
		if (ioctl(device_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats) != I2S_OK) {
			LOCALPLAY_LOG("  Error: Cannot read I2S statistics.\n");
		} else {
			LOCALPLAY_LOG("\n  Playback stats\n");
			LOCALPLAY_LOG("  Total interrupts:              %d\n", stats.INTERRUPTS);
			LOCALPLAY_LOG("  Bytes requested for transmit:  %d\n",
					stats.PACKETS_REQUESTED * format.audio_format.SIZE);
			LOCALPLAY_LOG("  Bytes transmitted:             %d\n",
					stats.TX_PACKETS * format.audio_format.SIZE);
			LOCALPLAY_LOG("  Underruns of hardware FIFO:    %d\n", stats.FIFO_ERROR);
			LOCALPLAY_LOG("  Software buffer empty:         %d\n", stats.BUFFER_ERROR);
		}
#endif
		/* LOCALPLAY_LOG*/ printf("\n  DONE\n");
			
		clean_up:
		printf("done to clean up,decoding %d \n",decoding);		
		/* Clean up for next song */
		if (NULL != mem_ptr) {
			_mem_free(mem_ptr);
			mem_ptr = NULL;
		}
		if (NULL != g_audio_buf_ptr) {
			_mem_free(g_audio_buf_ptr);
			g_audio_buf_ptr = NULL;
		}
		if (NULL != stream_ptr) {
			if (fclose(stream_ptr) != MQX_OK) {
				/*LOCALPLAY_LOG*/printf("  Error: Unable to close file %s.\n", full_path);
			}
			stream_ptr = NULL;
		}
		if (NULL != stream_ptr1) {
			if (fclose(stream_ptr1) != MQX_OK) {
				/*LOCALPLAY_LOG*/printf("  Error: Unable to close file %s.\n", full_path);
			}
			stream_ptr1 = NULL;
		}
		
		if(MQX_OK != _lwsem_destroy(&pcm_flush_sem))
		  {
			LOCALPLAY_LOG("\n Error - Unable to destroy lwsem: pcm_flush_sem\n");
		  }

		if (decoding == TRUE) { // playback finished in normal way, ie, next/prev not pressed
			if (!shell_cmd) { 	// and was not triggered by Shell
				// just like next btn being pressed
				_lwevent_set(&player_event, PLAYER_EVENT_MSK_NEXT_BTN_PRESSED);
			}
		}
		
	    /* if exit this task ,must route this point ! */
	    printf("play unlock umount at %d\n",lpp_param->lp_type);
	    _lwsem_post(lpp_param->mfs_io_sem);
	}//end while(1)
#if 0
	// clean up further
	if (NULL != device_ptr) {	
		if (fclose(device_ptr) != MQX_OK) {
			LOCALPLAY_LOG("  Error: Unable to close \"%s\" device driver.\n",
					full_path);
		}
	}
#else	
	msi_snd_deinit();
#endif
			
	if (NULL != metadata)
		_mem_free(metadata);	

	if (MQX_NULL_TASK_ID != pcm_flush_id)
	  {
	    _lwsem_post(&pcm_decoded_sem);
	    _task_destroy(pcm_flush_id);
	    pcm_flush_id = MQX_NULL_TASK_ID;
	    LOCALPLAY_LOG(" pcm flush task destoryed \n");
	  }
	
	if(MQX_OK != _lwsem_destroy(&pcm_decoded_sem))
		  {
			LOCALPLAY_LOG("\n Error - Unable to destroy lwsem: pcm_decoded_sem\n");
		  }
	
  printf("__guoyifang__: sd_player_task %d set PLAYER_TASK_KILLED.\n",lpp_param->lp_type);
  _lwevent_set(&player_event, PLAYER_EVENT_MSK_PLAYER_TASK_KILLED);
  
  LOCALPLAY_LOG("__guoyifang__: sd_player_task  destroyed.\n");
  _task_block(); //wait for being destroyed
  
}

