#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "arc_exception.h"

#include "hw_crc.h"

#ifdef OS_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#endif

#define SW_CRC32		0

#ifndef OS_FREERTOS
#define CRC_LOCK(lock)
#define CRC_UNLOCK(lock)
#else
#define CRC_LOCK(lock)		while (xSemaphoreTake(lock, portMAX_DELAY) != pdTRUE) {}
#define CRC_UNLOCK(lock)	xSemaphoreGive(lock)

static xSemaphoreHandle crclock;
#endif

#if SW_CRC32
#define POLYNOMIAL 0xD419CC15L
static unsigned long crc32_table[256];
#endif

static void (*crc_complete_callback)(void *);

static void gen_crc_table(void);
static void crc_complete_isr(void *params);
static void crc_error_isr(void *params);
int32_t crc_init(crc_poly_t poly, uint32_t mode)
{
	int32_t result = E_OK;

	//uint8_t err_int;
	//uint8_t com_int;
	hw_crc_t *crc_dev = (hw_crc_t *)crc_get_dev(0);

	do {
		if (NULL == crc_dev) {
			result = E_NOEXS;
			break;
		}

		crc_enable(1);

		hw_crc_mode(poly, mode);

		/* interrupt install. */
		/*
		err_int = crc_dev->err_int;
		result = int_handler_install(err_int, crc_error_isr);
		if (E_OK != result) {
			break;
		} else {
			hw_crc_interrupt_en(INT_HW_CRC_FAIL, 1);
			dmu_irq_enable(err_int, 1);
			int_enable(err_int);
		}
		*/

		/*
		com_int = crc_dev->complete_int;
		result = int_handler_install(com_int, crc_complete_isr);
		if (E_OK != result) {
			break;
		} else {
			hw_crc_interrupt_en(INT_HW_CRC_COMPLETE, 0);
			dmu_irq_enable(com_int, 1);
			int_enable(com_int);
		}
		*/
		hw_crc_interrupt_en(INT_HW_CRC_COMPLETE, 1);
		hw_crc_interrupt_en(INT_HW_CRC_FAIL, 0);
		hw_crc_interrupt_en(INT_HW_CRC_SUCCESS, 0);

#ifdef OS_FREERTOS
		crclock = xSemaphoreCreateBinary();
		xSemaphoreGive(crclock);
#endif
	} while (0);

#if SW_CRC32
	gen_crc_table();
#endif

	return result;
}

int32_t crc32_update(uint32_t crc, uint32_t *data, uint32_t len)
{
	int32_t result = E_OK;

	uint32_t idx, single_len = 0xFFFF;

	if ((NULL == data) || (0 == len)) {
		result = E_PAR;
	} else {
		//CRC_LOCK(crclock);
		while (len) {
			if (len < single_len) {
				single_len = len;
			}

			hw_crc_init_value(crc);
			hw_crc_count(single_len);

			for (idx = 0; idx < single_len; idx++) {
				hw_crc_raw_data(*data++);
			}

			HW_CRC_WAIT_COMPLETED();
			crc = hw_crc_sector_value();
			len -= single_len;
		}
		//CRC_UNLOCK(crclock);
	}

	return result;
}

int32_t crc16_update(uint32_t crc, uint16_t *data, uint32_t len, void (*func)(void *))
{
	int32_t result = E_OK;

	uint32_t idx, single_len = 0xFFFF;

	if ((NULL == data) || (0 == len)) {
		result = E_PAR;
	} else {
		//CRC_LOCK(crclock);
		while (len) {
			if (len < single_len) {
				single_len = len;
			}

			hw_crc_init_value(crc);
			hw_crc_count(single_len);

			for (idx = 0; idx < single_len; idx++) {
				hw_crc_raw_data((uint32_t)*data++);
			}

			HW_CRC_WAIT_COMPLETED();
			crc = hw_crc_sector_value();
			len -= single_len;
		}
		//CRC_UNLOCK(crclock);
	}

	return result;
}

uint32_t crc_output(void)
{
	return hw_crc_sector_value();
}

uint32_t crc_init_value(void)
{
	return hw_crc_init_value_get();
}

static void crc_complete_isr(void *params)
{
	if (NULL != crc_complete_callback) {
		crc_complete_callback(params);
	}
}

static void crc_error_isr(void *params)
{
	/* TODO: record error! */
}

#if SW_CRC32
static void gen_crc_table(void)
{
	int i, j;
	unsigned int crc_accum;

	for(i=0; i < 256; i++) {
		crc_accum = ((unsigned int) i << 24);
		for(j=0;  j < 8;  j++) {
			if (crc_accum & 0x80000000L) {
				crc_accum = (crc_accum << 1) ^ POLYNOMIAL;
			} else {
				crc_accum = (crc_accum << 1);
			}
		}
		crc32_table[i] = crc_accum;
	}
}

unsigned int update_crc(unsigned int crc_accum, unsigned char *datap, unsigned int datak_size)
{
	int i, j;
	for(j = 0; j < datak_size; j++) {
		i = ((unsigned int) (crc_accum >> 24) ^ *datap++) & 0xff;
		crc_accum = (crc_accum << 8) ^ crc32_table[i];
	}
	return crc_accum;
}
#endif
