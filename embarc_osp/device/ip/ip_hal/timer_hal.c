#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "arc_exception.h"

#include "dw_timer.h"
//#include "timer_hal.h"

#ifdef OS_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#endif

typedef void (*timer_callback)(void *);

static dw_timer_t *dw_timer = NULL;
static timer_callback expire_callback[CHIP_TIMER_NO];

static void dw_timer_isr(void *param);

int32_t dw_timer_init(void)
{
	int32_t result = E_OK;


	int idx = 0;
	uint32_t nof_int = 0;
	do {
		dw_timer = (dw_timer_t *)timer_get_dev();
		if (NULL == dw_timer) {
			result = E_NOEXS;
			break;
		}

		/* interrupt install. */
		for (; idx < CHIP_TIMER_NO; idx++) {
			nof_int = dw_timer->int_no[idx];
			result = int_handler_install(nof_int, dw_timer_isr);
			if (E_OK != result) {
				break;
			}
		}

		if (E_OK == result) {
			timer_enable(1);
		}
	} while (0);

	return result;
}

int32_t dw_timer_start(uint32_t id, uint32_t expire, void (*func)(void *param))
{
	int32_t result = E_OK;

	dw_timer_ops_t *timer_ops = (dw_timer_ops_t *)dw_timer->ops;

	if ((NULL == dw_timer) || (NULL == dw_timer->ops) || \
	    (id >= dw_timer->timer_cnt) || (0 == expire) || (NULL == func)) {
		result = E_PAR;
	} else {
		uint32_t cpu_sts = arc_lock_save();
		do {
			if (expire) {
				/* user define mode. */
				result = timer_ops->mode(dw_timer, id, USER_DEFINED_MODE);
				if (E_OK != result) {
					break;
				}

				result = timer_ops->load_count_update(dw_timer, id, expire);
				if (E_OK != result) {
					break;
				}
			}

			if (func) {
				/* timer handler. */
				expire_callback[id] = func;
				result = timer_ops->int_mask(dw_timer, id, 0);
				if (E_OK != result) {
					break;
				}

				result = int_enable(dw_timer->int_no[id]);
				if (E_OK != result) {
					break;
				}
				dmu_irq_enable(dw_timer->int_no[id], 1);

				result = timer_ops->enable(dw_timer, id, 1);
				if (E_OK != result) {
					break;
				}
			}
		} while (0);
		arc_unlock_restore(cpu_sts);
	}

	return result;
}

int32_t dw_timer_stop(uint32_t id)
{
	int32_t result = E_OK;

	dw_timer_ops_t *timer_ops = (dw_timer_ops_t *)dw_timer->ops;

	if ((NULL == dw_timer) || (NULL == dw_timer->ops) || \
	    (id >= dw_timer->timer_cnt)) {
		result = E_PAR;
	} else {
		uint32_t cpu_sts = arc_lock_save();
		result = timer_ops->enable(dw_timer, id, 0);
		if (E_OK == result) {
			expire_callback[id] = NULL;
			dmu_irq_enable(dw_timer->int_no[id], 0);
			result = int_disable(dw_timer->int_no[id]);
		}
		arc_unlock_restore(cpu_sts);
	}

	return result;
}

static int32_t dw_timer_current(uint32_t id, uint32_t *current)
{
	int32_t result = E_OK;

	dw_timer_ops_t *timer_ops = (dw_timer_ops_t *)dw_timer->ops;

	if ((NULL == dw_timer) || (NULL == dw_timer->ops) || \
	    (id >= dw_timer->timer_cnt)) {
		result = E_PAR;
	} else {
		result = timer_ops->current_count(dw_timer, id, current);
	}

	return result;
}

static void dw_timer_isr(void *param)
{
	int32_t result = E_OK;

	uint32_t timer_id = 0;

	dw_timer_ops_t *timer_ops = (dw_timer_ops_t *)dw_timer->ops;

	if ((NULL == dw_timer) || (NULL == timer_ops)) {
		result = E_SYS;
	} else {
		result = timer_ops->mod_int_status(dw_timer);
	}

	if (result > 0) {
		for (timer_id = 0; timer_id < dw_timer->timer_cnt; timer_id++) {
			result = timer_ops->int_status(dw_timer, timer_id);
			if (result > 0) {
				if (expire_callback[timer_id]) {
					expire_callback[timer_id]((void *)dw_timer);
				}

				result = timer_ops->int_clear(dw_timer, timer_id);
				if (E_OK != result) {
					break;
				}
			}
		}
	}

	if (E_OK == result) {
		/* TODO: System Crash! record! */
		;
	}
}
