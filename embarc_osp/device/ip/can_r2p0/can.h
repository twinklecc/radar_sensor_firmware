#ifndef _CAN_H_
#define _CAN_H_

#define CAN_INT_TIMEOUT_OCCURED		(1 << 25)
#define CAN_INT_ACCESS_PROTECT_REG	(1 << 24)
#define CAN_INT_PROTOCOL_ERR		(1 << 23)
#define CAN_INT_BUS_OFF			(1 << 22)
#define CAN_INT_WARNING			(1 << 21)
#define CAN_INT_ERR_PASSIVE		(1 << 20)
#define CAN_INT_ERR_LOGGING_OVERFLOW	(1 << 19)
#define CAN_INT_BIT_ERR_UNCORRECTED	(1 << 18)
#define CAN_INT_BIT_ERR_CORRECTED	(1 << 17)
#define CAN_INT_RAM_ACCESS_FAIL		(1 << 16)
#define CAN_INT_TIMESTAMP_WRAP_AROUND	(1 << 15)
#define CAN_INT_RX_NEW_MESSAGE		(1 << 14)
#define CAN_INT_RX_FIFO_LOST		(1 << 13)
#define CAN_INT_RX_FIFO_REAL_FULL	(1 << 12)
#define CAN_INT_RX_FIFO_FULL		(1 << 11)
#define CAN_INT_RX_FIFO_EMPTY		(1 << 10)
#define CAN_INT_TX_CANCEL_FINISHED	(1 << 9)
#define CAN_INT_TX_COMPLISHED		(1 << 8)
#define CAN_INT_TX_FIFO_LOST		(1 << 7)
#define CAN_INT_TX_FIFO_REAL_FULL	(1 << 6)
#define CAN_INT_TX_FIFO_FULL		(1 << 5)
#define CAN_INT_TX_FIFO_EMPTY		(1 << 4)
#define CAN_INT_TX_EVENT_FIFO_LOST	(1 << 3)
#define CAN_INT_TX_EVENT_FIFO_REAL_FULL	(1 << 2)
#define CAN_INT_TX_EVENT_FIFO_FULL	(1 << 1)
#define CAN_INT_TX_EVENT_FIFO_EMPTY	(1 << 0)

#define CAN_INT_RX_FIFO_FILLED	(CAN_INT_RX_FIFO_REAL_FULL | CAN_INT_RX_FIFO_FULL)
#define CAN_INT_TX_FIFO_READY	(CAN_INT_TX_FIFO_EMPTY | CAN_INT_TX_EVENT_FIFO_EMPTY)


typedef enum can_dma_request {
	dma_rx_req = 0,
	dma_tx_req
} can_dma_req_t;


typedef struct can_descriptor {
	uint32_t base;

	uint32_t id;

	uint8_t int_line[4];
	uint32_t int_line_bitmap[4];

	can_dma_req_t dma_req;

	void *ops;
} can_t;

typedef struct can_protocol_config {
	uint8_t fd_operation;
	uint8_t bit_rate_switch;
	uint8_t tx_delay_compensate;
	uint8_t auto_retransmission;
} can_protocol_cfg_t;

typedef struct can_baud_rate_parameters {
	uint8_t sync_jump_width;
	uint8_t bit_rate_prescale;
	uint8_t segment1;
	uint8_t segment2;
} can_baud_t;

typedef enum {
	eSTANDARD_FRAME = 0,
	eEXTENDED_FRAME
} eCAN_FRAME_FORMAT;
typedef enum {
	eDATA_FRAME = 0,
	eREMOTE_FRAME
} eCAN_FRAME_TYPE;
typedef enum {
	eELEMENT_DIS = 0,
	eRANGE,
	eCLASSIC,
	eDUAL_ID
} eCAN_STD_FILTER_TYPE;
typedef enum {
	eRX_FIFO = 0,
	eRX_BUF,
	eSPEC_RX_BUF,
	eREJ_ID
} eCAN_STD_FILTER_ELEMENT_CFG;
typedef struct can_id_filter_parameters {
	uint32_t frame_format;
	uint32_t filter_size;
	uint32_t mask;
	bool reject_no_match;
	bool reject_remote;
} can_id_filter_param_t;

typedef struct can_fifo_parameters {
	uint32_t watermark;
	uint32_t size;
	uint32_t mode;
} can_fifo_param_t;
typedef struct can_id_filter_element {
	uint32_t frame_format;
	uint32_t filter_type;
	uint32_t filter_cfg;
	uint32_t filter_id0;
	uint32_t filter_id1;
} can_id_filter_element_t;

typedef struct can_operation {
	int32_t (*dma_enable)(can_t *can, uint32_t rx, uint32_t tx);
	int32_t (*ecc_enable)(can_t *can, uint32_t en);
	int32_t (*reset)(can_t *can, uint32_t reset);

	int32_t (*operation_mode)(can_t *can, uint32_t mode);

	int32_t (*tx_delay_compensation)(can_t *can, uint32_t value, uint32_t offset, uint32_t win_len);
	int32_t (*protocol_config)(can_t *can, can_protocol_cfg_t *cfg);
	int32_t (*timestamp_config)(can_t *can, uint32_t prescale, uint32_t mode);
	int32_t (*timeout_config)(can_t *can, uint32_t enable, uint32_t period, uint32_t select);
	int32_t (*baud)(can_t *can, uint32_t fd_enabled, can_baud_t *param);
	int32_t (*id_filter_config)(can_t *can, can_id_filter_param_t *param);
	int32_t (*data_field_size)(can_t *can, uint32_t rx_or_tx, uint32_t fifo_dfs, uint32_t buf_dfs);
	int32_t (*fifo_config)(can_t *can, uint32_t fifo_type, can_fifo_param_t *param);

	int32_t (*rx_buf_status)(can_t *can, uint32_t buf_id);
	int32_t (*rx_buf_clear)(can_t *can, uint32_t buf_id);
	int32_t (*read_frame)(can_t *can, uint32_t buf_id, uint32_t *buf, uint32_t frame_size);
	int32_t (*read_frame_blocked)(can_t *can, uint32_t buf_id, uint32_t *buf, uint32_t frame_size);

	int32_t (*tx_buf_request_pending)(can_t *can, uint32_t buf_id);
	int32_t (*tx_buf_add_request)(can_t *can, uint32_t buf_id);
	int32_t (*tx_buf_cancel_request)(can_t *can, uint32_t buf_id);
	int32_t (*tx_buf_transmission_occurred)(can_t *can, uint32_t buf_id);
	int32_t (*tx_buf_cancel_finished)(can_t *can, uint32_t buf_id);
	int32_t (*tx_buf_transmission_int_enable)(can_t *can, uint32_t buf_id, uint32_t enable);
	int32_t (*tx_buf_cancel_trans_int_enable)(can_t *can, uint32_t buf_id, uint32_t enable);
	int32_t (*write_frame)(can_t *can, uint32_t buf_id, uint32_t *buf, uint32_t frame_size);

	int32_t (*fifo_status)(can_t *can, uint32_t fifo_type, uint32_t fifo_sts);
	int32_t (*fifo_read)(can_t *can, uint32_t fifo_type, uint32_t *buf, uint32_t len);

	int32_t (*sleep_ack)(can_t *can);
	uint32_t (*timestamp_value)(can_t *can);
	int32_t (*error_counter)(can_t *can, uint32_t type);
	int32_t (*protocol_status)(can_t *can, uint32_t type);
	int32_t (*ecc_error_status)(can_t *can);

	int32_t (*int_status)(can_t *can, uint32_t mask);
	int32_t (*int_clear)(can_t *can, uint32_t mask);
	int32_t (*int_enable)(can_t *can, uint32_t enable, uint32_t mask);
	int32_t (*int_line_select)(can_t *can, uint32_t int_source, uint32_t int_line);
	int32_t (*int_line_enable)(can_t *can, uint32_t int_line, uint32_t enable);

	int32_t (*test_transmit)(can_t *can, uint32_t data);
	int32_t (*test_receive)(can_t *can);

	uint32_t (*version)(can_t *can);
	int32_t (*id_filter_element)(can_t *can, uint32_t index, can_id_filter_element_t *param);
} can_ops_t;


typedef enum can_mode {
	NORMAL_MODE = 0,
	CONFIG_MODE,
	RESTRICTED_MODE,
	MONITOR_MODE,
	SLEEP_MODE,
	TEST_MODE,
	LOOP_MODE
} can_mode_t;

typedef enum can_fifo_type {
	CAN_RX_FIFO = 0,
	CAN_TX_FIFO,
	CAN_TX_EVENT_FIFO
} can_fifo_type_t;

typedef enum can_fifo_status {
	CAN_FIFO_ALL = 0,
	CAN_FIFO_PUT_INDEX,
	CAN_FIFO_GET_INDEX,
	CAN_FIFO_LEVEL,
	CAN_FIFO_MESSAGE_LOST,
	CAN_FIFO_FULL,
	CAN_FIFO_EMPTY,
	CAN_FIFO_ACK,
	CAN_FIFO_INVALID
} can_fifo_status_t;

typedef enum can_fifo_mode {
	FIFO_BLOCK_MODE = 0,
	FIFO_OVERWRITE_MODE,
	FIFO_INVALID_MODE
} can_fifo_mode_t;

typedef enum can_timeout_mode {
	TIMEOUT_MODE_CONTINUOUS = 0,
	TIMEOUT_MODE_TX_EVENT_FIFO,
	TIMEOUT_MODE_RX_FIFO,
	TIMEOUT_MODE_INVALID
} can_timeout_mode_t;

typedef enum can_protocol_status_type {
	PROTOCOL_STS_ALL = 0,
	PROTOCOL_STS_LAST_ERR_CODE,
	PROTOCOL_STS_FD_LAST_RX_ESI,
	PROTOCOL_STS_FD_LAST_RX_BRS,
	PROTOCOL_STS_FD_RX_MESSAGE,
	PROTOCOL_STS_EXCEPTION_EVENT,
	PROTOCOL_STS_ACTIVE,
	PROTOCOL_STS_ERR_PASSIVE,
	PROTOCOL_STS_WARNING,
	PROTOCOL_STS_BUS_OFF,
	PROTOCOL_STS_INVALID
} can_protocol_status_t;

typedef enum can_error_counter_type {
	ERROR_TYPE_LOGGING = 0,
	ERROR_TYPE_RECEIVE,
	ERROR_TYPE_TRANSMIT
} can_error_counter_type_t;

typedef enum can_timestamp_counter_mode {
	TIMESTAMP_ALWAYS = 0,
	TIMESTAMP_INCREMENT,
	TIMESTAMP_EXTERNAL
} can_timestamp_mode;


/* frame size mapping to data field size in register. */
static inline uint32_t can_fs2dfs(uint32_t frame_size)
{
	uint32_t dfs = (frame_size >> 2);
	if (dfs <= 6) {
		dfs -= 2;
	} else {
		dfs = ((dfs - 8) >> 2) + 5;
	}

	return dfs;
}

int32_t can_install_ops(can_t *can);
can_baud_t *can_get_baud(uint32_t ref_clock, uint32_t baud_rate);
#endif
