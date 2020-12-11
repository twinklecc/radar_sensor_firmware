#include "embARC_toolchain.h"
#include "embARC_error.h"

#include "mux.h"
#include "embARC.h"
#include "embARC_debug.h"

#include "alps_module_list.h"
#include "alps_dmu_reg.h"
#include "alps_dmu.h"
#include "baseband_hw.h"
#include "cascade.h"

#ifdef UNIT_TEST
#define UDELAY(us)
#else
#define UDELAY(us)  chip_hw_udelay(us);
#endif

/* ahb bus data write to baseband */
void dmu_hil_ahb_write(uint32_t hil_data)
{
        raw_writel(REL_REGBASE_DMU + REG_DMU_HIL_DAT_OFFSET, hil_data);
}

bool dmu_hil_input_mux(bool input_mux)
{
        bool old = raw_readl(REL_REGBASE_DMU + REG_DMU_HIL_ENA_OFFSET);
        raw_writel(REL_REGBASE_DMU + REG_DMU_HIL_ENA_OFFSET, input_mux); /* 1 -- gpio input; 0 -- ahb input */
        return old;
}

void dmu_adc_reset(void)
{
#ifdef CHIP_CASCADE
        if (chip_cascade_status() == CHIP_CASCADE_MASTER) {
                raw_writel(REL_REGBASE_DMU + REG_DMU_ADC_RSTN_OFFSET, 0); // asserted reset_n
                UDELAY(10);
                raw_writel(REL_REGBASE_DMU + REG_DMU_ADC_RSTN_OFFSET, 1); // deasserted reset_n
        }
#endif // CHIP_CASCADE
}
