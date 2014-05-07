/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: main.h$
* $Version : 3.8.3.0$
* $Date    : Sep-12-2012$
*
* Comments:
*
*   This include file is used to provide information needed by
*   applications using the SAI I/O functions.
*
*END************************************************************************/

#ifndef __lp_main_h_
#define __lp_main_h_

#include <mqx.h>
#include <bsp.h>
#include <lwevent.h>
#include <message.h>


#define LOCALPLAY_INIT_TASK 1
#define SDCARD_TASK 2
#define SHELL_TASK 3
#define SDCARD_WRITE_TASK 4
#define PLAYER_TASK	5
#define SD_WALKER_TASK  6
#define PCM_FLUSH_TASK  7         


#define AUDIO_SAMPLE_RATE   (44100)

//#define SD_PLAYER_SHELL_SUPPORTED

#ifndef CLK_MULT
#ifdef BSPCFG_ENABLE_SAI
#define CLK_MULT             (384)
#else
#define CLK_MULT             (256)
#endif
#endif


#define WRITE_QUEUE 8
#define REC_BLOCK_SIZE 512

/* Parameter structure required for SD write task */
typedef struct sd_write_param_struct
{
    /* Pointer to output file */
    MQX_FILE_PTR file_ptr;
    
    /* LW Semaphore pointer signaling finished writing to SD card */
    LWSEM_STRUCT sdwrite_sem;   
} SD_WRITE_PARAM_STRUCT;

/* This struct contains a data field and a message struct. */
typedef struct
{
    /* Message header */ 
    MESSAGE_HEADER_STRUCT HEADER;
    
    /* Data length */
    uint_32 LENGTH;
    
    /* Data */
    char DATA[REC_BLOCK_SIZE];   
} REC_MESSAGE, _PTR_ REC_MESSAGE_PTR;


#define PLAYER_EVENT_MSK_NEXT_BTN_PRESSED       0x01
#define PLAYER_EVENT_MSK_PREV_BTN_PRESSED       (0x01 << 1)
#define PLAYER_EVENT_MSK_SD_FS_MOUNTED          (0x01 << 2)
#define PLAYER_EVENT_MSK_SD_FS_UNMOUNTED        (0x01 << 3)
#define PLAYER_EVENT_MSK_SONG_READY             (0x01 << 4)
#define PLAYER_EVENT_MSK_PLAYER_TASK_KILLED		(0x01 << 5)
#define PLAYER_EVENT_MSK_SHELL_COMMAND			(0x01 << 6)

#define PLAYER_EVENT_MSK_USB_ATTACHED		(0x01 << 7)   

//#define PLAYER_EVENT_MSK_SD_FS_UNMOUNTED_DISPOSED		(0x01 << 8)   //add by guoyifang for sync Close FS, before CloseFS, close opened file firstly .

#define PLAYER_EVENT_MSK_SONG_RESUME		(0x01 << 8)   

#define PLAYER_EVENT_MSK_SEARCH_TASK_QUIT		(0x01 << 9)
#define PLAYER_EVENT_MSK_SEARCH_TASK_KILLED		(0x01 << 10)
#define AUDIO_BUF_CNT	1

extern volatile boolean decoding ;
extern LWEVENT_STRUCT player_event;

#endif /* __main_h_ */

/* EOF */
