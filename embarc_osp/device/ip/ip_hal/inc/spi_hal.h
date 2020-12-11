#ifndef _SPI_HAL_H_
#define _SPI_HAL_H_

#include "dev_common.h"

typedef enum spi_communication_mode {
	SPI_COM_SIMPLEX = 0,
	SPI_COM_HALF_DUPLEX,
	SPI_COM_DUPLEX,
	SPI_COM_MODE_INVALID
} spi_com_mode_t;

typedef enum {
	SPI_CLK_MODE_0 = 0,
	SPI_CLK_MODE_1,
	SPI_CLK_MODE_2,
	SPI_CLK_MODE_3,
	SPI_CLK_MODE_INVALID
} spi_clock_mode_t;

typedef enum {
	SPI_FRF_STANDARD = 0,
	SPI_FRF_DUAL,
	SPI_FRF_QUAD,
	SPI_FRF_OCTAL
} spi_frf_t;

typedef struct spi_transfer_description {
	uint8_t clock_mode;
	uint8_t dfs;
	uint8_t cfs;
	uint8_t spi_frf;

	uint8_t rx_thres;
	uint8_t tx_thres;
	uint8_t rx_sample_delay;
} spi_xfer_desc_t;
#define SPI_XFER_DESC_COPY(dst, src)	do { \
		(dst)->clock_mode = (src)->clock_mode; \
		(dst)->dfs = (src)->dfs; \
		(dst)->cfs = (src)->cfs; \
		(dst)->spi_frf = (src)->spi_frf; \
		(dst)->rx_thres = (src)->rx_thres; \
		(dst)->tx_thres = (src)->tx_thres; \
		(dst)->rx_sample_delay = (src)->rx_sample_delay; \
	} while (0)

int32_t spi_device_mode(uint32_t id);

int32_t spi_open(uint32_t id, uint32_t buad);
int32_t spi_transfer_config(uint32_t id, spi_xfer_desc_t *desc);
int32_t spi_xfer(uint32_t id, uint32_t *txdata, uint32_t *rxdata, uint32_t len);
int32_t spi_write(uint32_t id, uint32_t *data, uint32_t len);
int32_t spi_read(uint32_t id, uint32_t *data, uint32_t len);
int32_t spi_dma_write(uint32_t id, uint32_t *data, uint32_t len, void (*func)(void *));
int32_t spi_dma_read(uint32_t id, uint32_t *data, uint32_t len, void (*func)(void *));

int32_t spi_databuffer_install(uint32_t id, uint32_t rx_or_tx, DEV_BUFFER *devbuf);
int32_t spi_databuffer_uninstall(uint32_t id, uint32_t rx_or_tx);
int32_t spi_interrupt_enable(uint32_t id, uint32_t rx_or_tx, uint32_t enable);
int32_t spi_interrupt_install(uint32_t id, uint32_t rx_or_tx, void (*func)(void *));
int32_t spi_interrupt_uninstall(uint32_t id, uint32_t rx_or_tx);

int32_t spi_fifo_threshold_update(uint32_t id, uint32_t rx, uint32_t tx);

#endif
