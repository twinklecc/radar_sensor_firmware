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
#include "frame.h"


/*
 * @com_type: communication type, indicated by CFG_CASCADE_COM_TYPE.
 * @xfer_type: transfer type, indicate the current transfer type,
 *            receiving or transmitting?
 * @rx_mng: receiving manager.
 * @tx_mng: transmitting manager.
 * */
typedef struct cascade_protocol {
	uint32_t mode;

	uint8_t com_type;
	uint8_t xfer_type;

	cs_com_cfg_t *com_cfg;

	cs_xfer_mng_t rx_mng;
	cs_xfer_mng_t tx_mng;
} cs_global_t;


static uint32_t rx_data_buffer[CASCADE_BUFFER_SIZE];
static cs_global_t cs_proc;
//static TaskHandle_t cascade_handle = NULL;


static void cascade_task_main(void *params);
int32_t cascade_init(void)
{
	int32_t result = E_OK;

	do {
		/* check whether cascade status getting is ok or not. */
		int32_t cs_sts = chip_cascade_status();
		if (cs_sts < 0) {
			result = cs_sts;
			EMBARC_PRINTF("Error: cascade status read failed!\r\n");
			break;
		} else {
			cs_proc.mode = (uint32_t)cs_sts;
			EMBARC_PRINTF("Cascade status: %d\r\n", cs_sts);
		}

		//cs_proc.com_type = CFG_CASCADE_COM_TYPE;

		/* cascade xfer type. */
		cs_proc.xfer_type = CS_XFER_INVALID;

		result = cascade_if_init();
		if (E_OK != result) {
			EMBARC_PRINTF("Error: cascade interface init failed[%d].\r\n", result);
			break;
		}

		cs_proc.rx_mng.state = CS_XFER_IDLE;
		cs_proc.rx_mng.data = rx_data_buffer;
		DEV_BUFFER_INIT(&cs_proc.rx_mng.devbuf, NULL, 0);

		cs_proc.tx_mng.state = CS_XFER_IDLE;
		DEV_BUFFER_INIT(&cs_proc.tx_mng.devbuf, NULL, 0);

		cs_proc.rx_mng.queue_handle = xQueueCreate(CASCADE_QUEUE_LEN, 12);
		if (NULL == cs_proc.rx_mng.queue_handle) {
			result = E_SYS;
			EMBARC_PRINTF("Error: cascade queue create failed.\r\n");
		}

		cs_proc.tx_mng.queue_handle = xQueueCreate(CASCADE_QUEUE_LEN, 12);
		if (NULL == cs_proc.tx_mng.queue_handle) {
			result = E_SYS;
			EMBARC_PRINTF("Error: cascade queue(tx) create failed.\r\n");
		}

		result = crc_init(0, 1);
		if (E_OK != result) {
			EMBARC_PRINTF("Error: crc init failed.\r\n");
		}

		cascade_crc_init();

		//cascade_if_cli_register();

	} while (0);

        cascade_sync_bb_init(); // gpio_6 sync for bb

	return result;
}

int32_t cascade_in_xfer_session(cs_xfer_mng_t **xfer)
{
	int32_t result = CS_XFER_INVALID;
	if (xfer) {
		uint32_t cpu_sts = arc_lock_save();
		if (CS_RX == cs_proc.xfer_type) {
			*xfer = &cs_proc.rx_mng;
		} else if (CS_TX == cs_proc.xfer_type) {
			*xfer = &cs_proc.tx_mng;
		} else {
			*xfer = NULL;
		}
		result = cs_proc.xfer_type;
		arc_unlock_restore(cpu_sts);
	}
	return result;
}

/* read data:
 * this function will wait untill a message has been inserted in receiving queue.
 * receiver read data from hardware FIFO and store them to the local rx buffer in
 * interrupt context, once all of the package have been received, a message will
 * be inserted in receiving queue.
 * note: this function can only be called in task context.
 * */
int32_t cascade_read(uint32_t **data, uint32_t *len, TickType_t wait_ticks)
{
	int32_t result = E_OK;

	DEV_BUFFER msg;
	cs_xfer_mng_t *xfer = &cs_proc.rx_mng;

	if ((NULL == data) || (NULL == len)) {
		result = E_PAR;
	} else {
		DEV_BUFFER_INIT(&msg, NULL, 0);
		if (pdTRUE == xQueueReceive(xfer->queue_handle, \
					&msg, wait_ticks)) {
			*data = (uint32_t *)msg.buf;
			*len = msg.len;
		} else {
			result = E_SYS;
		}
	}

	return result;
}

int32_t cascade_package_init(cs_xfer_mng_t *xfer, uint32_t *data, uint16_t len)
{
	int32_t result = E_OK;

	if (NULL != xfer) {
		xfer->data = data;
		xfer->xfer_size = len;

		xfer->nof_frame = len / (CASCADE_FRAME_LEN_MAX >> 2);
		if (len % (CASCADE_FRAME_LEN_MAX >> 2)) {
			xfer->nof_frame += 1;
		}

		xfer->cur_frame_id = 0;
	} else {
		result = E_PAR;
	}

	return result;
}

void cascade_receiver_init(uint32_t length)
{
	cs_xfer_mng_t *xfer = &cs_proc.rx_mng;
	if (length > 0) {
		xfer->data = rx_data_buffer;
		xfer->xfer_size = length;

		xfer->nof_frame = length / (CASCADE_FRAME_LEN_MAX >> 2);
		if (length % (CASCADE_FRAME_LEN_MAX >> 2)) {
			xfer->nof_frame += 1;
		}
		xfer->cur_frame_id = 0;
		xfer->crc_done_cnt = 0;

		xfer->state = CS_XFER_BUSY;
	}
}


void cascade_package_done(cs_xfer_mng_t *xfer)
{
	if (NULL != xfer) {
		uint32_t cpu_sts = arc_lock_save();
		cs_proc.xfer_type = CS_XFER_INVALID;
		xfer->state = CS_XFER_IDLE;
		arc_unlock_restore(cpu_sts);
	}
}

void cascade_session_state(uint8_t state)
{
	cs_proc.xfer_type = state;
}

int32_t cascade_write(uint32_t *data, uint32_t len)
{
	int32_t result = E_OK;

	uint32_t cpu_sts = 0;

	cs_xfer_mng_t *xfer = &cs_proc.tx_mng;

	do {
		/* tx state: ready for writing? */
		cpu_sts = arc_lock_save();
		if ((CS_XFER_INVALID != cs_proc.xfer_type) || \
		    (CS_XFER_IDLE != cs_proc.tx_mng.state)) {
			arc_unlock_restore(cpu_sts);
			result = E_DBUSY;
			break;
		}
		cs_proc.xfer_type = CS_TX;
		cs_proc.tx_mng.state = CS_XFER_BUSY;
		arc_unlock_restore(cpu_sts);


		/* half complex: clear rx device buffer. */
		DEV_BUFFER_INIT(&cs_proc.rx_mng.devbuf, NULL, 0);

		/* package init. */
		result = cascade_package_init(xfer, data, len);
		if (E_OK != result) {
			break;
		}

		if (CHIP_CASCADE_MASTER == cs_proc.mode) {
			result = cascade_if_master_write(xfer);
		} else {
			result = cascade_if_slave_write(xfer);
		}

	} while (0);

	return result;
}

int32_t cascade_transmit_done(void)
{
	int32_t result = E_OK;

	DEV_BUFFER msg;
	cs_xfer_mng_t *xfer = &cs_proc.tx_mng;

	DEV_BUFFER_INIT(&msg, NULL, 0);
	if (pdTRUE != xQueueReceive(xfer->queue_handle, \
				&msg, portMAX_DELAY)) {
		result = E_SYS;
	}

	return result;
}

void cascade_process_done(void)
{
	cs_xfer_mng_t *xfer = &cs_proc.rx_mng;

	/* frame header field buffers the next package's length. */
	uint32_t length = xfer->cur_frame.header;

	uint32_t cpu_sts = arc_lock_save();
	cascade_package_done(xfer);
	if (CS_XFER_PENDING == xfer->state) {
		cascade_if_xfer_resume(xfer, length);
		cs_proc.xfer_type = CS_RX;
	}
	arc_unlock_restore(cpu_sts);
}

void cascade_crc_compute_done(uint32_t crc32)
{
	cs_xfer_mng_t *xfer = NULL;

	if (CS_RX == cs_proc.xfer_type) {
		xfer = &cs_proc.rx_mng;
		cascade_rx_crc_handle(xfer, crc32);

	} else if (CS_TX == cs_proc.xfer_type) {
		xfer = &cs_proc.tx_mng;
		cascade_tx_crc_handle(xfer, crc32);

	} else {
		/* panic: what happened? */
	}
}


void cascade_rx_indication(cs_xfer_mng_t *xfer)
{
	BaseType_t higher_task_wkup = 0;
	if (xfer) {
		DEV_BUFFER msg = {
			.buf = xfer->data,
			.len = xfer->xfer_size
		};
		if (pdTRUE == xQueueSendFromISR(xfer->queue_handle, \
					&msg, &higher_task_wkup)) {
			if (higher_task_wkup) {
				portYIELD_FROM_ISR();
			}
			xfer->state = CS_XFER_DONE;
		} else {
			/* TODO: record error! */
			;
		}
	}
}

void cascade_tx_confirmation(cs_xfer_mng_t *xfer)
{
	BaseType_t higher_task_wkup = 0;
	DEV_BUFFER msg = {
		.ofs = xfer->xfer_size,
	};

	if (xfer) {
		if (pdTRUE == xQueueSendFromISR(xfer->queue_handle, \
					&msg, &higher_task_wkup)) {
			if (higher_task_wkup) {
				portYIELD_FROM_ISR();
			}
		} else {
			/* TODO: record error! */
			;
		}
	}
}

void cascade_sync_callback(void *params)
{
	cs_xfer_mng_t *xfer = NULL;

	if (CS_TX == cs_proc.xfer_type) {
		xfer = &cs_proc.tx_mng;
		cascade_if_master_send_callback(xfer);
	} else if (CS_RX == cs_proc.xfer_type) {
		xfer = &cs_proc.rx_mng;
		cascade_if_master_receive_callback(xfer);
	} else {
		/* new package comming! */
		xfer = &cs_proc.rx_mng;
		cascade_if_master_package_init(xfer);
		if (xfer->xfer_size) {
			cs_proc.xfer_type = CS_RX;
		}
	}
}

void cascade_if_callback(void *params)
{
	cs_xfer_mng_t *xfer = NULL;

	if (CS_TX == cs_proc.xfer_type) {
		xfer = &cs_proc.tx_mng;
		cascade_if_slave_send_callback(xfer);
	} else if (CS_RX == cs_proc.xfer_type) {
		xfer = &cs_proc.rx_mng;
		cascade_if_slave_receive_callback(xfer);
	} else if (CS_SLV_TX_DONE == cs_proc.xfer_type) {
		while (raw_readl(0xb80024)) {
			raw_readl(0xb80060);
		}
		cs_proc.xfer_type = CS_XFER_INVALID;
	} else {
		xfer = &cs_proc.rx_mng;
		cascade_if_slave_package_init(xfer);
		if (xfer->xfer_size) {
			cs_proc.xfer_type = CS_RX;
		}
	}
}
