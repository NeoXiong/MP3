#include "main.h"


/* music play machine state. */
volatile music_play_state_type_t cur_musicPlayMS = mp_sIdle;


/* MQX template list */


TASK_TEMPLATE_STRUCT MQX_template_list[] = 
{ 
    /*  Task number, Entry point, Stack, Pri, String,   Auto? */
    { LOCALPLAY_INIT_TASK,       localPlay_init_task,  1000/*The stack size 16bytes alignment*/,   9 + 1,   "local_play_init", MQX_AUTO_START_TASK},   //MQX_AUTO_START_TASK
    { SDCARD_TASK,               Sdcard_task,          1500/*2000*/,  10 + 1 + 1,   "sdcard Task", 0 },  //2000  /* deserved high priority since it may setevent to others */
#ifdef SD_PLAYER_SHELL_SUPPORTED
    { SHELL_TASK,                Shell_task,           1500,  13,   "shell",  0 },  //2000
#endif
    { PLAYER_TASK,               local_player_task,       3200, /*4000*/ 10 + 1,   "local_player",  0 },
    { SD_WALKER_TASK,            sd_file_search_task,  2000/*4000*/,  10 + 1,   "sd_walker",  0 },
	{ PCM_FLUSH_TASK,            pcm_flush_task, 	   1000,   9  + 1/*8*/,   "pcm_flush", 0},
	

	{0L,    0L,                0L,       0L,  0L,           0L },
};






