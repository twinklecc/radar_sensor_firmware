#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC.h"
#include "embARC_debug.h"

#include "arc_exception.h"
#include "dw_dmac.h"
#include "dma_hal.h"

/* 0->raising eddge.
 * 1->falling eddge.
 * */
#define DW_DMAC_REQ_SIGNAL_POL		(0)

typedef struct dma_driver {
	void *dev_dma;
	uint8_t open_flag;

	/* cfg: global config for device. */
	uint8_t prot_ctrl;
	uint8_t fifo_mode;

	uint8_t chn_cnt;
	dma_channel_t *dma_chn;
} dma_drv_t;
static dma_drv_t drv_dma = {
	.prot_ctrl = 0,
	.fifo_mode = 0,
};

#define DMA_MEMCOPY_DEFAULT_DESC(desc) do { \
		(desc)->transfer_type = MEM_TO_MEM; \
		(desc)->priority = DMA_CHN_PRIOR_MAX; \
		(desc)->src_desc.addr_mode = ADDRESS_INCREMENT; \
		(desc)->src_desc.hw_if = 0; \
		(desc)->src_desc.sts_upd_en = 0; \
		(desc)->src_desc.hs = HS_SELECT_SW; \
		(desc)->src_desc.sts_addr = 0; \
		(desc)->dst_desc.addr_mode = ADDRESS_INCREMENT; \
		(desc)->dst_desc.hw_if = 0; \
		(desc)->dst_desc.sts_upd_en = 0; \
		(desc)->dst_desc.hs = HS_SELECT_SW; \
		(desc)->dst_desc.sts_addr = 0;\
	} while(0)


static void dma_isr(void *params);
static void dma_err_isr(void *params);
int32_t dma_init(void)
{
	int32_t result = E_OK;

	dw_dmac_t *dev_dma = (dw_dmac_t *)dma_get_dev(0);
	dw_dmac_ops_t *dma_ops = (dw_dmac_ops_t *)dev_dma->ops;

	do {
		uint32_t idx, chn_desc_size = 0;

		if ((NULL == dev_dma) || (NULL == dev_dma->ops)) {
			result = E_NOEXS;
			break;
		} else {
			drv_dma.dev_dma = (void *)dev_dma;
		}

		/* alloc memory for channel manager. */
		chn_desc_size = sizeof(dma_channel_t) * dev_dma->nof_chn;
		drv_dma.dma_chn = (dma_channel_t *)pvPortMalloc(chn_desc_size);
		if (NULL == drv_dma.dma_chn) {
			result = E_SYS;
			break;
		} else {
			drv_dma.chn_cnt = dev_dma->nof_chn;
		}

		for (idx = 0; idx < dev_dma->nof_chn; idx++)
		{
			drv_dma.dma_chn[idx].chn_id = idx;
			drv_dma.dma_chn[idx].status = DMA_CHN_IDLE;
		}

		/* regsiter isr. */
		result = int_handler_install(dev_dma->chn_int[INT_TFR], dma_isr);
		if (result < 0) {
			break;
		} else {
			/* enable intc of dma. */
			dmu_irq_enable(dev_dma->chn_int[INT_TFR], 1);
		}
		result = int_enable(dev_dma->chn_int[INT_TFR]);
		if (result < 0) {
			break;
		}

		result = int_handler_install(dev_dma->chn_int[INT_ERR], dma_err_isr);
		if (result < 0) {
			break;
		} else {
			dmu_irq_enable(dev_dma->chn_int[INT_ERR], 1);
		}
		result = int_enable(dev_dma->chn_int[INT_ERR]);
		if (result < 0) {
			break;
		}

		/* enable dmac. */
		dma_enable(1);
		result = dma_ops->enable(dev_dma, 1);

	} while (0);

	return result;
}

int32_t dma_apply_channel(void)
{
	int32_t result = E_DBUSY;

	dw_dmac_t *dev_dma = (dw_dmac_t *)drv_dma.dev_dma;
	dma_channel_t *chn_info = drv_dma.dma_chn;

	if ((NULL == chn_info) || (NULL == dev_dma)) {
		result = E_SYS;
	} else {
		uint32_t idx, chn_cnt = dev_dma->nof_chn;

		uint32_t cpu_sts = arc_lock_save();
		for (idx =0; idx < chn_cnt; idx++) {
			if (DMA_CHN_IDLE == chn_info->status) {
				result = idx;
				chn_info[idx].status = DMA_CHN_ALLOC;
				break;
			}
		}
		arc_unlock_restore(cpu_sts);
	}

	return result;
}


/*
 * work mode:
 **********************************************************
 * 		SRC	    *		DST		  *
 **********************************************************
 * 	5	4	3	2	1	0	  *
 * 	RELOAD	Gather	LLP	RELOAD	Scatter	LLP	  *
 **********************************************************/
static void dma_chn_desc2cfg(uint8_t work_mode, dma_trans_desc_t *desc, \
		dw_dmac_chn_cfg_t *chn_cfg)
{
	dma_chn_src_t *chn_src = &desc->src_desc;
	dma_chn_dst_t *chn_dst = &desc->dst_desc;

	chn_cfg->dst_hw_if = chn_dst->hw_if;
	chn_cfg->src_hw_if = chn_src->hw_if;
	chn_cfg->src_sts_upd_en = chn_src->sts_upd_en;
	chn_cfg->dst_sts_upd_en = chn_dst->sts_upd_en;
	//chn_cfg.prot_ctrl = 0;
	//chn_cfg->fifo_mode = 0;
	//chn_cfg->dst_reload = chn_dst->reload;
	//chn_cfg->src_reload = chn_dst->reload;
	chn_cfg->dst_reload = ((work_mode >> 2) & 0x1) ? (1) : (0);
	chn_cfg->src_reload = ((work_mode >> 5) & 0x1) ? (1) : (0);

	chn_cfg->src_hw_if_pol = DW_DMAC_REQ_SIGNAL_POL;
	chn_cfg->dst_hw_if_pol = DW_DMAC_REQ_SIGNAL_POL;

	chn_cfg->src_hw_hs_sel = chn_src->hs;
	chn_cfg->dst_hw_hs_sel = chn_dst->hs;
	//chn_cfg->suspend = 0;
	chn_cfg->priority = desc->priority;
}
static void dma_chn_desc2ctrl(uint8_t work_mode, dma_trans_desc_t *desc, \
		dw_dmac_chn_ctrl_t *chn_ctrl)
{
	dma_chn_src_t *chn_src = &desc->src_desc;
	dma_chn_dst_t *chn_dst = &desc->dst_desc;

	chn_ctrl->block_ts = desc->block_transfer_size;
	//chn_ctrl->llp_src_en = chn_src->llp_en;
	//chn_ctrl->llp_dst_en = chn_dst->llp_en;
	chn_ctrl->llp_src_en = ((work_mode >> 3) & 0x1) ? (1) : (0);
	chn_ctrl->llp_dst_en = (work_mode & 0x1) ? (1) : (0);
	chn_ctrl->tt_fc = desc->transfer_type;

	//chn_ctrl->dst_scatter_en = chn_dst->scatter_en;
	//chn_ctrl->src_gather_en = chn_src->gather_en;
	chn_ctrl->dst_scatter_en = ((work_mode >> 1) & 0x1) ? (1) : (0);
	chn_ctrl->src_gather_en = ((work_mode >> 4) & 0x1) ? (1) : (0);

	chn_ctrl->src_msize = chn_src->burst_tran_len;
	chn_ctrl->dst_msize = chn_dst->burst_tran_len;
	chn_ctrl->sinc = chn_src->addr_mode;
	chn_ctrl->dinc = chn_dst->addr_mode;
	//chn_ctrl->int_en = 1;
}
static inline uint32_t dma_llp_cnt(dma_trans_desc_t *desc)
{
	uint32_t block_cnt, llp_cnt = 1;

	uint32_t total_size = desc->block_transfer_size;

	/* TODO: byte to single transaction size(word). */
	block_cnt = total_size >> 2;
	if (total_size % 4) {
		block_cnt += 1;
	}
	if (block_cnt > 31) {
		llp_cnt = block_cnt / 31;
		if (block_cnt % 31) {
			llp_cnt += 1;
		}
	} else {
		llp_cnt = 1;
	}

	return llp_cnt;
}
static inline void dma_llp_filling(dma_channel_t *chn_info, dma_trans_desc_t *desc, \
		dw_dmac_chn_ctrl_t *ctrl)
{
	uint32_t idx = 0, l_ctrlr = 0;
	uint32_t msize, single_cnt, block_cnt = 31;

	uint32_t src = desc->src_desc.addr;
	uint32_t dst = desc->dst_desc.addr;
	uint32_t total_size = desc->block_transfer_size;

	uint32_t next_llp_base = 0;
	volatile dma_llp_desc_t *llp_desc = chn_info->llp;

	l_ctrlr = dma_ctrl2regl(ctrl);

	if (MEM_TO_PERI == desc->transfer_type) {
		block_cnt = 1;
		if (desc->dst_desc.burst_tran_len) {
			msize = 1 << (desc->dst_desc.burst_tran_len + 1);
			//msize *= 8;
		} else {
			msize = 1;
		}
	} else if (PERI_TO_MEM == desc->transfer_type) {
		block_cnt = 1;
		if (desc->src_desc.burst_tran_len) {
			msize = 1 << (desc->src_desc.burst_tran_len + 1);
			//msize *= 8;
		} else {
			msize = 1;
		}
	} else {
		msize = 1;
	}
	single_cnt = block_cnt * msize;

	/* filling llp. */
	while (total_size) {
		_arc_write_uncached_32((void *)&llp_desc->sar, src);
		_arc_write_uncached_32((void *)&llp_desc->dar, dst);

		_arc_write_uncached_32((void *)&llp_desc->lctrl, l_ctrlr);
		//llp_desc->hctrl = dma_ctrl2regh(&);

		if (total_size < single_cnt << 2) {
			if (MEM_TO_MEM == desc->transfer_type) {
				single_cnt = total_size >> 2;
				if (total_size & 0x3) {
					single_cnt += 1;
				}
				block_cnt = single_cnt;
			}

			/* llp end. */
			next_llp_base = 0;
			total_size = 0;
		} else {
			next_llp_base = (uint32_t)(llp_desc + 1);
			total_size -= single_cnt << 2;
		}
		_arc_write_uncached_32((void *)&llp_desc->next, next_llp_base);

		_arc_write_uncached_32((void *)&llp_desc->hctrl, block_cnt);
		/*
		llp_desc->sstat = desc->src_desc.sts_addr;
		llp_desc->dstat = desc->dst_desc.sts_addr;
		*/

		switch (desc->src_desc.addr_mode) {
			case ADDRESS_INCREMENT:
				src += single_cnt << 2;
				break;
			case ADDRESS_DECREMENT:
				src -= single_cnt << 2;
				break;
			case ADDRESS_FIXED:
				break;
			case ADDRESS_FIXED_1:
			default:
				/* nothing to do? */
				break;
		}

		switch (desc->dst_desc.addr_mode) {
			case ADDRESS_INCREMENT:
				dst += single_cnt << 2;
				break;
			case ADDRESS_DECREMENT:
				dst -= single_cnt << 2;
				break;
			case ADDRESS_FIXED:
				break;
			case ADDRESS_FIXED_1:
			default:
				/* nothing to do? */
				break;
		}

		idx += 1;
		llp_desc++;
	}
}
int32_t dma_config_channel(uint32_t chn_id, dma_trans_desc_t *desc, uint32_t int_flag)
{
	int32_t result = E_OK;


	dw_dmac_t *dev_dma = (dw_dmac_t *)drv_dma.dev_dma;
	dw_dmac_ops_t *dma_ops = (dw_dmac_ops_t *)dev_dma->ops;
	dma_channel_t *chn_info = drv_dma.dma_chn;

	uint8_t work_mode = chn_info[chn_id].work_mode;

	do {
		uint32_t llp_cnt = 0;
		dw_dmac_chn_ctrl_t chn_ctrl;
		dw_dmac_chn_cfg_t chn_cfg;

		if ((NULL == chn_info) || (NULL == dev_dma) || (NULL == dev_dma->ops) || \
		    (chn_id >= dev_dma->nof_chn) || (NULL == desc)) {
			result = E_SYS;
			break;
		}

		if (DMA_CHN_ALLOC != chn_info[chn_id].status) {
			result = E_SYS;
			break;
		}

		/* restore channel config... */
		//memcpy((void *)chn_info[chn_id].desc, (void *)desc, sizeof(dma_trans_desc_t));

		chn_cfg.prot_ctrl = drv_dma.prot_ctrl;
		chn_cfg.suspend = 0;
		chn_cfg.fifo_mode = drv_dma.fifo_mode;
		dma_chn_desc2cfg(work_mode, desc, &chn_cfg);
		result = dma_ops->chn_config(dev_dma, chn_id, &chn_cfg);
		if (E_OK != result) {
			break;
		}

		if ((chn_cfg.src_sts_upd_en) || (chn_cfg.dst_sts_upd_en)) {
			result = dma_ops->chn_status_address(dev_dma, chn_id, \
					desc->src_desc.sts_addr, desc->dst_desc.sts_addr);
			if (E_OK != result) {
				break;
			}
		}


		/* compute the transfer length, if need llp, then open it. */
		result = (int32_t)dma_llp_cnt(desc);
		if (result <= 0) {
			result = E_SYS;
			break;
		} else if (result > 1) {
			llp_cnt = (uint32_t)result;
			if (llp_cnt >= DMA_LLP_CNT_MAX) {
				result = E_PAR;
				break;
			} else {
				/* enable llp. */
				work_mode |= (1 << 3) | 1;
			}
			chn_ctrl.int_en = int_flag;
			dma_chn_desc2ctrl(work_mode, desc, &chn_ctrl);
			/* filling llp. */
			dma_llp_filling(&chn_info[chn_id], desc, &chn_ctrl);
		} else {
			EMBARC_PRINTF("single...%d!\r\n", desc->block_transfer_size);
			/* TODO: byte to single transaction size(word). */
			desc->block_transfer_size >>= 2;
			//desc->block_transfer_size -= 1;

			chn_ctrl.int_en = int_flag;
			dma_chn_desc2ctrl(work_mode, desc, &chn_ctrl);
		}

		result = dma_ops->chn_control(dev_dma, chn_id, &chn_ctrl);
		if (E_OK != result) {
			break;
		}

		if (chn_ctrl.src_gather_en) {
			result = dma_ops->chn_gather(dev_dma, chn_id, \
					desc->src_desc.gather_iv, desc->src_desc.gather_cnt);
			if (E_OK != result) {
				break;
			}
		}
		if (chn_ctrl.dst_scatter_en) {
			result = dma_ops->chn_scatter(dev_dma, chn_id, \
					desc->dst_desc.scatter_iv, desc->dst_desc.scatter_cnt);
			if (E_OK != result) {
				break;
			}
		}

		result = dma_ops->chn_address(dev_dma, chn_id, \
				desc->src_desc.addr, desc->dst_desc.addr);
		if (E_OK != result) {
			break;
		}

		if ((chn_ctrl.llp_src_en) || (chn_ctrl.llp_dst_en)) {
			result = dma_ops->chn_ll_pointer(dev_dma, chn_id, (uint32_t)chn_info[chn_id].llp);
			if (E_OK != result) {
				break;
			}
		}

		chn_info[chn_id].status = DMA_CHN_INIT;
	} while (0);

	return result;
}

int32_t dma_start_channel(uint32_t chn_id, dma_chn_callback func)
{
	int32_t result = E_OK;

	dw_dmac_t *dev_dma = (dw_dmac_t *)drv_dma.dev_dma;
	dw_dmac_ops_t *dma_ops = (dw_dmac_ops_t *)dev_dma->ops;
	dma_channel_t *chn_info = drv_dma.dma_chn;

	if ((NULL == chn_info) || (NULL == dev_dma) || (NULL == dev_dma->ops) || \
	    (chn_id >= dev_dma->nof_chn)) {
		result = E_SYS;
	} else {
		do {
			chn_info[chn_id].callback = func;
			if (func) {
				result = dma_ops->chn_int_enable(dev_dma, chn_id, \
						INT_TFR, 1);
				if (E_OK != result) {
					break;
				}
			}

			result = dma_ops->chn_enable(dev_dma, chn_id, 1);
			if (E_OK == result) {
				chn_info[chn_id].status = DMA_CHN_BUSY;
			}
		} while (0);
	}

	return result;
}

int32_t dma_release_channel(uint32_t chn_id)
{
	int32_t result = E_OK;

	dw_dmac_t *dev_dma = (dw_dmac_t *)drv_dma.dev_dma;
	dw_dmac_ops_t *dma_ops = (dw_dmac_ops_t *)dev_dma->ops;
	dma_channel_t *chn_info = drv_dma.dma_chn;

	if ((NULL == chn_info) || (NULL == dev_dma) || \
	    (chn_id >= dev_dma->nof_chn)) {
		result = E_SYS;
	} else {
		chn_info[chn_id].callback = NULL;
		result = dma_ops->chn_enable(dev_dma, chn_id, 0);
		if (E_OK == result) {
			chn_info[chn_id].status = DMA_CHN_IDLE;
		}
	}

	return result;
}

int32_t dma_transfer(dma_trans_desc_t *desc, dma_chn_callback func)
{
	int32_t result = E_OK;

	uint32_t chn_id, int_flag = 0;

	do {
		if (NULL == desc) {
			result = E_PAR;
			break;
		}

		result = dma_apply_channel();
		if (result < 0) {
			break;
		} else {
			chn_id = (uint32_t)result;
		}

		drv_dma.dma_chn[chn_id].work_mode = DMA_SRC_NR_NG_NLLP_DST_NR_NS_NLLP;

		if (func) {
			int_flag = 1;
		}
		result = dma_config_channel(chn_id, desc, int_flag);
		if (E_OK != result) {
			result = dma_release_channel(chn_id);
			break;
		}

		result = dma_start_channel(chn_id, func);
		if (E_OK != result) {
			break;
		}

	} while (0);

	return result;
}


static int32_t dma_sw_request(uint32_t chn_id)
{
	int32_t result = E_OK;

	dw_dmac_t *dev_dma = (dw_dmac_t *)drv_dma.dev_dma;
	dw_dmac_ops_t *dma_ops = (dw_dmac_ops_t *)dev_dma->ops;

	if ((NULL == dev_dma) || (NULL == dma_ops) || \
			(chn_id >= dev_dma->nof_chn)) {
		result = E_SYS;
	} else {
		result = dma_ops->sw_request(dev_dma, chn_id);
	}

	return result;
}

static void dma_memcopy_callback(void *params)
{
	int32_t result = dma_release_channel((uint32_t)params);
	if (E_OK != result) {
		/* TODO: record error! */
	}
}
static int32_t dma_raw_memcopy(uint32_t *dst, uint32_t *src, uint32_t len, dma_chn_callback func)
{
	int32_t result = E_OK;

	uint32_t chn_id = 0;
	//uint32_t llp_cnt, block_cnt = 0;
	dma_chn_callback chn_isr_callback = dma_memcopy_callback;

	dma_trans_desc_t *desc = NULL;
	//memcpy((void *)chn_info[chn_id].desc, (void *)desc, sizeof(dma_trans_desc_t));

	do {
		if ((NULL == dst) || (NULL == src) || (0 == len)) {
			result = E_PAR;
			break;
		}

		result = dma_apply_channel();
		if (E_OK != result) {
			break;
		} else {
			chn_id = (uint32_t)result;
		}

		DMA_MEMCOPY_DEFAULT_DESC(&drv_dma.dma_chn[chn_id].desc);
		desc = &drv_dma.dma_chn[chn_id].desc;

		drv_dma.dma_chn[chn_id].work_mode = DMA_SRC_NR_NG_NLLP_DST_NR_NS_NLLP;

		desc->src_desc.burst_tran_len = BURST_LEN1;
		desc->src_desc.addr = (uint32_t)src;
		desc->dst_desc.burst_tran_len = BURST_LEN1;
		desc->dst_desc.addr = (uint32_t)dst;
		//desc->block_transfer_size = (len >> 2);
		desc->block_transfer_size = len;

		result = dma_config_channel(chn_id, desc, 1);
		if (E_OK != result) {
			result = dma_release_channel(chn_id);
			break;
		}

		if (NULL != func) {
			chn_isr_callback = func;
		}
		result = dma_start_channel(chn_id, chn_isr_callback);
		if (E_OK != result) {
			break;
		}

	} while (0);

	return result;
}

int32_t dma_memcopy(uint32_t *dst, uint32_t *src, uint32_t len)
{
	return dma_raw_memcopy(dst, src, len, NULL);
}

int32_t dma_user_memcopy(uint32_t *dst, uint32_t *src, uint32_t len, dma_chn_callback func)
{
	return dma_raw_memcopy(dst, src, len, func);
}

static void dma_isr(void *params)
{
	int32_t result = E_OK;

	uint32_t chn_id = 0;
	uint32_t intsts = 0;

	dw_dmac_t *dev_dma = (dw_dmac_t *)drv_dma.dev_dma;
	dw_dmac_ops_t *dma_ops = (dw_dmac_ops_t *)dev_dma->ops;
	dma_channel_t *chn_info = drv_dma.dma_chn;

	do {
		if ((NULL == dev_dma) || (NULL == dma_ops)) {
			/* TODO: record error! */
			result = E_SYS;
			break;
		}

		result = dma_ops->int_status(dev_dma, INT_TFR);
		if (result <= 0) {
			/* TODO: record error! */
			result = dma_ops->int_disable(dev_dma, INT_TFR);
			break;
		}

		result = dma_ops->chn_int_status(dev_dma, INT_TFR);
		if (result <= 0) {
			/* TODO: record error! */
			break;
		} else {
			intsts = (uint32_t)result;
		}

		for (; chn_id < dev_dma->nof_chn; chn_id++) {
			if (intsts & (1 << chn_id)) {
				if (chn_info[chn_id].callback) {
					chn_info[chn_id].callback((void *)chn_id);
				}
				result = dma_ops->chn_int_clear(dev_dma, chn_id, INT_TFR);
				if (E_OK != result) {
					/* TODO: record error! */
					break;
				}
			}
		}

	} while (0);
}

static void dma_err_isr(void *params)
{
	int32_t result = E_OK;

	uint32_t chn_id = 0;
	uint32_t intsts = 0;

	dw_dmac_t *dev_dma = (dw_dmac_t *)drv_dma.dev_dma;
	dw_dmac_ops_t *dma_ops = (dw_dmac_ops_t *)dev_dma->ops;
	//dma_channel_t *chn_info = drv_dma.dma_chn;

	do {
		if ((NULL == dev_dma) || (NULL == dma_ops)) {
			/* TODO: record error! */
			result = E_SYS;
			break;
		}

		result = dma_ops->int_status(dev_dma, INT_ERR);
		if (E_OK != result) {
			/* TODO: record error! */
			result = dma_ops->int_disable(dev_dma, INT_ERR);
			break;
		}

		result = dma_ops->chn_int_status(dev_dma, INT_ERR);
		if (result < 0) {
			/* TODO: record error! */
			break;
		} else {
			intsts = (uint32_t)result;
		}

		for (; chn_id < dev_dma->nof_chn; chn_id++) {
			if (intsts & (1 << chn_id)) {
				/* TODO: record error! */
				;
			}
		}
	} while (0);
}
