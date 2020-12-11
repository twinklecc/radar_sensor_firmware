#include <string.h>
#include "embARC_toolchain.h"
#include "FreeRTOS.h"
#include "task.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "dw_gpio.h"
#include "gpio_hal.h"
#include "spi_hal.h"

#define SPIS_SERVER_S2M_SYNC_GPIO_ID	(3)

static uint32_t s2m_sync_sts = 0;

int32_t spis_server_s2m_sync(void)
{
	int32_t result = E_OK;

	chip_hw_udelay(50);
	//uint32_t cpu_sts = arc_lock_save();
	result = gpio_write(SPIS_SERVER_S2M_SYNC_GPIO_ID, s2m_sync_sts & 0x1);
	if (E_OK == result) {
		s2m_sync_sts = s2m_sync_sts + 1;
	} else {
		/* TODO: */
	}
	//arc_unlock_restore(cpu_sts);

	return result;
}

int32_t spis_server_sync_init(void)
{
	int32_t result = E_OK;

	result = gpio_set_direct(SPIS_SERVER_S2M_SYNC_GPIO_ID, \
			DW_GPIO_DIR_OUTPUT);
	if (E_OK == result) {
		result = spis_server_s2m_sync();
	}

	return result;
}
