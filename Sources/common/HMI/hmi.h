#ifndef _HMI_
#define _HMI_
/*
 * init human machine interface including touch pad and led
 */
int8_t hmi_init(void);
/*
 * btnNum: 1-4, indicating touch pad 1, 2, 3, 4
 * callback: corresponding push operation call back
 */
int8_t hmi_install(int32_t btnNum, void (*)(void *));
/*
 * the call backs calling need the hmi_polling to trigger
 */
void hmi_polling(void);
/*
 * value 0 on, non 0 off
 */
void hmi_setLed(uint8_t num, uint8_t value);
#endif
