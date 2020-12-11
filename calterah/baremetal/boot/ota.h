#ifndef _OTA_H_
#define _OTA_H_

#include "dw_uart.h"

typedef enum {
	REBOOT_UART_OTA = 0,
	REBOOT_CAN_OTA
} reboot_mode_t;


#ifdef SYSTEM_UART_OTA


/* command id:
******************************
*  cmd_id  *  SN  *  length  *
******************************/
#define UART_OTA_CMD_ECU_RESET			(0x01)
#define UART_OTA_CMD_IMAGE_INFO			(0x02)
#define UART_OTA_CMD_IMAGE			(0x03)
#define UART_OTA_PROGRAM_IMAGE			(0x04)


/* ack id:
******************************
*  cmd_id  *  sn  *  result  *
******************************/
#define ACK_DEVICE_ACCESS_FAILED		(0x0100U)
#define ACK_COMM_PAYLOAD_LEN_UNMATCHED		(0x0200U)
#define ACK_COMM_CRC32_UNMATCHED		(0x0300U)
#define ACK_NOR_FLASH_READ_FAILED		(0x0400U)
#define ACK_COMM_MAGIC_NUM_UNMATCHED		(0x0500U)
#define ACK_SW_VERSION_ERROR			(0x0600U)
#define ACK_SN_ORDER_ERROR			(0x0700U)
#define ACK_NOR_FLASH_ERASE_FAILED		(0x0800U)
#define ACK_NOR_FLASH_PROGRAM_FAILED		(0x0900U)
#define ACK_COMM_CMD_ID_INVALID			(0x0A00U)
#define ACK_NVM_CRC32_UNMATCHED			(0x0B00U)
#define ACK_NVM_MAGIC_NUM_UNMATCHED		(0x0C00U)
//#define ACK_NVM_RAM_CRC32_UNMATCHED		(0x0C00U)
#define ACK_FUNC_PARAMS_ERROR			(0x0D00U)

#define UART_OTA_ACK(cmd_id, sn, ack)	( \
		(((cmd_id) & 0xFF) << 24) | \
		(((sn) & 0xFF) << 16) | \
		((ack) & 0xFFFF))


#define UART_OTA_FLAG_CMD_INFO	(0x00000001)
#define UART_OTA_FLAG_ACK	(0xAAAAAAAA)
#define UART_OTA_FLAG_DATA	(0x55555555)


#define UART_OTA_COM_MAGIC_NUM	(0x12345678)
#define UART_OTA_COM_HS_CODE	(0xabcdef00)
#define UART_OTA_START_STR0		(0x74726175)
#define UART_OTA_START_STR1		(0x61746f5f)

#if 0
#define UART_OTA_FRAME_MAGIC_NUM(buf)	(\
		(((buf)[0]) << 24) | (((buf)[1]) << 16) | \
		(((buf)[2]) << 8) | ((buf)[3]))

#define UART_OTA_FRAME_CRC32(buf)	(\
		(((buf)[0]) << 24) | (((buf)[1]) << 16) | \
		(((buf)[2]) << 8) | ((buf)[3]))
#else
#define UART_OTA_FRAME_MAGIC_NUM(buf)	(\
		(((buf)[3]) << 24) | (((buf)[2]) << 16) | \
		(((buf)[1]) << 8) | ((buf)[0]))

#define UART_OTA_FRAME_CRC32(buf)	(\
		(((buf)[3]) << 24) | (((buf)[2]) << 16) | \
		(((buf)[1]) << 8) | ((buf)[0]))
#endif

#define UART_OTA_HS_CODE 		UART_OTA_FRAME_CRC32

void uart_ota_main(void);
#else
void uart_ota_main(void)
{
	return;
}
#endif

#ifdef SYSTEM_CAN_OTA
void can_ota_main(void);
#else
static inline void can_ota_main(void)
{
	return;
}
#endif


#endif
