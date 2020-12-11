#include "embARC_toolchain.h"
#include "FreeRTOS.h"
#include "task.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "dw_gpio.h"
#include "gpio_hal.h"
#include "spi_hal.h"

#include "spis_server_internal.h"


#define SPIS_SERVER_SPI_ID			(2)
#define SPIS_SERVER_FRAME_LEN_MAX		(0x1000)
#define SPIS_SERVER_FRAME_MAGIC			(0x1234)
#define SPIS_SERVER_TX_CONFIRM_QUEUE_LEN	(1)

typedef enum {
	SPIS_XFER_INVALID = 0,
	SPIS_XFER_IDLE,
	SPIS_RX_ONGOING,
	SPIS_TX_ONGOING,
} spis_server_state_t;


typedef struct {
	spis_server_state_t xfer_state;

	DEV_BUFFER dev_txbuf;

	uint32_t crc_state;
	uint32_t frame_last_part;

	uint32_t tx_total_size;
	uint32_t tx_handled_size;
	uint32_t *txdata;

	QueueHandle_t tx_confirm_queue;
} spis_server_global_t;


static spis_server_global_t spis_server;

static spi_xfer_desc_t spis_xfer_desc = {
	.clock_mode = SPI_CLK_MODE_0,
	.dfs = 32,
	.cfs = 0,
	.spi_frf = SPI_FRF_STANDARD,
	.rx_thres = 0,
	.tx_thres = 0,
};


void spis_server_init(void)
{
	int32_t result = E_OK;

	do {
		result = spi_open(SPIS_SERVER_SPI_ID, 500000);
		if (E_OK != result) {
			EMBARC_PRINTF("Error: spi slave controller init failed%d.\r\n", result);
			break;
		}

		result = spi_transfer_config(SPIS_SERVER_SPI_ID, &spis_xfer_desc);
		if (E_OK != result) {
			EMBARC_PRINTF("Error: spi slave xfer config failed%d.\r\n", result);
			break;
		}

		result = spis_server_sync_init();
		if (E_OK != result) {
			EMBARC_PRINTF("Error: spis s2m sync init failed%d.\r\n", result);
			break;
		}

		DEV_BUFFER_INIT(&spis_server.dev_txbuf, NULL, 0);

		spis_server.tx_confirm_queue = xQueueCreate(SPIS_SERVER_TX_CONFIRM_QUEUE_LEN, 4);
		if (NULL == spis_server.tx_confirm_queue) {
			EMBARC_PRINTF("Error: spis_server queue create failed.\r\n");
			break;
		}

		spis_server.xfer_state = SPIS_XFER_IDLE;

		spis_server_register();
	} while (0);

}

static void spis_server_tx_callback(void *params);
int32_t spis_server_write(uint32_t *data, uint32_t len)
{
	int32_t result = E_OK;

	uint32_t header[2] = {SPIS_SERVER_FRAME_MAGIC, len};
	DEV_BUFFER *dev_buf_ptr = &spis_server.dev_txbuf;

	/* configure xfer. */
	uint32_t cpu_sts = arc_lock_save();
	spis_server.tx_total_size = len;
	spis_server.tx_handled_size = 0;
	spis_server.txdata = data;
	spis_server.crc_state = 0;
	spis_server.frame_last_part = 0;

	dev_buf_ptr->ofs = 0;
	dev_buf_ptr->buf = (void *)data;
	if (len < SPIS_SERVER_FRAME_LEN_MAX >> 2) {
		dev_buf_ptr->len = len;
	} else {
		dev_buf_ptr->len = SPIS_SERVER_FRAME_LEN_MAX >> 2;
	}

	spis_server.xfer_state = SPIS_TX_ONGOING;

	arc_unlock_restore(cpu_sts);

	do {
		if (len >= (1 << 16)) {
			result = E_PAR;
			break;
		}

		/*
		result = spi_databuffer_uninstall(SPIS_SERVER_SPI_ID, 0);
		if (E_OK != result) {
			break;
		}
		*/

		/* install xfer callback. */
		result = spi_interrupt_install(SPIS_SERVER_SPI_ID, 1, spis_server_tx_callback);
		if (E_OK != result) {
			break;
		}

		/* write header. */
		result = spi_write(SPIS_SERVER_SPI_ID, header, 2);
		if (E_OK != result) {
			break;
		}

		/* install txbuf. */
		result = spi_databuffer_install(SPIS_SERVER_SPI_ID, 1, dev_buf_ptr);
		if (E_OK != result) {
			break;
		}

		result = spis_server_s2m_sync();
		if (E_OK != result) {
			break;
		}

		//chip_hw_udelay(150);
		result = spi_interrupt_enable(SPIS_SERVER_SPI_ID, 1, 1);

	} while (0);

	return result;
}


static void spis_server_package_done(void)
{
	int32_t result = E_OK;

	uint32_t tx_finished = 1;
	BaseType_t higher_task_wkup = 0;

	//result = spi_interrupt_enable(SPIS_SERVER_SPI_ID, 1, 0);
	//result = spi_databuffer_uninstall(SPIS_SERVER_SPI_ID, 1);

	/* confirm to upper layer. */
	if (pdTRUE != xQueueSendFromISR(spis_server.tx_confirm_queue, \
					&tx_finished, &higher_task_wkup)) {
		/* TODO: record error! */
	}

	result = spi_interrupt_enable(SPIS_SERVER_SPI_ID, 1, 0);
	if (E_OK != result) {
		/* TODO: record error! */
	}

	spis_server.crc_state = 0;
	DEV_BUFFER_INIT(&spis_server.dev_txbuf, NULL, 0);
	spis_server.xfer_state = SPIS_XFER_IDLE;
}

static void spis_server_frame_done(void)
{
	int32_t result = E_OK;

	uint32_t crc_header[3];
	uint32_t remain_size = 0, send_size = 0, send_cnt = 0;

	DEV_BUFFER *dev_buf_ptr = &spis_server.dev_txbuf;

	/* last part of a frame payload has been sent, now the HW FIFO is empty,
	 * filling the frame crc and the next frame's header. */
	if (spis_server.frame_last_part) {

		/* TODO: compute crc firstly. */
		crc_header[send_cnt++] = 0;

		/* next frame exists, structure its header. */
		if (spis_server.tx_total_size > spis_server.tx_handled_size) {
			crc_header[send_cnt++] = SPIS_SERVER_FRAME_MAGIC;

			remain_size = spis_server.tx_total_size - spis_server.tx_handled_size;
			if (remain_size  < SPIS_SERVER_FRAME_LEN_MAX >> 2) {
				send_size = remain_size;
			} else {
				send_size = SPIS_SERVER_FRAME_LEN_MAX >> 2;
			}
			crc_header[send_cnt++] = send_size;
		}

		/* filling crc&next header to hw FIFO... */
		result = spi_write(SPIS_SERVER_SPI_ID, crc_header, send_cnt);
		if (E_OK == result) {
			spis_server.crc_state = 1;
		} else {
			/* TODO: record error! */
		}

		/* next frame exists, reinstall data buffer to low layer driver. */
		if (spis_server.tx_total_size > spis_server.tx_handled_size) {
			/* filling next frame. */
			uint32_t data_base = (uint32_t)dev_buf_ptr->buf;

			data_base = (uint32_t)spis_server.txdata + (spis_server.tx_handled_size << 2);
			DEV_BUFFER_INIT(&spis_server.dev_txbuf, (void *)data_base, send_size);

			result = spi_databuffer_install(SPIS_SERVER_SPI_ID, 1, &spis_server.dev_txbuf);
			if (E_OK != result) {
				/* TODO: record error! */
			}
		} else {
			result = spi_databuffer_uninstall(SPIS_SERVER_SPI_ID, 1);
			if (E_OK != result) {
				/* TODO: record error! */
			}
		}

		spis_server.frame_last_part = 0;

		result = spis_server_s2m_sync();
		if (E_OK != result) {
			/* TODO: record error! */
		}

	/* last part of a frame payload has been filled into HW FIFO, now triggle s2m_sync signal. */
	} else {
		spis_server.tx_handled_size += dev_buf_ptr->len;

		spis_server.frame_last_part = 1;

		result = spis_server_s2m_sync();
		if (E_OK != result) {
			/* TODO: record error! */
		}
	}
}

static void spis_server_tx_callback(void *params)
{
	int32_t result = E_OK;

	DEV_BUFFER *dev_buf_ptr = &spis_server.dev_txbuf;
	uint32_t remain_size = spis_server.tx_total_size - spis_server.tx_handled_size;

	/* crc has been sent, then check whether all package has been sent? */
	if (spis_server.crc_state) {
		/* all package has been sent. the whole process done. */
		if (0 == remain_size) {
			spis_server_package_done();

		/* the first part of the next frame payload has been filled into HW FIFO.
		 * clear crc_state. */
		} else {
			spis_server.crc_state = 0;
			result = spis_server_s2m_sync();
			if (E_OK != result) {
				/* TODO: record error! */
			}
		}

	/* frame transmission is ongoing. */
	} else {
		/* all frame payload has been sent or filled into HW FIFO. */
		if (dev_buf_ptr->ofs >= dev_buf_ptr->len) {
			spis_server_frame_done();

		/* one part of a frame payload has been filled into HW FIFO.
		 * triggle s2m_sync signal, inform master receiving. */
		} else {
			result = spis_server_s2m_sync();
			if (E_OK != result) {
				/* TODO: record error! */
			}
		}
	}

#if 0
	chip_hw_udelay(50);
	result = spis_server_s2m_sync();
	if (E_OK != result) {
		/* TODO: record error! */
	}
#endif

}
			

int32_t spis_server_transmit_done(void)
{
	int32_t result = E_OK;

	uint32_t msg = 0;
	if (pdTRUE == xQueueReceive(spis_server.tx_confirm_queue, \
				&msg, portMAX_DELAY)) {
		result = (int32_t)msg;
	} else {
		result = E_SYS;
	}

	return result;
}
