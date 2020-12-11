#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "arc_exception.h"
#include "can.h"

#include "can_bus_config.h"
#include "can_dll.h"


#define CAN_DLL_HEADER0(id, xtd, esi) (\
		((id) & 0xfffffff) | (((xtd) & 0x1) << 30) |\
		(((esi) & 0x1) << 31)\
		)
#define CAN_DLL_REMOTE_HEADER0(id, rtr, xtd, esi) (\
		((id) & 0xfffffff) | (1 << 29) |\
		(((xtd) & 0x1) << 30) | (((esi) & 0x1) << 31)\
		)
#define CAN_DLL_HEADER1(fdf, brs, dlc) (\
		(((fdf) & 0x1) << 21) | (((brs) & 0x1) << 20) |\
		(((dlc) & 0xf) << 16)\
		)

#define CAN_DLL_RTR_FROM_HEADER(header0) (((header0) >> 29) & 0x1)
#define CAN_DLL_ID_FROM_HEADER(header0) ((header0) & 0xfffffff)
#define CAN_DLL_DLC_FROM_HEADER(header1) (((header1) >> 16) & 0xF)

#define BYTES2WORD(data) (\
		(((data)[0]) << 24) | (((data)[1]) << 16) |\
		(((data)[2]) << 8) | ((data)[3])\
		)
#define WORD2BYTES(data, word) do {\
		(data)[0] = ((word) >> 24) & 0xFF;\
		(data)[1] = ((word) >> 16) & 0xFF;\
		(data)[2] = ((word) >> 8) & 0xFF;\
		(data)[3] = (word) & 0xFF;\
	} while (0)

#define CAN_DLL_TX_BUFFER_CNT_MAX	(64)

/* using only in this file. */
typedef struct can_dll_message {
	uint32_t header0;
	uint32_t header1;
	uint32_t data[CAN_BUS_FRAME_SIZE >> 2];

	uint32_t xfer_status;
} dll_message_t;


/* point to the can port descriptor. */
static can_t *can_dev_ptr = NULL;

/* these callbacks as dll sevices. */
static dll_indication can_dll_indication;
static dll_confirm can_dll_confirm;
static dll_remote_indication can_dll_r_indication;
static dll_remote_confirm can_dll_r_confirm;

static uint32_t tx_filled_top;
static uint32_t tx_finish_top;
static dll_message_t dll_tx_message[CAN_DLL_TX_BUFFER_CNT_MAX];

static uint32_t rx_report_top;
static uint8_t dll_rx_buffer[CAN_BUS_FRAME_SIZE * CAN_DLL_FRAME_BUF_CNT_MAX];


static void can_dll_tx_isr(void *param);
static void can_dll_rx_isr(void *param);
static void can_dll_error_isr(void *param);

extern can_baud_config_find(uint32_t rate);
int32_t can_dll_init(void)
{
	int32_t result = E_OK;

	uint32_t int_bitmap = 0;
	can_ops_t *can_ops = NULL;

	can_dev_ptr = can_get_dev(CAN_BUS_DEV_ID);
	if (NULL == can_dev_ptr) {
		result = E_NOEXS;
	} else {
		can_baud_t *baud_ptr = can_baud_config_find(CAN_BUS_BUAD_RATE);
		if (NULL == baud_ptr) {
			result = E_NSUP;
		} else {
			can_ops = (can_ops_t *)can_dev_ptr->ops;
			if (NULL == can_ops) {
				result = E_NOOPS;
			} else {
				if ((CAN_BUS_FRAME_SIZE % 4) || \
				    (CAN_BUS_FRAME_SIZE < 8) || \
				    (CAN_BUS_FRAME_SIZE > 64)) {
					result = E_PAR;
				}
			}
		}
	}

	if ((E_OK == result)) {
		if ((NULL == can_ops->baud) || (NULL == can_ops->data_field_size)) {
			result = E_NOOPS;
		} else {
			result = can_ops->baud(can_dev_ptr, CAN_BUS_FD, baud_ptr);
			if (E_OK == result) {
				uint32_t dfs = can_fs2dfs(CAN_BUS_FRAME_SIZE);
				result = can_ops->data_field_size(can_dev_ptr, 0, dfs, dfs);
				if (E_OK == result) {
					result = can_ops->data_field_size(can_dev_ptr, 1, dfs, dfs);
				}
			}
		}
	}

	/* install interrupt for transmit. */
	if (E_OK == result) {
		result = int_handler_install(can_dev_ptr->int_line[0], can_dll_tx_isr);
		if (E_OK == result) {
			result = int_enable(can_dev_ptr->int_line[0]);
			if (E_OK == result) {
				/*
				int_bitmap = CAN_INT_TX_COMPLISHED;
				result = can_ops->int_enable(can_ptr, enable, int_bitmap);
				*/
			}
		}
	}

	/* install interrupt for receive. */
	if (E_OK == result) {
		result = int_handler_install(can_dev_ptr->int_line[1], can_dll_rx_isr);
		if (E_OK == result) {
			result = int_enable(can_dev_ptr->int_line[1]);
			if (E_OK == result) {
				int_bitmap = CAN_INT_RX_NEW_MESSAGE;
				result = can_ops->int_enable(can_ptr, enable, int_bitmap);
			}
		}
	}

	/*install error handle isr. */
	if (E_OK == result) {
		result = int_handler_install(can_dev_ptr->int_line[2], can_dll_error_isr);
		if (E_OK == result) {
			result = int_enable(can_dev_ptr->int_line[2]);
			if (E_OK == result) {
				/* TODO: Fix me! */
				result = can_ops->int_enable(can_ptr, enable, 0);
			}
		}
	}

	return result;
}


/*
 * description: sevice.request on data link layer.
 * @id: identifier of the frame.
 * @dlc: data length code.
 * @data: indicate the data of the frame.
 * */
int32_t can_dll_request(uint32_t id, uint32_t dlc, uint8_t *data)
{
	int32_t result = E_OK;

	dll_message_t *message_ptr = NULL;
	uint32_t f_data, xtd, esi = 0;

	can_ops_t *can_ops = NULL;

	if ((id >> 29) || (dlc > CAN_BUS_FRAME_SIZE) || (NULL == data)) {
		result = E_PAR;
	} else {
		if((NULL == can_dev_ptr) || (NULL == can_dev_ptr->ops)) {
			result = E_NOEXS;
		} else {
			can_ops = (can_ops_t *)can_dev_ptr->ops;
			if (NULL == protocol_status) {
				result = E_NOOPS;
			} else {
				result = can_ops->protocol_status(can_dev_ptr, \
						PROTOCOL_STS_ERR_PASSIVE);
				if (result >= 0) {
					esi = result;
					result = E_OK;
				}
			}
		}

		if (E_OK == result) {
			if ((CAN_BUS_FRAME_FORMAT == CAN_CEFF) || \
					(CAN_BUS_FRAME_FORMAT == CAN_FEFF)) {
				xtd = 1;
			} else {
				xtd = 0;
			}
		}

	}

	if (E_OK == result) {
		int cnt, idx = 0;
		uint32_t buf_id = tx_filled_top;

		uint32_t cpu_sts = arc_lock_save();
		message_ptr = &dll_tx_message[tx_filled_top++];
		if (tx_filled_top >= CAN_DLL_TX_BUFFER_CNT_MAX) {
			tx_filled_top = 0;
		}
		arc_unlock_restore(cpu_sts);

		message_ptr->header0 = CAN_DLL_HEADER0(id, xtd, esi);
		message_ptr->header1 = CAN_DLL_HEADER1(CAN_BUS_FD, CAN_BUS_FD_BRS, dlc);

		f_data = 0;
		for (cnt = 0; cnt < dlc/4; cnt++) {
			message_ptr->data[idx++] = BYTES2WORD(data);
			data += 4;
		}
		for (cnt = 0; cnt < dlc % 4; cnt++) {
			f_data |= (*data++ << (cnt << 3));
		}
		message_ptr->data[idx++] = f_data;

		if (NULL == can_ops->write_frame) {
			result = E_NOOPS;
		} else {
			/* TODO: whether need to lock? */
			result = can_ops->write_frame(can_dev_ptr, buf_id, \
					(uint32_t *)message_ptr, CAN_BUS_FRAME_SIZE + 2);
			if (E_OK == result) {
				result = can_ops->int_enable(can_dev_ptr, \
						1, CAN_INT_TX_COMPLISHED);
			}
		}
	}

	return result;
}

int32_t can_dll_remote_request(uint32_t id, uint32_t dlc)
{
	int32_t result = E_OK;

	dll_message_t *message_ptr = NULL;
	uint32_t xtd, esi = 0;

	if ((id >> 29) || (dlc > CAN_BUS_FRAME_SIZE)) {
		result = E_PAR;
	} else {
		if((NULL == can_dev_ptr) || (NULL == can_dev_ptr->ops)) {
			result = E_NOEXS;
		} else {
			can_ops = (can_ops_t *)can_dev_ptr->ops;
			if (NULL == protocol_status) {
				result = E_NOOPS;
			} else {
				result = can_ops->protocol_status(can_dev_ptr, \
						PROTOCOL_STS_ERR_PASSIVE);
				if (result >= 0) {
					esi = result;
					result = E_OK;
				}
			}
		}

		if (E_OK == result) {
			if ((CAN_BUS_FRAME_FORMAT == CAN_CEFF) || \
					(CAN_BUS_FRAME_FORMAT == CAN_FEFF)) {
				xtd = 1;
			} else {
				xtd = 0;
			}
		}
	}

	if (E_OK == result) {
		uint32_t buf_id = tx_filled_top;

		uint32_t cpu_sts = arc_lock_save();
		message_ptr = &dll_tx_message[tx_filled_top++];
		if (tx_filled_top >= CAN_DLL_TX_BUFFER_CNT_MAX) {
			tx_filled_top = 0;
		}
		arc_unlock_restore(cpu_sts);

		message_ptr->header0 = CAN_DLL_REMOTE_HEADER0(id, xtd, esi);
		message_ptr->header1 = CAN_DLL_HEADER1(CAN_BUS_FD, CAN_BUS_FD_BRS, dlc);

		if (NULL == can_ops->write_frame) {
			result = E_NOOPS;
		} else {
			result = can_ops->write_frame(can_dev_ptr, buf_id, \
					(uint32_t *)message_ptr, CAN_BUS_FRAME_SIZE + 2);
			if (E_OK == result) {
				result = can_ops->int_enable(can_dev_ptr, \
						1, CAN_INT_TX_COMPLISHED);
			}
		}
	}

	return result;
}

int32_t can_dll_indication_register(dll_indication func)
{
	int32_t result = E_OK;

	if (NULL == func) {
		result = E_PAR;
	} else {
		can_dll_indication = func;
	}

	return result;
}

int32_t can_dll_confirm_register(dll_confirm func)
{
	int32_t result = E_OK;

	if (NULL == func) {
		result = E_PAR;
	} else {
		can_dll_confirm = func;
	}

	return result;
}

int32_t can_dll_remote_indication_register(dll_remote_indication func)
{
	int32_t result = E_OK;

	if (NULL == func) {
		result = E_PAR;
	} else {
		can_dll_r_indication = func;
	}

	return result;
}

int32_t can_dll_remote_confirm_register(dll_remote_confirm func)
{
	int32_t result = E_OK;

	if (NULL == func) {
		result = E_PAR;
	} else {
		can_dll_r_confirm = func;
	}

	return result;
}

/*
 * description: interrupt service routine for transmitting.
 * @param: resevered.
 * */
static void can_dll_tx_isr(void *param)
{
	int32_t result = E_OK;

	can_ops_t *can_ops = NULL;

	dll_tx_message_t *message_ptr = &dll_tx_message[tx_finish_top++];
	if (tx_finish_top >= CAN_DLL_TX_BUFFER_CNT_MAX) {
		tx_finish_top = 0;
	}

	if ((NULL == can_dev_ptr)) {
		result = E_NOEXS;
	} else {
		if (NULL == can_dev_ptr->ops) {
			result = E_NOOPS;
		} else {
			/* there is no frame transmitting, disable interrupt. */
			can_ops = (can_ops_t *)can_dev_ptr->ops;
			if (tx_finish_top >= tx_filled_top) {
				result = can_ops->int_enable(can_ptr, 0, CAN_INT_TX_COMPLISHED);
			}
		}
	}

	/* TODO: need to check the hardware's current status? */
	if (E_OK == result) {
	}

	if (E_OK == result) {
		if (CAN_DLL_RTR_FROM_HEADER(message_ptr->header0)) {
			result = can_dll_remote_confirm(\
					CAN_DLL_ID_FROM_HEADER(message_ptr->header0), \
					CAN_XFER_DONE);
		} else {
			result = can_dll_confirm(\
					CAN_DLL_ID_FROM_HEADER(message_ptr->header0), \
					CAN_XFER_DONE);
		}
	}

	if (E_OK != result) {
		/* TODO: Error happened, panic! */
	}
}

static void can_dll_rx_isr(void *param)
{
	int32_t result = E_OK;

	can_ops_t *can_ops = NULL;

	uint32_t buf_id = rx_report_top;
	dll_message_t message;

	uint8_t *data_ptr = &dll_rx_buffer[CAN_BUS_FRAME_SIZE * rx_report_top++];
	if (rx_report_top >= CAN_DLL_FRAME_BUF_CNT_MAX) {
		rx_report_top = 0;
	}

	if ((NULL == can_dev_ptr)) {
		result = E_NOEXS;
	} else {
		if (NULL == can_dev_ptr->ops) {
			result = E_NOOPS;
		} else {
			can_ops = (can_ops_t *)can_dev_ptr;
			if (NULL == can_ops->read_frame) {
				result = E_NOOPS;
			}
		}
	}

	/* read frame! */
	if (E_OK == result) {
		result = can_ops->read_frame(can_dev_ptr, buf_id, \
				(uint32_t *)&message, CAN_BUS_FRAME_SIZE + 2);
	}

	/* parse and report data! */
	if (E_OK == result) {
		uint32_t f_data, cnt = 0;
		uint32_t id = CAN_DLL_ID_FROM_HEADER(message.header0);
		uint32_t dlc = CAN_DLL_DLC_FROM_HEADER(message.header1);

		for (; cnt < dlc/4; cnt++) {
			WORD2BYTES(data_ptr, message.data[cnt]);
			data_ptr += 4;
		}
		f_data = message.data[cnt];
		for (cnt = 0; cnt < dlc % 4; cnt++) {
			*data_ptr++ = (f_data >> ((3 - cnt) << 3)) & 0xFF;
		}

		if (CAN_DLL_RTR_FROM_HEADER(message.header0)) {
			result = can_dll_r_indication(id, dlc, data_ptr);
		} else {
			result = can_dll_indication(id, dlc, data_ptr);
		}
	}

	if (E_OK != result) {
		/* TODO: system error, panic! */
	}
}

static void can_dll_error_isr(void *param)
{
	/* TODO: add in future! */
}
