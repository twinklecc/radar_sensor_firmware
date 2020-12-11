#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "arc_exception.h"

#include "dw_uart.h"
#include "uart_hal.h"


#define UART_ISR(_func_name_) static void _func_name_(void *params)

#define UART_FF_DEFAULT	\
{.data_bits = UART_CHAR_8BITS, .parity = UART_NO_PARITY, .stop_bits = UART_STOP_1BIT }


#ifdef OS_FREERTOS
#include "FreeRTOS.h"
#include "task.h"

inline static uint32_t uart_lock(xSemaphoreHandle lock)
{
	uint32_t locked = 1;

	if (0 == exc_sense()) {
		if (xSemaphoreTake(lock, 0) != pdTRUE) {
			locked = 0;
		}
	} else {
 		BaseType_t higher = 0;
		if (xSemaphoreTakeFromISR(lock, &higher) != pdTRUE) {
			locked = 0;
#if 0
		} else {
			if (higher) {
				;
			}
#endif
		}
	}

	return locked;
}
inline static void uart_unlock(xSemaphoreHandle lock)
{
	xSemaphoreGive(lock);
}
#else
inline static uint32_t uart_lock(xSemaphoreHandle lock)
{
	return 1;
}
inline static void uart_unlock(xSemaphoreHandle lock)
{
	(void *)lock;
}
#endif

typedef struct {
	uint8_t inited;
	uint8_t state;

	dw_uart_format_t ff;

	/* interrupt context using. */
	DEV_BUFFER *rx_buf;
	//uart_pp_buf_t rx_pp_buf;
	DEV_BUFFER *tx_buf;
	//uart_pp_buf_t tx_rbuf;

	void *dev_uart;

#ifdef OS_FREERTOS
	xSemaphoreHandle lock;
#endif
	xfer_callback xfer_cb;
} uart_xfer_t;


static uart_xfer_t uart_runtime[UART_CNT_MAX];


static int32_t uart_int_install(uint32_t id);
static void uart_isr(uint32_t id, void *params);
UART_ISR(uart0_isr) { uart_isr(0, NULL); }
UART_ISR(uart1_isr) { uart_isr(1, NULL); }


int32_t uart_init(uint32_t id, uint32_t baud)
{
	int32_t result = E_OK;

	dw_uart_t *dev_uart = NULL;
	dw_uart_ops_t *uart_ops = NULL;

	if (id >= UART_CNT_MAX) {
		result = E_PAR;
	} else {
		dev_uart = (dw_uart_t *)uart_get_dev(id);
		if (NULL == dev_uart) {
			result = E_NOEXS;
		} else {
			uart_ops = (dw_uart_ops_t *)dev_uart->ops;
			if (NULL == uart_ops) {
				result = E_NOOPS;
			} else {
				uart_enable(id, 1);
				dmu_irq_enable(dev_uart->int_no, 1);
			}
		}
	}

	while (E_OK == result) {
		clock_source_t clk_src;
		dw_uart_format_t frame_format = UART_FF_DEFAULT;

		uart_runtime[id].dev_uart = (void *)dev_uart;

		result = uart_ops->format(dev_uart, &frame_format);
		if (E_OK != result) {
			break;
		} else {
			uart_runtime[id].ff.data_bits = frame_format.data_bits;
			uart_runtime[id].ff.parity = frame_format.parity;
			uart_runtime[id].ff.stop_bits = frame_format.stop_bits;
		}

#define UART_RX_THRESHOLD_DEAULT	(2)
#define UART_TX_THRESHOLD_DEAULT	(0)
		result = uart_ops->fifo_config(dev_uart, \
				1, \
				UART_RX_THRESHOLD_DEAULT, \
				UART_TX_THRESHOLD_DEAULT);
		if (E_OK != result) {
			break;
		}

		clk_src = uart_clock_source(id);
		if (CLOCK_SOURCE_INVALID != clk_src) {
			dev_uart->ref_clock = clock_frequency(clk_src);
		} else {
			result = E_SYS;
			break;
		}

		result = uart_ops->baud(dev_uart, baud);
		if (E_OK != result) {
			break;
		}

		result = uart_int_install(id);
		if (E_OK != result) {
			break;
		}

#ifdef OS_FREERTOS
		uart_runtime[id].lock = xSemaphoreCreateBinary();
		xSemaphoreGive(uart_runtime[id].lock);
#endif

		uart_runtime[id].inited = 1;
		break;
	}

	return result;
}

int32_t uart_baud_set(uint32_t id, uint32_t baud)
{
	int32_t result = E_OK;

	dw_uart_t *dev_uart = NULL;
	dw_uart_ops_t *uart_ops = NULL;

	if (id >= UART_CNT_MAX) {
		result = E_PAR;
	} else {
		if ((0 == uart_runtime[id].inited) || \
		    (NULL == uart_runtime[id].dev_uart)) {
			result = E_SYS;
		}
	}

	if (E_OK == result) {
		/* shouldn't be broken! */
		uint32_t cpu_sts = arc_lock_save();
		dev_uart = (dw_uart_t *)uart_runtime[id].dev_uart;
		uart_ops = (dw_uart_ops_t *)dev_uart->ops;
		result = uart_ops->baud(dev_uart, baud);
		arc_unlock_restore(cpu_sts);
	}

	return result;
}

int32_t uart_frame_config(uint32_t id, dw_uart_format_t *ff)
{
	int32_t result = E_OK;

	dw_uart_t *dev_uart = NULL;
	dw_uart_ops_t *uart_ops = NULL;

	if ((id >= UART_CNT_MAX) || (NULL == ff)) {
		result = E_PAR;
	} else {
		if ((0 == uart_runtime[id].inited) || \
		    (NULL == uart_runtime[id].dev_uart)) {
			result = E_SYS;
		}
	}

	if (E_OK == result) {
		/* shouldn't be broken! */
		uint32_t cpu_sts = arc_lock_save();
		dev_uart = (dw_uart_t *)uart_runtime[id].dev_uart;
		uart_ops = (dw_uart_ops_t *)dev_uart->ops;
		result = uart_ops->format(dev_uart, ff);
		arc_unlock_restore(cpu_sts);
	}

	return result;
}


int32_t uart_write(uint32_t id, uint8_t *data, uint32_t len)
{
	int32_t result = E_OK;

	uint32_t dev_sts = 0;

	dw_uart_t *dev_uart = NULL;
	dw_uart_ops_t *uart_ops = NULL;

	if ((id >= UART_CNT_MAX) || (NULL == data) || (0 == len)) {
		result = E_PAR;
	} else {
		if ((0 == uart_runtime[id].inited) || \
		    (NULL == uart_runtime[id].dev_uart)) {
			result = E_SYS;
		} else {
			dev_sts = uart_lock(uart_runtime[id].lock);
			if (0 == dev_sts) {
				result = E_DBUSY;
			}
		}
	}

	if (E_OK == result) {
		dev_uart = (dw_uart_t *)uart_runtime[id].dev_uart;
		uart_ops = (dw_uart_ops_t *)dev_uart->ops;

		while (len) {
			result = uart_ops->write(dev_uart, data, len);
			if (result > 0) {
				data += len - result;
				len = (uint32_t)result;
			} else {
				break;
			}
		}

		uart_unlock(uart_runtime[id].lock);
	}

	return result;
}


int32_t uart_read(uint32_t id, uint8_t *data, uint32_t len)
{
	int32_t result = E_OK;

	uint32_t dev_sts = 0;

	dw_uart_t *dev_uart = NULL;
	dw_uart_ops_t *uart_ops = NULL;

	if ((id >= UART_CNT_MAX) || (NULL == data) || (0 == len)) {
		result = E_PAR;
	} else {
		if ((0 == uart_runtime[id].inited) || \
		    (NULL == uart_runtime[id].dev_uart)) {
			result = E_SYS;
		} else {
			dev_sts = uart_lock(uart_runtime[id].lock);
			if (0 == dev_sts) {
				result = E_DBUSY;
			}
		}
	}

	if (E_OK == result) {
		dev_uart = (dw_uart_t *)uart_runtime[id].dev_uart;
		uart_ops = (dw_uart_ops_t *)dev_uart->ops;

		while (len) {
			result = uart_ops->read(dev_uart, data, len);
			if (result > 0) {
				data += len - result;
				len = (uint32_t)result;
			} else {
				break;
			}
		}

		uart_unlock(uart_runtime[id].lock);
	}

	return result;
}

#if 0
inline static void uart_rbuf_install(uart_pp_buf_t *pp_buf, void *new_buf)
{
	uart_pp_buf_t *n_pp_buf_ptr = (uart_pp_buf_t *)new_buf;
	pp_buf->buf = n_pp_buf_ptr->buf;
	pp_buf->len = n_pp_buf_ptr->len;
	pp_buf->nof_pp_buf = n_pp_buf_ptr->nof_pp_buf;
}
#endif

int32_t uart_buffer_register(uint32_t id, dev_buf_type_t type, void *buffer)
{
	int32_t result = E_OK;

	if ((id >= UART_CNT_MAX) || (type > DEV_TX_BUFFER) || \
			(NULL == buffer)) {
		result = E_PAR;
	} else {
		if ((0 == uart_runtime[id].inited) || \
		    (NULL == uart_runtime[id].dev_uart)) {
			result = E_SYS;
		}
	}

	if (E_OK == result) {
		//uint8_t *data_ptr = NULL;

		uint32_t cpu_sts = arc_lock_save();
		if (DEV_RX_BUFFER == type) {
			/*
			uart_pp_buf_t *pp_buf_ptr = &uart_runtime[id].rx_pp_buf;
			if ((NULL != pp_buf_ptr->buf)) {
				result = E_DBUSY;
			} else {
				uart_rbuf_install(pp_buf_ptr, buffer);
			}
			*/
			uart_runtime[id].rx_buf = (DEV_BUFFER *)buffer;
		} else {
			/*
			uart_pp_buf_t *pp_buf_ptr = &uart_runtime[id].tx_rbuf;
			if ((NULL != pp_buf_ptr->buf)) {
				result = E_DBUSY;
			} else {
				uart_rbuf_install(pp_buf_ptr, buffer);
			}
			*/
			/*
			DEV_BUFFER *dev_buf = &uart_runtime[id].tx_buf;
			data_ptr = (uint8_t *)dev_buf->buf;
			if ((NULL != data_ptr) && \
				(dev_buf->ofs < dev_buf->len)) {
				result = E_DBUSY;
			} else {
				DEV_BUFFER *new_buf = (DEV_BUFFER *)buffer;
				DEV_BUFFER_INIT(dev_buf, new_buf->buf, new_buf->len);
			}
			*/
			uart_runtime[id].tx_buf = (DEV_BUFFER *)buffer;
		}

		arc_unlock_restore(cpu_sts);
	}

	return result;
}


int32_t uart_interrupt_enable(uint32_t id, dev_xfer_type_t type, uint32_t en)
{
	int32_t result = E_OK;

	dw_uart_t *dev_uart = NULL;
	dw_uart_ops_t *uart_ops = NULL;

	if ((id >= UART_CNT_MAX) || (type > DEV_XFER)) {
		result = E_PAR;
	} else {
		if ((0 == uart_runtime[id].inited) || \
		    (NULL == uart_runtime[id].dev_uart)) {
			result = E_SYS;
		}
	}

	if (E_OK == result) {
		uint32_t mask = 0;

		switch (type) {
			case DEV_RECEIVE:
				mask |= DW_UART_RX_DATA_AVAILABLE_INT;
				mask |= DW_UART_RECEIVER_LINE_STS_INT;
				break;
			case DEV_TRANSMIT:
				mask |= DW_UART_TX_HOLDING_REG_EMPTY_INT;
				break;
			case DEV_XFER:
				mask |= DW_UART_RX_DATA_AVAILABLE_INT;
				//mask |= DW_UART_RECEIVER_LINE_STS_INT;
				mask |= DW_UART_TX_HOLDING_REG_EMPTY_INT;
				break;
			default:
				/* panic! */
				break;
		}

		uint32_t cpu_sts = arc_lock_save();
		dev_uart = (dw_uart_t *)uart_runtime[id].dev_uart;
		uart_ops = (dw_uart_ops_t *)dev_uart->ops;
		result = uart_ops->int_enable(dev_uart, en, mask);
		arc_unlock_restore(cpu_sts);
	}

	return result;
}

int32_t uart_callback_register(uint32_t id, xfer_callback func)
{
	int32_t result = E_OK;

	if ((id >= UART_CNT_MAX) || (NULL == func)) {
		result = E_PAR;
	} else {
		uint32_t cpu_sts = arc_lock_save();
		uart_runtime[id].xfer_cb = func;
		arc_unlock_restore(cpu_sts);
	}

	return result;
}

/* called by uart_init only. */
static int32_t uart_int_install(uint32_t id)
{
	int32_t result = E_OK;

	dw_uart_t *dev_uart = (dw_uart_t *)uart_runtime[id].dev_uart;
	if (dev_uart) {
		switch (id) {
			case 0:
				result = int_handler_install(dev_uart->int_no, \
						uart0_isr);
				break;
			case 1:
				result = int_handler_install(dev_uart->int_no, \
						uart1_isr);
				break;
			default:
				result = E_SYS;
		}

		if (E_OK == result) {
			int_enable(dev_uart->int_no);
		}
	}

	return result;
}

static void uart_isr(uint32_t id, void *params)
{
	int32_t result = E_OK;

	uint32_t int_sts = 0, status = 0;
	//uint32_t mask = 0;

	//uint8_t *buf_ptr = NULL;
	//uint32_t buf_len = 0;
	//static uint32_t cur_pp_buf_idx = 0;

	DEV_BUFFER *rx_buf = NULL;
	DEV_BUFFER *dev_buf = NULL;

	dw_uart_t *dev_uart = NULL;
	dw_uart_ops_t *uart_ops = NULL;

	if (id >= UART_CNT_MAX) {
		result = E_PAR;
	} else {
		if ((0 == uart_runtime[id].inited) || \
		    (NULL == uart_runtime[id].dev_uart)) {
			result = E_SYS;
		} else {
			dev_uart = (dw_uart_t *)uart_runtime[id].dev_uart;
			if (NULL == dev_uart->ops) {
				result = E_NOOPS;
			} else {
				uart_ops = (dw_uart_ops_t *)dev_uart->ops;
			}
		}
	}

	if (E_OK == result) {
		/* get uart interrupt status. */
		result = uart_ops->int_id(dev_uart);
		if (result >= 0) {
			int_sts = (uint32_t)result;
		}

		switch (int_sts) {
			case IID_MODEM_STATUS:
				/* record error! */
				result = E_SYS;
				break;
			case IID_THR_EMPTY:
				/* tx. */
				dev_buf = uart_runtime[id].tx_buf;
				if ((NULL == dev_buf) || (NULL == dev_buf->buf)) {
					/*TODO: record error! */
				} else {
					if (dev_buf->ofs < dev_buf->len) {
						uint8_t *buf_ptr = (uint8_t *)dev_buf->buf;
						uint32_t cnt = dev_buf->len - dev_buf->ofs;

						buf_ptr += dev_buf->ofs;
						result = uart_ops->write(dev_uart, buf_ptr, cnt);
						if (result >= 0) {
							dev_buf->ofs += cnt - result;
						} else {
							/* TODO: record error! */
							;
						}
					}
				}

				if (uart_runtime[id].xfer_cb) {
					uart_runtime[id].xfer_cb(NULL);
				}
				break;

			case IID_CHAR_TIMEOUT:
			case IID_RX_DATA_AVAIL:
				/* rx. */
				rx_buf = uart_runtime[id].rx_buf;
				if ((NULL == rx_buf) || (NULL == rx_buf->buf)) {
					/* TODO: record error! */
				} else {
					uint8_t *buf_ptr = (uint8_t *)rx_buf->buf;
					uint32_t free_size = rx_buf->len - rx_buf->ofs;
					if (free_size > 0) {
						buf_ptr += rx_buf->ofs;
						result = uart_ops->read(dev_uart, buf_ptr, free_size);

						if (result >= 0) {
							rx_buf->ofs += free_size - (uint32_t)result;
						} else {
							/* TODO: record error! */
						}
					}
				}

				if (uart_runtime[id].xfer_cb) {
					uart_runtime[id].xfer_cb(NULL);
				}
				break;

			case IID_RX_LINE_STATUS:
				/* rx line exists error! */
				status = uart_ops->line_status(dev_uart);
				if (status > 0) {
					EMBARC_PRINTF("[%s]IID_RX_LINE_STATUS: 0x%x\r\n", __func__, status);

					/* Clear the Rx fifo */
					uart_ops->fifo_flush(dev_uart, UART_RX);
				}
				break;
			case IID_NO_INT_PENDING:
				EMBARC_PRINTF("[%s]IID_NO_INT_PENDING!\r\n", __func__);
				result = E_SYS;
				break;
			case IID_BUSY_DETECT:
				EMBARC_PRINTF("[%s]IID_BUSY_DETECT!\r\n", __func__);
				break;
			default:
				result = E_SYS;
				break;
		}
	}
}
