#ifndef _ALPS_RESET_API_H_
#define _ALPS_RESET_API_H_


/****************************************
 * @rst = 1, assert reset the module.
 * @rst = 0, deassert reset the module.
 ****************************************/
static inline void xip_reset(uint32_t rst)
{
	if (rst) {
		raw_writel(REG_CLKGEN_RSTN_FLASH_CTRL, 0);
	} else {
		raw_writel(REG_CLKGEN_RSTN_FLASH_CTRL, 1);
	}
}

static inline void bb_top_reset(uint32_t rst)
{
	if (rst) {
		raw_writel(REG_CLKGEN_RSTN_BB_TOP, 0);
	} else {
		raw_writel(REG_CLKGEN_RSTN_BB_TOP, 1);
	}
}

static inline void bb_core_reset(uint32_t rst)
{
	if (rst) {
		raw_writel(REG_CLKGEN_RSTN_BB_CORE, 0);
	} else {
		raw_writel(REG_CLKGEN_RSTN_BB_CORE, 1);
	}
}

static inline void uart0_reset(uint32_t rst)
{
	if (rst) {
		raw_writel(REG_CLKGEN_RSTN_UART_0, 0);
	} else {
		raw_writel(REG_CLKGEN_RSTN_UART_0, 1);
	}
}

static inline void uart1_reset(uint32_t rst)
{
	if (rst) {
		raw_writel(REG_CLKGEN_RSTN_UART_1, 0);
	} else {
		raw_writel(REG_CLKGEN_RSTN_UART_1, 1);
	}
}

static inline void i2c_reset(uint32_t rst)
{
	if (rst) {
		raw_writel(REG_CLKGEN_RSTN_I2C, 0);
	} else {
		raw_writel(REG_CLKGEN_RSTN_I2C, 1);
	}
}

static inline void spi_m0_reset(uint32_t rst)
{
	if (rst) {
		raw_writel(REG_CLKGEN_RSTN_SPI_M0, 0);
	} else {
		raw_writel(REG_CLKGEN_RSTN_SPI_M0, 1);
	}
}

static inline void spi_m1_reset(uint32_t rst)
{
	if (rst) {
		raw_writel(REG_CLKGEN_RSTN_SPI_M1, 0);
	} else {
		raw_writel(REG_CLKGEN_RSTN_SPI_M1, 1);
	}
}

static inline void spi_s_reset(uint32_t rst)
{
	if (rst) {
		raw_writel(REG_CLKGEN_RSTN_SPI_S, 0);
	} else {
		raw_writel(REG_CLKGEN_RSTN_SPI_S, 1);
	}
}

static inline void qspi_reset(uint32_t rst)
{
	if (rst) {
		raw_writel(REG_CLKGEN_RSTN_QSPI, 0);
	} else {
		raw_writel(REG_CLKGEN_RSTN_QSPI, 1);
	}
}

static inline void gpio_reset(uint32_t rst)
{
	if (rst) {
		raw_writel(REG_CLKGEN_RSTN_GPIO, 0);
	} else {
		raw_writel(REG_CLKGEN_RSTN_GPIO, 1);
	}
}

static inline void CAN_0_reset(uint32_t rst)
{
	if (rst) {
		raw_writel(REG_CLKGEN_RSTN_CAN_0, 0);
	} else {
		raw_writel(REG_CLKGEN_RSTN_CAN_0, 1);
	}
}

static inline void CAN_1_reset(uint32_t rst)
{
	if (rst) {
		raw_writel(REG_CLKGEN_RSTN_CAN_1, 0);
	} else {
		raw_writel(REG_CLKGEN_RSTN_CAN_1, 1);
	}
}

static inline void dma_reset(uint32_t rst)
{
	if (rst) {
		raw_writel(REG_CLKGEN_RSTN_DMA, 0);
	} else {
		raw_writel(REG_CLKGEN_RSTN_DMA, 1);
	}
}

static inline void hw_crc_reset(uint32_t rst)
{
	if (rst) {
		raw_writel(REG_CLKGEN_RSTN_CRC, 0);
	} else {
		raw_writel(REG_CLKGEN_RSTN_CRC, 1);
	}
}

static inline void timer_reset(uint32_t rst)
{
	if (rst) {
		raw_writel(REG_CLKGEN_RSTN_TIMER, 0);
	} else {
		raw_writel(REG_CLKGEN_RSTN_TIMER, 1);
	}
}

static inline void dmu_reset(uint32_t rst)
{
	if (rst) {
		raw_writel(REG_CLKGEN_RSTN_DMU, 0);
	} else {
		raw_writel(REG_CLKGEN_RSTN_DMU, 1);
	}
}

static inline void pwm_reset(uint32_t rst)
{
	if (rst) {
		raw_writel(REG_CLKGEN_RSTN_PWM, 0);
	} else {
		raw_writel(REG_CLKGEN_RSTN_PWM, 1);
	}
}

static inline int32_t spi_reset(uint32_t id)
{
	int32_t result = 0;
	switch (id) {
		case 0:
			spi_m0_reset(1);
			spi_m0_reset(0);
			break;
		case 1:
			spi_m1_reset(1);
			spi_m1_reset(0);
			break;
		case 2:
			spi_s_reset(1);
			spi_s_reset(0);
			break;
		default: result = -1;
	}
	return result;
}
#endif
