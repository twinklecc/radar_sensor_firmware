#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC_debug.h"
#include "arc_exception.h"
#include "dev_common.h"
#include "dw_ssi_reg.h"
#include "dw_ssi.h"

static void dw_ssi_data_frame_config(dw_ssi_t *dw_ssi, uint32_t dfs, \
		uint32_t ins_len, uint32_t addr_len, uint32_t wait_cycle)
{
	uint32_t value = raw_readl(REG_DW_SSI_CTRLR0_OFFSET + dw_ssi->base);
	value &= ~(BITS_DW_SSI_CTRLR0_DFS32_MASK << BITS_DW_SSI_CTRLR0_DFS32_SHIFT);
	value |= (dfs & BITS_DW_SSI_CTRLR0_DFS32_MASK) << BITS_DW_SSI_CTRLR0_DFS32_SHIFT;
	raw_writel(REG_DW_SSI_CTRLR0_OFFSET + dw_ssi->base, value);

	value = raw_readl(REG_DW_SSI_SPI_CTRLR0_OFFSET + dw_ssi->base);
	value &= ~(BIT_DW_SSI_SPI_CTRLR0_WAIT_CYCLES_MASK << BIT_DW_SSI_SPI_CTRLR0_WAIT_CYCLES_SHIFT);
	value &= ~(BIT_DW_SSI_SPI_CTRLR0_INS_L_MASK << BIT_DW_SSI_SPI_CTRLR0_INS_L_SHIFT);
	value &= ~(BIT_DW_SSI_SPI_CTRLR0_ADDR_L_MASK << BIT_DW_SSI_SPI_CTRLR0_ADDR_L_SHIFT);

	value |= (wait_cycle & BIT_DW_SSI_SPI_CTRLR0_WAIT_CYCLES_MASK) << BIT_DW_SSI_SPI_CTRLR0_WAIT_CYCLES_SHIFT;
	value |= (ins_len & BIT_DW_SSI_SPI_CTRLR0_INS_L_MASK) << BIT_DW_SSI_SPI_CTRLR0_INS_L_SHIFT;
	value |= (addr_len & BIT_DW_SSI_SPI_CTRLR0_ADDR_L_MASK) << BIT_DW_SSI_SPI_CTRLR0_ADDR_L_SHIFT;
	raw_writel(REG_DW_SSI_SPI_CTRLR0_OFFSET + dw_ssi->base, value);
}

static int32_t dw_ssi_control(dw_ssi_t *dw_ssi, dw_ssi_ctrl_t *ctrl)
{
	int32_t result = E_OK;

	uint32_t value = 0;

	if ((NULL == dw_ssi) || (NULL == ctrl)) {
		result = E_PAR;
	} else {
		if (ctrl->slv_sel_toggle_en) {
			value |= BIT_DW_SSI_CTRLR0_SSTE;
		}

		value |= (ctrl->frame_format & BITS_DW_SSI_CTRLR0_SPI_FRF_MASK) << \
			BITS_DW_SSI_CTRLR0_SPI_FRF_SHIFT;

		value |= (((ctrl->data_frame_size - 1) & BITS_DW_SSI_CTRLR0_DFS32_MASK) << \
			BITS_DW_SSI_CTRLR0_DFS32_SHIFT);

		value |= ((ctrl->ctrl_frame_size & BITS_DW_SSI_CTRLR0_CFS_MASK) << \
			BITS_DW_SSI_CTRLR0_CFS_SHIFT);

		if (ctrl->slv_output_en) {
			value |= BIT_DW_SSI_CTRLR0_SLV_OE;
		}

		value |= (ctrl->transfer_mode & BITS_DW_SSI_CTRLR0_TMOD_MASK) << \
			BITS_DW_SSI_CTRLR0_TMOD_SHIFT;

		if (ctrl->serial_clk_pol) {
			value |= BIT_DW_SSI_CTRLR0_SCPOL;
		}

		if (ctrl->serial_clk_phase) {
			value |= BIT_DW_SSI_CTRLR0_SCPH;
		}

		value |= (ctrl->serial_protocol & BITS_DW_SSI_CTRLR0_FRF_MASK) << \
			BITS_DW_SSI_CTRLR0_FRF_SHIFT;
		raw_writel(REG_DW_SSI_CTRLR0_OFFSET + dw_ssi->base, value);
	}

	return result;
}

static inline void dw_ssi_transfer_mode_update(dw_ssi_t *dw_ssi, uint32_t transfer_mode)
{
	uint32_t value = raw_readl(REG_DW_SSI_CTRLR0_OFFSET + dw_ssi->base);
	value &= ~((0x3 & BITS_DW_SSI_CTRLR0_TMOD_MASK) << BITS_DW_SSI_CTRLR0_TMOD_SHIFT);
	value |= (transfer_mode & BITS_DW_SSI_CTRLR0_TMOD_MASK) << BITS_DW_SSI_CTRLR0_TMOD_SHIFT;
	raw_writel(REG_DW_SSI_CTRLR0_OFFSET + dw_ssi->base, value);
}

static void dw_ssi_read_count(dw_ssi_t *dw_ssi, uint32_t count)
{
	if (NULL != dw_ssi) {
		raw_writel(REG_DW_SSI_SSIENR_OFFSET + dw_ssi->base, 0);
		raw_writel(REG_DW_SSI_CTRLR1_OFFSET + dw_ssi->base, count & 0xFFFFU);
		raw_writel(REG_DW_SSI_SSIENR_OFFSET + dw_ssi->base, 1);
	}
}

static int32_t dw_ssi_enable(dw_ssi_t *dw_ssi, uint32_t enable)
{
	int32_t result = E_OK;

	if (NULL == dw_ssi) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = REG_DW_SSI_SSIENR_OFFSET + dw_ssi->base;
		if (enable) {
			raw_writel(reg_addr, 1);
		} else {
			raw_writel(reg_addr, 0);
		}
	}

	return result;
}

static int32_t dw_ssi_slave_enable(dw_ssi_t *dw_ssi, uint32_t sel_mask, uint32_t enable)
{
	int32_t result = E_OK;

	if (NULL == dw_ssi) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = REG_DW_SSI_SER_OFFSET + dw_ssi->base;
		uint32_t val = raw_readl(reg_addr);
		if (enable) {
			val |= sel_mask;
		} else {
			val &= ~sel_mask;
		}
		raw_writel(reg_addr, val);
	}

	return result;
}

static int32_t dw_ssi_baud_rate(dw_ssi_t *dw_ssi, uint32_t baud)
{
	int32_t result = E_OK;

	if (NULL == dw_ssi) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = REG_DW_SSI_BAUDR_OFFSET + dw_ssi->base;
		uint32_t baud_div = dw_ssi->ref_clock / baud;
		raw_writel(reg_addr, baud_div & 0xFFFFU);
	}

	return result;
}

static int32_t dw_ssi_fifo_entry(dw_ssi_t *dw_ssi, uint32_t *entry)
{
	int32_t result = E_OK;

	if ((NULL == dw_ssi) || (NULL == entry)) {
		result = E_PAR;
	} else {
		*entry = dw_ssi->base + REG_DW_SSI_DR_OFFSET(0);
	}

	return result;
}

static int32_t dw_ssi_fifo_threshold(dw_ssi_t *dw_ssi, uint32_t rx_thres, uint32_t tx_thres)
{
	int32_t result = E_OK;

	if (NULL == dw_ssi) {
		result = E_PAR;
	} else {
		raw_writel(REG_DW_SSI_RXFTLR_OFFSET + dw_ssi->base, rx_thres);
		raw_writel(REG_DW_SSI_TXFTLR_OFFSET + dw_ssi->base, tx_thres);
	}

	return result;
}

static int32_t dw_ssi_fifo_data_count(dw_ssi_t *dw_ssi, uint32_t rx_or_tx)
{
	int32_t result = E_OK;

	if (NULL == dw_ssi) {
		result = E_PAR;
	} else {
		uint32_t reg_addr;
		if (DW_SSI_RX_FIFO == rx_or_tx) {
			reg_addr = REG_DW_SSI_RXFLR_OFFSET + dw_ssi->base;
		} else {
			reg_addr = REG_DW_SSI_TXFLR_OFFSET + dw_ssi->base;
		}
		result = raw_readl(reg_addr);
	}

	return result;
}

static int32_t dw_ssi_status(dw_ssi_t *dw_ssi, uint32_t status)
{
	int32_t result = E_OK;

	if (NULL == dw_ssi) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = REG_DW_SSI_SR_OFFSET + dw_ssi->base;
		uint32_t val = raw_readl(reg_addr);

		switch (status) {
			case SSI_STS_DATA_COLLISION_ERROR:
				result = (val & BIT_DW_SSI_SR_DCOL) ? (1) : (0);
				break;
			case SSI_STS_TRANSMISSION_ERROR:
				result = (val & BIT_DW_SSI_SR_TXE) ? (1) : (0);
				break;
			case SSI_STS_RX_FIFO_FULL:
				result = (val & BIT_DW_SSI_SR_RFF) ? (1) : (0);
				break;
			case SSI_STS_RX_FIFO_NOT_EMPTY:
				result = (val & BIT_DW_SSI_SR_RFNE) ? (1) : (0);
				break;
			case SSI_STS_TX_FIFO_EMPTY:
				result = (val & BIT_DW_SSI_SR_TFE) ? (1) : (0);
				break;
			case SSI_STS_TX_FIFO_NOT_FULL:
				result = (val & BIT_DW_SSI_SR_TFNF) ? (1) : (0);
				break;
			case SSI_STS_BUSY:
				result = (val & BIT_DW_SSI_SR_BUSY) ? (1) : (0);
				break;
			case SSI_STS_ALL:
				result = val;
				break;
			default:
				result = E_PAR;
		}
	}

	return result;
}

static int32_t dw_ssi_interrupt_enable(dw_ssi_t *dw_ssi, uint32_t mask, uint32_t enable)
{
	int32_t result = E_OK;

	if (NULL == dw_ssi) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = REG_DW_SSI_IMR_OFFSET + dw_ssi->base;
		uint32_t val = raw_readl(reg_addr);

		if (enable) {
			val |= mask;
		} else {
			val &= ~mask;
		}
		raw_writel(reg_addr, val);
	}

	return result;
}

static int32_t dw_ssi_interrupt_all_status(dw_ssi_t *dw_ssi)
{
	int32_t result = E_OK;

	if (NULL == dw_ssi) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = REG_DW_SSI_ISR_OFFSET + dw_ssi->base;
		result = raw_readl(reg_addr);
	}

	return result;
}

static int32_t dw_ssi_interrupt_status(dw_ssi_t *dw_ssi, uint32_t status)
{
	int32_t result = E_OK;

	if (NULL == dw_ssi) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = REG_DW_SSI_ISR_OFFSET + dw_ssi->base;
		uint32_t val = raw_readl(reg_addr);

		switch (status) {
			case SSI_INT_STS_MULTI_MASTER_CONTENTION:
				result = (val & BIT_DW_SSI_IMR_MSTIS) ? (1) : (0);
				break;
			case SSI_INT_STS_RX_FIFO_FULL:
				result = (val & BIT_DW_SSI_IMR_RXFIS) ? (1) : (0);
				break;
			case SSI_INT_STS_RX_FIFO_OVERFLOW:
				result = (val & BIT_DW_SSI_IMR_RXOIS) ? (1) : (0);
				break;
			case SSI_INT_STS_RX_FIFO_UNDERFLOW:
				result = (val & BIT_DW_SSI_IMR_RXUIS) ? (1) : (0);
				break;
			case SSI_INT_STS_TX_FIFO_OVERFLOW:
				result = (val & BIT_DW_SSI_IMR_TXOIS) ? (1) : (0);
				break;
			case SSI_INT_STS_TX_FIFO_EMPTY:
				result = (val & BIT_DW_SSI_IMR_TXEIS) ? (1) : (0);
				break;
			case SSI_INT_STS_ALL:
				result = val;
				break;
			default:
				result = E_PAR;
		}
	}

	return result;
}


static int32_t dw_ssi_interrupt_status_clear(dw_ssi_t *dw_ssi, uint32_t status)
{
	int32_t result = E_OK;

	if (NULL == dw_ssi) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = dw_ssi->base;

		switch (status) {
			case SSI_INT_STS_MULTI_MASTER_CONTENTION:
				reg_addr += REG_DW_SSI_MSTICR_OFFSET;
				result = raw_readl(reg_addr);
				break;
			case SSI_INT_STS_RX_FIFO_OVERFLOW:
				reg_addr += REG_DW_SSI_RXOICR_OFFSET;
				result = raw_readl(reg_addr);
				break;
			case SSI_INT_STS_RX_FIFO_UNDERFLOW:
				reg_addr += REG_DW_SSI_RXUICR_OFFSET;
				result = raw_readl(reg_addr);
				break;
			case SSI_INT_STS_TX_FIFO_OVERFLOW:
				reg_addr += REG_DW_SSI_TXOICR_OFFSET;
				result = raw_readl(reg_addr);
				break;
			case SSI_INT_STS_RX_FIFO_FULL:
			case SSI_INT_STS_TX_FIFO_EMPTY:
				reg_addr += REG_DW_SSI_ICR_OFFSET;
				result = raw_readl(reg_addr);
				break;
			case SSI_INT_STS_ALL:
				reg_addr += REG_DW_SSI_ICR_OFFSET;
				result = raw_readl(reg_addr);
				break;
			default:
				result = E_PAR;
		}
	}

	return result;
}

static int32_t dw_ssi_dma_config(dw_ssi_t *dw_ssi, uint32_t rx_or_tx, uint32_t en, uint32_t threshold)
{
	int32_t result = E_OK;

	uint32_t value = 0;

	if ((NULL != dw_ssi)) {
		if (rx_or_tx) {
			/* tx. */
			value = raw_readl(REG_DW_SSI_DMACR_OFFSET + dw_ssi->base);
			if (en) {
				value |= BIT_DW_SSI_DMACR_TDMAE;
			} else {
				value &= ~BIT_DW_SSI_DMACR_TDMAE;
			}
			raw_writel(REG_DW_SSI_DMACR_OFFSET + dw_ssi->base, value);
			raw_writel(REG_DW_SSI_DMATDLR_OFFSET + dw_ssi->base, threshold);
		} else {
			value = raw_readl(REG_DW_SSI_DMACR_OFFSET + dw_ssi->base);
			if (en) {
				value |= BIT_DW_SSI_DMACR_RDMAE;
			} else {
				value &= ~BIT_DW_SSI_DMACR_RDMAE;
			}
			raw_writel(REG_DW_SSI_DMACR_OFFSET + dw_ssi->base, value);
			raw_writel(REG_DW_SSI_DMARDLR_OFFSET + dw_ssi->base, threshold);
		}
	} else {
		result = E_PAR;
	}

	return result;
}

static int32_t dw_ssi_version(dw_ssi_t *dw_ssi)
{
	int32_t result = E_OK;

	if (NULL == dw_ssi) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = REG_DW_SSI_VERSION_OFFSET + dw_ssi->base;
		result = raw_readl(reg_addr);
	}

	return result;
}

static int32_t dw_ssi_rx_sample_delay(dw_ssi_t *dw_ssi, uint32_t delay)
{
	int32_t result = E_OK;

	if (NULL == dw_ssi) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = REG_DW_SSI_RX_SAMPLE_DLY_OFFSET + dw_ssi->base;
		raw_writel(reg_addr, delay);
	}

	return result;
}

static int32_t dw_ssi_spi_control(dw_ssi_t *dw_ssi, dw_ssi_spi_ctrl_t *spi_ctrl)
{
	int32_t result = E_OK;

	uint32_t value = 0;

	if ((NULL == dw_ssi) || (NULL == spi_ctrl)) {
		result = E_PAR;
	} else {
		if (spi_ctrl->rd_strobe_en) {
			value |= BIT_DW_SSI_SPI_CTRLR0_RXDS_EN;
		}

		if (spi_ctrl->ins_ddr_en) {
			value |= BIT_DW_SSI_SPI_CTRLR0_INST_DDR_EN;
		}

		if (spi_ctrl->ddr_en) {
			value |= BIT_DW_SSI_SPI_CTRLR0_SPI_DDR_EN;
		}

		value |= ((spi_ctrl->wait_cycles & BIT_DW_SSI_SPI_CTRLR0_WAIT_CYCLES_MASK) << \
			BIT_DW_SSI_SPI_CTRLR0_WAIT_CYCLES_SHIFT);

		value |= (spi_ctrl->ins_len & BIT_DW_SSI_SPI_CTRLR0_INS_L_MASK) << \
			BIT_DW_SSI_SPI_CTRLR0_INS_L_SHIFT;

		value |= (spi_ctrl->addr_len & BIT_DW_SSI_SPI_CTRLR0_ADDR_L_MASK) << \
			BIT_DW_SSI_SPI_CTRLR0_ADDR_L_SHIFT;

		value |= (spi_ctrl->transfer_type & BIT_DW_SSI_SPI_CTRLR0_TRANS_TYPE_MASK) << \
			BIT_DW_SSI_SPI_CTRLR0_TRANS_TYPE_SHIFT;

		raw_writel(REG_DW_SSI_SPI_CTRLR0_OFFSET + dw_ssi->base, value);
	}

	return result;
}

static inline void dw_ssi_raw_write(dw_ssi_t *dw_ssi, uint32_t data)
{
	raw_writel(dw_ssi->base + REG_DW_SSI_DR_OFFSET(0), data);
}
static inline uint32_t dw_ssi_raw_read(dw_ssi_t *dw_ssi)
{
	return raw_readl((dw_ssi)->base + REG_DW_SSI_DR_OFFSET(0));
}
static int32_t dw_ssi_xfer(dw_ssi_t *dw_ssi, uint32_t *rx_buf, uint32_t *tx_buf, uint32_t len)
{
	int32_t result = E_OK;

	uint32_t idx, handled_cnt = 0;
	uint32_t dummy_data = 0;

	if ((NULL == dw_ssi) || (0 == len) || ((NULL == tx_buf) && (NULL == rx_buf))) {
		result = E_PAR;
	} else {
		uint32_t reg_addr = REG_DW_SSI_SR_OFFSET + dw_ssi->base;
		uint32_t sts = raw_readl(reg_addr);

		if (DEV_MASTER_MODE == dw_ssi->mode) {
			/* maybe move to dw_ssi_t. */
			uint32_t single_cnt = 32;

			/* wait TX FIFO real empty! */
			while((0 == (sts & BIT_DW_SSI_SR_TFE)) ||\
					(sts & BIT_DW_SSI_SR_BUSY)) {
				sts = raw_readl(reg_addr);
			}
			while(raw_readl(reg_addr) & BIT_DW_SSI_SR_RFNE) {
				raw_readl(dw_ssi->base + REG_DW_SSI_DR_OFFSET(0));
			}

			while (len) {
				if (len < single_cnt) {
					single_cnt = len;
				}

				if (NULL == tx_buf) {
					for (idx = 0; idx < single_cnt; idx++) {
						dw_ssi_raw_write(dw_ssi, dummy_data);
					}
				} else {
					for (idx = 0; idx < single_cnt; idx++) {
						dw_ssi_raw_write(dw_ssi, *tx_buf++);
					}
				}

				raw_writel(REG_DW_SSI_SER_OFFSET + dw_ssi->base, 1);
				/* wait TX FIFO real empty! */
				do {
					sts = raw_readl(reg_addr);
				} while((0 == (sts & BIT_DW_SSI_SR_TFE)) || (sts & BIT_DW_SSI_SR_BUSY));
				raw_writel(REG_DW_SSI_SER_OFFSET + dw_ssi->base, 0);

				if (NULL == rx_buf) {
					for (idx = 0; idx < single_cnt; idx++) {
						while(0 == (raw_readl(reg_addr) & BIT_DW_SSI_SR_RFNE));
						dw_ssi_raw_read(dw_ssi);
					}
				} else {
					for (idx = 0; idx < single_cnt; idx++) {
						while(0 == (raw_readl(reg_addr) & BIT_DW_SSI_SR_RFNE));
						*rx_buf++ = dw_ssi_raw_read(dw_ssi);
					}
				}

				len -= single_cnt;
				handled_cnt += single_cnt;
			}
		} else {
			if (NULL == rx_buf) {
				/* wait TX FIFO real empty! */
				while((0 == (sts & BIT_DW_SSI_SR_TFE)) || (sts & BIT_DW_SSI_SR_BUSY)) {
					sts = raw_readl(reg_addr);
				}

				while(raw_readl(reg_addr) & BIT_DW_SSI_SR_RFNE) {
					dw_ssi_raw_read(dw_ssi);
				}

				if (NULL != tx_buf) {
					for (idx = 0; idx < len; idx++) {
						if (0 == (raw_readl(reg_addr) & BIT_DW_SSI_SR_TFNF)) {
							break;
						}
						dw_ssi_raw_write(dw_ssi, *tx_buf++);
						handled_cnt++;
					}
				}
			} else {
				while(raw_readl(reg_addr) & BIT_DW_SSI_SR_RFNE) {
					*rx_buf++ = dw_ssi_raw_read(dw_ssi);
					handled_cnt++;
				}
			}
		}
	}

	if (E_OK == result) {
		result = handled_cnt;
	}

	return result;
}


static void dw_ssi_putdata(dw_ssi_t *dw_ssi, uint32_t data)
{
	while (0 == (raw_readl(REG_DW_SSI_SR_OFFSET + dw_ssi->base) & BIT_DW_SSI_SR_TFNF));
	raw_writel(dw_ssi->base + REG_DW_SSI_DR_OFFSET(0), data);
}
static uint32_t dw_ssi_getdata(dw_ssi_t *dw_ssi)
{
	while(0 == (raw_readl(REG_DW_SSI_SR_OFFSET + dw_ssi->base) & BIT_DW_SSI_SR_RFNE));
	return raw_readl((dw_ssi)->base + REG_DW_SSI_DR_OFFSET(0));
}

static uint32_t ctrl_save = 0;
static uint32_t spi_ctrl_save = 0;
static uint32_t dw_ssi_control_save_update(dw_ssi_t *dw_ssi,
		uint32_t dfs, dw_ssi_spi_ctrl_t *ctrl_desc, uint32_t flag)
{
	uint32_t transfer_mode = 0, comm_mode = 0;
	if (NULL != dw_ssi) {
		/* save control register. */
		ctrl_save = raw_readl(dw_ssi->base + REG_DW_SSI_CTRLR0_OFFSET);
		spi_ctrl_save = raw_readl(dw_ssi->base + REG_DW_SSI_CTRLR0_OFFSET);

		comm_mode = (ctrl_save >> BITS_DW_SSI_CTRLR0_SPI_FRF_SHIFT) & BITS_DW_SSI_CTRLR0_SPI_FRF_MASK;
		if (STANDARD_SPI_FRAME_FORMAT == comm_mode) {
			/* standard spi communication mode: using eeprom tr mode. */
			transfer_mode = 0;
		} else if (QUAD_FRAME_FORMAT == comm_mode) {
			if (flag) {
				transfer_mode = 1;
			} else {
				/* quad mode communication mode: using Read onlt transfer mode. */
				transfer_mode = 2;
			}
		} else {
			transfer_mode = 0;
		}

		/* config data frame format: (ins, addr, data) = (8, 0, 8). */
		raw_writel(REG_DW_SSI_SSIENR_OFFSET + dw_ssi->base, 0);
		dw_ssi_data_frame_config(dw_ssi,
				dfs,
				ctrl_desc->ins_len,
				ctrl_desc->addr_len,
				ctrl_desc->wait_cycles);
		dw_ssi_transfer_mode_update(dw_ssi, transfer_mode);
		raw_writel(REG_DW_SSI_SSIENR_OFFSET + dw_ssi->base, 1);
	}
	return transfer_mode;
}
static void dw_ssi_control_restore(dw_ssi_t *dw_ssi)
{
	if (NULL != dw_ssi) {
		raw_writel(dw_ssi->base + REG_DW_SSI_CTRLR0_OFFSET, ctrl_save);
		raw_writel(dw_ssi->base + REG_DW_SSI_CTRLR0_OFFSET, spi_ctrl_save);
	}
}

static void dw_ssi_ro_read(dw_ssi_t *dw_ssi, dw_ssi_xfer_t *xfer, uint32_t flag)
{
	if ((NULL != dw_ssi) && (NULL != xfer)) {
		uint32_t sts;
		uint32_t loop_cnt = 0;
		uint32_t single_read_cnt = 0x10000;
		uint32_t addr = xfer->addr;
		uint32_t length = xfer->len;
		uint32_t *wdata_ptr = (uint32_t *)xfer->buf;
		uint8_t *data_ptr = (uint8_t *)xfer->buf;

		while (length) {
			if (length < single_read_cnt) {
				single_read_cnt = length;
			}
			dw_ssi_read_count(dw_ssi, single_read_cnt - 1);

			dw_ssi_putdata(dw_ssi, xfer->ins);
			if (xfer->addr_valid) {
				dw_ssi_putdata(dw_ssi, xfer->addr);
			}

			loop_cnt = single_read_cnt;
			/* select salve: start to transfer data. */
			raw_writel(REG_DW_SSI_SER_OFFSET + dw_ssi->base, 1);

			if (flag) {
				while (loop_cnt--) {
					*wdata_ptr++ = dw_ssi_getdata(dw_ssi);
				}
				addr += single_read_cnt << 2;
			} else {
				while (loop_cnt--) {
					*data_ptr++ = dw_ssi_getdata(dw_ssi) & 0xFF;
				}
				addr += single_read_cnt;
			}
			length -= single_read_cnt;

			/* wait tx fifo real empty! */
			do {
				sts = raw_readl(REG_DW_SSI_SR_OFFSET + dw_ssi->base);
			} while((0 == (sts & BIT_DW_SSI_SR_TFE)) || (sts & BIT_DW_SSI_SR_BUSY));
			raw_writel(REG_DW_SSI_SER_OFFSET + dw_ssi->base, 0);

		}
	}
}

static void dw_ssi_normal_read(dw_ssi_t *dw_ssi, dw_ssi_xfer_t *xfer, uint32_t flag)
{
	if ((NULL != dw_ssi) && (NULL != xfer)) {
		uint32_t sts;
		uint32_t wait_bytes = 0;
		uint32_t unused_data_cnt = 1;
		uint32_t pre_filling_cnt = 0;
		uint32_t length = xfer->len;
		uint32_t rd_ins = xfer->ins;

		if (flag) {
			if (xfer->addr_valid) {
				rd_ins <<= 24;
				rd_ins |= (xfer->addr) & 0xFFFFFF;
			}
			dw_ssi_putdata(dw_ssi, rd_ins);
		} else {
			dw_ssi_putdata(dw_ssi, rd_ins);
			if (xfer->addr_valid) {
				/* if address width is 32bits, pls fix here! */
				dw_ssi_putdata(dw_ssi, (xfer->addr >> 16) & 0xFF);
				dw_ssi_putdata(dw_ssi, (xfer->addr >> 8) & 0xFF);
				dw_ssi_putdata(dw_ssi, xfer->addr & 0xFF);
				unused_data_cnt += 3;
			}
			wait_bytes = xfer->rd_wait_cycle/8;
			unused_data_cnt += wait_bytes;
			while (wait_bytes--) {
				dw_ssi_putdata(dw_ssi, 0);
			}
		}

		/* firstly, filling tx fifo fully. */
		while (raw_readl(REG_DW_SSI_SR_OFFSET + dw_ssi->base) & BIT_DW_SSI_SR_TFNF) {
			if (length) {
				raw_writel(dw_ssi->base + REG_DW_SSI_DR_OFFSET(0), 0);
				pre_filling_cnt++;
				length--;
			} else {
				break;
			}
		}

		/* select salve: start to transfer data. */
		raw_writel(REG_DW_SSI_SER_OFFSET + dw_ssi->base, 1);

		/* remove unused data. */
		while (unused_data_cnt--) {
			dw_ssi_getdata(dw_ssi);
			if (length) {
				//dw_ssi_putdata(dw_ssi, 0);
				raw_writel(dw_ssi->base + REG_DW_SSI_DR_OFFSET(0), 0);
				pre_filling_cnt++;
				length--;
			}
		}

		/* read data! */
		if (flag) {
			uint32_t *wdata_ptr = (uint32_t *)xfer->buf;
			while (length--) {
				*wdata_ptr++ = dw_ssi_getdata(dw_ssi);
				//dw_ssi_putdata(dw_ssi, 0);
				raw_writel(dw_ssi->base + REG_DW_SSI_DR_OFFSET(0), 0);
			}
			while (pre_filling_cnt--) {
				*wdata_ptr++ = dw_ssi_getdata(dw_ssi);
			}

		} else {
			uint8_t *data_ptr = (uint8_t *)xfer->buf;
			while (length--) {
				*data_ptr++ = (uint8_t)dw_ssi_getdata(dw_ssi);
				//dw_ssi_putdata(dw_ssi, 0);
				raw_writel(dw_ssi->base + REG_DW_SSI_DR_OFFSET(0), 0);
			}
			while (pre_filling_cnt--) {
				*data_ptr++ = (uint8_t)dw_ssi_getdata(dw_ssi);
			}
		}

		/* wait tx fifo real empty! */
		do {
			sts = raw_readl(REG_DW_SSI_SR_OFFSET + dw_ssi->base);
		} while((0 == (sts & BIT_DW_SSI_SR_TFE)) || (sts & BIT_DW_SSI_SR_BUSY));
		raw_writel(REG_DW_SSI_SER_OFFSET + dw_ssi->base, 0);
	}
}


static int32_t dw_ssi_read(dw_ssi_t *dw_ssi, dw_ssi_xfer_t *xfer, uint32_t flag)
{
	int32_t result = E_OK;


	if ((NULL == dw_ssi) || (NULL == xfer) || \
	    ((NULL == xfer->buf) || (0 == xfer->len))) {
		result = E_PAR;
	} else {
		uint32_t transfer_mode = 0;

		dw_ssi_spi_ctrl_t ctrl_desc = {
			.wait_cycles = xfer->rd_wait_cycle,
			.ins_len = DW_SSI_INS_LEN_8,
			.addr_len = DW_SSI_ADDR_LEN_24
		};

		if (flag) {
			transfer_mode = dw_ssi_control_save_update(dw_ssi, 31, &ctrl_desc, 0);
		} else {
			if (0 == xfer->addr_valid) {
				ctrl_desc.addr_len = DW_SSI_ADDR_LEN_0;
			}
			transfer_mode = dw_ssi_control_save_update(dw_ssi, 7, &ctrl_desc, 0);
		}

		/* clean up RX FIFO. */
		while(raw_readl(REG_DW_SSI_SR_OFFSET + dw_ssi->base) & BIT_DW_SSI_SR_RFNE) {
			dw_ssi_raw_read(dw_ssi);
		}


		if (DW_SSI_TR_MODE == transfer_mode) {
			dw_ssi_normal_read(dw_ssi, xfer, flag);

		} else if (DW_SSI_RECEIVE_ONLY == transfer_mode) {
			/* TODO: achieve in future! */
			dw_ssi_ro_read(dw_ssi, xfer, flag);

		} else if (DW_SSI_EEPROM == transfer_mode) {
			result = E_SYS;

		} else {
			result = E_SYS;
		}

		/* clean up RX FIFO. */
		while(raw_readl(REG_DW_SSI_SR_OFFSET + dw_ssi->base) & BIT_DW_SSI_SR_RFNE) {
			dw_ssi_raw_read(dw_ssi);
		}

		/* restore control register. */
		dw_ssi_control_restore(dw_ssi);
	}

	return result;
}

static void dw_ssi_normal_write(dw_ssi_t *dw_ssi, dw_ssi_xfer_t *xfer, uint32_t flag)
{
	if ((NULL != dw_ssi) || (NULL != xfer)) {
		uint32_t sts;
		uint32_t pre_filling_cnt = 0;
		uint32_t length = xfer->len;
		uint32_t w_ins = xfer->ins;
		uint32_t *wdata_ptr = (uint32_t *)xfer->buf;
		uint8_t *data_ptr = (uint8_t *)xfer->buf;

		if (flag) {
			if (xfer->addr_valid) {
				w_ins <<= 24;
				w_ins |= (xfer->addr) & 0xFFFFFF;
			}
			dw_ssi_putdata(dw_ssi, w_ins);
		} else {
			dw_ssi_putdata(dw_ssi, xfer->ins);
			if (xfer->addr_valid) {
				/* if address width is 32bits, pls fix here! */
				dw_ssi_putdata(dw_ssi, (xfer->addr >> 16) & 0xFF);
				dw_ssi_putdata(dw_ssi, (xfer->addr >> 8) & 0xFF);
				dw_ssi_putdata(dw_ssi, xfer->addr & 0xFF);
			}
		}

		/* filling tx fifo fully. */
		while (raw_readl(REG_DW_SSI_SR_OFFSET + dw_ssi->base) & BIT_DW_SSI_SR_TFNF) {
			if (length) {
				if (flag) {
					raw_writel(dw_ssi->base + REG_DW_SSI_DR_OFFSET(0), *wdata_ptr++);
				} else {
					raw_writel(dw_ssi->base + REG_DW_SSI_DR_OFFSET(0), *data_ptr++);
				}
				pre_filling_cnt++;
				length--;
			} else {
				break;
			}
		}

		/* select salve: start to transfer data. */
		raw_writel(REG_DW_SSI_SER_OFFSET + dw_ssi->base, 1);

		/* write data! */
		if (flag) {
			while (length--) {
				dw_ssi_getdata(dw_ssi);
				dw_ssi_putdata(dw_ssi, *wdata_ptr++);
			}
		} else {
			while (length--) {
				dw_ssi_getdata(dw_ssi);
				dw_ssi_putdata(dw_ssi, *data_ptr++);
			}
		}

		/* wait tx fifo real empty! */
		do {
			sts = raw_readl(REG_DW_SSI_SR_OFFSET + dw_ssi->base);
		} while((0 == (sts & BIT_DW_SSI_SR_TFE)) || (sts & BIT_DW_SSI_SR_BUSY));
		raw_writel(REG_DW_SSI_SER_OFFSET + dw_ssi->base, 0);
	}
}

static void dw_ssi_to_write(dw_ssi_t *dw_ssi, dw_ssi_xfer_t *xfer, uint32_t flag)
{
	if ((NULL != dw_ssi) || (NULL != xfer)) {
		uint32_t sts;
		uint32_t pre_filling_cnt = 32;
		uint32_t length = xfer->len;
		uint8_t *data_ptr = (uint8_t *)xfer->buf;

		dw_ssi_putdata(dw_ssi, xfer->ins);
		if (xfer->addr_valid) {
			/* if address width is 32bits, pls fix here! */
			dw_ssi_putdata(dw_ssi, (xfer->addr >> 16) & 0xFF);
			dw_ssi_putdata(dw_ssi, (xfer->addr >> 8) & 0xFF);
			dw_ssi_putdata(dw_ssi, xfer->addr & 0xFF);
		}

		/* filling tx fifo fully. */
		if (length < pre_filling_cnt) {
			pre_filling_cnt = length;
		}
		while (pre_filling_cnt--) {
			if (raw_readl(REG_DW_SSI_SR_OFFSET + dw_ssi->base) & BIT_DW_SSI_SR_TFNF) {
				raw_writel(dw_ssi->base + REG_DW_SSI_DR_OFFSET(0), *data_ptr++);
				length--;
			} else {
				break;
			}
		}

		/* select salve: start to transfer data. */
		raw_writel(REG_DW_SSI_SER_OFFSET + dw_ssi->base, 1);

		/* write data! */
		while (length--) {
			dw_ssi_putdata(dw_ssi, *data_ptr++);
		}

		/* wait tx fifo real empty! */
		do {
			sts = raw_readl(REG_DW_SSI_SR_OFFSET + dw_ssi->base);
		} while((0 == (sts & BIT_DW_SSI_SR_TFE)) || (sts & BIT_DW_SSI_SR_BUSY));
		raw_writel(REG_DW_SSI_SER_OFFSET + dw_ssi->base, 0);
	}
}

static int32_t dw_ssi_write(dw_ssi_t *dw_ssi, dw_ssi_xfer_t *xfer, uint32_t flag)
{
	int32_t result = E_OK;


	if ((NULL == dw_ssi) || (NULL == xfer) || \
	    ((NULL == xfer->buf) && (xfer->len > 0)) || \
	    ((NULL != xfer->buf) && (0 == xfer->len))) {
		result = E_PAR;
	} else {
		uint32_t transfer_mode = 0;

		dw_ssi_spi_ctrl_t ctrl_desc = {
			.wait_cycles = xfer->rd_wait_cycle,
			.ins_len = DW_SSI_INS_LEN_8,
			.addr_len = DW_SSI_ADDR_LEN_24
		};

		if (flag) {
			transfer_mode = dw_ssi_control_save_update(dw_ssi, 31, &ctrl_desc, 1);
		} else {
			if (0 == xfer->addr_valid) {
				ctrl_desc.addr_len = DW_SSI_ADDR_LEN_0;
			}
			transfer_mode = dw_ssi_control_save_update(dw_ssi, 7, &ctrl_desc, 1);
		}

		if (DW_SSI_TR_MODE == transfer_mode) {
			dw_ssi_normal_write(dw_ssi, xfer, flag);

		} else if (DW_SSI_TRANSMIT_ONLY == transfer_mode) {
			/* TODO: support in future. */
			dw_ssi_to_write(dw_ssi, xfer, flag);

		} else {
			result = E_SYS;
		}

		/* clean up RX FIFO. */
		while(raw_readl(REG_DW_SSI_SR_OFFSET + dw_ssi->base) & BIT_DW_SSI_SR_RFNE) {
			dw_ssi_raw_read(dw_ssi);
		}

		/* restore control register. */
		dw_ssi_control_restore(dw_ssi);
	}

	return result;
}

static dw_ssi_ops_t dw_ssi_ops = {
	.control = dw_ssi_control,
	.enable = dw_ssi_enable,
	.fifo_threshold = dw_ssi_fifo_threshold,
	.baud = dw_ssi_baud_rate,
	.rx_sample_delay = dw_ssi_rx_sample_delay,
	.xfer = dw_ssi_xfer,
	.read = dw_ssi_read,
	.write = dw_ssi_write,
	.dma_config = dw_ssi_dma_config,
	.int_enable = dw_ssi_interrupt_enable,
	.int_clear = dw_ssi_interrupt_status_clear,
	.int_status = dw_ssi_interrupt_status,
	.int_all_status = dw_ssi_interrupt_all_status,
	.status = dw_ssi_status,
	.fifo_data_count = dw_ssi_fifo_data_count,
	.spi_control = dw_ssi_spi_control,
	.fifo_entry = dw_ssi_fifo_entry,
	.version = dw_ssi_version
};

int32_t dw_ssi_install_ops(dw_ssi_t *dw_ssi)
{
	int32_t result = E_OK;

	if (NULL == dw_ssi) {
		result = E_PAR;
	} else {
		dw_ssi->ops = (void *)&dw_ssi_ops;
	}

	return result;
}
