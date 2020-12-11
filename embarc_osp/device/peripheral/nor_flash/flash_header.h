#ifndef _FLASH_HEADER_H_
#define _FLASH_HEADER_H_

#include "xip.h"
#include "version.h"


/* image header: */
typedef struct {
	uint32_t magic_number;
	sw_version_t sw_version;
	uint32_t payload_addr;
	uint32_t payload_size;
	uint32_t xip_en;
	uint32_t ram_base;
	uint32_t exec_offset;

	uint32_t crc_part_size;
	uint32_t reserved[41];
	uint32_t crc32;
} image_header_t;


/*
 * Description: external flash device command.
 *    ------------------------------------------------------
 *    |   Instruction   |    address     |      data       |
 *    ------------------------------------------------------
 * @valid: indicate whether this command is valid. and after it, CPU should wait how much time.
 *     [31:16], valid flag. [15:0], wait time.
 * @cmd: instruction of external flash device.
 * @value: the data of the command.
 **/
typedef struct ext_flash_cmd_t {
	uint32_t valid;

	uint32_t cmd;

	uint32_t addr;
	uint32_t value[4];
} dev_cmd_t;


/*
 * Description: flash header which the information about firmware.
 * @pload_addr: the start position of application image in external flash memory.
 * @pload_size: the size of application image in external flash memory.
 * @ram_base: the begin of application in internal RAM.
 * @exec_offset: the offset of the application image's enterpoint to its load address.
 * @xip_flag: XIP flag or flag indicate whether execute on CHIP RAM.
 * @pll_on: indicate whether to siwtch CPU and system clock to PLL Clock during Flash XIP mode.
 * @xip_ctrl: descript the XIP controller.
 * @cmd_sequence: include the external device's command, which makes the device enter a special mode,
 *    such as, from SPI to QIO or QPI.
 * @pll_param: indicate how to config PLL in RF side. its format as below:
 *    31      23        15      7       0
 *    -----------------------------------
 *    | valid | address | value | delay |
 *    -----------------------------------
 * @qspi_speed: while flash XIP is not enabled, user can raise up the baud of the QSPI, which connect
 *    to external flash device. unit is kbps.
 * @sec_dbg_cert_valid: indicate whether debug certify is valid.
 * @sec_dbg_cert_addr/size: indicate where the debug certify local on and its size.
 * @pload_crc_granularity: indicate the size of each CRC compution.
 * @boot_timeout: the max waiting time before firmware or boot executing.
 **/
typedef struct nvm_header {
	uint32_t magic_number;
	uint32_t version;
	uint32_t qspi_speed0;
	uint32_t id_padding;

	uint32_t pload_addr;
	uint32_t pload_size;
	uint32_t ram_base;
	uint32_t exec_offset;

	uint32_t quad_flag;
	uint32_t quad_ins;
	uint32_t xip_flag;

	uint32_t pll_on;

	xip_ctrl_t xip_ctrl;
	dev_cmd_t cmd_sequence[4];

	uint32_t pll_param[24];
	uint32_t ahb_div;
	uint32_t apb_div;
	uint32_t qspi_speed;
	uint32_t sec_dbg_cert_valid;
	uint32_t sec_dbg_cert_addr;
	uint32_t sec_dbg_cert_size;
	uint32_t pload_crc_granularity;

	uint32_t boot_timeout;

	sw_version_t sw_version;
	uint32_t reserved[24];

	uint32_t crc_of_pload_crc;
	uint32_t header_crc;
} flash_header_t;


#define FLASH_XIP_OFF		(0)
#define FLASH_XIP_ON		(1)

#define PLL_CLOCK_OFF		(0)
#define PLL_CLOCK_ON		(1)

#define SEC_DBG_CERT_INVLD	(0)
#define SEC_DBG_CERT_VLD	(1)

#define FLASH_DEV_CMD_INVLD	(0)
#define FLASH_DEV_CMD_VLD	(0xffff)


/* Unit: byte. */
#define QSPI_BOOT_HEADER_SIZE		(512)
#define SYS_PAYLOAD_CRC_MAX_LEN		(4000 << 2)


#define DEF_PLL_PARAM(addr, value, delay) (\
		(((addr) & 0xFF) << 16) |\
		(((value) & 0xFF) << 8) |\
		(((delay) & 0xFF)) |\
		(1 << 24) \
		)


//#define DEF_FLASH_CMD_VAL(val)	(((val) & 0xFF) | (1 << 8))
#define DEF_FLASH_CMD(ins, address, delay, val0, val1, val2, val3) {\
	.valid = ((delay) & 0xFFFF) | (FLASH_DEV_CMD_VLD << 16),\
	.cmd = ins,\
	.addr = address,\
	.value = {val0, val1, val2, val3}\
	}

#define DEF_FLASH_CMD_INV() {\
	.valid = 0,\
	.cmd = 0,\
	.addr = 0,\
	.value = {0, 0, 0, 0}\
	}

flash_header_t *flash_header_get(void);

#endif
