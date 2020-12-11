#ifndef _VENDOR_H_
#define _VENDOR_H_

#define CMD_RD_JEDEC_WC		(8)

#define FLASH_DEV_SAMPLE_DELAY	(0)
#define FLASH_DEV_DUMMY_CYCLE	(4)
#define FLASH_DEV_MODE_CYCLE	(2)


/* Add for boot split feature: */
#define FLASH_DEV_CMD0_INS	(0x06)
#define FLASH_DEV_CMD1_INS	(0x31)
#define FLASH_DEV_CMD2_INS	(0)
#define FLASH_DEV_CMD3_INS	(0)
/* 31 --- 24 23 --- 0 */
/*  valid   *  addr * */
#define FLASH_DEV_CMD0_ADDR	(0)
#define FLASH_DEV_CMD1_ADDR	(0)
#define FLASH_DEV_CMD2_ADDR	(0)
#define FLASH_DEV_CMD3_ADDR	(0)
/* 31 --- 8 7 --- 0 */
/*  valid  * value */
#define FLASH_DEV_CMD0_VAL0	(0)
#define FLASH_DEV_CMD0_VAL1	(0)
#define FLASH_DEV_CMD0_VAL2	(0)
#define FLASH_DEV_CMD0_VAL3	(0)
#define FLASH_DEV_CMD1_VAL0	(0xff02)
#define FLASH_DEV_CMD1_VAL1	(0)
#define FLASH_DEV_CMD1_VAL2	(0)
#define FLASH_DEV_CMD1_VAL3	(0)
#define FLASH_DEV_CMD2_VAL0	(0)
#define FLASH_DEV_CMD2_VAL1	(0)
#define FLASH_DEV_CMD2_VAL2	(0)
#define FLASH_DEV_CMD2_VAL3	(0)
#define FLASH_DEV_CMD3_VAL0	(0)
#define FLASH_DEV_CMD3_VAL1	(0)
#define FLASH_DEV_CMD3_VAL2	(0)
#define FLASH_DEV_CMD3_VAL3	(0)

/* (ins, addr, delay(ms), val0-val3(lowest byte, others as valid flag)). */
#define FLASH_DEV_CMD0		DEF_FLASH_CMD(FLASH_DEV_CMD0_INS, 0, 32, FLASH_DEV_CMD0_VAL0, FLASH_DEV_CMD0_VAL1, FLASH_DEV_CMD0_VAL2, FLASH_DEV_CMD0_VAL3)
#define FLASH_DEV_CMD1		DEF_FLASH_CMD(FLASH_DEV_CMD1_INS, 0, 32, FLASH_DEV_CMD1_VAL0, FLASH_DEV_CMD1_VAL1, FLASH_DEV_CMD1_VAL2, FLASH_DEV_CMD1_VAL3)
#define FLASH_DEV_CMD2		DEF_FLASH_CMD_INV()
#define FLASH_DEV_CMD3		DEF_FLASH_CMD_INV()

#endif
