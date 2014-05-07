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
* $FileName: main.c$
* $Version : 3.8.3.0$
* $Date    : Sep-12-2012$
*
* Comments:
*
*   This include file is used to provide information needed by
*   applications using the SAI I/O functions.
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
#include"msi.h"
#include "sh_audio.h"
#include "fs_walker.h"


#if !BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#if ! SHELLCFG_USES_MFS
#error This application requires SHELLCFG_USES_MFS defined non-zero in user_config.h. Please recompile libraries with this option.
#endif

#if defined BSP_SDCARD_ESDHC_CHANNEL
#if ! BSPCFG_ENABLE_ESDHC
#error This application requires BSPCFG_ENABLE_ESDHC defined non-zero in user_config.h. Please recompile libraries with this option.
#endif

#elif defined BSP_SDCARD_SDHC_CHANNEL

#if ! BSPCFG_ENABLE_SDHC
#error This application requires BSPCFG_ENABLE_SDHC defined non-zero in user_config.h. Please recompile libraries with this option.
#endif

#elif defined (BSP_TWRMCF51CN) || defined (BSP_TWRPXS20) || defined (BSP_TWR_K20D72M)

#if ! BSPCFG_ENABLE_SPI1
#error This application requires BSPCFG_ENABLE_SPI1 defined non-zero in user_config.h. Please recompile libraries with this option.
#endif

#else
#if ! BSPCFG_ENABLE_SPI0
#error This application requires BSPCFG_ENABLE_SPI0 defined non-zero in user_config.h. Please recompile libraries with this option.
#endif

#endif

#if defined (BSP_SDCARD_SPI_CHANNEL)
#define SDCARD_COM_CHANNEL BSP_SDCARD_SPI_CHANNEL
#elif defined (BSP_SDCARD_ESDHC_CHANNEL)
#define SDCARD_COM_CHANNEL BSP_SDCARD_ESDHC_CHANNEL
#elif defined (BSP_SDCARD_SDHC_CHANNEL)
#define SDCARD_COM_CHANNEL BSP_SDCARD_SDHC_CHANNEL
#else
#error "SDCARD low level communication device not defined!"
#endif

#define BTN_ISR_PRIORITY	5

#ifdef SD_PLAYER_SHELL_SUPPORTED
//extern int_32 Shell_play(int_32 argc, char_ptr argv[]);
extern int_32 Shell_dir_r(int_32 argc, char_ptr argv[]);
#endif



//extern void sd_player_task(uint_32 para);
//extern void sd_file_search_task(uint_32 para);

//extern void pcm_flush_task(uint_32 para);    //add by  guoyifang for test flac

/*
 * A flag to stop the decoder when it decoding, event not used to avoid schedule jitter.
 */
volatile boolean decoding = TRUE;
static LWGPIO_STRUCT  btn_next, btn_prev;
LWEVENT_STRUCT player_event;

LWEVENT_STRUCT		sddetect_event;
extern volatile music_play_state_type_t cur_musicPlayMS;

#if 0
TASK_TEMPLATE_STRUCT MQX_template_list[] = 
{ 
    /*  Task number, Entry point, Stack, Pri, String,   Auto? */
#ifdef SD_PLAYER_SHELL_SUPPORTED
    { SHELL_TASK,                  Shell_task,        4000,  13,  "shell",  0 },
#endif
    { SD_WALKER_TASK,              sd_file_search_task,  4000,  12,  "sd_walker",  0 },
    { PLAYER_TASK,                 sd_player_task,       4000,  11,  "sd_player",  0 },
    { SDCARD_TASK,                 Sdcard_task,       1000,  10,  "sdcard", 0 },  /* deserved high priority since it may setevent to others */
    { LOCALPLAY_INIT_TASK,         localPlay_init_task,         1000,   9,  "local_play_init", 0},   //MQX_AUTO_START_TASK
	{ PCM_FLUSH_TASK,              pcm_flush_task, 	1000,	8,	"pcm_flush", 0},
    { 0, 0, 0, 0, 0, 0 }
};

#endif

#ifdef SD_PLAYER_SHELL_SUPPORTED
const SHELL_COMMAND_STRUCT Shell_commands[] = {   
    { "cd",        Shell_cd },      
    { "del",       Shell_del },       
    { "dir",       Shell_dir },      
    { "exit",      Shell_exit }, 
    { "help",      Shell_help }, 
    { "mkdir",     Shell_mkdir },              
    { "ren",       Shell_rename },    
    { "rmdir",     Shell_rmdir },
    { "play",      Shell_play },
    { "dir_r",     Shell_dir_r},
    { "?",         Shell_command_list },     
    { NULL,        NULL } 
};
#endif


static uint_32 AccessroyHidEvent = 0;
static uint_32 BtAVRCPEvent = 0;

static uint_32 ModeSwitchEvent = 0;  // test0306


#define  ACCNEXTPLAY  (1u << 0)
#define  ACCPREVPLAY  (1u << 1)
#define  ACCPAUSEPLAY  (1u << 2)
#define  ACCRESUMEPLAY  (1u << 3)

extern void host_mode_switch_handle();
extern void dev_mode_switch_handle();

#define  MODESWITCH  (1u << 4)
void mode_switch_action_check(void)
{
   // if(ModeSwitchEvent & MODESWITCH)	
    {
        // if host pluged
    	 host_mode_switch_handle();
    	// if dev pluged
    	dev_mode_switch_handle();
    	//ModeSwitchEvent &= ~MODESWITCH;
    }
}

#define ADK_INTERNAL
#include "btAVRCP.h"
void bt_action_check(void)
{
#if 1
	if(BtAVRCPEvent & ACCNEXTPLAY)	
    {
    	avrcpServicePassthroughHandle(AVRCPCTNEXT,AVRCPCTPRESS);
    	BtAVRCPEvent &= ~ACCNEXTPLAY;
    
    }
    else  if(BtAVRCPEvent & ACCPREVPLAY)	
    {
    	avrcpServicePassthroughHandle(AVRCPCTPREVIOUS,AVRCPCTPRESS);
    	BtAVRCPEvent &= ~ACCPREVPLAY;
    }
    else  if(BtAVRCPEvent & ACCPAUSEPLAY)	
    {
    	avrcpServicePassthroughHandle(AVRCPCTPAUSE,AVRCPCTPRESS);
    	BtAVRCPEvent &= ~ACCPAUSEPLAY;
    }
    else  if(BtAVRCPEvent & ACCRESUMEPLAY)	
    {
    	avrcpServicePassthroughHandle(AVRCPCTPLAY,AVRCPCTPRESS);
    	BtAVRCPEvent &= ~ACCRESUMEPLAY;
    }
#endif
 }

#include "usb_host_aoa.h"
#ifdef  USE_ACC_HID
extern uint32_t AccessoryNextPlay(void);
extern uint32_t AccessoryPrevPlay(void);
extern uint32_t AccessoryPausePlay(void);
extern uint32_t AccessoryResumePlay(void);

void Accessory_action_check(void)
{
#if 1
	if(AccessroyHidEvent & ACCNEXTPLAY)	
    {
    	AccessoryNextPlay();
    	AccessroyHidEvent &= ~ACCNEXTPLAY;
    
    }
    else  if(AccessroyHidEvent & ACCPREVPLAY)	
    {
    	AccessoryPrevPlay();
    	AccessroyHidEvent &= ~ACCPREVPLAY;
    }
    else  if(AccessroyHidEvent & ACCPAUSEPLAY)	
    {
    	AccessoryPausePlay();
    	AccessroyHidEvent &= ~ACCPAUSEPLAY;
    }
    else  if(AccessroyHidEvent & ACCRESUMEPLAY)	
    {
    	AccessoryResumePlay();
    	AccessroyHidEvent &= ~ACCRESUMEPLAY;
    }
#endif
 }
#endif

void int_service_routine_btn_next(pointer text)              //TapKey 2
{  
//#if 0
	if((mp_sMicroSDplay == cur_musicPlayMS)||(mp_sMSDplay == cur_musicPlayMS))
	{
      _lwevent_set(&player_event, PLAYER_EVENT_MSK_NEXT_BTN_PRESSED);
      decoding = FALSE;
	}
//#else
	else if(mp_sAccessory == cur_musicPlayMS)
	{
      //AccessoryNextPlay();
		AccessroyHidEvent |= ACCNEXTPLAY;
		
	}
	else if(mp_sBT == cur_musicPlayMS)
	{
      //AccessoryNextPlay();
		BtAVRCPEvent |= ACCNEXTPLAY;
	}
//#endif
    hmi_setLed( 2, 0);   //TapKey 2 on
}

void int_service_routine_btn_prev(pointer text)             //TapKey 1
{   
//#if 0
	if((mp_sMicroSDplay == cur_musicPlayMS)||(mp_sMSDplay == cur_musicPlayMS))
	{
      _lwevent_set(&player_event, PLAYER_EVENT_MSK_PREV_BTN_PRESSED);
      decoding = FALSE;	 
	}
//#else
	else if(mp_sAccessory == cur_musicPlayMS)
	{
      //AccessoryPrevPlay();
		AccessroyHidEvent |= ACCPREVPLAY;
	}
	else if(mp_sBT == cur_musicPlayMS)
	{
      //AccessoryNextPlay();
		BtAVRCPEvent |= ACCPREVPLAY;
	}
//#endif
    hmi_setLed( 1, 0);   //TapKey 1 on
}

void int_service_routine_vol_up(pointer text)               //TapKey 3
{
	msi_snd_vol_up();
	hmi_setLed( 3, 0);   //TapKey 3 on
}

void int_service_routine_vol_down(pointer text)             //TapKey 4
{
	msi_snd_vol_down();
	hmi_setLed( 4, 0);   //TapKey 4 on
}
boolean pause_trigger = FALSE;
void int_service_routine_btn_pause(void *pin)                  //SW1 
{  
    lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR) pin);

    pause_trigger = !pause_trigger;
    if(TRUE == pause_trigger)            //pause a song
    	{
    	  if((mp_sMicroSDplay == cur_musicPlayMS)||(mp_sMSDplay == cur_musicPlayMS))
    	  {
    	    _lwevent_clear(&player_event, PLAYER_EVENT_MSK_SONG_RESUME);	
    	  }
    	  else if(mp_sAccessory == cur_musicPlayMS)
    	  {
    	    //AccessoryPausePlay();  
    		  AccessroyHidEvent |= ACCPAUSEPLAY;
    	  }
    	  else if(mp_sBT == cur_musicPlayMS)
    	  {
    		  BtAVRCPEvent |= ACCPAUSEPLAY;
    	  }
    	  printf("Paused\n");
    	}
    else                                //resume a song
    	{
    	 if((mp_sMicroSDplay == cur_musicPlayMS)||(mp_sMSDplay == cur_musicPlayMS))
    	  {
    	    _lwevent_set(&player_event, PLAYER_EVENT_MSK_SONG_RESUME);
    	  }
    	  else if(mp_sAccessory == cur_musicPlayMS)
    	  {
    	    //AccessoryResumePlay();  
    		  AccessroyHidEvent |= ACCRESUMEPLAY;
    	  }
    	  else if(mp_sBT == cur_musicPlayMS)
    	  {
    		  BtAVRCPEvent |= ACCRESUMEPLAY;			
    	  }
    	  printf("Resume\n");
    	}
    	
    //decoding = FALSE;
    //printf("SW1 pressed. %d \n", trigger);
}

void int_service_routine_btn_stop(void *pin)                   //SW2 
{   

    lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR) pin);
#if 0   
    //_lwevent_set(&player_event, PLAYER_EVENT_MSK_PREV_BTN_PRESSED);
    
    if(TRUE == trigger)
    	_lwevent_set(&player_event, PLAYER_EVENT_MSK_SONG_RESUME);    //if pause a song, resume firstly
    
    decoding = FALSE;	 
    
#endif
        if ((ModeSwitchEvent & MODESWITCH) == 0)
                ModeSwitchEvent |= MODESWITCH; 
        printf("Auto\n");


    //printf("SW2 pressed.\n");
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
#ifdef SD_PLAYER_SHELL_SUPPORTED
void Shell_task
(
    uint_32 temp
)
{ 
    /* Run the shell on the serial port */
    printf("Audio driver demo application\n");
    Shell(Shell_commands, NULL);
    _mqx_exit(0);
}
#endif

#define AUDIO_DIVECE_NAME_STR	"sai0:"
/*TASK*-----------------------------------------------------------------
*
* Function Name  : localPlay_init_task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
void localPlay_init_task
(
    uint_32 temp
)
{
    MQX_TICK_STRUCT time;
    _mqx_int errcode = 0;
   
    /* Install MQX default unexpected ISR routines */
    _int_install_unexpected_isr();
    
    if(_lwevent_create(&player_event, 0)!= MQX_OK) 
	{
      printf("\nMake event failed");
    }
    
    /* Setup time */
    printf("Setting up time......................");
    time.TICKS[0] = 0L;
    time.TICKS[1] = 0L;
    time.HW_TICKS = 0;
    _time_set_ticks(&time);
    printf("[OK]\n");
    
    /* Init GPIOs */    
    lwgpio_init(&btn_next, BSP_SW1, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
    lwgpio_set_functionality(&btn_next,BSP_SW1_MUX_GPIO);
    lwgpio_set_attribute(&btn_next, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
    lwgpio_int_init(&btn_next, LWGPIO_INT_MODE_RISING);
    _int_install_isr(lwgpio_int_get_vector(&btn_next), int_service_routine_btn_pause, (void *) &btn_next);
    hmi_init();
    sgtl5000_power_on();
    hmi_install(1, int_service_routine_btn_prev);
    
    _bsp_int_init(lwgpio_int_get_vector(&btn_next), BTN_ISR_PRIORITY, 0, TRUE);

    lwgpio_init(&btn_prev, BSP_SW2, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
    lwgpio_set_functionality(&btn_prev, BSP_SW2_MUX_GPIO);
    lwgpio_set_attribute(&btn_prev, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
    
#ifdef SD_DETECT_POLLING    
     lwgpio_int_init(&btn_prev, LWGPIO_INT_MODE_RISING);
    _int_install_isr(lwgpio_int_get_vector(&btn_prev), int_service_routine_btn_stop, (void *) &btn_prev);
    _bsp_int_init(lwgpio_int_get_vector(&btn_prev), BTN_ISR_PRIORITY, 0, TRUE);
#endif

    hmi_install(2, int_service_routine_btn_next);
    

    
    hmi_install(3, int_service_routine_vol_up);    
    hmi_install(4, int_service_routine_vol_down);
 
#if 0 
    /* Initialize audio codec */
    printf("Initializing audio codec.............");
    

    if (errcode != 0)
	{
        printf("[FAIL]\n");
        printf("  Error 0x%X\n", errcode);
    }
	else
    {
        printf("[OK]\n");
    }
    
//#else
    /* Initialize audio driver and codec */
 /************************************************/
	if ((errcode = msi_snd_init()) != 0)
	{
		printf("Initializing audio driver and codec........[FAIL]\n");   
		printf("  Error 0x%X\n", errcode);
		return;
	}
	else
	{
		printf("Initializing audio driver and codec........[OK]\n"); 
	}
/************************************************/     
#endif
    /* Create tasks */
    errcode = _task_create(0, SDCARD_TASK, 0);
    printf("Creating SD card task................");
    if (errcode == MQX_NULL_TASK_ID)
	{
        printf("[FAIL]\n");
        printf("  Error 0x%X.\n");
    }
	else
    {
        printf("[OK]\n");
    }
#ifdef SD_PLAYER_SHELL_SUPPORTED
    printf("Creating shell task..................");
    errcode = _task_create(0, SHELL_TASK, 0);
    if (errcode == MQX_NULL_TASK_ID)
	{
        printf("[FAIL]\n");
        printf("  Error 0x%X.\n");
    }
	else
    {
        printf("[OK]\n");
    }
#endif
    
    lwgpio_int_enable(&btn_next, TRUE);
    
#ifdef SD_DETECT_POLLING   
    lwgpio_int_enable(&btn_prev, TRUE);
#endif
    _task_abort(MQX_NULL_TASK_ID);
}

#if 1
void EXT_SDDETECT_ISR(void *pin)
{
    int sd_pin;
    boolean inserted;
   
   // if(sd_pin = lwgpio_int_get_flag((LWGPIO_STRUCT_PTR) pin)) {
        inserted = !lwgpio_get_value((LWGPIO_STRUCT_PTR) pin);

        // printf("sd detect pin changed %d,insert state %d,cur_musicPlayMS %d\n",sd_pin,inserted,cur_musicPlayMS);
        //if(cur_musicPlayMS != mp_sBT) {
            if(inserted)
                _lwevent_set(&sddetect_event,SD_ATTACHED_EVENT);
            else
                _lwevent_set(&sddetect_event,SD_DETTACHED_EVENT);
        //}
         lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR) pin);
    // }
}
#endif


MQX_FILE_PTR sdcard_handle, filesystem_handle, partman_handle;
    char         filesystem_name[] = "a:";
    char         partman_name[] = "pm:";

#if defined BSP_SDCARD_GPIO_DETECT
    LWGPIO_STRUCT      sd_detect;
#endif

int sd_mounted = 0;

int mount_sdcard()
{
    _mqx_uint    param;
    _mqx_int     error_code;
    //MQX_FILE_PTR com_handle, sdcard_handle, filesystem_handle, partman_handle;
    boolean readonly = /*FALSE*/TRUE;
    // char         filesystem_name[] = "a:";
    // char         partman_name[] = "pm:";

    printf("mounte sd card +\n");

    _lwsem_wait(&SD_MFS_IO_SEM);

    if(sd_mounted) {
        _lwsem_post(&SD_MFS_IO_SEM);
        return 0;
    }
    
    do {
    /* Open the device which MFS will be installed on */
    sdcard_handle = fopen("sdcard:", 0);
    if ( sdcard_handle == NULL ) {
        printf("Unable to open SD card device 2.\n");
        //_task_block();
        continue;          /* retry it */
    }
    
    /* Set read only flag as needed */
    param = 0;
    if (readonly)  {
            param = IO_O_RDONLY;
    }
    if (IO_OK != ioctl(sdcard_handle, IO_IOCTL_SET_FLAGS, (char_ptr) &param)) {
        printf("Setting device read only failed.\n");
        _task_block();
    }

    /* Install partition manager over SD card driver */
    error_code = _io_part_mgr_install(sdcard_handle, partman_name, 0);
    if (error_code != MFS_NO_ERROR) {
        printf("Error installing partition manager: %s\n", MFS_Error_text((uint_32)error_code));
        //_task_block();
        continue;       /* retry it */     
    }

    /* Open partition manager */
    partman_handle = fopen(partman_name, NULL);
    if (partman_handle == NULL) {
        error_code = ferror(partman_handle);
        printf("Error opening partition manager sd: %s\n", MFS_Error_text((uint_32)error_code));
        _task_block();
    }

    /* Validate partition 1 */
    param = 1;
    error_code = _io_ioctl(partman_handle, IO_IOCTL_VAL_PART, &param);
    if (error_code == MQX_OK) {
        /* Install MFS over partition 1 */
        error_code = _io_mfs_install(partman_handle, filesystem_name, param);
        if (error_code != MFS_NO_ERROR) {
            printf("Error initializing MFS over partition: %s\n", MFS_Error_text((uint_32)error_code));
            _task_block();
        }
    } else {
        /* Install MFS over SD card driver */
        error_code = _io_mfs_install(sdcard_handle, filesystem_name, (_file_size)0);
        if (error_code != MFS_NO_ERROR) {
            printf("Error initializing MFS: %s\n", MFS_Error_text((uint_32)error_code));
            _task_block();
        }
    }

    /* Open file system */
    filesystem_handle = fopen(filesystem_name, NULL);
    error_code = ferror (filesystem_handle);
    if ((error_code != MFS_NO_ERROR) && (error_code != MFS_NOT_A_DOS_DISK)) {
        printf("Error opening filesystem: %s\n", MFS_Error_text((uint_32)error_code));
        //_task_block();
        continue;                 /* retry it */
    }
    if ( error_code == MFS_NOT_A_DOS_DISK ) {
        printf("NOT A DOS DISK! You must format to continue.\n");
    }

    printf ("\n  SD card installed to \"%s\"\n", filesystem_name);

    if (readonly) {
        printf ("SD card is locked (read only).\n");
    }

    mfs_dir_reset();
    }while(0);

     sd_mounted = 1;   
     printf("mounted sd card -\n");
    _lwsem_post(&SD_MFS_IO_SEM);

    return 0;
}

int unmount_sdcard()
{
    _mqx_uint    param;
    _mqx_int     error_code;

    boolean readonly = FALSE;
    char         filesystem_name[] = "a:";
    char         partman_name[] = "pm:";

    printf("unmounte sd card +\n");
    _lwsem_wait(&SD_MFS_IO_SEM);

    if( ! sd_mounted) {
        _lwsem_post(&SD_MFS_IO_SEM);
        return 0;
    }

    /* Close the filesystem */
    if (MQX_OK != fclose (filesystem_handle)) {
        printf("Error closing filesystem.\n");
        _task_block();
    }
    filesystem_handle = NULL;

    /* Uninstall MFS  */
    error_code = _io_dev_uninstall(filesystem_name);
    if (error_code != MFS_NO_ERROR) {
        printf("Error uninstalling filesystem.\n");
        _task_block();
    }

    /* Close partition manager */
    if (MQX_OK != fclose (partman_handle)) {
        printf("Unable to close partition manager.\n");
        _task_block();
    }
    partman_handle = NULL;

    /* Uninstall partition manager  */
    error_code = _io_dev_uninstall(partman_name);
    if (error_code != MFS_NO_ERROR) {
        printf("Error uninstalling partition manager.\n");
        _task_block();
    }

    /* Close the SD card device */
    if (MQX_OK != fclose (sdcard_handle)) {
        printf("Unable to close SD card device.\n");
        _task_block();
    }
    sdcard_handle = NULL;

    sd_mounted = 0;
    printf("unmounted sd card -\n");
    _lwsem_post(&SD_MFS_IO_SEM);

    return 0;
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Sdcard_task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
void Sdcard_task
(
    uint_32 temp
)
{
    boolean      inserted = TRUE, last = FALSE;

    _mqx_int     error_code;
    
    MQX_FILE_PTR com_handle;

//#if defined BSP_SDCARD_GPIO_DETECT
//    LWGPIO_STRUCT      sd_detect;
//#endif
#if defined BSP_SDCARD_GPIO_PROTECT
    LWGPIO_STRUCT      sd_protect;
#endif

#ifdef BSP_SDCARD_GPIO_CS

    LWGPIO_STRUCT          sd_cs;
    SPI_CS_CALLBACK_STRUCT callback;

#endif
    _task_id player_task_id, sd_walker_id;
    
    _mqx_int        sd_event_value;
    _mqx_uint       wait_state;
    
#ifdef USB_ACCESSORY_PLAY
    connect_msg_t msg;
    int delaySetp = 0;
#endif
    
       if (MQX_OK !=_lwevent_create(&(sddetect_event), LWEVENT_AUTO_CLEAR)) {
		printf("\n_lwevent_create sddetect_event failed\n");
             _task_block();
	}

    /* Open low level communication device */
    com_handle = fopen (SDCARD_COM_CHANNEL, NULL);

    if (NULL == com_handle)
    {
        printf("Error installing communication handle.\n");
        _task_block();
    }

#ifdef BSP_SDCARD_GPIO_CS

    /* Open GPIO file for SPI CS signal emulation */
    error_code = lwgpio_init(&sd_cs, BSP_SDCARD_GPIO_CS, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE);
    if (!error_code)
    {
        printf("Initializing GPIO with associated pins failed.\n");
        _task_block();
    }
    lwgpio_set_functionality(&sd_cs,BSP_SDCARD_CS_MUX_GPIO);
    lwgpio_set_attribute(&sd_cs, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
    /* Set CS callback */
    callback.MASK = BSP_SDCARD_SPI_CS;
    callback.CALLBACK = set_CS;
    callback.USERDATA = &sd_cs;
    if (SPI_OK != ioctl (com_handle, IO_IOCTL_SPI_SET_CS_CALLBACK, &callback))
    {
        printf ("Setting CS callback failed.\n");
        _task_block();
    }

#endif


#if defined BSP_SDCARD_GPIO_DETECT
    /* Init GPIO pins for other SD card signals */
    error_code = lwgpio_init(&sd_detect, BSP_SDCARD_GPIO_DETECT, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
    if (!error_code)
    {
        printf("Initializing GPIO with sdcard detect pin failed.\n");
        _task_block();
    }
    /*Set detect and protect pins as GPIO Function */
    lwgpio_set_functionality(&sd_detect,BSP_SDCARD_DETECT_MUX_GPIO);
    lwgpio_set_attribute(&sd_detect, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);

#ifndef SD_DETECT_POLLING // init sd detcet pin interrupt
    lwgpio_int_init(&sd_detect,LWGPIO_INT_MODE_RISING | LWGPIO_INT_MODE_FALLING /* LWGPIO_INT_MODE_HIGH*/);     /* falling,raising mode = 3 */

    /* install gpio interrupt service routine */
    _int_install_isr(lwgpio_int_get_vector(&sd_detect), EXT_SDDETECT_ISR, (void *) &sd_detect);
    _bsp_int_init(lwgpio_int_get_vector(&sd_detect), 5, 0, TRUE);

    lwgpio_int_enable(&sd_detect, TRUE);
#endif

#endif

#if defined BSP_SDCARD_GPIO_PROTECT
    /* Init GPIO pins for other SD card signals */
    error_code = lwgpio_init(&sd_protect, BSP_SDCARD_GPIO_PROTECT, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
    if (!error_code)
    {
        printf("Initializing GPIO with sdcard protect pin failed.\n");
        _task_block();
    }
    /*Set detect and protect pins as GPIO Function */
    lwgpio_set_functionality(&sd_protect,BSP_SDCARD_PROTECT_MUX_GPIO);
    lwgpio_set_attribute(&sd_protect, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif

    /* Install SD card device */
    error_code = _io_sdcard_install("sdcard:", (pointer)&_bsp_sdcard0_init, com_handle);
    if ( error_code != MQX_OK )
    {
        printf("Error installing SD card device (0x%x)\n", error_code);
        _task_block();
    }

     _lwevent_set(&sddetect_event,SD_ATTACHED_EVENT); // set attached event at mode manager

    _time_delay(1000);   /* wait otg main task ready */
     printf("start sd card task\n");
    
    // use sd detect interrupt
    for (;;) {
#ifdef SD_DETECT_POLLING        
        wait_state = _lwevent_wait_ticks(&sddetect_event,SD_EVENT_MASK, FALSE, 4/* 0*/);
#else
        wait_state = _lwevent_wait_ticks(&sddetect_event,SD_EVENT_MASK, FALSE,   0);
#endif
        
        //if (wait_state == LWEVENT_WAIT_TIMEOUT/* MQX_OK*/) {
        if (wait_state !=  MQX_OK ) {
#ifndef SD_DETECT_POLLING            
            printf("waiting sddetect_event fail\n");
           // _task_block(); // _lwevent_destroy(&sddetect_event);
           //-goto wait_timeout;
           continue;
#else           
           _lwevent_set(&sddetect_event,SD_ATTACHED_EVENT); 
#endif
        }
        //else

        sd_event_value = _lwevent_get_signalled();
        
        if (sd_event_value == SD_ATTACHED_EVENT ) {
             _time_delay (200);
            inserted = !lwgpio_get_value(&sd_detect);
            if(!inserted)   // mount sd fs ,must attached sd card !
                continue;

            // printf("mount sd card...\n");
            // mount_sdcard();

#ifndef USB_ACCESSORY_PLAY         
            /* create player and sd_walker task*/
            player_task_id = _task_create(0, PLAYER_TASK, 0);
            printf("Creating sd player task................");
            if (player_task_id == MQX_NULL_TASK_ID) {
                printf("[FAIL]\n");
            }
            else {
                printf("[OK]\n");
            }

            sd_walker_id = _task_create(0, SD_WALKER_TASK, 0);
            printf("Creating sd walker task................");
            if (sd_walker_id == MQX_NULL_TASK_ID) {
                printf("[FAIL]\n");
            }
            else {
                printf("[OK]\n");
            }
#else
            msg.conct_source = mp_for_TF;
            msg.conct_action = mp_plugIn;     /* post message,  TFcard plug in*/
            if (LWMSGQ_FULL == _lwmsgq_send(connect_taskq, (uint_32 *) &msg, 0)) {
                printf("Could not inform  about TFCard device attached\n");
            }
            //_time_delay (1); // give mode manager task some times to cancel play ,if sd task high than mode task

#endif             
            // _lwevent_set(&player_event, PLAYER_EVENT_MSK_SD_FS_MOUNTED);     //auto play event
            last = inserted;

        } // SD_ATTACHED_EVENT
        else if (sd_event_value == SD_DETTACHED_EVENT ) {
            // _time_delay (100);
            //inserted = !lwgpio_get_value(&sd_detect);
            //if(inserted)
            //    continue;

            
#ifndef USB_ACCESSORY_PLAY
            _lwevent_set(&player_event, PLAYER_EVENT_MSK_SD_FS_UNMOUNTED);
            _lwevent_wait_ticks(&player_event, 
            PLAYER_EVENT_MSK_PLAYER_TASK_KILLED, 
            TRUE, 0);
            _lwevent_clear(&player_event, PLAYER_EVENT_MSK_PLAYER_TASK_KILLED);
            /* And the destroy play_task and sd_walker task */
            _task_destroy(sd_walker_id);
            _task_destroy(player_task_id);
#else
            /* post message,  TFcard plug out*/
            msg.conct_source = mp_for_TF;
            msg.conct_action = mp_plugOut;     /* post message,  TFcard plug out*/
            if (LWMSGQ_FULL == _lwmsgq_send(connect_taskq, (uint_32 *) &msg, 0))  {
                printf("Could not inform  about TFCard device de-attached\n");
            }
            //_time_delay (1); // give mode manager task some times to cancel play ,if sd task high than mode task

#endif 	
            // printf("unmount sd card...\n");
            // unmount_sdcard();
            // printf ("SD card uninstalled.\n");
        }
       
    }
    
}

void user_key_polling(void)
{
#ifndef SD_DETECT_POLLING
	static  LWGPIO_VALUE gpio_cur = LWGPIO_VALUE_NOCHANGE, gpio_last = LWGPIO_VALUE_NOCHANGE;
#endif
	
#ifdef USB_ACCESSORY_PLAY     
        hmi_setLed( 1, 1);
        hmi_setLed( 2, 1);
        hmi_setLed( 3, 1);
        hmi_setLed( 4, 1); 
#endif

#ifndef SD_DETECT_POLLING
        gpio_cur = lwgpio_get_value(&btn_prev);
        if((gpio_cur == LWGPIO_VALUE_LOW)&&(gpio_last == LWGPIO_VALUE_HIGH))
        {
           _int_disable();
           mode_switch_action_check();  
           _int_enable();
        }
        gpio_last = gpio_cur;   
#endif         

       if(0 != AccessroyHidEvent) {
#ifdef  USE_ACC_HID            		
        Accessory_action_check();
#endif            		
       }
       if(0 != BtAVRCPEvent ){
        bt_action_check();
       }
             
       hmi_polling();  /* at this pointer ,there must be  high priority task  is ready ,otherwise polling can't exist !!!  */

}

/* EOF */
