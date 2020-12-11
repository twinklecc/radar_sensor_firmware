#include "embARC.h"
#include "system.h"
#include "ota.h"
#include "flash.h"
#include "dw_uart.h"
#include "uart.h"
#include "dev_can.h"
#include "can_hal.h"


void fmcw_radio_pll_clock_en(void)
{
	/* assume that PLL has been configured in ROMCODE.
	 * the CPU and system clock has been switched to PPL clock. */
}

void fmcw_radio_clk_out_for_cascade(void)
{
}

void dummy_printf(const char*fmt, ...)
{
}

void *memcpy(uint8_t *dst, uint8_t *src, uint32_t len)
{
	while (len--) {
		*dst++ = *src++;
	}
	return (void *)dst;
}

#ifdef BOOT_SPLIT
void system_ota_entry(void)
#else
void board_main(void)
#endif
{
	int32_t result = 0;

	uint32_t reboot_mode = reboot_cause();

#ifndef BOOT_SPLIT
	system_clock_init();
	gen_crc_table();
#endif

	timer_init();

	system_tick_init();

    /* By default, Boot use UART0(UART_OTA_ID ?= 0) for UART communication and OTA function */
    /* If you change to use UART1(UART_OTA_ID ?= 1), please call "io_mux_init()" here */
    /* The reason is that after Reset in "uart_ota" command, according to Alps Reference Manual,
       "DMU_MUX_UART_1" register change to 0x4 which means the function of "IO Mux for UART_1" change to GPIO,
       so need to call "io_mux_init" to recover the function of "IO Mux for UART_1" back to UART1 */
	uart_init();

	result = flash_init();
	if (0 != result) {
		reboot_mode = ECU_REBOOT_UART_OTA;
	}


	/* check the reboot flag... */
	switch (reboot_mode) {
		case ECU_REBOOT_UART_OTA:
			uart_ota_main();
			break;

		case ECU_REBOOT_CAN_OTA:
			can_ota_main();
			break;

		default:
			if (E_OK != normal_boot()) {
				uart_ota_main();
			}
			break;
	}

	while (1);
}


void uart_print(uint32_t info)
{
#ifdef SYSTEM_UART_LOG_EN
	uint32_t len = 3 * sizeof(uint32_t);
	uint32_t msg[3] = {SYSTEM_LOG_START_FLAG, info, SYSTEM_LOG_END_FLAG};

	uint8_t *data_ptr = (uint8_t *)msg;
	uart_write(data_ptr, len);
#endif
}
