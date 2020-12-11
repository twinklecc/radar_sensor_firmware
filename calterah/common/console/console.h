#ifndef _CONSOLE_H_
#define _CONSOLE_H_

void console_init(void);

void bprintf(uint32_t *data, uint32_t len, void (*tx_end_callback)(void));

//int32_t pre_printf(void);

//void post_printf(int32_t buf_id);

uint32_t console_wait_newline(uint8_t *data, uint32_t wbuf_len);

void console_putchar(unsigned char chr);

#endif
