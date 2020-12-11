#include <string.h>
#include "embARC_toolchain.h"
#include "FreeRTOS.h"
#include "task.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "dw_gpio.h"
#include "gpio_hal.h"
#include "spi_hal.h"
#include "crc_hal.h"
#include "cascade_config.h"
#include "cascade.h"
#include "cascade_internal.h"
#include "dma_hal.h"

void cascade_crc_dmacallback(void *params)
{

	/* read crc32. */
	//uint32_t crc32 = crc_output();
	uint32_t crc32 = crc_init_value();
	EMBARC_PRINTF("dma isr callback: 0x%x\r\n", crc32);
	/* set crc_valid flag. */
	cascade_crc_compute_done(crc32);
}

int32_t cascade_crc_dmacopy(uint32_t *data, uint32_t len)
{
	int32_t result = E_OK;

	uint32_t chn_id = 0;

	dma_trans_desc_t desc = {
		.transfer_type = MEM_TO_MEM,
		.priority = DMA_CHN_PRIOR_MAX,
		.src_desc.addr_mode = ADDRESS_INCREMENT,
		.src_desc.hw_if = 0,
		.src_desc.sts_upd_en = 0,
		.src_desc.hs = HS_SELECT_SW,
		.src_desc.sts_addr = 0,
		.src_desc.burst_tran_len = BURST_LEN1,
		.src_desc.addr = (uint32_t)data,
		.dst_desc.addr_mode = ADDRESS_FIXED,
		.dst_desc.hw_if = 0,
		.dst_desc.sts_upd_en = 0,
		.dst_desc.hs = HS_SELECT_SW,
		.dst_desc.sts_addr = 0,
		.dst_desc.burst_tran_len = BURST_LEN1,
		.dst_desc.addr = 0xc10008,

		.block_transfer_size = len << 2
	};

	do {
		if ((NULL == data) || (0 == len)) {
			result = E_PAR;
			break;
		}

		hw_crc_reset(1);
		hw_crc_reset(0);

		result = dma_apply_channel();
		if (result >= 0) {
			chn_id = (uint32_t)result;
		} else {
			break;
		}

		result = dma_config_channel(chn_id, &desc, 1);
		if (E_OK != result) {
			result = dma_release_channel(chn_id);
			break;
		}

		result = dma_start_channel(chn_id, cascade_crc_dmacallback);
		if (E_OK != result) {
			break;
		}

	} while (0);

	if (E_OK == result) {
		result = (int32_t)chn_id;
	}

	return result;
}
