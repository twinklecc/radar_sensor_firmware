#include <string.h>

#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC_debug.h"

#include "arc_exception.h"

#include "dw_gpio_reg.h"
#include "dw_gpio.h"

#define DW_GPIO_PORT_NUM	(2)
#define DW_GPIO_PORT_WIDTH	(16)

static int32_t dw_gpio_write(dw_gpio_t *dw_gpio, uint32_t gpio_id, uint32_t level)
{
	int32_t result = E_OK;

	if ((NULL == dw_gpio)) {
		result = E_PAR;
	} else {
		uint32_t val = 0;
		uint32_t reg_addr = dw_gpio->base;
		uint32_t reg_addr1 = reg_addr;

		uint32_t port_id = gpio_id / DW_GPIO_PORT_WIDTH;
		uint32_t signal_id = gpio_id % DW_GPIO_PORT_WIDTH;
		switch (port_id) {
			case DW_GPIO_PORT_A:
				reg_addr += REG_DW_GPIO_SWPORTA_DR_OFFSET;
				reg_addr1 += REG_DW_GPIO_SWPORTA_DDR_OFFSET;
				break;
			case DW_GPIO_PORT_B:
				reg_addr += REG_DW_GPIO_SWPORTB_DR_OFFSET;
				reg_addr1 += REG_DW_GPIO_SWPORTB_DDR_OFFSET;
				break;
			case DW_GPIO_PORT_C:
			case DW_GPIO_PORT_D:
			default:
				result = E_PAR;
		}

		if (0 == (raw_readl(reg_addr1) & (1 << signal_id))) {
			result = E_SYS;
		}

		if (E_OK == result) {
			val = raw_readl(reg_addr);
			if (level) {
				val |= (1 << signal_id);
			} else {
				val &= ~(1 << signal_id);
			}
			raw_writel(reg_addr, val);
		}
	}

	return result;
}

static int32_t dw_gpio_direct(dw_gpio_t *dw_gpio, uint32_t gpio_id, uint32_t direct)
{
	int32_t result = E_OK;

	if ((NULL == dw_gpio)) {
		result = E_PAR;
	} else {
		uint32_t val = 0;
		uint32_t reg_addr = dw_gpio->base;

		uint32_t port_id = gpio_id / DW_GPIO_PORT_WIDTH;
		uint32_t signal_id = gpio_id % DW_GPIO_PORT_WIDTH;
		switch (port_id) {
			case DW_GPIO_PORT_A:
				reg_addr += REG_DW_GPIO_SWPORTA_DDR_OFFSET;
				break;
			case DW_GPIO_PORT_B:
				reg_addr += REG_DW_GPIO_SWPORTB_DDR_OFFSET;
				break;
			case DW_GPIO_PORT_C:
			case DW_GPIO_PORT_D:
			default:
				result = E_PAR;
		}

		if (E_OK == result) {
			val = raw_readl(reg_addr);
			if (DW_GPIO_DIR_OUTPUT == direct) {
				val |= (1 << signal_id);
			} else {
				val &= ~(1 << signal_id);
			}
			raw_writel(reg_addr, val);
		}
	}

	return result;
}

static int32_t dw_gpio_get_direct(dw_gpio_t *dw_gpio, uint32_t gpio_id)
{
	int32_t result = E_OK;

	if ((NULL == dw_gpio)) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = dw_gpio->base;

		uint32_t port_id = gpio_id / DW_GPIO_PORT_WIDTH;
		uint32_t signal_id = gpio_id % DW_GPIO_PORT_WIDTH;
		switch (port_id) {
			case DW_GPIO_PORT_A:
				reg_addr += REG_DW_GPIO_SWPORTA_DDR_OFFSET;
				break;
			case DW_GPIO_PORT_B:
				reg_addr += REG_DW_GPIO_SWPORTB_DDR_OFFSET;
				break;
			case DW_GPIO_PORT_C:
			case DW_GPIO_PORT_D:
			default:
				result = E_PAR;
		}

		if (E_OK == result) {
			if (raw_readl(reg_addr) & (1 << signal_id)) {
				result = DW_GPIO_DIR_OUTPUT;
			} else {
				result = DW_GPIO_DIR_INPUT;
			}
		}
	}

	return result;
}

static int32_t dw_gpio_mode(dw_gpio_t *dw_gpio, uint32_t gpio_id, uint32_t mode)
{
	int32_t result = E_OK;

	if ((NULL == dw_gpio)) {
		result = E_PAR;
	} else {
		uint32_t val = 0;
		uint32_t reg_addr = dw_gpio->base;

		uint32_t port_id = gpio_id / DW_GPIO_PORT_WIDTH;
		uint32_t signal_id = gpio_id % DW_GPIO_PORT_WIDTH;
		switch (port_id) {
			case DW_GPIO_PORT_A:
				reg_addr += REG_DW_GPIO_SWPORTA_CTL_OFFSET;
				break;
			case DW_GPIO_PORT_B:
				reg_addr += REG_DW_GPIO_SWPORTB_CTL_OFFSET;
				break;
			case DW_GPIO_PORT_C:
			case DW_GPIO_PORT_D:
			default:
				result = E_PAR;
		}

		if (E_OK == result) {
			val = raw_readl(reg_addr);
			if (DW_GPIO_HW_MODE == mode) {
				val |= (1 << signal_id);
			} else {
				val &= ~(1 << signal_id);
			}
			raw_writel(reg_addr, val);
		}
	}

	return result;
}
static int32_t dw_gpio_interrupt_enable(dw_gpio_t *dw_gpio, uint32_t gpio_id, uint32_t enable, uint32_t mask)
{
	int32_t result = E_OK;

	if ((NULL == dw_gpio)) {
		result = E_PAR;
	} else {
		uint32_t port_id = gpio_id / DW_GPIO_PORT_WIDTH;
		uint32_t signal_id = gpio_id % DW_GPIO_PORT_WIDTH;

		if (DW_GPIO_PORT_A != port_id) {
			result = E_PAR;
		} else {
			uint32_t reg_addr = REG_DW_GPIO_INTEN_OFFSET + dw_gpio->base;
			uint32_t val = raw_readl(reg_addr);
			if (enable) {
				val |= (1 << signal_id);
			} else {
				val &= ~(1 << signal_id);
			}
			raw_writel(reg_addr, val);

			reg_addr = REG_DW_GPIO_INTMASK_OFFSET + dw_gpio->base;
			val = raw_readl(reg_addr);
			if (mask) {
				val |= (1 << signal_id);
			} else {
				val &= ~(1 << signal_id);
			}
			raw_writel(reg_addr, val);
		}
	}

	return result;
}

static int32_t dw_gpio_interrupt_polarity(dw_gpio_t *dw_gpio, uint32_t gpio_id, uint32_t polarity)
{
	int32_t result = E_OK;

	if ((NULL == dw_gpio)) {
		result = E_PAR;
	} else {
		uint32_t val = 0;
		uint32_t reg_addr = dw_gpio->base;
		uint32_t port_id = gpio_id / DW_GPIO_PORT_WIDTH;
		uint32_t signal_id = gpio_id % DW_GPIO_PORT_WIDTH;

		if (DW_GPIO_PORT_A != port_id) {
			result = E_PAR;
		} else {
			switch (polarity) {
				case DW_GPIO_INT_HIGH_LEVEL_ACTIVE:
					val = raw_readl(reg_addr + REG_DW_GPIO_INTTYPE_LEVEL_OFFSET);
					val &= ~(1 << signal_id);
					raw_writel(reg_addr + REG_DW_GPIO_INTTYPE_LEVEL_OFFSET, val);
					val = raw_readl(REG_DW_GPIO_INTPOLARITY_OFFSET + reg_addr);
					val |= (1 << signal_id);
					raw_writel(REG_DW_GPIO_INTPOLARITY_OFFSET + reg_addr, val);
					val = raw_readl(REG_DW_GPIO_INT_BOTHEDGE_OFFSET + reg_addr);
					val &= ~(1 << signal_id);
					raw_writel(REG_DW_GPIO_INT_BOTHEDGE_OFFSET + reg_addr, val);
					break;
				case DW_GPIO_INT_LOW_LEVEL_ACTIVE:
					val = raw_readl(reg_addr + REG_DW_GPIO_INTTYPE_LEVEL_OFFSET);
					val &= ~(1 << signal_id);
					raw_writel(reg_addr + REG_DW_GPIO_INTTYPE_LEVEL_OFFSET, val);
					val = raw_readl(REG_DW_GPIO_INTPOLARITY_OFFSET + reg_addr);
					val &= ~(1 << signal_id);
					raw_writel(REG_DW_GPIO_INTPOLARITY_OFFSET + reg_addr, val);
					val = raw_readl(REG_DW_GPIO_INT_BOTHEDGE_OFFSET + reg_addr);
					val &= ~(1 << signal_id);
					raw_writel(REG_DW_GPIO_INT_BOTHEDGE_OFFSET + reg_addr, val);
					break;
				case DW_GPIO_INT_RISING_EDGE_ACTIVE:
					val = raw_readl(reg_addr + REG_DW_GPIO_INTTYPE_LEVEL_OFFSET);
					val |= (1 << signal_id);
					raw_writel(reg_addr + REG_DW_GPIO_INTTYPE_LEVEL_OFFSET, val);
					val = raw_readl(REG_DW_GPIO_INTPOLARITY_OFFSET + reg_addr);
					val |= (1 << signal_id);
					raw_writel(REG_DW_GPIO_INTPOLARITY_OFFSET + reg_addr, val);
					val = raw_readl(REG_DW_GPIO_INT_BOTHEDGE_OFFSET + reg_addr);
					val &= ~(1 << signal_id);
					raw_writel(REG_DW_GPIO_INT_BOTHEDGE_OFFSET + reg_addr, val);
					break;
				case DW_GPIO_INT_FALLING_EDGE_ACTIVE:
					val = raw_readl(reg_addr + REG_DW_GPIO_INTTYPE_LEVEL_OFFSET);
					val |= (1 << signal_id);
					raw_writel(reg_addr + REG_DW_GPIO_INTTYPE_LEVEL_OFFSET, val);
					val = raw_readl(REG_DW_GPIO_INTPOLARITY_OFFSET + reg_addr);
					val &= ~(1 << signal_id);
					raw_writel(REG_DW_GPIO_INTPOLARITY_OFFSET + reg_addr, val);
					val = raw_readl(REG_DW_GPIO_INT_BOTHEDGE_OFFSET + reg_addr);
					val &= ~(1 << signal_id);
					raw_writel(REG_DW_GPIO_INT_BOTHEDGE_OFFSET + reg_addr, val);
					break;
				case DW_GPIO_INT_BOTH_EDGE_ACTIVE:
					val = raw_readl(reg_addr + REG_DW_GPIO_INTTYPE_LEVEL_OFFSET);
					val |= (1 << signal_id);
					raw_writel(reg_addr + REG_DW_GPIO_INTTYPE_LEVEL_OFFSET, val);
					val = raw_readl(REG_DW_GPIO_INT_BOTHEDGE_OFFSET + reg_addr);
					val |= (1 << signal_id);
					raw_writel(REG_DW_GPIO_INT_BOTHEDGE_OFFSET + reg_addr, val);
					break;
				default:
					result = E_PAR;
			}
		}
	}

	return result;
}

static int32_t dw_gpio_interrupt_all_status(dw_gpio_t *dw_gpio)
{
	int32_t result = E_OK;

	if ((NULL == dw_gpio)) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = dw_gpio->base + REG_DW_GPIO_INTSTATUS_OFFSET;
		result = raw_readl(reg_addr);
	}

	return result;
}

static int32_t dw_gpio_interrupt_status(dw_gpio_t *dw_gpio, uint32_t gpio_id)
{
	int32_t result = E_OK;

	if ((NULL == dw_gpio)) {
		result = E_PAR;
	} else {
		uint32_t port_id = gpio_id / DW_GPIO_PORT_WIDTH;
		uint32_t signal_id = gpio_id % DW_GPIO_PORT_WIDTH;

		uint32_t reg_addr = dw_gpio->base + REG_DW_GPIO_INTSTATUS_OFFSET;
		if (DW_GPIO_PORT_A == port_id) {
			result = (raw_readl(reg_addr) & (1 << signal_id)) ? (1) : (0);
		} else {
			result = E_PAR;
		}
	}

	return result;
}

static int32_t dw_gpio_interrupt_raw_status(dw_gpio_t *dw_gpio, uint32_t gpio_id)
{
	int32_t result = E_OK;

	if ((NULL == dw_gpio)) {
		result = E_PAR;
	} else {
		uint32_t port_id = gpio_id / DW_GPIO_PORT_WIDTH;
		uint32_t signal_id = gpio_id % DW_GPIO_PORT_WIDTH;

		uint32_t reg_addr = dw_gpio->base + REG_DW_GPIO_RAW_INTSTATUS_OFFSET;
		if (DW_GPIO_PORT_A == port_id) {
			result = (raw_readl(reg_addr) & (1 << signal_id)) ? (1) : (0);
		} else {
			result = E_PAR;
		}
	}

	return result;
}

static int32_t dw_gpio_interrupt_clear(dw_gpio_t *dw_gpio, uint32_t gpio_id)
{
	int32_t result = E_OK;

	if ((NULL == dw_gpio)) {
		result = E_PAR;
	} else {
		uint32_t port_id = gpio_id / DW_GPIO_PORT_WIDTH;
		uint32_t signal_id = gpio_id % DW_GPIO_PORT_WIDTH;

		uint32_t reg_addr = dw_gpio->base + REG_DW_GPIO_PORTA_EOI_OFFSET;
		if (DW_GPIO_PORT_A == port_id) {
			raw_writel(reg_addr, 1 << signal_id);
		} else {
			result = E_PAR;
		}
	}

	return result;
}

static int32_t dw_gpio_debounce(dw_gpio_t *dw_gpio, uint32_t gpio_id, uint32_t enable)
{
	int32_t result = E_OK;

	if ((NULL == dw_gpio)) {
		result = E_PAR;
	} else {
		uint32_t port_id = gpio_id / DW_GPIO_PORT_WIDTH;
		uint32_t signal_id = gpio_id % DW_GPIO_PORT_WIDTH;

		uint32_t reg_addr = dw_gpio->base + REG_DW_GPIO_DEBOUNCE_OFFSET;
		if (DW_GPIO_PORT_A == port_id) {
			uint32_t val = raw_readl(reg_addr);
			if (enable) {
				val |= (1 << signal_id);
			} else {
				val &= ~(1 << signal_id);
			}
			raw_writel(reg_addr, val);
		} else {
			result = E_PAR;
		}
	}

	return result;
}

static int32_t dw_gpio_read(dw_gpio_t *dw_gpio, uint32_t gpio_id)
{
	int32_t result = E_OK;

	if ((NULL == dw_gpio)) {
		result = E_PAR;
	} else {
		uint32_t port_id = gpio_id / DW_GPIO_PORT_WIDTH;
		uint32_t signal_id = gpio_id % DW_GPIO_PORT_WIDTH;

		uint32_t reg_addr = dw_gpio->base;
		uint32_t reg_addr1 = reg_addr;
		switch (port_id) {
			case DW_GPIO_PORT_A:
				reg_addr += REG_DW_GPIO_EXT_PORTA_OFFSET;
				reg_addr1 += REG_DW_GPIO_SWPORTA_DDR_OFFSET;
				break;
			case DW_GPIO_PORT_B:
				reg_addr += REG_DW_GPIO_EXT_PORTB_OFFSET;
				reg_addr1 += REG_DW_GPIO_SWPORTB_DDR_OFFSET;
				break;
			case DW_GPIO_PORT_C:
				//reg_addr += REG_DW_GPIO_EXT_PORTC_OFFSET;
				break;
			case DW_GPIO_PORT_D:
				//reg_addr += REG_DW_GPIO_EXT_PORTD_OFFSET;
				break;
			default:
				result = E_PAR;
		}

		if (raw_readl(reg_addr1) & (1 << signal_id)) {
			result = E_SYS;
		}

		if (E_OK == result) {
			result = (raw_readl(reg_addr) & (1 << signal_id)) ? (1) : (0);
		}
	}

	return result;
}

static int32_t dw_gpio_version(dw_gpio_t *dw_gpio)
{
	int32_t result = E_OK;

	if (NULL == dw_gpio) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = REG_DW_GPIO_VER_ID_CODE_OFFSET + dw_gpio->base;
		result = raw_readl(reg_addr);
	}

	return result;
}

static dw_gpio_ops_t dw_gpio_ops = {
	.mode = dw_gpio_mode,
	.set_direct = dw_gpio_direct,
	.get_direct = dw_gpio_get_direct,
	.write = dw_gpio_write,
	.read = dw_gpio_read,
	.debounce = dw_gpio_debounce,
	.int_enable = dw_gpio_interrupt_enable,
	.int_polarity = dw_gpio_interrupt_polarity,
	.int_all_status = dw_gpio_interrupt_all_status,
	.int_status = dw_gpio_interrupt_status,
	.int_raw_status = dw_gpio_interrupt_raw_status,
	.int_clear = dw_gpio_interrupt_clear,
	.version = dw_gpio_version
};

int32_t dw_gpio_install_ops(dw_gpio_t *dw_gpio)
{
	int32_t result = E_OK;

	if (NULL == dw_gpio) {
		result = E_PAR;
	} else {
		dw_gpio->ops = (void *)&dw_gpio_ops;
	}

	return result;
}
