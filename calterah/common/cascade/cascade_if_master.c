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
#include "dma_hal.h"
#include "cascade_config.h"
#include "cascade.h"
#include "cascade_internal.h"

static uint8_t cs_if_master_id = 0;

int32_t cadcade_if_master_init(uint8_t id)
{
	cs_if_master_id = id;
	return cascade_if_master_sync_init();
}


int32_t cascade_if_master_write(cs_xfer_mng_t *xfer)
{
	int32_t result = E_OK;

	//uint32_t cpu_sts = 0;
	cs_frame_t *frame = &xfer->cur_frame;

	//EMBARC_PRINTF("package start: %d\r\n", xfer->nof_frame);
	cascade_tx_first_frame_init(&xfer->cur_frame, xfer->data, xfer->xfer_size);
	/* write package header. */
	//cpu_sts = arc_lock_save();
	frame->header_done = 1;
	//arc_unlock_restore(cpu_sts);
	result = spi_write(cs_if_master_id, &frame->header, 1);
	if (E_OK == result) {
#ifdef CASCADE_DMA
		result = cascade_crc_dmacopy(frame->payload, frame->len << 2);
		if (result >= 0) {
			xfer->crc_dma_chn_id = result;
		}
#else
		cascade_crc_request(frame->payload, frame->len);
#endif
	} else {
		frame->header_done = 0;
	}

	return result;
}

/* this function is called on the master side, by s2m_sync isr callback.  */
void cascade_if_master_send_callback(cs_xfer_mng_t *xfer)
{
	int32_t result = E_OK;

	uint32_t split_cnt;
	cs_frame_t *frame = &xfer->cur_frame;

	do {
		if ((NULL == xfer) || (NULL == xfer->data) || (0 == xfer->xfer_size)) {
			result = E_PAR;
			break;
		}

		/* frame header has not been sent, send it. */
		if (!frame->header_done) {
			//EMBARC_PRINTF("next frame header:\r\n");
			result = spi_write(cs_if_master_id, &frame->header, 1);
			if (E_OK != result) {
				break;
			}
			frame->header_done = 1;
#ifdef CASCADE_DMA
			result = cascade_crc_dmacopy(frame->payload, frame->len);
			if (result >= 0) {
				xfer->crc_dma_chn_id = result;
			}
#else
			cascade_crc_request(frame->payload, frame->len);
#endif
			break;
		}

		/* each s2m_sync signal interrupt happened, send a block of payload. */
		if (!frame->payload_done) {
			//uint32_t *buf = frame->payload + frame->handled_size;
			uint32_t *buf = FRAME_PART_BASE(frame);

			split_cnt = FRAME_PART_SIZE(frame);

			result = spi_write(cs_if_master_id, buf, split_cnt);
			if (E_OK == result) {
				cascade_frame_part_update(frame, split_cnt);
			}
			break;
		}

		/* after all payload have been sent, then send crc. if the crc32 computing
		 * has not finished, then change xfer manager state, and waiting task to 
		 * update the states. */
		if (!frame->crc_done) {
			if (frame->crc_valid) {
				result = spi_write(cs_if_master_id, &frame->crc32, 1);
				if (E_OK == result) {
					frame->crc_done = 1;
					frame->crc_valid = 0;
				} else {
					break;
				}
			} else {
				/* waiting crc32... */
				xfer->state = CS_XFER_CRC;
				break;
			}
		}

		/* check whether all package have been sent, if not, initialize the
		 * next frame. if so, reset xfer manafer. */
		if (xfer->cur_frame_id < xfer->nof_frame - 1) {
			//EMBARC_PRINTF("next frame start:\r\n");
			cascade_tx_next_frame_init(xfer);
		} else {
			//EMBARC_PRINTF("package done:\r\n");
			cascade_tx_confirmation(xfer);
			cascade_package_done(xfer);
		}

	} while (0);

}

/* start a package receiving session:
 * read package header from hardware FIFO, and parse it to gain the package length.
 * and then, init cascade receiver.
 * note: this function is called in interrupt context.
 * */
void cascade_if_master_package_init(cs_xfer_mng_t *xfer)
{
	int32_t result = E_OK;

	uint32_t total_size, header;

	result = spi_read(cs_if_master_id, &header, 1);
	if (E_OK == result) {
		/* check whether the received header is valid?
		 * and get package length. */
		if (CASCADE_HEADER_VALID(header)) {
			total_size = CASCADE_PACKAGE_LEN(header);

			cascade_receiver_init(total_size);
			cascade_rx_first_frame_init(xfer);
		} else {
			/* TODO: record error! */
		}

	} else {
		/* TODO: record error! */
	}
}

/* resume a pending receiving session.
 * */
void cascade_if_master_xfer_resume(cs_xfer_mng_t *xfer, uint32_t length)
{
	if (xfer) {
		cascade_if_master_package_init(xfer);
	}
}


/* interface(master) package receiving process:
 * before entering this function, cascade_if_master_package_init has been executed.
 * note: this function is called in s2m_sync interrupt context.
 * */
void cascade_if_master_receive_callback(cs_xfer_mng_t *xfer)
{
	int32_t result = E_OK;

	uint32_t header, total_size;
	uint32_t split_cnt;
	cs_frame_t *frame = &xfer->cur_frame;

	do {
		if ((NULL == xfer) || (NULL == xfer->data)) {
			result = E_PAR;
			break;
		}

		if (CS_XFER_DONE == xfer->state) {
			xfer->state = CS_XFER_PENDING;
		}

		/* frame header has not been received.
		 * firstly, read frame header from hw RX FIFO, then check its validation.
		 * if it's not a valid header, then return from the current flow. otherwise
		 * parse the frame length from header. lastly, if the cascade service currently
		 * is in idle, or transmitting work has been finished, then enter receiving
		 * flow. otherwise ignore the current message. */
		if (!frame->header_done) {
			result = spi_read(cs_if_master_id, &header, 1);
			if (result < 0) {
				break;
			}

			if (0 == CASCADE_HEADER_VALID(header)) {
				/* TODO: record error! */
				result = E_SYS;
				break;
			}
			total_size = CASCADE_PACKAGE_LEN(header);

			/* @frame_done will be set. */
			cascade_rx_next_frame_init(xfer, total_size);

			break;
		}

		/* receiving payload process: each time read a block. */
		if (!frame->payload_done) {
			uint32_t *buf = FRAME_PART_BASE(frame);
			split_cnt = FRAME_PART_SIZE(frame);

			result = spi_read(cs_if_master_id, buf, split_cnt);
			if (E_OK == result) {
				cascade_frame_part_update(frame, split_cnt);
			}
			break;
		}

		/* start to read frame payload crc32, if the previous frame payload
		 * crc32 computing isn't finished, then change the xfer manager state
		 * to WAITING. otherwise read crc32, and start computing the current
		 * frame payload crc32. */
		if (!frame->crc_done) {
			/* crc_valid: clear in dma callback. */
			if (frame->crc_valid) {
				/* previous frame still valid? */
				xfer->state = CS_XFER_CRC;
				break;
			}

			result = spi_read(cs_if_master_id, &frame->crc32, 1);
			if ( E_OK != result) {
				break;
			}
			frame->crc_done = 1;
			frame->crc_valid = 1;

			/* start crc computing action. */
#ifdef CASCADE_DMA
			result = cascade_crc_dmacopy(frame->payload, frame->len);
			if (E_OK != result) {
				break;
			}
#else
			cascade_crc_request(frame->payload, frame->len);
#endif
		} else {
		}

		/* check whether there is a next frame needing to receive. if yes,
		 * then reinitialize the current frame descriptor. otherwise, the
		 * receiving process has been finished. */
		if (xfer->cur_frame_id < xfer->nof_frame - 1) {
			cascade_frame_reset(frame);
		} else {
			/* must wait the last frame crc comparing finished. */
			if (0 == frame->crc_valid) {
				cascade_rx_indication(xfer);
				cascade_package_done(xfer);
				EMBARC_PRINTF("rx package done before crc done.\r\n");
			}
		}

	} while (0);

}

void cascade_if_master_rx_crc_done(cs_xfer_mng_t *xfer, uint32_t crc32)
{
	int32_t result = E_OK;

	cs_frame_t *frame = &xfer->cur_frame;
	do {
	 	if (0 == frame->crc_valid) {
			/* panic: what happened? */
			break;
		}

		xfer->crc_done_cnt += 1;

		/* compare the crc32. */
		if (crc32 != frame->crc32) {
			xfer->crc_unmatch_cnt += 1;
		}
		frame->crc_valid = 0;

#ifdef CASCADE_DMA
		result = dma_release_channel(xfer->crc_dma_chn_id);
		if (E_OK != result) {
			/* TODO: record error! */
			break;
		}
#endif

		/* session has been blocked. */
		if (CS_XFER_CRC == xfer->state) {
			/* relieve the block. */
			xfer->state = CS_XFER_BUSY;

			result = spi_read(cs_if_master_id, &frame->crc32, 1);
			if (result < 0) {
				/* TODO: record error! */
				break;
			}
			frame->crc_done = 1;
			frame->crc_valid = 1;

#ifdef CASCADE_DMA
			result = cascade_crc_dmacopy(frame->payload, frame->len);
			if (E_OK != result) {
				/* TODO: record error! */
				break;
			}
#else
			cascade_crc_request(frame->payload, frame->len);
#endif
		} else {
			/* whether is the last frame? */
			//if (xfer->cur_frame_id >= xfer->nof_frame - 1) {
			if (xfer->crc_done_cnt >= xfer->nof_frame) {
				cascade_rx_indication(xfer);
				cascade_package_done(xfer);
				EMBARC_PRINTF("rx package done.\r\n");
			}
		}

	} while (0);

}

void cascade_if_master_tx_crc_done(cs_xfer_mng_t *xfer, uint32_t crc32)
{
	int32_t result = E_OK;

	cs_frame_t *frame = &xfer->cur_frame;
	while (xfer) {
		frame->crc32 = crc32;
#ifdef CASCADE_DMA
		result = dma_release_channel(xfer->crc_dma_chn_id);
		if (E_OK != result) {
			/* TODO: record error! */
			break;
		}
#endif

		if (CS_XFER_CRC == xfer->state) {
			/* relieve the block. */
			xfer->state = CS_XFER_BUSY;

			result = spi_write(cs_if_master_id, &frame->crc32, 1);
			if (E_OK == result) {
				frame->crc_valid = 0;
				frame->crc_done = 1;
			}

			if (xfer->cur_frame_id < xfer->nof_frame - 1) {
				//EMBARC_PRINTF("next frame start1:\r\n");
				cascade_tx_next_frame_init(xfer);
			} else {
				//EMBARC_PRINTF("package done1:\r\n");
				cascade_tx_confirmation(xfer);
				cascade_package_done(xfer);
			}

		/* no block, set flag, waiting to be sent in isr callback. */
		} else {
			frame->crc_valid = 1;
		}

		break;
	}

}
