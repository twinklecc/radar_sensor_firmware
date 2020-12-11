#ifndef _BM_UART_H_
#define _BM_UART_H_

void uart_init(void);

int32_t uart_write(uint8_t *data, uint32_t len);
int32_t uart_read(uint8_t *data, uint32_t len);

#endif
