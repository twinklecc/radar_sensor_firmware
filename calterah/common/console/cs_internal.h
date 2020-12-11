#ifndef _CS_INTERNAL_H_
#define _CS_INTERNAL_H_

#define CFG_CS_RX_MODULE_PORT_ID	(0)

#define CS_RX_BUF_LEN_MAX	(0x1000)

/* tx buffer logic id range:
 * 0-15: user context range.
 * 16-31: task context range.
 * 32-39: interrupt context range.
 * 40: exception buffer id(virtual).
 * */
#define CS_TX_BUFFER_ID_INVALID	(0xFFFFFFFF)
#define CS_TASK_BUF_CNT_MAX	(16)
#define CS_TASK_BUF_LEN_MAX	(0x400)
#define CS_INT_BUF_CNT_MAX	(8)
#define CS_INT_BUF_LEN_MAX	(0x100)
#define CS_USER_BUF_CNT_MAX	(16)

/* transmit request queue: size & length. */
#define RX_MSG_QUEUE_ITEM_SIZE	(12)
#define RX_MSG_QUEUE_ITEM_LEN	(8)


#define CS_USER_BUF_ID_START	(0)
#define CS_USER_BUF_ID_END	(CS_USER_BUF_CNT_MAX)

#define CS_TASK_BUF_ID_START	(CS_USER_BUF_CNT_MAX)
#define CS_TASK_BUF_ID_END	(CS_USER_BUF_CNT_MAX + CS_TASK_BUF_CNT_MAX)

#define CS_INT_BUF_ID_START	(CS_USER_BUF_CNT_MAX + CS_TASK_BUF_CNT_MAX)
#define CS_INT_BUF_ID_END	(CS_USER_BUF_CNT_MAX + CS_TASK_BUF_CNT_MAX + CS_INT_BUF_CNT_MAX)


/* data will be transmitted can hang up.
 * @state: idle,
 * @used_size: the count of transmitted data.
 * @buf: the location of data.
 * @len: the length of data.
 * @timestamp: the time that the hooker has been applied.
 * */
typedef struct {
	uint16_t state;
	uint16_t used_size;

	void *buf;
	uint32_t len;

	uint64_t timestamp;
	void(*tx_end_callback)(void);
}cs_txbuf_hooker_t;


/* the console receiver:
 * @hw_type, UART/SPI/...
 * @port_id, the hardware port of hw_type.
 * @buf_status, normal or overflow.
 * @buf_rd_pos, read pointer, used to check whether overflow.
 * @buf, the buffer manager for low level(HAL).
 * @msg_queue, while receiving data, insert its location in it.
 * */
typedef struct  {
	//uint8_t hw_type;
	uint8_t port_id;

	uint8_t buf_status;
	uint32_t buf_rd_pos;
	uint32_t buf_rp_pos;
	DEV_BUFFER buf;

	//uint16_t range_l;
	//uint16_t range_h;

	QueueHandle_t msg_queue;
} cs_receiver_t;


/* the console transmitter:
 * @state: idle/busy.
 * @order_ovflw: indicate whether push order count is overflow.
 * @cur_buf_id: indicate the current txbuf_hooker id.
 * @buf: the buffer the the lower layer is handling.
 * @cur_userbuf_id:
 * @userbuf_pop_id: 
 *     indicate the current user txbuf_hooker id.
 * @push_order_cnt:
 *     indicate the last push order.
 * @pop_order_cnt:
 *     indicate the last pop order.
 * */
typedef struct {
	uint8_t state;

	uint8_t order_ovflw;

	uint8_t cur_buf_id;
	DEV_BUFFER buf;

	uint32_t cur_userbuf_id;
	uint32_t userbuf_pop_id;

	uint64_t push_order_cnt;
	uint64_t pop_order_cnt;
//	QueueHandle_t msg_queue;
} cs_transmitter_t;

#endif
