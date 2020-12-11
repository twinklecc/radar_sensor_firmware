#ifndef _FLASH_H_
#define _FLASH_H_
#include "dw_ssi.h"

typedef struct qspi_transfer_config {
	uint8_t clock_mode;
	uint8_t dfs;
	uint8_t cfs;
	uint8_t spi_frf;
} qspi_xfer_cfg_t;

/***************************************************************
** description: program external flash memory.
** @argument:
**	@addr: flash memory address.
**	@data: data buffer, qspi controller's data frame size is 8.
**	@len: data length. unit is byte.
** @return: 0 -> successful, < 0 --> failed.
*******************************************************************/
int32_t flash_memory_writeb(uint32_t addr, const uint8_t *data, uint32_t len);
int32_t flash_memory_writew(uint32_t addr, const uint32_t *data, uint32_t len);

/***************************************************************
** description: erase external flash memory.
** @argument:
**	@addr: flash memory address.
**	@len: data length. unit is byte.
**b@return: 0 -> successful, < 0 --> failed.
*******************************************************************/
int32_t flash_memory_erase(uint32_t addr, uint32_t len);

//int flash_chip_erase(void);

/***************************************************************
** description: read data from external flash memory.
** @argument:
**	@addr: flash memory address.
**	@data: receive data buffer, qspi controller's data frame size is 8.
**	@len: data length. unit is byte.
** @return: 0 -> successful, < 0 --> failed.
*******************************************************************/
int32_t flash_memory_readb(uint32_t addr, uint8_t *data, uint32_t len);
int32_t flash_memory_readw(uint32_t addr, uint32_t *data, uint32_t len);

int32_t flash_xip_decrypt(uint32_t *src, uint32_t *dst, uint32_t len);
int32_t flash_xip_encrypt(uint32_t *src, uint32_t *dst, uint32_t len);

/***************************************************************
** description: reset external flash device.
*******************************************************************/
int32_t flash_reset(void);

int32_t flash_init(void);

void flash_cli_register(void);

int32_t flash_program_resume(void);

int32_t flash_quad_entry(void);

int32_t flash_wait_status(dw_ssi_t *dw_ssi, uint32_t status, uint32_t cont_sts);

#define flash_memory_write	flash_memory_writeb
#define flash_memory_read	flash_memory_readb

#endif
