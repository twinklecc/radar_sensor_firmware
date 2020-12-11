#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC_debug.h"

#include "arc_exception.h"
#include "dw_dmac_reg.h"
#include "dw_dmac.h"

static int32_t dw_dmac_chn_address(dw_dmac_t *dmac, uint32_t chn_id, uint32_t src_addr, uint32_t dst_addr)
{
	int32_t result = E_OK;

	if ((NULL == dmac) || (chn_id >= dmac->nof_chn)) {
		result = E_PAR;
	} else {
		raw_writel(DMAC_REG_SAR(chn_id), src_addr);
		raw_writel(DMAC_REG_DAR(chn_id), dst_addr);
	}

	return result;
}

static int32_t dw_dmac_chn_linklist_pointer(dw_dmac_t *dmac, uint32_t chn_id, uint32_t addr)
{
	int32_t result = E_OK;

	if ((NULL == dmac) || (chn_id >= dmac->nof_chn)) {
		result = E_PAR;
	} else {
		raw_writel(DMAC_REG_LLP(chn_id), (addr >> 2) << 2);
	}

	return result;
}

static int32_t dw_dmac_chn_control(dw_dmac_t *dmac, uint32_t chn_id, dw_dmac_chn_ctrl_t *ctrl)
{
	int32_t result = E_OK;

	if ((NULL == dmac) || (chn_id >= dmac->nof_chn) || (NULL == ctrl)) {
		result = E_PAR;
	} else {
		uint32_t l_val = dma_ctrl2regl(ctrl);

		raw_writel(DMAC_REG_CTL(chn_id), l_val);
		raw_writel(DMAC_REG_CTL(chn_id) + 4, ctrl->block_ts & DMAC_BITS_CTL_BS_MASK);
	}

	return result;
}

static int32_t dw_dmac_chn_status_address(dw_dmac_t *dmac, uint32_t chn_id, uint32_t src_addr, uint32_t dst_addr)
{
	int32_t result = E_OK;

	if ((NULL == dmac) || (chn_id >= dmac->nof_chn)) {
		result = E_PAR;
	} else {
		raw_writel(DMAC_REG_SSTATAR(chn_id), src_addr);
		raw_writel(DMAC_REG_DSTATAR(chn_id), dst_addr);
	}

	return result;
}

static int32_t dw_dmac_chn_config(dw_dmac_t *dmac, uint32_t chn_id, dw_dmac_chn_cfg_t *cfg)
{
	int32_t result = E_OK;

	if ((NULL == dmac) || (chn_id >= dmac->nof_chn) || (NULL == cfg)) {
		result = E_PAR;
	} else {
		uint32_t l_val = 0, h_val = 0;

		if (cfg->dst_reload) {
			l_val |= DMAC_BITS_CFG_RELOAD_DST;
		}
		if (cfg->src_reload) {
			l_val |= DMAC_BITS_CFG_RELOAD_SRC;
		}

		if (cfg->src_hw_if_pol) {
			l_val |= DMAC_BITS_CFG_SRC_HS_POL;
		}
		if (cfg->dst_hw_if_pol) {
			l_val |= DMAC_BITS_CFG_DST_HS_POL;
		}

		if (cfg->src_hw_hs_sel) {
			l_val |= DMAC_BITS_CFG_HS_SEL_SRC;
		}
		if (cfg->dst_hw_hs_sel) {
			l_val |= DMAC_BITS_CFG_HS_SEL_DST;
		}

		if (cfg->suspend) {
			l_val |= DMAC_BITS_CFG_CHN_SUSP;
		}
		l_val |= dmac_chn_priority(cfg->priority);

		raw_writel(DMAC_REG_CFG(chn_id), l_val);


		h_val |= dmac_chn_dst_hw_if(cfg->dst_hw_if);
		h_val |= dmac_chn_src_hw_if(cfg->src_hw_if);

		if (cfg->src_sts_upd_en) {
			h_val |= DMAC_BITS_CFG_SS_UPD_EN;
		}
		if (cfg->dst_sts_upd_en) {
			h_val |= DMAC_BITS_CFG_DS_UPD_EN;
		}

		h_val |= dmac_chn_prot_ctrl(cfg->prot_ctrl);

		if (cfg->fifo_mode) {
			h_val |= DMAC_BITS_CFG_FIFOMODE;
		}

		raw_writel(DMAC_REG_CFG(chn_id) + 4, h_val);
	}

	return result;
}

static int32_t dw_dmac_chn_gather(dw_dmac_t *dmac, uint32_t chn_id, uint32_t interval, uint32_t count)
{
	int32_t result = E_OK;

	if ((NULL == dmac) || (chn_id >= dmac->nof_chn)) {
		result = E_PAR;
	} else {
		raw_writel(DMAC_REG_SGR(chn_id), dmac_source_gather(interval, count));
	}

	return result;
}

static int32_t dw_dmac_chn_scatter(dw_dmac_t *dmac, uint32_t chn_id, uint32_t interval, uint32_t count)
{
	int32_t result = E_OK;

	if ((NULL == dmac) || (chn_id >= dmac->nof_chn)) {
		result = E_PAR;
	} else {
		raw_writel(DMAC_REG_DSR(chn_id), dmac_dest_scatter(interval, count));
	}

	return result;
}

static int32_t dw_dmac_chn_int_raw_status(dw_dmac_t *dmac, dw_dmac_inttype_t int_type)
{
	int32_t result = E_OK;

	uint32_t int_sts;

	if ((NULL == dmac) || (int_type >= INT_TYPE_INVALID)) {
		result = E_PAR;
	} else {
		switch (int_type) {
			case INT_TFR:
				int_sts = raw_readl(DMAC_REG_RAW_TFR);
				break;
			case INT_BLOCK:
				int_sts = raw_readl(DMAC_REG_RAW_BLOCK);
				break;
			case INT_SRC_TRAN:
				int_sts = raw_readl(DMAC_REG_RAW_SRC_TRANS);
				break;
			case INT_DST_TRAN:
				int_sts = raw_readl(DMAC_REG_RAW_DST_TRANS);
				break;
			case INT_ERR:
				int_sts = raw_readl(DMAC_REG_RAW_ERR);
				break;
			default:
				result = E_PAR;
		}

		/*
		if (E_OK == result) {
			if (int_sts & (1 << chn_id)) {
				result = 1;
			}
		}
		*/
	}

	if (E_OK == result) {
		result = (int32_t)int_sts;
	}

	return result;
}

static int32_t dw_dmac_chn_int_status(dw_dmac_t *dmac, dw_dmac_inttype_t int_type)
{
	int32_t result = E_OK;

	uint32_t int_sts = 0;

	if ((NULL == dmac) || (int_type >= INT_TYPE_INVALID)) {
		result = E_PAR;
	} else {
		switch (int_type) {
			case INT_TFR:
				int_sts = raw_readl(DMAC_REG_STATUS_TFR);
				break;
			case INT_BLOCK:
				int_sts = raw_readl(DMAC_REG_STATUS_BLOCK);
				break;
			case INT_SRC_TRAN:
				int_sts = raw_readl(DMAC_REG_STATUS_SRC_TRANS);
				break;
			case INT_DST_TRAN:
				int_sts = raw_readl(DMAC_REG_STATUS_DST_TRANS);
				break;
			case INT_ERR:
				int_sts = raw_readl(DMAC_REG_STATUS_ERR);
				break;
			default:
				result = E_PAR;
		}

		/*
		if (E_OK == result) {
			if (int_sts & (1 << chn_id)) {
				result = 1;
			}
		}
		*/
	}

	if (E_OK == result) {
		result = (int32_t)int_sts;
	}

	return result;
}

static int32_t dw_dmac_chn_int_enable(dw_dmac_t *dmac, uint32_t chn_id, dw_dmac_inttype_t int_type, uint32_t en)
{
	int32_t result = E_OK;

	if ((NULL == dmac) || (chn_id >= dmac->nof_chn)) {
		result = E_PAR;
	} else {
		uint32_t raddr = 0;
		switch (int_type) {
			case INT_TFR:
				raddr = DMAC_REG_MASK_TFR;
				break;
			case INT_BLOCK:
				raddr = DMAC_REG_MASK_BLOCK;
				break;
			case INT_SRC_TRAN:
				raddr = DMAC_REG_MASK_SRC_TRANS;
				break;
			case INT_DST_TRAN:
				raddr = DMAC_REG_MASK_DST_TRANS;
				break;
			case INT_ERR:
				raddr = DMAC_REG_MASK_ERR;
				break;
			default:
				result = E_PAR;
		}

		if (E_OK == result) {
			uint32_t value = raw_readl(raddr);
			if (en) {
				value |= ((1 << chn_id) << 8) | (1 << chn_id);
			} else {
				value &= ~(1 << chn_id);
				value |= (1 << chn_id) << 8;
			}
			raw_writel(raddr, value);
		}
	}

	return result;
}

static int32_t dw_dmac_chn_int_clear(dw_dmac_t *dmac, uint32_t chn_id, dw_dmac_inttype_t int_type)
{
	int32_t result = E_OK;

	if ((NULL == dmac) || (chn_id >= dmac->nof_chn)) {
		result = E_PAR;
	} else {
		uint32_t raddr = 0;
		switch (int_type) {
			case INT_TFR:
				raddr = DMAC_REG_CLEAR_TFR;
				break;
			case INT_BLOCK:
				raddr = DMAC_REG_CLEAR_BLOCK;
				break;
			case INT_SRC_TRAN:
				raddr = DMAC_REG_CLEAR_SRC_TRANS;
				break;
			case INT_DST_TRAN:
				raddr = DMAC_REG_CLEAR_DST_TRANS;
				break;
			case INT_ERR:
				raddr = DMAC_REG_CLEAR_ERR;
				break;
			default:
				result = E_PAR;
		}

		if (E_OK == result) {
			raw_writel(raddr, (1 << chn_id));
		}
	}

	return result;
}

static int32_t dw_dmac_int_status(dw_dmac_t *dmac, dw_dmac_inttype_t int_type)
{
	int32_t result = E_OK;

	if ((NULL == dmac) || (int_type >= INT_TYPE_INVALID)) {
		result = E_PAR;
	} else {
		uint32_t intsts = raw_readl(DMAC_REG_STATUS);
		result = (intsts & (1 << int_type)) ? (1) : (0);
	}

	return result;
}

static int32_t dw_dmac_int_disable_all(dw_dmac_t *dmac, dw_dmac_inttype_t int_type)
{
	int32_t result = E_OK;

	if ((NULL == dmac)) {
		result = E_PAR;
	} else {
		//uint32_t intsts = raw_readl(DMAC_REG_STATUS);
		switch (int_type) {
			case INT_TFR:
				raw_writel(DMAC_REG_MASK_TFR, 0xFF00);
				break;
			case INT_BLOCK:
				raw_writel(DMAC_REG_MASK_BLOCK, 0xFF00);
				break;
			case INT_SRC_TRAN:
				raw_writel(DMAC_REG_MASK_SRC_TRANS, 0xFF00);
				break;
			case INT_DST_TRAN:
				raw_writel(DMAC_REG_MASK_DST_TRANS, 0xFF00);
				break;
			case INT_ERR:
				raw_writel(DMAC_REG_MASK_ERR, 0xFF00);
				break;
			default:
				result = E_PAR;
		}
	}

	return result;
}

static int32_t dw_dmac_sw_request(dw_dmac_t *dmac, uint32_t chn_id)
{
	int32_t result = E_OK;

	if ((NULL == dmac) || (chn_id >= dmac->nof_chn)) {
		result = E_PAR;
	} else {
		uint32_t raddr, value;

		raddr = DMAC_REG_REQSRC;
		value = raw_readl(raddr);
		value |= ((1 << chn_id) << 8) | ((1 << chn_id));
		raw_writel(raddr, value);

		raddr = DMAC_REG_REQDST;
		value = raw_readl(raddr);
		value |= ((1 << chn_id) << 8) | ((1 << chn_id));
		raw_writel(raddr, value);

		raddr = DMAC_REG_SGLRQSRC;
		value = raw_readl(raddr);
		value |= ((1 << chn_id) << 8) | ((1 << chn_id));
		raw_writel(raddr, value);

		raddr = DMAC_REG_SGLRQDST;
		value = raw_readl(raddr);
		value |= ((1 << chn_id) << 8) | ((1 << chn_id));
		raw_writel(raddr, value);
	}

	return result;
}

static int32_t dw_dmac_enable(dw_dmac_t *dmac, uint32_t en)
{
	int32_t result = E_OK;

	if (NULL == dmac) {
		result = E_PAR;
	} else {
		if (en) {
			raw_writel(DMAC_REG_CFGREG, 1);
		} else {
			raw_writel(DMAC_REG_CFGREG, 0);
		}
	}

	return result;
}

static int32_t dw_dmac_chn_en(dw_dmac_t *dmac, uint32_t chn_id, uint32_t en)
{
	int32_t result = E_OK;

	if ((NULL == dmac) || (chn_id >= dmac->nof_chn)) {
		result = E_PAR;
	} else {
		uint32_t value = raw_readl(DMAC_REG_CHNEN);
		value |= ((1 << chn_id) << 8);
		if (en) {
			value |= (1 << chn_id);
		} else {
			value &= ~(1 << chn_id);
		}
		raw_writel(DMAC_REG_CHNEN, value);
	}

	return result;
}


static dw_dmac_ops_t dmac_ops = {
	.enable = dw_dmac_enable,
	.chn_enable = dw_dmac_chn_en,
	.sw_request = dw_dmac_sw_request,
	.int_status = dw_dmac_int_status,
	.int_disable = dw_dmac_int_disable_all,
	.chn_int_clear = dw_dmac_chn_int_clear,
	.chn_int_enable = dw_dmac_chn_int_enable,
	.chn_int_status = dw_dmac_chn_int_status,
	.chn_int_raw_status = dw_dmac_chn_int_raw_status,
	.chn_scatter = dw_dmac_chn_scatter,
	.chn_gather = dw_dmac_chn_gather,
	.chn_config = dw_dmac_chn_config,
	.chn_status_address = dw_dmac_chn_status_address,
	.chn_control = dw_dmac_chn_control,
	.chn_ll_pointer = dw_dmac_chn_linklist_pointer,
	.chn_address = dw_dmac_chn_address
};

int32_t dw_dmac_install_ops(dw_dmac_t *dev_dmac)
{
	int32_t result = E_OK;

	if ((NULL == dev_dmac)) {
		result = E_PAR;
	} else {
		dev_dmac->ops = (void *)&dmac_ops;
	}

	return result;
}
