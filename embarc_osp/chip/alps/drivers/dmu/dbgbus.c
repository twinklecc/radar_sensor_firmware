#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "mux.h"
#include "embARC.h"
#include "embARC_debug.h"

#include "alps_module_list.h"
#include "alps_dmu_reg.h"
#include "alps_dmu.h"
#include "baseband_hw.h"
#include "gpio_hal.h"

#define DATA_DUMP_DONE 21
#define DATA_DUMP_RESET 22
#define HIL_INPUT_READY 23

/* dbgbus raw operating methods. */
void dbgbus_input_config(void)
{
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_VAL_OEN_OFFSET, DBGBUS_OUTPUT_DISABLE);
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_DAT_OEN_OFFSET, DBGBUS_DAT_DISABLE);
        dbgbus_enable(1); /* enable dbgbus clock */
}

void dbgbus_dump_enable(uint8_t dbg_src)
{
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_SRC_OFFSET, dbg_src);
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_VAL_OEN_OFFSET, DBGBUS_OUTPUT_ENABLE);
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_DAT_OEN_OFFSET, DBGBUS_DAT_ENABLE);
}

void lvds_dump_reset(void)
{
        gpio_write(DATA_DUMP_RESET, 0);
        gpio_write(DATA_DUMP_RESET, 1);
        gpio_write(DATA_DUMP_RESET, 0);
}

void lvds_dump_done(void)
{
        gpio_write(DATA_DUMP_DONE, 0);
        gpio_write(DATA_DUMP_DONE, 1);
        gpio_write(DATA_DUMP_DONE, 0);
}

/* gpio_23, ready signal of hil */
/* ready signal should be sent to FPGA at the beginning of every hil frame */
void dbgbus_hil_ready(void)
{
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_SRC_OFFSET, DBG_SRC_CPU);
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_DAT_OEN_OFFSET, DBGBUS_DAT_23_OEN);
        /* Only 1 posedge is needed */
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_DOUT_OFFSET, DBGBUS_DAT_RESET);   /* write 0 */
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_DOUT_OFFSET, DBGBUS_DAT_23_MASK); /* write 1 */
}

/* gpio_22, reset signal */
/* reset signal should be sent to FPGA at the beginning of every data collection frame */
/* reset signal and done signal should be used in pair */
void dbgbus_dump_reset(void)
{
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_SRC_OFFSET, DBG_SRC_CPU);
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_DAT_OEN_OFFSET, DBGBUS_DAT_22_OEN);
        /* Only 1 posedge is needed */
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_DOUT_OFFSET, DBGBUS_DAT_RESET);   /* write 0 */
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_DOUT_OFFSET, DBGBUS_DAT_22_MASK); /* write 1 */
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_DAT_OEN_OFFSET, DBGBUS_DAT_DISABLE);
}

/* gpio_21, done signal */
/* done signal should be sent to FPGA at the end of every data collection frame */
/* reset signal and done signal should be used in pair */
void dbgbus_dump_done(void)
{
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_SRC_OFFSET, DBG_SRC_CPU);
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_DAT_OEN_OFFSET, DBGBUS_DAT_21_OEN);
        /* Only 1 posedge is needed */
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_DOUT_OFFSET, DBGBUS_DAT_RESET);   /* write 0 */
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_DOUT_OFFSET, DBGBUS_DAT_21_MASK); /* write 1 */
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_DAT_OEN_OFFSET, DBGBUS_DAT_DISABLE);
}

void dbgbus_dump_disable(void)
{
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_SRC_OFFSET, DBG_SRC_CPU);
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_VAL_OEN_OFFSET, DBGBUS_OUTPUT_DISABLE);
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_DAT_OEN_OFFSET, DBGBUS_DAT_DISABLE);
}

void dbgbus_free_run_enable(void)
{
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_SRC_OFFSET, DBG_SRC_CPU);
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_VAL_OEN_OFFSET, DBGBUS_OUTPUT_DISABLE);
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_DAT_OEN_OFFSET, DBGBUS_DAT_20_OEN);
}

void gpio_free_run_sync(void)
{
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_DOUT_OFFSET, DBGBUS_DAT_20_MASK);
        raw_writel(REL_REGBASE_DMU + REG_DMU_DBG_DOUT_OFFSET, DBGBUS_DAT_RESET);
}

void dbgbus_dump_start( uint8_t dbg_src)
{
        /* gpio config */
        io_mux_dbgbus_dump();
        dbgbus_dump_reset(); /* reset fpga board */
        dbgbus_dump_enable(dbg_src);

        dbgbus_enable(1); /* enable dbgbus clock, !! this should be at the last line of this function !! */
}

void dbgbus_dump_stop(void)
{
        dbgbus_enable(0); /* disable dbgbus clock */
        dbgbus_dump_done();
        io_mux_dbgbus_mode_stop();
}

#ifdef CHIP_CASCADE
// Note, debugbus(gpio) 5, 6, 12 are multiplexed with cascade control signals
// cascade_irq -- gpio_dat_5, 6, fmcw_start -- gpio_dat_12
// when  data dump, these 3 pins should be switched to dbgbus
// after data dump, these 3 pins should be switched to cascade control immediately.

void dbgbus_dump_cascade_switch(bool enable)
{
        if (enable) {
                io_mux_casade_irq_disable();
                io_mux_fmcw_start_sel_disable();
                dbgbus_enable(1); /* enable dbgbus clock */
        } else {
                dbgbus_enable(0); /* disable dbgbus clock firstly to avoid abnormal signal */
                io_mux_casade_irq();
                io_mux_fmcw_start_sel();
        }
}
#endif

