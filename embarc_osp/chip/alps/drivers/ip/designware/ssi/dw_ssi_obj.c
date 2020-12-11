#include "embARC_toolchain.h"
#include "dev_common.h"
#include "alps/alps.h"
#include "dw_ssi.h"
#include "dw_ssi_obj.h"
#include "xip.h"

static dw_ssi_t dw_spi_m0 = {
	.base = REL_REGBASE_SPI0,
	.mode = DEV_MASTER_MODE,
	.int_tx = SPI_M0_TXE_INTR,
	.int_rx = SPI_M0_RXF_INTR,
	.int_err = SPI_M0_ERR_INTR,
	.rx_dma_req = DMA_REQ_SPI_M0_RX,
	.tx_dma_req = DMA_REQ_SPI_M0_TX,
	.ref_clock = 0,
};

static dw_ssi_t dw_spi_m1 = {
	.base = REL_REGBASE_SPI1,
	.mode = DEV_MASTER_MODE,
	.int_tx = SPI_M1_TXE_INTR,
	.int_rx = SPI_M1_RXF_INTR,
	.int_err = SPI_M1_ERR_INTR,
	.rx_dma_req = DMA_REQ_SPI_M1_RX,
	.tx_dma_req = DMA_REQ_SPI_M1_TX,
	.ref_clock = 0,
};

static dw_ssi_t dw_spi_s = {
	.base = REL_REGBASE_SPI2,
	.mode = DEV_SLAVE_MODE,
	//.int_tx = SPI_S_TXE_INTR,
	.int_tx = INT_MASK_IRQ0,
	.int_rx = SPI_S_RXF_INTR,
	.int_err = SPI_S_ERR_INTR,
	.rx_dma_req = DMA_REQ_SPI_S_RX,
	.tx_dma_req = DMA_REQ_SPI_S_TX,
	.ref_clock = 0,
};

static dw_ssi_t dw_qspi = {
	.base = REL_REGBASE_QSPI,
	.mode = DEV_MASTER_MODE,
	.int_tx = INT_QSPI_M_TXE_IRQ,
	.int_rx = INT_QSPI_M_RXF_IRQ,
	.int_err = INT_QSPI_M_ERR_IRQ,
	.rx_dma_req = DMA_REQ_QSPI_M_RX,
	.tx_dma_req = DMA_REQ_QSPI_M_TX,
	.ref_clock = 0,
};

static dw_ssi_t flash_xip = {
	.base = REL_REGBASE_XIP,
	.ref_clock = 0,
};

static void spi_resource_install(void)
{
	dw_ssi_install_ops(&dw_spi_m0);
	dw_ssi_install_ops(&dw_spi_m1);
	dw_ssi_install_ops(&dw_spi_s);
	dw_ssi_install_ops(&dw_qspi);
	dw_ssi_install_ops(&flash_xip);
}

void *dw_ssi_get_dev(uint32_t id)
{
	int32_t result = 0;

	static uint32_t spi_install_flag = 0;

	void *dev_ptr = NULL;

	if (0 == spi_install_flag) {
		dmu_irq_select(INT_MASK_IRQ0, SPI_S_TXE_INTR);
		spi_resource_install();
		spi_install_flag = 1;
	}

	switch (id) {
		case DW_SPI_0_ID:
			if (0 == dw_spi_m0.ref_clock) {
				result = clock_frequency(SPI_M0_CLOCK);
				if (result > 0) {
					dw_spi_m0.ref_clock = result;
				} else {
					break;
				}
			}
			dev_ptr = (void *)&dw_spi_m0;
			break;
		case DW_SPI_1_ID:
			if (0 == dw_spi_m1.ref_clock) {
				result = clock_frequency(SPI_M1_CLOCK);
				if (result > 0) {
					dw_spi_m1.ref_clock = result;
				} else {
					break;
				}
			}
			dev_ptr = (void *)&dw_spi_m1;
			break;
		case DW_SPI_2_ID:
			if (0 == dw_spi_s.ref_clock) {
				result = clock_frequency(SPI_S_CLOCK);
				if (result > 0) {
					dw_spi_s.ref_clock = result;
				} else {
					break;
				}
			}
			dev_ptr = (void *)&dw_spi_s;
			break;
		case DW_QSPI_ID:
			if (0 == dw_qspi.ref_clock) {
				result = clock_frequency(QSPI_CLOCK);
				if (result > 0) {
					dw_qspi.ref_clock = result;
				} else {
					break;
				}
			}
			dev_ptr = (void *)&dw_qspi;
			break;
		case XIP_QSPI_ID:
			if (0 == flash_xip.ref_clock) {
				result = clock_frequency(XIP_CLOCK);
				if (result > 0) {
					flash_xip.ref_clock = result;
				} else {
					break;
				}
			}
			dev_ptr = (void *)&flash_xip;
			break;
		default:
			dev_ptr = NULL;
	}

	return dev_ptr;
}
