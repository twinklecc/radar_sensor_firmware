#include "embARC_toolchain.h"

#include "dw_dmac.h"
#include "dw_dma_obj.h"

#include "alps/alps.h"

static dw_dmac_t dev_dma = {
	.base = REL_REGBASE_HDMA,
	.nof_chn = 8,
	.chn_int = {
		INT_DMA_M0_IRQ0,
		INT_DMA_M0_IRQ1,
		INT_DMA_M0_IRQ2,
		INT_DMA_M0_IRQ3,
		INT_DMA_M0_IRQ4
	},
};

void *dma_get_dev(int32_t reserved)
{
	static uint32_t dma_install_flag = 0;
	if (0 == dma_install_flag) {
		dw_dmac_install_ops(&dev_dma);
		dma_install_flag = 1;
	}

	return (void *)&dev_dma;
}
