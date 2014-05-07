#ifndef _main_h_
#define _main_h_

#include <mqx.h>
#include <lwmsgq.h>
#include "lp_main.h"

extern void localPlay_init_task(uint_32);
extern void Sdcard_task(uint_32);
extern void local_player_task(uint_32 para);
extern void pcm_flush_task(uint_32 para);
extern void sd_file_search_task(uint_32 para);
#ifdef SD_PLAYER_SHELL_SUPPORTED
extern void Shell_task(uint_32);
#endif
//extern void Sdcard_write_task(uint_32);
//extern void AutoPlayer_task(uint_32);

//#define USB_ACCESSORY_PLAY
//#define USB_MSD_PLAY
//#define USB_BT_PLAY

#define  ACCESSORY_PRINTF        //debug
#ifdef USB_ACCESSORY_PLAY	
extern void mode_mgr_task(uint_32);
#endif
//extern void Android_Task(uint_32);
//extern void UsbAcc_main_task(uint_32);
//extern void ReqAcc_task(uint_32);

void otg_main_task(uint_32);

typedef enum _music_play_state
{
	mp_sBT = 0,
	mp_sAccessory,
	mp_sMSDplay,
	mp_sPCstream,
	mp_sMicroSDplay,
	mp_sIdle,
}music_play_state_type_t;

#ifdef USB_ACCESSORY_PLAY
typedef enum _music_play_source
{
	mp_for_BT = 0,                /* Play Bluetooth music is the highest priority */
	mp_for_USB_ACC,
	mp_for_USB_MSD,
	mp_for_USB_PCstream,
	mp_for_TF,
	mp_for_NULL
}music_play_source_type_t;


typedef enum _music_play_action
{
	mp_plugIn,
	mp_plugOut,
	mp_plugNone

}music_play_action_type_t;

typedef struct {
	music_play_source_type_t     conct_source;     /* class call struct of MSD instance */
	music_play_action_type_t     conct_action;     /* message body one of USB_EVENT_xxx as defined above */
	char content[5];                               /* file path */
} connect_msg_t;

extern _mqx_max_type  connect_taskq[];

extern LWSEM_STRUCT MSD_MFS_IO_SEM, SD_MFS_IO_SEM;

#endif
extern volatile music_play_state_type_t  cur_musicPlayMS;   //prev_musicPlayMS

typedef enum _music_play_source
{
	mp_for_BT = 0,                /* Play Bluetooth music is the highest priority */
	mp_for_USB_ACC,
	mp_for_USB_MSD,
	mp_for_USB_PCstream,
	mp_for_TF,
	mp_for_NULL
}music_play_source_type_t;

typedef struct {
	music_play_source_type_t  lp_type;
	LWSEM_STRUCT * mfs_io_sem;
	char path[16];
}  lp_param_t;

#endif
