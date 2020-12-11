#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "flash_mmap.h"

/* QSPI Frame Format.
 * 0->std, 1->dual, 2->quad, 3->octal. */
#define CONFIG_QSPI_FF			0

/* QSPI Baud Rate(bps). */
#define CONFIG_QSPI_BAUD		1000000


/* XIP config: */
/* XIP endian, 0->little, 1->big? */
#define CONFIG_XIP_RD_ENDIAN		0
#define CONFIG_XIP_WR_ENDIAN		0

/* XIP buffer length(word),
 * 0->16, 1->32, 2->64, 3->128, 4->256, 5->512. */
#define CONFIG_XIP_INS_BUF_LEN		1
#define CONFIG_XIP_RD_BUF_LEN		1
#define CONFIG_XIP_WR_BUF_LEN		4

/* XIP section offset. */
#define CONFIG_XIP_INS_OFFSET		FLASH_HEADER_BASE
#define CONFIG_XIP_DATA_OFFSET		FLASH_HEADER_BASE

/* AES config: */
#define CONFIG_AES_VLD_BLK_CNT		4
#define CONFIG_AES_LAST_BLK_SIZE	0x80


/* security: byte. */
#define CONFIG_SEC_DBG_CERT_LEN		16
#endif
