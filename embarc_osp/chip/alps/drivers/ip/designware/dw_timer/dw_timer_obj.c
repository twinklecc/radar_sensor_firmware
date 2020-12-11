#include "embARC_toolchain.h"
#include "alps/alps.h"
#include "dw_timer.h"

static uint8_t dev_timer_int[4] = {
	INTNO_DW_TIMER0,
	INTNO_DW_TIMER1,
	INTNO_DW_TIMER2,
	INTNO_DW_TIMER3
};
static dw_timer_t dev_timer = {
	.base = REL_REGBASE_TIMER0,
	.timer_cnt = 4,
	.int_no = dev_timer_int
};

static uint8_t dev_pwm_int[] = {
	INTNO_DW_PWM0,
	INTNO_DW_PWM1,
#ifdef CHIP_ALPS_MP
	INT_PWM2_IRQ,
	INT_PWM3_IRQ,
	INT_PWM4_IRQ,
	INT_PWM5_IRQ,
	INT_PWM6_IRQ,
	INT_PWM7_IRQ
#endif
};
static dw_timer_t dev_pwm = {
	.base = REL_REGBASE_PWM,
	.int_no = dev_pwm_int,
	.timer_cnt = CHIP_PWM_NO
};

static void timer_resource_install(void)
{
	dw_timer_install_ops(&dev_timer);
}

void *timer_get_dev(void)
{
	static uint32_t timer_install_flag = 0;

	if (0 == timer_install_flag) {
		timer_resource_install();
		timer_install_flag = 1;
	}

	return (void *)&dev_timer;
}


static void pwm_resource_install(void)
{
	dw_timer_install_ops(&dev_pwm);
}

void *pwm_get_dev(void)
{
	static uint32_t pwm_install_flag = 0;

	if (0 == pwm_install_flag) {
		pwm_resource_install();
		pwm_install_flag = 1;
	}

	return (void *)&dev_pwm;
}
