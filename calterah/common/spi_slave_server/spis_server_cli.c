#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "embARC_toolchain.h"
#include "embARC.h"
#include "embARC_error.h"
#include "embARC_debug.h"
#include "arc_exception.h"
#include "board.h"

#ifdef OS_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_CLI.h"
#endif

#include "spis_server.h"


static uint32_t txdata[0x6000 >> 2];

static BaseType_t spis_command_handle(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	int32_t result = E_OK;

	int32_t xfer_done = 0;
	static uint32_t first_xfer = 0;

        BaseType_t len1;
	uint32_t idx, length;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	length = (uint32_t)strtol(param1, NULL, 0);

	EMBARC_PRINTF("txdata: 0x%x, length: 0x%x\r\n", (uint32_t)txdata, length);
	for (idx = 0; idx < length; idx++) {
		txdata[idx] = 0x12340000 + idx;
	}

	if (0 == first_xfer) {
		first_xfer = 1;
	} else {
		xfer_done = spis_server_transmit_done();
	}
	result = spis_server_write(txdata, length);
	if (E_OK != result) {
		EMBARC_PRINTF("spis server write failed%d\r\n", result);
	}

	return pdFALSE;
}

static const CLI_Command_Definition_t spis_send_command = {
	"spis_send_example",
	"\rspis_send:"
	"\r\n\tspis_send [length(word)]\r\n\r\n",
	spis_command_handle,
	-1
};

void spis_server_register(void)
{
	FreeRTOS_CLIRegisterCommand(&spis_send_command);
}
