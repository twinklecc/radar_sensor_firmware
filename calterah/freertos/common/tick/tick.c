#include "embARC.h"
#include "embARC_debug.h"
#include "embARC_assert.h"
#include "baseband_cli.h"
#include "tick.h"
#include "dw_gpio.h"
#include "gpio_hal.h"
#include "alps_dmu_reg.h"


static void switch_led(uint32_t status)
{
	int32_t ret = E_OK;

	if (CHK_DMU_MODE == 1) {
		if (io_get_dmumode() < 0)
			return;
	}

	ret = gpio_set_direct(LED_D2, DW_GPIO_DIR_OUTPUT);
	if (ret != 0) {
		EMBARC_PRINTF("Dir gpio(%d) error! ret %d \n", LED_D2, ret);
	}

	ret = gpio_write(LED_D2, status);
	if (ret != 0) {
		EMBARC_PRINTF("write gpio(%d) error! ret %d \n", LED_D2, ret);
	}
}


void vApplicationTickHook( void )
{
	static uint32_t tick = 0;
	static uint8_t pv = 0;

	tick++;
	if ((tick * portTICK_PERIOD_MS) > LED_FLICKER_MS) {
		if (pv == 0) {
			switch_led(LED_OFF);
			pv = 1;
		} else {
			switch_led(LED_ON);
			pv = 0;
		}
		tick = 0;
	}
}
