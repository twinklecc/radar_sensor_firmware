#ifndef _DW_SSI_H_
#define _DW_SSI_H_

#define SSI_INT_MULTI_MASTER_CONTENTION			(1 << 5)
#define SSI_INT_RX_FIFO_FULL				(1 << 4)
#define SSI_INT_RX_FIFO_OVERFLOW			(1 << 3)
#define SSI_INT_RX_FIFO_UNDERFLOW			(1 << 2)
#define SSI_INT_TX_FIFO_OVERFLOW			(1 << 1)
#define SSI_INT_TX_FIFO_EMPTY				(1 << 0)


typedef struct dw_ssi_control {
	uint32_t slv_sel_toggle_en;
	uint32_t frame_format;
	uint32_t data_frame_size;
	uint32_t ctrl_frame_size;
	uint32_t slv_output_en;
	uint32_t transfer_mode;
	uint32_t serial_clk_pol;
	uint32_t serial_clk_phase;
	uint32_t serial_protocol;
} dw_ssi_ctrl_t;
#define DW_SSI_CTRL_DEFAULT(ctrl) do { \
		(ctrl)->slv_sel_toggle_en = 0; \
		(ctrl)->frame_format = STANDARD_SPI_FRAME_FORMAT; \
		(ctrl)->data_frame_size = 32; \
		(ctrl)->ctrl_frame_size = 0; \
		(ctrl)->slv_output_en = 0; \
		(ctrl)->transfer_mode = DW_SSI_TR_MODE; \
		(ctrl)->serial_clk_pol = 0; \
		(ctrl)->serial_clk_phase = 0; \
		(ctrl)->serial_protocol = MOTOROLA_SPI;\
	} while(0)
#define DW_SSI_CTRL_STATIC_DEFAULT { \
		.slv_sel_toggle_en = 0, \
		.frame_format = STANDARD_SPI_FRAME_FORMAT, \
		.data_frame_size = 32, \
		.ctrl_frame_size = 0, \
		.slv_output_en = 0, \
		.transfer_mode = DW_SSI_TR_MODE, \
		.serial_clk_pol = 0, \
		.serial_clk_phase = 0, \
		.serial_protocol = MOTOROLA_SPI \
	}

/* not use. */
#if 0
typedef struct dw_ssi_microwire_control {
	uint32_t mw_handshaking;
	uint32_t mw_ctrl;
	uint32_t mw_transfer_mode;
} dw_ssi_mw_ctrl_t;
#endif

typedef struct dw_ssi_spi_control {
	uint32_t rd_strobe_en;
	uint32_t ins_ddr_en;
	uint32_t ddr_en;
	uint32_t wait_cycles;
	uint32_t ins_len;
	uint32_t addr_len;
	uint32_t transfer_type;
} dw_ssi_spi_ctrl_t;
#define DW_SSI_SPI_CTRL_DEFAULT(spi_ctrl) do { \
		(spi_ctrl)->rd_strobe_en = 0; \
		(spi_ctrl)->ins_ddr_en = 0; \
		(spi_ctrl)->ddr_en = 0; \
		(spi_ctrl)->wait_cycles = 0; \
		(spi_ctrl)->ins_len = DW_SSI_INS_LEN_8; \
		(spi_ctrl)->addr_len = DW_SSI_ADDR_LEN_24; \
		(spi_ctrl)->transfer_type = DW_SSI_1_1_X;\
	} while(0)
#define DW_SSI_SPI_CTRL_STATIC_DEFAULT { \
		.rd_strobe_en = 0, \
		.ins_ddr_en = 0, \
		.ddr_en = 0, \
		.wait_cycles = 0, \
		.ins_len = DW_SSI_INS_LEN_8, \
		.addr_len = DW_SSI_ADDR_LEN_24, \
		.transfer_type = DW_SSI_1_1_X \
	}

typedef enum dw_ssi_mode {
	DW_SSI_MASTER_MODE = 0,
	DW_SSI_SLAVER_MODE
} dw_ssi_mode_t;

typedef enum dw_ssi_transfer_mode {
	DW_SSI_TR_MODE = 0,
	DW_SSI_TRANSMIT_ONLY,
	DW_SSI_RECEIVE_ONLY,
	DW_SSI_EEPROM
} dw_ssi_transfer_mode_t;

typedef enum dw_ssi_frame_format {
	STANDARD_SPI_FRAME_FORMAT = 0,
	DUAL_FRAME_FORMAT,
	QUAD_FRAME_FORMAT,
	OCTAL_FRAME_FORMAT
} dw_ssi_frame_format_t;

typedef enum dw_ssi_instruct_length {
	DW_SSI_INS_LEN_0 = 0,
	DW_SSI_INS_LEN_4,
	DW_SSI_INS_LEN_8,
	DW_SSI_INS_LEN_16
} dw_ssi_ins_len_t;

typedef enum dw_ssi_address_length {
	DW_SSI_ADDR_LEN_0 = 0,
	DW_SSI_ADDR_LEN_4,
	DW_SSI_ADDR_LEN_8,
	DW_SSI_ADDR_LEN_12,
	DW_SSI_ADDR_LEN_16,
	DW_SSI_ADDR_LEN_20,
	DW_SSI_ADDR_LEN_24,
	DW_SSI_ADDR_LEN_28,
	DW_SSI_ADDR_LEN_32,
	DW_SSI_ADDR_LEN_36,
	DW_SSI_ADDR_LEN_40,
	DW_SSI_ADDR_LEN_44,
	DW_SSI_ADDR_LEN_48,
	DW_SSI_ADDR_LEN_52,
	DW_SSI_ADDR_LEN_56,
	DW_SSI_ADDR_LEN_60
} dw_ssi_addr_len_t;

typedef enum dw_ssi_data_length {
	DW_SSI_DATA_LEN_0 = 0,
	DW_SSI_DATA_LEN_4 = 3,
	DW_SSI_DATA_LEN_5,
	DW_SSI_DATA_LEN_6,
	DW_SSI_DATA_LEN_7,
	DW_SSI_DATA_LEN_8,
	DW_SSI_DATA_LEN_9,
	DW_SSI_DATA_LEN_10,
	DW_SSI_DATA_LEN_11,
	DW_SSI_DATA_LEN_12,
	DW_SSI_DATA_LEN_13,
	DW_SSI_DATA_LEN_14,
	DW_SSI_DATA_LEN_15,
	DW_SSI_DATA_LEN_16,
	DW_SSI_DATA_LEN_17,
	DW_SSI_DATA_LEN_18,
	DW_SSI_DATA_LEN_19,
	DW_SSI_DATA_LEN_20,
	DW_SSI_DATA_LEN_21,
	DW_SSI_DATA_LEN_22,
	DW_SSI_DATA_LEN_23,
	DW_SSI_DATA_LEN_24,
	DW_SSI_DATA_LEN_25,
	DW_SSI_DATA_LEN_26,
	DW_SSI_DATA_LEN_27,
	DW_SSI_DATA_LEN_28,
	DW_SSI_DATA_LEN_29,
	DW_SSI_DATA_LEN_30,
	DW_SSI_DATA_LEN_31,
	DW_SSI_DATA_LEN_32,
	DW_SSI_DATA_LEN_MAX = DW_SSI_DATA_LEN_32
} dw_ssi_data_len_t;

typedef enum dw_ssi_tranfer_type {
	DW_SSI_1_1_X = 0,
	DW_SSI_1_X_X,
	DW_SSI_X_X_X
} dw_ssi_transfer_type_t;

typedef struct dw_ssi_transfer_info {
	uint8_t ins;
	uint8_t rd_wait_cycle;
	uint8_t addr_valid;
	uint32_t addr;

	void *buf;
	uint32_t len;
} dw_ssi_xfer_t;
#define DW_SSI_XFER_INIT(instruct, address, wait_cycle, addr_flag) {\
		.ins = (instruct),\
		.rd_wait_cycle = (wait_cycle),\
		.addr_valid = (addr_flag),\
		.addr = (address),\
		.buf = NULL,\
		.len = 0\
	}

typedef struct dw_ssi_descriptor {
	uint32_t base;
	uint32_t mode;

	uint8_t int_rx;
	uint8_t int_tx;
	uint8_t int_err;

	uint8_t rx_dma_req;
	uint8_t tx_dma_req;

	uint32_t ref_clock;

	void *ops;
} dw_ssi_t;

typedef struct dw_ssi_operations {
	int32_t (*control)(dw_ssi_t *dw_ssi, dw_ssi_ctrl_t *ctrl);

	int32_t (*enable)(dw_ssi_t *dw_ssi, uint32_t enable);

	//int32_t (*slave_enable)(dw_ssi_t *dw_ssi, uint32_t sel_mask, uint32_t enable);

	int32_t (*fifo_entry)(dw_ssi_t *dw_ssi, uint32_t *entry);
	int32_t (*fifo_threshold)(dw_ssi_t *dw_ssi, uint32_t rx_thres, uint32_t tx_thres);

	int32_t (*baud)(dw_ssi_t *dw_ssi, uint32_t baud);
	//int32_t (*rx_data_frame_number)(dw_ssi_t *dw_ssi, uint32_t number);
	int32_t (*rx_sample_delay)(dw_ssi_t *dw_ssi, uint32_t delay);

	int32_t (*xfer)(dw_ssi_t *dw_ssi, uint32_t *tx_buf, uint32_t *rx_buf, uint32_t len);

	/* operate external device, such as, nor flash. */
	int32_t (*read)(dw_ssi_t *dw_ssi, dw_ssi_xfer_t *xfer, uint32_t flag);
	int32_t (*write)(dw_ssi_t *dw_ssi, dw_ssi_xfer_t *xfer, uint32_t flag);

	/* @rx_or_tx: 0->rx, 1->tx. */
	int32_t (*dma_config)(dw_ssi_t *dw_ssi, uint32_t rx_or_tx, uint32_t en, uint32_t threshold);

	int32_t (*int_enable)(dw_ssi_t *dw_ssi, uint32_t mask, uint32_t enable);
	int32_t (*int_clear)(dw_ssi_t *dw_ssi, uint32_t status);

	int32_t (*int_all_status)(dw_ssi_t *dw_ssi);
	int32_t (*int_status)(dw_ssi_t *dw_ssi, uint32_t status);
	//int32_t (*int_raw_status)(dw_ssi_t *dw_ssi, uint32_t status);
	int32_t (*status)(dw_ssi_t *dw_ssi, uint32_t status);

	int32_t (*fifo_data_count)(dw_ssi_t *dw_ssi, uint32_t rx_or_tx);

	int32_t (*spi_control)(dw_ssi_t *dw_ssi, dw_ssi_spi_ctrl_t *spi_ctrl);

	int32_t (*version)(dw_ssi_t *dw_ssi);
} dw_ssi_ops_t;

/*
typedef enum dw_ssi_clock_mode {
	SSI_CPOL_0_CPHA_0 = 0,
	SSI_CPOL_0_CPHA_1 = 1,
	SSI_CPOL_1_CPHA_0 = 2,
	SSI_CPOL_1_CPHA_1 = 3,
	SSI_CLK_MODE_0 = SSI_CPOL_0_CPHA_0,
	SSI_CLK_MODE_1 = SSI_CPOL_0_CPHA_1,
	SSI_CLK_MODE_2 = SSI_CPOL_1_CPHA_0,
	SSI_CLK_MODE_3 = SSI_CPOL_1_CPHA_1
} dw_ssi_clock_mode_t;
*/

typedef enum dw_ssi_status_type {
	SSI_STS_DATA_COLLISION_ERROR = 0,
	SSI_STS_TRANSMISSION_ERROR,
	SSI_STS_RX_FIFO_FULL,
	SSI_STS_RX_FIFO_NOT_EMPTY,
	SSI_STS_TX_FIFO_EMPTY,
	SSI_STS_TX_FIFO_NOT_FULL,
	SSI_STS_BUSY,
	SSI_STS_ALL
} dw_ssi_sts_t;

typedef enum dw_ssi_interrupt_status_type {
	SSI_INT_STS_TX_FIFO_EMPTY = 0,
	SSI_INT_STS_TX_FIFO_OVERFLOW,
	SSI_INT_STS_RX_FIFO_UNDERFLOW,
	SSI_INT_STS_RX_FIFO_OVERFLOW,
	SSI_INT_STS_RX_FIFO_FULL,
	SSI_INT_STS_MULTI_MASTER_CONTENTION,
	SSI_INT_STS_ALL
} dw_ssi_int_sts_t;

typedef enum dw_ssi_fifo_type {
	DW_SSI_RX_FIFO = 0,
	DW_SSI_TX_FIFO
} dw_ssi_fifo_type_t;

typedef enum dw_ssi_serial_protocol {
	MOTOROLA_SPI = 0,
	TEXAS_SSP,
	NS_MICROWIRE
} dw_ss_serial_prot_t;

int32_t dw_ssi_install_ops(dw_ssi_t *dw_ssi);
#endif
