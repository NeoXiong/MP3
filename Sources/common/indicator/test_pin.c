

#include "mqx.h"
#include "bsp.h"
#include "lwgpio.h"

#include "test_pin.h"

static LWGPIO_STRUCT test_pin;
static int32_t test_pin_inited = 0;

void init_test_pin(uint_8 init_val)
{
    if (!lwgpio_init(&test_pin, BSP_TEST_PING, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE)) {
        printf("Initializing test pin GPIO as output failed.\n");
        //_task_block();
    }
    lwgpio_set_functionality(&test_pin, BSP_TEST_PING_MUX_GPIO);

lwgpio_set_value(&test_pin,/*init_val*/ 0);
	  
    test_pin_inited = 1;
}

void set_test_pin(void)
{
  if(test_pin_inited) {
    lwgpio_set_value(&test_pin,LWGPIO_VALUE_HIGH ); /* set pin to 1 */
  }
}

void clear_test_pin(void)
{
  if(test_pin_inited) {
    lwgpio_set_value(&test_pin, LWGPIO_VALUE_LOW); /* set pin to 0 */
  }
}

