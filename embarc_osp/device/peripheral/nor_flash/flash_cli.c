#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "embARC_toolchain.h"
#include "embARC.h"
#include "embARC_error.h"
#include "embARC_debug.h"
#include "arc_exception.h"
#include "board.h"
#include "dw_ssi.h"
#include "xip_hal.h"
#include "device.h"
#include "instruction.h"
#include "cfi.h"
#include "sfdp.h"
#include "flash.h"

#ifdef OS_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_CLI.h"

#define FLASH_TEST_COMMAND		(1)

static BaseType_t flash_erase_command_handle(char *pcWriteBuffer, \
		size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1, len2;
	uint32_t addr, length;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
	addr = (uint32_t)strtol(param1, NULL, 0);
	length = (uint32_t)strtol(param2, NULL, 0);

	EMBARC_PRINTF("addr: 0x%x, length: 0x%x\r\n", addr, length);
	flash_memory_erase(addr, length);

	return pdFALSE;
}
static const CLI_Command_Definition_t flash_erase_command =
{
	"flash_erase",
	"\rflash_erase:"
	"\r\n\tflash_erase [address] [length(byte)]\r\n\r\n",
	flash_erase_command_handle,
	-1
};

static BaseType_t flash_read_command_handle(char *pcWriteBuffer, \
		size_t xWriteBufferLen, const char *pcCommandString)
{
    BaseType_t len1, len2;
	uint32_t addr, length;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
	addr = (uint32_t)strtol(param1, NULL, 0);
	length = (uint32_t)strtol(param2, NULL, 0); //number of words
	length = length << 2;      //number of bytes
	//start
	int32_t status;
    uint8_t buff[512];
    status = flash_memory_read(addr, buff, length);
	if (status != E_OK) {
            snprintf(pcWriteBuffer, xWriteBufferLen - 1, "Fail to read on-flash steering vector info!\n\r");
    } else {
		   uint32_t *tmp = (uint32_t *)buff;
		   for (int cnt = 0; cnt < (length>>2); cnt++)
                EMBARC_PRINTF("addr(Bytes): 0x%x value: 0x%x\r\n", addr+cnt*4, tmp[cnt]);
	}
    //end
	return pdFALSE;
}

static const CLI_Command_Definition_t flash_read_command =
{
	"flash_read",
	"\rflash_read:"
	"\r\n\tflash_read [address] [length(words)]128 words most\r\n\r\n",
	flash_read_command_handle,
	-1
};

#if FLASH_TEST_COMMAND
static BaseType_t flash_memory_write_command_handle(char *pcWriteBuffer, \
		size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1, len2, len3;
	uint32_t addr, data, length;
	uint32_t buffer[1024];

	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
	const char *param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);
	addr = (uint32_t)strtol(param1, NULL, 0);
	data = (uint32_t)strtol(param2, NULL, 0);
	length = (uint32_t)strtol(param3, NULL, 0);

	EMBARC_PRINTF("addr: 0x%x, data: 0x%x length: 0x%x\r\n", addr, data, length);

	for (uint32_t i = 0; i < length; i++) {
		buffer[i] = (i << 16) | data;
	}

	flash_memory_writew(addr, buffer, length);

	return pdFALSE;
}

static const CLI_Command_Definition_t flash_memory_write_command =
{
	"flash_memory_write",
	"\rflash_memory_write:"
	"\r\n\tflash_memory_write [address] [length(words)] most 1024\r\n\r\n",
	flash_memory_write_command_handle,
	-1
};

static BaseType_t flash_memory_read_command_handle(char *pcWriteBuffer, \
		size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1, len2;
	uint32_t addr, length;
	uint32_t buffer;

	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
	addr = (uint32_t)strtol(param1, NULL, 0);
	length = (uint32_t)strtol(param2, NULL, 0);

	for (uint32_t i = 0; i < length; i++) {
		if ((i == 0) || (i % 4 == 0)) {
			EMBARC_PRINTF("%08x", (addr + (i * 4)));
		}

		flash_memory_readw(addr + i * 4, &buffer, 1);
		if ((i+1) % 4 == 0) {
			EMBARC_PRINTF("\t %08x \r\n", buffer);
		} else {
			EMBARC_PRINTF("\t %08x", buffer);
		}
	}

	return pdFALSE;
}

static const CLI_Command_Definition_t flash_memory_read_command =
{
	"flash_memory_read",
	"\rflash_memory_read:"
	"\r\n\tflash_memory_read [address] [length(words)]\r\n\r\n",
	flash_memory_read_command_handle,
	-1
};

#endif

void flash_cli_register(void)
{
	FreeRTOS_CLIRegisterCommand(&flash_erase_command);
	FreeRTOS_CLIRegisterCommand(&flash_read_command);
#if FLASH_TEST_COMMAND
	FreeRTOS_CLIRegisterCommand(&flash_memory_write_command);
	FreeRTOS_CLIRegisterCommand(&flash_memory_read_command);
#endif
}
#endif
