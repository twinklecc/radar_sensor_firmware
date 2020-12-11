#include "embARC.h"
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "arc_exception.h"
#include "dw_uart_reg.h"
#include "dw_uart.h"

static inline void dw_uart_divisor_latch_access(dw_uart_t *dw_uart, uint32_t enable)
{
	uint32_t reg_addr = dw_uart->base + REG_DW_UART_LCR_OFFSET;
	uint32_t val = raw_readl(reg_addr);
	if (enable) {
		val |= BIT_REG_DW_UART_LCR_DLAB;
	} else {
		val &= ~BIT_REG_DW_UART_LCR_DLAB;
	}
	raw_writel(reg_addr, val);
}

static int32_t dw_uart_baud_rate(dw_uart_t *dw_uart, uint32_t baud)
{
	int32_t result = E_OK;

	if ((NULL == dw_uart) || (0 == baud)) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = 0;
		uint32_t baud_divisor = dw_uart->ref_clock / (16 * baud);
		uint32_t baud_fraction = (dw_uart->ref_clock / baud) % 16;

		dw_uart_divisor_latch_access(dw_uart, 1);
		reg_addr = REG_DW_UART_RBR_DLL_THR_OFFSET + dw_uart->base;
		raw_writel(reg_addr, baud_divisor & 0xFF);
		reg_addr = REG_DW_UART_DLH_IER_OFFSET + dw_uart->base;
		raw_writel(reg_addr, (baud_divisor >> 8) & 0xFF);

		reg_addr = REG_DW_UART_DLF_OFFSET + dw_uart->base;
		raw_writel(reg_addr, baud_fraction & 0xFF);
		dw_uart_divisor_latch_access(dw_uart, 0);
	}

	return result;
}

static int32_t dw_uart_interrupt_enable(dw_uart_t *dw_uart, int enable, int mask)
{
	int32_t result = E_OK;

	if ((NULL == dw_uart)) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = REG_DW_UART_DLH_IER_OFFSET + dw_uart->base;
		uint32_t val = raw_readl(reg_addr);

		if (enable) {
			val |= mask;
		} else {
			val &= ~mask;
		}
		raw_writel(reg_addr, val);
	}

	return result;
}

static int32_t dw_uart_fifo_config(dw_uart_t *dw_uart, uint32_t enable, 
		uint32_t rx_threshold, uint32_t tx_threshold)
{
	int32_t result = E_OK;

	if ((NULL == dw_uart)) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = REG_DW_UART_FCR_IIR_OFFSET + dw_uart->base;
		uint32_t val = 0;

		if (enable) {
			val |= BIT_REG_DW_UART_FCR_FIFOE;
		} else {
			val &= ~BIT_REG_DW_UART_FCR_FIFOE;
		}
		val |= (rx_threshold & BITS_REG_DW_UART_FCR_RT_MASK) << \
		       BITS_REG_DW_UART_FCR_RT_SHIFT;
		val |= (tx_threshold & BITS_REG_DW_UART_FCR_TET_MASK) << \
		       BITS_REG_DW_UART_FCR_TET_SHIFT;

		val |= BIT_REG_DW_UART_FCR_XFIFOR | BIT_REG_DW_UART_FCR_RFIFOR;
		raw_writel(reg_addr, val);
	}

	return result;
}

static int32_t dw_uart_interrupt_id(dw_uart_t *dw_uart)
{
	int32_t result = E_OK;

	if ((NULL == dw_uart)) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = REG_DW_UART_FCR_IIR_OFFSET + dw_uart->base;
		result = raw_readl(reg_addr) & BITS_REG_DW_UART_IIR_IID_MASK;
	}

	return result;
}

static int32_t dw_uart_frame_format(dw_uart_t *dw_uart, dw_uart_format_t *format)
{
	int32_t result = E_OK;

	if ((NULL == dw_uart) || (NULL == format)) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = REG_DW_UART_LCR_OFFSET + dw_uart->base;
		uint32_t val = raw_readl(reg_addr);

		val &= ~(BIT_REG_DW_UART_LCR_EPS | BIT_REG_DW_UART_LCR_PEN);
		switch (format->parity) {
			case UART_NO_PARITY:
				break;
			case UART_EVEN_PARITY:
				val |= BIT_REG_DW_UART_LCR_EPS;
			case UART_ODD_PARITY:
				val |= BIT_REG_DW_UART_LCR_PEN;
				break;
			default:
				result = E_PAR;
		}

		if (E_OK == result) { 
			if ((format->data_bits < UART_CHAR_MIN_BITS) || \
				(format->data_bits > UART_CHAR_MAX_BITS)) {
				result = E_PAR;
			} else {
				val |= (format->data_bits - UART_CHAR_MIN_BITS) & \
				       BIT_REG_DW_UART_LCR_DLS_MASK;
				if (format->stop_bits == UART_STOP_1BIT) {
					val &= ~BIT_REG_DW_UART_LCR_STOP;
				} else {
					val |= BIT_REG_DW_UART_LCR_STOP;
				}
				raw_writel(reg_addr, val);
			}
		}
	}

	return result;
}

static int32_t dw_uart_line_status(dw_uart_t *dw_uart)
{
	int32_t result = E_OK;

	if ((NULL == dw_uart)) {
		result = E_PAR;
	} else {
		/* return the rx line status and clear the error interrupts automatically by hardware */
		uint32_t reg_addr = REG_DW_UART_LSR_OFFSET + dw_uart->base;
		result = raw_readl(reg_addr);
	}

	return result;
}

static int32_t dw_uart_status(dw_uart_t *dw_uart, uint32_t status)
{
	int32_t result = E_OK;

	if ((NULL == dw_uart)) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = REG_DW_UART_USR_OFFSET + dw_uart->base;
		uint32_t val = raw_readl(reg_addr);

		switch (status) {
			case UART_RX_FIFO_FULL:
				result = (val & BIT_REG_DW_UART_USR_RFF) ? (1) : (0);
				break;
			case UART_RX_FIFO_NOT_EMPTY:
				result = (val & BIT_REG_DW_UART_USR_RFNE) ? (1) : (0);
				break;
			case UART_TX_FIFO_EMPTY:
				result = (val & BIT_REG_DW_UART_USR_TFE) ? (1) : (0);
				break;
			case UART_TX_FIFO_NOT_FULL:
				result = (val & BIT_REG_DW_UART_USR_TFNF) ? (1) : (0);
				break;
			case UART_BUSY:
				result = (val & BIT_REG_DW_UART_USR_BUSY) ? (1) : (0);
				break;
			case UART_ALL_STS:
				result = val;
				break;
			default:
				result = E_PAR;
		}
	}

	return result;
}

static int32_t dw_uart_version(dw_uart_t *dw_uart)
{
	int32_t result = E_OK;

	if ((NULL == dw_uart)) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = REG_DW_UART_UCV_OFFSET + dw_uart->base;
		result = raw_readl(reg_addr);
	}

	return result;
}


static int32_t dw_uart_read(dw_uart_t *dw_uart, uint8_t *buf, uint32_t len)
{
	int32_t result = E_OK;

	uint32_t cnt = 0;

	if ((NULL == dw_uart) || (NULL == buf) || (0 == len)) {
		result = E_PAR;
	} else {
		uint32_t fifo_entry;
		uint32_t reg_addr = REG_DW_UART_CPR_OFFSET + dw_uart->base;
		uint32_t val = raw_readl(reg_addr);

		uint32_t bit_pos = 0;
		if (val & BIR_REG_DW_UART_CPR_FIFO_STAT) {
			reg_addr = REG_DW_UART_USR_OFFSET + dw_uart->base;
			bit_pos = BIT_REG_DW_UART_USR_RFNE;
		} else {
			reg_addr = REG_DW_UART_LSR_OFFSET + dw_uart->base;
			bit_pos = BIT_REG_DW_UART_LSR_DR;
		}

		fifo_entry = REG_DW_UART_RBR_DLL_THR_OFFSET + dw_uart->base;
		for (cnt = 0; cnt < len; cnt++) {
			if (raw_readl(reg_addr) & bit_pos) {
				*buf++ = (uint8_t)raw_readl(fifo_entry);
			} else {
				/* remain size. */
				result = len - cnt;
				break;
			}
		}
	}

	return result;
}

static int32_t dw_uart_write(dw_uart_t *dw_uart, uint8_t *buf, uint32_t len)
{
	int32_t result = E_OK;

	if ((NULL == dw_uart) || (NULL == buf) || (0 == len)) {
		result = E_PAR;
	} else {
		uint32_t fifo_entry;
		uint32_t reg_addr = REG_DW_UART_CPR_OFFSET + dw_uart->base;
		uint32_t val = raw_readl(reg_addr);

		uint32_t bit_pos = 0;
		if (val & BIR_REG_DW_UART_CPR_FIFO_STAT) {
			reg_addr = REG_DW_UART_USR_OFFSET + dw_uart->base;
			bit_pos = BIT_REG_DW_UART_USR_TFNF;
		} else {
			reg_addr = REG_DW_UART_LSR_OFFSET + dw_uart->base;
			bit_pos = BIT_REG_DW_UART_LSR_TEMT;
		}

		fifo_entry = REG_DW_UART_RBR_DLL_THR_OFFSET + dw_uart->base;
		while (len) {
			if (raw_readl(reg_addr) & bit_pos) {
				raw_writel(fifo_entry, (uint32_t)*buf++);
				len -= 1;
			} else {
				/* remain size. */
				result = len;
				break;
			}
		}
	}

	return result;
}

int32_t dw_uart_fifo_flush(dw_uart_t *dw_uart, uint32_t channel)
{
	int32_t result = E_OK;
	uint32_t reg_addr = 0, fifo_entry_addr = 0;
	uint8_t fifo_data_num = 0, i = 0;

	if (NULL == dw_uart)
		result = E_PAR;

	if (channel == UART_TX)
		reg_addr = dw_uart->base + REG_DW_UART_TFL_OFFSET;
	else if (channel == UART_RX)
		reg_addr = dw_uart->base + REG_DW_UART_RFL_OFFSET;
	else
		result = E_PAR;

	/* Get the number of data in the FIFO */
	fifo_data_num = raw_readl(reg_addr);

	fifo_entry_addr = dw_uart->base + REG_DW_UART_RBR_DLL_THR_OFFSET;

	/* clear fifo data */
	for(i = 0; i < fifo_data_num; i++) {
		raw_readl(fifo_entry_addr);
	}

	return result;
}


static dw_uart_ops_t dw_uart_ops = {
	.version = dw_uart_version,
	.read = dw_uart_read,
	.write = dw_uart_write,
	.format = dw_uart_frame_format,
	.fifo_config = dw_uart_fifo_config,
	.baud = dw_uart_baud_rate,
	.int_enable = dw_uart_interrupt_enable,
	.int_id = dw_uart_interrupt_id,
	.line_status = dw_uart_line_status,
	.status = dw_uart_status,
	.fifo_flush = dw_uart_fifo_flush
};

int32_t dw_uart_install_ops(dw_uart_t *dw_uart)
{
	int32_t result = E_OK;

	if ((NULL == dw_uart)) {
		result = E_PAR;
	} else {
		dw_uart->ops = (void *)&dw_uart_ops;
	}

	return result;
}
