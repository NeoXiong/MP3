#ifndef __test_pin_h__
#define __test_pin_h__

#if 0
#define BSP_TEST_PING        (GPIO_PORT_A | GPIO_PIN14)
#define BSP_TEST_PING_MUX_GPIO LWGPIO_MUX_A14_GPIO
#else
#define BSP_TEST_PING        (GPIO_PORT_B | GPIO_PIN9)
#define BSP_TEST_PING_MUX_GPIO 1
#endif

void init_test_pin(uint8_t init_val);
void set_test_pin(void);
void clear_test_pin(void);

#endif
