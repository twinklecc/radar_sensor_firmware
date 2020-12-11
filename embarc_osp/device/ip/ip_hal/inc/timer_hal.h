#ifndef _TIMER_HAL_H_
#define _TIMER_HAL_H_

int32_t dw_timer_init(void);

int32_t dw_timer_start(uint32_t id, uint32_t expire, void (*func)(void *param));

int32_t dw_timer_stop(uint32_t id);
int32_t dw_timer_stop_fromisr(uint32_t id);

#endif
