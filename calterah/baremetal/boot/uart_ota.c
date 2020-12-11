#include "embARC.h"
#include "system.h"
#include "flash.h"
#include "flash_header.h"
#include "flash_mmap.h"
#include "dw_uart.h"
#include "ota.h"
#include "uart.h"


#define IMAGE_RAM_BUF_BASE	(0x780000)

static uint32_t ram_image_base = IMAGE_RAM_BUF_BASE;
static uint32_t image_size = 0;

static uint32_t ext_flash_mem_base = FLASH_FIRMWARE_BASE;
static image_header_t *header_ptr = NULL;
//image_header_t *new_header_ptr = NULL;

static void uart_ota_comm_handshake(void);
static uint32_t uart_ota_image_handle(uint32_t sn, uint32_t payload_len);
static uint32_t uart_ota_program_verify(image_header_t *header_ptr);
static uint32_t uart_ota_program_handle(uint32_t verify);
static int32_t uart_ota_send_ack(uint32_t ack);

/*
 * handshake frame:
 *************************************
 *  magic number  *  handshake code  *
 *************************************
 **/
static void uart_ota_comm_handshake(void)
{
	int32_t result = -1;

	uint32_t len = 8;
	static uint8_t hs_msg[8] = {0};

	uint32_t magic_num = 0;
	uint32_t hs_code = 0;

	/* need to reset boot watchdog? */
	/* polling handshake package from host. */
	while (1) {
		result = uart_read(hs_msg, len);
		if (0 != result) {
			/* TODO: reset and reinit uart device. */
			continue;
		}

		magic_num = UART_OTA_FRAME_MAGIC_NUM(hs_msg);
		hs_code = UART_OTA_HS_CODE(&hs_msg[4]);

		if ((UART_OTA_COM_MAGIC_NUM == magic_num) && \
		    (UART_OTA_COM_HS_CODE == hs_code)) {
			result = uart_write(hs_msg, len);
			if (0 == result) {
				break;
			}
			/* TODO: reset and reinit uart device? */
		}


		if ((UART_OTA_START_STR0 == magic_num) && (UART_OTA_START_STR1 == hs_code)) {
			result = uart_read(hs_msg, 2);
		}
	}
}

static uint32_t uart_ota_image_handle(uint32_t sn, uint32_t payload_len)
{
	int32_t result = 0;

	uint32_t ack = 0;
	uint32_t crc32 = 0, rx_crc32 = 0;

	static uint32_t last_sn = 0;
	uint32_t rx_len = payload_len + sizeof(uint32_t);
	uint8_t *payload_ptr = (uint8_t *)(ram_image_base + image_size);

	do {
		if (sn != last_sn) {
			ack = ACK_SN_ORDER_ERROR;
			break;
		}

		result = uart_read(payload_ptr, rx_len);
		if (0 != result) {
			ack = ACK_DEVICE_ACCESS_FAILED;
			break;
		}

		/* check crc. */
		//rx_crc32 = *(uint32_t *)(payload_ptr + payload_len);
		rx_crc32 = UART_OTA_FRAME_CRC32(payload_ptr + payload_len);
		crc32 = update_crc(0, payload_ptr, payload_len);
		if (crc32 != rx_crc32) {
			ack = ACK_COMM_CRC32_UNMATCHED;
			break;
		}

		last_sn += 1;
		//ram_image_base += payload_len;
		image_size += payload_len;

	} while (0);

	return ack;
}

static uint8_t flash_read_data_buffer[4096];
static uint32_t uart_ota_program_verify(image_header_t *header_ptr)
{
	uint32_t ack = 0;
	int32_t result = 0;

	uint32_t crc32_buffer[256];
	uint32_t *crc32_ptr = NULL;
	uint32_t crc32, data_size = 0;
	uint32_t single_rd_size = 0;

	do {
		if (NULL == header_ptr) {
			ack = ACK_FUNC_PARAMS_ERROR;
			break;
		}

		/* check the program operation. */
		data_size = header_ptr->payload_size;
		ext_flash_mem_base = ext_flash_mem_base + sizeof(image_header_t);
		single_rd_size = header_ptr->crc_part_size;

		if (chip_security()) {
			/* decrypt: */
			crc32_ptr = crc32_buffer;
		} else {
			uint32_t real_image_base = ram_image_base + sizeof(image_header_t);
			crc32_ptr = (uint32_t *)(real_image_base + data_size);
		}

		while (data_size) {
			if (data_size < header_ptr->crc_part_size) {
				single_rd_size = data_size;
			}

			result = flash_memory_read(ext_flash_mem_base, \
					flash_read_data_buffer, \
					header_ptr->crc_part_size);
			if (0 != result) {
				ack = ACK_NOR_FLASH_PROGRAM_FAILED;
				break;
			}

			crc32 = update_crc(0, flash_read_data_buffer, single_rd_size);
			if (*crc32_ptr++ != crc32) {
				ack = ACK_NVM_CRC32_UNMATCHED;
				break;
			}

			ext_flash_mem_base += header_ptr->crc_part_size;
			data_size -= header_ptr->crc_part_size;
		}

	} while (0);

	return ack;
}

static uint32_t uart_ota_program_handle(uint32_t verify)
{
	int32_t result = 0;

	uint32_t ack = 0;
	uint32_t data_size = image_size;
	image_header_t old_header;

#if 1
	do {
#if 0
		if (chip_security()) {
			/* firstly decrypt the flash header. */
			header_ptr = &header;
		} else {
			header_ptr = (image_header_t *)ram_image_base;
		}
#else
		header_ptr = (image_header_t *)ram_image_base;
#endif

		/* read image header from external flash. */
		result = flash_memory_read(ext_flash_mem_base, \
				(uint8_t *)&old_header, \
				sizeof(image_header_t));
		if (0 != result) {
			ack = ACK_NOR_FLASH_READ_FAILED;
			break;
		}

		if (old_header.magic_number != header_ptr->magic_number) {
			ack = ACK_NVM_MAGIC_NUM_UNMATCHED;
			break;
		}

		if (old_header.sw_version.major_ver_id > header_ptr->sw_version.major_ver_id) {
			ack = ACK_SW_VERSION_ERROR;
			break;
		} else if (old_header.sw_version.major_ver_id == header_ptr->sw_version.major_ver_id) {
			if (old_header.sw_version.minor_ver_id > header_ptr->sw_version.minor_ver_id){
				ack = ACK_SW_VERSION_ERROR;
				break;
			} else if (old_header.sw_version.minor_ver_id == header_ptr->sw_version.minor_ver_id) {
				if (old_header.sw_version.stage_ver_id > header_ptr->sw_version.stage_ver_id){
					ack = ACK_SW_VERSION_ERROR;
					break;
				}
			}
		}

		result = flash_memory_erase(ext_flash_mem_base, data_size);
		if (0 != result) {
			ack = ACK_NOR_FLASH_ERASE_FAILED;
			break;
		}

		result = flash_memory_write(ext_flash_mem_base, \
				(const uint8_t *)IMAGE_RAM_BUF_BASE, data_size);
		if (0 != result) {
			ack = ACK_NOR_FLASH_PROGRAM_FAILED;
			break;
		}

		if (0 != verify) {
			ack = uart_ota_program_verify(header_ptr);
		}

	} while (0);
#endif

	return ack;
}

static int32_t uart_ota_send_ack(uint32_t ack)
{
	/*
	 ********************************************************
	 *  magic umber  *  cmd_id  *  sn  *  result  *  crc32  *
	 ********************************************************
	 * */
	uint32_t ack_msg[3] = {UART_OTA_COM_MAGIC_NUM, ack, update_crc(0, (uint8_t *)ack, 4)};
	return uart_write((uint8_t *)ack_msg, 12);
}

#define UART_OTA_HEADER_LEN		(12)
void uart_ota_main(void)
{
	int32_t result = E_OK;

	uint32_t ack = 0;

	uint8_t header[12] = {0};
	uint8_t cmd_id, sn;
	uint16_t payload_len = 0;
	//uint8_t *payload_ptr = NULL;
	uint32_t com_magic_num, rx_crc32, crc32 = 0;

	do {
		/* handshake with host: maybe enter watchdog timeout flow. */
		uart_ota_comm_handshake();
		//session_state = COMM_READY;

		while (1) {
			/* wait command:
			 *********************************************************
			 *  magic number  *  cmd_id  *  SN  *  length  *  CRC32  *
			 *********************************************************
			 * */
			result = uart_read(header, UART_OTA_HEADER_LEN);
			if (0 != result) {
				/* reset and re-init uart device. */
				break;
			}

			/* check magic number and crc32, gain command id. */
			com_magic_num = UART_OTA_FRAME_MAGIC_NUM(header);
			cmd_id = header[4];
			sn = header[5];
			payload_len = (header[7] << 8) | header[6];
			rx_crc32 = UART_OTA_FRAME_CRC32(&header[8]);
			if (UART_OTA_COM_MAGIC_NUM != com_magic_num) {
				ack = ACK_COMM_MAGIC_NUM_UNMATCHED;
			} else {
				crc32 = update_crc(0, &header[4], 4);
				if (rx_crc32 != crc32) {
					ack = ACK_COMM_CRC32_UNMATCHED;
				}
			}

			if (0 == ack) {
				switch (cmd_id) {
					case UART_OTA_CMD_IMAGE:
						ack = uart_ota_image_handle(sn, payload_len);
						break;

					case UART_OTA_CMD_ECU_RESET:
						/* call ecu_reset(void). */
						reboot_cause_set(ECU_REBOOT_NORMAL);
						chip_hw_udelay(2000);
						raw_writel(REG_EMU_BOOT_DONE,0x1);
						chip_reset();
						break;

					case UART_OTA_PROGRAM_IMAGE:
						ack = uart_ota_program_handle(sn);
						break;

					default:
						ack = ACK_COMM_CMD_ID_INVALID;
						break;
				}
			}

			ack = UART_OTA_ACK(cmd_id, sn, ack);
			result = uart_ota_send_ack(ack);
			if (0 != result) {
				break;
			} else {
				ack = 0;
			}
		}

	} while (0);

	/* Error: turn on the indicator light after boot watchdog timeout. */
}

