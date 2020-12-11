#ifndef _DW_DMAC_H_
#define _DW_DMAC_H_

#include "dw_dmac_reg.h"

typedef enum {
	INT_TFR = 0,
	INT_BLOCK,
	INT_SRC_TRAN,
	INT_DST_TRAN,
	INT_ERR,
	INT_TYPE_INVALID
} dw_dmac_inttype_t;

/*
 * @block_ts, block transfer size.it indicates the total
 * number of single transactions to perform for every block
 * transfer.
 * @llp_src_en, linklist mode enable for source.
 * @llp_dst_en, linklist mode enable for destination.
 * @tt_fc, transfer type and flow control.
 * @dst_scatter_en, destination scatter enable.
 * @src_gather_en, source gather enable.
 * @src_msize, source burst transaction length. Number of data
 * items, to be read from the source every time a burst transferred
 * request is made from either the corresponding hardware or
 * software handshaking interface.
 * @dst_msize, destination burst transaction length.
 * @sinc, source address increment.
 * @dinc, destination address increment.
 * */
typedef struct dw_dmac_channel_control {
	//uint8_t done;
	uint32_t block_ts;
	uint32_t llp_src_en;
	uint32_t llp_dst_en;
	uint32_t tt_fc;
	uint32_t dst_scatter_en;
	uint32_t src_gather_en;
	uint32_t src_msize;
	uint32_t dst_msize;
	uint32_t sinc;
	uint32_t dinc;
	uint32_t int_en;
} dw_dmac_chn_ctrl_t;

typedef struct dw_dmac_channel_config {
	uint32_t dst_hw_if;
	uint32_t src_hw_if;
	uint32_t src_sts_upd_en;
	uint32_t dst_sts_upd_en;
	uint32_t prot_ctrl;
	uint32_t fifo_mode;
	uint32_t dst_reload;
	uint32_t src_reload;
	uint32_t src_hw_if_pol;
	uint32_t dst_hw_if_pol;
	uint32_t src_hw_hs_sel;
	uint32_t dst_hw_hs_sel;
	uint32_t suspend;
	uint32_t priority;
} dw_dmac_chn_cfg_t;

typedef struct dw_ahb_dmac_descriptor {
	uint32_t base;
	uint32_t nof_chn;
	uint32_t chn_int[INT_TYPE_INVALID];

	void *ops;
}dw_dmac_t;


typedef struct dw_ahb_dmac_operation {
	int32_t (*enable)(dw_dmac_t *dmac, uint32_t en);
	int32_t (*chn_enable)(dw_dmac_t *dmac, uint32_t chn_id, uint32_t en);
	int32_t (*sw_request)(dw_dmac_t *dmac, uint32_t chn_id);

	int32_t (*int_status)(dw_dmac_t *dmac, dw_dmac_inttype_t int_type);
	int32_t (*int_disable)(dw_dmac_t *dmac, dw_dmac_inttype_t int_type);

	int32_t (*chn_int_clear)(dw_dmac_t *dmac, uint32_t chn_id, dw_dmac_inttype_t int_type);
	int32_t (*chn_int_enable)(dw_dmac_t *dmac, uint32_t chn_id, dw_dmac_inttype_t int_type, uint32_t en);
	int32_t (*chn_int_status)(dw_dmac_t *dmac, dw_dmac_inttype_t int_type);
	int32_t (*chn_int_raw_status)(dw_dmac_t *dmac, dw_dmac_inttype_t int_type);

	int32_t (*chn_scatter)(dw_dmac_t *dmac, uint32_t chn_id, uint32_t interval, uint32_t count);
	int32_t (*chn_gather)(dw_dmac_t *dmac, uint32_t chn_id, uint32_t interval, uint32_t count);

	int32_t (*chn_config)(dw_dmac_t *dmac, uint32_t chn_id, dw_dmac_chn_cfg_t *cfg);
	int32_t (*chn_status_address)(dw_dmac_t *dmac, uint32_t chn_id, uint32_t src_addr, uint32_t dst_addr);

	int32_t (*chn_control)(dw_dmac_t *dmac, uint32_t chn_id, dw_dmac_chn_ctrl_t *ctrl);
	int32_t (*chn_ll_pointer)(dw_dmac_t *dmac, uint32_t chn_id, uint32_t addr);
	int32_t (*chn_address)(dw_dmac_t *dmac, uint32_t chn_id, uint32_t src_addr, uint32_t dst_addr);
} dw_dmac_ops_t;


static inline uint32_t dma_ctrl2regl(dw_dmac_chn_ctrl_t *ctrl)
{
	uint32_t l_val = 0;

	if (ctrl->llp_src_en) {
		l_val |= DMAC_BITS_CTL_LLP_SRC_EN;
	}

	if (ctrl->llp_dst_en) {
		l_val |= DMAC_BITS_CTL_LLP_DST_EN;
	}

	l_val |= dmac_trans_type(ctrl->tt_fc);

	if (ctrl->dst_scatter_en) {
		l_val |= DMAC_BITS_CTL_DST_SCATTER_EN;
	}

	if (ctrl->src_gather_en) {
		l_val |= DMAC_BITS_CTL_SRC_GATHER_EN;
	}

	l_val |= dmac_src_burst_transaction_length(ctrl->src_msize);
	l_val |= dmac_dst_burst_transaction_length(ctrl->dst_msize);

	l_val |= dmac_src_address_mode_length(ctrl->sinc);
	l_val |= dmac_dst_address_mode_length(ctrl->dinc);

	if (ctrl->int_en) {
		l_val |= DMAC_BITS_CTL_INT_EN;
	}

	return l_val;
}
static inline uint32_t dma_ctrl2regh(dw_dmac_chn_ctrl_t *ctrl)
{
	return (ctrl->block_ts & DMAC_BITS_CTL_BS_MASK);
}


int32_t dw_dmac_install_ops(dw_dmac_t *dev_dmac);
#endif
