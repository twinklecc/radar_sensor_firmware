#ifndef _DW_UART_H_
#define _DW_UART_H_

#define UART_TX								(1)
#define UART_RX								(2)

#define DW_UART_MODEM_STS_INT				(1 << 3)
#define DW_UART_RECEIVER_LINE_STS_INT		(1 << 2)
#define DW_UART_TX_HOLDING_REG_EMPTY_INT	(1 << 1)
#define DW_UART_RX_DATA_AVAILABLE_INT		(1 << 0)

typedef struct dw_uart_frame_format {
	uint8_t data_bits;
	uint8_t parity;
	uint8_t stop_bits;
	uint8_t reserved;
} dw_uart_format_t;

typedef struct dw_uart_descriptor {
	uint32_t base;
	uint32_t ref_clock;

	uint32_t int_no;

	void *ops;
} dw_uart_t;

typedef struct dw_uart_operations {
	int32_t (*version)(dw_uart_t *dw_uart);

	int32_t (*write)(dw_uart_t *dw_uart, uint8_t *buf, uint32_t len);
	int32_t (*read)(dw_uart_t *dw_uart, uint8_t *buf, uint32_t len);

	int32_t (*format)(dw_uart_t *dw_uart, dw_uart_format_t *format);
	int32_t (*fifo_config)(dw_uart_t *dw_uart, uint32_t enable, \
			uint32_t rx_threshold, uint32_t tx_threshold);
	int32_t (*baud)(dw_uart_t *dw_uart, uint32_t baud);

	int32_t (*int_enable)(dw_uart_t *dw_uart, int enable, int mask);
	int32_t (*int_id)(dw_uart_t *dw_uart);

	int32_t (*line_status)(dw_uart_t *dw_uart);
	int32_t (*status)(dw_uart_t *dw_uart, uint32_t status);

	int32_t (*fifo_flush)(dw_uart_t *dw_uart, uint32_t channel);
} dw_uart_ops_t;


typedef enum dw_uart_parity {
	UART_NO_PARITY = 0,
	UART_ODD_PARITY,
	UART_EVEN_PARITY
} dw_uart_parity_t;

typedef enum dw_uart_data_bits {
	UART_CHAR_MIN_BITS = 5,
	UART_CHAR_5BITS = UART_CHAR_MIN_BITS,
	UART_CHAR_6BITS,
	UART_CHAR_7BITS,
	UART_CHAR_8BITS,
	UART_CHAR_MAX_BITS,
	UART_CHAR_INVALID,
} uart_data_bits_t;

typedef enum dw_uart_stop_bits {
	UART_STOP_1BIT = 0,
	UART_STOP_1BIT5,
	UART_STOP_2BITS
} uart_stop_bits_t;

typedef enum dw_uart_line_status {
	UART_LINE_ADDR_RECEIVED = 0,
	UART_LINE_RX_FIFO_ERR,
	UART_LINE_TX_REAL_EMPTY,
	UART_LINE_TX_HOLDING_REG_EMPTY,
	UART_LINE_BREAK_OCCURED,
	UART_LINE_FRAME_ERR,
	UART_LINE_PARITY_ERR,
	UART_LINE_OVERRUN_ERR,
	UART_LINE_DATA_READY,
	UART_LINE_ALL_STS,
	UART_LINE_INVALID_STS
} uart_line_sts_t;

typedef enum dw_uart_status_t {
	UART_RX_FIFO_FULL = 0,
	UART_RX_FIFO_NOT_EMPTY,
	UART_TX_FIFO_EMPTY,
	UART_TX_FIFO_NOT_FULL,
	UART_BUSY,
	UART_ALL_STS
} uart_status_t;

typedef enum {
	IID_MODEM_STATUS = 0,
	IID_NO_INT_PENDING,
	IID_THR_EMPTY,
	IID_RX_DATA_AVAIL = 4,
	IID_RX_LINE_STATUS = 6,
	IID_BUSY_DETECT,
	IID_CHAR_TIMEOUT = 0xc
} dw_uart_iid_t;
int32_t dw_uart_install_ops(dw_uart_t *dw_uart);
#endif
