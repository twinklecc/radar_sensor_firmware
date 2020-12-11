#ifndef _DMA_HAL_H_
#define _DMA_HAL_H_

#include "arc_exception.h"

enum hs_select {
	HS_SELECT_HW = 0,
	HS_SELECT_SW,
	HS_SELECT_INVALID
};

enum dma_channel_id {
	DMA_CHN_0 = 0,
	DMA_CHN_1,
	DMA_CHN_2,
	DMA_CHN_3,
	DMA_CHN_4,
	DMA_CHN_5,
	DMA_CHN_6,
	DMA_CHN_7
};

enum dma_transfer_type {
	MEM_TO_MEM = 0,
	MEM_TO_PERI,
	PERI_TO_MEM,
	PERI_TO_PERI,
	TRANS_TYPE_INVALID
};

enum dma_burst_length {
	BURST_LEN1 = 0,
	BURST_LEN4,
	BURST_LEN8,
	BURST_LEN16,
	BURST_LEN32,
	BURST_LEN64,
	BURST_LEN128,
	BURST_LEN256,
	BURST_LEN_INVALID
};

enum dma_address_mode {
	ADDRESS_INCREMENT = 0,
	ADDRESS_DECREMENT,
	ADDRESS_FIXED,
	ADDRESS_FIXED_1,
	ADDRESS_MODE_INVALID
};

enum dma_chn_priority {
	DMA_CHN_PRIOR_0 = 0,
	DMA_CHN_PRIOR_MIN = DMA_CHN_PRIOR_0,
	DMA_CHN_PRIOR_1,
	DMA_CHN_PRIOR_2,
	DMA_CHN_PRIOR_3,
	DMA_CHN_PRIOR_4,
	DMA_CHN_PRIOR_5,
	DMA_CHN_PRIOR_6,
	DMA_CHN_PRIOR_7,
	DMA_CHN_PRIOR_MAX = DMA_CHN_PRIOR_7,
	DMA_CHN_PEIOR_INVALID
};

enum dma_channel_status {
	DMA_CHN_IDLE = 0,
	DMA_CHN_ALLOC,
	DMA_CHN_INIT,
	DMA_CHN_BUSY,
	DMA_CHN_ERR,
	DMA_CHN_SUSPEND
};

enum dma_int_type {
	INT_TYPE_TRANSFER_COMPLETED = 0,
	INT_TYPE_BLOCK_COMPLETED,
	INT_TYPE_SRC_TRANSACTION_COMPLETED,
	INT_TYPE_DST_TRANSACTION_COMPLETED,
	INT_TYPE_ERR,
	INT_TYPE_MAX,
};

#define DMA_LLP_CNT_MAX		(35)
typedef struct dma_llp_descriptor {
	uint32_t sar;
	uint32_t dar;
	uint32_t next;
	uint32_t lctrl;
	uint32_t hctrl;
	/*
	uint32_t sstat;
	uint32_t dstat;
	*/
} dma_llp_desc_t;

typedef struct dma_chn_source {
	/* ctrl. */
	//uint8_t llp_en;
	//uint8_t gather_en;
	/* msize: not for user. */
	enum dma_burst_length burst_tran_len;
	/* sinc. */
	enum dma_address_mode addr_mode;

	/* cfg. */
	uint8_t hw_if;
	uint8_t sts_upd_en;
	//uint8_t reload;
	//hw_if_pol;
	/* hw_hs_sel. */
	enum hs_select hs;

	uint32_t addr;
	uint32_t sts_addr;

	uint32_t gather_iv;
	uint32_t gather_cnt;
} dma_chn_src_t;
typedef struct dma_chn_destination {
	/* ctrl. */
	//uint8_t llp_en;
	//uint8_t scatter_en;
	/* msize: not for user. */
	enum dma_burst_length burst_tran_len;
	/* dinc. */
	enum dma_address_mode addr_mode;

	/* cfg. */
	uint8_t hw_if;
	uint8_t sts_upd_en;
	//uint8_t reload;
	//hw_if_pol;
	/* hw_hs_sel. */
	enum hs_select hs;

	uint32_t addr;
	uint32_t sts_addr;

	uint32_t scatter_iv;
	uint32_t scatter_cnt;
} dma_chn_dst_t;
typedef struct dma_transfer_description {
	//uint8_t work_mode;

	/* ctrl. */
	/* block_ts. */
	//uint32_t block_transfer_size;
	/* tt_fc: M2M, M2P, P2M, P2P. */
	enum dma_transfer_type transfer_type;
	//int_en;

	/* cfg. */
	/* priority. */
	enum dma_chn_priority priority;

	dma_chn_src_t src_desc;
	dma_chn_dst_t dst_desc;

	uint32_t block_transfer_size;

} dma_trans_desc_t;

typedef void (*dma_chn_callback)(void *parameter);
typedef struct dma_channel {
	uint8_t chn_id;
	enum dma_channel_status status;

	uint8_t work_mode;

	dma_trans_desc_t desc;
	volatile dma_llp_desc_t llp[DMA_LLP_CNT_MAX];

	dma_chn_callback callback;
} dma_channel_t;


int32_t dma_init(void);
int32_t dma_transfer(dma_trans_desc_t *desc, dma_chn_callback func);
int32_t dma_memcopy(uint32_t *dst, uint32_t *src, uint32_t len);
int32_t dma_user_memcopy(uint32_t *dst, uint32_t *src, uint32_t len, dma_chn_callback func);

int32_t dma_apply_channel(void);
int32_t dma_config_channel(uint32_t chn_id, dma_trans_desc_t *desc, uint32_t int_flag);
int32_t dma_start_channel(uint32_t chn_id, dma_chn_callback func);
int32_t dma_release_channel(uint32_t chn_id);

typedef enum {
	DMA_SRC_NR_NG_NLLP_DST_NR_NS_NLLP = 0,
	DMA_SRC_NR_NG_NLLP_DST_NR_NS_LLP,
	DMA_SRC_NR_NG_NLLP_DST_NR_S_NLLP,
	DMA_SRC_NR_NG_NLLP_DST_NR_S_LLP,
	DMA_SRC_NR_NG_NLLP_DST_R_NS_NLLP,

	/* not supported. */
	//DMA_SRC_NR_NG_NLLP_DST_R_NS_LLP,
	//DMA_SRC_NR_NG_NLLP_DST_R_S_NLLP,
	//DMA_SRC_NR_NG_NLLP_DST_R_S_LLP,

	DMA_SRC_NR_NG_LLP_DST_NR_NS_NLLP = 0x8,
	DMA_SRC_NR_NG_LLP_DST_NR_NS_LLP,
	DMA_SRC_NR_NG_LLP_DST_NR_S_NLLP,
	DMA_SRC_NR_NG_LLP_DST_NR_S_LLP,
	DMA_SRC_NR_NG_LLP_DST_R_NS_NLLP,

	/* not supported. */
	//DMA_SRC_NR_NG_LLP_DST_R_NS_LLP,
	//DMA_SRC_NR_NG_LLP_DST_R_S_NLLP,
	//DMA_SRC_NR_NG_LLP_DST_R_S_LLP,

	DMA_SRC_NR_G_NLLP_DST_NR_NS_NLLP = 0x10,
	DMA_SRC_NR_G_NLLP_DST_NR_NS_LLP,
	DMA_SRC_NR_G_NLLP_DST_NR_S_NLLP,
	DMA_SRC_NR_G_NLLP_DST_NR_S_LLP,
	DMA_SRC_NR_G_NLLP_DST_R_NS_NLLP,

	/* not supported. */
	//DMA_SRC_NR_G_NLLP_DST_R_NS_LLP,
	//DMA_SRC_NR_G_NLLP_DST_R_S_NLLP,
	//DMA_SRC_NR_G_NLLP_DST_R_S_LLP,

	DMA_SRC_NR_G_LLP_DST_NR_NS_NLLP = 0x18,
	DMA_SRC_NR_G_LLP_DST_NR_NS_LLP,
	DMA_SRC_NR_G_LLP_DST_NR_S_NLLP,
	DMA_SRC_NR_G_LLP_DST_NR_S_LLP,
	DMA_SRC_NR_G_LLP_DST_R_NS_NLLP,

	/* not supported. */
	//DMA_SRC_NR_G_LLP_DST_R_NS_LLP,
	//DMA_SRC_NR_G_LLP_DST_R_S_NLLP,
	//DMA_SRC_NR_G_LLP_DST_R_S_LLP,

	DMA_SRC_R_NG_NLLP_DST_NR_NS_NLLP = 0x20,
	DMA_SRC_R_NG_NLLP_DST_NR_NS_LLP,
	DMA_SRC_R_NG_NLLP_DST_NR_S_NLLP,
	DMA_SRC_R_NG_NLLP_DST_NR_S_LLP,
	DMA_SRC_R_NG_NLLP_DST_R_NS_NLLP,

	/* not supported. */
	//DMA_SRC_R_NG_NLLP_DST_R_NS_LLP,
	//DMA_SRC_R_NG_NLLP_DST_R_S_NLLP,
	//DMA_SRC_R_NG_NLLP_DST_R_S_LLP,

	DMA_SRC_R_NG_LLP_DST_NR_NS_NLLP = 0x28,
	DMA_SRC_R_NG_LLP_DST_NR_NS_LLP,
	DMA_SRC_R_NG_LLP_DST_NR_S_NLLP,
	DMA_SRC_R_NG_LLP_DST_NR_S_LLP,
	DMA_SRC_R_NG_LLP_DST_R_NS_NLLP,

	/* not supported. */
	//DMA_SRC_R_NG_LLP_DST_R_NS_LLP,
	//DMA_SRC_R_NG_LLP_DST_R_S_NLLP,
	//DMA_SRC_R_NG_LLP_DST_R_S_LLP,
#if 0
	DMA_SRC_NR_G_LLP_DST_NR_NS_NLLP = 0x30,
	DMA_SRC_NR_G_LLP_DST_NR_NS_LLP,
	DMA_SRC_NR_G_LLP_DST_NR_S_NLLP,
	DMA_SRC_NR_G_LLP_DST_NR_S_LLP,
	DMA_SRC_NR_G_LLP_DST_R_NS_NLLP,
	DMA_SRC_NR_G_LLP_DST_R_NS_LLP,
	DMA_SRC_NR_G_LLP_DST_R_S_NLLP,
	DMA_SRC_NR_G_LLP_DST_R_S_LLP,
	DMA_SRC_R_G_LLP_DST_NR_NS_NLLP,
	DMA_SRC_R_G_LLP_DST_NR_NS_LLP,
	DMA_SRC_R_G_LLP_DST_NR_S_NLLP,
	DMA_SRC_R_G_LLP_DST_NR_S_LLP,
	DMA_SRC_R_G_LLP_DST_R_NS_NLLP,
	DMA_SRC_R_G_LLP_DST_R_NS_LLP,
	DMA_SRC_R_G_LLP_DST_R_S_NLLP,
	DMA_SRC_R_G_LLP_DST_R_S_LLP
#endif
	DMA_WORK_MODE_INVALID
} dma_work_mode_t;
#endif
