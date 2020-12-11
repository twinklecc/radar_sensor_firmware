#ifndef _CFI_H_
#define _CFI_H_

#include "embARC_toolchain.h"

#define CFI_QUERY_CMD			(0x98)
#define CFI_QUERY_ADDR			(0x55)

/* cfi query identification string. */
#define CFI_QUERY_ID_STRING_ADDR	(0x10)
#define CFI_QUERY_ID_STRING		"QRY"
#define CFI_QUERY_ID_STRING_FIELD_LEN	(11)

typedef struct cfi_query_identification {
	char unique_str[3];
	uint8_t primary_algorithm[4];
	uint8_t alternative_algorithm[4];
} cfi_query_id_t;


/* cfi query system interface information. */
#define CFI_QUERY_SYS_IF_INFO_ADDR	(0x1B)
#define CFI_QUERY_SYS_IF_INFO_LEN	(12)

typedef struct cfi_query_sys_if_info {
	uint8_t pew_vcc_min;
	uint8_t pew_vcc_max;
	uint8_t pe_vpp_min;
	uint8_t pe_vpp_max;
	uint8_t single_program_timeout;
	uint8_t multi_program_timeout;
	uint8_t ind_block_erase_timeout;
	uint8_t chip_erase_timeout;
	uint8_t single_program_max_timeout;
	uint8_t multi_program_max_timeout;
	uint8_t ind_block_erase_max_timeout;
	uint8_t chip_erase_max_timeout;
} cfi_sys_if_info_t;

/* cfi query device geometry definition. */
#define CFI_DEV_GEOMETRY_DEF_OFFSET	(0x27)
#define CFI_DEV_GEOMETRY_DEF_LEN	(14)
#define CFI_ERASE_BLOCK_REGION_OFFSET	(0x2D)

typedef struct cfi_device_geometry_definition {
	uint8_t device_size;
	uint8_t device_if_code[2];
	uint8_t max_program_size[2];
	uint8_t num_erase_block_region;
	uint8_t erase_block_region_info[4];
} cfi_device_geometry_t;


int32_t flash_cfi_detect(dw_ssi_t *dw_ssi);
int32_t flash_cfi_param_read(dw_ssi_t *dw_ssi, flash_device_t *device);

#endif
