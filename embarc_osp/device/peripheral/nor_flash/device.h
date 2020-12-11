#ifndef _DEVICE_H_
#define _DEVICE_H_

#define EXT_FLASH_REGION_CNT_MAX	(4)

typedef enum external_nor_flash_status {
	FLASH_WIP = 0,
	FLASH_WEL,
	FLASH_BP,
	FLASH_BP0 = FLASH_BP,
	FLASH_BP1,
	FLASH_BP2,
	FLASH_E_ERR,
	FLASH_P_ERR,
	FLASH_STSR_WD,
	FLASH_P_SUSPEND,
	FLASH_E_SUSPEND,
	FLASH_INVALID_STS
} flash_sts_t;

typedef struct flash_region_description {
	uint32_t base;
	uint32_t size;

	uint32_t sector_size;
	uint32_t sec_erase_ins;
	uint32_t erase_type_mask;
} flash_region_t;

#define DEF_FLASH_REGION(r_base, r_size, sec_size, sec_e_ins) {\
		.base = (r_base), .size = (r_size),\
		.sector_size = (sec_size), .sec_erase_ins = (sec_e_ins)\
	}

typedef struct flash_device_operation {
	int32_t (*quad_entry)(dw_ssi_t *dw_ssi);
	int32_t (*quad_exit)(dw_ssi_t *dw_ssi);
	int32_t (*qpi_entry)(dw_ssi_t *dw_ssi);
	int32_t (*qpi_exit)(dw_ssi_t *dw_ssi);

	int32_t (*status)(dw_ssi_t *dw_ssi, uint32_t sts_type);
} flash_dev_ops_t;

typedef struct flash_device {
	uint32_t total_size;
	uint32_t page_size;

	flash_region_t m_region[EXT_FLASH_REGION_CNT_MAX];

	flash_dev_ops_t *ops;
} flash_device_t;

typedef struct flash_xip_parameters {
	uint32_t rd_sts_cmd;
	uint32_t rd_cmd;
	uint32_t wr_en_cmd;
	uint32_t program_cmd;
	uint32_t suspend_cmd;
	uint32_t resume_cmd;

	uint32_t wip_pos;
	uint32_t xsb;
	uint32_t suspend_wait;
} xip_param_t;

static inline uint32_t flash_dev_find_region(flash_region_t *region, uint32_t addr)
{
	uint32_t i;

	for (i = 0; i < EXT_FLASH_REGION_CNT_MAX; i++) {
		if ((addr >= region[i].base) && \
			(addr < (region[i].base + region[i].size))) {
			break;
		}
	}

	return i;
}

static inline void flash_region_fill(flash_region_t *region, \
		uint32_t base, uint32_t size, uint32_t sec_size, uint32_t sec_e_ins)
{
	region->base = base;
	region->size = size;
	region->sector_size = sec_size;
	region->sec_erase_ins = sec_e_ins;
}

flash_device_t *get_flash_device(dw_ssi_t *dw_ssi);
xip_param_t *flash_xip_param_get(void);
#endif
