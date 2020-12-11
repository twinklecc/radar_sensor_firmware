#ifndef _DA9061_H_
#define _DA9061_H_

#define REG_PAGE_CON		(0x00)
#define REG_STATUS_A		(0x01)
#define REG_STATUS_B		(0x02)
#define REG_STATUS_D		(0x04)
#define REG_FAULT_LOG		(0x05)

#define REG_IRQ_MASK_A		(0x0A)
#define REG_IRQ_MASK_B		(0x0B)
#define REG_IRQ_MASK_C		(0x0C)

#define REG_CONTROL_A		(0x0E)
#define REG_CONTROL_B		(0x0F)
#define REG_CONTROL_C		(0x10)
#define REG_CONTROL_D		(0x11)
#define REG_CONTROL_E		(0x12)
#define REG_CONTROL_F		(0x13)

#define REG_PD_DIS		(0x14)

#define REG_GPIO_01		(0x15)
#define REG_GPIO_23		(0x16)
#define REG_GPIO_4		(0x17)
#define REG_GPIO_WKUP_MODE	(0x1C)
#define REG_GPIO_MODE		(0x1D)
#define REG_GPIO_OUT0_2		(0x1E)
#define REG_GPIO_OUT3_4		(0x1F)

#define REG_BUCK1_CONT		(0x21)
#define REG_BUCK3_CONT		(0x22)
#define REG_BUCK2_CONT		(0x24)

#define REG_LDO1_CONT		(0x26)
#define REG_LDO2_CONT		(0x27)
#define REG_LDO3_CONT		(0x28)
#define REG_LDO4_CONT		(0x29)

#define REG_DVC_1		(0x32)

#define REG_SEQ			(0x81)
#define REG_SEQ_TIMER		(0x82)
#define REG_ID_2_1		(0x83)
#define REG_ID_4_3		(0x84)
#define REG_ID_12_11		(0x88)
#define REG_ID_14_13		(0x89)
#define REG_ID_16_15		(0x8A)
#define REG_ID_22_21		(0x8D)
#define REG_ID_24_23		(0x8E)
#define REG_ID_26_25		(0x8F)
#define REG_ID_28_27		(0x90)
#define REG_ID_30_29		(0x91)
#define REG_ID_32_31		(0x92)
#define REG_SEQ_A		(0x95)
#define REG_SEQ_B		(0x96)
#define REG_WAIT		(0x97)
#define REG_RESET		(0x99)

#define REG_BUCK_ILIM_A		(0x9A)
#define REG_BUCK_ILIM_B		(0x9B)
#define REG_BUCK_ILIM_C		(0x9C)
#define REG_BUCK1_CFG		(0x9E)
#define REG_BUCK3_CFG		(0x9F)
#define REG_BUCK2_CFG		(0xA0)

#define REG_VBUCK1_A		(0xA4)
#define REG_VBUCK3_A		(0xA5)
#define REG_VBUCK2_A		(0xA7)

#define REG_VLDO1_A		(0xA9)
#define REG_VLDO2_A		(0xAA)
#define REG_VLDO3_A		(0xAB)
#define REG_VLDO4_A		(0xAC)

#define REG_VBUCK1_B		(0xB5)
#define REG_VBUCK3_B		(0xB6)
#define REG_VBUCK2_B		(0xB8)

#define REG_VLDO1_B		(0xBA)
#define REG_VLDO2_B		(0xBB)
#define REG_VLDO3_B		(0xBC)
#define REG_VLDO4_B		(0xBD)

#endif
