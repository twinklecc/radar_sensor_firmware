#include "embARC.h"
#include "embARC_debug.h"
#include "embARC_assert.h"

//#include "dw_qspi_reg.h"
#include "dw_ssi_reg.h"
#include "dw_ssi.h"
#include "xip_hal.h"

#include "flash_header.h"
#include "flash_mmap.h"

#include "instruction.h"
#include "vendor.h"

#include "config.h"


#if (defined SYSTEM_3_STAGES_BOOT) && (defined __FIRMWARE__)
__attribute__ ((aligned(4), section(".nvm_header")))
image_header_t sensor_flash_header = {
	.magic_number = 0x43616c74UL,
	.sw_version = {
		.major_ver_id = MAJOR_VERSION_ID,
		.minor_ver_id = MINOR_VERSION_ID,
		.stage_ver_id = STAGE_VERSION_ID,
		.date = __DATE__,
		.time = __TIME__,
		.info = SYS_NAME_STR
	},
	.payload_addr = FLASH_FIRMWARE_BASE + sizeof(image_header_t),
	//.payload_size
	//.xip_en
	//.ram_base
	//.exec_offset
	.crc_part_size = 0x1000,
};
#else
__attribute__ ((aligned(4), section(".nvm_header")))
flash_header_t sensor_flash_header = {
	.magic_number = 0x43616c74UL,
	.version = 0,

#if ((FMCW_SDM_FREQ == 400)||(FMCW_SDM_FREQ == 450))
    .qspi_speed0 = 2000000,
#elif FMCW_SDM_FREQ == 360
    .qspi_speed0 = 1500000,
#endif

#if (defined SYSTEM_3_STAGES_BOOT)
	.pload_addr = FLASH_BOOT_BASE,
#else
	.pload_addr = FLASH_FIRMWARE_BASE,
#endif

	/* filled by script! */
	/*
	.pload_size = 0,
	.ram_base = 0,
	.exec_offset = 0,
	*/
#if	FLASH_XIP_EN
	.xip_flag = FLASH_XIP_ON,
	.pll_on = PLL_CLOCK_ON,
#else
	.xip_flag = FLASH_XIP_OFF,
	.pll_on = PLL_CLOCK_ON,
#endif

	/* TODO: while xip, the QSPI baud rate is fixed to 40Mbps! */
	.xip_ctrl = {
		//.ctrl0 = 0x5f0200,
		.ctrl0 = CLK_MODE0_SSI_CTRL0(QUAD_FRAME_FORMAT, DW_SSI_DATA_LEN_32, DW_SSI_RECEIVE_ONLY),
		.ctrl1 = 0x7ff,
		.baud = 0xa,
		.rx_sample_delay = FLASH_DEV_SAMPLE_DELAY,

		//.spi_ctrl = 0x3219,
		.spi_ctrl = SSI_SPI_CTRLR0(FLASH_DEV_DUMMY_CYCLE + FLASH_DEV_MODE_CYCLE, \
				DW_SSI_INS_LEN_8, DW_SSI_ADDR_LEN_24, DW_SSI_1_X_X),

		//.read_cmd = (0x1f << 16) | (0x6 << 21) | (0x2 << 25) | 0xeb,
		.read_cmd = FLASH_COMMAND(CMD_Q_READ, \
			DW_SSI_INS_LEN_8, DW_SSI_ADDR_LEN_24, DW_SSI_DATA_LEN_32),
		.xip_offset = FLASH_HEADER_BASE,
		//.xip_endian = XIP_BIG_ENDIAN,
		.ahb_endian = 0x33,
		.aes_endian = 0x33,
		.xip_rx_buf_len = CONFIG_XIP_INS_BUF_LEN,
		.data_offset = FLASH_HEADER_BASE,
		.data_rx_buf_len = CONFIG_XIP_RD_BUF_LEN,

#if FLASH_XIP_EN
		.mode = 0x7,
#else
		.mode = 0,
#endif
		.block_cnt = CONFIG_AES_VLD_BLK_CNT,
		.last_block_size = CONFIG_AES_LAST_BLK_SIZE
	},

	.cmd_sequence = {
		FLASH_DEV_CMD0, FLASH_DEV_CMD1, FLASH_DEV_CMD2, FLASH_DEV_CMD3
	},

	/* TODO: move to a separate file! */
	.pll_param = {
		/* (addr, value, delay). ms. */
		DEF_PLL_PARAM(0x00, 0x00, 0x00),
		DEF_PLL_PARAM(0x03, 0x07, 0x00),
		DEF_PLL_PARAM(0x05, 0xc0, 0x00),
		//DEF_PLL_PARAM(0x09, 0x30, 0x00),
		DEF_PLL_PARAM(0x12, 0xc0, 0x00),
		DEF_PLL_PARAM(0x13, 0xc0, 0x00),
		DEF_PLL_PARAM(0x14, 0xc0, 0x00),
		DEF_PLL_PARAM(0x15, 0xb0, 0x00),
		DEF_PLL_PARAM(0x16, 0xb0, 0x00),
		DEF_PLL_PARAM(0x1b, 0xeb, 0x00),
		
#if FMCW_SDM_FREQ == 400
		DEF_PLL_PARAM(0x1d, 0x19, 0x00),
		DEF_PLL_PARAM(0x1e, 0xd0, 0x00),
#elif FMCW_SDM_FREQ == 450
		DEF_PLL_PARAM(0x1d, 0x18, 0x00),
		DEF_PLL_PARAM(0x1e, 0xc0, 0x00),
#elif FMCW_SDM_FREQ == 360
		DEF_PLL_PARAM(0x1d, 0x18, 0x00),
		DEF_PLL_PARAM(0x1e, 0xe0, 0x00),
#endif

		DEF_PLL_PARAM(0x25, 0xa0, 0x00),
		DEF_PLL_PARAM(0x26, 0x6f, 0x00),
		DEF_PLL_PARAM(0x7d, 0x00, 0x00),
		DEF_PLL_PARAM(0x7d, 0x01, 0x07)

	},

	.qspi_speed = 10000000,

	.sec_dbg_cert_valid = SEC_DBG_CERT_INVLD,
	.sec_dbg_cert_addr = FLASH_DBG_CERT_BASE,
	.sec_dbg_cert_size = CONFIG_SEC_DBG_CERT_LEN,

	.pload_crc_granularity = 0x1000,

	.boot_timeout = 0xFFFFFFFFU,

	.sw_version = {
		.major_ver_id = MAJOR_VERSION_ID,
		.minor_ver_id = MINOR_VERSION_ID,
		.stage_ver_id = STAGE_VERSION_ID,
		.date = __DATE__,
		.time = __TIME__,
		.info = SYS_NAME_STR
	}
};
#endif

flash_header_t *flash_header_get(void)
{
	flash_header_t *pflash_header = &sensor_flash_header;

	return pflash_header;
}
