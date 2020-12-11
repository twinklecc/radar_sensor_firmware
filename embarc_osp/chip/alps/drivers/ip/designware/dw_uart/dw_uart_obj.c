#include "embARC_toolchain.h"
#include "alps/alps.h"
#include "dw_uart.h"

static dw_uart_t dw_uart0 = {
	.base = REL_REGBASE_UART0,
	.int_no = INTNO_UART0,
};

static dw_uart_t dw_uart1 = {
	.base = REL_REGBASE_UART1,
	.int_no = INTNO_UART1,
};

void *uart_get_dev(uint32_t id)
{
	void *ret_ptr = NULL;

	static uint32_t uart_install_flag = 0;

	if (0 == uart_install_flag) {
		dw_uart_install_ops(&dw_uart0);
		dw_uart_install_ops(&dw_uart1);

		uart_install_flag = 1;
	}

	switch (id) {
		case 0:
			ret_ptr =  (void *)&dw_uart0;
			break;
		case 1:
			ret_ptr =  (void *)&dw_uart1;
			break;
		default:
			ret_ptr = NULL;
	}

	return ret_ptr;
}

void uart_enable(uint32_t id, uint32_t en)
{
	switch (id) {
		case 0:
			uart0_enable(en);
			break;
		case 1:
			uart1_enable(en);
			break;
		default:
			break;
	}
}

clock_source_t uart_clock_source(uint32_t id)
{
	clock_source_t clk_src;

	switch (id) {
		case 0:
			clk_src = UART0_CLOCK;
			break;
		case 1:
			clk_src = UART1_CLOCK;
			break;
		default:
			clk_src = CLOCK_SOURCE_INVALID;
			break;
	}

	return clk_src;
}
