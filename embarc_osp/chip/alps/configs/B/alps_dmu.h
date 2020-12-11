#ifndef _DMU_RAW_API_H_
#define _DMU_RAW_API_H_

#include "alps_dmu_reg.h"

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

#define IO_MUX_SPI_S1_FUNC_MIX(clk, sel, mosi, miso)	( (((clk) & 0xF)) )
static inline void io_mux_spi_s1_func_sel(uint32_t mix_func)
{
	raw_writel(REG_DMU_MUX_SPI_S1_OFFSET + REL_REGBASE_DMU, mix_func & 0xF);
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
	/* dummy. */
}

static inline void io_mux_fmcw_start_sel(void)
{
}

static inline void io_mux_qspi_s_sel(void)
{
}

static inline void dmu_irq_enable(uint32_t irq, uint32_t en)
{
}

static inline void dmu_irq_select(uint32_t irq, uint32_t irq_sel)
{
}
#endif
