#ifndef _CRC_REG_H_
#define _CRC_REG_H_

#define REG_CRC_MODE		(0x0000)
#define REG_CRC_COUNT		(0x0004)
#define REG_CRC_RAW		(0x0008)
#define REG_CRC_INIT		(0x000C)
#define REG_CRC_PRE		(0x0010)
#define REG_CRC_INTR_EN		(0x0014)
#define REG_CRC_INTR_CLR	(0x0018)
#define REG_CRC_INTR_STA	(0x001C)
#define REG_CRC_SEC		(0x0020)

#define BITS_CRC_POLY_MASK	(0x7)
#define BITS_CRC_POLY_SHIFT	(0)
#define BITS_CRC_MODE_MASK	(0x3)
#define BITS_CRC_MODE_SHIFT	(8)

#endif
