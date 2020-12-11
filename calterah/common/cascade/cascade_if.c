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

#define CASCADE_IF_BAUD			(50000000)
#define CASCADE_SPIM_RX_SAMPLE_DELAY	(3)


static spi_xfer_desc_t cs_spi_xfer_desc = {
		.clock_mode = SPI_CLK_MODE_0,
		.dfs = 32,
		.cfs = 0,
		.spi_frf = SPI_FRF_STANDARD,
		.rx_thres = 0,
		.tx_thres = 0,
		.rx_sample_delay = CASCADE_SPIM_RX_SAMPLE_DELAY
};

static cs_com_cfg_t cs_com_cfg = {
	.tx_mode = 0,
	.rx_mode = 0,
	.xfer_desc = (void *)&cs_spi_xfer_desc
};

int32_t cascade_if_init(void)
{
	int32_t result = E_OK;

	do {
		int32_t cascade_status = chip_cascade_status();
		if (cascade_status < 0) {
			result = cascade_status;
			break;
		}

		if (CHIP_CASCADE_MASTER == cascade_status) {
			cs_com_cfg.hw_if_id = CHIP_CASCADE_SPI_M_ID;
			result = cadcade_if_master_init(CHIP_CASCADE_SPI_M_ID);
		} else {
			cs_com_cfg.hw_if_id = CHIP_CASCADE_SPI_S_ID;
			result = cadcade_if_slave_init(CHIP_CASCADE_SPI_S_ID);
		}
		if (E_OK != result) {
			EMBARC_PRINTF("cascade interface init failed[%d]\r\n", result);
			break;
		}

		result = spi_open(cs_com_cfg.hw_if_id, CASCADE_IF_BAUD);
		if (E_OK != result) {
			break;
		}
		result = spi_transfer_config(cs_com_cfg.hw_if_id, &cs_spi_xfer_desc);
		if (E_OK != result) {
			break;
		}

		if (CHIP_CASCADE_SLAVE == cascade_status) {
			result = spi_interrupt_install(cs_com_cfg.hw_if_id, \
					0, cascade_if_callback);
			if (E_OK != result) {
				break;
			}
			result = spi_interrupt_enable(cs_com_cfg.hw_if_id, 0, 1);
			if (E_OK != result) {
				break;
			}

			result = spi_interrupt_install(cs_com_cfg.hw_if_id, \
					1, cascade_if_callback);
		}

	} while (0);

	return result;
}


void cascade_rx_crc_handle(cs_xfer_mng_t *xfer, uint32_t crc32)
{
	int32_t result = spi_device_mode(cs_com_cfg.hw_if_id);
	if (DEV_MASTER_MODE == result) {
		cascade_if_master_rx_crc_done(xfer, crc32);
	} else if (DEV_SLAVE_MODE == result) {
		cascade_if_slave_rx_crc_done(xfer, crc32);
	} else {
		/* panic: lower layer error! */
	}
}

void cascade_tx_crc_handle(cs_xfer_mng_t *xfer, uint32_t crc32)
{
	int32_t result = spi_device_mode(cs_com_cfg.hw_if_id);
	if (DEV_MASTER_MODE == result) {
		cascade_if_master_tx_crc_done(xfer, crc32);
	} else if (DEV_SLAVE_MODE == result) {
		cascade_if_slave_tx_crc_done(xfer, crc32);
	} else {
		/* panic: lower layer error! */
	}
}

void cascade_if_xfer_resume(cs_xfer_mng_t *xfer, uint32_t length)
{
	int32_t result = spi_device_mode(cs_com_cfg.hw_if_id);
	if (DEV_MASTER_MODE == result) {
		cascade_if_master_xfer_resume(xfer, 0);
	} else if (DEV_SLAVE_MODE == result) {
		cascade_if_slave_xfer_resume(xfer, length);
	} else {
	}
}

void cascade_sync_bb_init(void)
{
        if (chip_cascade_status() == CHIP_CASCADE_MASTER)
                cascade_if_master_sync_bb_init();
        else
                cascade_if_slave_sync_bb_init();
}
