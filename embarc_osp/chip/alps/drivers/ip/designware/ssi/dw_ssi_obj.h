#ifndef _DW_SSI_OBJ_H_
#define _DW_SSI_OBJ_H_

#define DW_SPI_0_ID		0	/*!< SPI 0 id macro (master node) */
#define DW_SPI_1_ID		1	/*!< SPI 1 id macro (master node) */
#define DW_SPI_2_ID		2	/*!< SPI 1 id macro (slave node) */
#define DW_QSPI_ID		3
#define XIP_QSPI_ID		4

#define DW_SPI_NUM		(3)

void *dw_ssi_get_dev(uint32_t id);

#endif
