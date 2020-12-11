#include "embARC.h"
#include "alps_emu_reg.h"
#include "func_safety.h"

void func_safety_init(func_safety_t *fsm)
{
        raw_writel(REG_EMU_BOOT_DONE, 1); // BB_LBIST dependent on EMU_BOOT_DONE
}

void func_safety_start(func_safety_t *fsm)
{
}

void func_safety_stop(func_safety_t *fsm)
{
}
