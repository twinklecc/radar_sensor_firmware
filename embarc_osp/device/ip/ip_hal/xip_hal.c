#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "arc_exception.h"
#include "xip_hal.h"
#include "xip.h"
#include "dw_ssi.h"
#include "config.h"
#include "device.h"
#include "instruction.h"
#include "flash.h"
#include "flash_header.h"
#include "dw_ssi_reg.h"
#include "alps/alps.h"
#include "vendor.h"
#include "uart_hal.h"

#ifdef OS_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#endif

static xip_t *flash_xip_ptr = NULL;

int32_t flash_xip_init(void)
{
	int32_t result = E_OK;

	do {
		flash_xip_ptr = (xip_t *)xip_get_dev();
		if ((NULL == flash_xip_ptr)) {
			result = E_NOEXS;
			break;
		}

		/* if none security, call it. */
		if (0 == (raw_readl(OTP_MMAP_SEC_BASE) & (1 << 0))) {
			aes_cfg_t aes_cfg = {
				.path_sel = AES_IN_DATA_PATH,
				.data_path_bypass = 0,
				.tweak_mode = AES_TWEAK_BY_ADDR_BLOCKSZ_MASK,
				.tweak_mask= 0,
				.seed = 0
			};

			result = flash_xip_aes_config(flash_xip_ptr, &aes_cfg);
			if (E_OK != result) {
				break;
			}
		}

	} while(0);

	return result;
}


static int32_t _flash_xip_read(uint32_t addr, void *buf, uint32_t len, uint8_t flag)
{
	int32_t result = E_OK;

	uint32_t flash_mmap_pos = 0, cnt = 0;
	uint8_t *pbuf_byte = (uint8_t *)buf;
	uint32_t *pbuf_word = (uint32_t *)buf;
	uint32_t src = 0;

	do {
		if ((addr < CONFIG_XIP_DATA_OFFSET) \
			|| ((addr > (CONFIG_XIP_DATA_OFFSET + FLASH_BOOT_BASE)) \
			&& (addr < (CONFIG_XIP_DATA_OFFSET + FLASH_ANGLE_CALIBRATION_INFO_BASE))) \
			|| (addr > (CONFIG_XIP_DATA_OFFSET + REL_XIP_MMAP_LEN))) {
			/* invalid address. */
			break;
		}

		flash_mmap_pos = XIP_MEM_BASE_ADDR + (addr - CONFIG_XIP_DATA_OFFSET);
		if (flash_mmap_pos + len > REL_XIP_MMAP_MAX) {
			/* overflow! */
			break;
		}

		if ((buf == NULL) || (flag < 0)) {
			result = E_PAR;
		}

		if (!flag)
			len = (len / sizeof(uint32_t)) + 1;

		while(len--) {
			if (flag) {
				*pbuf_word++ = raw_readl(flash_mmap_pos + cnt);
				cnt += 4;
			} else {
				src = raw_readl(flash_mmap_pos + cnt);

				if (len == 0) {
					for (uint32_t i = 0; i < (len / sizeof(uint32_t)); i++) {
						*pbuf_byte++ = (uint8_t)(src >> (i << 3));
					}
				} else {
					for (uint32_t i = 0; i < sizeof(uint32_t); i++) {
						*pbuf_byte++ = (uint8_t)(src >> (i << 3));
					}
				}
				cnt += 4;
			}
		}
	} while(0);

	return result;
}

int32_t flash_xip_readb(uint32_t addr, uint8_t *buf, uint32_t len)
{
	return _flash_xip_read(addr, (void *)buf, len, 0);

}

int32_t flash_xip_read(uint32_t addr, uint32_t *buf, uint32_t len)
{
	return _flash_xip_read(addr, (void *)buf, len, 1);
}

int32_t flash_xip_program(uint32_t addr, uint32_t *buf, uint32_t len)
{
	int32_t result = E_OK;

	uint32_t cnt = 0;
	uint32_t flash_mmap_pos = 0;


	if ((flash_xip_ptr) && (addr >= CONFIG_XIP_DATA_OFFSET) && \
	    (addr - CONFIG_XIP_DATA_OFFSET < REL_XIP_MMAP_LEN) && \
	    (addr + len <= REL_XIP_MMAP_MAX)) {
		uint32_t single_cnt = 1 << (4 + CONFIG_XIP_WR_BUF_LEN);

		flash_mmap_pos = addr - CONFIG_XIP_DATA_OFFSET;
		while (len) {
			if (len < single_cnt) {
				single_cnt = len;
				if (single_cnt & 0x3) {
					single_cnt = ((single_cnt >> 2) + 1) << 2;
				}
			}

			while (flash_xip_program_in_progress(flash_xip_ptr) ||\
				flash_xip_erase_in_progress(flash_xip_ptr)) {
#ifdef OS_FREERTOS
				vTaskDelay(2);
#endif
			}

			for (cnt = 0; cnt < single_cnt; cnt += 4) {
				raw_writel(flash_mmap_pos + cnt, *buf++);
			}
			flash_xip_program_en(flash_xip_ptr);

			len -= single_cnt;
			flash_mmap_pos += single_cnt;
		}
	} else {
		result = E_PAR;
	}

	return result;
}

int32_t flash_xip_program_done(void)
{
	int32_t result = E_OK;

	if ((NULL == flash_xip_ptr)) {
		result = E_SYS;
	} else {
		result = flash_xip_program_in_progress(flash_xip_ptr);
		if (0 == result) {
			result = 1;
		} else {
			result = 0;
		}
	}

	return result;
}


int32_t flash_xip_encrypt(uint32_t *src, uint32_t *dst, uint32_t len)
{
	int32_t result = E_OK;

	uint32_t single_cnt = 64;

	do {
		if ((NULL == src) || (NULL == dst) || (len % 64)) {
			result = E_PAR;
			break;
		}
		if (NULL == flash_xip_ptr) {
			result = E_SYS;
			break;
		}

		flash_xip_aes_mode_set(flash_xip_ptr, CIPHER_MODE);

		while (len) {
			if (len < 64) {
				single_cnt = len;
				if (single_cnt % 4) {
					single_cnt = ((single_cnt >> 2) + 1) << 2;
				}
			}

			flash_xip_aes_input(flash_xip_ptr, src, single_cnt >> 2);
			flash_xip_aes_start(flash_xip_ptr, 0, 1, 1);

			while (0 == flash_xip_aes_done(flash_xip_ptr)) {
#ifdef OS_FREERTOS
				vTaskDelay(2);
#endif
			}
			result = falsh_xip_aes_ouput(flash_xip_ptr, dst, single_cnt >> 2);
			if (E_OK != result) {
				break;
			}

			len -= single_cnt;
			src += single_cnt >> 2;
			dst += single_cnt >> 2;
		}
	} while (0);

	return result;
}

int32_t flash_xip_decrypt(uint32_t *src, uint32_t *dst, uint32_t len)
{
	int32_t result = E_OK;

	uint32_t single_cnt = 64;

	do {
		if ((NULL == src) || (NULL == dst) || (len % 64)) {
			result = E_PAR;
			break;
		}
		if (NULL == flash_xip_ptr) {
			result = E_SYS;
			break;
		}

		flash_xip_aes_mode_set(flash_xip_ptr, DECIPHER_MODE);

		while (len) {
			if (len < 64) {
				single_cnt = len;
				if (single_cnt % 4) {
					single_cnt = ((single_cnt >> 2) + 1) << 2;
				}
			}

			flash_xip_aes_input(flash_xip_ptr, src, single_cnt >> 2);
			flash_xip_aes_start(flash_xip_ptr, 0, 1, 1);

			while (0 == flash_xip_aes_done(flash_xip_ptr)) {
#ifdef OS_FREERTOS
				vTaskDelay(2);
#endif
			}
			result = falsh_xip_aes_ouput(flash_xip_ptr, dst, single_cnt >> 2);
			if (E_OK != result) {
				break;
			}

			len -= single_cnt;
			src += single_cnt >> 2;
			dst += single_cnt >> 2;
		}
	} while (0);

	return result;
}
