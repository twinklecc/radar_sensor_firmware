#ifndef _DW_TIMER_REG_H_
#define _DW_TIMER_REG_H_

#define DW_TIMER_N_REG_LOADCOUNT_OFFSET(n)	( (n) * 0x14 )
#define DW_TIMER_N_REG_CURCOUNT_OFFSET(n)	( ((n) * 0x14) + 0x4 )
#define DW_TIMER_N_REG_CONTROL_OFFSET(n)	( ((n) * 0x14) + 0x8 )
#define DW_TIMER_N_REG_NEOI_OFFSET(n)		( ((n) * 0x14) + 0xC )
#define DW_TIMER_N_REG_INT_STS_OFFSET(n)	( ((n) * 0x14) + 0x10 )
#define DW_TIMER_REG_INT_STS_OFFSET		(0xA0)
#define DW_TIMER_REG_EOI_OFFSET			(0xA4)
#define DW_TIMER_REG_RAW_INT_STS_OFFSET		(0xA8)
#define DW_TIMER_REG_VERSION_OFFSET		(0xAC)
#define DW_TIMER_N_REG_LOADCOUNT2_OFFSET(n)	( ((n) * 0x4) + 0xB0 )
#define DW_TIMER_N_REG_PROT_LEVEL_OFFSET(n)	( ((n) * 0x4) + 0xD0 )

/* control reg bits field define. */
#define DW_TIMER_BIT_0N100PWM_EN		(1 << 4)
#define DW_TIMER_BIT_PWM_EN			(1 << 3)
#define DW_TIMER_BIT_INT_MASK			(1 << 2)
#define DW_TIMER_BIT_MODE			(1 << 1)
#define DW_TIMER_BIT_EN				(1 << 0)

#endif
