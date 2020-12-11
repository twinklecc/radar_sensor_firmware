#ifndef _HW_CRC_H_
#define _HW_CRC_H_
/*
#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC_debug.h"
#include "arc_exception.h"
*/
#include "crc_reg.h"

typedef enum {
	CRC_POLY_32C93 = 0,
	CRC_POLY_32,
	CRC_POLY_16_CCITT,
	CRC_POLY_8_AUTOSAR,
	CRC_POLY_8_SAE
} crc_poly_t;

typedef enum {
	INT_HW_CRC_COMPLETE = 0,
	INT_HW_CRC_FAIL,
	INT_HW_CRC_SUCCESS
} hw_crc_int_type_t;

typedef struct {
	uint32_t base;
	uint8_t err_int;
	uint8_t complete_int;
} hw_crc_t;

static inline void hw_crc_mode(crc_poly_t poly, uint32_t mode)
{
	uint32_t val = 0;
	val = (poly & BITS_CRC_POLY_MASK) << BITS_CRC_POLY_SHIFT;
	val |= (mode & BITS_CRC_MODE_MASK) << BITS_CRC_MODE_SHIFT;
	raw_writel(REL_REGBASE_CRC + REG_CRC_MODE, val);
}

static inline void hw_crc_count(uint32_t count)
{
	raw_writel(REL_REGBASE_CRC + REG_CRC_COUNT, count);
}

static inline uint32_t hw_crc_count_get(void)
{
	return raw_readl(REL_REGBASE_CRC + REG_CRC_COUNT);
}

static inline void hw_crc_raw_data(uint32_t data)
{
	raw_writel(REL_REGBASE_CRC + REG_CRC_RAW, data);
}

static inline void hw_crc_init_value(uint32_t value)
{
	raw_writel(REL_REGBASE_CRC + REG_CRC_INIT, value);
}

static inline uint32_t hw_crc_init_value_get(void)
{
	return raw_readl(REL_REGBASE_CRC + REG_CRC_INIT);
}

static inline void hw_crc_pre(uint32_t value)
{
	raw_writel(REL_REGBASE_CRC + REG_CRC_PRE, value);
}

static inline void hw_crc_interrupt_en(hw_crc_int_type_t type, uint32_t en)
{
	uint32_t val = raw_readl(REL_REGBASE_CRC + REG_CRC_INTR_EN);
	if (en) {
		val |= (1 << type);
	} else {
		val &= ~(1 << type);
	}
	raw_writel(REL_REGBASE_CRC + REG_CRC_INTR_EN, val);
}

static inline void hw_crc_interrupt_clear(hw_crc_int_type_t type)
{
	raw_writel(REL_REGBASE_CRC + REG_CRC_INTR_CLR, (1 << type));
}

static inline uint32_t hw_crc_interrupt_status(void)
{
	return raw_readl(REL_REGBASE_CRC + REG_CRC_INTR_STA);
}

static inline uint32_t hw_crc_sector_value(void)
{
	return raw_readl(REL_REGBASE_CRC + REG_CRC_SEC);
}

#define INT_HW_CRC_COMPLETED(status)	(((status) & (1 << INT_HW_CRC_COMPLETE)) ? (1) : (0))
#define INT_HW_CRC_FAILED(status)	(((status) & (1 << INT_HW_CRC_FAIL)) ? (1) : (0))
#define INT_HW_CRC_SUCCESS(status)	(((status) & (1 << INT_HW_CRC_SECCESS)) ? (1) : (0))

#define HW_CRC_WAIT_COMPLETED()		while (0 == (hw_crc_interrupt_status() & (1 << INT_HW_CRC_COMPLETE)))
#endif
