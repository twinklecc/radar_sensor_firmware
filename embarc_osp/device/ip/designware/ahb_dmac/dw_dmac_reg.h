#ifndef _DW_DMAC_REG_H_
#define _DW_DMAC_REG_H_

#include "board.h"

#define CHIP_DMAC_BASE			(REL_REGBASE_HDMA)

#define DMAC_REG_SAR(chn)		(CHIP_DMAC_BASE + (chn) * 0x58)
#define DMAC_REG_DAR(chn)		(CHIP_DMAC_BASE + 0x8 + (chn) * 0x58)
#define DMAC_REG_LLP(chn)		(CHIP_DMAC_BASE + 0x10 + (chn) * 0x58)
#define DMAC_REG_CTL(chn)		(CHIP_DMAC_BASE + 0x18 + (chn) * 0x58)
#define DMAC_REG_SSTAT(chn)		(CHIP_DMAC_BASE + 0x20 + (chn) * 0x58)
#define DMAC_REG_DSTAT(chn)		(CHIP_DMAC_BASE + 0x28 + (chn) * 0x58)
#define DMAC_REG_SSTATAR(chn)		(CHIP_DMAC_BASE + 0x30 + (chn) * 0x58)
#define DMAC_REG_DSTATAR(chn)		(CHIP_DMAC_BASE + 0x38 + (chn) * 0x58)
#define DMAC_REG_CFG(chn)		(CHIP_DMAC_BASE + 0x40 + (chn) * 0x58)
#define DMAC_REG_SGR(chn)		(CHIP_DMAC_BASE + 0x48 + (chn) * 0x58)
#define DMAC_REG_DSR(chn)		(CHIP_DMAC_BASE + 0x50 + (chn) * 0x58)

/* raw status. */
#define DMAC_REG_RAW_STATUS_BASE	(CHIP_DMAC_BASE + 0x2C0)
#define DMAC_REG_RAW_TFR		(DMAC_REG_RAW_STATUS_BASE)
#define DMAC_REG_RAW_BLOCK		(DMAC_REG_RAW_STATUS_BASE + 0x8)
#define DMAC_REG_RAW_SRC_TRANS		(DMAC_REG_RAW_STATUS_BASE + 0x10)
#define DMAC_REG_RAW_DST_TRANS		(DMAC_REG_RAW_STATUS_BASE + 0x18)
#define DMAC_REG_RAW_ERR		(DMAC_REG_RAW_STATUS_BASE + 0x20)

/* status. */
#define DMAC_REG_STATUS_TFR		(CHIP_DMAC_BASE + 0x2E8)
#define DMAC_REG_STATUS_BLOCK		(CHIP_DMAC_BASE + 0x2F0)
#define DMAC_REG_STATUS_SRC_TRANS	(CHIP_DMAC_BASE + 0x2F8)
#define DMAC_REG_STATUS_DST_TRANS	(CHIP_DMAC_BASE + 0x300)
#define DMAC_REG_STATUS_ERR		(CHIP_DMAC_BASE + 0x308)

/* mask. */
#define DMAC_REG_MASK_TFR		(CHIP_DMAC_BASE + 0x310)
#define DMAC_REG_MASK_BLOCK		(CHIP_DMAC_BASE + 0x318)
#define DMAC_REG_MASK_SRC_TRANS		(CHIP_DMAC_BASE + 0x320)
#define DMAC_REG_MASK_DST_TRANS		(CHIP_DMAC_BASE + 0x328)
#define DMAC_REG_MASK_ERR		(CHIP_DMAC_BASE + 0x330)

/* clear. */
#define DMAC_REG_CLEAR_TFR		(CHIP_DMAC_BASE + 0x338)
#define DMAC_REG_CLEAR_BLOCK		(CHIP_DMAC_BASE + 0x340)
#define DMAC_REG_CLEAR_SRC_TRANS	(CHIP_DMAC_BASE + 0x348)
#define DMAC_REG_CLEAR_DST_TRANS	(CHIP_DMAC_BASE + 0x350)
#define DMAC_REG_CLEAR_ERR		(CHIP_DMAC_BASE + 0x358)

#define DMAC_REG_STATUS			(CHIP_DMAC_BASE + 0x360)


/* software handshaking. */
#define DMAC_REG_REQSRC			(CHIP_DMAC_BASE + 0x368)
#define DMAC_REG_REQDST			(CHIP_DMAC_BASE + 0x370)
#define DMAC_REG_SGLRQSRC		(CHIP_DMAC_BASE + 0x378)
#define DMAC_REG_SGLRQDST		(CHIP_DMAC_BASE + 0x380)
#define DMAC_REG_LSTSRC			(CHIP_DMAC_BASE + 0x388)
#define DMAC_REG_LSTDST			(CHIP_DMAC_BASE + 0x390)


/* miscellaneous. */
#define DMAC_REG_CFGREG			(CHIP_DMAC_BASE + 0x398)
#define DMAC_REG_CHNEN			(CHIP_DMAC_BASE + 0x3A0)
#define DMAC_REG_DMAID			(CHIP_DMAC_BASE + 0x3A8)
#define DMAC_REG_TESTREG		(CHIP_DMAC_BASE + 0x3B0)
#define DMAC_REG_LPTIMEOUT		(CHIP_DMAC_BASE + 0x3B8)
#define DMAC_REG_COMP_PARAMS(idx)	(CHIP_DMAC_BASE + 0x3C8 + ((id) << 3))
#define DMAC_REG_COMPSID		(CHIP_DMAC_BASE + 0x3F8)


/* channel ctlx bits. */
#define DMAC_BITS_CTL_DONE		(1 << 12)
#define DMAC_BITS_CTL_BS_MASK		(0x1F)

#define DMAC_BITS_CTL_LLP_SRC_EN	(1 << 28)
#define DMAC_BITS_CTL_LLP_DST_EN	(1 << 27)
#define DMAC_BITS_CTL_TT_FC_MASK	(0x7)
#define DMAC_BITS_CTL_TT_FC_SHIFT	(20)
#define DMAC_BITS_CTL_DST_SCATTER_EN	(1 << 18)
#define DMAC_BITS_CTL_SRC_GATHER_EN	(1 << 17)
#define DMAC_BITS_CTL_SRC_MSIZE_MASK	(0x7)
#define DMAC_BITS_CTL_SRC_MSIZE_SHIFT	(14)
#define DMAC_BITS_CTL_DST_MSIZE_MASK	(0x7)
#define DMAC_BITS_CTL_DST_MSIZE_SHIFT	(11)
#define DMAC_BITS_CTL_SINC_MASK		(0x3)
#define DMAC_BITS_CTL_SINC_SHIFT	(9)
#define DMAC_BITS_CTL_DINC_MASK		(0x3)
#define DMAC_BITS_CTL_DINC_SHIFT	(7)
#define DMAC_BITS_CTL_INT_EN		(1 << 0)

#define dmac_trans_type(type)			(((type) & DMAC_BITS_CTL_TT_FC_MASK) << DMAC_BITS_CTL_TT_FC_SHIFT)
#define dmac_src_burst_transaction_length(len)	(((len) & DMAC_BITS_CTL_SRC_MSIZE_MASK) << DMAC_BITS_CTL_SRC_MSIZE_SHIFT)
#define dmac_dst_burst_transaction_length(len)	(((len) & DMAC_BITS_CTL_DST_MSIZE_MASK) << DMAC_BITS_CTL_DST_MSIZE_SHIFT)
#define dmac_src_address_mode_length(mode)	(((mode) & DMAC_BITS_CTL_SINC_MASK) << DMAC_BITS_CTL_SINC_SHIFT)
#define dmac_dst_address_mode_length(mode)	(((mode) & DMAC_BITS_CTL_DINC_MASK) << DMAC_BITS_CTL_DINC_SHIFT)


/* channel cfg bits. */
#define DMAC_BITS_CFG_DST_PER_MASK	(0xF)
#define DMAC_BITS_CFG_DST_PER_SHIFT	(11)
#define DMAC_BITS_CFG_SRC_PER_MASK	(0xF)
#define DMAC_BITS_CFG_SRC_PER_SHIFT	(7)
#define DMAC_BITS_CFG_SS_UPD_EN		(1 << 6)
#define DMAC_BITS_CFG_DS_UPD_EN		(1 << 5)
#define DMAC_BITS_CFG_PROTCTL_MASK	(0x7)
#define DMAC_BITS_CFG_PROTCTL_SHIFT	(2)
#define DMAC_BITS_CFG_FIFOMODE		(1 << 1)
#define DMAC_BITS_CFG_FCMODE		(1 << 0)

#define DMAC_BITS_CFG_RELOAD_DST	(1 << 31)
#define DMAC_BITS_CFG_RELOAD_SRC	(1 << 30)
//#define DMAC_BITS_CFG_MAX_ABRST_MASK	(0x3FF)
//#define DMAC_BITS_CFG_MAX_ABRST_SHIFT	(20)
#define DMAC_BITS_CFG_SRC_HS_POL	(1 << 19)
#define DMAC_BITS_CFG_DST_HS_POL	(1 << 18)
#define DMAC_BITS_CFG_HS_SEL_SRC	(1 << 11)
#define DMAC_BITS_CFG_HS_SEL_DST	(1 << 10)
#define DMAC_BITS_CFG_FIFO_EMPTY	(1 << 9)
#define DMAC_BITS_CFG_CHN_SUSP		(1 << 8)
#define DMAC_BITS_CFG_CHN_PRIOR_MASK	(0x7)
#define DMAC_BITS_CFG_CHN_PRIOR_SHIFT	(5)

#define dmac_chn_dst_hw_if(id)		(((id) & DMAC_BITS_CFG_DST_PER_MASK) << DMAC_BITS_CFG_DST_PER_SHIFT)
#define dmac_chn_src_hw_if(id)		(((id) & DMAC_BITS_CFG_SRC_PER_MASK) << DMAC_BITS_CFG_SRC_PER_SHIFT)
#define dmac_chn_prot_ctrl(val)		(((val) & DMAC_BITS_CFG_PROTCTL_MASK) << DMAC_BITS_CFG_PROTCTL_SHIFT)

//#define dmac_chn_max_amba_burst_len(len)	(((len) & DMAC_BITS_CFG_MAX_ABRST_MASK) << DMAC_BITS_CFG_MAX_ABRST_SHIFT)
#define dmac_chn_priority(prior)		(((prior) & DMAC_BITS_CFG_CHN_PRIOR_MASK) << DMAC_BITS_CFG_CHN_PRIOR_SHIFT)
#define dmac_chn_src_hs_select(hs)		(((hs) & 0x1) << 11)
#define dmac_chn_dst_hs_select(hs)		(((hs) & 0x1) << 10)

/* source gather bits. */
#define DMAC_BITS_SGR_SGC_MASK		(0x1F)
#define DMAC_BITS_SGR_SGC_SHIFT		(20)
#define DMAC_BITS_SGR_SGI_MASK		(0xFFFFF)
#define DMAC_BITS_SGR_SGI_SHIFT		(0)
#define dmac_source_gather(iv, cnt)	( \
		((iv) & DMAC_BITS_SGR_SGI_MASK) | \
		(((cnt) & DMAC_BITS_SGR_SGC_MASK) << DMAC_BITS_SGR_SGC_SHIFT) \
		)

/* destination scatter bits. */
#define DMAC_BITS_DSR_DSC_MASK		(0x1F)
#define DMAC_BITS_DSR_DSC_SHIFT		(20)
#define DMAC_BITS_DSR_DSI_MASK		(0xFFFFF)
#define DMAC_BITS_DSR_DSI_SHIFT		(0)
#define dmac_dest_scatter(iv, cnt)	( \
		((iv) & DMAC_BITS_DSR_DSI_MASK) | \
		(((cnt) & DMAC_BITS_DSR_DSC_MASK) << DMAC_BITS_DSR_DSC_SHIFT) \
		)

/* interrupt mask bits. */
#define DMAC_BITS_MASK_WE_MASK		(0x1F)
#define DMAC_BITS_MASK_WE_SHIFT		(8)
#define DMAC_BITS_MASK_MASK_MASK	(0x1F)

#define dmac_int_mask_write_enable(chn)		(((1 << (chn)) & DMAC_BITS_MASK_WE_MASK) << DMAC_BITS_MASK_WE_SHIFT);
#define dmac_int_mask(chn)			(((1 << (chn)) & DMAC_BITS_MASK_MASK_MASK));


/* channel enable bits. */
#define DMAC_BITS_CHNEN_WE_MASK		(0x1F)
#define DMAC_BITS_CHNEN_WE_SHIFT	(8)
#define DMAC_BITS_CHNEN_EN_MASK		(0x1F)

#define dmac_chn_enable_write_enable(chn)	(((1 << (chn)) & DMAC_BITS_CHNEN_WE_MASK) << DMAC_BITS_CHNEN_WE_SHIFT);
#define dmac_chn_enable(chn)			(((1 << (chn)) & DMAC_BITS_CHNEN_EN_MASK));


inline static int dmac_chn_fifo_empty(uint32_t chn_id)
{
	return !!(raw_readl(DMAC_REG_CFG(chn_id)) & DMAC_BITS_CFG_FIFO_EMPTY);
}

#endif /* _DW_DMAC_REG_H_ */
