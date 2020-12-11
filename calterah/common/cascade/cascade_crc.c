#include <string.h>
#include "embARC_toolchain.h"
#include "FreeRTOS.h"
#include "task.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "dev_common.h"
#include "crc_hal.h"
#include "cascade_config.h"
#include "cascade.h"
#include "cascade_internal.h"


typedef struct cascade_crc_info {
	uint32_t *data;
	uint32_t length;
} cs_crc_info_t;

/* computing crc while not using dma to transfer data to CRC engineer. */
static TaskHandle_t cascade_crc_handle = NULL;

static cs_crc_info_t cs_last_frame_info;


/* note: this method is called in interrupt context. */
void cascade_crc_request(uint32_t *data, uint32_t length)
{
	cs_last_frame_info.data = data;
	cs_last_frame_info.length = length;
}

static void cascade_crc_rx_frame_update(cs_xfer_mng_t *xfer)
{
	int32_t result = E_OK;
	uint32_t crc32 = 0;
	if ((NULL != xfer) && (xfer->cur_frame.crc_valid)) {
		result = crc32_update(0, cs_last_frame_info.data,
					cs_last_frame_info.length);
		if (E_OK == result) {
			crc32 = crc_output();
			cascade_rx_crc_handle(xfer, crc32);
		}
	}
}

static void cascade_crc_tx_frame_update(cs_xfer_mng_t *xfer)
{
	int32_t result = E_OK;
	uint32_t crc32 = 0;
	if ((NULL != xfer) && (0 == xfer->cur_frame.crc_valid)) {
		result = crc32_update(0, cs_last_frame_info.data,
					cs_last_frame_info.length);
		if (E_OK == result) {
			crc32 = crc_output();
			cascade_tx_crc_handle(xfer, crc32);
		}
	}
}

void cascade_crc_task(void *params)
{
	int32_t result = E_OK;

	cs_xfer_mng_t *xfer = NULL;

	while (1) {
		xfer = NULL;
		result = cascade_in_xfer_session(&xfer);
		switch (result) {
			case CS_RX:
				cascade_crc_rx_frame_update(xfer);
				break;

			case CS_TX:
				cascade_crc_tx_frame_update(xfer);
				break;

			default:
				break;
		}
		taskYIELD();
	}
}

void cascade_crc_init(void)
{
	if (pdPASS != xTaskCreate(cascade_crc_task, "cs_crc_task",
			128, (void *)0, configMAX_PRIORITIES - 1,
			&cascade_crc_handle)) {
		EMBARC_PRINTF("create cs_xfer_handle error\r\n");
        }
}
