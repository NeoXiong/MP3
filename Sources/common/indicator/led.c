/*
 * led.c
 *
 *  Created on: Jul 9, 2013
 *      Author: B46407
 */
#include<mqx.h>
#include "bsp.h"
#include"led.h"

LWGPIO_STRUCT led1;
LWGPIO_STRUCT led2;
LWGPIO_STRUCT led3;
LWGPIO_STRUCT led4;
 void led_init(void)
{
	 
	 if (!lwgpio_init(&led1, BSP_LED1, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE)\
			 || !lwgpio_init(&led2, BSP_LED2, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE)\
			 ||!lwgpio_init(&led3, BSP_LED3, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE)	\
			 || !lwgpio_init(&led4, BSP_LED4, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
	      {
	          printf("Initializing LED GPIO as output failed.\n");
	         
	      }
	lwgpio_set_functionality(&led1, BSP_LED1_MUX_GPIO);
	lwgpio_set_functionality(&led2, BSP_LED1_MUX_GPIO);
	lwgpio_set_functionality(&led3, BSP_LED1_MUX_GPIO);
	lwgpio_set_functionality(&led4, BSP_LED1_MUX_GPIO);
	
	lwgpio_set_value(&led1, LWGPIO_VALUE_HIGH); /* set pin to 1 */
	lwgpio_set_value(&led2, LWGPIO_VALUE_HIGH); /* set pin to 1 */
	lwgpio_set_value(&led3, LWGPIO_VALUE_HIGH); /* set pin to 1 */
	lwgpio_set_value(&led4, LWGPIO_VALUE_HIGH); /* set pin to 1 */
}

 void light_led(int8_t ledNum)
{
	switch(ledNum)
	{
		case 0x0:
			lwgpio_set_value(&led1, LWGPIO_VALUE_HIGH); /* set pin to 1 */
			lwgpio_set_value(&led2, LWGPIO_VALUE_HIGH); /* set pin to 1 */
			lwgpio_set_value(&led3, LWGPIO_VALUE_HIGH); /* set pin to 1 */
			lwgpio_set_value(&led4, LWGPIO_VALUE_HIGH); /* set pin to 1 */
			break;
		case 0x01:
			lwgpio_set_value(&led1, LWGPIO_VALUE_LOW); /* set pin to 1 */
		
			break;
		case 0x02:
		
			lwgpio_set_value(&led2, LWGPIO_VALUE_LOW); /* set pin to 1 */
			
			break;
		case 0x04:
			
			lwgpio_set_value(&led3, LWGPIO_VALUE_LOW); /* set pin to 1 */
		
			break;
		case 0x08:
			
			lwgpio_set_value(&led4, LWGPIO_VALUE_LOW); /* set pin to 1 */
			break;
		case 0x03:
			lwgpio_set_value(&led1, LWGPIO_VALUE_LOW); /* set pin to 1 */
			lwgpio_set_value(&led2, LWGPIO_VALUE_LOW); /* set pin to 1 */
			lwgpio_set_value(&led3, LWGPIO_VALUE_HIGH); /* set pin to 1 */
			lwgpio_set_value(&led4, LWGPIO_VALUE_HIGH); /* set pin to 1 */
			break;
		case 0x07:
			lwgpio_set_value(&led1, LWGPIO_VALUE_LOW); /* set pin to 1 */
			lwgpio_set_value(&led2, LWGPIO_VALUE_LOW); /* set pin to 1 */
			lwgpio_set_value(&led3, LWGPIO_VALUE_LOW); /* set pin to 1 */
			lwgpio_set_value(&led4, LWGPIO_VALUE_HIGH); /* set pin to 1 */
			break;
		case 0x0f:
			lwgpio_set_value(&led1, LWGPIO_VALUE_LOW); /* set pin to 1 */
			lwgpio_set_value(&led2, LWGPIO_VALUE_LOW); /* set pin to 1 */
			lwgpio_set_value(&led3, LWGPIO_VALUE_LOW); /* set pin to 1 */
			lwgpio_set_value(&led4, LWGPIO_VALUE_LOW); /* set pin to 1 */
			break;	
		default:
			break;
	}
}
 void light_down_led(int8_t ledNum)
 {
 	switch(ledNum)
 	{
 		case 0x0f:
 			lwgpio_set_value(&led1, LWGPIO_VALUE_HIGH); /* set pin to 1 */
 			lwgpio_set_value(&led2, LWGPIO_VALUE_HIGH); /* set pin to 1 */
 			lwgpio_set_value(&led3, LWGPIO_VALUE_HIGH); /* set pin to 1 */
 			lwgpio_set_value(&led4, LWGPIO_VALUE_HIGH); /* set pin to 1 */
 			break;
 		case 0x01:
 			lwgpio_set_value(&led1, LWGPIO_VALUE_HIGH); /* set pin to 1 */
 		
 			break;
 		case 0x02:
 		
 			lwgpio_set_value(&led2, LWGPIO_VALUE_HIGH); /* set pin to 1 */
 			
 			break;
 		case 0x04:
 			
 			lwgpio_set_value(&led3, LWGPIO_VALUE_HIGH); /* set pin to 1 */
 		
 			break;
 		case 0x08:
 			
 			lwgpio_set_value(&led4, LWGPIO_VALUE_HIGH); /* set pin to 1 */
 			break;
 		
 		default:
 			break;
 	}
 }


