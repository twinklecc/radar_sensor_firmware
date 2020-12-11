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


static uint8_t cs_if_slave_id = 0;

int32_t cadcade_if_slave_init(uint8_t id)
{
	cs_if_slave_id = id;
	return cascade_if_slave_sync_init();
}

int32_t cascade_if_slave_write(cs_xfer_mng_t *xfer)
{
	int32_t result = E_OK;

	cs_frame_t *frame = &xfer->cur_frame;

	cascade_tx_first_frame_init(frame, xfer->data, xfer->xfer_size);
	DEV_BUFFER_INIT((&xfer->devbuf), frame->payload, frame->len);

	EMBARC_PRINTF("package start: %d.\r\n", xfer->nof_frame);
	do {
		/* filling header to HW FIFO. */
		result = spi_write(cs_if_slave_id, &frame->header, 1);
		if (E_OK != result) {
			break;
		}

		frame->header_done = 1;

		result = spi_databuffer_install(cs_if_slave_id, 1, &xfer->devbuf);
		if (E_OK != result) {
			break;
		}

#ifdef CASCADE_DMA
		result = cascade_crc_dmacopy(frame->payload, frame->len);
		if (result >= 0) {
			xfer->crc_dma_chn_id = result;
		} else {
			break;
		}
#else
		//chip_hw_udelay(50);
		cascade_crc_request(frame->payload, frame->len);
#endif

		result = spi_interrupt_enable(cs_if_slave_id, 0, 0);

		/* triggle s2m sync signal. */
		result = cascade_spi_s2m_sync();
		if (E_OK != result) {
			break;
		}

		result = spi_interrupt_enable(cs_if_slave_id, 1, 1);

	} while (0);

	return result;
}

/* this function is called on the slave side, in TX FIFO empty isr. */
void cascade_if_slave_send_callback(cs_xfer_mng_t *xfer)
{
	int32_t result = E_OK;

	cs_frame_t *frame = &xfer->cur_frame;

	do {
		if ((NULL == xfer) || (NULL == xfer->data) || (0 == xfer->xfer_size)) {
			result = E_PAR;
			break;
		}

		/* frame header has not been sent. firstly, install data buffer
		 * to device driver. and then, send header. */
		if (!frame->header_done) {
			//EMBARC_PRINTF("next frame header:\r\n");
			result = spi_write(cs_if_slave_id, &frame->header, 1);
			if (E_OK != result) {
				/* panic: record error! */
				break;
			}
			frame->header_done = 1;

			DEV_BUFFER_INIT((&xfer->devbuf), frame->payload, frame->len);
			result = spi_databuffer_install(cs_if_slave_id, 1, &xfer->devbuf);
			if (E_OK != result) {
				/* panic: record error! */
				break;
			}

#ifdef CASCADE_DMA
			result = cascade_crc_dmacopy(frame->payload, frame->len);
			if (result >= 0) {
				xfer->crc_dma_chn_id = result;
				/* triggle s2m sync signal. */
				result = cascade_spi_s2m_sync();
			} else {
				/* panic: record error! */
			}
#else
			cascade_crc_request(frame->payload, frame->len);
			//chip_hw_udelay(50);
			result = cascade_spi_s2m_sync();
#endif
			break;
		}

		/* each time, send a block of frame payload. while all the payload has 
		 * been received, clean up the device buffer. */
		if (!frame->payload_done) {
			if (xfer->devbuf.ofs >= xfer->devbuf.len) {
				frame->payload_done = 1;
				DEV_BUFFER_INIT(&xfer->devbuf, NULL, 0);
				/* continue to send crc32. */
			}
			/* triggle s2m sync signal. */
			//chip_hw_udelay(50);
			result = cascade_spi_s2m_sync();
			break;
		} else {
		}

		/* check whether the crc computing finished? if not, start waiting. */
		if (!frame->crc_done) {
			if (!frame->crc_valid) {
				xfer->state = CS_XFER_CRC;
				/* HW FIFO is empty, waiting crc32! */
				result = spi_interrupt_enable(cs_if_slave_id, 1, 0);
				break;
			}

			result = spi_write(cs_if_slave_id, &frame->crc32, 1);
			if (E_OK != result) {
				break;
			}
			frame->crc_valid = 0;
			frame->crc_done = 1;

			/* triggle s2m sync signal. */
			//chip_hw_udelay(50);
			result = cascade_spi_s2m_sync();
		} else {
		}

		if (xfer->cur_frame_id < xfer->nof_frame - 1) {
			//EMBARC_PRINTF("next frame start:\r\n");
			cascade_tx_next_frame_init(xfer);
		} else {
			cascade_tx_confirmation(xfer);
			cascade_package_done(xfer);
			result = spi_interrupt_enable(cs_if_slave_id, 0, 1);
			result = spi_interrupt_enable(cs_if_slave_id, 1, 0);
			cascade_session_state(CS_SLV_TX_DONE);
			EMBARC_PRINTF("tx package done.\r\n");
		}

	} while (0);

}

void cascade_if_slave_package_init(cs_xfer_mng_t *xfer)
{
	int32_t result = E_OK;

	uint32_t total_size, header;
	uint32_t rx_thres = CASCADE_FRAME_PART_LEN_MAX - 1;

	cs_frame_t *frame = &xfer->cur_frame;

	while (xfer) {
		result = spi_read(cs_if_slave_id, &header, 1);
		if (E_OK != result) {
			/* TODO: record error! */
			break;
		}

		/* check whether the received header is valid?
		 * and get package length. */
		if (0 == CASCADE_HEADER_VALID(header)) {
			/* TODO: record error! */
			break;
		}
		total_size = CASCADE_PACKAGE_LEN(header);

		cascade_receiver_init(total_size);
		cascade_rx_first_frame_init(xfer);
		//EMBARC_PRINTF("package start: %d.\r\n", xfer->nof_frame);

		/* install rxbuffer and triggle s2m_sync. */
		DEV_BUFFER_INIT(&xfer->devbuf, frame->payload, frame->len);
		result = spi_databuffer_install(cs_if_slave_id, 0, &xfer->devbuf);
		if (E_OK != result) {
			break;
		}

		if (rx_thres > frame->len) {
			rx_thres = frame->len - 1;
		}
		result = spi_fifo_threshold_update(cs_if_slave_id, rx_thres, 0);
		if (E_OK != result) {
			break;
		}

		/* trigger s2m_sync signal to inform the other end starting
		 * send payload. */
		//chip_hw_udelay(80);
		result = cascade_spi_s2m_sync();
		break;
	}
}

void cascade_if_slave_xfer_resume(cs_xfer_mng_t *xfer, uint32_t length)
{
	int32_t result = E_OK;

	uint32_t rx_thres = CASCADE_FRAME_PART_LEN_MAX - 1;
	while (xfer) {
		cs_frame_t *frame = &xfer->cur_frame;

		cascade_receiver_init(length);
		cascade_rx_first_frame_init(xfer);

		/* install rxbuffer and triggle s2m_sync. */
		DEV_BUFFER_INIT(&xfer->devbuf, frame->payload, frame->len);
		result = spi_databuffer_install(cs_if_slave_id, 0, &xfer->devbuf);
		if (E_OK != result) {
			break;
		}

		if (rx_thres > frame->len) {
			rx_thres = frame->len - 1;
		}
		result = spi_fifo_threshold_update(cs_if_slave_id, rx_thres, 0);
		if (E_OK != result) {
			break;
		}

		/* trigger s2m_sync signal to inform the other end starting
		 * send payload. */
		//chip_hw_udelay(50);
		result = cascade_spi_s2m_sync();
		break;
	}
}

/* this function is called on the slave side, in RX FIFO full isr. */
void cascade_if_slave_receive_callback(cs_xfer_mng_t *xfer)
{
	int32_t result = E_OK;

	uint32_t total_size, header;
	cs_frame_t *frame = &xfer->cur_frame;

	do {
		if ((NULL == xfer) || (NULL == xfer->data)) {
			result = E_PAR;
			break;
		}

		if (CS_XFER_DONE == xfer->state) {
			//EMBARC_PRINTF("next frame header:\r\n");
			result = spi_read(cs_if_slave_id, &header, 1);
			if (result < 0) {
				break;
			}

			if (0 == CASCADE_HEADER_VALID(header)) {
				/* TODO: record error! */
				result = E_SYS;
				break;
			}

			frame->header = CASCADE_PACKAGE_LEN(header);
			xfer->state = CS_XFER_PENDING;
		}

		/* firstly, no data buffer regsitered to low layer driver.
		 * so read header in isr callback. */
		if (!frame->header_done) {
			uint32_t new_rx_thres = CASCADE_FRAME_PART_LEN_MAX - 1;

			result = spi_read(cs_if_slave_id, &header, 1);
			if (result < 0) {
				break;
			}

			/* check whether the received header is valid? and get package length. */
			if (0 == CASCADE_HEADER_VALID(header)) {
				/* TODO: record error! */
				result = E_SYS;
				break;
			}
			total_size = CASCADE_PACKAGE_LEN(header);
			if (total_size >= CASCADE_BUFFER_SIZE << 2) {
				result = E_BOVR;
				break;
			}

			cascade_rx_next_frame_init(xfer, total_size);

			/* install buffer to receive frame payload. */
			DEV_BUFFER_INIT(&xfer->devbuf, frame->payload, frame->len);
			result = spi_databuffer_install(cs_if_slave_id, 0, &xfer->devbuf);
			if (E_OK != result) {
				break;
			}

			if (new_rx_thres > xfer->devbuf.len - xfer->devbuf.ofs) {
				new_rx_thres = xfer->devbuf.len - xfer->devbuf.ofs - 1;
			}
			result = spi_fifo_threshold_update(cs_if_slave_id, new_rx_thres, 0);
			if (E_OK != result) {
				break;
			}

			/* trigger s2m_sync signal to inform the other end starting
			 * send payload. */
			//chip_hw_udelay(50);
			result = cascade_spi_s2m_sync();
			break;
		}

		/* recieving frame payload in low level layer. if all the payload has been
		 * recieved, prepare to recieve crc32. anyway, s2m_sync has to be triggered,
		 * in order to inform the other side to send next payload block or paylaod
		 * crc32.*/
		if (!frame->payload_done) {
			uint32_t rx_thres_update = 0;
			uint32_t new_rx_thres = CASCADE_FRAME_PART_LEN_MAX - 1;
			if (xfer->devbuf.ofs >= xfer->devbuf.len) {
				DEV_BUFFER_INIT(&xfer->devbuf, NULL, 0);
				frame->payload_done = 1;
				new_rx_thres = 0;
				rx_thres_update = 1;
			} else {
				if (new_rx_thres > xfer->devbuf.len - xfer->devbuf.ofs) {
					new_rx_thres = xfer->devbuf.len - xfer->devbuf.ofs - 1;
					rx_thres_update = 1;
				}
			}

			if (rx_thres_update) {
				result = spi_fifo_threshold_update(cs_if_slave_id, new_rx_thres, 0);
				if (E_OK != result) {
					break;
				}
			}

			//chip_hw_udelay(50);
			result = cascade_spi_s2m_sync();
			break;
		}

		/* the xfer process enter receiving current frame payload crc32.
		 * if the previous crc32 still valid. then the process has been
		 * blocked, change the xfer manager state to WAITING CRC. otherwise
		 * read crc32, and start computing the current frame payload crc32. */
		if (!frame->crc_done) {
			if (frame->crc_valid) {
				/* previous frame still valid? */
				xfer->state = CS_XFER_CRC;
				result = spi_interrupt_enable(cs_if_slave_id, 0, 0);
				break;
			}

			result = spi_read(cs_if_slave_id, &frame->crc32, 1);
			if (result < 0) {
				break;
			}
			frame->crc_done = 1;
			frame->crc_valid = 1;

			/*
			result = cascade_spi_s2m_sync();
			if (E_OK != result) {
				break;
			}
			*/

			/* start crc computing action. */
#ifdef CASCADE_DMA
			result = cascade_crc_dmacopy(frame->payload, frame->len);
			if (E_OK != result) {
				break;
			}
#else
			cascade_crc_request(frame->payload, frame->len);
#endif
		}

		/* check whether there is a next frame needing to receive. if yes,
		 * re-initialize frame descriptor. if the current frame is the last
		 * one, then the entire receiving process has been finished. */
		if (xfer->cur_frame_id >= xfer->nof_frame - 1) {
		/*
			cascade_rx_next_frame_init(xfer);
		} else {
		*/
			if (0 == frame->crc_valid) {
				//EMBARC_PRINTF("package done before crc done\r\n");
				cascade_rx_indication(xfer);
				cascade_package_done(xfer);
				result = spi_databuffer_uninstall(cs_if_slave_id, 0);
				if (E_OK != result) {
				}
			}
		} else {
			//EMBARC_PRINTF("next frame start:\r\n");
			cascade_frame_reset(frame);
			//chip_hw_udelay(50);
			result = cascade_spi_s2m_sync();
			if (E_OK != result) {
			}
		}

	} while (0);

}

void cascade_if_slave_rx_crc_done(cs_xfer_mng_t *xfer, uint32_t crc32)
{
	int32_t result = E_OK;

	cs_frame_t *frame = &xfer->cur_frame;
	while (xfer) {
	 	if (0 == frame->crc_valid) {
			/* what happened? */
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
			break;
		}
#endif

		/* session has been blocked. */
		if (CS_XFER_CRC == xfer->state) {
			xfer->state = CS_XFER_BUSY;
			result = spi_interrupt_enable(cs_if_slave_id, 0, 1);
		} else {
			/* whether is the last frame? */
			//if (xfer->cur_frame_id >= xfer->nof_frame - 1) {
			if (xfer->crc_done_cnt >= xfer->nof_frame) {
				//EMBARC_PRINTF("package done after crc done\r\n");
				cascade_rx_indication(xfer);
				cascade_package_done(xfer);
				result = spi_databuffer_uninstall(cs_if_slave_id, 0);
				if (E_OK != result) {
				}
			}
		}
		break;
	}
}

void cascade_if_slave_tx_crc_done(cs_xfer_mng_t *xfer, uint32_t crc32)
{
	int32_t result = E_OK;

	cs_frame_t *frame = &xfer->cur_frame;

	do {
		frame->crc32 = crc32;
		frame->crc_valid = 1;
#ifdef CASCADE_DMA
		result = dma_release_channel(xfer->crc_dma_chn_id);
		if (E_OK != result) {
			break;
		}
#endif

		if (CS_XFER_CRC == xfer->state) {
			/* has been blocked. */
			xfer->state = CS_XFER_BUSY;

			//frame->crc_valid = 0;
			result = spi_interrupt_enable(cs_if_slave_id, 1, 1);
			if (E_OK != result) {
				break;
			}
		//} else {
		//	EMBARC_PRINTF("next frame start1:\r\n");
		}

	} while (0);

}
