#include <string.h>
#include <stdarg.h>
#include "embARC_toolchain.h"
#include "FreeRTOS.h"
#include "task.h"
#include "embARC.h"
#include "embARC_debug.h"

#include "uart_hal.h"
#include "cs_internal.h"

#include "sw_trace.h"


/* whether use console printf. */
static uint32_t dbg_printf_flag = 0;

/* console receiver's buffer. */
static uint8_t cs_rx_buffer[CS_RX_BUF_LEN_MAX];

/* task context tx buffer. */
static uint8_t cs_task_txbuf[CS_TASK_BUF_CNT_MAX * CS_TASK_BUF_LEN_MAX];
/* interrupt context tx buffer. */
static uint8_t cs_int_txbuf[CS_INT_BUF_CNT_MAX * CS_INT_BUF_LEN_MAX];

/* tx buffer hooker. */
static cs_txbuf_hooker_t txbuf_hooker[CS_INT_BUF_ID_END];

static cs_receiver_t rt_cs_receiver;
static cs_transmitter_t rt_cs_transmitter;

static int32_t pre_printf(void);
static void post_printf(int32_t buf_id, uint32_t used_len);
static void cs_txbuf_install(void);
static void cs_isr_callback(void *params);


/* console transmitter initialize. */
static void cs_transmitter_init(void)
{
	uint32_t limit, idx, s_idx = CS_USER_BUF_CNT_MAX;

	/* hooker[0-15] is for application().
	 * firstly, register application data buffer on hooker.
	 * then, console core will install it onto lower layer(driver).
	 * lastly, hardware send these data outside in interrupt context.
	 * */
	for (idx = 0; idx < CS_USER_BUF_CNT_MAX; idx++) {
		txbuf_hooker[idx].state = 0;
		txbuf_hooker[idx].used_size = 0;
		txbuf_hooker[idx].buf = NULL;
		txbuf_hooker[idx].len = 0;
	}

	/* hooker[16-31] is for task LOG output(EMBARC_PRINTF).
	 * firstly, call pre_printf to alloc local buffer.
	 * secondly, xprintf parse the format onto loal buffer.
	 * then, call post_printf inform console core(data ready), or
	 *       install it onto lower layer(driver) directly.
	 * lastly, hardware send these data outside in interrupt context.
	 * */
	s_idx = CS_USER_BUF_CNT_MAX;
	limit = CS_USER_BUF_CNT_MAX + CS_TASK_BUF_CNT_MAX;
	for (idx = s_idx; idx < limit; idx++) {
		txbuf_hooker[idx].state = DATA_BUF_IDLE;
		txbuf_hooker[idx].used_size = 0;
		txbuf_hooker[idx].buf = cs_task_txbuf + ((idx - s_idx) * CS_TASK_BUF_LEN_MAX);
		txbuf_hooker[idx].len = CS_TASK_BUF_LEN_MAX;
	}

	/* hooker[32-39] is for interrupt LOG output(EMBARC_PRINTF).
	 * firstly, call pre_printf to alloc local buffer.
	 * secondly, xprintf parse the format onto loal buffer.
	 * then, call post_printf inform console core(data ready), or
	 *       install it onto lower layer(driver) directly.
	 * lastly, hardware send these data outside in interrupt context.
	 * */
	s_idx = CS_USER_BUF_CNT_MAX + CS_TASK_BUF_CNT_MAX;
	limit = CS_USER_BUF_CNT_MAX + CS_TASK_BUF_CNT_MAX + CS_INT_BUF_CNT_MAX;
	for (; idx < limit; idx++) {
		txbuf_hooker[idx].state = 0;
		txbuf_hooker[idx].used_size = 0;
		txbuf_hooker[idx].buf = cs_int_txbuf + ((idx - s_idx) * CS_TASK_BUF_LEN_MAX);
		txbuf_hooker[idx].len = CS_INT_BUF_LEN_MAX;
	}

	rt_cs_transmitter.state = 0;
	rt_cs_transmitter.cur_buf_id = 0;
	rt_cs_transmitter.order_ovflw = 0;
	rt_cs_transmitter.cur_userbuf_id = 0;
	rt_cs_transmitter.userbuf_pop_id = 0;

	rt_cs_transmitter.push_order_cnt = 0;
	rt_cs_transmitter.pop_order_cnt = 0;
	DEV_BUFFER_INIT(&rt_cs_transmitter.buf, NULL, 0);
}



/* console receiver descriptor. */
static void cs_receiver_init(void)
{
	//rt_cs_receiver.hw_type = CFG_CS_RX_MODULE_ID;
	rt_cs_receiver.port_id = CFG_CS_RX_MODULE_PORT_ID;

	rt_cs_receiver.buf_status = 0;
	rt_cs_receiver.buf_rd_pos = (uint32_t)cs_rx_buffer;
	rt_cs_receiver.buf_rp_pos = (uint32_t)cs_rx_buffer;
	DEV_BUFFER_INIT(&rt_cs_receiver.buf, cs_rx_buffer, CS_RX_BUF_LEN_MAX);
}

/* note: just call on interrupt context.
 * inform system that a new message is coming.
 * */
static void cs_rxdata_indication(void)
{
	DEV_BUFFER *dev_rx_buf = &rt_cs_receiver.buf;

	uint32_t cur_wr_pos = ((uint32_t)dev_rx_buf->buf) + dev_rx_buf->ofs;
	if (cur_wr_pos > rt_cs_receiver.buf_rp_pos) {
		BaseType_t higher_task_wkup = 0;

		DEV_BUFFER msg = {
			.buf = (void *)rt_cs_receiver.buf_rp_pos,
			.len = cur_wr_pos - rt_cs_receiver.buf_rp_pos
		};

		if (pdTRUE == xQueueSendFromISR(rt_cs_receiver.msg_queue, \
					&msg, &higher_task_wkup)) {
			if (higher_task_wkup) {
				portYIELD_FROM_ISR();
			}

			/* update the reported position. */
			rt_cs_receiver.buf_rp_pos = cur_wr_pos;
		} else {
			/* TODO: record error! */
			;
		}
	}
}

/* when the current buffer is full, reinstall a new one.
 * there are three situations as bellow,
 1.:
 *********************************************
 *	* current buffer *  next  *	     *
 *-------------------------------------------*
 *			 	  RD	     *
 * *******************************************
 2.:
 *********************************************
 * 	*	* current buffer *   next    *
 *-------------------------------------------*
 *	RD		 	  	     *
 *********************************************
 3.:
 *********************************************
 * next	*		*  current buffer    *
 *-------------------------------------------*
 *	RD		 	  	     *
 *********************************************
 * */
static void cs_rxbuf_reinstall(void)
{
	DEV_BUFFER *dev_rx_buf = &rt_cs_receiver.buf;

	uint32_t rd_pos = rt_cs_receiver.buf_rd_pos;

	//uint32_t pre_range_s = cs_rx_buffer;
	uint32_t pre_range_e = (uint32_t)dev_rx_buf->buf;
	uint32_t post_range_s = pre_range_e + dev_rx_buf->len;
	uint32_t post_range_e = (uint32_t)cs_rx_buffer + CS_RX_BUF_LEN_MAX;


	if (post_range_e - post_range_s) {
		if (rd_pos > post_range_s) {
			dev_rx_buf->buf = (void *)post_range_s;
			dev_rx_buf->len = rd_pos - post_range_s;
			dev_rx_buf->ofs = 0;
		} else {
			uint32_t buf_end = (uint32_t)cs_rx_buffer + CS_RX_BUF_LEN_MAX;
			dev_rx_buf->buf = (void *)post_range_s;
			dev_rx_buf->len = buf_end - post_range_s;
			dev_rx_buf->ofs = 0;
		}
	} else {
		dev_rx_buf->buf = cs_rx_buffer;
		dev_rx_buf->len = rd_pos - (uint32_t)cs_rx_buffer;
		dev_rx_buf->ofs = 0;
	}

	/* init the reported position. */
	rt_cs_receiver.buf_rp_pos = (uint32_t)dev_rx_buf->buf;
}

/* console hardware interrupt callback. */
static void cs_isr_callback(void *params)
{
	DEV_BUFFER *dev_rx_buf = &rt_cs_receiver.buf;
	DEV_BUFFER *dev_tx_buf = &rt_cs_transmitter.buf;

	/* firstly check whether receiver buffer is full. */
	if (NULL != dev_rx_buf->buf) {
		/* add new queue item. */
		cs_rxdata_indication();

		/* overflow! reinstall rx buffer. */
		if (dev_rx_buf->ofs >= dev_rx_buf->len) {
			cs_rxbuf_reinstall();
		}
	} else {
		/* record error info. */
		;
	}

	if (rt_cs_transmitter.state) {
		if (NULL != dev_tx_buf->buf) {
			/* check whether transmitter finished transmitting. */
			if (dev_tx_buf->ofs >= dev_tx_buf->len) {
				/* done: release current buffer. */
				uint32_t buf_id = rt_cs_transmitter.cur_buf_id;
				if (buf_id < CS_INT_BUF_ID_END) {
					txbuf_hooker[buf_id].state = DATA_BUF_IDLE;
					if (buf_id < CS_USER_BUF_ID_END) {
						if (txbuf_hooker[buf_id].tx_end_callback) {
								txbuf_hooker[buf_id].tx_end_callback();
						}
					}
				}

				/* re_install new buffer. */
				cs_txbuf_install();
			}
		} else {
			/* enable transmit interrupt. */
			/* re_install new buffer. */
			cs_txbuf_install();
		}
	}
}

/* note: call in task context.
 * application call this function to get information from console.
 * */
int32_t console_wait_newline(uint8_t *data, uint32_t wbuf_len)
{
	int32_t count = 0;

	DEV_BUFFER msg;

	QueueHandle_t queue_ptr = rt_cs_receiver.msg_queue;
	if (pdTRUE == xQueueReceive(queue_ptr, &msg, 1)) { // should not be 0, add one tick to disturb the idle task loop
		uint32_t buf_tail;
		uint32_t cpu_sts = arc_lock_save();
		if (msg.len <= wbuf_len) {
			memcpy(data, msg.buf, msg.len);
			count = msg.len;

			buf_tail = (uint32_t)cs_rx_buffer + CS_RX_BUF_LEN_MAX;
			rt_cs_receiver.buf_rd_pos += msg.len;
			if (rt_cs_receiver.buf_rd_pos >= buf_tail) {
				rt_cs_receiver.buf_rd_pos = (uint32_t)cs_rx_buffer;
			}
		} else {
			/* data buffer too small! */
			count = -1;
		}
		arc_unlock_restore(cpu_sts);
	}

	return count;
}


static int32_t cs_intbuf_alloc(void)
{
	int32_t result = E_SYS;

	uint32_t buf_id = CS_INT_BUF_ID_START;
	uint32_t buf_id_e = CS_INT_BUF_ID_END;

	uint32_t cpu_sts = arc_lock_save();
	uint64_t push_order = rt_cs_transmitter.push_order_cnt;
	if (rt_cs_transmitter.order_ovflw) {
		/* ignore the current allocate! */
		buf_id = CS_INT_BUF_ID_END;
	}

	for (; buf_id < buf_id_e; buf_id++) {
		if (DATA_BUF_IDLE == txbuf_hooker[buf_id].state) {
			txbuf_hooker[buf_id].state = DATA_BUF_FILLING;
			txbuf_hooker[buf_id].timestamp = push_order;
			result = (int32_t)buf_id;

			rt_cs_transmitter.push_order_cnt += 1;
			if (rt_cs_transmitter.push_order_cnt < push_order) {
				rt_cs_transmitter.order_ovflw = 1;
			}

			break;
		}
	}
	arc_unlock_restore(cpu_sts);

	return result;
}

static int32_t cs_txbuf_alloc(void)
{
	int32_t result = E_SYS;

	uint32_t buf_id = CS_TASK_BUF_ID_START;
	uint32_t buf_id_e = CS_TASK_BUF_ID_END;

	uint32_t cpu_sts = arc_lock_save();
	uint64_t push_order = rt_cs_transmitter.push_order_cnt;
	if (rt_cs_transmitter.order_ovflw) {
		while (rt_cs_transmitter.push_order_cnt >= \
		    rt_cs_transmitter.pop_order_cnt) {
			/* send too lowly, task waiting! */
			vTaskDelay(1);
		}
	}

	for (; buf_id < buf_id_e; buf_id++) {
		if (DATA_BUF_IDLE == txbuf_hooker[buf_id].state) {
			txbuf_hooker[buf_id].state = DATA_BUF_FILLING;
			txbuf_hooker[buf_id].timestamp = push_order;
			result = (int32_t)buf_id;

			rt_cs_transmitter.push_order_cnt += 1;
			if (rt_cs_transmitter.push_order_cnt < push_order) {
				rt_cs_transmitter.order_ovflw = 1;
			}
			break;
		}
	}
	arc_unlock_restore(cpu_sts);

	return result;
}

static int32_t cs_userbuf_alloc(void)
{
	int32_t result = E_SYS;

	uint32_t buf_id = rt_cs_transmitter.cur_userbuf_id;
	uint64_t push_order = rt_cs_transmitter.push_order_cnt;
	if (rt_cs_transmitter.order_ovflw) {
		while (rt_cs_transmitter.push_order_cnt >= \
		    rt_cs_transmitter.pop_order_cnt) {
			/* send too lowly, task waiting! */
			vTaskDelay(1);
		}
	}

	uint32_t cpu_sts = arc_lock_save();
	if (DATA_BUF_IDLE == txbuf_hooker[buf_id].state) {
		txbuf_hooker[buf_id].state = DATA_BUF_FILLING;
		txbuf_hooker[buf_id].timestamp = push_order;
		result = (int32_t)buf_id;

		rt_cs_transmitter.cur_userbuf_id += 1;
		if (CS_USER_BUF_ID_END <= rt_cs_transmitter.cur_userbuf_id) {
			rt_cs_transmitter.cur_userbuf_id = 0;
		}

		rt_cs_transmitter.push_order_cnt += 1;
		if (rt_cs_transmitter.push_order_cnt < push_order) {
			rt_cs_transmitter.order_ovflw = 1;
		}
	}
	arc_unlock_restore(cpu_sts);

	return result;
}

static int32_t cs_userbuf_ready(void)
{
	int32_t result = E_SYS;

	uint32_t buf_id = 0;

	uint64_t pop_order = rt_cs_transmitter.pop_order_cnt;

	uint32_t cpu_sts = arc_lock_save();
	for (; buf_id < CS_USER_BUF_ID_END; buf_id++) {
		if ((DATA_BUF_READY == txbuf_hooker[buf_id].state) && \
		    (pop_order == txbuf_hooker[buf_id].timestamp)) {
			result = (int32_t)buf_id;

			rt_cs_transmitter.pop_order_cnt += 1;
			if (rt_cs_transmitter.pop_order_cnt < pop_order) {
				if (rt_cs_transmitter.order_ovflw) {
					rt_cs_transmitter.order_ovflw = 0;
				}
			}
			break;
		}
	}
	arc_unlock_restore(cpu_sts);

	return result;
}

static int32_t cs_local_buf_ready(void)
{
	int32_t result = E_SYS;

	uint32_t buf_id = CS_TASK_BUF_ID_START;

	uint64_t pop_order = rt_cs_transmitter.pop_order_cnt;

	uint32_t cpu_sts = arc_lock_save();
	for (; buf_id < CS_INT_BUF_ID_END; buf_id++) {
		if ((DATA_BUF_READY == txbuf_hooker[buf_id].state) && \
		    (pop_order == txbuf_hooker[buf_id].timestamp)) {
			result = (int32_t)buf_id;

			rt_cs_transmitter.pop_order_cnt += 1;
			if (rt_cs_transmitter.pop_order_cnt < pop_order) {
				if (rt_cs_transmitter.order_ovflw) {
					rt_cs_transmitter.order_ovflw = 0;
				}
			}
			break;
		}
	}
	arc_unlock_restore(cpu_sts);

	return result;
}


static int32_t pre_printf(void)
{
	int32_t result = E_OK;

	if (dbg_printf_flag) {
		uint8_t *buf_ptr = NULL;

		if (exc_sense()) {
			/* save global buffer,
			 * which maybe use in task context. */
			xbuffer_save();

			/* interrupt context. */
			if(arc_int_active()) {
				/* allocation int_buf. */
				result = cs_intbuf_alloc();
				if (result >= 0) {
					buf_ptr = txbuf_hooker[result].buf;
					xbuffer_install((char *)buf_ptr);
				} else {
					/* TODO: buffer overflow! */
				}
			} else {
				/* seriously, exception happened!
				 * directly ouput information. */
				xbuffer_install(NULL);
				xfunc_out_set(console_putchar);
			}
		} else {
			/* task context. */
			uint32_t cpu_sts;

			/* alloc task buffer. */
			do {
				result = cs_txbuf_alloc();
				if (result >= 0) {
					break;
				} else {
					vTaskDelay(1);
				}
			} while (1);

			/* register buffer: */
			cpu_sts = arc_lock_save();
			while (!xbuffer_idle()) {
				arc_unlock_restore(cpu_sts);
				vTaskDelay(1);
				cpu_sts = arc_lock_save();
			}
			buf_ptr = txbuf_hooker[result].buf;
			xbuffer_install((char *)buf_ptr);
			arc_unlock_restore(cpu_sts);
		}
	}

	return result;
}

static void post_printf(int32_t buf_id, uint32_t len)
{
	if (dbg_printf_flag) {
		int32_t result = E_OK;
		uint32_t re_en_flg = 1;


		if (exc_sense()) {
			xbuffer_restore();

			/* interrupt context. */
			if (arc_int_active()) {
				txbuf_hooker[buf_id].state = DATA_BUF_READY;
				txbuf_hooker[buf_id].used_size = len;
			} else {
				re_en_flg = 0;
				xdev_out(NULL);
			}
		} else {
			/* task context. */

			/* release xprintf... */
			xbuffer_install(NULL);

			txbuf_hooker[buf_id].state = DATA_BUF_READY;
			txbuf_hooker[buf_id].used_size = len;
		}

		if ((re_en_flg) && (0 == rt_cs_transmitter.state)) {
			cs_txbuf_install();
			result = uart_interrupt_enable(CHIP_CONSOLE_UART_ID, \
					DEV_TRANSMIT, 1);
			if (E_OK != result) {
				/* record error:! */
			}
		}
	}
}


static void cs_txbuf_install(void)
{
	int32_t result = E_OK;

	uint32_t buf_id, data_len;
	uint8_t *data = NULL;
	DEV_BUFFER *dev_txbuf = &rt_cs_transmitter.buf;

	uint32_t re_install = 0;
	uint32_t int_dis_flag = 0;


	/* firstly, check whether user buffer hooker is ready. */
	result = cs_userbuf_ready();
	if (result >= 0) {
		re_install = 1;
	} else {
		result = cs_local_buf_ready();
		if (result >= 0) {
			re_install = 1;
		} else {
			/* nothing to do, disable interrupt. */
			int_dis_flag = 1;
		}
	}

	if (re_install) {
		buf_id = (uint32_t)result;
		rt_cs_transmitter.cur_buf_id = buf_id;

		data = txbuf_hooker[buf_id].buf;
		data_len = txbuf_hooker[buf_id].used_size;

		DEV_BUFFER_INIT(dev_txbuf, data, data_len);
		result = uart_buffer_register(CHIP_CONSOLE_UART_ID, \
					DEV_TX_BUFFER, dev_txbuf);
		if (E_OK == result) {
			txbuf_hooker[buf_id].state = DATA_BUF_INSTALLED;
			rt_cs_transmitter.state = 1;
		} else {
			/* TODO: record error info.
			 * then disable interrupt. */
			int_dis_flag = 1;
		}
	}

	if (int_dis_flag) {
		result = uart_interrupt_enable(CHIP_CONSOLE_UART_ID, \
					DEV_TRANSMIT, 0);
		if (E_OK != result) {
			/* record error! */
			;
		} else {
			rt_cs_transmitter.state = 0;
		}
	}
}


/* later console init: */
void console_init(void)
{
	int32_t result = E_OK;

	DEV_BUFFER *dev_buf = NULL;
#if 0
	/* hardware initialize. */
	result = uart_init(CFG_CS_RX_MODULE_PORT_ID, CHIP_CONSOLE_UART_BAUD);
	if (E_OK == result) {
		/* register buffer and enable interrupt. */
	}
#endif
#ifdef HEX_DATA_UART1
	uart_init(DW_UART_1_ID, CHIP_CONSOLE_UART_BAUD);
#endif
	cs_transmitter_init();

	cs_receiver_init();

	do {
		/* register xfer buffer. */
		dev_buf = &rt_cs_transmitter.buf;
		result = uart_buffer_register(CHIP_CONSOLE_UART_ID, DEV_TX_BUFFER, dev_buf);
		if (E_OK != result) {
			EMBARC_PRINTF("console uart tx_buf reigster failed.\r\n");
			break;
		}

		dev_buf = &rt_cs_receiver.buf;
		result = uart_buffer_register(CHIP_CONSOLE_UART_ID, DEV_RX_BUFFER, dev_buf);
		if (E_OK != result) {
			EMBARC_PRINTF("console uart rx_buf reigster failed.\r\n");
			break;
		}

		/* register xfer_callback. */
		result = uart_callback_register(CHIP_CONSOLE_UART_ID, cs_isr_callback);
		if (E_OK != result) {
			EMBARC_PRINTF("console uart xfer callback reigster failed.\r\n");
			break;
		}

		/* enable console rx uart interupt. */
		result = uart_interrupt_enable(CHIP_CONSOLE_UART_ID, DEV_RECEIVE, 1);
		if (E_OK != result) {
			EMBARC_PRINTF("console uart rx_int enable failed.\r\n");
			break;
		}

		/* create queue. */
		rt_cs_receiver.msg_queue = xQueueCreate(RX_MSG_QUEUE_ITEM_LEN, RX_MSG_QUEUE_ITEM_SIZE);
		if (NULL == rt_cs_receiver.msg_queue) {
			EMBARC_PRINTF("create console uart rx queue failed!\r\n");
			break;
		}

		xprintf_callback_install(pre_printf, post_printf);

		/* reset early console. */
		xdev_in(NULL);
		xdev_out(NULL);

		/* maybe need delay several seconds. */
		;

		dbg_printf_flag = 1;
	} while (0);
	//EMBARC_PRINTF("console later init finished.\r\n");
}


void bprintf(uint32_t *data, uint32_t len, void (*tx_end_callback)(void))
{
#ifdef HEX_DATA_UART1
	/* using the uart1 port to output the hex data */
	while (len--) {
		int32_t i = 0;
		for (i = 0; i < 4; i++) {
			uint8_t c = (*data >> (i << 3));
			uart_write(DW_UART_1_ID, &c, 1);
		}
		data++;
	}

#else
	/* using the uart0 port to output the hex data */
	int32_t result = E_OK;
	uint32_t cpu_sts = 0;

	/* alloc user buffer. */
	do {
		result = cs_userbuf_alloc();
		if (result >= 0) {
			break;
		} else {
			vTaskDelay(1);
		}
	} while (1);

	txbuf_hooker[result].buf = data;
	txbuf_hooker[result].used_size = len;
	txbuf_hooker[result].tx_end_callback = tx_end_callback;
	txbuf_hooker[result].state = DATA_BUF_READY;
	/* avoid re-enter. */
	cpu_sts = arc_lock_save();
	if (0 == rt_cs_transmitter.state) {
		cs_txbuf_install();
		result = uart_interrupt_enable(CHIP_CONSOLE_UART_ID, DEV_TRANSMIT, 1);
		if (E_OK != result) {
			EMBARC_PRINTF("[%s] uart_interrupt_enable failed. ret:%d \r\n", __func__, result);
		}
	}
	arc_unlock_restore(cpu_sts);
#endif
}
