#ifndef _DW_I2C_H_
#define _DW_I2C_H_

#define DATA_COMMAND(cmd , stop, restart, data)	( \
		(((cmd) & 0x1) << 8) | \
		(((stop) & 0x1) << 9) | \
		(((restart) & 0x1) << 10) | \
		((data) & 0xFF) )

#define I2C_INT_GEN_CALL			(1 << 11)
#define I2C_INT_START_DET			(1 << 10)
#define I2C_INT_STOP_DET			(1 << 9)
#define I2C_INT_ACTIVITY			(1 << 8)
#define I2C_INT_TX_ABRT				(1 << 6)
#define I2C_INT_TX_EMPTY			(1 << 4)
#define I2C_INT_TX_OVERFLW			(1 << 3)
#define I2C_INT_RX_FULL				(1 << 2)
#define I2C_INT_RX_OVERFLW			(1 << 1)
#define I2C_INT_RX_UNDERFLOW			(1 << 0)

typedef enum {
	DW_I2C_STS_ACT = 0,
	DW_I2C_STS_TFNF,
	DW_I2C_STS_TFE,
	DW_I2C_STS_RFNE,
	DW_I2C_STS_RFF,
	DW_I2C_STS_M_ACT,
	DW_I2C_STS_S_ACT,
	DW_I2C_STS_INVALID
} dw_i2c_status_t;
#define DW_I2C_ACTIVITY(status)		((status) & (1 << DW_I2C_STS_ACT))
#define DW_I2C_TXFIFO_N_FULL(status)	((status) & (1 << DW_I2C_STS_TFNF))
#define DW_I2C_TXFIFO_EMPTY(status)	((status) & (1 << DW_I2C_STS_TFE))
#define DW_I2C_RXFIFO_N_EMPTY(status)	((status) & (1 << DW_I2C_STS_RFNE))
#define DW_I2C_RXFIFO_FULL(status)	((status) & (1 << DW_I2C_STS_RFF))


typedef enum dw_i2c_int_type {
	DW_I2C_RX_UNDER = 0,
	DW_I2C_RX_OVER,
	DW_I2C_RX_FULL,
	DW_I2C_TX_OVER,
	DW_I2C_TX_EMPTY,
	DW_I2C_RESERVED,
	DW_I2C_TX_ABRT,
	DW_I2C_RX_DONE,
	DW_I2C_ACTIVITY,
	DW_I2C_STOP_DET,
	DW_I2C_START_DET,
	DW_I2C_GEN_CALL,
	DW_I2C_RESTART_DET,
	DW_I2C_M_ON_HOLD,
	DW_I2C_STUCK_AT_LOW,
	DW_I2C_INT_INVALID
} dw_i2c_int_type_t;
#define DW_I2C_INT_RX_UNDER(status)	(((status) & (1 << DW_I2C_RX_UNDER)) ? (1) : (0))
#define DW_I2C_INT_RX_OVER(status)	(((status) & (1 << DW_I2C_RX_OVER)) ? (1) : (0))

#define DW_I2C_INT_RX_UNDER_EN		(1 << DW_I2C_RX_UNDER)
#define DW_I2C_INT_RX_OVER_EN		(1 << DW_I2C_RX_OVER)

typedef enum {
	DW_I2C_ABRT_7ADDR_NOACK = 0,
	DW_I2C_ABRT_10ADDR1_NOACK,
	DW_I2C_ABRT_10ADDR2_NOACK,
	DW_I2C_ABRT_TXDATA_NOACK,
	DW_I2C_ABRT_GCALL_NOACK,
	DW_I2C_ABRT_GCALL_READ,
	DW_I2C_ABRT_HS_ACKDET,
	DW_I2C_ABRT_SBYTE_ACKDET,
	DW_I2C_ABRT_HS_NORSTRT,
	DW_I2C_ABRT_SBYTE_NORSTRT,
	DW_I2C_ABRT_10B_RD_NORSTRT,
	DW_I2C_ABRT_M_DIS,
	DW_I2C_ARB_LOST,
	DW_I2C_ABRT_USER_ABRT = 16,
	DW_I2C_ABRT_TX_FLUSHED_CNT = 23
} dw_i2c_tx_abort_src_t;
#define DW_I2C_TX_ABRT_7ADDR_NO_ACK(status)	((status) & (1 << DW_I2c_ABRT_7ADDR_NOACK))
#define DW_I2C_TX_ABRT_10ADDR_NO_ACK(status)	(\
		((status) & (1 << DW_I2c_ABRT_10ADDR1_NOACK)) || \
		((status) & (1 << DW_I2c_ABRT_10ADDR2_NOACK)))

#define DW_i2c_TX_ABRT_FLUSHED_CNT(status)	(((status) & 0x1FF) >> DW_I2C_ABRT_TX_FLUSHED_CNT)

typedef enum {
	I2C_SPPED_STANDARD_MODE = 0,
	I2C_SPEED_FAST_MODE,
	I2C_SPEED_HIGH_MODE
} dw_i2c_speed_mode_t;

typedef enum {
	I2C_7BIT_ADRRESS = 0,
	I2C_10BIT_ADDRESS = 1,
} dw_i2c_address_mode_t;

/* @mode: 0->slave, 1->master. */
typedef struct {
	uint8_t mode;
	dw_i2c_speed_mode_t speed;
	dw_i2c_address_mode_t addr_mode;
	uint8_t restart_en;
	uint8_t tx_empty_inten;
} dw_i2c_control_t;
typedef struct {
	uint32_t base;
	uint32_t int_no;

	/* 0->slave, 1->master. */
	uint32_t mode;

	void *ops;
} dw_i2c_t;
int32_t dw_i2c_install_ops(dw_i2c_t *dev_i2c);

typedef struct {
	int32_t (*control)(dw_i2c_t *dev_i2c, dw_i2c_control_t *ctrl);
	int32_t (*target_address)(dw_i2c_t *dev_i2c, dw_i2c_address_mode_t addr_mode, uint32_t addr);
	int32_t (*master_code)(dw_i2c_t *dev_i2c, uint32_t code);
	int32_t (*write)(dw_i2c_t *dev_i2c, uint32_t cmd);
	int32_t (*read)(dw_i2c_t *dev_i2c, uint32_t *cmd);
	int32_t (*scl_count)(dw_i2c_t *dev_i2c, dw_i2c_speed_mode_t speed, uint32_t high, uint32_t low);
	int32_t (*int_status)(dw_i2c_t *dev_i2c, uint32_t *status);
	int32_t (*int_mask)(dw_i2c_t *dev_i2c, uint32_t bitmap, uint32_t flag);
	int32_t (*int_clear)(dw_i2c_t *dev_i2c, dw_i2c_int_type_t *type);
	int32_t (*fifo_threshold)(dw_i2c_t *dev_i2c, uint32_t *rx_thres, uint32_t *tx_thres);
	int32_t (*enable)(dw_i2c_t *dev_i2c, uint32_t tx_block, uint32_t abort, uint32_t enable);
	int32_t (*status)(dw_i2c_t *dev_i2c, uint32_t *status);
	int32_t (*fifo_data_count)(dw_i2c_t *dev_i2c, uint32_t *rxfifo_cnt, uint32_t *txfifo_cnt);
	int32_t (*sda_hold_time)(dw_i2c_t *dev_i2c, uint32_t rx_cycles, uint32_t tx_cycles);
	int32_t (*spike_suppression_limit)(dw_i2c_t *dev_i2c, dw_i2c_speed_mode_t speed, uint32_t limit);
} dw_i2c_ops_t;

#endif
