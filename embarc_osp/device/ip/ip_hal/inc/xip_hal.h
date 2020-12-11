#ifndef _XIP_HAL_H_
#define _XIP_HAL_H_

#include "xip.h"

#define FLASH_COMMAND(ins, ins_len, addr_len, dfs) FLASH_XIP_CMD_INIT(ins, ins_len, addr_len, dfs)

#define XIP_MEM_BASE_ADDR 0x300000

int32_t flash_xip_readb(uint32_t addr, uint8_t *buf, uint32_t len);

int32_t flash_xip_read(uint32_t addr, uint32_t *buf, uint32_t len);

int32_t flash_xip_init(void);

int32_t flash_xip_encrypt(uint32_t *src, uint32_t *dst, uint32_t len);

int32_t flash_xip_decrypt(uint32_t *src, uint32_t *dst, uint32_t len);

#endif
