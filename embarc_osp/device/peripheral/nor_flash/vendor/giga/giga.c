#include "embARC.h"
#include "embARC_debug.h"
//#include "dw_qspi.h"
#include "dw_ssi.h"
#include "device.h"
#include "xip_hal.h"

#include "instruction.h"
#include "sfdp.h"

/* us. */
#define SUSPEND_LATENCY_MAX		(40)



static int32_t giga_param_read(dw_ssi_t *dw_ssi);
static int32_t giga_quad_entry(dw_ssi_t *dw_ssi);
static int32_t giga_quad_exit(dw_ssi_t *dw_ssi);
static int32_t giga_qpi_entry(dw_ssi_t *dw_ssi);
static int32_t giga_qpi_exit(dw_ssi_t *dw_ssi);
static int32_t giga_status_read(dw_ssi_t *dw_ssi, uint32_t status);

static flash_dev_ops_t giga_ops = {
	.quad_entry = giga_quad_entry,
	.quad_exit = giga_quad_exit,
	.qpi_entry = giga_qpi_entry,
	.qpi_exit = giga_qpi_exit,

	.status = giga_status_read
};

#ifdef STATIC_EXT_FLASH_PARAM
static flash_device_t giga_dev = {
	.total_size = 32000000 >> 3,
	.page_size = 256,
	.m_region = {
		DEF_FLASH_REGION(0x0, 0x400000, 0x1000, 0x20)
	},
	.ops = &giga_ops
};
#else
static flash_device_t giga_dev;
#endif

/* description: called during boot. */
flash_device_t *get_flash_device(dw_ssi_t *dw_ssi)
{
	int32_t result = E_OK;

	static uint32_t ext_dev_open_flag = 0;

	flash_device_t *ext_dev = NULL;

#ifdef STATIC_EXT_FLASH_PARAM
	ext_dev = &giga_dev;
#else
	if (0 == ext_dev_open_flag) {
		result = giga_param_read(dw_ssi);
		if (E_OK == result) {
			ext_dev = &giga_dev;
			ext_dev->ops = &giga_ops;
		}
	} else {
		ext_dev = &giga_dev;
	}
#endif

	return ext_dev;
}

static int32_t giga_param_read(dw_ssi_t *dw_ssi)
{
	int32_t result = E_OK;

	if ((NULL == dw_ssi) || (NULL == dw_ssi->ops)) {
		result = E_PAR;
	} else {
		result = flash_sfdp_detect(dw_ssi);
		if (result > 0) {
			result = flash_sfdp_param_read(dw_ssi, \
					&giga_dev);
		} else {
			if (E_OK == result) {
				result = E_NOEXS;
			}
		}
	}

	return result;
}

static int32_t giga_quad_entry(dw_ssi_t *dw_ssi)
{
	int32_t result = E_OK;

	uint32_t cpu_status = 0;
	uint8_t wdata[2] = {0x02, 0x02};


	dw_ssi_xfer_t xfer = DW_SSI_XFER_INIT(0x31, 0, 8, 0);
	dw_ssi_xfer_t we_xfer = DW_SSI_XFER_INIT(0x06, 0, 0, 0);

	if ((NULL == dw_ssi) || (NULL != dw_ssi->ops)) {
		dw_ssi_ops_t *ssi_ops = (dw_ssi_ops_t *)dw_ssi->ops;

		cpu_status = arc_lock_save();
		result = ssi_ops->write(dw_ssi, &we_xfer, 0);
		arc_unlock_restore(cpu_status);

		while (E_OK == result) {
			chip_hw_mdelay(1);
			result = giga_status_read(dw_ssi, FLASH_WEL);
		}

		xfer.buf = (void *)wdata;
		xfer.len = 1;
		result = ssi_ops->write(dw_ssi, &xfer, 0);
		if (E_OK == result) {
			chip_hw_mdelay(1);
		}
	}

	return result;
}

static int32_t giga_quad_exit(dw_ssi_t *dw_ssi)
{
	int32_t result = E_OK;

	return result;
}

static int32_t giga_qpi_entry(dw_ssi_t *dw_ssi)
{
	return E_NOEXS;
}

static int32_t giga_qpi_exit(dw_ssi_t *dw_ssi)
{
	return E_NOEXS;
}

static int32_t giga_status_read(dw_ssi_t *dw_ssi, uint32_t status)
{
	int32_t result = E_OK;

	uint8_t dev_status = 0;
	uint8_t sts_pos = 0;

	dw_ssi_ops_t *ssi_ops = NULL;
	dw_ssi_xfer_t xfer = DW_SSI_XFER_INIT(0, 0, 0, 0);

	if ((NULL == dw_ssi) || (NULL == dw_ssi->ops)) {
		result = E_PAR;
	} else {
		ssi_ops = (dw_ssi_ops_t *)dw_ssi->ops;

		xfer.buf = (void *)&dev_status;
		xfer.len = 1;

		switch (status) {
			case FLASH_WIP:
			case FLASH_WEL:
				xfer.ins = 0x05;
				sts_pos = 1 << status;
				break;
			case FLASH_BP:
			case FLASH_E_ERR:
			case FLASH_P_ERR:
			case FLASH_STSR_WD:
				xfer.ins = CMD_READ_RSTS1;
				sts_pos = 1 << status;
				break;
			case FLASH_P_SUSPEND:
			case FLASH_E_SUSPEND:
				xfer.ins = CMD_READ_RSTS2;
				sts_pos = 1 << (status - 8);
				break;
			default:
				result = E_PAR;
		}

		if (E_OK == result) {
			result = ssi_ops->read(dw_ssi, &xfer, 0);
			if (E_OK == result) {
				if (sts_pos & dev_status) {
					result = 1;
				}
			}
		}
	}


	return result;
}

#if 0
int32_t giga_status_read_test(dw_ssi_t *dw_ssi)
{
	int32_t result = E_OK;

	uint8_t dev_status[4] = {0xa5, 0xa5, 0xa5, 0xa5};
	dw_ssi_xfer_t xfer = DW_SSI_XFER_INIT(CMD_READ_RSTS1, 0, 0, 0);

	dw_ssi_ops_t *ssi_ops = NULL;

	if ((NULL == dw_ssi) || (NULL == dw_ssi->ops)) {
		result = E_PAR;
	} else {
		ssi_ops = (dw_ssi_ops_t *)dw_ssi->ops;

		xfer.buf = (void *)&dev_status;
		xfer.len = 2;
		//xfer.ins = CMD_READ_RSTS1;

		result = ssi_ops->readb(dw_ssi, &xfer);
		if (E_OK == result) {
			EMBARC_PRINTF("fls_dev_sts: 0x%x, 0x%x, 0x%x, 0x%x\r\n", \
					dev_status[0],\
					dev_status[1],\
					dev_status[2],\
					dev_status[3]\
					);
		}
	}

	return result;
}
#endif

static xip_param_t xip_info = {
	.rd_sts_cmd = FLASH_COMMAND(CMD_READ_RSTS1, \
			DW_SSI_INS_LEN_8, DW_SSI_ADDR_LEN_0, DW_SSI_DATA_LEN_8),
	.rd_cmd = FLASH_COMMAND(CMD_Q_READ, \
			DW_SSI_INS_LEN_8, DW_SSI_ADDR_LEN_24, DW_SSI_DATA_LEN_32),
	.wr_en_cmd = FLASH_COMMAND(CMD_WRITE_ENABLE, \
			DW_SSI_INS_LEN_8, DW_SSI_ADDR_LEN_0, DW_SSI_DATA_LEN_8),
	.program_cmd = FLASH_COMMAND(CMD_PAGE_PROG, \
			DW_SSI_INS_LEN_8, DW_SSI_ADDR_LEN_24, DW_SSI_DATA_LEN_32),
	.suspend_cmd = FLASH_COMMAND(CMD_PGSP, \
			DW_SSI_INS_LEN_8, DW_SSI_ADDR_LEN_0, DW_SSI_DATA_LEN_8),
	.resume_cmd = FLASH_COMMAND(CMD_PGRS, \
			DW_SSI_INS_LEN_8, DW_SSI_ADDR_LEN_0, DW_SSI_DATA_LEN_8),

	.wip_pos = (1 << 0),
	.xsb = (1 << 8) | (2 << 16),
	.suspend_wait = 0x1ff
};

xip_param_t *flash_xip_param_get(void)
{
	uint32_t freq = clock_frequency(XIP_CLOCK);

	xip_info.suspend_wait = 40 * (freq / 1000000);

	return &xip_info;
}
