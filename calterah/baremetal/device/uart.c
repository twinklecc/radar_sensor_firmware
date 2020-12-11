#include "embARC.h"
#include "system.h"
#include "dw_uart.h"
#include "uart.h"

static dw_uart_format_t frame_format = UART_FF_DEFAULT;
static dw_uart_t *dev_uart = NULL;
static dw_uart_ops_t *uart_ops = NULL;

void uart_init(void)
{
	int32_t result = 0;

	uint32_t flash_cnt = 0;

	clock_source_t clk_src;

	dev_uart = (dw_uart_t *)uart_get_dev(UART_OTA_ID);
	if (NULL != dev_uart) {
		uart_ops = (dw_uart_ops_t *)dev_uart->ops;
		if (NULL != uart_ops) {
			/* maybe need to reset uart first. */
			uart_enable(UART_OTA_ID, 1);
		} else {
			/* TODO: maybe can output wave on UART_OTA_ID:
			 * such as, on 1ms + off 1min + on 1ms + ... 
			 **/
			//flash_cnt = 1;
			goto err_handle;
		}
	} else {
		//flash_cnt = 2;
		goto err_handle;
	}

	result = uart_ops->format(dev_uart, &frame_format);
	if (E_OK != result) {
		//flash_cnt = 3;
		goto err_handle;
	}

	result = uart_ops->fifo_config(dev_uart, 1, 2, 0);
	if (E_OK != result) {
		//flash_cnt = 4;
		goto err_handle;
	}

	clk_src = uart_clock_source(UART_OTA_ID);
	if (CLOCK_SOURCE_INVALID != clk_src) {
		dev_uart->ref_clock = clock_frequency(clk_src);
	} else {
		//flash_cnt = 5;
		goto err_handle;
	}

	result = uart_ops->baud(dev_uart, CHIP_CONSOLE_UART_BAUD);
	if (E_OK != result) {
		//flash_cnt = 6;
		goto err_handle;
	}

err_handle:
	if (flash_cnt > 0) {
		/* TODO: maybe can output wave on UART_OTA_ID.
		 *       and wait for boot timeout. */
		while (1);
	} else {
		/* IOMUX of UART_OTA_ID back to uart mode. */

		/* For testing OTA, can output an protocol error
		uint8_t data[2] = {0x55, 0x55};
		uart_write(data, 2);
		*/
	}
}

int32_t uart_write(uint8_t *data, uint32_t len)
{
	int32_t result = -1;

	while (len) {
		if ((NULL == uart_ops) || (NULL == dev_uart)) {
			result = -2;
			break;
		}

		result = uart_ops->write(dev_uart, data, len);
		if (result > 0) {
			data += len - result;
			len = result;
		} else {
			break;
		}
	}

	return result;
}

int32_t uart_read(uint8_t *data, uint32_t len)
{
	int32_t result = -1;

	while (len) {
		if ((NULL == uart_ops) || (NULL == dev_uart)) {
			result = -2;
			break;
		}

		result = uart_ops->read(dev_uart, data, len);
		if (result > 0) {
			data += len - result;
			len = result;
		} else {
			break;
		}
	}

	return result;
}

