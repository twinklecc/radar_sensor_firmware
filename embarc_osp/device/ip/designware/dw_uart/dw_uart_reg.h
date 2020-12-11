#ifndef _DW_UART_REG_H_
#define _DW_UART_REG_H_

#define REG_DW_UART_RBR_DLL_THR_OFFSET		(0x0000)
#define REG_DW_UART_DLH_IER_OFFSET		(0x0004)
#define REG_DW_UART_FCR_IIR_OFFSET		(0x0008)
#define REG_DW_UART_LCR_OFFSET			(0x000c)
#define REG_DW_UART_MCR_OFFSET			(0x0010)
#define REG_DW_UART_LSR_OFFSET			(0x0014)
#define REG_DW_UART_MSR_OFFSET			(0x0018)
#define REG_DW_UART_SCR_OFFSET			(0x001c)
#define REG_DW_UART_LPDLL_OFFSET		(0x0020)
#define REG_DW_UART_LPDLH_OFFSET		(0x0024)
#define REG_DW_UART_SRBR_OFFSET(n)		(0x0030 + ((n) << 2))
#define REG_DW_UART_STHR_OFFSET(n)		(0x0030 + ((n) << 2))
#define REG_DW_UART_FAR_OFFSET			(0x0070)
#define REG_DW_UART_TFR_OFFSET			(0x0074)
#define REG_DW_UART_RFW_OFFSET			(0x0078)
#define REG_DW_UART_USR_OFFSET			(0x007c)
#define REG_DW_UART_TFL_OFFSET			(0x0080)
#define REG_DW_UART_RFL_OFFSET			(0x0084)
#define REG_DW_UART_SRT_OFFSET			(0x009c)
#define REG_DW_UART_HTX_OFFSET			(0x00a4)
#define REG_DW_UART_DLF_OFFSET			(0x00c0)
#define REG_DW_UART_CPR_OFFSET			(0x00f4)
#define REG_DW_UART_UCV_OFFSET			(0x00f8)
#define REG_DW_UART_CTR_OFFSET			(0x00fc)

/* interrupt enable. */
#define BIT_REG_DW_UART_IER_PTIME		(1 << 7)
#define BIT_REG_DW_UART_IER_ELCOLR		(1 << 4)
#define BIT_REG_DW_UART_IER_EDSSI		(1 << 3)
#define BIT_REG_DW_UART_IER_ELSI		(1 << 2)
#define BIT_REG_DW_UART_IER_ETBEI		(1 << 1)
#define BIT_REG_DW_UART_IER_ERBFI		(1 << 0)

/* fifo control. */
#define BITS_REG_DW_UART_FCR_RT_MASK		(0x3)
#define BITS_REG_DW_UART_FCR_RT_SHIFT		(6)
#define BITS_REG_DW_UART_FCR_TET_MASK		(0x3)
#define BITS_REG_DW_UART_FCR_TET_SHIFT		(4)
#define BIT_REG_DW_UART_FCR_XFIFOR		(1 << 2)
#define BIT_REG_DW_UART_FCR_RFIFOR		(1 << 1)
#define BIT_REG_DW_UART_FCR_FIFOE		(1 << 0)

/* interrupt identification. */
#define BITS_REG_DW_UART_IIR_FIFOSE_MASK	(0x3)
#define BITS_REG_DW_UART_IIR_FIFOSE_SHIFT	(6)
#define BITS_REG_DW_UART_IIR_IID_MASK		(0xF)
#define BITS_REG_DW_UART_IIR_IID_SHIFT		(0)

/* line control. */
#define BIT_REG_DW_UART_LCR_DLAB		(1 << 7)
#define BIT_REG_DW_UART_LCR_BC			(1 << 6)
#define BIT_REG_DW_UART_LCR_SP			(1 << 5)
#define BIT_REG_DW_UART_LCR_EPS			(1 << 4)
#define BIT_REG_DW_UART_LCR_PEN			(1 << 3)
#define BIT_REG_DW_UART_LCR_STOP		(1 << 2)
#define BIT_REG_DW_UART_LCR_DLS_MASK		(0x3)
#define BIT_REG_DW_UART_LCR_DLS_SHIFT		(0)

/* line status. */
#define BIT_REG_DW_UART_LSR_ADDR_RCVD		(1 << 8)
#define BIT_REG_DW_UART_LSR_RFE			(1 << 7)
#define BIT_REG_DW_UART_LSR_TEMT		(1 << 6)
#define BIT_REG_DW_UART_LSR_THRE		(1 << 5)
#define BIT_REG_DW_UART_LSR_BI			(1 << 4)
#define BIT_REG_DW_UART_LSR_FE			(1 << 3)
#define BIT_REG_DW_UART_LSR_PE			(1 << 2)
#define BIT_REG_DW_UART_LSR_OE			(1 << 1)
#define BIT_REG_DW_UART_LSR_DR			(1 << 0)

/* uart status. */
#define BIT_REG_DW_UART_USR_RFF			(1 << 4)
#define BIT_REG_DW_UART_USR_RFNE		(1 << 3)
#define BIT_REG_DW_UART_USR_TFE			(1 << 2)
#define BIT_REG_DW_UART_USR_TFNF		(1 << 1)
#define BIT_REG_DW_UART_USR_BUSY		(1 << 0)

/* component parameter. */
#define BIR_REG_DW_UART_CPR_FIFO_MODE_MASK	(0xff)
#define BIR_REG_DW_UART_CPR_FIFO_MODE_SHIFT	(16)
#define BIR_REG_DW_UART_CPR_DMA_EXTRA		(1 << 13)
#define BIR_REG_DW_UART_CPR_ADDR_ENCODED	(1 << 12)
#define BIR_REG_DW_UART_CPR_ADDR_SHADOW		(1 << 11)
#define BIR_REG_DW_UART_CPR_FIFO_STAT		(1 << 10)
#define BIR_REG_DW_UART_CPR_FIFO_ACCESS		(1 << 9)
#define BIR_REG_DW_UART_CPR_ADDITIONAL_FEAT	(1 << 8)
#define BIR_REG_DW_UART_CPR_SIR_LP_MODE		(1 << 7)
#define BIR_REG_DW_UART_CPR_SIR_MODE		(1 << 6)
#define BIR_REG_DW_UART_CPR_THRE_MODE		(1 << 5)
#define BIR_REG_DW_UART_CPR_AFCE_MODE		(1 << 4)
#define BIR_REG_DW_UART_CPR_APB_DWIDTH_MASK	(0x3)
#define BIR_REG_DW_UART_CPR_APB_DWIDTH_SHIFT	(0)

#endif
