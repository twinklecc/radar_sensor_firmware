#include "embARC.h"
#include "apb_lvds.h"
#include "alps_lvds_reg.h"
#include "radio_ctrl.h"

static inline void lvds_raw_write(int addr, int val)
{
        *(volatile int *)(addr)= val;
}

static inline int lvds_raw_read(int addr)
{
        return *(volatile int *)(addr);
}

static void lvds_data_from_bb(uint8_t dump_src)
{
        lvds_raw_write(LVDS_BASE + LENR           , LENR_OFF           );
        lvds_raw_write(LVDS_BASE + BB_DAT_MUX     , dump_src           );
        lvds_raw_write(LVDS_BASE + DUMP_MUX       , DUMP_MUX_BB        );
        lvds_raw_write(LVDS_BASE + OUTPUT_MUX     , OUTPUT_MUX_APB     );
        lvds_raw_write(LVDS_BASE + LVDS_EN        , LVDS_EN_ON         );
        lvds_raw_write(LVDS_BASE + LVDS_FRAME     , LVDS_FRAME_POS     );
        lvds_raw_write(LVDS_BASE + LVDS_BIT_ORDER , LVDS_BIT_ORDER_POS );
        lvds_raw_write(LVDS_BASE + LENR           , LENR_ON            );
}

void lvds_dump_config(uint8_t dump_src)
{
        lvds_enable(1);
        lvds_data_from_bb(dump_src); /* must after lvds clock enable */
}

void lvds_dump_start(uint8_t dump_src)
{
        fmcw_radio_lvds_on(NULL, true);
        lvds_dump_config(dump_src);
        lvds_dump_reset();  /* reset signal to fpga board */
}

void lvds_dump_stop(void)
{
        lvds_raw_write(LVDS_BASE + LENR           , LENR_OFF           );
        lvds_raw_write(LVDS_BASE + BB_DAT_MUX     , BB_MUX_DEF         );
        lvds_raw_write(LVDS_BASE + LENR           , LENR_ON            );
        lvds_enable(0);
        lvds_dump_done();  /* done signal to  fpga board */
}
