#ifndef _DW_SSI_REG_H_
#define _DW_SSI_REG_H_

#define DW_SSI_MAX_XFER_SIZE			(32)

#define REG_DW_SSI_CTRLR0_OFFSET		(0x0000)
#define REG_DW_SSI_CTRLR1_OFFSET		(0x0004)
#define REG_DW_SSI_SSIENR_OFFSET		(0x0008)
#define REG_DW_SSI_MWCR_OFFSET			(0x000c)
#define REG_DW_SSI_SER_OFFSET			(0x0010)
#define REG_DW_SSI_BAUDR_OFFSET			(0x0014)
#define REG_DW_SSI_TXFTLR_OFFSET		(0x0018)
#define REG_DW_SSI_RXFTLR_OFFSET		(0x001c)
#define REG_DW_SSI_TXFLR_OFFSET			(0x0020)
#define REG_DW_SSI_RXFLR_OFFSET			(0x0024)
#define REG_DW_SSI_SR_OFFSET			(0x0028)
#define REG_DW_SSI_IMR_OFFSET			(0x002c)
#define REG_DW_SSI_ISR_OFFSET			(0x0030)
#define REG_DW_SSI_RISR_OFFSET			(0x0034)
#define REG_DW_SSI_TXOICR_OFFSET		(0x0038)
#define REG_DW_SSI_RXOICR_OFFSET		(0x003c)
#define REG_DW_SSI_RXUICR_OFFSET		(0x0040)
#define REG_DW_SSI_MSTICR_OFFSET		(0x0044)
#define REG_DW_SSI_ICR_OFFSET			(0x0048)
#define REG_DW_SSI_DMACR_OFFSET			(0x004c)
#define REG_DW_SSI_DMATDLR_OFFSET		(0x0050)
#define REG_DW_SSI_DMARDLR_OFFSET		(0x0054)
#define REG_DW_SSI_IDR_OFFSET			(0x0058)
#define REG_DW_SSI_VERSION_OFFSET		(0x005c)
#define REG_DW_SSI_DR_OFFSET(x)			(0x0060 + ((x) << 2))
#define REG_DW_SSI_RX_SAMPLE_DLY_OFFSET		(0x00f0)
#define REG_DW_SSI_SPI_CTRLR0_OFFSET		(0x00f4)
#define REG_DW_SSI_TXD_DRIVE_EDGE_OFFSET	(0x00f8)


/* control register. */
#define BIT_DW_SSI_CTRLR0_SSTE			(1 << 24)
#define BITS_DW_SSI_CTRLR0_SPI_FRF_MASK		(0x3)
#define BITS_DW_SSI_CTRLR0_SPI_FRF_SHIFT	(21)
#define BITS_DW_SSI_CTRLR0_DFS32_MASK		(0x1F)
#define BITS_DW_SSI_CTRLR0_DFS32_SHIFT		(16)
#define BITS_DW_SSI_CTRLR0_CFS_MASK		(0xF)
#define BITS_DW_SSI_CTRLR0_CFS_SHIFT		(12)
#define BIT_DW_SSI_CTRLR0_SRL			(1 << 11)
#define BIT_DW_SSI_CTRLR0_SLV_OE		(1 << 10)
#define BITS_DW_SSI_CTRLR0_TMOD_MASK		(0x3)
#define BITS_DW_SSI_CTRLR0_TMOD_SHIFT		(8)
#define BIT_DW_SSI_CTRLR0_SCPOL			(1 << 7)
#define BIT_DW_SSI_CTRLR0_SCPH			(1 << 6)
#define BITS_DW_SSI_CTRLR0_FRF_MASK		(0x3)
#define BITS_DW_SSI_CTRLR0_FRF_SHIFT		(4)
#define BITS_DW_SSI_CTRLR0_DFS_MASK		(0xF)
#define BITS_DW_SSI_CTRLR0_DFS_SHIFT		(0)

/* microwire control register. */
#define BIT_DW_SSI_MWCR_MHS			(1 << 2)
#define BIT_DW_SSI_MWCR_MDD			(1 << 1)
#define BIT_DW_SSI_MWCR_MVMOD			(1 << 0)

/* status register. */
#define BIT_DW_SSI_SR_DCOL			(1 << 6)
#define BIT_DW_SSI_SR_TXE			(1 << 5)
#define BIT_DW_SSI_SR_RFF			(1 << 4)
#define BIT_DW_SSI_SR_RFNE			(1 << 3)
#define BIT_DW_SSI_SR_TFE			(1 << 2)
#define BIT_DW_SSI_SR_TFNF			(1 << 1)
#define BIT_DW_SSI_SR_BUSY			(1 << 0)

/* interrupt mask register. */
#define BIT_DW_SSI_IMR_MSTIM			(1 << 5)
#define BIT_DW_SSI_IMR_RXFIM			(1 << 4)
#define BIT_DW_SSI_IMR_RXOIM			(1 << 3)
#define BIT_DW_SSI_IMR_RXUIM			(1 << 2)
#define BIT_DW_SSI_IMR_TXOIM			(1 << 1)
#define BIT_DW_SSI_IMR_TXEIM			(1 << 0)

/* interrupt status register. */
#define BIT_DW_SSI_IMR_MSTIS			(1 << 5)
#define BIT_DW_SSI_IMR_RXFIS			(1 << 4)
#define BIT_DW_SSI_IMR_RXOIS			(1 << 3)
#define BIT_DW_SSI_IMR_RXUIS			(1 << 2)
#define BIT_DW_SSI_IMR_TXOIS			(1 << 1)
#define BIT_DW_SSI_IMR_TXEIS			(1 << 0)

/* interrupt status register. */
#define BIT_DW_SSI_IMR_MSTIR			(1 << 5)
#define BIT_DW_SSI_IMR_RXFIR			(1 << 4)
#define BIT_DW_SSI_IMR_RXOIR			(1 << 3)
#define BIT_DW_SSI_IMR_RXUIR			(1 << 2)
#define BIT_DW_SSI_IMR_TXOIR			(1 << 1)
#define BIT_DW_SSI_IMR_TXEIR			(1 << 0)

/* dma control register. */
#define BIT_DW_SSI_DMACR_TDMAE			(1 << 1)
#define BIT_DW_SSI_DMACR_RDMAE			(1 << 0)

/* spi control register. */
#define BIT_DW_SSI_SPI_CTRLR0_RXDS_EN		(1 << 18)
#define BIT_DW_SSI_SPI_CTRLR0_INST_DDR_EN	(1 << 17)
#define BIT_DW_SSI_SPI_CTRLR0_SPI_DDR_EN	(1 << 16)
#define BIT_DW_SSI_SPI_CTRLR0_WAIT_CYCLES_MASK	(0x1F)
#define BIT_DW_SSI_SPI_CTRLR0_WAIT_CYCLES_SHIFT	(11)
#define BIT_DW_SSI_SPI_CTRLR0_INS_L_MASK	(0x3)
#define BIT_DW_SSI_SPI_CTRLR0_INS_L_SHIFT	(8)
#define BIT_DW_SSI_SPI_CTRLR0_ADDR_L_MASK	(0xF)
#define BIT_DW_SSI_SPI_CTRLR0_ADDR_L_SHIFT	(2)
#define BIT_DW_SSI_SPI_CTRLR0_TRANS_TYPE_MASK	(0x3)
#define BIT_DW_SSI_SPI_CTRLR0_TRANS_TYPE_SHIFT	(0)

#define CLK_MODE0_SSI_CTRL0(frf, dfs32, tmod)	(\
	(((frf) & BITS_DW_SSI_CTRLR0_SPI_FRF_MASK) << BITS_DW_SSI_CTRLR0_SPI_FRF_SHIFT) |\
	(((dfs32) & BITS_DW_SSI_CTRLR0_DFS32_MASK) << BITS_DW_SSI_CTRLR0_DFS32_SHIFT) |\
	(((tmod) & BITS_DW_SSI_CTRLR0_TMOD_MASK) << BITS_DW_SSI_CTRLR0_TMOD_SHIFT)\
	)

#define SSI_SPI_CTRLR0(wait_cycle, ins_len, addr_len, trans_type) (\
	(((trans_type) & BIT_DW_SSI_SPI_CTRLR0_TRANS_TYPE_MASK) << BIT_DW_SSI_SPI_CTRLR0_TRANS_TYPE_SHIFT) |\
	(((addr_len) & BIT_DW_SSI_SPI_CTRLR0_ADDR_L_MASK) << BIT_DW_SSI_SPI_CTRLR0_ADDR_L_SHIFT) |\
	(((ins_len) & BIT_DW_SSI_SPI_CTRLR0_INS_L_MASK) << BIT_DW_SSI_SPI_CTRLR0_INS_L_SHIFT) |\
	(((wait_cycle) & BIT_DW_SSI_SPI_CTRLR0_WAIT_CYCLES_MASK) << BIT_DW_SSI_SPI_CTRLR0_WAIT_CYCLES_SHIFT)\
	)

#endif
