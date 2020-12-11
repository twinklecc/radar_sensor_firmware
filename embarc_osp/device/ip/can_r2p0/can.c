#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC_debug.h"
#include "arc_exception.h"
#include "can_reg.h"
#include "can.h"

static inline int32_t can_opmode_config(can_t *can, uint32_t mode);

static uint32_t can_version(can_t *can)
{
	uint32_t result = E_OK;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_VERSION_OFFSET + can->base;
		result = raw_readl(reg);
	}

	return result;
}

static int32_t can_dma_enable(can_t *can, uint32_t rx, uint32_t tx)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_MODE_CTRL_OFFSET	+ can->base;
		val = raw_readl(reg);
		if (rx) {
			val |= BIT_MODE_CTRL_RDENA;
		} else {
			val &= ~BIT_MODE_CTRL_RDENA;
		}
		if (tx) {
			val |= BIT_MODE_CTRL_TDENA;
		} else {
			val &= ~BIT_MODE_CTRL_TDENA;
		}
		raw_writel(reg, val);
	}

	return result;
}

static int32_t can_ecc_enable(can_t *can, uint32_t en)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_MODE_CTRL_OFFSET	+ can->base;

		result = can_opmode_config(can, CONFIG_MODE);
		if (E_OK == result) {
			val = raw_readl(reg);
			if (en) {
				val |= BIT_MODE_CTRL_ECCENA;
			} else {
				val &= ~BIT_MODE_CTRL_ECCENA;
			}
			raw_writel(reg, val);
		}
		result = can_opmode_config(can, NORMAL_MODE);
	}

	return result;
}

static inline int32_t can_opmode_config(can_t *can, uint32_t mode)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	uint32_t reg = REG_CAN_MODE_CTRL_OFFSET	+ can->base;

	/* enter config mode. */
	val = raw_readl(reg);
	val |= BIT_MODE_CTRL_CFG;
	raw_writel(reg, val);

	val &= ~(
		BIT_MODE_CTRL_TEST | \
		BIT_MODE_CTRL_SLEEP | \
		BIT_MODE_CTRL_ROPT | \
		BIT_MODE_CTRL_MON | \
		BIT_MODE_CTRL_LBACK);

	switch (mode) {
		case NORMAL_MODE:
			break;
		case CONFIG_MODE:
			val |= BIT_MODE_CTRL_CFG;
			break;
		case RESTRICTED_MODE:
			val |= BIT_MODE_CTRL_ROPT;
			break;
		case MONITOR_MODE:
			val |= BIT_MODE_CTRL_MON;
			break;
		case SLEEP_MODE:
			val |= BIT_MODE_CTRL_SLEEP;
			break;
		case TEST_MODE:
			val |= BIT_MODE_CTRL_TEST;
			break;
		case LOOP_MODE:
			val |= BIT_MODE_CTRL_LBACK;
			break;
		default:
			result = E_PAR;
	}

	if (mode != CONFIG_MODE) {
		raw_writel(reg, val);

		/* exit config mode. */
		_arc_sync();
		val = raw_readl(reg);
		val &= ~BIT_MODE_CTRL_CFG;
		raw_writel(reg, val);
	}

	return result;
}
static int32_t can_operation_mode_config(can_t *can, uint32_t mode)
{
	int32_t result = E_OK;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		result = can_opmode_config(can, mode);
	}

	return result;
}
static int32_t can_reset(can_t *can, uint32_t reset)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_MODE_CTRL_OFFSET	+ can->base;
		val = raw_readl(reg);
		if (reset) {
			val |= BIT_MODE_CTRL_RESET;
		} else {
			val &= ~BIT_MODE_CTRL_RESET;
		}
		raw_writel(reg, val);
	}

	return result;
}

static int32_t can_sleep_ack(can_t *can)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_MODE_CTRL_OFFSET	+ can->base;
		val = raw_readl(reg);
		result = (val & BIT_MODE_CTRL_SPACK) ? (1) : (0);
	}

	return result;
}

static int32_t can_test_transmit(can_t *can, uint32_t data)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_MODE_CTRL_OFFSET	+ can->base;
		val = raw_readl(reg);
		val &= ~(BITS_MODE_CTRL_TESTRX_MASK << BITS_MODE_CTRL_TESTRX_SHIFT);
		val |= (data & BITS_MODE_CTRL_TESTRX_MASK) << BITS_MODE_CTRL_TESTRX_SHIFT;
		raw_writel(reg, val);
	}

	return result;
}

static int32_t can_test_receive(can_t *can)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_MODE_CTRL_OFFSET	+ can->base;
		val = raw_readl(reg);
		result = (val | BIT_MODE_CTRL_TESTRX) ? (1) : (0);
	}

	return result;
}

static int32_t can_protocol_config(can_t *can, can_protocol_cfg_t *protocol_cfg)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) || (NULL == protocol_cfg) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_PROTOCOL_CTRL_OFFSET + can->base;

		result = can_opmode_config(can, CONFIG_MODE);
		if (E_OK == result) {
			val = raw_readl(reg);
			if (protocol_cfg->fd_operation) {
				val |= BIT_PROTOCOL_CTRL_FDO;
			} else {
				val &= ~BIT_PROTOCOL_CTRL_FDO;
			}

			if (protocol_cfg->bit_rate_switch) {
				val |= BIT_PROTOCOL_CTRL_BRS;
			} else {
				val &= ~BIT_PROTOCOL_CTRL_BRS;
			}

			if (protocol_cfg->tx_delay_compensate) {
				val |= BIT_PROTOCOL_CTRL_TDC;
			} else {
				val &= ~BIT_PROTOCOL_CTRL_TDC;
			}

			if (protocol_cfg->auto_retransmission) {
				val |= BIT_PROTOCOL_CTRL_ART;
			} else {
				val &= ~BIT_PROTOCOL_CTRL_ART;
			}
			raw_writel(reg, val);
		}
		result = can_opmode_config(can, CONFIG_MODE);
	}

	return result;
}

static int32_t can_timestamp_config(can_t *can, uint32_t prescale, uint32_t mode)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_TIMESTAMP_COUNTER_CFG_OFFSET + can->base;
		val |= (BITS_TIMESTAMP_CFG_TSCP_MASK & prescale) << \
				BITS_TIMESTAMP_CFG_TSCP_SHIFT;
		val |= (BITS_TIMESTAMP_CFG_TSCM_MASK & mode) << \
				BITS_TIMESTAMP_CFG_TSCM_SHIFT;

		result = can_opmode_config(can, CONFIG_MODE);
		if (E_OK == result) {
			raw_writel(reg, val);
		}
		result = can_opmode_config(can, NORMAL_MODE);
	}

	return result;
}

static uint32_t can_timestamp_counter_value(can_t *can)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_TIMESTAMP_COUNTER_VAL_OFFSET + can->base;
		val = raw_readl(reg);
		result = val & 0xFFFFU;
	}

	return result;
}


static int32_t can_baud_rate(can_t *can, uint32_t fd_enabled, can_baud_t *param)
{
	int32_t result = E_OK;

	if ( (NULL == can) || (NULL == param)) {
		result = E_PAR;
	} else {
		uint32_t val = 0;
		uint32_t reg = can->base;

		if (fd_enabled) {
			reg += REG_CAN_DATA_BIT_TIME_CTRL_OFFSET;
			val |= (param->sync_jump_width & BITS_DATA_BIT_TIME_CTRL_DSJW_MASK) << \
					BITS_DATA_BIT_TIME_CTRL_DSJW_SHIFT;
			val |= (param->bit_rate_prescale & BITS_DATA_BIT_TIME_CTRL_DBRP_MASK) << \
					BITS_DATA_BIT_TIME_CTRL_DBRP_SHIFT;
			val |= (param->segment1 & BITS_DATA_BIT_TIME_CTRL_DTSEG1_MASK) << \
					BITS_DATA_BIT_TIME_CTRL_DTSEG1_SHIFT;
			val |= (param->segment2 & BITS_DATA_BIT_TIME_CTRL_DTSEG2_MASK) << \
					BITS_DATA_BIT_TIME_CTRL_DTSEG2_SHIFT;
		} else {
			reg += REG_CAN_NOMINAL_BIT_TIME_CTRL_OFFSET;
			val |= (param->sync_jump_width & BITS_NOMINAL_BIT_TIME_CTRL_NSJW_MASK) << \
					BITS_NOMINAL_BIT_TIME_CTRL_NSJW_SHIFT;
			val |= (param->bit_rate_prescale & BITS_NOMINAL_BIT_TIME_CTRL_NBRP_MASK) << \
					BITS_NOMINAL_BIT_TIME_CTRL_NBRP_SHIFT;
			val |= (param->segment1 & BITS_NOMINAL_BIT_TIME_CTRL_NTSEG1_MASK) << \
			       BITS_NOMINAL_BIT_TIME_CTRL_NTSEG1_SHIFT;
			val |= (param->segment2 & BITS_NOMINAL_BIT_TIME_CTRL_NTSEG2_MASK) << \
					BITS_NOMINAL_BIT_TIME_CTRL_NTSEG2_SHIFT;
		}

		result = can_opmode_config(can, CONFIG_MODE);
		if (E_OK == result) {
			raw_writel(reg, val);
		}
		result = can_opmode_config(can, NORMAL_MODE);
	}

	return result;
}

static int32_t can_error_counter(can_t *can, uint32_t type)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_ERROR_COUNTER_OFFSET + can->base;
		val = raw_readl(reg);

		switch (type) {
			case ERROR_TYPE_LOGGING:
				result = (val >> BITS_ERROR_CTRL_CEL_SHIFT) & \
					 BITS_ERROR_CTRL_CEL_MASK;
				break;
			case ERROR_TYPE_RECEIVE:
				result = (val >> BITS_ERROR_CTRL_REC_SHIFT) & \
					 BITS_ERROR_CTRL_REC_MASK;
				break;
			case ERROR_TYPE_TRANSMIT:
				result = (val >> BITS_ERROR_CTRL_TEC_SHIFT) & \
					BITS_ERROR_CTRL_TEC_MASK;
				break;
			default:
				result = E_PAR;
		}
	}

	return result;
}

static int32_t can_protocol_status(can_t *can, uint32_t type)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_PROTOCOL_STATUS_OFFSET + can->base;
		val = raw_readl(reg);
		switch (type) {
			case PROTOCOL_STS_ALL:
				result = val;
				break;
			case PROTOCOL_STS_LAST_ERR_CODE:
				result = (val >> BITS_PROTOCOL_STATUS_LEC_SHIFT) & \
					 BITS_PROTOCOL_STATUS_LEC_MASK;
				break;
			case PROTOCOL_STS_FD_LAST_RX_ESI:
				result = (val & BIT_PROTOCOL_STATUS_RESI) ? (1) : (0);
				break;
			case PROTOCOL_STS_FD_LAST_RX_BRS:
				result = (val & BIT_PROTOCOL_STATUS_RBRS) ? (1) : (0);
				break;
			case PROTOCOL_STS_FD_RX_MESSAGE:
				result = (val & BIT_PROTOCOL_STATUS_RFDF) ? (1) : (0);
				break;
			case PROTOCOL_STS_EXCEPTION_EVENT:
				result = (val & BIT_PROTOCOL_STATUS_PXE) ? (1) : (0);
				break;
			case PROTOCOL_STS_ACTIVE:
				result = (val >> BITS_PROTOCOL_STATUS_ACT_SHIFT) & \
					 BITS_PROTOCOL_STATUS_ACT_MASK;
				break;
			case PROTOCOL_STS_ERR_PASSIVE:
				result = (val & BIT_PROTOCOL_STATUS_EP) ? (1) : (0);
				break;
			case PROTOCOL_STS_WARNING:
				result = (val & BIT_PROTOCOL_STATUS_EW) ? (1) : (0);
				break;
			case PROTOCOL_STS_BUS_OFF:
				result = (val & BIT_PROTOCOL_STATUS_BO) ? (1) : (0);
				break;
			default:
				result = E_PAR;
		}
	}

	return result;
}

static int32_t can_ecc_error_status(can_t *can)
{
	int32_t result = E_OK;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_ECC_ERROR_STATUS_OFFSET + can->base;
		result = raw_readl(reg);
	}

	return result;
}

static int32_t can_transmitter_delay_compensation(can_t *can, \
		uint32_t value, uint32_t offset, uint32_t win_len)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_TRANSMIT_DELAY_COM_OFFSET + can->base;
		//val = raw_readl(reg);
		val |= (value & BITS_TRANSMIT_DELAY_COM_TDCV_MASK) << BITS_TRANSMIT_DELAY_COM_TDCV_SHIFT;
		val |= (offset & BITS_TRANSMIT_DELAY_COM_TDCO_MASK) << BITS_TRANSMIT_DELAY_COM_TDCO_SHIFT;
		val |= (win_len & BITS_TRANSMIT_DELAY_COM_TDCF_MASK) << BITS_TRANSMIT_DELAY_COM_TDCF_SHIFT;

		result = can_opmode_config(can, CONFIG_MODE);
		if (E_OK == result) {
			raw_writel(reg, val);
		}
		result = can_opmode_config(can, NORMAL_MODE);
	}

	return result;
}

static int32_t can_timeout_config(can_t *can, uint32_t enable, uint32_t period, uint32_t select)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_TIMEOUT_COUNTER_OFFSET + can->base;
		val |= (period & BITS_TIMEOUT_COUNTER_TOP_MASK) << BITS_TIMEOUT_COUNTER_TOP_SHIFT;
		val |= (select & BITS_TIMEOUT_COUNTER_TOS_MASK) << BITS_TIMEOUT_COUNTER_TOS_SHIFT;
		if (enable) {
			val |= BIT_TIMEOUT_COUNTER_TOE;
		} else {
			val &= ~BIT_TIMEOUT_COUNTER_TOE;
		}
		raw_writel(reg, val);
	}

	return result;
}

static int32_t can_interrupt_status(can_t *can, uint32_t mask)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_INTERRUPT_OFFSET + can->base;
		val = raw_readl(reg);
		result = val & mask;
	}

	return result;
}

static int32_t can_interrupt_clear(can_t *can, uint32_t mask)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_INTERRUPT_OFFSET + can->base;

		val = raw_readl(reg);
		val |= mask;
		raw_writel(reg, val);
	}

	return result;
}

static int32_t can_interrupt_enable(can_t *can, uint32_t enable, uint32_t mask)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_INTERRUPT_ENABLE_OFFSET + can->base;
		val = raw_readl(reg);
		if (enable) {
			val |= mask;
		}  else {
			val &= ~mask;
		}
		raw_writel(reg, val);
	}

	return result;
}

static int32_t can_interrupt_line_select(can_t *can, uint32_t int_source, uint32_t int_line)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t sel0 = REG_CAN_INTERRUPT_LINE_SELECT0_OFFSET + can->base;
		uint32_t sel1 = REG_CAN_INTERRUPT_LINE_SELECT1_OFFSET + can->base;
		switch (int_source) {
			case CAN_INT_TIMEOUT_OCCURED:
				val = raw_readl(sel0);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_TOOLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_TOOLS_SHIFT;
				raw_writel(sel0, val);
				break;
			case CAN_INT_ACCESS_PROTECT_REG:
				val = raw_readl(sel0);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_APRLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_APRLS_SHIFT;
				raw_writel(sel0, val);
				break;
			case CAN_INT_PROTOCOL_ERR:
				val = raw_readl(sel0);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_PEDLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_PEDLS_SHIFT;
				raw_writel(sel0, val);
				break;
			case CAN_INT_BUS_OFF:
				val = raw_readl(sel0);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_BOLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_BOLS_SHIFT;
				raw_writel(sel0, val);
				break;
			case CAN_INT_WARNING:
				val = raw_readl(sel0);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_EWLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_EWLS_SHIFT;
				raw_writel(sel0, val);
				break;
			case CAN_INT_ERR_PASSIVE:
				val = raw_readl(sel0);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_EPLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_EPLS_SHIFT;
				raw_writel(sel0, val);
				break;
			case CAN_INT_ERR_LOGGING_OVERFLOW:
				val = raw_readl(sel0);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_ELOLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_ELOLS_SHIFT;
				raw_writel(sel0, val);
				break;
			case CAN_INT_BIT_ERR_UNCORRECTED:
				val = raw_readl(sel0);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_BEULS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_BEULS_SHIFT;
				raw_writel(sel0, val);
				break;
			case CAN_INT_BIT_ERR_CORRECTED:
				val = raw_readl(sel0);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_BECLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_BECLS_SHIFT;
				raw_writel(sel0, val);
				break;
			case CAN_INT_RAM_ACCESS_FAIL:
				val = raw_readl(sel0);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_MRAFLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_MRAFLS_SHIFT;
				raw_writel(sel0, val);
				break;
			case CAN_INT_TIMESTAMP_WRAP_AROUND:
				val = raw_readl(sel0);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_TSWLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_TSWLS_SHIFT;
				raw_writel(sel0, val);
				break;

			case CAN_INT_RX_NEW_MESSAGE:
				val = raw_readl(sel1);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_RXBNLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_RXBNLS_SHIFT;
				raw_writel(sel1, val);
				break;
			case CAN_INT_RX_FIFO_LOST:
				val = raw_readl(sel1);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_RXFLLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_RXFLLS_SHIFT;
				raw_writel(sel1, val);
				break;
			case CAN_INT_RX_FIFO_REAL_FULL:
				val = raw_readl(sel1);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_RXFFLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_RXFFLS_SHIFT;
				raw_writel(sel1, val);
				break;
			case CAN_INT_RX_FIFO_FULL:
				val = raw_readl(sel1);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_RXFWLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_RXFWLS_SHIFT;
				raw_writel(sel1, val);
				break;
			case CAN_INT_RX_FIFO_EMPTY:
				val = raw_readl(sel1);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_RXFELS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_RXFELS_SHIFT;
				raw_writel(sel1, val);
				break;
			case CAN_INT_TX_CANCEL_FINISHED:
				val = raw_readl(sel1);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_TXBCLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_TXBCLS_SHIFT;
				raw_writel(sel1, val);
				break;
			case CAN_INT_TX_COMPLISHED:
				val = raw_readl(sel1);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_TXBTLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_TXBTLS_SHIFT;
				raw_writel(sel1, val);
				break;
			case CAN_INT_TX_FIFO_LOST:
				val = raw_readl(sel1);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_TXFLLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_TXFLLS_SHIFT;
				raw_writel(sel1, val);
				break;
			case CAN_INT_TX_FIFO_REAL_FULL:
				val = raw_readl(sel1);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_TXFFLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_TXFFLS_SHIFT;
				raw_writel(sel1, val);
				break;
			case CAN_INT_TX_FIFO_FULL:
				val = raw_readl(sel1);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_TXFWLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_TXFWLS_SHIFT;
				raw_writel(sel1, val);
				break;
			case CAN_INT_TX_FIFO_EMPTY:
				val = raw_readl(sel1);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_TXFELS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_TXFELS_SHIFT;
				raw_writel(sel1, val);
				break;
			case CAN_INT_TX_EVENT_FIFO_LOST:
				val = raw_readl(sel1);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_TEFLLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_TEFLLS_SHIFT;
				raw_writel(sel1, val);
				break;
			case CAN_INT_TX_EVENT_FIFO_REAL_FULL:
				val = raw_readl(sel1);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_TEFFLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_TEFFLS_SHIFT;
				raw_writel(sel1, val);
				break;
			case CAN_INT_TX_EVENT_FIFO_FULL:
				val = raw_readl(sel1);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_TEFWLS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_TEFWLS_SHIFT;
				raw_writel(sel1, val);
				break;
			case CAN_INT_TX_EVENT_FIFO_EMPTY:
				val = raw_readl(sel1);
				val |= (int_line & BITS_INTERRUPT_LINE_SELECT_TEFELS_MASK) << \
				       BITS_INTERRUPT_LINE_SELECT_TEFELS_SHIFT;
				raw_writel(sel1, val);
				break;
			default:
				result = E_PAR;
		}
	}

	return result;
}

static int32_t can_interrupt_line_enable(can_t *can, uint32_t int_line, uint32_t enable)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_INTERRUPT_LINE_ENABLE_OFFSET + can->base;
		val = raw_readl(reg);
		if (enable) {
			val |= 1 << (int_line & 0x3);
		} else {
			val &= ~( 1 << (int_line & 0x3));
		}
		raw_writel(reg, val);
	}

	return result;
}

static int32_t can_id_filter_standard_element(can_t *can, uint32_t index, can_id_filter_element_t *ele_cfg)
{
	int32_t result = E_OK;
	uint32_t sub_element = 0;
	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		sub_element |= ((ele_cfg->filter_type & BITS_STD_ID_FILTER_ELEMENT_SFT_MSASK) << \
						BITS_STD_ID_FILTER_ELEMENT_SFT_SHIFT);
		sub_element |= ((ele_cfg->filter_cfg & BITS_STD_ID_FILTER_ELEMENT_SFEC_MASK) << \
						BITS_STD_ID_FILTER_ELEMENT_SFEC_SHIT);
		sub_element |= ((ele_cfg->filter_id0 & BITS_STD_ID_FILTER_ELEMENT_SFID0_MASK) << \
						BITS_STD_ID_FILTER_ELEMENT_SFID0_SHIFT);
		sub_element |= ((ele_cfg->filter_id1 & BITS_STD_ID_FILTER_ELEMENT_SFID1_MASK) << \
						BITS_STD_ID_FILTER_ELEMENT_SFID1_SHIFT);
		uint32_t reg = can->base + REG_CAN_STD_ID_FILTER_OFFSET + (index << 2);
		raw_writel(reg, sub_element);
	}
	return result;
}
static int32_t can_id_filter_extended_element(can_t *can, uint32_t index, can_id_filter_element_t *ele_cfg)
{
	int32_t result = E_OK;
	uint32_t element_f0 = 0, element_f1 = 0;
	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = can->base + REG_CAN_EXT_ID_FILTER_OFFSET + (index << 3);
		element_f0 |= ((ele_cfg->filter_type & BITS_EXT_ID_FILTER_ELEMENT_EFT_MASK) << \
						BITS_EXT_ID_FILTER_ELEMENT_EFT_SHIFT);
		element_f0 |= ((ele_cfg->filter_id0 & BITS_EXT_ID_FILTER_ELEMENT_EFID0_MASK) << \
						BITS_EXT_ID_FILTER_ELEMENT_EFID0_SHIFT);
		element_f1 |= ((ele_cfg->filter_cfg & BITS_EXT_ID_FILTER_ELEMENT_EFEC_MASK) << \
						BITS_EXT_ID_FILTER_ELEMENT_EFEC_SHIFT);
		element_f1 |= ((ele_cfg->filter_id1 & BITS_EXT_ID_FILTER_ELEMENT_EFID1_MASK) << \
						BITS_EXT_ID_FILTER_ELEMENT_EFID1_SHIFT);

		raw_writel(reg, element_f0);
		reg += REG_CAN_EXT_ID_FILTER_ELEMENT_F1_OFFSET;
		raw_writel(reg, element_f1);
	}
	return result;
}
static int32_t can_id_filter_element_config(can_t *can, uint32_t index, can_id_filter_element_t *ele_cfg)
{
	int32_t result = E_OK;
	result = can_opmode_config(can, CONFIG_MODE);
	if ( (NULL == can) || (NULL == ele_cfg) || (E_OK != result)) {
		result = E_PAR;
	} else {
		if (ele_cfg->frame_format == eSTANDARD_FRAME) {
			result = can_id_filter_standard_element(can, index, ele_cfg);
		} else {
			result = can_id_filter_extended_element(can, index, ele_cfg);
		}
	}
	if (E_OK == result) {
		result = can_opmode_config(can, NORMAL_MODE);
	}
	return result;
}
static int32_t can_id_filter_standard_frame(can_t *can, can_id_filter_param_t *param)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_ID_FILTER_CTRL_OFFSET + can->base;

		val = raw_readl(reg);
		if (param->reject_no_match) {
			val |= BIT_ID_FILTER_CTRL_RNMFS;
		} else {
			val &= ~BIT_ID_FILTER_CTRL_RNMFS;
		}
		if (param->reject_remote == eEXTENDED_FRAME) {
			val |= BIT_ID_FILTER_CTRL_RRFE;
		} else {
			val &= ~BIT_ID_FILTER_CTRL_RRFE;
		}
		val |= (param->filter_size & BITS_ID_FILTER_CTRL_SIDFS_MASK);
		raw_writel(reg, val);
	}

	return result;
}

static int32_t can_id_filter_extended_frame(can_t *can, can_id_filter_param_t *param)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_ID_FILTER_CTRL_OFFSET + can->base;

		val = raw_readl(reg);
		if (param->reject_no_match) {
			val |= BIT_ID_FILTER_CTRL_RNMFE;
		} else {
			val &= ~BIT_ID_FILTER_CTRL_RNMFE;
		}
		if (param->reject_remote == eSTANDARD_FRAME) {
			val |= BIT_ID_FILTER_CTRL_RRFS;
		} else {
			val &= ~BIT_ID_FILTER_CTRL_RRFS;
		}
		val |= (param->filter_size & BITS_ID_FILTER_CTRL_XIDFS_MASK) << \
				BITS_ID_FILTER_CTRL_XIDFS_SHIFT;
		raw_writel(reg, val);
	}

	return result;
}


static int32_t can_extended_id_and_mask(can_t *can, uint32_t mask)
{
	int32_t result = E_OK;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_EXTEND_ID_AND_MASK_OFFSET + can->base;
		raw_writel(reg, mask);
	}

	return result;
}

static int32_t can_id_filter_config(can_t *can, can_id_filter_param_t *param)
{
	int32_t result = E_OK;

	result = can_opmode_config(can, CONFIG_MODE);
	if ( (NULL == can) || (NULL == param) || (E_OK != result)) {
		result = E_PAR;
	} else {
		if (param->frame_format == eSTANDARD_FRAME) {
			result = can_id_filter_standard_frame(can, param);
		} else {
			result = can_id_filter_extended_frame(can, param);
			if (E_OK == result) {
				result = can_extended_id_and_mask(can, param->mask);
			}
		}
	}
	if (E_OK == result) {
		result = can_opmode_config(can, NORMAL_MODE);
	}

	return result;
}

static int32_t can_rx_data_field_size(can_t *can, uint32_t fifo_dfs, uint32_t buf_dfs)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_RX_ELEMENT_SIZE_CFG_OFFSET + can->base;
		val |= (fifo_dfs & BITS_RX_ELEMENT_SIZE_CFG_RXFDS_MASK) << \
				BITS_RX_ELEMENT_SIZE_CFG_RXFDS_SHIFT;
		val |= ( buf_dfs & BITS_RX_ELEMENT_SIZE_CFG_RXBDS_MASK) << \
				BITS_RX_ELEMENT_SIZE_CFG_RXBDS_SHIFT;
		raw_writel(reg, val);
	}

	return result;
}

static int32_t can_tx_data_field_size(can_t *can, uint32_t fifo_dfs, uint32_t buf_dfs)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_TX_ELEMENT_SIZE_CFG_OFFSET + can->base;
		val |= (fifo_dfs & BITS_TX_ELEMENT_SIZE_CFG_TXFDS_MASK) << \
				BITS_TX_ELEMENT_SIZE_CFG_TXFDS_SHIFT;
		val |= (buf_dfs & BITS_TX_ELEMENT_SIZE_CFG_TXBDS_MASK) << \
				BITS_TX_ELEMENT_SIZE_CFG_TXBDS_SHIFT;
		raw_writel(reg, val);
	}

	return result;
}

static int32_t can_data_field_size(can_t *can, uint32_t rx_or_tx, uint32_t fifo_dfs, uint32_t buf_dfs)
{
	int32_t result = E_OK;

	result = can_opmode_config(can, CONFIG_MODE);
	if (E_OK == result) {
		if (rx_or_tx) {
			result = can_tx_data_field_size(can, fifo_dfs, buf_dfs);
		} else {
			result = can_rx_data_field_size(can, fifo_dfs, buf_dfs);
		}
	}
	result = can_opmode_config(can, NORMAL_MODE);

	return result;
}

static int32_t can_rx_buffer_status(can_t *can, uint32_t buf_id)
{
	int32_t result = E_OK;

	if ( (NULL == can) || (buf_id > 63)) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_RX_BUFFER_STATUS0_OFFSET + can->base;
		if (buf_id < 32) {
			if (raw_readl(reg) & (1 << buf_id)) {
				result = 1;
			}
		} else {
			reg = REG_CAN_RX_BUFFER_STATUS1_OFFSET + can->base;
			if (raw_readl(reg) & (1 << (buf_id - 32))) {
				result = 1;
			}
		}
	}

	return result;
}

static int32_t can_rx_buffer_clear(can_t *can, uint32_t buf_id)
{
	int32_t result = E_OK;
	uint32_t reg, val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		if (buf_id < 32) {
			reg = REG_CAN_RX_BUFFER_STATUS0_OFFSET + can->base;
		} else if (buf_id < 64) {
			reg = REG_CAN_RX_BUFFER_STATUS1_OFFSET + can->base;
			buf_id -= 32;
		} else {
			result = E_PAR;
		}
	}

	if (E_OK == result) {
		val |= (1 << buf_id);
		raw_writel(reg, val);
	}

	return result;
}


static int32_t can_fifo_status(can_t *can, uint32_t fifo_type, uint32_t fifo_sts)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg;
		switch (fifo_type) {
			case CAN_RX_FIFO:
				reg = REG_CAN_RX_FIFO_STATUS_OFFSET + can->base;
				break;
			case CAN_TX_FIFO:
				reg = REG_CAN_TX_FIFO_STATUS_OFFSET + can->base;
				break;
			case CAN_TX_EVENT_FIFO:
				reg = REG_CAN_TX_EVENT_FIFO_STATUS_OFFSET + can->base;
				break;
			default:
				result = E_PAR;
		}

		if (E_OK != result) {
			goto err_handle;
		}

		val = raw_readl(reg);
		switch (fifo_sts) {
			case CAN_FIFO_ALL:
				result = val;
				break;
			case CAN_FIFO_PUT_INDEX:
				result = (val >> CAN_FIFO_STATUS_FPI_SHIFT) & \
					 CAN_FIFO_STATUS_FPI_MASK;
				break;
			case CAN_FIFO_GET_INDEX:
				result = (val >> CAN_FIFO_STATUS_FGI_SHIFT) & \
					 CAN_FIFO_STATUS_FGI_MASK;
				break;
			case CAN_FIFO_LEVEL:
				result = (val >> CAN_FIFO_STATUS_FLV_SHIFT) & \
					CAN_FIFO_STATUS_FLV_MASK;
				break;
			case CAN_FIFO_MESSAGE_LOST:
				result = (val & CAN_FIFO_STATUS_FML) ? (1) : (0);
				break;
			case CAN_FIFO_FULL:
				result = (val & CAN_FIFO_STATUS_FF) ? (1) : (0);
				break;
			case CAN_FIFO_EMPTY:
				result = (val & CAN_FIFO_STATUS_FE) ? (1) : (0);
				break;
			case CAN_FIFO_ACK:
				result = (val & CAN_FIFO_STATUS_FACK) ? (1) : (0);
				break;
			default:
				result = E_PAR;
		}
	}

err_handle:
	return result;
}


static int32_t can_tx_buffer_request_pending(can_t *can, uint32_t buf_id)
{
	int32_t result = E_OK;

	if ((NULL == can) || (buf_id > 63)) {
		result = E_PAR;
	} else {
		uint32_t reg;
		if (buf_id < 32) {
			reg = REG_CAN_TX_BUF_REQ_PENDING0_OFFSET + can->base;
		} else {
			reg = REG_CAN_TX_BUF_REQ_PENDING1_OFFSET + can->base;
			buf_id -= 32;
		}

		if (E_OK == result) {
			result = (raw_readl(reg) & (1 << buf_id)) ? (1) : (0);
		}
	}

	return result;
}

static int32_t can_tx_buffer_add_request(can_t *can, uint32_t buf_id)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg;
		if (buf_id < 32) {
			reg = REG_CAN_TX_BUF_ADD_REQ0_OFFSET + can->base;
		} else if (buf_id < 64) {
			reg = REG_CAN_TX_BUF_ADD_REQ1_OFFSET + can->base;
			buf_id -= 32;
		} else {
			result = E_PAR;
		}

		if (E_OK == result) {
			val = raw_readl(reg);
			val |= (1 << buf_id);
			raw_writel(reg, val);
		}
	}

	return result;
}

static int32_t can_tx_buffer_cancel_request(can_t *can, uint32_t buf_id)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg;
		if (buf_id < 32) {
			reg = REG_CAN_TX_BUF_CANCEL_REQ0_OFFSET + can->base;
		} else if (buf_id < 64) {
			reg = REG_CAN_TX_BUF_CANCEL_REQ1_OFFSET + can->base;
			buf_id -= 32;
		} else {
			result = E_PAR;
		}

		if (E_OK == result) {
			val = raw_readl(reg);
			val |= (1 << buf_id);
			raw_writel(reg, val);
		}
	}

	return result;
}

static int32_t can_tx_buffer_transmission_occurred(can_t *can, uint32_t buf_id)
{
	int32_t result = E_OK;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg;
		if (buf_id < 32) {
			reg = REG_CAN_TX_BUF_TRANS_OCCURRED0_OFFSET + can->base;
		} else if (buf_id < 64) {
			reg = REG_CAN_TX_BUF_TRANS_OCCURRED1_OFFSET + can->base;
			buf_id -= 32;
		} else {
			result = E_PAR;
		}

		if (E_OK == result) {
			result = (raw_readl(reg) & (1 << buf_id)) ? (1) : (0);
		}
	}

	return result;
}

static int32_t can_tx_buffer_cancel_finished(can_t *can, uint32_t buf_id)
{
	int32_t result = E_OK;
	//uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg;
		if (buf_id < 32) {
			reg = REG_CAN_TX_BUF_CANCEL_FINISHED0_OFFSET + can->base;
		} else if (buf_id < 64) {
			reg = REG_CAN_TX_BUF_CANCEL_FINISHED1_OFFSET + can->base;
			buf_id -= 32;
		} else {
			result = E_PAR;
		}

		if (E_OK == result) {
			result = (raw_readl(reg) & (1 << buf_id)) ? (1) : (0);
		}
	}

	return result;
}

static int32_t can_tx_buffer_transmission_int_enable(can_t *can, uint32_t buf_id, uint32_t enable)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg;
		if (buf_id < 32) {
			reg = REG_CAN_TX_BUF_TRANS_INT_EN0_OFFSET + can->base;
		} else if (buf_id < 64) {
			reg = REG_CAN_TX_BUF_TRANS_INT_EN1_OFFSET + can->base;
			buf_id -= 32;
		} else {
			result = E_PAR;
		}

		if (E_OK == result) {
			val = raw_readl(reg);
			if (enable) {
				val |= (1 << buf_id);
			} else {
				val &= ~(1 << buf_id);
			}
			raw_writel(reg, val);
		}
	}

	return result;
}

static int32_t can_tx_buffer_cancel_trans_int_enable(can_t *can, uint32_t buf_id, uint32_t enable)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg;
		if (buf_id < 32) {
			reg = REG_CAN_TX_BUF_CANCELED_INT_EN0_OFFSET + can->base;
		} else if (buf_id < 64) {
			reg = REG_CAN_TX_BUF_CANCELED_INT_EN1_OFFSET + can->base;
			buf_id -= 32;
		} else {
			result = E_PAR;
		}

		if (E_OK == result) {
			val = raw_readl(reg);
			if (enable) {
				val |= (1 << buf_id);
			} else {
				val &= ~(1 << buf_id);
			}
			raw_writel(reg, val);
		}
	}

	return result;
}

static int32_t can_rx_fifo_config(can_t *can, uint32_t watermark, uint32_t size, uint32_t mode)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_RX_FIFO_CFG_OFFSET + can->base;
		val |= (watermark & BITS_RX_FIFO_CFG_RXFWM_MASK) << \
				BITS_RX_FIFO_CFG_RXFWM_SHIFT;
		val |= (size & BITS_RX_FIFO_CFG_RXFSZ_MASK) << \
				BITS_RX_FIFO_CFG_RXFSZ_SHIFT;
		if (FIFO_OVERWRITE_MODE == mode) {
			val |= BIT_RX_FIFO_CFG_RXFOPM;
		} else {
			val &= ~BIT_RX_FIFO_CFG_RXFOPM;
		}
		raw_writel(reg, val);
	}

	return result;
}

static int32_t can_tx_fifo_config(can_t *can, uint32_t watermark, uint32_t size, uint32_t mode)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_TX_FIFO_CFG_OFFSET + can->base;
		val |= (watermark & BITS_TX_FIFO_CFG_TXFWM_MASK) << \
				BITS_TX_FIFO_CFG_TXFWM_SHIFT;
		val |= (size & BITS_TX_FIFO_CFG_TXFSZ_MASK) << \
				BITS_TX_FIFO_CFG_TXFSZ_SHIFT;
		if (FIFO_OVERWRITE_MODE == mode) {
			val |= BIT_TX_FIFO_CFG_TXFOPM;
		} else {
			val &= ~BIT_TX_FIFO_CFG_TXFOPM;
		}
		raw_writel(reg, val);
	}

	return result;
}

static int32_t can_tx_event_fifo_config(can_t *can, uint32_t watermark, uint32_t size, uint32_t mode)
{
	int32_t result = E_OK;
	uint32_t val = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		uint32_t reg = REG_CAN_TX_EVENT_FIFO_CFG_OFFSET + can->base;
		val |= (watermark & BITS_TX_EVENT_FIFO_CFG_TEFWM_MASK) << \
				BITS_TX_EVENT_FIFO_CFG_TEFWM_SHIFT;
		val |= (size & BITS_TX_EVENT_FIFO_CFG_TEFSZ_MASK) << \
				BITS_TX_EVENT_FIFO_CFG_TEFSZ_SHIFT;
		if (FIFO_OVERWRITE_MODE == mode) {
			val |= BIT_TX_EVENT_FIFO_CFG_TEFOPM;
		} else {
			val &= ~BIT_TX_EVENT_FIFO_CFG_TEFOPM;
		}
		raw_writel(reg, val);
	}

	return result;
}

static int32_t can_fifo_config(can_t *can, uint32_t fifo_type, can_fifo_param_t *param)
{
	int32_t result = E_OK;

	if ((NULL == can) || (NULL == param)) {
		result = E_PAR;
	} else {
		switch (fifo_type) {
			case CAN_RX_FIFO:
				result = can_rx_fifo_config(can, \
					param->watermark, param->size, param->mode);
				break;
			case CAN_TX_FIFO:
				result = can_tx_fifo_config(can, \
					param->watermark, param->size, param->mode);
				break;
			case CAN_TX_EVENT_FIFO:
				result = can_tx_event_fifo_config(can, \
					param->watermark, param->size, param->mode);
				break;
			default:
				result = E_PAR;
		}
	}

	return result;
}

static int32_t can_write_single_frame(can_t *can, uint32_t buf_id, uint32_t *buf, uint32_t frame_size)
{
	int32_t result = E_OK;

	uint32_t status = 0;
	uint32_t status_pos = buf_id % 32;

	uint32_t reg_addr = 0;
	uint32_t buf_pos = REG_CAN_TX_BUFFER_OFFSET + can->base;

	if ((NULL == can) || (buf_id > 63) || (NULL == buf) || (0 == frame_size)) {
		result = E_PAR;
	} else {
		if (buf_id < 32) {
			reg_addr = REG_CAN_TX_BUF_REQ_PENDING0_OFFSET + can->base;
		} else {
			reg_addr = REG_CAN_TX_BUF_REQ_PENDING1_OFFSET + can->base;
		}
		status = raw_readl(reg_addr);

		if (status & (1 << status_pos)) {
			result = E_DBUSY;
			return result;
		}

		buf_pos += (buf_id * frame_size) << 2;
		while (frame_size--) {
			raw_writel(buf_pos, *buf++);
			buf_pos += 4;
		}

		if (buf_id < 32) {
			reg_addr = REG_CAN_TX_BUF_ADD_REQ0_OFFSET + can->base;
		} else {
			reg_addr = REG_CAN_TX_BUF_ADD_REQ1_OFFSET + can->base;
		}

		status = (1 << status_pos);
		raw_writel(reg_addr, status);

	}

	return result;
}

static int32_t can_read_single_frame(can_t *can, uint32_t buf_id, uint32_t *buf, uint32_t frame_size)
{
	int32_t result = E_OK;

	uint32_t status = 0;
	uint32_t status_pos = buf_id % 32;

	uint32_t reg_addr = 0;
	uint32_t buf_pos = REG_CAN_RX_BUFFER_OFFSET + can->base;

	if ((NULL == can) || (buf_id > 63) || (NULL == buf) || (0 == frame_size)) {
		result = E_PAR;
	} else {
		if (buf_id < 32) {
			reg_addr = REG_CAN_RX_BUFFER_STATUS0_OFFSET + can->base;
		} else {
			reg_addr = REG_CAN_RX_BUFFER_STATUS1_OFFSET + can->base;
		}

		/* Read RX Buffer Status Register (RXBS0R/RXBS1R) */
		status = raw_readl(reg_addr);

		if (status & (1 << status_pos)) {
			/* Found Data in current Buffer location
			 * (FW buf_id point to this position).
			 */
			buf_pos += (buf_id * frame_size) << 2;
			while (frame_size--) {
				*buf++ = raw_readl(buf_pos);
				buf_pos += 4;
			}

			/* Clear corresponding bit in RX Status Regster (RXBS0R/RXBS1R),
			 * to tell HW that FW have already fetched frame in corresponding buffer location.
			 */
			raw_writel(reg_addr, (1 << status_pos));
		} else {
			/* Currently, Buffer have no data.
			   SO, No need to clear corresponding bit in RX Status Register (RXBS0R/RXBS1R).
			*/
			result = E_NODATA;
		}
	}

	return result;
}

static int32_t can_read_single_frame_blocked(can_t *can, uint32_t buf_id, uint32_t *buf, uint32_t frame_size)
{
	int32_t result = E_OK;

	uint32_t status_pos = buf_id % 32;

	uint32_t reg_addr = 0;
	uint32_t buf_pos = REG_CAN_RX_BUFFER_OFFSET + can->base;

	if ((NULL == can) || (buf_id > 63) || (NULL == buf) || (0 == frame_size)) {
		result = E_PAR;
	} else {
		if (buf_id < 32) {
			reg_addr = REG_CAN_RX_BUFFER_STATUS0_OFFSET + can->base;
		} else {
			reg_addr = REG_CAN_RX_BUFFER_STATUS1_OFFSET + can->base;
		}
		while(0 == (raw_readl(reg_addr) & (1 << status_pos)));

		buf_pos += (buf_id * frame_size) << 2;
		while (frame_size--) {
			*buf++ = raw_readl(buf_pos);
			buf_pos += 4;
		}

		raw_writel(reg_addr, (1 << status_pos));
	}

	return result;
}

static int32_t can_fifo_read(can_t *can, uint32_t fifo_type, uint32_t *buf, uint32_t len)
{
	int32_t result = E_OK;
	uint32_t reg = 0;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {

		switch (fifo_type) {
			case CAN_RX_FIFO:
				reg = REG_CAN_RX_FIFO_ELEMENT_OFFSET(0) + can->base;
				break;
			case CAN_TX_FIFO:
				reg = REG_CAN_TX_FIFO_ELEMENT_OFFSET(0) + can->base;
				break;
			case CAN_TX_EVENT_FIFO:
				reg = REG_CAN_TX_EV_FIFO_ELEMENT_OFFSET(0) + can->base;
				break;
			default:
				result = E_PAR;
		}
	}

	if (E_OK != result) {
		while (len--) {
			*buf++ = raw_readl(reg);
			reg += 4;
		}
	}

	return result;
}

static can_ops_t basic_can_ops = {
	.dma_enable = can_dma_enable,
	.ecc_enable = can_ecc_enable,
	.reset = can_reset,

	.operation_mode = can_operation_mode_config,

	.tx_delay_compensation = can_transmitter_delay_compensation,
	.protocol_config = can_protocol_config,
	.timestamp_config = can_timestamp_config,
	.timeout_config = can_timeout_config,
	.baud = can_baud_rate,
	.id_filter_config = can_id_filter_config,
	.data_field_size = can_data_field_size,
	.fifo_config = can_fifo_config,

	.rx_buf_status = can_rx_buffer_status,
	.rx_buf_clear = can_rx_buffer_clear,

	.tx_buf_request_pending = can_tx_buffer_request_pending,
	.tx_buf_add_request = can_tx_buffer_add_request,
	.tx_buf_cancel_request = can_tx_buffer_cancel_request,
	.tx_buf_transmission_occurred = can_tx_buffer_transmission_occurred,
	.tx_buf_cancel_finished = can_tx_buffer_cancel_finished,
	.tx_buf_transmission_int_enable = can_tx_buffer_transmission_int_enable,
	.tx_buf_cancel_trans_int_enable = can_tx_buffer_cancel_trans_int_enable,

	.fifo_status = can_fifo_status,
	.fifo_read = can_fifo_read,

	.sleep_ack = can_sleep_ack,
	.timestamp_value = can_timestamp_counter_value,
	.error_counter = can_error_counter,
	.protocol_status = can_protocol_status,
	.ecc_error_status = can_ecc_error_status,

	.int_status = can_interrupt_status,
	.int_clear = can_interrupt_clear,
	.int_enable = can_interrupt_enable,
	.int_line_select = can_interrupt_line_select,
	.int_line_enable = can_interrupt_line_enable,

	.test_transmit = can_test_transmit,
	.test_receive = can_test_receive,
	.write_frame = can_write_single_frame,
	.read_frame = can_read_single_frame,
	.read_frame_blocked = can_read_single_frame_blocked,

	.version = can_version,
	.id_filter_element = can_id_filter_element_config
};

int32_t can_install_ops(can_t *can)
{
	int32_t result = E_OK;

	if ( (NULL == can) ) {
		result = E_PAR;
	} else {
		can->ops = (void *)&basic_can_ops;
	}

	return result;
}
