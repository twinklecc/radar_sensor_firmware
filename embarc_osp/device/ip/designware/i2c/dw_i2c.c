#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "dev_common.h"
#include "arc_exception.h"
#include "dw_i2c.h"
#include "dw_i2c_reg.h"
#include "i2c_hal.h"


static int32_t dw_i2c_control(dw_i2c_t *dev_i2c, dw_i2c_control_t *ctrl)
{
	int32_t result = E_OK;

	uint32_t value = 0;
	if ((NULL == dev_i2c) || (NULL == ctrl)) {
		result = E_PAR;
	} else {
		if (ctrl->tx_empty_inten) {
			value |= BIT_I2C_CON_TX_EMPTY_CTRL;
		}
		if (ctrl->mode) {
			value |= BIT_I2C_CON_M_MODE;
		}
		if (ctrl->restart_en) {
			value |= BIT_I2C_CON_RESTART_EN;
		}
		value |= (ctrl->speed & BITS_I2C_CON_SPEED_MASK) << BITS_I2C_CON_SPEED_OFFSET;
		if (ctrl->addr_mode) {
			value |= BIT_I2C_CON_M_10BIT_ADDR;
		}
		raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_CON_OFFSET, value);
	}

	return result;
}

static int32_t dw_i2c_target_address(dw_i2c_t *dev_i2c, dw_i2c_address_mode_t addr_mode, uint32_t addr)
{
	int32_t result = E_OK;

	if (NULL == dev_i2c) {
		result = E_PAR;
	} else {
		uint32_t value = addr & 0x3FF;
		if (addr_mode) {
			value |= (1 << 12);
		}
		raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_TAR_OFFSET, value);
	}
	return result;
}


static int32_t dw_i2c_hs_master_code(dw_i2c_t *dev_i2c, uint32_t code)
{
	int32_t result = E_OK;
	if (NULL == dev_i2c) {
		result = E_PAR;
	} else {
		raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_HS_MADDR_OFFSET, code);
	}
	return result;
}
static int32_t dw_i2c_fifo_data_count(dw_i2c_t *dev_i2c, uint32_t *rxfifo_cnt, uint32_t *txfifo_cnt)
{
	int32_t result = E_OK;
	if ((NULL == dev_i2c) || ((NULL == rxfifo_cnt) && (NULL == txfifo_cnt))) {
		result = E_PAR;
	}
	else {
		if (rxfifo_cnt) {
			*rxfifo_cnt = raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_RXFLR_OFFSET) & 0x3F;
		}
		if (txfifo_cnt) {
			*txfifo_cnt = raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_TXFLR_OFFSET) & 0x3F;
		}
	}
	return result;
}
static int32_t dw_i2c_fifo_level(dw_i2c_t *dev_i2c, uint32_t *rx_thres, uint32_t *tx_thres)
{
	int32_t result = E_OK;
	int32_t val = 0;
	if ((NULL == dev_i2c) || ((NULL == rx_thres) && (NULL == tx_thres))) {
		result = E_PAR;
	}
	else {
		if (rx_thres) {
			val = raw_readl(dev_i2c->base + REG_DW_I2C_RXFLR_OFFSET);
			*rx_thres = (val & 0x3F);
		}
		if (tx_thres) {
			val = raw_readl(dev_i2c->base + REG_DW_I2C_TXFLR_OFFSET);
			*tx_thres = (val & 0x3F);
		}
	}
	return result;
}
static int32_t dw_i2c_write_command(dw_i2c_t *dev_i2c, uint32_t cmd)
{
	int32_t result = E_OK;

	if ((NULL == dev_i2c)) {
		result = E_PAR;
	} else {
		/* TODO: wait fifo not full. */
		raw_writel(dev_i2c->base + REG_DW_I2C_DATA_CMD_OFFSET, cmd);
	}
	return result;
}

static int32_t dw_i2c_getdata(dw_i2c_t *dev_i2c, uint32_t *cmd)
{
	int32_t result = E_OK;
	uint32_t rx_fifo_level = 0;
	uint32_t *count = &rx_fifo_level;
	if ((NULL == dev_i2c)) {
		result = E_PAR;
	}	
	else {		
		while (1) {
			dw_i2c_fifo_level(dev_i2c, count, NULL);
			if (*count > 0) {
				break;
			}
		}
		result = raw_readl(dev_i2c->base + REG_DW_I2C_DATA_CMD_OFFSET) & 0xFF;
	}
	return result;
}

static int32_t dw_i2c_scl_count(dw_i2c_t *dev_i2c, dw_i2c_speed_mode_t speed, uint32_t high, uint32_t low)
{
	int32_t result = E_OK;
	if (NULL == dev_i2c) {
		result = E_PAR;
	} else {
		uint32_t h_offset = 0, l_offset = 0;
		switch (speed) {
			case I2C_SPPED_STANDARD_MODE:
				h_offset = REG_DW_I2C_SS_SCL_HCNT_OFFSET;
				l_offset = REG_DW_I2C_SS_SCL_LCNT_OFFSET;
				break;
			case I2C_SPEED_FAST_MODE:
				h_offset = REG_DW_I2C_FS_SCL_HCNT_OFFSET;
				l_offset = REG_DW_I2C_FS_SCL_LCNT_OFFSET;
				break;
			case I2C_SPEED_HIGH_MODE:
				h_offset = REG_DW_I2C_HS_SCL_HCNT_OFFSET;
				l_offset = REG_DW_I2C_HS_SCL_LCNT_OFFSET;
				break;
			default:
				result = E_PAR;
		}
		if (E_OK == result) {
			raw_writel(dev_i2c->base + h_offset, high);
			raw_writel(dev_i2c->base + l_offset, low);
		}
	}
	return result;
}

static int32_t dw_i2c_interrupt_status(dw_i2c_t *dev_i2c, uint32_t *status)
{
	int32_t result = E_OK;
	if ((NULL == dev_i2c) || (NULL == status)) {
		result = E_PAR;
	} else {
		*status = raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_INTR_STAT_OFFSET);
	}
	return result;
}

static int32_t dw_i2c_interrupt_mask(dw_i2c_t *dev_i2c, uint32_t bitmap, uint32_t flag)
{
	int32_t result = E_OK;
	if (NULL == dev_i2c) {
		result = E_PAR;
	} else {
		uint32_t val = raw_readl(dev_i2c->base + REG_DW_I2C_INTR_MASK_OFFSET);
		if (flag) {
			val |= bitmap;
		} else {
			val &= ~bitmap;
		}
		raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_INTR_MASK_OFFSET, val);
	}
	return result;
}

static int32_t dw_i2c_interrupt_clear(dw_i2c_t *dev_i2c, dw_i2c_int_type_t *type)
{
	int32_t result = E_OK;
	if (NULL == dev_i2c) {
		result = E_PAR;
	} else {
		if (type) {
			uint32_t offset = 0;
			switch (*type) {
				case DW_I2C_RX_UNDER:
					offset = REG_DW_I2C_CLR_RX_UNDER_OFFSET;
					break;
				case DW_I2C_RX_OVER:
					offset = REG_DW_I2C_CLR_RX_OVER_OFFSET;
					break;
				case DW_I2C_TX_OVER:
					offset = REG_DW_I2C_CLR_TX_OVER_OFFSET;
					break;
				case DW_I2C_TX_EMPTY:
					break;
				case DW_I2C_TX_ABRT:
					offset = REG_DW_I2C_CLR_TX_ABORT_OFFSET;
					break;
				case DW_I2C_ACTIVITY:
					offset = REG_DW_I2C_CLR_ACTIVITY_OFFSET;
					break;
				case DW_I2C_STOP_DET:
					offset = REG_DW_I2C_CLR_STOP_DET_OFFSET;
					break;
				case DW_I2C_START_DET:
					offset = REG_DW_I2C_CLR_START_DET_OFFSET;
					break;
				case DW_I2C_GEN_CALL:
					offset = REG_DW_I2C_CLR_GEN_CALL_OFFSET;
					break;
				default:
					raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CLR_INTR_OFFSET);
					result = E_PAR;
					break;
			}
			if (E_OK == result) {
				raw_readl(dev_i2c->base + offset);
			}

		} else {
			raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CLR_INTR_OFFSET);
		}
	}
	return result;
}

static int32_t dw_i2c_fifo_threshold(dw_i2c_t *dev_i2c, uint32_t *rx_thres, uint32_t *tx_thres)
{
	int32_t result = E_OK;
	if ((NULL == dev_i2c) || ((NULL == rx_thres) && (NULL == tx_thres))) {
		result = E_PAR;
	} else {
		if (rx_thres) {
			raw_writel(dev_i2c->base + REG_DW_I2C_RX_TL_OFFSET, *rx_thres);
		}
		if (tx_thres) {
			raw_writel(dev_i2c->base + REG_DW_I2C_TX_TL_OFFSET, *tx_thres);
		}
	}
	return result;
}

static int32_t dw_i2c_enable(dw_i2c_t *dev_i2c, uint32_t tx_block, uint32_t abort, uint32_t enable)
{
	int32_t result = E_OK;
	if (NULL == dev_i2c) {
		result = E_PAR;
	} else {
		uint32_t value = 0;
		if (tx_block) {
			value |= (1 << 2);
		}
		if (abort) {
			value |= (1 << 1);
		}
		if (enable) {
			value |= (1 << 0);
		}
		raw_writel(dev_i2c->base + REG_DW_I2C_ENABLE_OFFSET, value);
	}
	return result;
}

static int32_t dw_i2c_status(dw_i2c_t *dev_i2c, uint32_t *status)
{
	int32_t result = E_OK;
	if ((NULL == dev_i2c) || (NULL == status)) {
		result = E_PAR;
	} else {
		*status = raw_readl(dev_i2c->base + REG_DW_I2C_STATUS_OFFSET);
	}
	return result;
}

static int32_t dw_i2c_sda_hold_time(dw_i2c_t *dev_i2c, uint32_t rx_cycles, uint32_t tx_cycles)
{
	int32_t result = E_OK;
	if (NULL == dev_i2c) {
		result = E_PAR;
	} else {
		uint32_t value = tx_cycles & 0xFFFF;
		value |= (rx_cycles & 0xFF) << 16;
		raw_writel(dev_i2c->base + REG_DW_I2C_SDA_HOLD_OFFSET, value);
	}
	return result;
}

static int32_t dw_i2c_spike_suppression_limit(dw_i2c_t *dev_i2c, dw_i2c_speed_mode_t speed, uint32_t limit)
{
	int32_t result = E_OK;
	if (NULL == dev_i2c) {
		result = E_PAR;
	} else {
		uint32_t offset = 0;
		switch (speed) {
			case I2C_SPPED_STANDARD_MODE:
			case I2C_SPEED_FAST_MODE:
				offset = REG_DW_I2C_FS_SPKLEN_OFFSET;
				break;
			case I2C_SPEED_HIGH_MODE:
				offset = REG_DW_I2C_HS_SPKLEN_OFFSET;
				break;
			default:
				result = E_PAR;
				break;
		}
		if (E_OK == result) {
			raw_writel(dev_i2c->base + offset, limit);
		}
	}
	return result;
}

static dw_i2c_ops_t i2c_ops = {
	.control = dw_i2c_control,
	.target_address = dw_i2c_target_address,
	.master_code = dw_i2c_hs_master_code,
	.write = dw_i2c_write_command,
	.read = dw_i2c_getdata,
	.scl_count = dw_i2c_scl_count,
	.int_status = dw_i2c_interrupt_status,
	.int_mask = dw_i2c_interrupt_mask,
	.int_clear = dw_i2c_interrupt_clear,
	.fifo_threshold = dw_i2c_fifo_threshold,
	.enable = dw_i2c_enable,
	.status = dw_i2c_status,
	.fifo_data_count = dw_i2c_fifo_data_count,
	.sda_hold_time = dw_i2c_sda_hold_time,
	.spike_suppression_limit = dw_i2c_spike_suppression_limit
};


int32_t dw_i2c_install_ops(dw_i2c_t *dev_i2c)
{
	int32_t result = E_OK;

	if (NULL == dev_i2c) {
		result = E_PAR;
	} else {
		dev_i2c->ops = (void *)&i2c_ops;
	}

	return result;
}
