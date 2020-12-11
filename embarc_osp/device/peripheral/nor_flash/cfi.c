#include "embARC.h"
#include "embARC_debug.h"
//#include "dw_qspi.h"
#include "dw_ssi.h"
#include "instruction.h"
#include "device.h"
#include "cfi.h"
//#include "flash.h"


/*description: read flash device id. and judge whether it's CFI device.
 * note: if flash xip is enabled, pls don't call this function.
 * */
int32_t flash_cfi_detect(dw_ssi_t *dw_ssi)
{
	int32_t result = E_OK;

	uint8_t read_buffer[256];

	dw_ssi_xfer_t xfer;

	dw_ssi_ops_t *ssi_ops = (dw_ssi_ops_t *)dw_ssi->ops;


	if ((NULL == dw_ssi) || (NULL == dw_ssi->ops)) {
		result = E_PAR;
	} else {
		xfer.ins = CMD_READ_ID;
		xfer.addr = 0;
		xfer.buf = (void *)read_buffer;
		xfer.len = 0x1b;
		//xfer.dummy_data = 0x55;
		xfer.rd_wait_cycle = 0x0;
		xfer.addr_valid = 0x0;

		result = ssi_ops->read(dw_ssi, &xfer, 0);
		if (E_OK == result) {
			cfi_query_id_t *cfi_id = (cfi_query_id_t *)&read_buffer[0x10];
			if ((cfi_id->unique_str[0] == 'Q') && \
				(cfi_id->unique_str[1] == 'R') && \
				(cfi_id->unique_str[2] == 'Y')) {
				result = 1;
			}
		}
	}

	return result;
}

static int32_t _cfi_region_pickup(uint8_t *region_info, uint32_t region_cnt, flash_device_t *device)
{
	int32_t result = E_OK;

#if 0
	/* TODO: move to vendor/xxx.c */
	static uint8_t erase_ins[4] = {
	};
#endif

	if (region_cnt > EXT_FLASH_REGION_CNT_MAX) {
		result = -1;
	} else {
		uint32_t i = 0;
		uint32_t granularity, sec_count, base = 0;

		for (; i < region_cnt; i++) {
			granularity = region_info[2] | (region_info[3] << 8);
			granularity *= 256;
			device->m_region[i].base = base;
			device->m_region[i].sector_size = granularity;

			sec_count = region_info[0] | (region_info[1] << 8);
			sec_count += 1;
			device->m_region[i].size = granularity * sec_count;
			base += device->m_region[i].size;

			region_info += 4;
		}
	}

	return result;
}

int32_t flash_cfi_param_read(dw_ssi_t *dw_ssi, flash_device_t *device)
{
	int32_t result = E_OK;

	uint8_t read_buffer[256];

	dw_ssi_xfer_t xfer;

	cfi_device_geometry_t *dev_geometry = NULL;
	dw_ssi_ops_t *ssi_ops = (dw_ssi_ops_t *)dw_ssi->ops;

	if ((NULL == dw_ssi) || (NULL == dw_ssi->ops) || (NULL == device)) {
		result = E_PAR;
	} else {
		xfer.ins = CMD_READ_ID;
		xfer.addr = 0;
		xfer.buf = (void *)read_buffer;
		xfer.len = 256;
		//xfer.dummy_data = 0x0;
		xfer.rd_wait_cycle = 0;
		xfer.addr_valid = 0;

		result = ssi_ops->read(dw_ssi, &xfer, 0);
		if (E_OK == result) {
			/*
			int32_t i = 0;
			while (i < 256) {
				EMBARC_PRINTF("0x%x\r\n", read_buffer[i++]);
			}
			*/
			dev_geometry = (cfi_device_geometry_t *)(&read_buffer[CFI_DEV_GEOMETRY_DEF_OFFSET]);
#if 0
			/* if need, pls add into flash_device_t firstly! */
			//device->std = STD_CFI;
			//device->manufacturer_id = read_buffer[0];
			//device->device_id = (read_buffer[1] | (read_buffer[2] << 8));
			//device->family_id = read_buffer[5];
			//device->sector_arch = read_buffer[4];
#endif
			device->total_size = 1 << dev_geometry->device_size;
			device->page_size = 1 << dev_geometry->max_program_size[0];
			if (dev_geometry->max_program_size[1]) {
				/* the size of external device memory is too large!!! */
				result = -1;
			} else {
				/* parse erase region... */
				uint32_t count = dev_geometry->num_erase_block_region;
				uint8_t *region_info = &read_buffer[CFI_ERASE_BLOCK_REGION_OFFSET];

				result = _cfi_region_pickup(region_info, count, device);
			}
		}
	}

	return result;
}
