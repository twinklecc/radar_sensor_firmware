#include "embARC_toolchain.h"
#include "embARC_error.h"
//#include "calterah_error.h"

#include "clkgen.h"
#include "radio_ctrl.h"
#include "arc_builtin.h"
#include "arc.h"
#include "arc_exception.h"
#include "alps_clock_reg.h"
#include "alps_clock.h"
#include "alps_module_list.h"
#include "alps_timer.h"
#include "embARC_debug.h"


int32_t clock_select_source(clock_source_t clk_src, clock_source_t sel)
{
	int32_t result = E_OK;

	uint32_t reg_addr = 0;

	switch (clk_src) {
		case SYSTEM_REF_CLOCK:
			reg_addr = REG_CLKGEN_SEL_400M;
			if (PLL0_CLOCK == sel) {
				raw_writel(reg_addr, 1);
			} else {
				raw_writel(reg_addr, 0);
			}
			break;

		case CPU_REF_CLOCK:
			reg_addr = REG_CLKGEN_SEL_300M;
			if (PLL1_CLOCK == sel) {
				raw_writel(reg_addr, 1);
			} else {
				raw_writel(reg_addr, 0);
			}
			break;
		default:
			result = E_PAR;
	}

	return result;
}

int32_t clock_divider(clock_source_t clk_src, uint32_t div)
{
	int32_t result = E_OK;

	if (div && !(div & 1)) {
		div -= 1;
	}

	switch (clk_src) {
		case APB_REF_CLOCK:
			raw_writel(REG_CLKGEN_DIV_APB_REF, div);
			break;
		case CAN0_CLOCK:
			raw_writel(REG_CLKGEN_DIV_CAN_0, div);
			break;
		case CAN1_CLOCK:
			raw_writel(REG_CLKGEN_DIV_CAN_1, div);
			break;
		case APB_CLOCK:
			raw_writel(REG_CLKGEN_DIV_APB, div);
			break;
		case AHB_CLOCK:
			raw_writel(REG_CLKGEN_DIV_AHB, div);
			break;
		case CPU_CLOCK:
			raw_writel(REG_CLKGEN_DIV_CPU, div);
			break;
		case MEM_CLOCK:
		case ROM_CLOCK:
		case RAM_CLOCK:
			raw_writel(REG_CLKGEN_DIV_MEM, div);
			break;
		default:
			result = E_PAR;
	}

	return result;
}

int32_t clock_frequency(clock_source_t clk_src)
{
	int32_t result = E_OK;

	uint32_t val = 0;

	switch (clk_src) {
		case SYSTEM_REF_CLOCK:
			if (raw_readl(REG_CLKGEN_SEL_400M)) {
				result = PLL0_OUTPUT_CLOCK_FREQ;
			} else {
				result = XTAL_CLOCK_FREQ;
			}
			break;
		case CPU_REF_CLOCK:
			if (raw_readl(REG_CLKGEN_SEL_300M)) {
				result = PLL1_OUTPUT_CLOCK_FREQ;
			} else {
				result = XTAL_CLOCK_FREQ;
			}
			break;
		case PLL0_CLOCK:
			result = PLL0_OUTPUT_CLOCK_FREQ;
			break;
		case PLL1_CLOCK:
			result = PLL1_OUTPUT_CLOCK_FREQ;
			break;

		case UART0_CLOCK:
		case UART1_CLOCK:
		case I2C_CLOCK:
		case SPI_M0_CLOCK:
		case SPI_M1_CLOCK:
		case SPI_S_CLOCK:
		case QSPI_CLOCK:
		case XIP_CLOCK:
		case APB_REF_CLOCK:
			val = raw_readl(REG_CLKGEN_DIV_APB_REF) & 0xF;
			result = clock_frequency(SYSTEM_REF_CLOCK);
			if (result > 0) {
				if (val) {
					result /= (val >> 1) << 1;
				}
			}
			break;

		//case GPIO_CLOCK:
		case TIMER_CLOCK:
		//case DMU_CLOCK:
		case PWM_CLOCK:
		case APB_CLOCK:
			val = raw_readl(REG_CLKGEN_DIV_AHB) & 0xF;
			result = clock_frequency(SYSTEM_REF_CLOCK);
			if (result > 0) {
#ifdef CHIP_ALPS_B
				if (val) {
					result /= (val + 1);
				}
#endif
				val = raw_readl(REG_CLKGEN_DIV_APB) & 0xF;
				if (val) {
					result /= (val + 1);
				}
			}
			break;

		case BB_CLOCK:
		case CRC_CLOCK:
		case AHB_CLOCK:
			val = raw_readl(REG_CLKGEN_DIV_AHB) & 0xF;
			result = clock_frequency(SYSTEM_REF_CLOCK);
			if (result > 0) {
				if (val) {
					result /= (val + 1);
				}
			}
			break;

		case CPU_CLOCK:
			val = raw_readl(REG_CLKGEN_DIV_CPU) & 0xF;
			result = clock_frequency(CPU_REF_CLOCK);
			if (result > 0) {
				if (val) {
					result /= (val + 1);
				}
			}
			break;
		/*
		case MEM_CLOCK:
		case ROM_CLOCK:
		case RAM_CLOCK:
			val = raw_readl(REG_CLKGEN_DIV_MEM) & 0xF;
			result = clock_frequency(CPU_WORK_CLOCK);
			if (result > 0) {
				if (val) {
					result /= (val + 1);
				}
			}
			break;
		*/
		case RC_BOOT_CLOCK:
			result = RC_BOOT_CLOCK_FREQ;
			break;
		case XTAL_CLOCK:
			result = XTAL_CLOCK_FREQ;
			break;
		case CAN0_CLOCK:
			val = raw_readl(REG_CLKGEN_DIV_CAN_0) & 0xF;
			result = clock_frequency(SYSTEM_REF_CLOCK);
			if (result > 0) {
				if (val) {
					result /= (val + 1);
				}
			}
			break;
		case CAN1_CLOCK:
			val = raw_readl(REG_CLKGEN_DIV_CAN_1) & 0xF;
			result = clock_frequency(SYSTEM_REF_CLOCK);
			if (result > 0) {
				if (val) {
					result /= (val + 1);
				}
			}
			break;
		case DAC_OUT_CLOCK:
		default:
			result = E_PAR;
	}

	return result;
}

void system_clock_init(void)
{
	uint32_t pll_ready = raw_readl(REG_CLKGEN_READY_PLL);
	if (0 == pll_ready) {
		/* PLL on. */
#ifndef PLAT_ENV_FPGA
		fmcw_radio_pll_clock_en();
#endif
	} else {
		fmcw_radio_clk_out_for_cascade();
	}

	pll_ready = raw_readl(REG_CLKGEN_READY_PLL);
	if (pll_ready > 0) {
		raw_writel(REG_CLKGEN_DIV_AHB, 1); // AHB should firstly be set.
		raw_writel(REG_CLKGEN_DIV_APB, 3); // APB should be set after AHB
		raw_writel(REG_CLKGEN_DIV_CAN_0, 3);//set can_clk=400/4=100MHz
		raw_writel(REG_CLKGEN_DIV_CAN_1, 3);
		raw_writel(REG_CLKGEN_SEL_300M, 1);
		raw_writel(REG_CLKGEN_SEL_400M, 1);
		set_current_cpu_freq(PLL1_OUTPUT_CLOCK_FREQ);
	}else {
		set_current_cpu_freq(XTAL_CLOCK_FREQ);
	}
 }


void bus_clk_div(uint32_t div)
{
	clock_divider(AHB_CLOCK, div);
}

void apb_clk_div(uint32_t div)
{
	clock_divider(APB_CLOCK, div);
}

void apb_ref_clk_div(uint32_t div)
{
	clock_divider(APB_REF_CLOCK, div);
}

void switch_gpio_out_clk(bool ctrl)
{
	if (ctrl) {
            gpio_out_enable(1);
	} else {
            gpio_out_enable(0);
	}
}
