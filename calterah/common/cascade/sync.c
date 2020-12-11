#include <string.h>
#include "embARC_toolchain.h"
#include "FreeRTOS.h"
#include "task.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "dw_gpio.h"
#include "gpio_hal.h"
#include "spi_hal.h"
#include "cascade_config.h"
#include "cascade.h"
#include "cascade_internal.h"
#include "baseband.h"

#define GPIO_CASCADE_S2M_SYNC	(7)
#define GPIO_CASCADE_S2M_SYNC_BB    (8)

static uint32_t s2m_sync_sts = 0;
static uint32_t s2m_sync_bb_sts = 0;
static uint32_t chip_cascade = CHIP_CASCADE_MASTER;
static uint32_t cascade_status_inited = 0;

static QueueHandle_t sync_queue_handle = NULL;
static TaskHandle_t s2m_sync_handle = NULL;


static void cascade_s2m_sync_task(void *params);

int32_t chip_cascade_status(void)
{
	int32_t result = chip_cascade;

	while (0 == cascade_status_inited) {
		uint32_t loop_cnt, value = 0, cnt = 0;

		io_mux_adc_sync_disable();

		result = gpio_set_direct(CHIP_CASCADE_SYNC_M, DW_GPIO_DIR_OUTPUT);
		if (E_OK != result) {
			break;
		}

		result = gpio_set_direct(CHIP_CASCADE_SYNC_S, DW_GPIO_DIR_INPUT);
		if (E_OK != result) {
			break;
		}

		loop_cnt = 3;
		while (loop_cnt--) {
			result = gpio_write(CHIP_CASCADE_SYNC_M, value);
			if (E_OK != result) {
				break;
			} else {
				chip_hw_udelay(10);
			}

			result = gpio_read(CHIP_CASCADE_SYNC_S);
			if (value != result) {
				chip_cascade = CHIP_CASCADE_SLAVE;
				result = chip_cascade;
				break;
			} else {
				value = !value;
				cnt++;
			}
		}

		if (3 == cnt) {
			chip_cascade = CHIP_CASCADE_MASTER;
			result = chip_cascade;
		}

		cascade_status_inited = 1;
		io_mux_adc_sync();
	}

	return result;
}


int32_t cascade_spi_s2m_sync(void)
{
	int32_t result = E_OK;

	result = gpio_write(GPIO_CASCADE_S2M_SYNC, s2m_sync_sts);
	if (E_OK == result) {
		s2m_sync_sts = !s2m_sync_sts;
	}

	return result;
}

int32_t cascade_if_slave_sync_init(void)
{
	int32_t result = E_OK;

	result = gpio_set_direct(GPIO_CASCADE_S2M_SYNC, \
			DW_GPIO_DIR_OUTPUT);
	if (E_OK == result) {
		result = cascade_spi_s2m_sync();
	}

	return result;
}


static void cascade_s2m_sync_isr(void *params)
{
	BaseType_t higher_task_wkup = 0;
	uint32_t msg = 1;

	if (pdTRUE == xQueueSendFromISR(sync_queue_handle, \
				&msg, &higher_task_wkup)) {
		if (higher_task_wkup) {
			portYIELD_FROM_ISR();
		}
	} else {
		/* TODO: record error! */
		;
	}
}

int32_t cascade_if_master_sync_init(void)
{
	int32_t result = E_OK;

	do {
		result = gpio_set_direct(GPIO_CASCADE_S2M_SYNC, \
			DW_GPIO_DIR_INPUT);
		if (E_OK != result) {
			break;
		}

		result = gpio_int_register(GPIO_CASCADE_S2M_SYNC, \
			cascade_s2m_sync_isr, \
			GPIO_INT_BOTH_EDGE_ACTIVE);
		if (E_OK != result) {
			break;
		}

		sync_queue_handle = xQueueCreate(CASCADE_QUEUE_LEN, 4);
		if (NULL == sync_queue_handle) {
			EMBARC_PRINTF("Error: cascade queue create failed.\r\n");
			result = E_SYS;
			break;
		}

		if (pdPASS != xTaskCreate(cascade_s2m_sync_task, "s2m_sync_task",
				128, (void *)0, configMAX_PRIORITIES - 1,
				&s2m_sync_handle)) {
			EMBARC_PRINTF("create cs_xfer_handle error\r\n");
			result = E_SYS;
		}

	} while (0);

	return result;
}

// sync_bb
int32_t cascade_s2m_sync_bb(void)
{
	int32_t result = E_OK;

	result = gpio_write(GPIO_CASCADE_S2M_SYNC_BB, s2m_sync_bb_sts);
	if (E_OK == result)
		s2m_sync_bb_sts = !s2m_sync_bb_sts;

	return result;
}


int32_t cascade_if_slave_sync_bb_init(void)
{
	int32_t result = E_OK;

	result = gpio_set_direct(GPIO_CASCADE_S2M_SYNC_BB, \
			DW_GPIO_DIR_OUTPUT);
	if (E_OK == result)
                result = cascade_s2m_sync_bb();

	return result;
}

int32_t cascade_if_master_sync_bb_init(void)
{
	int32_t result = E_OK;
	result = gpio_set_direct(GPIO_CASCADE_S2M_SYNC_BB, \
			DW_GPIO_DIR_INPUT);
	if (E_OK == result)
		result = gpio_int_register(GPIO_CASCADE_S2M_SYNC_BB, \
			baseband_cascade_sync_handler, \
			GPIO_INT_BOTH_EDGE_ACTIVE);

	return result;
}

static void cascade_s2m_sync_task(void *params)
{
	uint32_t msg = 0;
	while (1) {
		if (pdTRUE == xQueueReceive(sync_queue_handle, \
				&msg, portMAX_DELAY))
			cascade_sync_callback(NULL);
		else
			taskYIELD();
	}
}
