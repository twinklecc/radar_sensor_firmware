#include "embARC.h"
#include "system.h"
#include "dw_ssi_reg.h"
#include "dw_ssi.h"
#include "flash.h"
#include "flash_header.h"
#include "flash_mmap.h"
#include "instruction.h"
#include "vendor.h"
#include "crc_hal.h"
#include "xip_hal.h"
#include "config.h"

#define BOOT_CRC_ENABLE				(1)

#ifndef BOOT_SPLIT
#include "xip_early.c"
#endif

static void aes_init(void);
static int aes_decrypt(uint32_t *data_in, uint32_t *data_out, uint32_t len);

int32_t normal_boot(void)
{
	int32_t result = 0;

	image_header_t image_header;
	image_header_t *image_header_ptr = (image_header_t *)&image_header;

	uint32_t crc32_size = 0;
#if BOOT_CRC_ENABLE
	uint8_t *image_ptr = NULL;
	uint32_t *crc32_ptr = NULL;
	uint32_t crc32 = 0;
	uint32_t single_crc_size = 0;
#endif
	uint32_t payload_size = 0;

	next_image_entry firmware_entry;

	do {
		result = crc_init(0, 1);
		if (E_OK != result) {
			break;
		}

		/* read image header. */
		result = flash_memory_read(FLASH_FIRMWARE_BASE, (uint8_t *)image_header_ptr, sizeof(image_header_t));
		if (0 != result) {
			break;
		}

        /* Check whether AES is enabled */
        if (raw_readl(0xb00004) & 1) {
                /* aes init process */
                aes_init();

                /* Start AES decrypt process to decrypt firmware image header */
                aes_decrypt((uint32_t *)image_header_ptr, (uint32_t *)image_header_ptr, sizeof(image_header_t));
        }

		if (image_header_ptr->xip_en) {
            /* Boot firmware image under XIP mode */
			/* send command to configure external nor flash to Quad mode. */
			result = flash_quad_entry();
			if (0 != result) {
				break;
			}

			/* firmware memory mapping address */
			image_ptr = (uint8_t *)(FLASH_MMAP_FIRMWARE_BASE + image_header_ptr->payload_addr);

			/* configure XIP controller: */
			flash_xip_init_early();

		} else {
		    /* Boot firmware image under "copy to ram" mode */
            /* calculate the amount of crc */
			crc32_size = image_header_ptr->payload_size / image_header_ptr->crc_part_size;
			if (image_header_ptr->payload_size % image_header_ptr->crc_part_size) {
				crc32_size += 1;
			}
			crc32_size <<= 2;

            /* Check whether AES is enabled */
            if (raw_readl(0xb00004) & 1) {
                /* AES is enabled */
                uint32_t read_count = image_header_ptr->payload_size + crc32_size;

                /* align read_count to 4 byte */
                if (image_header_ptr->payload_size % 0x4) {
                    read_count += 4 - (image_header_ptr->payload_size % 0x4);
                }

                /* align read_count to 64 byte. */
                if (read_count % 64) {
                    read_count += 64 - (read_count % 64);
                }

                /* Read firmware image from external flash and load the image data to RAM space at "ram_base" address */
                result = flash_memory_read(image_header_ptr->payload_addr, (uint8_t *)image_header_ptr->ram_base, read_count);
                if (0 != result) {
                    break;
                }

                /* aes init process */
                aes_init();

                /* Start AES decrypt process to decrypt firmware image */
                aes_decrypt((uint32_t *)image_header_ptr->ram_base, (uint32_t *)image_header_ptr->ram_base, read_count);
            } else {
                /* AES is disabled */
                /* Only need to Read firmware image from external flash and load image data to RAM space at "ram_base" address */
                result = flash_memory_read(image_header_ptr->payload_addr, (uint8_t *)image_header_ptr->ram_base, (image_header_ptr->payload_size + crc32_size));
                if (0 != result) {
                    break;
                }
            }

			/* firmware memory address */
			image_ptr = (uint8_t *)image_header_ptr->ram_base;
		}

		firmware_entry = (next_image_entry)(image_ptr + image_header_ptr->exec_offset);

#if BOOT_CRC_ENABLE
		crc32_ptr = (uint32_t *)(image_ptr + image_header_ptr->payload_size);
		payload_size = image_header_ptr->payload_size;
		single_crc_size = image_header_ptr->crc_part_size;
		while (payload_size) {
			if (payload_size < single_crc_size) {
				single_crc_size = payload_size;
			}

			crc32 = update_crc(0, image_ptr, single_crc_size);
			//crc32_update(0, image_ptr, single_crc_size);
			//crc32 = crc_output();
			if (crc32 != *crc32_ptr) {
				result = -1;
				break;
			}
			crc32_ptr++;
			image_ptr += single_crc_size;
			payload_size -= single_crc_size;
		}
#endif

		if (0 == payload_size) {
			_arc_aux_write(0x4B, 1);
			while (_arc_aux_read(0x48) & 0x100);
			icache_invalidate();

			/* jump to the next image. */
			firmware_entry();
		}
	} while (0);

	return result;
}

static void aes_init(void)
{
    /* Enable XIP clock */
    /* AES function is part of XIP module, so before use AES function, need to enable XIP clock */
    xip_enable(1);
    chip_hw_udelay(100);
    /* Set AES Mode Register - AMR */
    raw_writel(0xd00124, 0x2);
    /* Set AES Cipher/Decipher Mode Register - decipher mode */
    raw_writel(0xd00128, 1);
    /* Set AES Valid Block Register - AVBR */
    raw_writel(0xd00140, 4);
    /* Set AES Last Block Size Register - ALBSR */
    raw_writel(0xd00144, 0x80);
}

#define REG_FLASH_AES_DIN_OFFSET(x, y)	(((0x12 + ((x) << 2) + (y)) << 2))
#define REG_FLASH_AES_DOUT_OFFSET(x, y) (((0x26 + ((x) << 2) + (y)) << 2))
static int aes_decrypt(uint32_t *data_in, uint32_t *data_out, uint32_t len)
{
    int ret = 0;

    unsigned int i, j;

    if (len % 64) {
        ret = -1;
    } else {
        while (len) {
            for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                raw_writel(REG_FLASH_AES_DIN_OFFSET(i, j) + 0xd00100, *data_in++);
            }
            }

            raw_writel(0xd00188, 1);
            raw_writel(0xd0018c, 1);
            //raw_writel(REG_FLASH_AES_DIN_VAL_OFFSET +XIP_ADDR, 1);

            while (0 == (raw_readl(0xd00194) & 0x1));
            for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                *data_out++ = raw_readl(REG_FLASH_AES_DOUT_OFFSET(i, j) + 0xd00100);
            }
            }
            len -= 64;
        }
    }

    return ret;
}

