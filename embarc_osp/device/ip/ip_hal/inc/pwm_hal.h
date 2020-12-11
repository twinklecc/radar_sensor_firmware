#ifndef _PWM_HAL_H_
#define _PWM_HAL_H_

int32_t pwm_init(void);

int32_t pwm_start(uint32_t id, uint32_t duty_cycle, void (*func)(void *param));

int32_t pwm_stop(uint32_t id);
#endif
