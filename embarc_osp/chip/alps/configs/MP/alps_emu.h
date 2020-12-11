#ifndef _ALPS_EMU_H_
#define _ALPS_EMU_H_

#include "alps_emu_reg.h"
#include "alps_reboot_def.h"

static inline uint32_t chip_security(void)
{
	return raw_readl(REG_EMU_SEC_STA);
}

static inline void reboot_cause_set(uint32_t cause)
{
	raw_writel(REG_EMU_SPARED_BASE, cause);
}

static inline uint32_t reboot_cause(void)
{
	return raw_readl(REG_EMU_SPARED_BASE);
}
#endif
