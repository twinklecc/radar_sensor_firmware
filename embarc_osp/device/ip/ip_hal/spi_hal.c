#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "arc_exception.h"

#include "dev_common.h"

#include "dw_ssi.h"
#include "spi_hal.h"
#include "dma_hal.h"

#define SPI_ISR(_func_name_) static void _func_name_(void *params)

typedef enum {
	SPI_XFER_IDLE = 0,
	SPI_XFER_LOCK,
	SPI_XFER_INITED,
	SPI_XFER_BUSY
} spi_xfer_state_t;

typedef enum {
	SPI_XFER_POLLING = 0,
	SPI_XFER_INT,
	SPI_XFER_DMA
} spi_xfer_mode_t;


typedef struct {
	uint32_t xfer_err_cnt;
	uint32_t xfer_cnt;
	uint32_t xfer_unexcepted;
} spi_xfer_statics_t;

typedef struct {
	uint8_t state;

	/* @xfer_mode. */
	spi_xfer_mode_t rx_mode;
	spi_xfer_mode_t tx_mode;

#if 0
	/* static configuration. */
	uint8_t transfer_mode;
	uint8_t slv_sel_toggle_en;
	uint8_t serial_protocol;

	uint8_t rd_strobe_en;
	uint8_t wait_cycles;
	uint8_t ins_len;
	uint8_t transfer_type;
#endif
	uint32_t baud;
	spi_xfer_desc_t desc;

	dw_ssi_t *dev;
	DEV_BUFFER *txbuf;
	DEV_BUFFER *rxbuf;

	/* callback for interrupt or DMA? */
	void (*callback)(void *);

	spi_xfer_statics_t statics;
} spi_xfer_t;

/*
 * @inited_flag, bitmap to id.
 * @
 * */
typedef struct {
	uint8_t inited_flag;

	uint8_t nof_dev;
	spi_xfer_t *xfer_info;
} spi_driver_t;
static spi_driver_t spi_drv;

static spi_xfer_desc_t xfer_desc_default = {
	.clock_mode = SPI_CLK_MODE_0,
	.dfs = 32,
	.cfs = 0,
	.spi_frf = SPI_FRF_STANDARD,

	.rx_thres = 31,
	.tx_thres = 0,
};

static void spi_isr(uint32_t id, void *params);
SPI_ISR(spi0_isr) { spi_isr(0, NULL); }
SPI_ISR(spi1_isr) { spi_isr(1, NULL); }
SPI_ISR(spi2_isr) { spi_isr(2, NULL); }

static inline int32_t spi_int_register(uint32_t id, dw_ssi_t *spi_dev)
{
	int32_t result = E_OK;

	uint32_t err_irq, rx_irq, tx_irq;

	void (*isr)(void *);

	tx_irq = spi_dev->int_tx;
	rx_irq = spi_dev->int_rx;
	err_irq = spi_dev->int_err;

	if (0 == id) {
		isr = spi0_isr;
	} else if (1 == id) {
		isr = spi1_isr;
	} else if (2 == id) {
		isr = spi2_isr;
	} else {
		isr = NULL;
	}

	do {
		result = int_handler_install(tx_irq, isr);
		if (E_OK != result) {
			break;
		}

		result = int_handler_install(rx_irq, isr);
		if (E_OK != result) {
			break;
		}

		result = int_handler_install(err_irq, isr);
		if (E_OK != result) {
			break;
		}
	} while (0);

	return result;
}

static int32_t spi_init(void)
{
	int32_t result = E_OK;

	uint32_t info_size = 0;

	spi_drv.nof_dev = DW_SPI_NUM;

	info_size = DW_SPI_NUM * sizeof(spi_xfer_t);
	spi_drv.xfer_info = (spi_xfer_t *)pvPortMalloc(info_size);
	if (NULL == spi_drv.xfer_info) {
		result = E_NORES;
	} else {
		spi_drv.inited_flag = 1;
	}

	return result;
}

int32_t spi_open(uint32_t id, uint32_t baud)
{
	int32_t result = E_OK;

	spi_xfer_t *xfer = NULL;
	dw_ssi_t *spi_dev = NULL;
	dw_ssi_ops_t *spi_ops = NULL;

	uint32_t cpu_sts = arc_lock_save();
	if (0 == spi_drv.inited_flag) {
		result = spi_init();
	}

	do {
		/* spi_init failed. */
		if (E_OK != result) {
			break;
		}

		if ((id >= spi_drv.nof_dev) || (0 == baud)) {
			result = E_PAR;
			break;
		}

		xfer = &(spi_drv.xfer_info[id]);
		if (SPI_XFER_IDLE != xfer->state) {
			result = E_DBUSY;
			break;
		} else {
			spi_enable(id, 1);
		}

		spi_dev = (dw_ssi_t *)dw_ssi_get_dev(id);
		if ((NULL == spi_dev) || (NULL == spi_dev->ops)) {
			result = E_NOEXS;
			break;
		} else {
			xfer->dev = spi_dev;
		}

		spi_ops = (dw_ssi_ops_t *)spi_dev->ops;
		if (DEV_MASTER_MODE == spi_dev->mode) {
			result = spi_ops->baud(spi_dev, baud);
			if (E_OK != result) {
				break;
			}
		}

		/* TODO: register interrupt isr. */
		result  = spi_int_register(id, spi_dev);
		if (E_OK != result) {
			break;
		}

		result = spi_ops->int_enable(spi_dev, 0x1f, 0);
		if (E_OK != result) {
			break;
		}

		result = spi_ops->enable(spi_dev, 1);
		if (E_OK != result) {
			break;
		}

		xfer->rx_mode = SPI_XFER_POLLING;
		xfer->tx_mode = SPI_XFER_POLLING;
		xfer->baud = baud;

		xfer->state = SPI_XFER_LOCK;

	} while (0);
	arc_unlock_restore(cpu_sts);

	return result;
}

int32_t spi_device_mode(uint32_t id)
{
	int32_t result = E_OK;

	spi_xfer_t *xfer = NULL;
	dw_ssi_t *spi_dev = NULL;

	if ((id >= spi_drv.nof_dev)) {
		result = E_PAR;
	} else {
		xfer = &(spi_drv.xfer_info[id]);
		spi_dev = xfer->dev;
		if ((NULL == spi_dev)) {
			result = E_SYS;
		} else {
			result = (int32_t)spi_dev->mode;
		}
	}

	return result;
}

int32_t spi_fifo_threshold_update(uint32_t id, uint32_t rx, uint32_t tx)
{
	int32_t result = E_OK;


	spi_xfer_t *xfer = &(spi_drv.xfer_info[id]);
	dw_ssi_t *spi_dev = xfer->dev;

	if ((id < spi_drv.nof_dev) && (NULL != spi_dev) && (NULL != spi_dev->ops)) {
		if (SPI_XFER_IDLE != xfer->state) {
			result = E_DBUSY;
		} else {
			dw_ssi_ops_t *spi_ops = (dw_ssi_ops_t *)spi_dev->ops;
			result = spi_ops->fifo_threshold(spi_dev, rx, tx);
		}
	} else {
		result = E_PAR;
	}
	return result;
}

int32_t spi_transfer_config(uint32_t id, spi_xfer_desc_t *desc)
{
	int32_t result = E_OK;

	spi_xfer_t *xfer = NULL;
	dw_ssi_t *spi_dev = NULL;
	dw_ssi_ops_t *spi_ops = NULL;

	dw_ssi_ctrl_t ctrl = DW_SSI_CTRL_STATIC_DEFAULT;
	dw_ssi_spi_ctrl_t spi_ctrl = DW_SSI_SPI_CTRL_STATIC_DEFAULT;

	uint32_t cpu_sts = arc_lock_save();
	do {
		if ((id >= spi_drv.nof_dev) || (NULL == desc)) {
			result = E_PAR;
			break;
		}

		xfer = &(spi_drv.xfer_info[id]);
		if ((SPI_XFER_IDLE == xfer->state) || \
		    (SPI_XFER_BUSY == xfer->state)) {
			result = E_DBUSY;
			break;
		}

		spi_dev = xfer->dev;
		if ((NULL == spi_dev) || (NULL == spi_dev->ops)) {
			result = E_SYS;
			break;
		} else {
			spi_ops = (dw_ssi_ops_t *)spi_dev->ops;
		}

		if (NULL == desc) {
			desc = &xfer_desc_default;
		}
		ctrl.serial_clk_pol = (desc->clock_mode >> 1) & 0x1;
		ctrl.serial_clk_phase = (desc->clock_mode & 0x1);
		ctrl.data_frame_size = desc->dfs;
		ctrl.ctrl_frame_size = desc->cfs;
		ctrl.frame_format = desc->spi_frf;

		result = spi_ops->fifo_threshold(spi_dev, desc->rx_thres, desc->tx_thres);
		if (E_OK != result) {
			break;
		}

		result = spi_ops->enable(spi_dev, 0);
		if (E_OK != result) {
			break;
		}

		result = spi_ops->control(spi_dev, &ctrl);
		if (E_OK != result) {
			break;
		}

		SPI_XFER_DESC_COPY(&(xfer->desc), desc);

		if (ctrl.frame_format > SPI_FRF_STANDARD) {
			spi_ctrl.ins_len = DW_SSI_INS_LEN_8;
		}
		result = spi_ops->spi_control(spi_dev, &spi_ctrl);
		if (E_OK != result) {
			break;
		} else {
			xfer->state = SPI_XFER_INITED;
		}

		if (DEV_MASTER_MODE == spi_dev->mode) {
			result = spi_ops->rx_sample_delay(spi_dev,
					desc->rx_sample_delay);
			if (E_OK != result) {
				break;
			}
		}

		result = spi_ops->enable(spi_dev, 1);
		if (E_OK != result) {
			break;
		}

	} while (0);
	arc_unlock_restore(cpu_sts);

	return result;
}

static int32_t spi_raw_xfer(dw_ssi_t *dw_spi, dw_ssi_ops_t *ssi_ops,\
		DEV_BUFFER *rx_buf, DEV_BUFFER *tx_buf);
int32_t spi_xfer(uint32_t id, uint32_t *txdata, uint32_t *rxdata, uint32_t len)
{
	int32_t result = E_OK;

	uint32_t cpu_sts;
	DEV_BUFFER rx_buf;
	DEV_BUFFER tx_buf;

	spi_xfer_t *xfer = NULL;
	dw_ssi_t *spi_dev = NULL;
	dw_ssi_ops_t *spi_ops = NULL;

	do {
		if ((id >= spi_drv.nof_dev) || (0 == len) || \
			((NULL == txdata) && (NULL == rxdata))) {
			result = E_PAR;
			break;
		}

		cpu_sts = arc_lock_save();
		xfer = &(spi_drv.xfer_info[id]);
		if (SPI_XFER_BUSY == xfer->state) {
			result = E_DBUSY;
			break;
		} else {
			xfer->state = SPI_XFER_BUSY;
		}
		arc_unlock_restore(cpu_sts);

		spi_dev = xfer->dev;
		spi_ops = spi_dev->ops;
		if ((NULL == spi_dev) || (NULL == spi_ops)) {
			result = E_SYS;
			break;
		}

		if (txdata) {
			DEV_BUFFER_INIT(&tx_buf, txdata, len);
			xfer->txbuf = &tx_buf;
		} else {
			xfer->txbuf = NULL;
		}
		if (rxdata) {
			DEV_BUFFER_INIT(&rx_buf, rxdata, len);
			xfer->rxbuf = &rx_buf;
		} else {
			xfer->rxbuf = NULL;
		}

		while (len) {
			cpu_sts = arc_lock_save();
			result = spi_raw_xfer(spi_dev, spi_ops, \
					xfer->rxbuf, xfer->txbuf);
			arc_unlock_restore(cpu_sts);
			if (result <= 0) {
				break;
			} else {
				len -= result;
				result = E_OK;
			}
		}

		/*
		if (result > 0) {
			xfer->state = SPI_XFER_IDLE;
		}
		*/
		xfer->rxbuf = NULL;
		xfer->txbuf = NULL;
		xfer->state = SPI_XFER_IDLE;

	} while (0);

	return result;
}

int32_t spi_write(uint32_t id, uint32_t *data, uint32_t len)
{
	return spi_xfer(id, data, NULL, len);
}

int32_t spi_read(uint32_t id, uint32_t *data, uint32_t len)
{
	return spi_xfer(id, NULL, data, len);
}

int32_t spi_dma_write(uint32_t id, uint32_t *data, uint32_t len, void (*func)(void *))
{
	int32_t result = E_OK;

	uint32_t cpu_sts;

	spi_xfer_t *xfer = NULL;
	dw_ssi_t *spi_dev = NULL;
	dw_ssi_ops_t *spi_ops = NULL;

	dma_trans_desc_t desc;

	do {
		if ((id >= spi_drv.nof_dev) || (0 == len) || (NULL == data)) {
			result = E_PAR;
			break;
		}

		cpu_sts = arc_lock_save();
		xfer = &(spi_drv.xfer_info[id]);
		if (SPI_XFER_INITED != xfer->state) {
			result = E_SYS;
			break;
		} else {
			/* after dma done, back to SPI_XFER_INIT. */
			xfer->state = SPI_XFER_BUSY;
		}
		arc_unlock_restore(cpu_sts);

		spi_dev = xfer->dev;
		spi_ops = spi_dev->ops;
		if ((NULL == spi_dev) || (NULL == spi_ops)) {
			result = E_SYS;
			break;
		}

		desc.transfer_type = MEM_TO_PERI;
		desc.priority = DMA_CHN_PRIOR_4;
		desc.block_transfer_size = len;

		desc.src_desc.burst_tran_len = BURST_LEN8;
		desc.src_desc.addr_mode = ADDRESS_INCREMENT;
		desc.src_desc.sts_upd_en = 0;
		desc.src_desc.hs = HS_SELECT_SW;
		desc.src_desc.addr = (uint32_t)data;

		desc.dst_desc.burst_tran_len = BURST_LEN8;
		desc.dst_desc.addr_mode = ADDRESS_FIXED;
		desc.dst_desc.hw_if = spi_dev->tx_dma_req;
		desc.dst_desc.sts_upd_en = 0;
		desc.dst_desc.hs = HS_SELECT_HW;

		result = spi_ops->fifo_entry(spi_dev, (&desc.dst_desc.addr));
		if (E_OK != result) {
			break;
		}

		result = spi_ops->dma_config(spi_dev, 1, 1, xfer->desc.tx_thres);
		if (E_OK != result) {
			break;
		}

		result = dma_transfer(&desc, func);

	} while (0);

	return result;
}

int32_t spi_dma_read(uint32_t id, uint32_t *data, uint32_t len, void (*func)(void *))
{
	int32_t result = E_OK;

	uint32_t cpu_sts;
	dma_trans_desc_t desc;

	spi_xfer_t *xfer = NULL;
	dw_ssi_t *spi_dev = NULL;
	dw_ssi_ops_t *spi_ops = NULL;

	do {
		if ((id >= spi_drv.nof_dev) || (0 == len) || (NULL == data)) {
			result = E_PAR;
			break;
		}

		cpu_sts = arc_lock_save();
		xfer = &(spi_drv.xfer_info[id]);
		if (SPI_XFER_INITED != xfer->state) {
			arc_unlock_restore(cpu_sts);
			result = E_SYS;
			break;
		} else {
			/* after dma done, back to SPI_XFER_INIT. */
			xfer->state = SPI_XFER_BUSY;
		}
		arc_unlock_restore(cpu_sts);

		spi_dev = xfer->dev;
		spi_ops = spi_dev->ops;
		if ((NULL == spi_dev) || (NULL == spi_ops)) {
			result = E_SYS;
			break;
		}

		desc.transfer_type = PERI_TO_MEM;
		desc.priority = DMA_CHN_PRIOR_4;
		desc.block_transfer_size = len;

		desc.src_desc.burst_tran_len = BURST_LEN1;
		desc.src_desc.addr_mode = ADDRESS_FIXED;
		desc.src_desc.hw_if = spi_dev->rx_dma_req;
		desc.src_desc.sts_upd_en = 0;
		desc.src_desc.hs = HS_SELECT_HW;
		//desc.src_desc.addr = (uint32_t)data;

		desc.dst_desc.burst_tran_len = BURST_LEN1;
		desc.dst_desc.addr_mode = ADDRESS_INCREMENT;
		desc.dst_desc.sts_upd_en = 0;
		desc.dst_desc.hs = HS_SELECT_SW;
		desc.dst_desc.addr = (uint32_t)data;

		result = spi_ops->fifo_entry(spi_dev, (&desc.src_desc.addr));
		if (E_OK != result) {
			break;
		}

		result = spi_ops->dma_config(spi_dev, 0, 1, xfer->desc.rx_thres);
		if (E_OK != result) {
			break;
		}

		result = dma_transfer(&desc, func);

	} while (0);

	return result;
}

int32_t spi_databuffer_install(uint32_t id, uint32_t rx_or_tx, DEV_BUFFER *devbuf)
{
	int32_t result = E_OK;

	uint32_t cpu_sts;

	if ((id >= spi_drv.nof_dev) || (NULL == devbuf)) {
		result = E_PAR;
	} else {
		spi_xfer_t *xfer = &(spi_drv.xfer_info[id]);

		cpu_sts = arc_lock_save();
		if (SPI_XFER_BUSY == xfer->state) {
			result = E_DBUSY;
		} else {
			if (rx_or_tx) {
				xfer->txbuf = devbuf;
			} else {
				xfer->rxbuf = devbuf;
			}
		}
		arc_unlock_restore(cpu_sts);
	}

	return result;
}

int32_t spi_databuffer_uninstall(uint32_t id, uint32_t rx_or_tx)
{
	int32_t result = E_OK;

	uint32_t cpu_sts;

	if ((id >= spi_drv.nof_dev)) {
		result = E_PAR;
	} else {
		spi_xfer_t *xfer = &(spi_drv.xfer_info[id]);

		cpu_sts = arc_lock_save();
		if (rx_or_tx) {
			xfer->txbuf = NULL;
		} else {
			xfer->rxbuf = NULL;
		}
		arc_unlock_restore(cpu_sts);
	}

	return result;
}

int32_t spi_interrupt_install(uint32_t id, uint32_t rx_or_tx, void (*func)(void *))
{
	int32_t result = E_OK;

	uint32_t cpu_sts;
	//uint32_t mask;
	uint32_t int_id;
	dw_ssi_t *spi_dev = NULL;
	dw_ssi_ops_t *spi_ops = NULL;

	do {
		if ((id >= spi_drv.nof_dev) || (NULL == func)) {
			result = E_PAR;
			break;
		}

		spi_xfer_t *xfer = &(spi_drv.xfer_info[id]);

		spi_dev = xfer->dev;
		spi_ops = spi_dev->ops;
		if ((NULL == spi_dev) || (NULL == spi_ops)) {
			result = E_SYS;
			break;
		}

		cpu_sts = arc_lock_save();
		if (SPI_XFER_BUSY == xfer->state) {
			result = E_DBUSY;
		} else {
			if (func) {
				xfer->callback = func;
			}
		}
		arc_unlock_restore(cpu_sts);
		if (E_OK != result) {
			break;
		}

		if (rx_or_tx) {
			//mask = SSI_INT_TX_FIFO_EMPTY;
			int_id = spi_dev->int_tx;
		} else {
			//mask = SSI_INT_RX_FIFO_FULL;
			int_id = spi_dev->int_rx;
		}
		/*
		result = spi_ops->int_enable(spi_dev, mask, 1);
		if (E_OK != result) {
			break;
		} else
		*/{
			dmu_irq_enable(int_id, 1);
		}

		result = int_enable(int_id);

	} while (0);

	return result;
}

int32_t spi_interrupt_enable(uint32_t id, uint32_t rx_or_tx, uint32_t enable)
{
	int32_t result = E_OK;

	uint32_t mask;

	if ((id >= spi_drv.nof_dev)) {
		result = E_PAR;
	} else {
		spi_xfer_t *xfer = &(spi_drv.xfer_info[id]);
		dw_ssi_t *spi_dev = xfer->dev;
		dw_ssi_ops_t *spi_ops = spi_dev->ops;
		if ((NULL == spi_dev) || (NULL == spi_ops)) {
			result = E_SYS;
		} else {
			if (rx_or_tx) {
				mask = SSI_INT_TX_FIFO_EMPTY;
			} else {
				mask = SSI_INT_RX_FIFO_FULL;
			}
			result = spi_ops->int_enable(spi_dev, mask, enable);
		}
	}

	return result;
}

int32_t spi_interrupt_uninstall(uint32_t id, uint32_t rx_or_tx)
{
	int32_t result = E_OK;

	//uint32_t cpu_sts;
	uint32_t mask, int_id;
	dw_ssi_t *spi_dev = NULL;
	dw_ssi_ops_t *spi_ops = NULL;

	do {
		if ((id >= spi_drv.nof_dev)) {
			result = E_PAR;
			break;
		}

		spi_xfer_t *xfer = &(spi_drv.xfer_info[id]);

		spi_dev = xfer->dev;
		spi_ops = spi_dev->ops;
		if ((NULL == spi_dev) || (NULL == spi_ops)) {
			result = E_SYS;
			break;
		}

		if (rx_or_tx) {
			mask = SSI_INT_TX_FIFO_EMPTY;
			int_id = spi_dev->int_tx;
		} else {
			mask = SSI_INT_RX_FIFO_FULL;
			int_id = spi_dev->int_rx;
		}
		result = spi_ops->int_enable(spi_dev, mask, 0);
		if (E_OK != result) {
			break;
		} else {
			dmu_irq_enable(int_id, 0);
		}

		result = int_disable(int_id);
		if (E_OK != result) {
			break;
		}
		/*
		cpu_sts = arc_lock_save();
		xfer->callback = NULL;
		arc_unlock_restore(cpu_sts);
		*/

	} while (0);

	return result;
}


static void spi_isr(uint32_t id, void *params)
{
	int32_t result = E_OK;

	//uint32_t mask = 0;
	uint32_t int_sts = 0;

	dw_ssi_t *spi_dev = NULL;
	dw_ssi_ops_t *spi_ops = NULL;
	spi_xfer_t *xfer = NULL;

	do {
		if ((id >= spi_drv.nof_dev)) {
			result = E_PAR;
			break;
		}

		xfer = &(spi_drv.xfer_info[id]);

		spi_dev = xfer->dev;
		spi_ops = spi_dev->ops;
		if ((NULL == spi_dev) || (NULL == spi_ops)) {
			result = E_SYS;
			break;
		}

		/* get the interrupt status. */
		result = spi_ops->int_all_status(spi_dev);
		if (result <= 0) {
			break;
		} else {
			int_sts = result;
		}

		/* error happened, record and report! */
		if ((int_sts & SSI_INT_RX_FIFO_OVERFLOW) || \
		    (int_sts & SSI_INT_RX_FIFO_UNDERFLOW) || \
		    (int_sts & SSI_INT_TX_FIFO_OVERFLOW)) {
			//xfer->statics.xfer_err_cnt++;
			/* TODO: record error! */
		}

		/* unexpected interrupt generated, record and report! */
		if (int_sts & SSI_INT_MULTI_MASTER_CONTENTION) {
			//xfer->statics.unexcepted++;
			/* TODO: record error! */
		}

		result = spi_raw_xfer(spi_dev, spi_ops, xfer->rxbuf, xfer->txbuf);
		if (result >= 0) {
			if (xfer->callback) {
				xfer->callback(params);
			}
			//xfer->statics.xfer_cnt++;
		} else if (E_PAR == result) {
			if (xfer->callback) {
				xfer->callback(params);
			}
		} else {
			/* TODO: record error! */
		}

#if 0
		result = spi_ops->int_clear(spi_dev, SSI_INT_STS_ALL);
		if (E_OK != result) {
			/* TODO: record error! */
		}
#endif

	} while (0);
}

static int32_t spi_raw_xfer(dw_ssi_t *dw_spi, dw_ssi_ops_t *ssi_ops,\
		DEV_BUFFER *rx_buf, DEV_BUFFER *tx_buf)
{
	int32_t result = E_OK;

	uint32_t xfer_cnt = 0;
	uint32_t *rxdata = NULL, *txdata = NULL;

	if ((NULL != rx_buf) && (NULL != tx_buf)) {
		/* only SPI-M duplex mode enter.
		 * SPI-S can't read and write at the same time. */
		rxdata = (uint32_t *)rx_buf->buf;
		txdata = (uint32_t *)tx_buf->buf;
		rxdata += rx_buf->ofs;
		txdata += tx_buf->ofs;
		xfer_cnt = rx_buf->len - rx_buf->ofs;
		if (xfer_cnt > tx_buf->len - tx_buf->ofs) {
			xfer_cnt = tx_buf->len - tx_buf->ofs;
		}

		result = ssi_ops->xfer(dw_spi, rxdata, txdata, xfer_cnt);
		if (result >= 0) {
			rx_buf->ofs += result;
			tx_buf->ofs += result;
		}
	} else if ((NULL != rx_buf) && (NULL == tx_buf)) {
		/* SPI-M read, SPI-S read. */
		rxdata = (uint32_t *)rx_buf->buf;
		rxdata += rx_buf->ofs;
		xfer_cnt = rx_buf->len - rx_buf->ofs;
		result = ssi_ops->xfer(dw_spi, rxdata, NULL, xfer_cnt);
		if (result >= 0) {
			rx_buf->ofs += result;
		}
	} else if ((NULL == rx_buf) && (NULL != tx_buf)) {
		/* SPI-M write, SPI-S write. */
		txdata = (uint32_t *)tx_buf->buf;
		txdata += tx_buf->ofs;
		xfer_cnt = tx_buf->len - tx_buf->ofs;
		result = ssi_ops->xfer(dw_spi, NULL, txdata, xfer_cnt);
		if (result >= 0) {
			tx_buf->ofs += result;
		}
	} else {
		/* warning! nothing to do! */
		//result = E_PAR;
	}

	return result;
}
