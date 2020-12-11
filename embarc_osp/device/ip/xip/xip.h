#ifndef _FLASH_XIP_H_
#define _FLASH_XIP_H_

#include "xip_reg.h"

#define XIP_AES_TWEAK_COUNT		(16)

typedef enum xip_endian {
	XIP_LIT_ENDIAN = 0,
	XIP_BIG_ENDIAN
} xip_endian_t;

typedef struct xip_controller {
	uint32_t base;

	void *ops;
} xip_t;

typedef enum xip_instruction_type {
	INS_READ_STATUS = 0,
	INS_READ_MEMORY,
	INS_WRITE_ENABLE,
	INS_PROGRAM,
	INS_PROGRAM_SUSPEND,
	INS_PROGRAM_RESUME
} xip_ins_type_t;

/*
 * description: define for nvm header.
 * */
typedef struct xip_controller_reg {
	uint32_t ctrl0;
	uint32_t ctrl1;
	uint32_t baud;
	uint32_t rx_sample_delay;
	uint32_t spi_ctrl;

	/* xip reg. */
	uint32_t read_cmd;
	uint32_t ahb_endian;
	uint32_t aes_endian;
	uint32_t xip_offset;
	uint32_t xip_rx_buf_len;
	uint32_t data_offset;
	uint32_t data_rx_buf_len;

	/* aes reg. */
	uint32_t mode;
	uint32_t block_cnt;
	uint32_t last_block_size;
} xip_ctrl_t;

int32_t xip_install_ops(xip_t *xip);

typedef enum flash_xip_aes_mode {
	CIPHER_MODE = 0,
	DECIPHER_MODE
} aes_mode_t;

typedef enum flash_xip_aes_path {
	AES_IN_DATA_PATH = 0,
	AES_IN_CPU_INS_PATH
} aes_path_t;

typedef enum flash_xip_aes_tweak_mode {
	AES_TWEAK_BY_ADDR_BLOCKSZ = 0,
	AES_TWEAK_BY_ADDR_BLOCKSZ_MASK
} aes_tweak_mode_t;

typedef struct flash_xip_aes_config {
	//uint8_t mode;
	uint8_t path_sel;
	uint8_t data_path_bypass;
	uint8_t tweak_mode;
	uint32_t tweak_mask;
	uint32_t tweak[4];
	uint32_t seed;
} aes_cfg_t;

typedef struct flash_xip_command_format {
	uint8_t ins;
	uint8_t ins_len;
	uint8_t addr_len;
	uint8_t dfs;
} flash_xip_cmd_t;

typedef enum flash_xip_preload_buffer_size {
	FLASH_XIP_BUF_16W = 0,
	FLASH_XIP_BUF_32W,
	FLASH_XIP_BUF_64W,
	FLASH_XIP_BUF_128W,
	FLASH_XIP_BUF_256W,
	FLASH_XIP_BUF_512W
} flash_xip_buf_lvl_t;

#define FLASH_XIP_CMD_INIT(ins, ins_len, addr_len, dfs) (\
	((ins) & 0xFF) | \
	(((ins_len) & BITS_FLASH_XIP_CMD_INS_LEN_MASK) << BITS_FLASH_XIP_CMD_INS_LEN_SHIFT) |\
	(((addr_len) & BITS_FLASH_XIP_CMD_ADDR_LEN_MASK) << BITS_FLASH_XIP_CMD_ADDR_LEN_SHIFT) |\
	(((dfs) & BITS_FLASH_XIP_CMD_DFS_MASK) << BITS_FLASH_XIP_CMD_DFS_SHIFT) )

static inline void flash_xip_enable(xip_t *xip, uint32_t enable)
{
	if (enable) {
		raw_writel(xip->base + REG_FLASH_XIP_XER_OFFSET, 1);
	} else {
		raw_writel(xip->base + REG_FLASH_XIP_XER_OFFSET, 0);
	}
}

static inline void flash_xip_read_status_cmd(xip_t *xip, uint32_t cmd)
{
	raw_writel(xip->base + REG_FLASH_XIP_XRDSRCR_OFFSET, cmd);
}

static inline void flash_xip_read_cmd(xip_t *xip, uint32_t cmd)
{
	raw_writel(xip->base + REG_FLASH_XIP_XRDCR_OFFSET, cmd);
}

static inline void flash_xip_write_enable_cmd(xip_t *xip, uint32_t cmd)
{
	raw_writel(xip->base + REG_FLASH_XIP_XWENACR_OFFSET, cmd);
}

static inline void flash_xip_write_cmd(xip_t *xip, uint32_t cmd)
{
	raw_writel(xip->base + REG_FLASH_XIP_XWECR_OFFSET, cmd);
}

static inline void flash_xip_suspend_cmd(xip_t *xip, uint32_t cmd)
{
	raw_writel(xip->base + REG_FLASH_XIP_XSSCR_OFFSET, cmd);
}

static inline void flash_xip_read_flag_cmd(xip_t *xip, uint32_t cmd)
{
	raw_writel(xip->base + REG_FLASH_XIP_XRDFSRCR_OFFSET, cmd);
}


static inline void flash_xip_cpu_ins_base(xip_t *xip, uint32_t addr)
{
	raw_writel(xip->base + REG_FLASH_XIP_XISOR_OFFSET, addr);
}

static inline void flash_xip_data_base(xip_t *xip, uint32_t addr)
{
	raw_writel(xip->base + REG_FLASH_XIP_XDSOR_OFFSET, addr);
}

static inline int32_t flash_xip_aes_config(xip_t *xip, aes_cfg_t *cfg)
{
	int32_t result = 0;
	uint32_t value = 0;
	if ((NULL == xip) || (NULL == cfg)) {
		result = -1;
	} else {
		value = cfg->path_sel & 0x1;
		if (0 == cfg->data_path_bypass) {
			value |= (1 << 1);
		}
		value |= (cfg->tweak_mode & 0x1) << 2;
		value |= (cfg->tweak_mask >> 3) << 3;
		value = 4;
		raw_writel(xip->base + REG_FLASH_XIP_AMR_OFFSET, value);

		for (value = 0; value < 4; value++) {
			raw_writel(xip->base + REG_FLASH_XIP_ATWR_OFFSET(value), cfg->tweak[value]);
		}

		raw_writel(xip->base + REG_FLASH_XIP_ARSR_OFFSET, cfg->seed);
	}
	return result;
}

static inline void flash_xip_aes_start(xip_t *xip, \
		uint32_t in_valid, uint32_t update, uint32_t stream)
{
	raw_writel(xip->base + REG_FLASH_XIP_AWER_OFFSET, update);
	raw_writel(xip->base + REG_FLASH_XIP_ANSR_OFFSET, stream);
	raw_writel(xip->base + REG_FLASH_XIP_AIDVR_OFFSET, in_valid);
}

static inline void flash_xip_aes_input(xip_t *xip, uint32_t *data, uint32_t nof_data)
{
	uint32_t cnt;
	if (nof_data >= 16) {
		nof_data = 16;
	}
	for (cnt = 0; cnt < nof_data; cnt++) {
		raw_writel(xip->base + REG_FLASH_XIP_AIDR_OFFSET(cnt), *data++);
	}
}

static inline int32_t falsh_xip_aes_ouput(xip_t *xip, uint32_t *data, uint32_t nof_data)
{
	int32_t cnt, result = 0;
	if (raw_readl(xip->base + REG_FLASH_XIP_ADSR_OFFSET) & 0x1) {
		if (nof_data >= 16) {
			nof_data = 16;
		}
		for (cnt = 0; cnt < nof_data; cnt++) {
			*data++ = raw_readl(xip->base + REG_FLASH_XIP_AODR_OFFSET(cnt));
		}
	} else {
		result = -1;
	}
	return result;
}

static inline uint32_t flash_xip_aes_done(xip_t *xip)
{
	return raw_readl(xip->base + REG_FLASH_XIP_ADSR_OFFSET);
}

static inline void flash_xip_aes_mode_set(xip_t *xip, uint8_t mode)
{
	raw_writel(xip->base + REG_FLASH_XIP_AMR_OFFSET, mode);
}

static inline void flash_xip_data_endian(xip_t *xip, \
		uint32_t wr_halfw_swap, uint32_t wr_byte_swap,\
		uint32_t rd_halfw_swap, uint32_t rd_byte_swap)
{
	uint32_t value = 0;
	if (rd_byte_swap) {
		value |= 1;
	}
	if (rd_halfw_swap) {
		value |= (1 << 1);
	}

	if (wr_halfw_swap) {
		value |= (1 << 8);
	}
	if (wr_byte_swap) {
		value |= (1 << 9);
	}
	raw_writel(xip->base + REG_FLASH_XIP_XABECR_OFFSET, value);
}
static inline void flash_xip_ins_endian(xip_t *xip, \
		uint32_t w_swap, uint32_t byte_swap, uint32_t bit_swap)
{
	uint32_t value = 0;
	if (w_swap) {
		value |= (1 << 2);
	}
	if (byte_swap) {
		value |= (1 << 1);
	}
	if (bit_swap) {
		value |= (1 << 0);
	}
	raw_writel(xip->base + REG_FLASH_XIP_XADBECR_OFFSET, value);
}

static inline void flash_xip_ins_buffer_ctrl(xip_t *xip, flash_xip_buf_lvl_t buf_lvl)
{
	raw_writel(xip->base + REG_FLASH_XIP_XIBCR_OFFSET, buf_lvl);
}
static inline void flash_xip_data_rdbuf_ctrl(xip_t *xip, flash_xip_buf_lvl_t buf_lvl)
{
	raw_writel(xip->base + REG_FLASH_XIP_XRBCR_OFFSET, buf_lvl);
}
static inline void flash_xip_data_wrbuf_ctrl(xip_t *xip, flash_xip_buf_lvl_t buf_lvl)
{
	raw_writel(xip->base + REG_FLASH_XIP_XWBCR_OFFSET, buf_lvl);
}

static inline void flash_xip_wip_config(xip_t *xip, uint32_t wip)
{
	raw_writel(xip->base + REG_FLASH_XIP_XWIPCR_OFFSET, wip);
}

static inline void flash_xip_xsb_config(xip_t *xip, uint32_t xsb, uint32_t latency)
{
	raw_writel(xip->base + REG_FLASH_XIP_XXBCR_OFFSET, xsb);
	raw_writel(xip->base + REG_FLASH_XIP_XXLCR_OFFSET, latency);
}

static inline void flash_xip_erase_en(xip_t *xip)
{
	raw_writel(xip->base + REG_FLASH_XIP_XEFBR_OFFSET, 1);
}
static inline uint32_t flash_xip_erase_in_progress(xip_t *xip)
{
	return raw_readl(xip->base + REG_FLASH_XIP_XEFBR_OFFSET);
}

static inline void flash_xip_program_en(xip_t *xip)
{
	raw_writel(xip->base + REG_FLASH_XIP_XPFBR_OFFSET, 1);
}
static inline uint32_t flash_xip_program_in_progress(xip_t *xip)
{
	return raw_readl(xip->base + REG_FLASH_XIP_XPFBR_OFFSET);
}
#endif
