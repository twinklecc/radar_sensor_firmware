#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "embARC_toolchain.h"
#include "embARC.h"
#include "embARC_error.h"
#include "embARC_debug.h"
#include "arc_exception.h"
#include "board.h"
#include "dw_ssi.h"
#include "xip.h"
#include "xip_hal.h"
#include "device.h"
#include "instruction.h"
#include "cfi.h"
#include "sfdp.h"
#include "flash.h"
#include "flash_header.h"
#include "config.h"

#ifdef OS_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_CLI.h"
#endif

#ifdef OS_FREERTOS
#define FLASH_DEVICE_LOCK(lock)		while (xSemaphoreTake(lock, portMAX_DELAY) != pdTRUE) {}
#define FLASH_DEVICE_UNLOCK(lock)	xSemaphoreGive(lock)
#else
#define FLASH_DEVICE_LOCK(lock)
#define FLASH_DEVICE_UNLOCK(lock)
#endif

static uint32_t cur_frame_format = 0;
static flash_device_t *ext_dev_desc = NULL;
static int32_t flash_open_flag = 0;

#ifdef OS_FREERTOS
static xSemaphoreHandle flash_lock;
#endif

static int32_t _flash_memory_write(uint32_t addr, \
		const void *data, uint32_t len, uint32_t flag);
static int32_t _flash_memory_read(uint32_t addr, \
		void *data, uint32_t len, uint32_t flag);
static uint32_t flash_memory_read_instruct(void);
static int32_t flash_device_mode(dw_ssi_t *dw_ssi, uint32_t mode);

/* description: initialize QSPI and external flash device. */
int32_t flash_init(void)
{
	int32_t result = E_OK;

#if FLASH_XIP_EN
	do {
		dw_ssi_t *dw_ssi = dw_ssi_get_dev(DW_QSPI_ID);
		ext_dev_desc = get_flash_device(dw_ssi);
		if (NULL == ext_dev_desc) {
			result = E_NORES;
			break;
		}
		result = flash_xip_init();
		if (E_OK != result) {
			break;
		}
		flash_open_flag = 1;
	} while(0);
#else
	dw_ssi_ops_t *ssi_ops = NULL;
	dw_ssi_t *dw_ssi = dw_ssi_get_dev(DW_QSPI_ID);

	dw_ssi_ctrl_t ctrl = DW_SSI_CTRL_STATIC_DEFAULT;
	dw_ssi_spi_ctrl_t spi_ctrl = DW_SSI_SPI_CTRL_STATIC_DEFAULT;

	do {
		if ((NULL == dw_ssi) || (NULL == dw_ssi->ops)) {
			result = E_NORES;
			break;
		}
		ssi_ops = (dw_ssi_ops_t *)dw_ssi->ops;

		/* enable QSPI Pclk. */
		qspi_reset(1);
		chip_hw_udelay(50);
		qspi_reset(0);

		qspi_enable(1);

		result = ssi_ops->control(dw_ssi, &ctrl);
		if (E_OK != result) {
			break;
		}

		result = ssi_ops->spi_control(dw_ssi, &spi_ctrl);
		if (E_OK != result) {
			break;
		}

		result = ssi_ops->baud(dw_ssi, CONFIG_QSPI_BAUD);
		if (E_OK != result) {
			break;
		}

		result = ssi_ops->enable(dw_ssi, 1);
		if (E_OK != result) {
			break;
		}

		chip_hw_mdelay(10);
		result = flash_reset();
		if (E_OK != result) {
			break;
		}

		ext_dev_desc = get_flash_device(dw_ssi);
		if (NULL == ext_dev_desc) {
			result = E_NORES;
			break;
		}
		result = flash_device_mode(dw_ssi, CONFIG_QSPI_FF);
		cur_frame_format = CONFIG_QSPI_FF;

#ifdef OS_FREERTOS
		flash_lock = xSemaphoreCreateBinary();
		xSemaphoreGive(flash_lock);
#endif
		flash_open_flag = 1;
	} while (0);
#endif /* !FLASH_XIP_EN! */

	return result;
}


int32_t flash_reset(void)
{
	int32_t result = E_OK;

	uint32_t cpu_sts = 0;

	dw_ssi_xfer_t xfer;

	dw_ssi_ops_t *ssi_ops = NULL;
	dw_ssi_t *dw_ssi = dw_ssi_get_dev(DW_QSPI_ID);

	if ((NULL == dw_ssi) || (NULL == dw_ssi->ops)) {
		result = E_PAR;
	} else {
		ssi_ops = (dw_ssi_ops_t *)dw_ssi->ops;

		xfer.ins = CMD_RST_ENABLE;
		xfer.addr = 0;
		xfer.buf = NULL;
		xfer.len = 0;
		//xfer.dummy_data = 0x0;
		xfer.rd_wait_cycle = 0;
		xfer.addr_valid = 0;

		cpu_sts = arc_lock_save();
		result = ssi_ops->write(dw_ssi, &xfer, 0);
		arc_unlock_restore(cpu_sts);
		chip_hw_mdelay(1);

		if (E_OK == result) {
			xfer.ins = CMD_RST;
			cpu_sts = arc_lock_save();
			result = ssi_ops->write(dw_ssi, &xfer, 0);
			arc_unlock_restore(cpu_sts);
			chip_hw_mdelay(1);
		}
	}

	return result;
}

int32_t flash_memory_readb(uint32_t addr, uint8_t *data, uint32_t len)
{
	int32_t result = E_OK;

#if FLASH_XIP_EN
	result = flash_xip_readb(addr, (void *)data, len);
#else
	result = _flash_memory_read(addr, (void *)data, len, 0);
#endif

	return result;
}

int32_t flash_memory_readw(uint32_t addr, uint32_t *data, uint32_t len)
{
	int32_t result = E_OK;

#if FLASH_XIP_EN
	result = flash_xip_read(addr, (void *)data, len);
#else
	result = _flash_memory_read(addr, (void *)data, len, 1);
#endif

	return result;
}

int32_t flash_memory_writeb(uint32_t addr, const uint8_t *data, uint32_t len)
{
	int32_t result = E_OK;

#if FLASH_XIP_EN
	result = _flash_memory_write(addr, (const void *)data, len, 0);
#else
	FLASH_DEVICE_LOCK(flash_lock);
	result = _flash_memory_write(addr, (const void *)data, len, 0);
	FLASH_DEVICE_UNLOCK(flash_lock);
#endif
	return result;
}

int32_t flash_memory_writew(uint32_t addr, const uint32_t *data, uint32_t len)
{
	int32_t result = E_OK;

#if FLASH_XIP_EN
	result = _flash_memory_write(addr, (const void *)data, len, 1);
#else
	FLASH_DEVICE_LOCK(flash_lock);
	result = _flash_memory_write(addr, (const void *)data, len, 1);
	FLASH_DEVICE_UNLOCK(flash_lock);
#endif
	return result;
}

int32_t flash_program_resume(void)
{
	int32_t result = E_OK;

	dw_ssi_ops_t *ssi_ops = NULL;
	dw_ssi_t *xip_ptr = dw_ssi_get_dev(XIP_QSPI_ID);

	dw_ssi_xfer_t xfer = DW_SSI_XFER_INIT(CMD_PGRS, 0, 0, 0);

	if ((NULL == xip_ptr) || (xip_ptr->ops)) {
		result = E_SYS;
	} else {
		ssi_ops = (dw_ssi_ops_t *)xip_ptr->ops;

		uint32_t cpu_sts = arc_lock_save();
		result = ssi_ops->write(xip_ptr, &xfer, 0);
		arc_unlock_restore(cpu_sts);
	}

	return result;
}


int32_t _flash_memory_erase(uint32_t addr, uint32_t len)
{
	int32_t result = E_OK;

	uint32_t cpu_status = 0;

	dw_ssi_ops_t *ssi_ops = NULL;
	dw_ssi_t *dw_ssi = dw_ssi_get_dev(DW_QSPI_ID);

	dw_ssi_xfer_t we_xfer = DW_SSI_XFER_INIT(CMD_WRITE_ENABLE, 0, 0, 0);
	dw_ssi_xfer_t xfer = DW_SSI_XFER_INIT(CMD_WRITE_ENABLE, 0, 0, 1);


	if ((NULL == dw_ssi) || (NULL == dw_ssi->ops) || \
	    (NULL == ext_dev_desc) || (NULL == ext_dev_desc->ops)) {
		result = E_SYS;
	} else {
		uint32_t r_idx;
		uint32_t sector_size = 0;

		ssi_ops = (dw_ssi_ops_t *)dw_ssi->ops;

		while (len) {
			cpu_status = arc_lock_save();
			r_idx = flash_dev_find_region(ext_dev_desc->m_region, addr);
			if (r_idx >= EXT_FLASH_REGION_CNT_MAX) {
				result = E_PAR;
				break;
			}

			sector_size = ext_dev_desc->m_region[r_idx].sector_size;
			if (addr % sector_size) {
				result = E_PAR;
				break;
			} else {
			}

			result = ssi_ops->write(dw_ssi, &we_xfer, 0);
			if (E_OK != result) {
				break;
			} else {
				/* waiting! */
				result = flash_wait_status(dw_ssi, FLASH_WEL, 0);
				if (E_OK != result) {
					break;
				}
			}

			xfer.ins = ext_dev_desc->m_region[r_idx].sec_erase_ins;
			xfer.addr = addr;

			result = ssi_ops->write(dw_ssi, &xfer, 0);
			if (E_OK != result) {
				break;
			} else {
				/* waiting! */
				result = flash_wait_status(dw_ssi, FLASH_WIP, 1);
				if (E_OK != result) {
					break;
				}
			}

			if (len < sector_size) {
				len = 0;
			} else {
				len -= sector_size;
				addr += sector_size;
			}

			arc_unlock_restore(cpu_status);
		}
	}

	return result;
}

int32_t flash_quad_entry(void)
{
	int32_t result = E_OK;
	dw_ssi_t *dw_ssi = dw_ssi_get_dev(DW_QSPI_ID);
	if ((NULL != ext_dev_desc) && (NULL != ext_dev_desc->ops) && \
		(ext_dev_desc->ops->quad_entry)) {
		result = ext_dev_desc->ops->quad_entry(dw_ssi);
	} else {
		result = E_SYS;
	}
	return result;
}

int32_t flash_memory_erase(uint32_t addr, uint32_t len)
{
	int32_t result = E_OK;

#if FLASH_XIP_EN
	result = _flash_memory_erase(addr, len);
#else
	FLASH_DEVICE_LOCK(flash_lock);
	result = _flash_memory_erase(addr, len);
	FLASH_DEVICE_UNLOCK(flash_lock);
#endif
	return result;
}

static int32_t _flash_memory_read(uint32_t addr, void *data, uint32_t len, uint32_t flag)
{
	int32_t result = E_OK;

	uint32_t cpu_status = 0;
	uint32_t read_ins = flash_memory_read_instruct();

	dw_ssi_ops_t *ssi_ops = NULL;
	dw_ssi_t *dw_ssi = dw_ssi_get_dev(DW_QSPI_ID);

	dw_ssi_xfer_t xfer = DW_SSI_XFER_INIT(read_ins, addr, 0, 1);

	if ((NULL == ext_dev_desc) || \
	    (NULL == dw_ssi) || (NULL == dw_ssi->ops) || \
	    (NULL == data) || (0 == len)) {
		result = E_PAR;
	} else {
		uint32_t s_read_cnt = ext_dev_desc->page_size;

#ifdef OS_FREERTOS
		/* waiting... */
		FLASH_DEVICE_LOCK(flash_lock);
#endif

		ssi_ops = (dw_ssi_ops_t *)dw_ssi->ops;
		while (len) {
			if (len < s_read_cnt) {
				s_read_cnt = len;
			}

			xfer.addr = addr;
			xfer.buf = data;
			xfer.len = s_read_cnt;

			cpu_status = arc_lock_save();
			result = ssi_ops->read(dw_ssi, &xfer, flag);
			arc_unlock_restore(cpu_status);
			if (E_OK != result) {
				break;
			}

			addr += s_read_cnt;
			data = (void *)((uint32_t)data + s_read_cnt);
			len -= s_read_cnt;
		}

#ifdef OS_FREERTOS
		FLASH_DEVICE_UNLOCK(flash_lock);
#endif
	}

	return result;
}

int32_t flash_wait_status(dw_ssi_t *dw_ssi, uint32_t status, uint32_t cont_sts)
{
	int32_t result = E_OK;

	uint32_t flash_status = 0;

	do {
		chip_hw_udelay(100);

		if (NULL == ext_dev_desc->ops->status) {
			result = E_NOOPS;
			break;
		}

		result = ext_dev_desc->ops->status(dw_ssi, status);
		if (result >= 0) {
			flash_status = result;
		} else {
			break;
		}
	} while (cont_sts == flash_status);

	if (result > 0) {
		result = E_OK;
	}

	return result;
}
static int32_t _flash_memory_write(uint32_t addr, const void *data, uint32_t len, uint32_t flag)
{
	int32_t result = E_OK;

	uint32_t cpu_status = 0, s_write_cnt = 0;

	dw_ssi_xfer_t we_xfer = DW_SSI_XFER_INIT(CMD_WRITE_ENABLE, 0, 0, 0);
	dw_ssi_xfer_t xfer = DW_SSI_XFER_INIT(CMD_PAGE_PROG, 0, 0, 1);

	dw_ssi_ops_t *ssi_ops = NULL;
	dw_ssi_t *dw_ssi = dw_ssi_get_dev(DW_QSPI_ID);

	if ((NULL == ext_dev_desc) || (NULL == ext_dev_desc->ops) || \
	    (NULL == dw_ssi) || (NULL == dw_ssi->ops) || \
	    (addr % ext_dev_desc->page_size) || \
	    (NULL == data) || (0 == len)) {
		result = E_PAR;
	} else {
		if (flag) {
			s_write_cnt = (ext_dev_desc->page_size >> 2);
		} else {
			s_write_cnt = ext_dev_desc->page_size;
		}

		ssi_ops = (dw_ssi_ops_t *)dw_ssi->ops;

		while (len) {
			cpu_status = arc_lock_save();
			if (len < s_write_cnt) {
				s_write_cnt = len;
			}

			xfer.addr = addr;
			xfer.buf = (void *)data;
			xfer.len = s_write_cnt;

			result = ssi_ops->write(dw_ssi, &we_xfer, 0);
			if (E_OK != result) {
				break;
			} else {
				/* waiting! */
				result = flash_wait_status(dw_ssi, FLASH_WEL, 0);
				if (E_OK != result) {
					break;
				}
			}

			result = ssi_ops->write(dw_ssi, &xfer, flag);
			if (E_OK != result) {
				break;
			} else {
				/* waiting! */
				result = flash_wait_status(dw_ssi, FLASH_WIP, 1);
				if (E_OK != result) {
					break;
				}
			}

			if (flag) {
				addr += (s_write_cnt << 2);
				data = (void *)((uint32_t)data + (s_write_cnt << 2));
			} else {
				addr += s_write_cnt;
				data = (void *)((uint32_t)data + s_write_cnt);
			}
			len -= s_write_cnt;
			arc_unlock_restore(cpu_status);
		}
	}


	return result;
}

static uint32_t flash_memory_read_instruct(void)
{
	uint32_t instruct = 0;

	switch (cur_frame_format) {
		case STANDARD_SPI_FRAME_FORMAT:
			instruct = CMD_READ;
			break;
		case DUAL_FRAME_FORMAT:
			break;
		case QUAD_FRAME_FORMAT:
			instruct = CMD_Q_READ;
		case OCTAL_FRAME_FORMAT:
		default:
			break;
	}

	return instruct;
}

static int32_t flash_device_mode(dw_ssi_t *dw_ssi, uint32_t mode)
{
	int32_t result = E_OK;

	switch (mode) {
		case DW_SSI_1_1_X:
			break;
		case DW_SSI_1_X_X:
			result = ext_dev_desc->ops->quad_entry(dw_ssi);
			break;
		case DW_SSI_X_X_X:
			result = ext_dev_desc->ops->qpi_entry(dw_ssi);
			break;
		default:
			result = E_PAR;
	}

	return result;
}
