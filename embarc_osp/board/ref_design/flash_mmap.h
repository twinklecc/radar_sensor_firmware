#ifndef _FLASH_MMAP_H_
#define _FLASH_MMAP_H_

/* the memory mapping address for flash */
#define FLASH_MMAP_FIRMWARE_BASE				(0x300000)

/* flash address */
#define FLASH_HEADER_BASE	(0x000000)
#define FLASH_DBG_CERT_BASE	(0x004000)
#define FLASH_HEADER_BK_BASE	(0x008000)
#define FLASH_ANTENNA_INFO_BASE	(0x010000)
#define FLASH_BOOT_BASE		(0x020000)
#define FLASH_FIRMWARE_BASE	(0x030000)
#define FLASH_ANGLE_CALIBRATION_INFO_BASE	(FLASH_FIRMWARE_BASE+0x100000) //ensure firmware space 1MB

#endif
