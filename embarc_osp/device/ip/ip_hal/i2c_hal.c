#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "dev_common.h"
#include "arc_exception.h"
#include "dw_i2c_reg.h"
#include "dw_i2c.h"
#include "i2c_hal.h"


#define I2C_RX_FIFO_DEFAULT_THRES	(0)
#define I2C_TX_FIFO_DEFAULT_THRES	(0)

#define i2c_cmd_write  (0)
#define i2c_cmd_read   (1)
#define stop_dis       (0)
#define stop_enable    (1)
#define restart_dis    (0)
#define restart_enable (1)

typedef struct {
	/* 0->idle, 1->busy. */
	uint8_t state;
	/* 1->write, 2->read. */
	i2c_xfer_type_t xfer_type;

	uint8_t addr_mode;
	uint32_t cur_slave_addr;
	uint32_t ext_addr;
	uint8_t ext_addr_len;

	uint32_t rx_thres;
	uint32_t tx_thres;

	DEV_BUFFER xfer_buffer;
	xfer_callback xfer_done;

	dw_i2c_control_t ctrl;
} i2c_runtime_t;

typedef struct {
	uint32_t inited;

	i2c_runtime_t runtime;

} i2c_global_t;

//int32_t cback = 0;
//int32_t *i2c_back = &cback;

static i2c_global_t i2c_drv;

static void i2c_isr(void *params);
static int32_t i2c_io_timing_config(dw_i2c_t *i2c_dev, uint32_t speed_mode, i2c_io_timing_t *timing);
static int32_t i2c_send_device_address(dw_i2c_t *i2c_dev, dw_i2c_ops_t *i2c_ops, i2c_runtime_t *runtime);

int32_t i2c_init(uint32_t id, i2c_params_t *params)
{
	int32_t result = E_OK;

	uint32_t enable = 0;
	dw_i2c_t *i2c_dev = NULL;
	dw_i2c_ops_t *i2c_ops = NULL;

	i2c_runtime_t *runtime = &i2c_drv.runtime;

	do {
		if (i2c_drv.inited) {
			EMBARC_PRINTF("please deinit firstly\r\n");
			break;
		}
		i2c_dev = (dw_i2c_t *)i2c_get_dev(id);
		if ((NULL == i2c_dev) || (NULL == i2c_dev->ops)) {
			result = E_NOEXS;
			break;
		}
		i2c_ops = (dw_i2c_ops_t *)i2c_dev->ops;
		i2c_enable(1);
		/* before init I2C, disable it. */
		result = i2c_ops->enable(i2c_dev, 0, 0, enable);
		if (E_OK != result) {
			break;
		}
		/* disable all interrupts.*/
		result = i2c_ops->int_mask(i2c_dev, 0xFFF, 0);
		if (E_OK != result) {
			break;
		}

		/* master code.*/
		result = i2c_ops->master_code(i2c_dev, I2C_MASTER_CODE);
		if (E_OK != result) {
			break;
		}

		if (NULL != params) {
			runtime->ctrl.mode = 1;
			runtime->ctrl.speed = params->speed_mode;
			runtime->ctrl.addr_mode = params->addr_mode;
			runtime->ctrl.restart_en = 0;
			runtime->ctrl.tx_empty_inten = 1;
			result = i2c_ops->control(i2c_dev, &runtime->ctrl);
			if (E_OK != result) {
				break;
			}
			runtime->addr_mode = params->addr_mode;

			if (params->timing) {
				result = i2c_io_timing_config(i2c_dev, runtime->ctrl.speed, params->timing);
				if (E_OK != result) {
					break;
				}
			}
		}
		runtime->rx_thres = I2C_RX_FIFO_DEFAULT_THRES;
		runtime->tx_thres = I2C_TX_FIFO_DEFAULT_THRES;
		result = i2c_ops->fifo_threshold(i2c_dev, &runtime->rx_thres, &runtime->tx_thres);
		if (E_OK != result) {
			break;
		}

		result = int_handler_install(i2c_dev->int_no, i2c_isr);
		if (E_OK != result) {
			EMBARC_PRINTF("interrupt install fail\r\n");
		} else {
			int_enable(i2c_dev->int_no);
		}

		enable = 1;
		result = i2c_ops->enable(i2c_dev, 0, 0, enable);

		dmu_irq_enable(i2c_dev->int_no, 1);
	} while (0);

	return result;
}

int32_t i2c_fifo_threshold_config(i2c_xfer_type_t xfer_type, uint32_t thres)
{
	int32_t result = E_OK;

	i2c_runtime_t *runtime = &i2c_drv.runtime;

	dw_i2c_ops_t *i2c_ops = NULL;
	dw_i2c_t *i2c_dev = (dw_i2c_t *)i2c_get_dev(0);

	if ((NULL != i2c_dev) && (NULL != i2c_dev->ops)) {
		uint32_t cpu_sts = arc_lock_save();
		i2c_ops = (dw_i2c_ops_t *)i2c_dev->ops;
		if (i2c_drv.runtime.state) {
			result = E_DBUSY;
		} else {
			if (2 == xfer_type) {
				runtime->rx_thres = thres;
			} else if (1 == xfer_type) {
				runtime->tx_thres = thres;
			} else {
				result = E_PAR;
			}
			result = i2c_ops->fifo_threshold(i2c_dev, \
					&runtime->rx_thres, \
					&runtime->tx_thres);
		}
		arc_unlock_restore(cpu_sts);

	} else {
		result = E_PAR;
	}
	return result;
}

int32_t i2c_transfer_config(uint32_t addr_mode, uint32_t slave_addr, uint32_t ext_addr, uint32_t ext_addr_len)
{
	int32_t result = E_OK;

	dw_i2c_t *i2c_dev = NULL;
	dw_i2c_ops_t *i2c_ops = NULL;

	i2c_runtime_t *runtime = &i2c_drv.runtime;

	uint32_t cpu_sts = arc_lock_save();
	if (runtime->state) {
		result = E_DBUSY;
		arc_unlock_restore(cpu_sts);
		return result;
	} else {
		runtime->state = 1;
	}
	arc_unlock_restore(cpu_sts);

	i2c_dev = (dw_i2c_t *)i2c_get_dev(0);
	if ((NULL == i2c_dev) || (NULL == i2c_dev->ops)) {
		result = E_NOEXS;
	} else {
		i2c_ops = (dw_i2c_ops_t *)i2c_dev->ops;

		if (addr_mode != runtime->addr_mode) {
			uint32_t old_addr_mode = runtime->ctrl.addr_mode;
			runtime->ctrl.addr_mode = addr_mode;
			result = i2c_ops->control(i2c_dev, &runtime->ctrl);
			if (E_OK != result) {
				runtime->ctrl.addr_mode = old_addr_mode;
			}
		}

		if (E_OK == result) {
			runtime->cur_slave_addr = slave_addr;
			runtime->ext_addr = ext_addr;
			runtime->ext_addr_len = ext_addr_len;
		}

		runtime->state = 0;
	}

	return result;
}

int32_t i2c_write(uint8_t *data, uint32_t len)
{
	int32_t result = E_OK;

	uint32_t dev_status;

	dw_i2c_t *i2c_dev = NULL;

	dw_i2c_ops_t *i2c_ops = NULL;
	uint32_t idx;
	uint32_t data_cmd = 0;
	uint32_t enable = 1;
	uint32_t disable = 0;
	i2c_runtime_t *runtime = &i2c_drv.runtime;

	do {
		if ((NULL == data) || (0 == len)) {
			result = E_PAR;
			break;
		}
		uint32_t cpu_sts = arc_lock_save();
		if (runtime->state) {
			result = E_DBUSY;
			arc_unlock_restore(cpu_sts);
			return result;
		}
		else {
			runtime->state = 1;
		}
		arc_unlock_restore(cpu_sts);

		i2c_dev = (dw_i2c_t *)i2c_get_dev(0);
		if ((NULL == i2c_dev) || (NULL == i2c_dev->ops)) {
			result = E_NOEXS;
			break;
		}
		i2c_ops = (dw_i2c_ops_t *)i2c_dev->ops;
		if (runtime->ctrl.addr_mode) {
			result = i2c_ops->enable(i2c_dev, 0, 0, enable);

			data_cmd = DATA_COMMAND(i2c_cmd_write, stop_dis, restart_dis, 0xF0);
			result = i2c_ops->write(i2c_dev, data_cmd);
		}

		result = i2c_ops->enable(i2c_dev, 0, 0, disable);

		/* send slave address. */
		result = i2c_ops->target_address(i2c_dev, runtime->ctrl.addr_mode, runtime->cur_slave_addr);
		if (E_OK != result) {
			break;
		}

		result = i2c_ops->enable(i2c_dev, 0, 0, enable);

		if (runtime->ext_addr_len) {
			result = i2c_send_device_address(i2c_dev, i2c_ops, runtime);
			if (E_OK != result) {
				break;
			}
		}
		/* send len - 1. */
		for (idx = 0; idx < len - 1; idx++) {
		result = i2c_ops->status(i2c_dev, &dev_status);
		if (E_OK != result) {
			break;
		}
		while (0 == DW_I2C_TXFIFO_N_FULL(dev_status)) {
		i2c_ops->status(i2c_dev, &dev_status);}
		data_cmd = DATA_COMMAND(i2c_cmd_write, stop_dis, restart_dis, data[idx]);
		result = i2c_ops->write(i2c_dev, data_cmd);
		if (E_OK != result) {
			break;
		   }
	    }
		/* send last byte. */
		result = i2c_ops->status(i2c_dev, &dev_status);
		if (E_OK != result) {
			break;
		}
		while (0 == DW_I2C_TXFIFO_N_FULL(dev_status)) {
			i2c_ops->status(i2c_dev, &dev_status);}
		data_cmd = DATA_COMMAND(i2c_cmd_write, stop_enable, restart_dis, data[len - 1]);
		result = i2c_ops->write(i2c_dev, data_cmd);
		if (E_OK != result) {
			break;
		}

	} while (0);


	if (E_DBUSY != result) {
		runtime->state = 0;
	}

	return result;
}

int32_t i2c_read(uint8_t *data, uint32_t len)
{
	int32_t result = E_OK;
	dw_i2c_t *i2c_dev = NULL;
	dw_i2c_ops_t *i2c_ops = NULL;
	uint32_t dev_status;
	uint32_t idx, data_cmd = 0;
	uint32_t enable = 1;
	uint32_t disable = 0;
	i2c_runtime_t *runtime = &i2c_drv.runtime;

	do {
		if ((NULL == data) || (0 == len)) {
			result = E_PAR;
			break;
		}

		uint32_t cpu_sts = arc_lock_save();
		if (runtime->state) {
			result = E_DBUSY;
			arc_unlock_restore(cpu_sts);
			return result;
		}
		else {
			runtime->state = 1;
		}
		arc_unlock_restore(cpu_sts);

		i2c_dev = (dw_i2c_t *)i2c_get_dev(0);
		if ((NULL == i2c_dev) || (NULL == i2c_dev->ops)) {
			result = E_NOEXS;
			break;
		}
		i2c_ops = (dw_i2c_ops_t *)i2c_dev->ops;
		if (runtime->ctrl.addr_mode) {
			result = i2c_ops->enable(i2c_dev, 0, 0, enable);

			data_cmd = DATA_COMMAND(i2c_cmd_write, stop_dis, restart_dis, 0xF0);
			result = i2c_ops->write(i2c_dev, data_cmd);
		}
		/* before write target_address,disable I2C */
		result = i2c_ops->enable(i2c_dev, 0, 0, disable);

		/* send slave address. */
		result = i2c_ops->target_address(i2c_dev, runtime->ctrl.addr_mode, runtime->cur_slave_addr);
		if (E_OK != result) {
			break;
		}

		result = i2c_ops->enable(i2c_dev, 0, 0, enable);

		if (runtime->ext_addr_len) {
			result = i2c_send_device_address(i2c_dev, i2c_ops, runtime);
			if (E_OK != result) {
				break;
			}
		}
		//read len-1 bytes
		for (idx = 0; idx < len - 1; idx++) {
		result = i2c_ops->status(i2c_dev, &dev_status);
		if (E_OK != result) {
			break;
		}
		/*while (1 == DW_I2C_RXFIFO_FULL(dev_status)) {
			result = i2c_ops->read(i2c_dev, NULL);
			if (result >= 0) {
				data[idx] = result & 0xFF;
			}
			i2c_ops->status(i2c_dev, &dev_status);
		}*/
		while (0 == DW_I2C_TXFIFO_N_FULL(dev_status)) {
			i2c_ops->status(i2c_dev, &dev_status);
		}
		data_cmd = DATA_COMMAND(i2c_cmd_read, stop_dis, restart_dis, 0);
		result = i2c_ops->write(i2c_dev, data_cmd);
		if (E_OK != result) {
			break;
		}
		result = i2c_ops->status(i2c_dev, &dev_status);
		if (E_OK != result) {
			break;
		}
		while (0 == DW_I2C_RXFIFO_N_EMPTY(dev_status)) {
			i2c_ops->status(i2c_dev, &dev_status);
		}
		result = i2c_ops->read(i2c_dev, NULL);
		if (result >= 0) {
			data[idx] = result & 0xFF;
		} else {
			break;
		   }
		}
		//read last byte
		result = i2c_ops->status(i2c_dev, &dev_status);
		if (E_OK != result) {
			break;
		}
		/*while (1 == DW_I2C_RXFIFO_FULL(dev_status)) {
			result = i2c_ops->read(i2c_dev, NULL);
			if (result >= 0) {
				data[idx] = result & 0xFF;
			}
			i2c_ops->status(i2c_dev, &dev_status);
		}*/
		while (0 == DW_I2C_TXFIFO_N_FULL(dev_status)) {
			i2c_ops->status(i2c_dev, &dev_status);
		}
		data_cmd = DATA_COMMAND(i2c_cmd_read, stop_enable, restart_dis, 0);
		result = i2c_ops->write(i2c_dev, data_cmd);
		if (E_OK != result) {
			break;
		}
		result = i2c_ops->status(i2c_dev, &dev_status);
		if (E_OK != result) {
			break;
		}
		while (0 == DW_I2C_RXFIFO_N_EMPTY(dev_status)) {
			i2c_ops->status(i2c_dev, &dev_status);
		}
		result = i2c_ops->read(i2c_dev, NULL);
		if (result >= 0) {
			data[len-1] = result & 0xFF;
		} else {
			break;
		}

	} while (0);

	if (E_DBUSY != result) {
		runtime->state = 0;
	}

	return result;
}

int32_t i2c_interrupt_enable(void)
{
	int32_t result = E_OK;
	dw_i2c_t *i2c_dev = (dw_i2c_t *)i2c_get_dev(0);
	if ((NULL != i2c_dev) && (NULL != i2c_dev->ops)) {
		int_enable(i2c_dev->int_no);
	} else {
		result = E_PAR;
	}
	return result;
}

int32_t i2c_interrupt_disable(void)
{
	int32_t result = E_OK;
	dw_i2c_t *i2c_dev = (dw_i2c_t *)i2c_get_dev(0);
	if ((NULL != i2c_dev) && (NULL != i2c_dev->ops)) {
		int_disable(i2c_dev->int_no);
	} else {
		result = E_PAR;
	}
	return result;
}

int32_t i2c_transfer_start(i2c_xfer_type_t xfer_type, uint8_t *data, uint32_t len, xfer_callback func)
{
	int32_t result = E_OK;
	uint32_t enable = 1;
	uint32_t disable = 0;
	uint32_t data_cmd = 0;
	dw_i2c_ops_t *i2c_ops = NULL;
	dw_i2c_t *i2c_dev = (dw_i2c_t *)i2c_get_dev(0);
	i2c_runtime_t *runtime = &i2c_drv.runtime;
	i2c_drv.runtime.state = 0;
	DEV_BUFFER *dev_buf = &runtime->xfer_buffer;
	do {
		if ((NULL == data) || (0 == len)) {
			result = E_PAR;
			break;
		}

		if ((NULL == i2c_dev) || (NULL == i2c_dev->ops)) {
			break;
		}
		i2c_ops = (dw_i2c_ops_t *)i2c_dev->ops;

		uint32_t cpu_sts = arc_lock_save();
		if (i2c_drv.runtime.state) {
			result = E_DBUSY;
			arc_unlock_restore(cpu_sts);
			break;
		} else {
			i2c_drv.runtime.state = 1;
		}
		arc_unlock_restore(cpu_sts);

		runtime->xfer_type = xfer_type;

		DEV_BUFFER_INIT(dev_buf, data, len);
		if (func) {
			runtime->xfer_done = func;
		} else {
			runtime->xfer_done = NULL;
		}
		if (runtime->ctrl.addr_mode) {
			result = i2c_ops->enable(i2c_dev, 0, 0, enable);

			data_cmd = DATA_COMMAND(i2c_cmd_write, stop_dis, restart_dis, 0xF0);
			result = i2c_ops->write(i2c_dev, data_cmd);
		}
		/* before write target_address,disable I2C */
		result = i2c_ops->enable(i2c_dev, 0, 0, disable);

		/* send slave address.*/
		result = i2c_ops->target_address(i2c_dev, runtime->ctrl.addr_mode, \
				runtime->cur_slave_addr);
		if (E_OK != result) {
			break;
		}

		result = i2c_ops->enable(i2c_dev, 0, 0, enable);

		if (runtime->ext_addr_len) {
			result = i2c_send_device_address(i2c_dev, i2c_ops, runtime);
			if (E_OK != result) {
				break;
			}
		}
		/* enable interrupt signal.*/
		if (2 == xfer_type) {

			if (1 == len) {

				data_cmd = DATA_COMMAND(i2c_cmd_read, stop_enable, restart_dis, 0);

				result = i2c_ops->write(i2c_dev, data_cmd);

				result = i2c_ops->int_mask(i2c_dev, I2C_INT_RX_FULL, 1);

			}
			else {

				data_cmd = DATA_COMMAND(i2c_cmd_read, stop_dis, restart_dis, 0);

				result = i2c_ops->write(i2c_dev, data_cmd);

				result = i2c_ops->int_mask(i2c_dev, I2C_INT_RX_FULL, 1);

			}

		} else if (1 == xfer_type) {
			result = i2c_ops->int_mask(i2c_dev, I2C_INT_TX_EMPTY, 1);
		} else {
			result = E_PAR;
		}

	} while (0);

	return result;
}

/*************************************** local fuctions. ***************************************/
static int32_t i2c_send_device_address(dw_i2c_t *i2c_dev, dw_i2c_ops_t *i2c_ops, i2c_runtime_t *runtime)
{
	int32_t result = E_OK;

	uint8_t idx, data = 0;
	uint32_t data_cmd, ext_m_base = runtime->ext_addr;

	if ((NULL == i2c_dev) || (NULL == i2c_ops) || (NULL == runtime)) {
		result = E_PAR;
	} else {
		/* first of all, send external device memory address. */
		for (idx = 0; idx < runtime->ext_addr_len; idx++) {
			data = (ext_m_base >> ((runtime->ext_addr_len - idx - 1) << 3)) & 0xFF;
			data_cmd = DATA_COMMAND(i2c_cmd_write, stop_dis, restart_dis, data);
			result = i2c_ops->write(i2c_dev, data_cmd);
			if (E_OK != result) {
				break;
			}
		}
	}

	return result;
}

static int32_t i2c_io_timing_config(dw_i2c_t *i2c_dev, uint32_t speed_mode, i2c_io_timing_t *timing)
{
	int32_t result = E_OK;
	dw_i2c_ops_t *i2c_ops = (dw_i2c_ops_t *)i2c_dev->ops;
	result = i2c_ops->scl_count(i2c_dev, speed_mode, timing->scl_h_cnt, timing->scl_l_cnt);
	if (E_OK == result) {
		result = i2c_ops->sda_hold_time(i2c_dev, timing->sda_rx_hold, timing->sda_tx_hold);
		if (E_OK == result) {
			result = i2c_ops->spike_suppression_limit(i2c_dev, speed_mode, timing->spike_len);
		}
	}
	return result;
}

/******************************** ISR ***************************************/
static inline void i2c_error_isr(dw_i2c_t *i2c_dev, dw_i2c_ops_t *i2c_ops, uint32_t status)
{
	if (I2C_INT_RX_UNDERFLOW & status) {
	}

	if (I2C_INT_RX_OVERFLW & status) {
	}

	if (I2C_INT_TX_OVERFLW & status) {
	}

	if (I2C_INT_TX_ABRT & status) {
	}
}

static inline void i2c_receive_isr(dw_i2c_t *i2c_dev, dw_i2c_ops_t *i2c_ops, uint32_t status)
{
	int32_t result = E_OK;
	i2c_runtime_t *runtime = &i2c_drv.runtime;
	DEV_BUFFER *dev_buf = &runtime->xfer_buffer;
	if (I2C_INT_RX_FULL & status) {
		uint32_t over_err_flag = 0;
		uint32_t data_cmd, dev_status = 0;
		if ((NULL != dev_buf->buf) && (dev_buf->ofs < dev_buf->len)) {
			uint8_t *buf = (uint8_t *)dev_buf->buf;
			uint32_t remain_size = dev_buf->len - dev_buf->ofs;
			uint32_t remain_cmd_size = remain_size - runtime->rx_thres;
			do {
				result = i2c_ops->status(i2c_dev, &dev_status);
				if (E_OK != result) {
					break;
				}

				//if (DW_I2C_RXFIFO_N_EMPTY(dev_status)) {
				while (0 == DW_I2C_RXFIFO_N_EMPTY(dev_status)) {
					i2c_ops->status(i2c_dev, &dev_status);
				}
					result = i2c_ops->read(i2c_dev, NULL);
					if (result >= 0) {
						buf[dev_buf->ofs++] = result & 0xFF;
						remain_size--;
					} else {
						break;
					}
				//}

				if (remain_cmd_size) {
					if (DW_I2C_TXFIFO_N_FULL(dev_status)) {
						uint32_t last_flag = 0;
						if (2 == remain_cmd_size) {
							last_flag = 1;
						}
						if (remain_cmd_size > 1) {

							data_cmd = DATA_COMMAND(i2c_cmd_read, last_flag, restart_dis, 0);
							result = i2c_ops->write(i2c_dev, data_cmd);
							if (E_OK != result) {
								break;
							}
						}
						remain_cmd_size--;
					}
				}
			} while (remain_size);

			if (0 == remain_size) {
				/* indicate to upper layer. */
				if (runtime->xfer_done) {
					runtime->xfer_done(NULL);
				}
				over_err_flag = 1;
			}

		} else {
			over_err_flag = 1;
		}

		if (over_err_flag) {
			/* disable interrupt.*/
			result = i2c_ops->int_mask(i2c_dev, I2C_INT_RX_FULL, 0);
			if (E_OK == result) {
				DEV_BUFFER_INIT(dev_buf, NULL, 0);
			}
			runtime->xfer_type = 0;
		}
	}
}

static inline void i2c_transmit_isr(dw_i2c_t *i2c_dev, dw_i2c_ops_t *i2c_ops, uint32_t status)
{
	int32_t result = E_OK;

	i2c_runtime_t *runtime = &i2c_drv.runtime;

	DEV_BUFFER *dev_buf = &runtime->xfer_buffer;

	if (I2C_INT_TX_EMPTY & status) {
		uint32_t over_err_flag = 0;
		uint32_t data_cmd, dev_status = 0;
		if ((NULL != dev_buf->buf) && (dev_buf->ofs < dev_buf->len)) {
			    uint8_t *buf = (uint8_t *)dev_buf->buf;

				uint32_t remain_size = dev_buf->len - dev_buf->ofs;
				//uint32_t remain_cmd_size = remain_size - runtime->tx_thres;
		   do {
				result = i2c_ops->status(i2c_dev, &dev_status);
				if (E_OK != result) {
					/* TODO: record error! and restart the transmission? */
					break;
				}

				if (remain_size > 1) {
					if (DW_I2C_TXFIFO_N_FULL(dev_status)) {
						data_cmd = DATA_COMMAND(i2c_cmd_write, stop_dis, restart_dis, buf[dev_buf->ofs]);
						dev_buf->ofs++;
						result = i2c_ops->write(i2c_dev, data_cmd);
						remain_size--;
						//remain_cmd_size = remain_size - runtime->tx_thres;
						if (E_OK != result) {
							break;
						}
					}
					else {
						/* waiting hardware... */
						break;
					}
				}
				 if (remain_size == 1){
					if (DW_I2C_TXFIFO_N_FULL(dev_status)) {
						data_cmd = DATA_COMMAND(i2c_cmd_write, stop_enable, restart_dis, buf[dev_buf->ofs]);
						result = i2c_ops->write(i2c_dev, data_cmd);
						remain_size--;
						//remain_cmd_size = remain_size - runtime->tx_thres;
						if (E_OK != result) {
							break;
						}
					}
				}

			} while (remain_size);

			if (0 == remain_size) {
				if (runtime->xfer_done) {
					runtime->xfer_done(NULL);
				}
				over_err_flag = 1;
			}

		} else {
			over_err_flag = 1;
		}

		if (over_err_flag) {
			/* disable interrupt. */
			result = i2c_ops->int_mask(i2c_dev, I2C_INT_TX_EMPTY, 0);
			if (E_OK == result) {
				DEV_BUFFER_INIT(dev_buf, NULL, 0);
			}
			runtime->xfer_type = 0;
		}
	}
}

static inline void i2c_protocol_isr(dw_i2c_t *i2c_dev, dw_i2c_ops_t *i2c_ops, uint32_t status)
{
	if (I2C_INT_GEN_CALL & status) {
	}

	if (I2C_INT_STOP_DET & status) {
	}

	if (I2C_INT_START_DET & status) {
	}

	if (I2C_INT_ACTIVITY & status) {
	}
}

static void i2c_isr(void *params)
{

	int32_t result = E_SYS;

	uint32_t status = 0;

	i2c_runtime_t *runtime = &i2c_drv.runtime;

	dw_i2c_t *i2c_dev = NULL;
	dw_i2c_ops_t *i2c_ops = NULL;

	i2c_dev = (dw_i2c_t *)i2c_get_dev(0);

	if ((NULL != i2c_dev) && (NULL != i2c_dev->ops)) {
		i2c_ops = (dw_i2c_ops_t *)i2c_dev->ops;
		result = i2c_ops->int_status(i2c_dev, &status);
		}

	if (E_OK == result) {
		i2c_error_isr(i2c_dev, i2c_ops, status);

		if (2 == runtime->xfer_type) {
			i2c_receive_isr(i2c_dev, i2c_ops, status);
			}
		else if (1 == runtime->xfer_type) {
			i2c_transmit_isr(i2c_dev, i2c_ops, status);
			}
		else {
			i2c_protocol_isr(i2c_dev, i2c_ops, status);
			}
		}
}
//void i2c_callb(int32_t *cb) {
//
//	int32_t call = 1;
//
//	if (*cb == 0) {
//
//		cb = &call;
//
//	}
//
//}
