#include <string.h>
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC_debug.h"
#include "arc_exception.h"
#include "dw_timer_reg.h"
#include "dw_timer.h"

static int32_t dw_timer_enable(dw_timer_t *timer, uint32_t id, uint32_t en)
{
	int32_t result = E_OK;

	if ( (NULL == timer) || (id >= timer->timer_cnt)) {
		result = E_PAR;
	} else {
		uint32_t reg = DW_TIMER_N_REG_CONTROL_OFFSET(id) + timer->base;
		uint32_t value = raw_readl(reg);

		if (en) {
			value |= DW_TIMER_BIT_EN;
		} else {
			value &= ~DW_TIMER_BIT_EN;
		}
		raw_writel(reg, value);
	}

	return result;
}

static int32_t dw_timer_load_count_update(dw_timer_t *timer, uint32_t id, uint32_t count)
{
	int32_t result = E_OK;

	if ( (NULL == timer) || (id >= timer->timer_cnt)) {
		result = E_PAR;
	} else {
		uint32_t reg = DW_TIMER_N_REG_LOADCOUNT_OFFSET(id) + timer->base;

		raw_writel(reg, count);
	}

	return result;
}

static int32_t dw_timer_load_count2_update(dw_timer_t *timer, uint32_t id, uint32_t count)
{
	int32_t result = E_OK;

	if ( (NULL == timer) || (id >= timer->timer_cnt)) {
		result = E_PAR;
	} else {
		uint32_t reg = DW_TIMER_N_REG_LOADCOUNT2_OFFSET(id) + timer->base;

		raw_writel(reg, count);
	}

	return result;
}

static int32_t dw_timer_current_count(dw_timer_t *timer, uint32_t id, uint32_t *cur)
{
	uint32_t result = E_OK;

	if ( (NULL == timer) ||(id >= timer->timer_cnt)) {
		result = E_PAR;
	} else {
		uint32_t reg = DW_TIMER_N_REG_CURCOUNT_OFFSET(id) + timer->base;

		*cur = raw_readl(reg);
	}

	return result;
}

static int32_t dw_timer_int_mask(dw_timer_t *timer, uint32_t id, uint32_t mask)
{
	int32_t result = E_OK;

	if ( (NULL == timer) || (id >= timer->timer_cnt)) {
		result = E_PAR;
	} else {
		uint32_t reg = DW_TIMER_N_REG_CONTROL_OFFSET(id) + timer->base;
		uint32_t value = raw_readl(reg);

		if (mask) {
			value |= DW_TIMER_BIT_INT_MASK;
		} else {
			value &= ~DW_TIMER_BIT_INT_MASK;
		}
		raw_writel(reg, value);
	}

	return result;
}

static int32_t dw_timer_int_clear(dw_timer_t *timer, uint32_t id)
{
	int32_t result = E_OK;

	if ( (NULL == timer) || (id >= timer->timer_cnt)) {
		result = E_PAR;
	} else {
		uint32_t reg = DW_TIMER_N_REG_NEOI_OFFSET(id) + timer->base;

		result = raw_readl(reg);
	}

	return result;
}

static int32_t dw_timer_int_status(dw_timer_t *timer, uint32_t id)
{
	int32_t result = E_OK;

	if ( (NULL == timer) || (id >= timer->timer_cnt)) {
		result = E_PAR;
	} else {
		uint32_t reg = DW_TIMER_N_REG_INT_STS_OFFSET(id) + timer->base;

		result = raw_readl(reg);
	}

	return result;
}


static int32_t dw_timer_status_clear(dw_timer_t *timer)
{
	int32_t result = E_OK;

	if ( (NULL == timer) ) {
		result = E_PAR;
	} else {
		uint32_t reg = DW_TIMER_REG_EOI_OFFSET + timer->base;

		result = raw_readl(reg);
	}

	return result;
}

static int32_t dw_timer_raw_status(dw_timer_t *timer)
{
	int32_t result = E_OK;

	if ( (NULL == timer) ) {
		result = E_PAR;
	} else {
		uint32_t reg = DW_TIMER_REG_RAW_INT_STS_OFFSET + timer->base;

		result = raw_readl(reg);
	}

	return result;
}

static int32_t dw_timer_status(dw_timer_t *timer)
{
	int32_t result = E_OK;

	if ( (NULL == timer) ) {
		result = E_PAR;
	} else {
		uint32_t reg = DW_TIMER_REG_INT_STS_OFFSET + timer->base;

		result = raw_readl(reg);
	}

	return result;
}

static int32_t dw_timer_mode(dw_timer_t *timer, uint32_t id, uint32_t mode)
{
	int32_t result = E_OK;

	if ( (NULL == timer) || (id >= timer->timer_cnt) ) {
		result = E_PAR;
	} else {
		uint32_t reg = DW_TIMER_N_REG_CONTROL_OFFSET(id) + timer->base;
		uint32_t value = raw_readl(reg);

		if (mode) {
			value |= DW_TIMER_BIT_MODE;
		} else {
			value &= ~DW_TIMER_BIT_MODE;
		}
		raw_writel(reg, value);
	}

	return result;
}

static int32_t dw_timer_pwm(dw_timer_t *timer, uint32_t id, uint32_t pwm_en, uint32_t pwm_0n100_en)
{
	int32_t result = 0;

	if ( (NULL == timer) || (id >= timer->timer_cnt)) {
		result = E_PAR;
	} else {
		uint32_t reg = DW_TIMER_N_REG_CONTROL_OFFSET(id) + timer->base;
		uint32_t value = raw_readl(reg);

		if (pwm_en) {
			value |= DW_TIMER_BIT_PWM_EN;
		} else {
			value &= ~DW_TIMER_BIT_PWM_EN;
		}
		if (pwm_0n100_en) {
			value |= DW_TIMER_BIT_0N100PWM_EN;
		} else {
			value &= ~DW_TIMER_BIT_0N100PWM_EN;
		}
		raw_writel(reg, value);
	}

	return result;
}

static uint32_t dw_timer_version(dw_timer_t *timer)
{
	uint32_t result = E_OK;

	if ( (NULL == timer) ) {
		result = E_PAR;
	} else {
		uint32_t reg = DW_TIMER_REG_VERSION_OFFSET + timer->base;
		result = raw_readl(reg);
	}
	return result;
}

static dw_timer_ops_t basic_timer_ops = {
	.enable = dw_timer_enable,
	.load_count_update = dw_timer_load_count_update,
	.load_count2_update = dw_timer_load_count2_update,
	.current_count = dw_timer_current_count,

	.int_mask = dw_timer_int_mask,
	.int_clear = dw_timer_int_clear,
	.int_status = dw_timer_int_status,

	/* for module. */
	.mod_int_clear = dw_timer_status_clear,
	.mod_int_raw_status = dw_timer_raw_status,
	.mod_int_status = dw_timer_status,

	.mode = dw_timer_mode,
	.pwm =  dw_timer_pwm,

	.version = dw_timer_version
};

int32_t dw_timer_install_ops(dw_timer_t *timer)
{
	int32_t result = E_OK;

	if ( (NULL == timer) ) {
		result = E_PAR;
	} else {
		timer->ops = (void *)&basic_timer_ops;
	}

	return result;
}
