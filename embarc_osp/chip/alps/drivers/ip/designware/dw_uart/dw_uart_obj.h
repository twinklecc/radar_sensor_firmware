#ifndef _DW_UART_OBJ_H_
#define _DW_UART_OBJ_H_

#define DW_UART_NUM	(2)

#define DW_UART_0_ID	0
#define DW_UART_1_ID	1

#define UART_CNT_MAX	(2)

#include "alps_clock.h"

void *uart_get_dev(uint32_t id);
void uart_enable(uint32_t id, uint32_t en);
clock_source_t uart_clock_source(uint32_t id);

#endif
