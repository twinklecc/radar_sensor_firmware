#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "mux.h"
#include "embARC.h"
#include "embARC_debug.h"

#include "alps_module_list.h"
#include "alps_io_mux.h"
#include "alps_dmu.h"

/* IO-MUX. */
static io_mux_t io_mux_info[] = {
#include "pin_table.h"
};

static uint32_t old_dmu_sel = 0;
void io_mux_init(void)
{
	int i = 0;

	for (; i < sizeof(io_mux_info)/sizeof(io_mux_t); i++) {
		raw_writel(io_mux_info[i].reg_offset + REL_REGBASE_DMU, \
				io_mux_info[i].func);
	}

	/* select gpio usage: using for gpio */
	sys_dmu_select(SYS_DMU_SEL_GPIO);
}


int8_t io_get_dmumode(void)
{
	int8_t result = 0;

	uint32_t m  = sys_dmu_mode_get();
	if (m != SYS_DMU_SEL_GPIO) {
		result = -1;
	}

	return result;
}

void io_sel_dmumode(uint8_t m)
{
	if (m == SYS_DMU_SEL_DBG) {
		sys_dmu_select(SYS_DMU_SEL_DBG);
	} else {
		sys_dmu_select(SYS_DMU_SEL_GPIO);
	}
}


/* gpio_21 ~ gpio_24 switch */
void io_mux_lvds_dump(void)
{
	/* select gpio usage: using for debug. */
	old_dmu_sel = sys_dmu_mode_get();
	sys_dmu_select(SYS_DMU_SEL_DBG);
	io_mux_spi_m0_func_sel(IO_MUX_FUNC4);
}

void io_mux_dbgbus_dump(void)
{

	/* select gpio usage: using for debug. */
	old_dmu_sel = sys_dmu_mode_get();

	sys_dmu_select(SYS_DMU_SEL_DBG);

        /* gpio_0 ~ gpio20 and gpio_valid. */
	io_mux_can_clk_func_sel(IO_MUX_FUNC4);
	io_mux_pwm0_func_sel(IO_MUX_FUNC4);
	io_mux_pwm1_func_sel(IO_MUX_FUNC4);
	io_mux_i2c_func_sel(IO_MUX_FUNC4);
	io_mux_can1_func_sel(IO_MUX_FUNC4);
	io_mux_uart1_func_sel(IO_MUX_FUNC4);
	io_mux_spi_s1_func_sel(IO_MUX_SPI_S1_FUNC_MIX(IO_MUX_FUNC4, \
				IO_MUX_FUNC4, IO_MUX_FUNC4, IO_MUX_FUNC4));
	io_mux_spi_m1_func_sel(IO_MUX_FUNC4);
	io_mux_spi_s0_func_sel(IO_MUX_FUNC4);
	io_mux_spi_m0_func_sel(IO_MUX_FUNC4); /* gpio_21 ~ gpio_24 */

	/* debug data of cfar and bfm is 20 bits*/
}

void io_mux_dbgbus_mode_stop(void)
{
        /* stop gpio usage */
	sys_dmu_select(old_dmu_sel);
}

void io_mux_free_run(void)
{
        /* CAN1_RX and SYNC are used */
	io_mux_can1_func_sel(IO_MUX_FUNC4);
	io_mux_adc_sync_func_sel(IO_MUX_FUNC0);
}

void io_mux_free_run_disable()
{
        /* CAN1_RX and SYNC are used */
	io_mux_can1_func_sel(IO_MUX_FUNC4);
	io_mux_adc_sync_func_sel(IO_MUX_FUNC4);
}

#ifdef CHIP_CASCADE

void io_mux_adc_sync(void)
{
	io_mux_adc_sync_func_sel(IO_MUX_FUNC0);
}

void io_mux_adc_sync_disable(void)
{
	io_mux_adc_sync_func_sel(IO_MUX_FUNC4);
}

void io_mux_casade_irq(void)
{
	io_mux_can1_func_sel(IO_MUX_FUNC4);
	sys_dmu_select(SYS_DMU_SEL_GPIO);
}

void io_mux_casade_irq_disable(void)
{
	sys_dmu_select(SYS_DMU_SEL_DBG);
}

#endif
