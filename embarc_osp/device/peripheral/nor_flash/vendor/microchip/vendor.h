#ifndef _VENDOR_H_
#define _VENDOR_H_

#define CMD_RD_JEDEC_WC		(8)

#define FLASH_DEV_SAMPLE_DELAY	(0)
#define FLASH_DEV_DUMMY_CYCLE	(4)
#define FLASH_DEV_MODE_CYCLE	(2)

/* (ins, addr, delay(ms), val0-val3(lowest byte, others as valid flag)). */
#define FLASH_DEV_CMD0		DEF_FLASH_CMD(CMD_WRITE_ENABLE, 0, 32, 0, 0, 0, 0)
#define FLASH_DEV_CMD1		DEF_FLASH_CMD(CMD_WRITE_STS1_CFG1, 0, 32, 0xff02, 0, 0, 0)
#define FLASH_DEV_CMD2		DEF_FLASH_CMD(0, 0, 0, 0, 0, 0, 0)
#define FLASH_DEV_CMD3		DEF_FLASH_CMD(0, 0, 0, 0, 0, 0, 0)

#endif
