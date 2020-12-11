#ifndef _UART_HAL_H_
#define _UART_HAL_H_

#include "dev_common.h"
#include "dw_uart.h"

typedef void (*xfer_callback)(void *);

typedef enum uart_callback_msg_id {
	UART_CB_RX_ERR = 0,
	UART_CB_RX_DONE,
	UART_CB_RX_NO_BUF,
	UART_CB_RX_BUF_FULL,
	UART_CB_TX_ERR,
	UART_CB_TX_DONE,
	UART_CB_TX_NO_BUF,
	UART_CB_TX_WORKING,
} uart_cb_msg_t;

typedef struct uart_ping_pong_buffer {
	void *buf;
	uint32_t len;
	uint32_t nof_pp_buf;
} uart_pp_buf_t;

typedef enum uart_data_buffer_state {
	DATA_BUF_IDLE = 0,
	DATA_BUF_FILLING,
	DATA_BUF_READY,
	DATA_BUF_INSTALLED,
	DATA_BUF_DONE
} uart_buf_state_t;

typedef struct uart_pp_buffer_header {
	uint8_t state;
	uint8_t used_size;
} uart_ppbuf_header_t;

int32_t uart_init(uint32_t id, uint32_t baud);

int32_t uart_baud_set(uint32_t id, uint32_t baud);

int32_t uart_frame_config(uint32_t id, dw_uart_format_t *ff);

int32_t uart_write(uint32_t id, uint8_t *data, uint32_t len);

int32_t uart_read(uint32_t id, uint8_t *data, uint32_t len);

int32_t uart_buffer_register(uint32_t id, dev_buf_type_t type, void *buffer);

int32_t uart_interrupt_enable(uint32_t id, dev_xfer_type_t type, uint32_t en);

int32_t uart_callback_register(uint32_t id, xfer_callback func);


#endif
