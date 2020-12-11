#ifndef _CHIP_CLOCK_H_
#define _CHIP_CLOCK_H_

#include "alps_clock_reg.h"

#define CLOCK_10MHZ			(10000000)
#define CLOCK_20MHZ			(20000000)
#define CLOCK_40MHZ			(40000000)
#define CLOCK_50MHZ			(50000000)
#define CLOCK_100MHZ			(100000000)

#ifdef PLAT_ENV_FPGA
#define RC_BOOT_CLOCK_FREQ		(10000000UL)
#define XTAL_CLOCK_FREQ			(10000000UL)
#define PLL0_OUTPUT_CLOCK_FREQ		(10000000UL)
#define PLL1_OUTPUT_CLOCK_FREQ		(10000000UL)
#else
#define RC_BOOT_CLOCK_FREQ		(50000000UL)
#define XTAL_CLOCK_FREQ			(50000000UL)
#define PLL0_OUTPUT_CLOCK_FREQ		(400000000UL)
#define PLL1_OUTPUT_CLOCK_FREQ		(300000000UL)
#endif

#define BUS_CLK_200M                    1
#define BUS_CLK_100M                    3
#define BUS_CLK_50M                     7
#define APB_DIV_DISABLE                 0
#define APB_DIV_2                       1
#define APB_DIV_4                       3

#define CLK_ON                          1
#define CLK_OFF                         0

typedef enum alps_b_clock_node {
	RC_BOOT_CLOCK = 0,
	XTAL_CLOCK,
	PLL0_CLOCK,
	PLL1_CLOCK,
	CPU_REF_CLOCK,
	SYSTEM_REF_CLOCK,
	APB_REF_CLOCK,
	APB_CLOCK,
	AHB_CLOCK,
	CPU_CLOCK,
	MEM_CLOCK,

	ROM_CLOCK,
	RAM_CLOCK,
	CAN0_CLOCK,
	CAN1_CLOCK,
	XIP_CLOCK,
	BB_CLOCK,
	UART0_CLOCK,
	UART1_CLOCK,
	I2C_CLOCK,
	SPI_M0_CLOCK,
	SPI_M1_CLOCK,
	SPI_S_CLOCK,
	QSPI_CLOCK,
	GPIO_CLOCK,
	DAC_OUT_CLOCK,
	CRC_CLOCK,
	TIMER_CLOCK,
	DMU_CLOCK,
	PWM_CLOCK,
	CLOCK_SOURCE_INVALID = 0xFF
} clock_source_t;

static inline void xip_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_FLASH_CTRL, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_FLASH_CTRL, 0);
	}
}

static inline void bb_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_BB, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_BB, 0);
	}
}

static inline void uart0_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_UART_0, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_UART_0, 0);
	}
}

static inline void uart1_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_UART_1, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_UART_1, 0);
	}
}

static inline void i2c_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_I2C, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_I2C, 0);
	}
}

static inline void spi_m0_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_SPI_M0, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_SPI_M0, 0);
	}
}

static inline void spi_m1_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_SPI_M1, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_SPI_M1, 0);
	}
}

static inline void spi_s_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_SPI_S, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_SPI_S, 0);
	}
}

static inline int32_t spi_enable(uint32_t id, uint32_t en)
{
	int32_t result = 0;
	switch (id) {
		case 0: spi_m0_enable(en); break;
		case 1: spi_m1_enable(en); break;
		case 2: spi_s_enable(en); break;
		default: result = -1;
	}
	return result;
}

static inline void qspi_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_QSPI, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_QSPI, 0);
	}
}

static inline void gpio_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_GPIO, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_GPIO, 0);
	}
}

static inline void can0_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_CAN_0, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_CAN_0, 0);
	}
}

static inline void can1_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_CAN_1, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_CAN_1, 0);
	}
}

static inline void dac_out_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_DAC_OUT, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_DAC_OUT, 0);
	}
}

static inline void gpio_out_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_GPIO_OUT, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_GPIO_OUT, 0);
	}
}

static inline void rom_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_ROM, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_ROM, 0);
	}
}

static inline void ram_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_RAM, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_RAM, 0);
	}
}

static inline void crc_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_CRC, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_CRC, 0);
	}
}

static inline void timer_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_TIMER, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_TIMER, 0);
	}
}

static inline void dmu_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_DMU, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_DMU, 0);
	}
}

static inline void pwm_enable(uint32_t en)
{
	if (en) {
		raw_writel(REG_CLKGEN_ENA_PWM, 1);
	} else {
		raw_writel(REG_CLKGEN_ENA_PWM, 0);
	}
}

static inline clock_source_t uart_clock_source(uint32_t id)
{
	clock_source_t clk_src;

	switch (id) {
		case 0:
			clk_src = UART0_CLOCK;
			break;
		case 1:
			clk_src = UART1_CLOCK;
			break;
		default:
			clk_src = CLOCK_SOURCE_INVALID;
			break;
	}

	return clk_src;
}
#endif

