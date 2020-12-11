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
#include "cascade.h"

#define DMA_TEST	0
#define CRC32_TEST	0
#define DMA_CRC_TEST	0


static uint32_t txdata[0x6000 >> 2];
static uint32_t rxdata[0x6000 >> 2];

static TaskHandle_t cs_xfer_handle = NULL;

static BaseType_t cs_command_handle(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	int32_t result = E_OK;

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
		//xfer_done = spis_server_transmit_done();
		cascade_transmit_done();
	}
	result = cascade_write(txdata, length);
	if (E_OK != result) {
		EMBARC_PRINTF("cascade write failed%d\r\n", result);
	}

	return pdFALSE;
}

static const CLI_Command_Definition_t cs_send_command = {
	"cascade_send",
	"\rcascade_send:"
	"\r\n\tcascade_send [length(word)]\r\n\r\n",
	cs_command_handle,
	-1
};

#if DMA_TEST
static uint32_t cs_dma_callback(void *params)
{
	EMBARC_PRINTF("DMA Done\r\n");
	int32_t result = dma_release_channel((uint32_t)params);
	if (E_OK != result) {
		EMBARC_PRINTF("DMA channel release failed.\r\n");
	}
}

static BaseType_t dma_test_command_handle(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	int32_t result = E_OK;

        BaseType_t len1;
	uint32_t idx, length;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	length = (uint32_t)strtol(param1, NULL, 0);

	EMBARC_PRINTF("txdata: 0x%x, length: 0x%x --> 0x%x\r\n", (uint32_t)txdata, length, (uint32_t)rxdata);
	for (idx = 0; idx < length; idx++) {
		raw_writel((uint32_t)&txdata[idx], 0x12340000 + idx);
		//txdata[idx] = 0x12340000 + idx;
	}

	result = dma_user_memcopy(rxdata, txdata, length << 2, cs_dma_callback);
	if (E_OK != result) {
		EMBARC_PRINTF("DMA copy init failed%d\r\n", result);
	}

	return pdFALSE;
}

static const CLI_Command_Definition_t dma_test_command = {
	"dma_test",
	"\rdma_test:"
	"\r\n\tdma_test [length(word)]\r\n\r\n",
	dma_test_command_handle,
	-1
};
#endif

#define ENDIAN_ORDER_REV(word) \
	((((word) & 0xFF) << 24) |\
	 ((((word) >> 8) & 0xFF) << 16) |\
	 ((((word) >> 16) & 0xFF) << 8) |\
	 ((((word) >> 24) & 0xFF) << 0)\
	)
#if CRC32_TEST
static BaseType_t crc32_test_command_handle(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	int32_t result = E_OK;

        BaseType_t len1;
	uint32_t crc32;
	uint32_t idx, length;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	length = (uint32_t)strtol(param1, NULL, 0);

	EMBARC_PRINTF("txdata: 0x%x, length: 0x%x\r\n", (uint32_t)txdata, length);
	for (idx = 0; idx < length; idx++) {
		raw_writel((uint32_t)&txdata[idx], 0x12340000 + idx);
		rxdata[idx] = ENDIAN_ORDER_REV(raw_readl((uint32_t)&txdata[idx]));
	}

	crc32 = update_crc(0, rxdata, length << 2);
	EMBARC_PRINTF("sw_crc: 0x%x\r\n", crc32);

	result = crc32_update(0, txdata, length);
	if (E_OK != result) {
		EMBARC_PRINTF("crc init failed%d\r\n", result);
	}
	EMBARC_PRINTF("hw_crc: 0x%x-->0x%x\r\n", raw_readl(0xc1000c), crc_output());

	return pdFALSE;
}

static const CLI_Command_Definition_t crc32_test_command = {
	"crc32_test",
	"\rcrc32_test:"
	"\r\n\tcrc32_test [length(word)]\r\n\r\n",
	crc32_test_command_handle,
	-1
};
#endif

#if DMA_CRC_TEST
static BaseType_t dma_crc_test_command_handle(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	int32_t result = E_OK;

        BaseType_t len1;
	uint32_t crc32;
	uint32_t idx, length;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	length = (uint32_t)strtol(param1, NULL, 0);

	EMBARC_PRINTF("txdata: 0x%x, length: 0x%x --> 0x%x\r\n", (uint32_t)txdata, length, (uint32_t)rxdata);
	for (idx = 0; idx < length; idx++) {
		raw_writel((uint32_t)&txdata[idx], 0x12340000 + idx);
		//rxdata[idx] = ENDIAN_ORDER_REV(txdata[idx]);
		rxdata[idx] = ENDIAN_ORDER_REV(raw_readl((uint32_t)&txdata[idx]));
	}

	crc32 = update_crc(0, rxdata, length << 2);
	EMBARC_PRINTF("sw_crc: 0x%x\r\n", crc32);

	hw_crc_reset(1);
	hw_crc_reset(0);
	result = cascade_crc_dmacopy(txdata, length);
	if (E_OK != result) {
		EMBARC_PRINTF("DMA crc init failed%d\r\n", result);
	}

	return pdFALSE;
}

static const CLI_Command_Definition_t dma_crc_test_command = {
	"dma_crc_test",
	"\rdma_crc_test:"
	"\r\n\tdma_crc_test [length(word)]\r\n\r\n",
	dma_crc_test_command_handle,
	-1
};
#endif


static void cascade_xfer_task(void *params)
{
	uint32_t length = 0;
	uint32_t *data_ptr = rxdata;
	while (1) {
		if (E_OK == cascade_read(&data_ptr, &length, portMAX_DELAY)) {
			EMBARC_PRINTF("package has been handled[0x%x].\r\n", length);
			cascade_process_done();
		} else {
			taskYIELD();
		}
	}
}

void cascade_if_cli_register(void)
{
	FreeRTOS_CLIRegisterCommand(&cs_send_command);
#if DMA_TEST
	FreeRTOS_CLIRegisterCommand(&dma_test_command);
#endif
#if CRC32_TEST
	FreeRTOS_CLIRegisterCommand(&crc32_test_command);
#endif
#if DMA_CRC_TEST
	FreeRTOS_CLIRegisterCommand(&dma_crc_test_command);
#endif
        if (xTaskCreate(cascade_xfer_task, "cs_rx_task", 128, (void *)0, configMAX_PRIORITIES - 1, &cs_xfer_handle)
            != pdPASS) {
		EMBARC_PRINTF("create cs_xfer_handle error\r\n");
        }
}
