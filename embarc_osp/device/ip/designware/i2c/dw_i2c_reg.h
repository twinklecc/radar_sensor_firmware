#ifndef _DW_I2C_REG_H_
#define _DW_I2C_REG_H_

#define REG_DW_I2C_CON_OFFSET		(0x0000)
#define REG_DW_I2C_TAR_OFFSET		(0x0004)
#define REG_DW_I2C_SAR_OFFSET		(0x0008)
#define REG_DW_I2C_HS_MADDR_OFFSET	(0x000C)
#define REG_DW_I2C_DATA_CMD_OFFSET	(0x0010)
#define REG_DW_I2C_SS_SCL_HCNT_OFFSET	(0x0014)
#define REG_DW_I2C_SS_SCL_LCNT_OFFSET	(0x0018)
#define REG_DW_I2C_FS_SCL_HCNT_OFFSET	(0x001C)
#define REG_DW_I2C_FS_SCL_LCNT_OFFSET	(0x0020)
#define REG_DW_I2C_HS_SCL_HCNT_OFFSET	(0x0024)
#define REG_DW_I2C_HS_SCL_LCNT_OFFSET	(0x0028)
#define REG_DW_I2C_INTR_STAT_OFFSET	(0x002C)
#define REG_DW_I2C_INTR_MASK_OFFSET	(0x0030)
#define REG_DW_I2C_RAW_INTR_STAT_OFFSET	(0x0034)
#define REG_DW_I2C_RX_TL_OFFSET		(0x0038)
#define REG_DW_I2C_TX_TL_OFFSET		(0x003C)
#define REG_DW_I2C_CLR_INTR_OFFSET	(0x0040)
#define REG_DW_I2C_CLR_RX_UNDER_OFFSET	(0x0044)
#define REG_DW_I2C_CLR_RX_OVER_OFFSET	(0x0048)
#define REG_DW_I2C_CLR_TX_OVER_OFFSET	(0x004C)
#define REG_DW_I2C_CLR_RD_REQ_OFFSET	(0x0050)
#define REG_DW_I2C_CLR_TX_ABORT_OFFSET	(0x0054)
#define REG_DW_I2C_CLR_RX_DONE_OFFSET	(0x0058)
#define REG_DW_I2C_CLR_ACTIVITY_OFFSET	(0x005C)
#define REG_DW_I2C_CLR_STOP_DET_OFFSET	(0x0060)
#define REG_DW_I2C_CLR_START_DET_OFFSET	(0x0064)
#define REG_DW_I2C_CLR_GEN_CALL_OFFSET	(0x0068)
#define REG_DW_I2C_ENABLE_OFFSET	(0x006C)
#define REG_DW_I2C_STATUS_OFFSET	(0x0070)
#define REG_DW_I2C_TXFLR_OFFSET		(0x0074)
#define REG_DW_I2C_RXFLR_OFFSET		(0x0078)
#define REG_DW_I2C_SDA_HOLD_OFFSET	(0x007C)
#define REG_DW_I2C_TX_ABRT_SRC_OFFSET	(0x0080)
#define REG_DW_I2C_SLV_DATA_NACK_OFFSET	(0x0084)
#define REG_DW_I2C_DMA_CR_OFFSET	(0x0088)
#define REG_DW_I2C_DMA_TDLR_OFFSET	(0x008C)
#define REG_DW_I2C_DMA_RDLR_OFFSET	(0x0090)
#define REG_DW_I2C_SDA_SETUP_OFFSET	(0x0094)
#define REG_DW_I2C_ACK_GEN_CALL_OFFSET	(0x0098)
#define REG_DW_I2C_ENABLE_STATUS_OFFSET	(0x009C)
#define REG_DW_I2C_FS_SPKLEN_OFFSET	(0x00A0)
#define REG_DW_I2C_UFM_SPKLEN_OFFSET	(0x00A0)
#define REG_DW_I2C_HS_SPKLEN_OFFSET	(0x00A4)
#define REG_DW_I2C_CLR_RS_DET_OFFSET	(0x00A8)
#define REG_DW_I2C_SCL_STUCK_LT_OFFSET	(0x00AC)
#define REG_DW_I2C_SDA_STUCK_LT_OFFSET	(0x00B0)
#define REG_DW_I2C_CLR_SCL_STUCK_OFFSET	(0x00B4)
#define REG_DW_I2C_DEVICE_ID_OFFSET	(0x00B8)
#define REG_DW_I2C_COMP_PARAM_1_OFFSET	(0x00F4)
#define REG_DW_I2C_COMP_VERSION_OFFSET	(0x00F8)
#define REG_DW_I2C_COMP_TYPE_OFFSET	(0x00FC)

/* control register. */
#define BIT_I2C_CON_STOP_DET_ADDR   (1 << 10)
#define BIT_I2C_CON_TX_EMPTY_CTRL	(1 << 8)
#define BIT_I2C_CON_SLAVE_DISABLE	(1 << 6)
#define BIT_I2C_CON_RESTART_EN		(1 << 5)
#define BIT_I2C_CON_M_10BIT_ADDR	(1 << 4)
#define BIT_I2C_CON_S_10BIT_ADDR    (1 << 3)
#define BITS_I2C_CON_SPEED_OFFSET	(1)
#define BITS_I2C_CON_SPEED_MASK		(0x3)
#define BIT_I2C_CON_M_MODE		(1 << 0)

/*The following interface is based on synopsys designware file,you will not use them in normal.
if you want achieve some customized function,you can use interfaces in the dw_i2c.c file.
If you really want to use the following interface.Please check your chip whether support corresponding function. */
static inline void dw_i2c_tx_empty_control(uint32_t enabled)
{
	uint32_t value = raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CON_OFFSET);
	if (enabled) {
		value |= BIT_I2C_CON_TX_EMPTY_CTRL;
	} else {
		value &= ~BIT_I2C_CON_TX_EMPTY_CTRL;
	}
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_CON_OFFSET, value);
}
static inline void dw_i2c_slave_issue_stop_det_int(uint32_t mode)
{
	uint32_t value = raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CON_OFFSET);
	if (mode) {
		value |= BIT_I2C_CON_STOP_DET_ADDR;
	} else {
		value &= ~BIT_I2C_CON_STOP_DET_ADDR;
	}
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_CON_OFFSET, value);
}
static inline uint32_t dw_i2c_slave_disabled(void)
{
	if (raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CON_OFFSET) & \
			BIT_I2C_CON_SLAVE_DISABLE) {
		return 1;
	} else {
		return 0;
	}
}
static inline void dw_i2c_restart(uint32_t enable)
{
	uint32_t value = raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CON_OFFSET);
	if (enable) {
		value |= BIT_I2C_CON_RESTART_EN;
	} else {
		value &= ~BIT_I2C_CON_RESTART_EN;
	}
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_CON_OFFSET, value);
}
static inline void dw_i2c_address_bit_width(uint32_t m_s, uint32_t w_10bit)
{
	uint32_t value = raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CON_OFFSET);
	if (m_s) {
		if (w_10bit) {
			value |= BIT_I2C_CON_S_10BIT_ADDR;
		} else {
			value &= ~BIT_I2C_CON_S_10BIT_ADDR;
		}
	} else {
		if (w_10bit) {
			value |= BIT_I2C_CON_M_10BIT_ADDR;
		} else {
			value &= ~BIT_I2C_CON_M_10BIT_ADDR;
		}
	}
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_CON_OFFSET, value);
}
static inline void dw_i2c_speed(uint32_t speed)
{
	uint32_t value = raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CON_OFFSET);
	value &= ~(BITS_I2C_CON_SPEED_MASK << BITS_I2C_CON_SPEED_OFFSET);
	value |= (BITS_I2C_CON_SPEED_MASK & speed) << BITS_I2C_CON_SPEED_OFFSET;
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_CON_OFFSET, value);
}
static inline void dw_i2c_master_mode(uint32_t enable)
{
	uint32_t value = raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CON_OFFSET);
	if (enable) {
		value |= BIT_I2C_CON_M_MODE;
	} else {
		value &= ~BIT_I2C_CON_M_MODE;
	}
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_CON_OFFSET, value);
}

static inline void dw_i2c_master_control(uint32_t tx_empty_ctrl, \
		uint32_t restart_en, uint32_t addr_10bits, uint32_t speed)
{
	uint32_t value = 1;
	if (tx_empty_ctrl) {
		value |= BIT_I2C_CON_TX_EMPTY_CTRL;
	}
	if (restart_en) {
		value |= BIT_I2C_CON_RESTART_EN;
	}
	if (addr_10bits) {
		value |= BIT_I2C_CON_M_10BIT_ADDR;
	}
	value |= (speed & 0x3) << 1;
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_CON_OFFSET, value);
}

/*static inline void dw_i2c_target_address(uint32_t special, uint32_t gc_or_start, uint32_t addr)
{
	uint32_t value = 0;
	if (special) {
		value |= (1 << 11);
		if (gc_or_start) {
			value |= (1 << 10);
		}
	} else {
		value = addr & 0x3FF;
	}
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_TAR_OFFSET, value);
}*/

static inline void dw_i2c_slave_address(uint32_t addr)
{
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_SAR_OFFSET, addr & 0x3FF);
}

/*static inline void dw_i2c_hs_master_code(uint32_t addr)
{
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_HS_MADDR_OFFSET, addr & 0x7);
}*/

static inline uint32_t dw_i2c_first_data_byte(void)
{
	uint32_t value = raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_DATA_CMD_OFFSET);
	if (value & (1 << 11)) {
		return 1;
	} else {
		return 0;
	}
}
static inline void dw_i2c_data_cmd(uint32_t restart, uint32_t stop, \
		uint32_t rd, uint32_t data)
{
	uint32_t value = 0;
	if (restart) {
		value |= (1 << 10);
	}
	if (stop) {
		value |= (1 << 9);
	}
	if (rd) {
		value |= (1 << 8);
	} else {
		value |= data & 0xFF;
	}
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_DATA_CMD_OFFSET, value);
}
static inline uint32_t dw_i2c_data_read(void)
{
	return raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_DATA_CMD_OFFSET) & 0xFF;
}

static inline void dw_i2c_ss_scl_count(uint32_t h_count, uint32_t l_count)
{
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_SS_SCL_HCNT_OFFSET, h_count & 0xFFFF);
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_SS_SCL_LCNT_OFFSET, l_count & 0xFFFF);
}

static inline void dw_i2c_fs_scl_count(uint32_t h_count, uint32_t l_count)
{
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_FS_SCL_HCNT_OFFSET, h_count & 0xFFFF);
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_FS_SCL_LCNT_OFFSET, l_count & 0xFFFF);
}

static inline void dw_i2c_hs_scl_count(uint32_t h_count, uint32_t l_count)
{
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_HS_SCL_HCNT_OFFSET, h_count & 0xFFFF);
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_HS_SCL_LCNT_OFFSET, l_count & 0xFFFF);
}

static inline uint32_t dw_i2c_int_status(void)
{
	return raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_INTR_STAT_OFFSET);
}

static inline uint32_t dw_i2c_raw_int_status(void)
{
	return raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_RAW_INTR_STAT_OFFSET);
}

static inline void dw_i2c_int_enable(uint32_t bits)
{
	uint32_t value = raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_INTR_MASK_OFFSET);
	value |= bits;
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_INTR_MASK_OFFSET, value);
}

static inline void dw_i2c_int_mask(uint32_t mask)
{
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_INTR_MASK_OFFSET, mask);
}

static inline void dw_i2c_rxfifo_threshold(uint32_t thres)
{
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_RX_TL_OFFSET, thres & 0xFF);
}

static inline void dw_i2c_txfifo_threshold(uint32_t thres)
{
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_TX_TL_OFFSET, thres & 0xFF);
}

static inline void dw_i2c_all_int_clear(void)
{
	raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CLR_INTR_OFFSET);
}

static inline void dw_i2c_rx_under_clear(void)
{
	raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CLR_RX_UNDER_OFFSET);
}
static inline void dw_i2c_rx_over_clear(void)
{
	raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CLR_RX_OVER_OFFSET);
}
static inline void dw_i2c_tx_over_clear(void)
{
	raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CLR_TX_OVER_OFFSET);
}
static inline void dw_i2c_tx_abort_clear(void)
{
	raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CLR_TX_ABORT_OFFSET);
}
static inline void dw_i2c_rx_done_clear(void)
{
	raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CLR_RX_DONE_OFFSET);
}
static inline void dw_i2c_activity_clear(void)
{
	raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CLR_ACTIVITY_OFFSET);
}
static inline void dw_i2c_stop_det_clear(void)
{
	raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CLR_STOP_DET_OFFSET);
}
static inline void dw_i2c_start_det_clear(void)
{
	raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CLR_START_DET_OFFSET);
}
static inline void dw_i2c_gen_call_clear(void)
{
	raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CLR_GEN_CALL_OFFSET);
}
static inline void dw_i2c_restart_det_clear(void)
{
	raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CLR_RS_DET_OFFSET);
}
static inline void dw_i2c_stuck_at_low_clear(void)
{
	raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_CLR_SCL_STUCK_OFFSET);
}

/*static inline void dw_i2c_enable(uint32_t tx_block, \
		uint32_t abort, uint32_t enable)
{
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
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_ENABLE_OFFSET, value);
}*/

/*static inline uint32_t dw_i2c_status(void)
{
	return raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_STATUS_OFFSET);
}*/

static inline uint32_t dw_i2c_txfifo_data_cnt(void)
{
	return raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_TXFLR_OFFSET) & 0x3F;
}

static inline uint32_t dw_i2c_rxfifo_data_cnt(void)
{
	return raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_RXFLR_OFFSET) & 0x3F;
}

/*static inline void dw_i2c_sda_hold_time(uint32_t rx_hold_time, uint32_t tx_hold_time)
{
	uint32_t value = tx_hold_time & 0xFFFF;
	value |= (rx_hold_time & 0xFF) << 16;
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_SDA_HOLD_OFFSET, value);
}*/


static inline uint32_t dw_i2c_abort_source(void)
{
	return raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_TX_ABRT_SRC_OFFSET);
}

static inline void dw_i2c_dma_enable(uint32_t rx, uint32_t tx)
{
	uint32_t value = 0;
	if (rx) {
		value |= (1 << 0);
	}
	if (tx) {
		value |= (1 << 1);
	}
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_DMA_CR_OFFSET, value);
}

static inline void dw_i2c_dma_tx_threshold(uint32_t thres)
{
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_DMA_TDLR_OFFSET, thres & 0x1F);
}
static inline void dw_i2c_dma_rx_threshold(uint32_t thres)
{
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_DMA_RDLR_OFFSET, thres & 0x1F);
}

static inline void dw_i2c_ack_general_call(uint32_t enable)
{
	if (enable) {
		raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_ACK_GEN_CALL_OFFSET, 1);
	} else {
		raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_ACK_GEN_CALL_OFFSET, 0);
	}
}

static inline uint32_t dw_i2c_enabled(void)
{
	return raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_ENABLE_STATUS_OFFSET);
}

static inline void dw_i2c_fs_spike(uint32_t spklen)
{
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_FS_SPKLEN_OFFSET, spklen & 0xFF);
}

static inline void dw_i2c_hs_spike(uint32_t spklen)
{
	raw_writel(REL_REGBASE_I2C0 + REG_DW_I2C_HS_SPKLEN_OFFSET, spklen & 0xFF);
}

static inline uint32_t dw_i2c_device_id(void)
{
	return raw_readl(REL_REGBASE_I2C0 + REG_DW_I2C_DEVICE_ID_OFFSET);
}


#endif
