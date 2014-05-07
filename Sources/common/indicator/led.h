/*
 * led.h
 *
 *  Created on: Jul 9, 2013
 *      Author: B46407
 */

#ifndef LED_H_
#define LED_H_

#include<mqx.h>
#include"bsp.h" 

void led_init(void);
void light_led(int_8 ledNum);
void light_down_led(int_8 ledNum);


#endif /* LED_H_ */
