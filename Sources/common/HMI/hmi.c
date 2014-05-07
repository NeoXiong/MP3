/**HEADER********************************************************************
*
* Copyright (c) 2008-2009 Freescale Semiconductor;
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
* $FileName: hmi.c$
* $Version : 3.8.1.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   This file contains the source for the HMI example program.
*
*END************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include<cstdint>

#ifndef HMI_DEBUG
#define HMI_DEBUG       0
#else 
#undef HMI_DEBUG
#define HMI_DEBUG       0
#endif

#if HMI_DEBUG
#define HMI_LOG(...)  \
        {   \
            printf("\nI2S_LOG: %s: %d", __FILE__, __LINE__);  \
            printf(__VA_ARGS__);    \
        }
#else
#define HMI_LOG(...) {}
#endif

#define MAX_TOUCH_PAD  4

#if !BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#if 	(MQX_CPU == PSP_CPU_MK70F120M || MQX_CPU == PSP_CPU_MK60D100M)
static HMI_CLIENT_STRUCT_PTR bsp_btnled_driver_handle;    /* BTNLED handle pointer */
#endif



int8_t hmi_init(void)
{
#if 	(MQX_CPU == PSP_CPU_MK70F120M || MQX_CPU == PSP_CPU_MK60D100M)
	 bsp_btnled_driver_handle = _bsp_btnled_init(); /* BTNLED initialization */ 
	 
	 btnled_set_value(bsp_btnled_driver_handle, HMI_LED_1, HMI_VALUE_OFF);
	 btnled_set_value(bsp_btnled_driver_handle, HMI_LED_2, HMI_VALUE_OFF);
	 btnled_set_value(bsp_btnled_driver_handle, HMI_LED_3, HMI_VALUE_OFF);
	 btnled_set_value(bsp_btnled_driver_handle, HMI_LED_4, HMI_VALUE_OFF);

	 if (bsp_btnled_driver_handle == NULL)  
	 {
	        /* error in initialization */
		 	 HMI_LOG("BTNLED initialization failed. \n");
	        return -1;
	 }
	 return 0;
#endif
}

int8_t hmi_install(int32_t btnNum, void (*callBack)(void *))
{ 
#if 	(MQX_CPU == PSP_CPU_MK70F120M || MQX_CPU == PSP_CPU_MK60D100M)
	 if (bsp_btnled_driver_handle == NULL)  
	 {
			 
		 	 HMI_LOG("Init HMI first\n");
			return -1;
	 }
	
	if(btnNum > MAX_TOUCH_PAD || btnNum < 1)	
	{
		HMI_LOG("Unable to install btnled callback: button number %d illegal ",btnNum);
		return -1;
	}
	

	switch(btnNum)
	{
	case 1:
		 btnled_add_clb(bsp_btnled_driver_handle, HMI_BUTTON_1, HMI_VALUE_PUSH, callBack, "button 1");
		 //printf("Button 1 callback installed\n");
		break;
	case 2:
	 
		 btnled_add_clb(bsp_btnled_driver_handle, HMI_BUTTON_2, HMI_VALUE_PUSH, callBack, "button 2");
		 //printf("Button 2 callback installed\n");

		break;
	case 3:
		 
		 btnled_add_clb(bsp_btnled_driver_handle, HMI_BUTTON_3, HMI_VALUE_PUSH, callBack, "button 3");
		 //printf("Button 3 callback installed\n");

		break;
	case 4:
		 
		 btnled_add_clb(bsp_btnled_driver_handle, HMI_BUTTON_4, HMI_VALUE_PUSH, callBack, "button 4");
		 //printf("Button 4 callback installed\n");

		break;
	default:
		HMI_LOG("Registering: Unrecognized button number\n");

		return -4;
		break;
	}
	
    
#if 0
    /* set LED ON */
    btnled_set_value(bsp_btnled_driver_handle, HMI_LED_1, HMI_VALUE_ON);
    btnled_set_value(bsp_btnled_driver_handle, HMI_LED_2, HMI_VALUE_ON);

    /* add callbacks for PUSH/RELEASE/ROTARY MOVEMENT */
    btnled_add_clb(bsp_btnled_driver_handle, HMI_BUTTON_1, HMI_VALUE_PUSH, btn1Callback, "button 1");
    btnled_add_clb(bsp_btnled_driver_handle, HMI_BUTTON_2, HMI_VALUE_PUSH, btn2Callback, "button 2");
    btnled_add_clb(bsp_btnled_driver_handle, HMI_BUTTON_3, HMI_VALUE_PUSH, btn3Callback, "button 3");
  //  btnled_add_clb(bsp_btnled_driver_handle, HMI_BUTTON_3, HMI_VALUE_RELEASE, callback_print_release, "button 3");
    btnled_add_clb(bsp_btnled_driver_handle, HMI_BUTTON_4, HMI_VALUE_PUSH, btn4Callback, "button 4");
#endif    

    return 0;
#endif
}

void hmi_polling(void)
{
#if 	(MQX_CPU == PSP_CPU_MK70F120M || MQX_CPU == PSP_CPU_MK60D100M)

   btnled_poll(bsp_btnled_driver_handle); /* polls TSS and LWGPIO buttons */
   return;
#endif
}
/*
 * value 1 off, 0 on
 */
void hmi_setLed(uint8_t num, uint8_t value)
{
#if 	(MQX_CPU == PSP_CPU_MK70F120M || MQX_CPU == PSP_CPU_MK60D100M)
	switch(num)
	{
	case 1:
		btnled_set_value(bsp_btnled_driver_handle, HMI_LED_1, value != 0/*HMI_VALUE_OFF*/);
		break;
	case 2:
		btnled_set_value(bsp_btnled_driver_handle, HMI_LED_2, value != 0);
		break;
	case 3:
		btnled_set_value(bsp_btnled_driver_handle, HMI_LED_3, value != 0);
		break;
	case 4:
		btnled_set_value(bsp_btnled_driver_handle, HMI_LED_4, value != 0);
		break;
	default:
		break;
	}
#endif
	 
}

