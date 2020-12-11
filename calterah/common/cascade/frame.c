#include <string.h>
#include "embARC_toolchain.h"
#include "FreeRTOS.h"
#include "task.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "dw_gpio.h"
#include "gpio_hal.h"
#include "spi_hal.h"
#include "dma_hal.h"
#include "cascade_config.h"
#include "cascade.h"
#include "cascade_internal.h"
#include "frame.h"


void cascade_tx_first_frame_init(cs_frame_t *frame, uint32_t *data, uint16_t len)
{
	if ((NULL != frame) && (NULL != data) && (0 != len)) {
		frame->header = (CASCADE_HEADER_MAGIC << 16) | len;
		frame->payload = data;

		if (len <= CASCADE_FRAME_LEN_MAX >> 2) {
			frame->len = len;
		} else {
			frame->len = (CASCADE_FRAME_LEN_MAX >> 2);
		}

		frame->handled_size = 0;
		frame->header_done = 0;
		frame->payload_done = 0;
		frame->crc_done = 0;
		frame->crc_valid = 0;
	}
}


void cascade_rx_first_frame_init(cs_xfer_mng_t *xfer)
{
	if (NULL != xfer) {
		if (xfer->xfer_size <= (CASCADE_BUFFER_SIZE)) {
			cs_frame_t *frame = &xfer->cur_frame;
			frame->payload = xfer->data;

			if (xfer->xfer_size <= CASCADE_FRAME_LEN_MAX >> 2) {
				frame->len = xfer->xfer_size;
			} else {
				frame->len = (CASCADE_FRAME_LEN_MAX >> 2);
			}

			frame->handled_size = 0;
			frame->crc_valid = 0;
			frame->payload_done = 0;
			frame->crc_done = 0;

			frame->header_done = 1;

		} else {
			/* TODO: record error, buffer will overflow,
			 * ignore the package! */
			EMBARC_PRINTF("Error: cascade rx buffer overflow!");
		}
	}
}

void cascade_tx_next_frame_init(cs_xfer_mng_t *xfer)
{
	uint32_t total_size, handled_size, remain_size;

	uint32_t *data = NULL;
	cs_frame_t *frame = &xfer->cur_frame;

	if (NULL != xfer) {
		total_size = xfer->xfer_size;
		handled_size = (xfer->cur_frame_id + 1) * (CASCADE_FRAME_LEN_MAX >> 2);

		remain_size = total_size - handled_size;
		data = xfer->data + handled_size;

		frame->payload = data;

		if (remain_size <= CASCADE_FRAME_LEN_MAX >> 2) {
			frame->len = remain_size;
		} else {
			frame->len = (CASCADE_FRAME_LEN_MAX >> 2);
		}
		frame->header = (CASCADE_HEADER_MAGIC << 16) | frame->len;

		frame->handled_size = 0;
		//frame->crc_valid = 0;
		frame->header_done = 0;
		frame->payload_done = 0;
		frame->crc_done = 0;

		xfer->cur_frame_id += 1;
	}
}

void cascade_rx_next_frame_init(cs_xfer_mng_t *xfer, uint32_t frame_len)
{
	uint32_t handled_size, remain_size;

	if ((NULL != xfer) || (frame_len <= (CASCADE_FRAME_LEN_MAX >> 2))) {
		cs_frame_t *frame = &xfer->cur_frame;

		handled_size = (xfer->cur_frame_id + 1) * (CASCADE_FRAME_LEN_MAX >> 2);
		frame->payload = xfer->data + handled_size;

		remain_size = xfer->xfer_size - handled_size;
		if (remain_size <= CASCADE_FRAME_LEN_MAX >> 2) {
			frame->len = remain_size;
		} else {
			frame->len = (CASCADE_FRAME_LEN_MAX >> 2);
		}

		frame->handled_size = 0;
		//frame->crc_valid = 0;
		frame->payload_done = 0;
		frame->crc_done = 0;
		frame->header_done = 1;

		xfer->cur_frame_id += 1;

	} else {
		/* TODO: record error! */
	}
}


void cascade_frame_part_update(cs_frame_t *frame, uint32_t split_cnt)
{
	if (NULL != frame) {
		frame->handled_size += split_cnt;
		if (frame->handled_size >= frame->len) {
			frame->payload_done = 1;
		}
	}
}

void cascade_frame_reset(cs_frame_t *frame)
{
	if (NULL != frame) {
		frame->len = 0;
		frame->handled_size = 0;
		frame->payload = NULL;
		frame->header_done = 0;
		frame->payload_done = 0;
		frame->crc_done = 0;
		//frame->crc_valid = 0;
	}
}
