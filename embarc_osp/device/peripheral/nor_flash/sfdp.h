#ifndef _SFDP_H_
#define _SFDP_H_

#include "embARC_toolchain.h"

/*
 * @signature: a constant value, 0x50444653(P-D-F-S).
 * @param_header_number: number of parameter headers.
 * */
typedef struct {
	uint32_t signature;
	uint8_t minor_reversion;
	uint8_t major_reversion;
	uint8_t param_header_number;
	uint8_t byte_reserved;
} sfdp_header_t;

/*
 * @ld_lsb: parameter id lsb.
 * @ld_msb: parameter id msb.
 * @param_table_pointer[3]: parameter table pointer
 *   equal to (([2] << 16) | ([1] << 8) | [0]).
 * */
typedef struct {
	uint8_t id_lsb;
	uint8_t minor_reversion;
	uint8_t major_reversion;
	uint8_t param_length;
	uint8_t param_table_pointer[3];
	uint8_t id_msb;
} sfdp_param_header_t;

#define SFDP_PTID_BASIC_SPI_PROTOCOL	(0xFF00U)
#define SFDP_PTID_SECTOR_MAP		(0xFF81U)
#define SFDP_PTID_RPMC			(0xFF03U)
#define SFDP_PTID_4BYTE_INS_TABLE	(0xFF84U)


#define SFDP_READ_MSB_FISRT	(0)
#if SFDP_READ_MSB_FIRST
#define SFDP_PARAMETER_DWORD(paramter, n)	(\
		(parameter[((n) << 2)] << 24) | (parameter[((n) << 2) + 1] << 16) |\
		(parameter[((n) << 2) + 2] << 8) | (parameter[((n) << 2) + 3]))
#else
#define SFDP_PARAMETER_DWORD(parameter, n)	(\
		(parameter[((n) << 2)]) | (parameter[((n) << 2) + 1] << 8) |\
		(parameter[((n) << 2) + 2] << 16) | (parameter[((n) << 2) + 3] << 24))
#endif

/*
 * parse basic flash parameter.
 *
 * */
#define FLASH_QOR_FAST_READ		(1 << 22)
#define FLASH_QIO_FAST_READ		(1 << 21)
#define FLASH_DIO_FAST_READ		(1 << 20)
#define FLASH_DTR_CLOCKING		(1 << 19)
#define FLASH_ADDRESS_BYTE_MASK		(0x3)
#define FLASH_ADDRESS_BYTE_SHIFT	(17)
#define FLASH_DOR_FAST_READ		(1 << 16)
#define FLASH_4KB_ERASE_INSTRUCT_MASK	(0xFF)
#define FLASH_4KB_ERASE_INSTRUCT_SHIFT	(8)
#define FLASH_WE_INSTRUCT_SELECT	(1 << 4)
#define FLASH_VSR_BP			(1 << 3)
#define FLASH_WRITE_GRANULARITY		(1 << 2)
#define FLASH_BLOCK_SECTOR_ERASE_SIZE	(0x3)

static inline int flash_qor_read_supported(uint8_t *parameter)
{
	int ret = 0;

	uint32_t dword1 = SFDP_PARAMETER_DWORD(parameter, 0);
	if (dword1 & FLASH_QOR_FAST_READ) {
		ret = 1;
	}

	return ret;
}
static inline int flash_qio_read_supported(uint8_t *parameter)
{
	int ret = 0;

	uint32_t dword1 = SFDP_PARAMETER_DWORD(parameter, 0);
	if (dword1 & FLASH_QIO_FAST_READ) {
		ret = 1;
	}

	return ret;
}
static inline int flash_dtr_clocking_supported(uint8_t *parameter)
{
	int ret = 0;

	uint32_t dword1 = SFDP_PARAMETER_DWORD(parameter, 0);
	if (dword1 & FLASH_DTR_CLOCKING) {
		ret = 1;
	}

	return ret;
}
static inline int flash_volatile_register_we_select(uint8_t *parameter)
{
	int ret = 0;

	uint32_t dword1 = SFDP_PARAMETER_DWORD(parameter, 0);
	if (dword1 & FLASH_WE_INSTRUCT_SELECT) {
		ret = 1;
	}

	return ret;
}

static inline uint32_t flash_total_size(uint8_t *parameter)
{
	uint32_t size = 0;
	uint32_t dword2 = SFDP_PARAMETER_DWORD(parameter, 1) + 1;

	if (dword2 & (1 << 31)) {
		size = (dword2 & 0x7FFFFFFFU) - 3;
		if (size < 32) {
			size = (1 << size);
		} else {
			size = size | (1 << 31);
		}
	} else {
		size = dword2 >> 3;
	}

	return size;
}

/* dword3: */
#define FLASH_QOR_FREAD_INS_MASK	(0xFF)
#define FLASH_QOR_FREAD_INS_SHIFT	(24)
#define FLASH_QOR_FREAD_MODE_CLK_MASK	(0x7)
#define FLASH_QOR_FREAD_MODE_CLK_SHIFT	(21)
#define FLASH_QOR_FREAD_DUMMY_CLK_MASK	(0x1F)
#define FLASH_QOR_FREAD_DUMMY_CLK_SHIFT	(16)
#define FLASH_QIO_FREAD_INS_MASK	(0xFF)
#define FLASH_QIO_FREAD_INS_SHIFT	(8)
#define FLASH_QIO_FREAD_MODE_CLK_MASK	(0x7)
#define FLASH_QIO_FREAD_MODE_CLK_SHIFT	(5)
#define FLASH_QIO_FREAD_DUMMY_CLK_MASK	(0x1F)
#define FLASH_QIO_FREAD_DUMMY_CLK_SHIFT	(0)

/* dword4: */
#define FLASH_DIO_FREAD_INS_MASK	(0xFF)
#define FLASH_DIO_FREAD_INS_SHIFT	(24)
#define FLASH_DIO_FREAD_MODE_CLK_MASK	(0x7)
#define FLASH_DIO_FREAD_MODE_CLK_SHIFT	(21)
#define FLASH_DIO_FREAD_DUMMY_CLK_MASK	(0x1F)
#define FLASH_DIO_FREAD_DUMMY_CLK_SHIFT	(16)
#define FLASH_DOR_FREAD_INS_MASK	(0xFF)
#define FLASH_DOR_FREAD_INS_SHIFT	(8)
#define FLASH_DOR_FREAD_MODE_CLK_MASK	(0x7)
#define FLASH_DOR_FREAD_MODE_CLK_SHIFT	(5)
#define FLASH_DOR_FREAD_DUMMY_CLK_MASK	(0x1F)
#define FLASH_DOR_FREAD_DUMMY_CLK_SHIFT	(0)

/* dword5: */
#define FLASH_QPI_FAST_READ		(1 << 4)
#define FLASH_DPI_FAST_READ		(1 << 0)

/* dword6: */
#define FLASH_DPI_FREAD_INS_MASK	(0xFF)
#define FLASH_DPI_FREAD_INS_SHIFT	(24)
#define FLASH_DPI_FREAD_MODE_CLK_MASK	(0x7)
#define FLASH_DPI_FREAD_MODE_CLK_SHIFT	(21)
#define FLASH_DPI_FREAD_DUMMY_CLK_MASK	(0x1F)
#define FLASH_DPI_FREAD_DUMMY_CLK_SHIFT	(16)

/* dword7: */
#define FLASH_QPI_FREAD_INS_MASK	(0xFF)
#define FLASH_QPI_FREAD_INS_SHIFT	(24)
#define FLASH_QPI_FREAD_MODE_CLK_MASK	(0x7)
#define FLASH_QPI_FREAD_MODE_CLK_SHIFT	(21)
#define FLASH_QPI_FREAD_DUMMY_CLK_MASK	(0x1F)
#define FLASH_QPI_FREAD_DUMMY_CLK_SHIFT	(16)

/* dword8: */
#define FLASH_ERASE_TYPE2_INS_MASK	(0xFF)
#define FLASH_ERASE_TYPE2_INS_SHIFT	(24)
#define FLASH_ERASE_TYPE2_SIZE_MASK	(0xFF)
#define FLASH_ERASE_TYPE2_SIZE_SHIFT	(16)
#define FLASH_ERASE_TYPE1_INS_MASK	(0xFF)
#define FLASH_ERASE_TYPE1_INS_SHIFT	(8)
#define FLASH_ERASE_TYPE1_SIZE_MASK	(0xFF)
#define FLASH_ERASE_TYPE1_SIZE_SHIFT	(0)

/* dword9: */
#define FLASH_ERASE_TYPE4_INS_MASK	(0xFF)
#define FLASH_ERASE_TYPE4_INS_SHIFT	(24)
#define FLASH_ERASE_TYPE4_SIZE_MASK	(0xFF)
#define FLASH_ERASE_TYPE4_SIZE_SHIFT	(16)
#define FLASH_ERASE_TYPE3_INS_MASK	(0xFF)
#define FLASH_ERASE_TYPE3_INS_SHIFT	(8)
#define FLASH_ERASE_TYPE3_SIZE_MASK	(0xFF)
#define FLASH_ERASE_TYPE3_SIZE_SHIFT	(0)

inline static uint8_t sfdp_flash_erase_type1_instruct(uint8_t *parameter)
{
       uint32_t dword8 = SFDP_PARAMETER_DWORD(parameter, 7);
       return ((dword8 >> FLASH_ERASE_TYPE1_INS_SHIFT) & FLASH_ERASE_TYPE1_INS_MASK);
}
inline static uint32_t sfdp_flash_erase_type1_size(uint8_t *parameter)
{
       uint32_t dword8 = SFDP_PARAMETER_DWORD(parameter, 7);
       return ((dword8 >> FLASH_ERASE_TYPE1_SIZE_SHIFT) & FLASH_ERASE_TYPE1_SIZE_MASK);
}
inline static uint8_t sfdp_flash_erase_type2_instruct(uint8_t *parameter)
{
       uint32_t dword8 = SFDP_PARAMETER_DWORD(parameter, 7);
       return ((dword8 >> FLASH_ERASE_TYPE2_INS_SHIFT) & FLASH_ERASE_TYPE2_INS_MASK);
}
inline static uint32_t sfdp_flash_erase_type2_size(uint8_t *parameter)
{
       uint32_t dword8 = SFDP_PARAMETER_DWORD(parameter, 7);
       return ((dword8 >> FLASH_ERASE_TYPE2_SIZE_SHIFT) & FLASH_ERASE_TYPE2_SIZE_MASK);
}
inline static uint8_t sfdp_flash_erase_type3_instruct(uint8_t *parameter)
{
       uint32_t dword9 = SFDP_PARAMETER_DWORD(parameter, 8);
       return ((dword9 >> FLASH_ERASE_TYPE3_INS_SHIFT) & FLASH_ERASE_TYPE3_INS_MASK);
}
inline static uint32_t sfdp_flash_erase_type3_size(uint8_t *parameter)
{
       uint32_t dword9 = SFDP_PARAMETER_DWORD(parameter, 8);
       return ((dword9 >> FLASH_ERASE_TYPE3_SIZE_SHIFT) & FLASH_ERASE_TYPE3_SIZE_MASK);
}
inline static uint8_t sfdp_flash_erase_type4_instruct(uint8_t *parameter)
{
        uint32_t dword9 = SFDP_PARAMETER_DWORD(parameter, 8);
	return ((dword9 >> FLASH_ERASE_TYPE4_INS_SHIFT) & FLASH_ERASE_TYPE4_INS_MASK);
}
inline static uint32_t sfdp_flash_erase_type4_size(uint8_t *parameter)
{
       uint32_t dword9 = SFDP_PARAMETER_DWORD(parameter, 8);
       *(volatile uint32_t *)0x7f0000 = dword9;
       return ((dword9 >> FLASH_ERASE_TYPE4_SIZE_SHIFT) & FLASH_ERASE_TYPE4_SIZE_MASK);
}
/* dword10: */


/* dword11: */
#define FLASH_PAGE_SIZE_MASK		(0xF)
#define FLASH_PAGE_SIZE_SHIFT		(4)
static inline uint32_t flash_page_size(uint8_t *parameter)
{
	uint32_t dword11 = SFDP_PARAMETER_DWORD(parameter, 10);
	uint32_t page_size = ((dword11 >> FLASH_PAGE_SIZE_SHIFT) & FLASH_PAGE_SIZE_MASK);
	return (1 << page_size);
}

/* dword15: */
#define FLASH_QER_MASK			(0x7)
#define FLASH_QER_SHIFT			(20)
#define FLASH_ENTER_CONT_MODE_MASK	(0xF)
#define FLASH_ENTER_CONT_MODE_SHIFT	(16)
#define FLASH_EXIT_CONT_MODE_MASK	(0x3F)
#define FLASH_EXIT_CONT_MODE_SHIFT	(10)
#define FLASH_CONT_MODE_SUPPORT		(1 << 9)
#define FLASH_ENTER_QPI_MASK		(0x1F)
#define FLASH_ENTER_QPI_SHIFT		(4)
#define FLASH_EXIT_QPI_MASK		(0xF)
#define FLASH_EXIT_QPI_SHIFT		(0)

typedef enum {
	DEV_NO_QE = 0,
	/* cannot write status registers with 1 byte, or status2 will be clear.  */
	DEV_QE_RSTS2_BIT1_NO_W1BYTE,
	DEV_QE_RSTS1_BIT6,
	DEV_QE_RSTS2_BIT7,
	DEV_QE_RSTS2_BIT1_W1BYTE,
	DEV_QE_RSTS2_BIT1,
	DEV_QE_INVALID
} DEVICE_QE_REQUIREMENT_T;
#define DEV_QPI_QE_PLUS_INS38H		(1 << 0)
#define	DEV_QPI_INS38H			(1 << 1)
#define DEV_QPI_INS35H			(1 << 2)
#define DEV_QPI_W_CONFIGURATION		(1 << 3)
#define DEV_QPI_W_CONFIGURATION_NO_ADDR	(1 << 4)

inline static uint32_t flash_device_quad_enable_requirement(uint8_t *parameter)
{
	uint32_t dword15 = SFDP_PARAMETER_DWORD(parameter, 14);
	uint32_t requirement = ((dword15 >> FLASH_QER_SHIFT) & FLASH_QER_MASK);

	return requirement;
}
inline static uint32_t flash_device_qpi_enable_sequence(uint8_t *parameter)
{
	uint32_t dword15 = SFDP_PARAMETER_DWORD(parameter, 14);
	uint32_t requirement = ((dword15 >> FLASH_ENTER_QPI_SHIFT) & FLASH_ENTER_QPI_MASK);

	return requirement;
}
inline static uint32_t flash_device_qpi_disable_sequence(uint8_t *parameter)
{
	uint32_t dword15 = SFDP_PARAMETER_DWORD(parameter, 14);
	uint32_t requirement = ((dword15 >> FLASH_EXIT_QPI_SHIFT) & FLASH_EXIT_QPI_MASK);

	return requirement;
}

/*
 * sector map parameter.
 *
 * */
typedef struct {
	uint8_t region_index;
	uint8_t erase_type_mask;
	uint32_t region_size;
} flash_region_info_t;

/* CDCD: configuration detection command descriptor. */
#define SFDP_CDCD_READ_DATA_MASK_MASK	(0xFF)
#define SFDP_CDCD_READ_DATA_MASK_SHIFT	(24)
#define SFDP_CDCD_ADDRESS_LENGTH_MASK	(0x3)
#define SFDP_CDCD_ADDRESS_LENGTH_SHIFT	(22)
#define SFDP_CDCD_READ_LATENCY_MASK	(0xF)
#define SFDP_CDCD_READ_LATENCY_SHIFT	(16)
#define SFDP_CDCD_INSTRUCTION_MASK	(0xFF)
#define SFDP_CDCD_INSTRUCTION_SHIFT	(8)
#define SFDP_CDCD_TYPE			(1 << 1)
#define SFDP_CDCD_END_INDICATOR		(1 << 0)

/* CMDH: configuration map descriptor header. */
#define SFDP_CMDH_REGION_COUNT_MASK	(0xFF)
#define SFDP_CMDH_REGION_COUNT_SHIFT	(16)
#define SFDP_CMDH_REGION_ID_MASK	(0xFF)
#define SFDP_CMDH_REGION_ID_SHIFT	(8)
#define SFDP_CMDH_TYPE			(1 << 1)
#define SFDP_CMDH_END_INDICATOR		(1 << 0)

/* region info */
#define SFDP_REGION_SIZE_SHIFT		(8)
#define SFDP_REGION_ERASE_TYPE_MASK	(0xF)

#define SFDP_FLASH_REGION_COUNT_MAX	(8)

typedef enum {
       SFDP_CMD_DESCRIPTOR = 0,
       SFDP_MAP_DESCRIPTOR
} sfdp_descriptor_type_t;
static inline uint32_t sfdp_sector_descriptor_type(uint8_t *param)
{
       uint32_t desc_type = SFDP_CMD_DESCRIPTOR;
       if (param[0] & SFDP_CMDH_TYPE) {
               desc_type = SFDP_MAP_DESCRIPTOR;
       }
       return desc_type;
}

int32_t flash_sfdp_detect(dw_ssi_t *dw_ssi);
int32_t flash_sfdp_param_read(dw_ssi_t *dw_ssi, flash_device_t *device);
#endif
