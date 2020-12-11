#include "embARC.h"
#include "system.h"

static uint32_t current_cpu_freq = 0;
static uint64_t system_ticks = 0;

static tick_callback tick_timer_callback;

static void tick_timer_isr(void *params)
{
	timer_int_clear(CHIP_SYS_TIMER_ID);
	system_ticks += 1;
}

void system_tick_init(void)
{
	if (timer_present(CHIP_SYS_TIMER_ID)) {
		/* disable first then enable */
		int_disable(CHIP_SYS_TIMER_INTNO);

		int_handler_install(CHIP_SYS_TIMER_INTNO, tick_timer_isr);

		/* start 1ms timer interrupt */
		timer_start(CHIP_SYS_TIMER_ID, TIMER_CTRL_IE|TIMER_CTRL_NH, \
				current_cpu_freq / CHIP_SYS_TIMER_HZ);
		int_enable(CHIP_SYS_TIMER_INTNO);
	}
}

uint64_t system_ticks_get(void)
{
	return system_ticks;
}

void chip_hw_udelay(uint32_t us)
{
        uint32_t cur_cnt_high, s_cnt_high = 0;
        uint64_t cur_cnt, s_cnt = 0;

        uint32_t delta, tick_cnt = (current_cpu_freq / 1000000) * us;

        timer_current_high(TIMER_RTC, (void *)&s_cnt_high);
        timer_current(TIMER_RTC, (void *)&s_cnt);
        s_cnt |= ((uint64_t)s_cnt_high) << 32;
        do {
                timer_current_high(TIMER_RTC, (void *)&cur_cnt_high);
                timer_current(TIMER_RTC, (void *)&cur_cnt);
                cur_cnt |= ((uint64_t)cur_cnt_high) << 32;
                delta = cur_cnt - s_cnt;
        } while (delta < tick_cnt);
}

void chip_hw_mdelay(uint32_t ms)
{
	chip_hw_udelay(1000 * ms);
}

void set_current_cpu_freq(uint32_t freq)
{
        current_cpu_freq = freq;
}

void system_tick_timer_callback_register(tick_callback func)
{
	tick_timer_callback = func;
}
