#include "embARC.h"
#include "uart_hal.h"

/** put one char */
void console_putchar(unsigned char chr)
{
	uart_write(CHIP_CONSOLE_UART_ID, &chr, 1);
}

/** put string */
int console_putstr(const char *str, unsigned int len)
{
	return uart_write(CHIP_CONSOLE_UART_ID, (uint8_t *)(str), len);
}

/** get one char*/
int console_getchar(void)
{
	unsigned char data;
	uart_read(CHIP_CONSOLE_UART_ID, (void *)(&data), 1);
	return (int)data;
}

/** get string */
int console_getstr(char *str, unsigned int len)
{
	return uart_read(CHIP_CONSOLE_UART_ID, (void *)(str), len);
}
