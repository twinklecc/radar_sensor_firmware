#ifndef _ALPS_DMU_REG_H_
#define _ALPS_DMU_REG_H_

#define REG_DMU_CMD_SRC_SEL_OFFSET	(0 << 2)
#define REG_DMU_CMD_OUT_OFFSET		(1 << 2)
#define REG_DMU_CMD_IN_OFFSET		(2 << 2)

#define REG_DMU_ADC_RSTN_OFFSET		(3 << 2)
#define REG_DMU_ADC_CNT_OFFSET		(4 << 2)
#define DMU_ADC_SYN_OFFSET		(5 << 2)

#define REG_DMU_DAC_ENA_OFFSET		(6 << 2)
#define REG_DMU_DAC_MUL_OFFSET		(7 << 2)
#define REG_DMU_DAC_DIV_OFFSET		(8 << 2)
#define REG_DMU_FMCW_START_OFFSET	(9 << 2)
#define REG_DMU_FMCW_STATUS_OFFSET	(10 << 2)

#define REG_DMU_IRQ_EXT_MASK_OFFSET	(11 << 2)
#define REG_DMU_IRQ_EXT_OFFSET		(12 << 2)
#define REG_DMU_IRQ_SW_SET_OFFSET	(13 << 2)
#define REG_DMU_IRQ_SW_OFFSET		(14 << 2)

#define REG_DMU_DBG_SRC_OFFSET		(15 << 2)
#define REG_DMU_DBG_VAL_OEN_OFFSET	(16 << 2)
#define DMU_DBG_DAT_OEN_OFFSET		(17 << 2)
#define DMU_DBG_DAT_O_OFFSET		(18 << 2)
#define DMU_DBG_DAT_I_OFFSET		(19 << 2)

#define REG_DMU_HIL_ENA_OFFSET		(20 << 2)

/* IO MUX registers. */
#define REG_DMU_MUX_SPI_M1_OFFSET	(22 << 2)
#define REG_DMU_MUX_UART0_OFFSET	(23 << 2)
#define REG_DMU_MUX_UART1_OFFSET	(24 << 2)
#define REG_DMU_MUX_CAN0_OFFSET		(25 << 2)
#define REG_DMU_MUX_CAN1_OFFSET		(26 << 2)
#define REG_DMU_MUX_RESET_OFFSET	(27 << 2)
#define REG_DMU_MUX_SYNC_OFFSET		(28 << 2)
#define REG_DMU_MUX_I2C_OFFSET		(29 << 2)
#define REG_DMU_MUX_PWM0_OFFSET		(30 << 2)
#define REG_DMU_MUX_PWM1_OFFSET		(31 << 2)
#define REG_DMU_MUX_ADC_CLK_OFFSET	(32 << 2)
#define REG_DMU_MUX_CAN_CLK_OFFSET	(33 << 2)
#define REG_DMU_MUX_SPI_M0_OFFSET	(34 << 2)
#define REG_DMU_MUX_SPI_S_OFFSET	(35 << 2)
#define REG_DMU_MUX_SPI_S1_OFFSET	(36 << 2)
#define REG_DMU_MUX_JTAG_OFFSET		(37 << 2)

#define REG_DMU_SYS_DMA_ENDIAN_OFFSET	(38 << 2)
#define REG_DMU_SYS_DMA_REQ_S_OFFSET	(39 << 2)
#define REG_DMU_SYS_SHSEL_NP_OFFSET	(40 << 2)
#define REG_DMU_SYS_DMU_SEL_OFFSET	(41 << 2)
#define REG_DMU_SYS_ICM0_FIX_P_OFFSET	(42 << 2)
#define REG_DMU_SYS_ICM1_FIX_P_OFFSET	(43 << 2)
#define REG_DMU_SYS_PWM0_ENA_OFFSET	(44 << 2)
#define REG_DMU_SYS_PWM1_ENA_OFFSET	(45 << 2)
#define REG_DMU_SYS_MEMRUN_ENA_OFFSET	(46 << 2)
#define REG_DMU_SYS_MEMINI_ENA_OFFSET	(47 << 2)


/* cmd out bits fields define. */
#define BIT_REG_DMU_CMD_WR		(1 << 15)
#define BITS_REG_DMU_CMD_ADDR_MASK	(0x7F)
#define BITS_REG_DMU_CMD_ADDR_SHIFT	(8)
#define BITS_REG_DMU_CMD_WR_DAT_MASK	(0xFF)
#define BITS_REG_DMU_CMD_WR_DAT_SHIFT	(0)

/* dmu select. */
#define SYS_DMU_SEL_DBG			(1)
#define SYS_DMU_SEL_GPIO		(0)

#endif
