#ifndef _DMU_RAW_API_H_
#define _DMU_RAW_API_H_

#include "alps_dmu_reg.h"
#include "alps_io_mux.h"

static inline void sys_dmu_select(uint32_t mode)
{
	raw_writel(REG_DMU_SYS_DMU_SEL_OFFSET + REL_REGBASE_DMU, mode);
}

static inline uint32_t sys_dmu_mode_get(void)
{
	return raw_readl(REG_DMU_SYS_DMU_SEL_OFFSET + REL_REGBASE_DMU);
}

static inline void io_mux_spi_m1_func_sel(uint32_t func)
{
	raw_writel(REG_DMU_MUX_SPI_M1_OFFSET + REL_REGBASE_DMU, func & 0xF);
}

/*
 * [31:24], spi_s1_clk.
 * [23:16], spi_s1_sel.
 * [15:8], spi_s1_mosi.
 * [7:0], spi_s1_miso.
 * */
#define IO_MUX_SPI_S1_FUNC_MIX(clk, sel, mosi, miso)	( \
		(((clk) & 0xF) << 24) | (((sel) & 0xF) << 16) | \
		(((mosi) & 0xF) << 8) | (((miso) & 0xF)) )

static inline void io_mux_spi_s1_func_sel(uint32_t mix_func)
{
	raw_writel(REG_DMU_MUX_SPI_S1_CLK_OFFSET + REL_REGBASE_DMU, (mix_func >> 24) & 0xF);
	raw_writel(REG_DMU_MUX_SPI_S1_SEL_OFFSET + REL_REGBASE_DMU, (mix_func >> 16) & 0xF);
	raw_writel(REG_DMU_MUX_SPI_S1_MOSI_OFFSET + REL_REGBASE_DMU, (mix_func >> 8) & 0xF);
	raw_writel(REG_DMU_MUX_SPI_S1_MISO_OFFSET + REL_REGBASE_DMU, (mix_func) & 0xF);
}

static inline void io_mux_uart0_func_sel(uint32_t func)
{
	raw_writel(REG_DMU_MUX_UART0_OFFSET + REL_REGBASE_DMU, func & 0xF);
}

static inline void io_mux_uart1_func_sel(uint32_t func)
{
	raw_writel(REG_DMU_MUX_UART1_OFFSET + REL_REGBASE_DMU, func & 0xF);
}

static inline void io_mux_can0_func_sel(uint32_t func)
{
	raw_writel(REG_DMU_MUX_CAN0_OFFSET + REL_REGBASE_DMU, func & 0xF);
}

static inline void io_mux_can1_func_sel(uint32_t func)
{
	raw_writel(REG_DMU_MUX_CAN1_OFFSET + REL_REGBASE_DMU, func & 0xF);
}

static inline void io_mux_adc_reset_func_sel(uint32_t func)
{
	raw_writel(REG_DMU_MUX_RESET_OFFSET + REL_REGBASE_DMU, func & 0xF);
}

static inline void io_mux_adc_sync_func_sel(uint32_t func)
{
	raw_writel(REG_DMU_MUX_SYNC_OFFSET + REL_REGBASE_DMU, func & 0xF);
}

static inline void io_mux_i2c_func_sel(uint32_t func)
{
	raw_writel(REG_DMU_MUX_I2C_OFFSET + REL_REGBASE_DMU, func & 0xF);
}

static inline void io_mux_pwm0_func_sel(uint32_t func)
{
	raw_writel(REG_DMU_MUX_PWM0_OFFSET + REL_REGBASE_DMU, func & 0xF);
}

static inline void io_mux_pwm1_func_sel(uint32_t func)
{
	raw_writel(REG_DMU_MUX_PWM1_OFFSET + REL_REGBASE_DMU, func & 0xF);
}

static inline void io_mux_adc_clk_func_sel(uint32_t func)
{
	raw_writel(REG_DMU_MUX_ADC_CLK_OFFSET + REL_REGBASE_DMU, func & 0xF);
}

static inline void io_mux_can_clk_func_sel(uint32_t func)
{
	raw_writel(REG_DMU_MUX_CAN_CLK_OFFSET + REL_REGBASE_DMU, func & 0xF);
}

static inline void io_mux_spi_m0_func_sel(uint32_t func)
{
	raw_writel(REG_DMU_MUX_SPI_M0_OFFSET + REL_REGBASE_DMU, func & 0xF);
}

/* spi-s on APB. */
static inline void io_mux_spi_s0_func_sel(uint32_t func)
{
	raw_writel(REG_DMU_MUX_SPI_S_OFFSET + REL_REGBASE_DMU, func & 0xF);
}

static inline void io_mux_jtag_func_sel(uint32_t func)
{
	raw_writel(REG_DMU_MUX_JTAG_OFFSET + REL_REGBASE_DMU, func & 0xF);
}



static inline void io_mux_qspi_m1_sel(void)
{
	io_mux_spi_m1_func_sel(IO_MUX_FUNC2);
}

static inline void io_mux_fmcw_start_sel(void)
{
	raw_writel(REG_DMU_MUX_SPI_S1_CLK_OFFSET + REL_REGBASE_DMU, IO_MUX_FUNC2);
}

static inline void io_mux_fmcw_start_sel_disable(void)
{
	raw_writel(REG_DMU_MUX_SPI_S1_CLK_OFFSET + REL_REGBASE_DMU, IO_MUX_FUNC4);
}

static inline void io_mux_qspi_s_sel(void)
{
	io_mux_spi_s0_func_sel(IO_MUX_FUNC2);
	raw_writel(REG_DMU_MUX_SPI_S1_MOSI_OFFSET + REL_REGBASE_DMU, IO_MUX_FUNC2);
	raw_writel(REG_DMU_MUX_SPI_S1_MISO_OFFSET + REL_REGBASE_DMU, IO_MUX_FUNC2);
}

static inline void dmu_irq_enable(uint32_t irq, uint32_t en)
{
	uint32_t shift = 0;
	uint32_t addr = REL_REGBASE_DMU + REG_DMU_IRQ_ENA0_31_OFFSET;

	addr += (irq >> 5) << 2;
	shift = irq & 0x1F;
	if (en) {
		raw_writel(addr, raw_readl(addr) | (1 << shift));
	} else {
		raw_writel(addr, raw_readl(addr) & ~(1 << shift));
	}
}

static inline void dmu_irq_select(uint32_t irq, uint32_t irq_sel)
{
	uint32_t addr = REL_REGBASE_DMU + REG_DMU_IRQ25_SEL_OFFSET;
	if ((irq >= 25) && (irq <= 31)) {
		addr += (irq - 25) << 2;
		raw_writel(addr, irq_sel);
	}
}

static inline void dmu_pwm_output_enable(uint32_t id, uint32_t en)
{
	if (en) {
		raw_writel(REL_REGBASE_DMU + REG_DMU_SYS_PWM0_ENA_OFFSET, 0);
	} else {
		raw_writel(REL_REGBASE_DMU + REG_DMU_SYS_PWM0_ENA_OFFSET, 1);
	}
}

static inline void dmu_otp_write_en(uint32_t enable)
{
	if (enable) {
		raw_writel(REL_REGBASE_DMU + REG_DMU_SYS_OTP_PRGM_EN_OFFSET, 1);
	} else {
		raw_writel(REL_REGBASE_DMU + REG_DMU_SYS_OTP_PRGM_EN_OFFSET, 0);
	}
}
#endif
