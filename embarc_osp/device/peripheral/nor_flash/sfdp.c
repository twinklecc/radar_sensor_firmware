#include <string.h>
#include "embARC.h"
#include "embARC_debug.h"
//#include "dw_qspi.h"
#include "dw_ssi.h"
#include "device.h"
#include "vendor.h"
#include "instruction.h"
#include "sfdp.h"

#define E_SFDP_NOPAR			(-100)
#define E_SFDP_NORES			(-101)

typedef struct {
	uint32_t ins;
	uint32_t sector_size;
} flash_erase_type_t;

static int32_t sfdp_read_sfdp_header(dw_ssi_t *dw_ssi, \
		sfdp_header_t **header);
static int32_t sfdp_read_basic_parameter_header(dw_ssi_t *dw_ssi, \
		sfdp_header_t *header, sfdp_param_header_t **param_header);
static int32_t sfdp_read_parameter(dw_ssi_t *dw_ssi, \
		sfdp_param_header_t *param_header, uint8_t *params);
static int32_t sfdp_read_sector_map_param_header(dw_ssi_t *dw_ssi, \
		sfdp_header_t *header, sfdp_param_header_t **param_header);
static int32_t sfdp_flash_region_parse(dw_ssi_t *dw_ssi, \
		flash_device_t *device, uint8_t *param, uint32_t len);


/* description: detect whether the external device obeys the SFDP standard.
 *   @dw_qspi, the HW port driver.
 *   @return,
 *     < 0, indicate that there exist an issue.
 *     = 0, indicate that the external device doesn't obey SFDP standard.
 *     > 0, indicate that the external device obey the SFDP standard.
 * */
int32_t flash_sfdp_detect(dw_ssi_t *dw_ssi)
{
	int result = E_OK;

	uint8_t read_buffer[8];

	dw_ssi_xfer_t xfer;
	dw_ssi_ops_t *ssi_ops = (dw_ssi_ops_t *)dw_ssi->ops;

	if ((NULL == dw_ssi) || (NULL == dw_ssi->ops)) {
		result = E_PAR;
	} else {
		xfer.ins = CMD_READ_JEDEC;
		xfer.addr = 0;
		xfer.buf = (void *)read_buffer;
		xfer.len = 4;
		//xfer.dummy_data = 0x0;
		xfer.rd_wait_cycle = CMD_RD_JEDEC_WC;
		xfer.addr_valid = 1;

		result = ssi_ops->read(dw_ssi, &xfer, 0);
		if (E_OK == result) {
			if ((read_buffer[0] == 'S') && \
			    (read_buffer[1] == 'F') && \
			    (read_buffer[2] == 'D') && \
			    (read_buffer[3] == 'P')) {
				result = 1;
			}
		}
	}

	return result;
}

static inline void sfdp_erase_min_sector_size(flash_erase_type_t *erase_type, \
		uint32_t erase_type_mask, uint32_t *sec_ins, uint32_t *sec_size)
{
	uint32_t j, ins = 0;
	uint32_t size = 0;

	for (j = 0; j < 4; j++) {
		if (erase_type_mask & (1 << j)) {
			if (0 == size) {
				size = erase_type[j].sector_size;
				ins = erase_type[j].ins;
			} else {
				if (erase_type[j].sector_size < size) {
					size = erase_type[j].sector_size;
					ins = erase_type[j].ins;
				}
			}
		}
	}

	*sec_ins = ins;
	*sec_size = size;
}

/* description: gain the basic information of external device.
 *   @dw_qspi, the hardware port driver.
 *   @device, the external flash device descriptor.
 *   @result,
 *     < 0, an issue happened.
 *     >= 0, success.
 * */
int32_t flash_sfdp_param_read(dw_ssi_t *dw_ssi, flash_device_t *device)
{
	int32_t result = E_OK;

	sfdp_header_t sfdp_header;
	sfdp_header_t *sfdp_header_ptr = &sfdp_header;
	sfdp_param_header_t param_header;
	sfdp_param_header_t *param_header_ptr = &param_header;

	uint8_t read_buffer[256];

	uint8_t erase_type_mask = 0;
	flash_erase_type_t erase_type[4];

	if ((NULL == dw_ssi) || (NULL == dw_ssi->ops) || (NULL == device)) {
		result = E_PAR;
	} else {
		/* read sfdp header. */
		result = sfdp_read_sfdp_header(dw_ssi, &sfdp_header_ptr);
	}

	/* read basic parameter. */
	if (E_OK == result) {
//		sfdp_param_header_t *param_header_ptr = &param_header;
		result = sfdp_read_basic_parameter_header(dw_ssi, \
				sfdp_header_ptr, &param_header_ptr);
		if (E_OK == result) {
			result = sfdp_read_parameter(dw_ssi, \
					&param_header, read_buffer);
		}
	}

	/* get device total size and page size. */
	if (E_OK == result) {
		if (param_header_ptr->param_length >= 2) {
			device->total_size = flash_total_size(read_buffer);
		} else {
			device->total_size = 0;
			result = E_SYS;
		}
		if (param_header_ptr->param_length >= 11) {
			device->page_size = flash_page_size(read_buffer);
		} else {
			device->page_size = 0x100;
		}

		if (param_header_ptr->param_length >= 9) {
			if (sfdp_flash_erase_type1_size(read_buffer)) {
				erase_type[0].ins = sfdp_flash_erase_type1_instruct(read_buffer);
				erase_type[0].sector_size = 1 << sfdp_flash_erase_type1_size(read_buffer);
				erase_type_mask |= (1 << 0);
			}
			if (sfdp_flash_erase_type2_size(read_buffer)) {
				erase_type[1].ins = sfdp_flash_erase_type2_instruct(read_buffer);
				erase_type[1].sector_size = 1 << sfdp_flash_erase_type2_size(read_buffer);
				erase_type_mask |= (1 << 1);
			}

			if (sfdp_flash_erase_type3_size(read_buffer)) {
				erase_type[2].ins = sfdp_flash_erase_type3_instruct(read_buffer);
				erase_type[2].sector_size = 1 << sfdp_flash_erase_type3_size(read_buffer);
				erase_type_mask |= (1 << 2);
			}
			if (sfdp_flash_erase_type4_size(read_buffer)) {
				erase_type[3].ins = sfdp_flash_erase_type4_instruct(read_buffer);
				erase_type[3].sector_size = 1 << sfdp_flash_erase_type4_size(read_buffer);
				erase_type_mask |= (1 << 3);
			}
		} else {
			result = E_SYS;
			EMBARC_PRINTF("no erase type\r\n");
		}
	}

	/* read sector map parameter. */
	if (E_OK == result) {
		result = sfdp_read_sector_map_param_header(dw_ssi, \
				sfdp_header_ptr, &param_header_ptr);
		if (E_OK == result) {
			result = sfdp_read_parameter(dw_ssi, \
					&param_header, read_buffer);
			if (E_OK == result) {
				result = sfdp_flash_region_parse(dw_ssi, device, \
					read_buffer, param_header.param_length);
			}
		} else if (E_SFDP_NOPAR == result) {
			/* no parameter! only one region! */
			device->m_region[0].size = device->total_size;
			device->m_region[0].base = 0x0;
			device->m_region[0].erase_type_mask = erase_type_mask;

			result = E_OK;
		} else {
			/* TODO: sector map header read failed! */
			result = E_SYS;
		}
	}

	if (E_OK == result) {
		uint32_t i, sec_ins, sec_size = 0;
		for (i = 0; i < EXT_FLASH_REGION_CNT_MAX; i++) {
			if (device->m_region[i].erase_type_mask) {
				sfdp_erase_min_sector_size(erase_type, \
					device->m_region[i].erase_type_mask,
					&sec_ins, &sec_size);
				device->m_region[i].sector_size = sec_size;
				device->m_region[i].sec_erase_ins = sec_ins;
			}
		}
	}

	return result;
}

static int32_t sfdp_read_sfdp_header(dw_ssi_t *dw_ssi, sfdp_header_t **header)
{
	int result = E_OK;

	dw_ssi_xfer_t xfer;
	dw_ssi_ops_t *ssi_ops = (dw_ssi_ops_t *)dw_ssi->ops;

	if ((NULL == dw_ssi) || (NULL == dw_ssi->ops) || \
		(NULL == header) || (NULL == *header)) {
		result = E_PAR;
	} else {
		xfer.ins = CMD_READ_JEDEC;
		xfer.addr = 0;
		xfer.buf = (void *)(*header);
		xfer.len = sizeof(sfdp_header_t);
		//xfer.dummy_data = 0x0;
		xfer.rd_wait_cycle = CMD_RD_JEDEC_WC;
		xfer.addr_valid = 1;

		result = ssi_ops->read(dw_ssi, &xfer, 0);
	}

	return result;
}

static int32_t sfdp_read_basic_parameter_header(dw_ssi_t *dw_ssi, \
		sfdp_header_t *header, sfdp_param_header_t **param_header)
{
	int result = E_OK;

	/* the first 8 bytes is sfdp header. */
	uint32_t cur_address = 8;

	uint32_t cur_reversion, reversion = 0;
	uint32_t param_id = 0;

	dw_ssi_xfer_t xfer;
	dw_ssi_ops_t *ssi_ops = (dw_ssi_ops_t *)dw_ssi->ops;

	sfdp_param_header_t tmp_param;
	sfdp_param_header_t *param_header_ptr = *param_header;

	if ((NULL == dw_ssi) || (NULL == dw_ssi->ops) || \
		(NULL == header) || (NULL == param_header) || (NULL == *param_header)) {
		result = E_PAR;
	} else {
		uint8_t param_header_cnt = header->param_header_number + 1;

		xfer.ins = CMD_READ_JEDEC;
		//xfer.d_buf = (void *)(*param_header);
		xfer.buf = (void *)(&tmp_param);
		xfer.len = sizeof(sfdp_param_header_t);
		//xfer.dummy_data = 0x0;
		xfer.rd_wait_cycle = CMD_RD_JEDEC_WC;
		xfer.addr_valid = 1;

		/* in some device there may be over one basic paramter header.
		 * so the while loop uses to look up the lastest version. */
		while (param_header_cnt--) {
			xfer.addr = cur_address;

			result = ssi_ops->read(dw_ssi, &xfer, 0);
			if (E_OK == result) {
				param_id = tmp_param.id_lsb;
				param_id |= (tmp_param.id_msb << 8);

				cur_reversion = tmp_param.minor_reversion;
				cur_reversion |= (tmp_param.major_reversion << 8);

				if (SFDP_PTID_BASIC_SPI_PROTOCOL == param_id) {
					memcpy(param_header_ptr, (void *)&tmp_param, xfer.len);
					if (cur_reversion > reversion) {
						reversion = cur_reversion;
					}
				} else {
					break;
				}

				cur_address += 8;
			} else {
				break;
			}
		}
	}

	return result;
}

static int32_t sfdp_read_parameter(dw_ssi_t *dw_ssi, \
		sfdp_param_header_t *param_header, uint8_t *params)
{
	int32_t result = E_OK;

	//uint32_t param_pointer, param_length;

	dw_ssi_xfer_t xfer;
	dw_ssi_ops_t *ssi_ops = (dw_ssi_ops_t *)dw_ssi->ops;

	if ((NULL == dw_ssi) || (NULL == dw_ssi->ops) || \
		(NULL == param_header) || (NULL == params)) {
		result = E_PAR;
	} else {
		xfer.addr = param_header->param_table_pointer[0] | \
				(param_header->param_table_pointer[1] << 8) | \
				(param_header->param_table_pointer[2] << 16);
		xfer.len = param_header->param_length << 2;
		xfer.ins = CMD_READ_JEDEC;
		xfer.buf = (void *)params;
		//xfer.dummy_data = 0x0;
		xfer.rd_wait_cycle = CMD_RD_JEDEC_WC;
		xfer.addr_valid = 1;

		result = ssi_ops->read(dw_ssi, &xfer, 0);
	}

	return result;
}

static int32_t sfdp_read_sector_map_param_header(dw_ssi_t *dw_ssi, \
		sfdp_header_t *header, sfdp_param_header_t **param_header)
{
	int result = E_OK;

	/* the first 8 bytes is sfdp header. */
	uint32_t cur_address = 8;
	uint16_t param_id = 0;

	uint32_t sec_map_flag = 0;

	dw_ssi_xfer_t xfer;
	dw_ssi_ops_t *ssi_ops = (dw_ssi_ops_t *)dw_ssi->ops;

	sfdp_param_header_t *param_header_ptr = *param_header;

	if ((NULL == dw_ssi) || (NULL == dw_ssi->ops) || \
		(NULL == header) || (NULL == param_header) || (NULL == *param_header)) {
		result = E_PAR;
	} else {
		uint8_t param_header_cnt = header->param_header_number + 1;

		xfer.ins = CMD_READ_JEDEC;
		xfer.buf = (void *)(*param_header);
		xfer.len = sizeof(sfdp_param_header_t);
		//xfer.dummy_data = 0x0;
		xfer.rd_wait_cycle = CMD_RD_JEDEC_WC;
		xfer.addr_valid = 1;

		while (param_header_cnt--) {
			xfer.addr = cur_address;

			result = ssi_ops->read(dw_ssi, &xfer, 0);
			if (E_OK == result) {
				param_id = param_header_ptr->id_lsb;
				param_id |= (param_header_ptr->id_msb << 8);

				if (SFDP_PTID_SECTOR_MAP == param_id) {
					sec_map_flag = 1;
					break;
				}

				cur_address += 8;
			} else {
				break;
			}
		}
		if (0 == sec_map_flag) {
			result = E_SFDP_NOPAR;
		}
	}

	return result;
}

static int32_t sfdp_command_descriptor_id(dw_ssi_t *dw_ssi, \
		uint32_t *cfg_id, uint8_t *param, uint32_t len)
{
	int32_t result = E_OK;

	uint8_t data, dword1;
	uint32_t flag = 0;
	uint32_t index = 0;

	dw_ssi_xfer_t xfer;
	dw_ssi_ops_t *ssi_ops = (dw_ssi_ops_t *)dw_ssi->ops;

	if ((NULL == dw_ssi) || (NULL == dw_ssi->ops) || \
		(NULL == cfg_id) || (NULL == param) || (0 == len)) {
		result = E_PAR;
	} else {
		*cfg_id = 0;

		xfer.buf = (void *)(&data);
		xfer.len = 1;
		//xfer.dummy_data = 0x0;
		xfer.rd_wait_cycle = 0;
		xfer.addr_valid = 1;

		/* read flash configuration indicated by command descriptor. */
		while (len) {
			dword1 = SFDP_PARAMETER_DWORD(param, index);

			xfer.addr = SFDP_PARAMETER_DWORD(param, index + 1);
			xfer.ins = (dword1 >> SFDP_CDCD_INSTRUCTION_SHIFT) & \
					SFDP_CDCD_INSTRUCTION_MASK;
	
			result = ssi_ops->read(dw_ssi, &xfer, 0);
			if (E_OK == result) {
				if (data & ((dword1 >> SFDP_CDCD_READ_DATA_MASK_SHIFT) & \
							SFDP_CDCD_READ_DATA_MASK_MASK)) {
					flag = (flag << 1) | 1;
				} else {
					flag = flag << 1;
				}
			} else {
				break;
			}

			if (dword1 & SFDP_CDCD_END_INDICATOR) {
				/* for next step... */
				index += 2;
				break;
			} else {
				index += 2;
				len -= 8;
			}
		}

		if (result >= 0) {
			result = index << 2;
			*cfg_id = flag;
		}
	}

	return result;
}

static int32_t sfdp_cmd_descriptor_param_header(uint32_t *region_cnt, uint32_t id, \
		uint8_t *param, uint32_t len)
{
	int32_t result = E_OK;

	uint32_t count = 0;
	uint32_t index = 0;
	uint32_t region_header = 0;

	if ((NULL == region_cnt) || (NULL == param) || (0 == len)) {
		result = E_PAR;
	} else {
		while (len) {
			region_header = SFDP_PARAMETER_DWORD(param, index);
			count = (region_header >> SFDP_CMDH_REGION_COUNT_SHIFT) & \
				       SFDP_CMDH_REGION_COUNT_MASK;
			if (id != ((region_header >> SFDP_CMDH_REGION_ID_SHIFT) & \
					SFDP_CMDH_REGION_ID_MASK)) {
				if (region_header & SFDP_CMDH_END_INDICATOR) {
					result = E_SYS;
					break;
				} else {
					index += 1 + count;
					len -= (count + 1) << 2;
				}
			} else {
				index += 1;
				len -= 4;
				break;
			}
		}

		if (result >= 0) {
			result = index << 2;
			*region_cnt = count;
		}
	}

	return result;
}


static int32_t sfdp_cmd_descriptor_region(flash_device_t *device, \
		uint8_t *param, uint32_t len, uint32_t region_count)
{
	int32_t result = E_OK;

	uint8_t dword1;
	uint32_t i = 0;
	uint32_t index = 0;

	uint32_t erase_type_mask, base = 0;

	if ((len < (region_count << 2)) || \
	    (region_count > EXT_FLASH_REGION_CNT_MAX)) {
		result = E_SYS;
	} else {
		for (; i < region_count; i++) {
			dword1 = SFDP_PARAMETER_DWORD(param, index);
			device->m_region[i].size = dword1 >> SFDP_REGION_SIZE_SHIFT;
			device->m_region[i].base = base;
			base += device->m_region[i].size;

			erase_type_mask = dword1 & SFDP_REGION_ERASE_TYPE_MASK;
			device->m_region[i].erase_type_mask = erase_type_mask;

			index += 1;
		}
	}

	return result;
}

static int sfdp_map_descriptor_parse(flash_device_t *device, uint8_t *param, uint32_t len)
{
	int32_t result = E_OK;

	uint32_t dword1;
	uint32_t index, region_cnt = 0;
	uint32_t erase_type_mask, base = 0;


	if ((NULL == device) || (NULL == param) || (len < 4)) {
		result = -1;
	} else {
		region_cnt = param[2] + 1;

		param += 4;
		for (index = 0; index < region_cnt; index++) {
			dword1 = SFDP_PARAMETER_DWORD(param, index);
			device->m_region[index].size = dword1 >> SFDP_REGION_SIZE_SHIFT;
			device->m_region[index].base = base;
			base += device->m_region[index].size;

			erase_type_mask = dword1 & SFDP_REGION_ERASE_TYPE_MASK;
			device->m_region[index].erase_type_mask = erase_type_mask;

			index += 1;
		}

		result = region_cnt;
	}

	return result;
}

static int32_t sfdp_flash_region_parse(dw_ssi_t *dw_ssi, \
		flash_device_t *device, uint8_t *param, uint32_t len)
{
	int32_t result = E_OK;

	uint32_t cfg_id = 0;
	uint32_t region_cnt = 0;

	if ((NULL == dw_ssi) || (NULL == dw_ssi->ops) || \
	    (NULL == device) || (NULL == param)) {
		result = E_PAR;
	} else {
       		if (SFDP_MAP_DESCRIPTOR == sfdp_sector_descriptor_type(param)) {
			/* parse sector map directly. */
			result = sfdp_map_descriptor_parse(device, param, len);
			if (result > 0) {
				result = E_OK;
			}
		} else {
			/* read ID through command... */
			result = sfdp_command_descriptor_id(dw_ssi, \
					&cfg_id, param, len);
			if (result > 0) {
				len -= result;
				param += result;
				result = sfdp_cmd_descriptor_param_header(&region_cnt, \
						cfg_id, param, len);
			}

			if (result > 0) {
				len -= result;
				param += result;
				result = sfdp_cmd_descriptor_region(device, \
						param, len, region_cnt);
				if (result >= 0) {
					result = region_cnt;
				}
			}
		}
	}

	return result;
}
