#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "arc_exception.h"

#include "dw_timer.h"
//#include "timer_hal.h"


typedef void (*timer_callback)(void *);
static dw_timer_t *pwm_timer = NULL;
static timer_callback expire_callback[CHIP_PWM_NO];

static void pwm_isr(void *param);

int32_t pwm_init(void)
{
	int32_t result = E_OK;

	int idx = 0;
	do {
		uint32_t nof_int = 0;
		pwm_timer = (dw_timer_t *)pwm_get_dev();
		if (NULL == pwm_timer) {
			result = E_NOEXS;
			break;
		}

		/* interrupt install. */
		for (; idx < pwm_timer->timer_cnt; idx++) {
			nof_int = pwm_timer->int_no[idx];
			result = int_handler_install(nof_int, pwm_isr);
			if (E_OK != result) {
				break;
			}
		}

		if (E_OK == result) {
			pwm_enable(1);
		}
	} while (0);

	return result;
}

int32_t pwm_start(uint32_t id, uint32_t freq, uint32_t duty_cycle, void (*func)(void *param))
{
	int32_t result = E_OK;

	dw_timer_ops_t *pwm_ops = (dw_timer_ops_t *)pwm_timer->ops;
	uint32_t load_value0, load_value1 = duty_cycle;

	if ((NULL == pwm_timer) || (NULL == pwm_timer->ops) || \
	    (id >= pwm_timer->timer_cnt) || (0 == freq) || \
	    (duty_cycle > 100)) {
		result = E_PAR;
	} else {
		uint32_t cpu_sts;
		uint32_t peroid = 0;

		uint32_t clk_src = clock_frequency(PWM_CLOCK);
		peroid = clk_src / freq;
		load_value0 = peroid * (100 - duty_cycle) / 100;
		load_value1 = peroid * duty_cycle / 100;

		cpu_sts = arc_lock_save();
		do {
			result = pwm_ops->mode(pwm_timer, id, USER_DEFINED_MODE);
			if (E_OK != result) {
				break;
			}

			result = pwm_ops->load_count_update(pwm_timer, id, load_value0);
			if (E_OK != result) {
				break;
			}

			result = pwm_ops->load_count2_update(pwm_timer, id, load_value1);
			if (E_OK != result) {
				break;
			}

			if (func) {
				expire_callback[id] = func;
				result = pwm_ops->int_mask(pwm_timer, id, 0);
				if (E_OK != result) {
					break;
				}

				result = int_enable(pwm_timer->int_no[id]);
				if (E_OK != result) {
					break;
				}
			}

			result = pwm_ops->pwm(pwm_timer, id, 1, 1);
			if (E_OK != result) {
				break;
			}

			result = pwm_ops->enable(pwm_timer, id, 1);
			if (E_OK != result) {
				break;
			}

			dmu_pwm_output_enable(id, 1);
		} while (0);
		arc_unlock_restore(cpu_sts);

	}

	return result;
}

int32_t pwm_stop(uint32_t id)
{
	int32_t result = E_OK;

	dw_timer_ops_t *pwm_ops = (dw_timer_ops_t *)pwm_timer->ops;

	if ((NULL == pwm_timer) || (NULL == pwm_timer->ops) || \
	    (id >= pwm_timer->timer_cnt)) {
		result = E_PAR;
	} else {
		uint32_t cpu_sts = arc_lock_save();
		result = pwm_ops->pwm(pwm_timer, id, 0, 0);
		if (E_OK == result) {
			result = pwm_ops->int_mask(pwm_timer, id, 1); 
		}

		if (E_OK == result) {
			result = int_disable(pwm_timer->int_no[id]);
			if (E_OK == result) {
				expire_callback[id] = NULL;
			}
		}
		arc_unlock_restore(cpu_sts);
	}

	return result;
}

void pwm_isr(void *params)
{
}
