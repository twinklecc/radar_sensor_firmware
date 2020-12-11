#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "arc_exception.h"

#include "dw_gpio.h"
#include "gpio_hal.h"


static dw_gpio_t *dev_gpio_ptr = NULL;
static callback gpio_callback_list[CHIP_GPIO_NO];


static void gpio_isr(void *param);

int32_t gpio_init(void)
{
	int32_t result = E_OK;


	if (NULL == dev_gpio_ptr) {
		dev_gpio_ptr = (dw_gpio_t *)dw_gpio_get_dev();
		if (NULL == dev_gpio_ptr) {
			result = E_NOEXS;
		} else {
			gpio_enable(1);
		}
	}

	if (E_OK == result) {
		uint32_t int_id = dev_gpio_ptr->int_no;
		for (; int_id < INTNO_GPIO_MAX; int_id++) {
			int_handler_install(int_id, gpio_isr);
		}
	}

	return result;
}

int32_t gpio_set_direct(uint32_t gpio_no, uint32_t dir)
{
	int32_t result = E_OK;

	dw_gpio_ops_t *gpio_ops = NULL;


	if (NULL == dev_gpio_ptr) {
		dev_gpio_ptr = (dw_gpio_t *)dw_gpio_get_dev();
		if (NULL == dev_gpio_ptr) {
			result = E_NOEXS;
		}
	}

	if (E_OK == result) {
		gpio_ops = (dw_gpio_ops_t *)dev_gpio_ptr->ops;
		if (NULL == gpio_ops) {
			result = E_NOOPS;
		} else {
			uint32_t cpu_sts = arc_lock_save();
			result = gpio_ops->set_direct(dev_gpio_ptr, \
					gpio_no, dir);
			arc_unlock_restore(cpu_sts);
		}
	}


	return result;
}

int32_t gpio_write(uint32_t gpio_no, uint32_t level)
{
	int32_t result = E_OK;

	dw_gpio_ops_t *gpio_ops = NULL;


	if (NULL == dev_gpio_ptr) {
		dev_gpio_ptr = (dw_gpio_t *)dw_gpio_get_dev();
		if (NULL == dev_gpio_ptr) {
			result = E_NOEXS;
		}
	}

	if (E_OK == result) {
		gpio_ops = (dw_gpio_ops_t *)dev_gpio_ptr->ops;
		if (NULL == gpio_ops) {
			result = E_NOOPS;
		} else {
			uint32_t cpu_sts = arc_lock_save();
			result = gpio_ops->write(dev_gpio_ptr, \
					gpio_no, level);
			arc_unlock_restore(cpu_sts);
		}
	}


	return result;
}

int32_t gpio_read(uint32_t gpio_no)
{
	int32_t result = E_OK;

	dw_gpio_ops_t *gpio_ops = NULL;


	if (NULL == dev_gpio_ptr) {
		dev_gpio_ptr = (dw_gpio_t *)dw_gpio_get_dev();
		if (NULL == dev_gpio_ptr) {
			result = E_NOEXS;
		}
	}

	if (E_OK == result) {
		gpio_ops = (dw_gpio_ops_t *)dev_gpio_ptr->ops;
		if (NULL == gpio_ops) {
			result = E_NOOPS;
		} else {
			result = gpio_ops->read(dev_gpio_ptr, gpio_no);
		}
	}


	return result;
}

int32_t gpio_int_register(uint32_t gpio_no, callback func, gpio_int_active_t type)
{
	int32_t result = E_OK;

	dw_gpio_ops_t *gpio_ops = NULL;

	uint32_t cpu_sts = arc_lock_save();

	if ( (gpio_no >= 16) || (NULL == func) || \
	     (type > GPIO_INT_BOTH_EDGE_ACTIVE)) {
		result = E_PAR;
	} else {
		gpio_callback_list[gpio_no] = func;

		if (NULL == dev_gpio_ptr) {
			dev_gpio_ptr = (dw_gpio_t *)dw_gpio_get_dev();
			if (NULL == dev_gpio_ptr) {
				result = E_NOEXS;
			}
		}
	}

	if (E_OK == result) {
		gpio_ops = (dw_gpio_ops_t *)dev_gpio_ptr->ops;
		if (NULL == gpio_ops) {
			result = E_NOOPS;
		} else {
			result = gpio_ops->set_direct(dev_gpio_ptr, \
					gpio_no, DW_GPIO_DIR_INPUT);
			if (E_OK == result) {
				result = gpio_ops->int_polarity(dev_gpio_ptr, \
						gpio_no, type);
			}
		}
	}

	if (E_OK == result) {
		/* reigster callback... */
		result = gpio_ops->int_enable(dev_gpio_ptr, gpio_no, 1, 0);
		if (E_OK == result) {
			int_enable(dev_gpio_ptr->int_no + gpio_no);
			dmu_irq_enable(dev_gpio_ptr->int_no + gpio_no, 1);
		} else {
			/* TODO: unregsiter callback! */
			gpio_callback_list[gpio_no] = NULL;
		}
	}
	arc_unlock_restore(cpu_sts);


	return result;
}

int32_t gpio_int_unregister(uint32_t gpio_no)
{
	int32_t result = E_OK;

	dw_gpio_ops_t *gpio_ops = NULL;

	uint32_t cpu_sts = arc_lock_save();

	if (gpio_no >= 16) {
		result = E_PAR;
	} else {
		if (NULL != dev_gpio_ptr) {
			int_disable(dev_gpio_ptr->int_no + gpio_no);
			gpio_ops = (dw_gpio_ops_t *)dev_gpio_ptr->ops;
			if (NULL == gpio_ops) {
				result = E_NOOPS;
			} else {
				result = gpio_ops->int_enable(dev_gpio_ptr, gpio_no, 0, 1);
			}
		} else {
			result = E_NOEXS;
		}
		gpio_callback_list[gpio_no] = NULL;
	}

	arc_unlock_restore(cpu_sts);

	return result;
}

int32_t gpio_interrupt_enable(uint32_t gpio_no, uint32_t en)
{
	int32_t result = E_OK;

	uint32_t irq_id = 0;
	dw_gpio_ops_t *gpio_ops = NULL;

	if ((NULL != dev_gpio_ptr) && (NULL != dev_gpio_ptr->ops)) {
		irq_id = dev_gpio_ptr->int_no + gpio_no;
		gpio_ops = (dw_gpio_ops_t *)dev_gpio_ptr->ops;
	}

	if ((gpio_no < 16) && (irq_id > 0)) {
		uint32_t cpu_sts = arc_lock_save();
		if (en) {
			dmu_irq_enable(irq_id, 1);
			result = gpio_ops->int_enable(dev_gpio_ptr,
					gpio_no, 1, 0);
		} else {
			dmu_irq_enable(irq_id, 0);
			result = gpio_ops->int_enable(dev_gpio_ptr,
					gpio_no, 0, 1);
		}
		arc_unlock_restore(cpu_sts);
	} else {
		result = E_PAR;
	}

	return result;
}

void gpio_interrupt_clear(uint32_t gpio_no)
{
	dw_gpio_ops_t *gpio_ops = NULL;
	if ((gpio_no < 16) && (NULL != dev_gpio_ptr)) {
		gpio_ops = (dw_gpio_ops_t *)dev_gpio_ptr->ops;
		if (NULL != gpio_ops) {
			gpio_ops->int_clear(dev_gpio_ptr, gpio_no);
		}
	}
}

static void gpio_isr(void *param)
{
	int32_t result = E_OK;

	uint32_t id = 0;
	uint32_t int_sts = 0;
	dw_gpio_ops_t *gpio_ops = NULL;

	if (NULL == dev_gpio_ptr) {
		dev_gpio_ptr = (dw_gpio_t *)dw_gpio_get_dev();
		if (NULL == dev_gpio_ptr) {
			result = E_NOEXS;
		}
	}

	if (E_OK == result) {
		gpio_ops = (dw_gpio_ops_t *)dev_gpio_ptr->ops;
		if (NULL != gpio_ops) {
			result = gpio_ops->int_all_status(dev_gpio_ptr);
			if (result <= 0) {
				result = E_SYS;
			} else {
				int_sts = (uint32_t)result;
				result = E_OK;
			}
		} else {
			result = E_NOOPS;
		}
	}

	if (E_OK == result) {
		/* handle interrupt! */
		while (int_sts) {
			if (int_sts & (1 << id)) {
				int_sts &= ~(1 << id);
				gpio_callback_list[id]((void *)dev_gpio_ptr);
				result = gpio_ops->int_clear(dev_gpio_ptr, id);
				if (E_OK != result) {
					break;
				}
			}
			id++;
		}
	}

	if (E_OK != result) {
		/* TODO: seriously, System Crash! */
	}
}
