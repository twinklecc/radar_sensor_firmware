#include "embARC_toolchain.h"
#include "embARC_error.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "arc_exception.h"
#include "can_hal.h"
#include "dev_can.h"

#define CAN_REF_CLOCK_DEFAULT       (40000000)
#define CAN_DFS_2_FRAMESIZE(dfs) \
        ( ((dfs) < 5) ? ((dfs) + 4) : ((((dfs) - 5) << 2) + 10) )

#define CAN_DATASIZE_2_DFS(data_size) \
        ( ((data_size) < 32) ? ((data_size >> 2) - 2) : (((((data_size) >> 2) - 8) >> 2) + 5) )

#define CAN_FRAM_BUF_ELEMENT_NUM            (CAN_FRAM_BUF_DATA_SIZE/4+2)

// Possible Data Field Length of Frame: 8, 12, 16, 20, 24, 32, 48, 64. In Bytes.
#if   CAN_FRAM_BUF_DATA_SIZE == 8
#define CAN_FRAM_MAX_BUF_NUM  (64)
#elif CAN_FRAM_BUF_DATA_SIZE == 12
#define CAN_FRAM_MAX_BUF_NUM  (48)
#elif CAN_FRAM_BUF_DATA_SIZE == 16
#define CAN_FRAM_MAX_BUF_NUM  (40)
#elif CAN_FRAM_BUF_DATA_SIZE == 20
#define CAN_FRAM_MAX_BUF_NUM  (32)
#elif CAN_FRAM_BUF_DATA_SIZE == 24
#define CAN_FRAM_MAX_BUF_NUM  (32)
#elif CAN_FRAM_BUF_DATA_SIZE == 32
#define CAN_FRAM_MAX_BUF_NUM  (24)
#elif CAN_FRAM_BUF_DATA_SIZE == 48
#define CAN_FRAM_MAX_BUF_NUM  (16)
#elif CAN_FRAM_BUF_DATA_SIZE == 64
#define CAN_FRAM_MAX_BUF_NUM  (14)
#else
#error "Incorrect Frame Size!"
#endif


#define CAN_MAX_TX_BUF_ID                    (0)
#define CAN_MAX_RX_BUF_ID                    (CAN_FRAM_MAX_BUF_NUM - 1)

/* the number of ID ﬁlter elements */
#define CAN_ID_FILTER_ELEMENT_NUM            1

#ifdef OS_FREERTOS
#include "FreeRTOS.h"
#include "task.h"

#define CAN_LOCK(lock)    \
        while (xSemaphoreTake(lock, portMAX_DELAY) != pdTRUE) { \
                taskYIELD(); \
        }
#define CAN_UNLOCK(lock)    xSemaphoreGive(lock)

static xSemaphoreHandle can_sema[2] = {NULL, NULL};
#else
#define CAN_LOCK(lock)
#define CAN_UNLOCK(lock)
#endif

typedef enum{
        eCAN_INT_TYPE_RX = 0,
        eCAN_INT_TYPE_TX = 1,
        eCAN_INT_TYPE_ERROR = 3
} eCAN_INT_TYPE;

typedef enum{
        eCAN_INT_DISABLE = 0,
        eCAN_INT_ENABLE  = 1
} eCAN_INT_OP;

typedef enum{
        eCAN_BUFFER_MODE = 0,
        eCAN_FIFO_MODE   = 1
} eCAN_OPRATION_MODE;

#ifndef CHIP_ALPS_A
typedef struct can_xfer_descriptor {
        /* @mode: 0->buffer mode, 1->fifo mode. */
        uint8_t mode;

        /* @xfer_mode: 0->polling, 1->int, 2->dma, 3->task agency. */
        uint8_t data_rx_mode;
        uint8_t data_tx_mode;

        void *tx_buf;
        uint32_t tx_len;
        uint32_t tx_ofs;

        void *rx_buf;
        uint32_t rx_len;
        uint32_t rx_ofs;

        /* 000 - 8Bytes, 001 - 12Bytes, 010 - 16Bytes, 011 - 20Bytes,
         * 100 - 24Bytes, 101 - 32Bytes, 110 - 48Bytes, 111 - 64Byte.
         */
        uint8_t rx_dfs;
        uint8_t tx_dfs;
        uint8_t rx_last_id;
        uint8_t tx_last_id;

#ifdef OS_FREERTOS
        QueueHandle_t tx_queue;
        QueueHandle_t rx_queue;
#endif

        can_frame_params_t *frame_params;

        rx_indication_callback rx_indication;
        void (*xfer_callback)(void *);
} can_xfer_t;

#ifdef OS_FREERTOS
/* Task Communication */
extern QueueHandle_t queue_can_isr;
#endif
static can_t *dev_can[2] = { NULL, NULL};
static can_xfer_t can_buf[2] = {
    {
        /* buffer mode. */
#ifdef OS_FREERTOS
        .tx_queue = NULL,
        .rx_queue = NULL,
#endif
        .mode = eCAN_BUFFER_MODE,
        .data_rx_mode = 1,
#if (CAN_TX_INT_MODE == 1)
        .data_tx_mode = DEV_XFER_INTERRUPT,
#else
        .data_tx_mode = DEV_XFER_POLLING,
#endif
        .rx_dfs = CAN_DATASIZE_2_DFS(CAN_FRAM_BUF_DATA_SIZE),
        .tx_dfs = CAN_DATASIZE_2_DFS(CAN_FRAM_BUF_DATA_SIZE)
    },
    {
        /* buffer mode. */
#ifdef OS_FREERTOS
        .tx_queue = NULL,
        .rx_queue = NULL,
#endif
        .mode = eCAN_BUFFER_MODE,
        .rx_dfs = 0,
        .tx_dfs = 0
    }
};

/* the configuration of the id filter */
static can_id_filter_param_t id_filter_cfg = {
        .frame_format = eSTANDARD_FRAME,
        .reject_no_match = true,
        .reject_remote = eEXTENDED_FRAME,
        .filter_size = CAN_ID_FILTER_ELEMENT_NUM,
        .mask = 0x1FFFFFFF
};

/* the rule configuration of the id filter element */
static can_id_filter_element_t id_filter_element_cfg[CAN_ID_FILTER_ELEMENT_NUM] = {
        {
                /* element 0 */
                .frame_format = eSTANDARD_FRAME,
                .filter_type = eDUAL_ID,
                .filter_cfg = eRX_BUF,
                .filter_id0 = 0x111,
                .filter_id1 = 0x200
        }
};

can_protocol_cfg_t protocol_cfg = {
#if (USE_CAN_FD == 1)
        .fd_operation = 1,
        .bit_rate_switch = 1,
        .tx_delay_compensate = 0,
#endif
        .auto_retransmission = 1,
};

static can_frame_params_t frame_params[2];

static inline int32_t _can_interrupt_enable(can_t *can_ptr, uint32_t type, uint32_t enable, uint32_t mode);
static int32_t can_int_install(uint32_t id);
static int32_t can_install_data_buffer(uint32_t id, uint32_t *buf, uint32_t len, uint32_t rx_or_tx);
static void can_reset_data_buffer(can_xfer_t *buf);

static inline clock_source_t can_clock_source(uint32_t id)
{
        if (id > 0) {
                return CAN1_CLOCK;
        } else {
                return CAN0_CLOCK;
        }
}

/*
 * Default: standard frame, buffer mode with len 8bytes.
 * */
int32_t can_init(uint32_t id, uint32_t nomi_baud, uint32_t data_baud)
{
        int32_t result = E_OK;
        can_ops_t *can_ops = NULL;
        can_baud_t *baud_param = NULL;

        uint32_t index = 0;

        if ((id >= 2) || (0 == nomi_baud)) {
                result = E_PAR;
        } else {
                dev_can[id] = can_get_dev(id);
                if (NULL == dev_can[id]) {
                        result = E_NOEXS;
                } else {
                        can_ops = (can_ops_t *)dev_can[id]->ops;
                        if (NULL == can_ops) {
                            result = E_NOOPS;
                        } else {
                                /* As long as the firmware starts to run, the CAN controller may encounter an uncertain state.
                                 * So need to do a software reset before initializing CAN controller.
                                 * */
                                result = can_ops->reset(dev_can[id], 1);
                                if (result != E_OK) {
                                        EMBARC_PRINTF("CAN rest failed!\r\n");
                                }
                                if (0 == id) {
                                        can0_enable(1);
                                } else {
                                        can1_enable(1);
                                }

                                /* Enable the automatic retransmission */
                                /* The maximum value of the retransferring times is 16 */
                                protocol_cfg.auto_retransmission = 1;
                                can_ops->protocol_config(dev_can[id], &protocol_cfg);

                                /* Configurate ID Filter Control Register */
                                can_ops->id_filter_config(dev_can[id], &id_filter_cfg);

                                /* Setting the configuraton of ID filter element */
                                for (index = 0; (E_OK == result) && (index < CAN_ID_FILTER_ELEMENT_NUM); index++) {
                                        result = can_ops->id_filter_element(dev_can[id], index, &id_filter_element_cfg[index]);
                                }

                                clock_source_t clk_src = can_clock_source(id);
                                baud_param = can_get_baud(clock_frequency(clk_src), nomi_baud);
                                if (NULL == baud_param) {
                                        result = E_NOEXS;
                                } else {
                                        result = can_ops->baud(dev_can[id], 0, baud_param);
                                }
#if (USE_CAN_FD == 1)
                                /* Initial CAN fd*/
                                can_ops->protocol_config(dev_can[id], &protocol_cfg);

                                baud_param = can_get_baud(clock_frequency(clk_src), data_baud);
                                if (NULL == baud_param) {
                                        result = E_NOEXS;
                                } else {
                                        /* Setting the baud rate parameters of DATA bit */
                                        result = can_ops->baud(dev_can[id], 1, baud_param);
                                }
#endif
                                if (E_OK == result) {
                                        result = can_int_install(id);
                                }
                        }
                }
        }

        if (E_OK == result) {
            result = can_ops->data_field_size(dev_can[id], 0, \
                    can_buf[id].rx_dfs, can_buf[id].rx_dfs);
            if (E_OK == result) {
                    result = can_ops->data_field_size(dev_can[id], 1, \
                            can_buf[id].tx_dfs, can_buf[id].tx_dfs);
            }
        }

#ifdef OS_FREERTOS
        if ((E_OK == result)) {
                if (DEV_XFER_INTERRUPT <= can_buf[id].data_tx_mode) {
                        if (can_buf[id].tx_queue == NULL) {
                                can_buf[id].tx_queue = xQueueCreate(5, 12);
                                if (NULL == can_buf[id].tx_queue) {
                                        result = E_SYS;
                                }
                        }
                }
        }

        if ((E_OK == result)) {
                if (DEV_XFER_INTERRUPT <= can_buf[id].data_rx_mode) {
                        if (can_buf[id].rx_queue == NULL) {
                                can_buf[id].rx_queue = xQueueCreate(5, 12);
                                if (NULL == can_buf[id].rx_queue) {
                                        result = E_SYS;
                                }
                        }
                }
        }

        if (E_OK == result) {
                if(can_sema[id] == NULL)
                {
                        can_sema[id] = xSemaphoreCreateBinary();
                }
                xSemaphoreGive(can_sema[id]);
        }
#else
        /* TODO: if need, add self-define queue scheme. */
#endif

        return result;
}


/* mainly ID filter! */
int32_t can_config(uint32_t id, void *param)
{
    int32_t result = E_OK;


    CAN_LOCK(can_sema[id]);
    result = E_SYS;
    CAN_UNLOCK(can_sema[id]);

    return result;
}

int32_t can_indication_register(uint32_t dev_id, rx_indication_callback func)
{
    int32_t result = E_OK;

    if ((dev_id >= 2) || (NULL == func)) {
            result = E_PAR;
    } else {
            uint32_t cpu_status = arc_lock_save();
            can_buf[dev_id].rx_indication = func;
            arc_unlock_restore(cpu_status);
    }

    return result;
}

#ifdef OS_FREERTOS
int32_t can_queue_install(uint32_t id, QueueHandle_t queue, uint32_t rx_or_tx)
{
        int32_t result = E_OK;

        if ((id >= 2) || (NULL == queue)) {
                result = E_PAR;
        } else {
                if (rx_or_tx) {
                        can_buf[id].tx_queue = queue;
                } else {
                        can_buf[id].rx_queue = queue;
                }
        }

        return result;
}
#endif

void *can_xfer_buffer(uint32_t id, uint32_t rx_or_tx)
{
        void *buf = NULL;

        if (id < 2) {
                if (rx_or_tx) {
                        buf = (void *)((uint32_t)&can_buf[id].tx_buf);
                } else {
                        buf = (void *)((uint32_t)&can_buf[id].rx_buf);
                }
        }

        return buf;
}

int32_t can_read(uint32_t id, uint32_t *buf, uint32_t len)
{
        int32_t result = E_OK;

        uint32_t cpu_sts = 0;

        uint32_t frame_size = 0;
        dev_xfer_mode_t xfer_mode = DEV_XFER_POLLING;

        can_ops_t *can_ops = (can_ops_t *)dev_can[id]->ops;

        CAN_LOCK(can_sema[id]);
        if ((id >= 2) || (NULL == dev_can[id]) || (NULL == can_ops) || \
                (NULL == buf) || (0 == len)) {
                result = E_PAR;
        } else {
                frame_size = CAN_DFS_2_FRAMESIZE(can_buf[id].rx_dfs);
                if (len % frame_size) {
                        result = E_PAR;
                } else {
                        xfer_mode = can_buf[id].data_rx_mode;
                }
        }

        if (E_OK == result) {
            uint32_t buf_id = can_buf[id].rx_last_id;

            switch (xfer_mode) {
            case DEV_XFER_POLLING:
                    while (len) {
                            result = can_ops->read_frame_blocked(dev_can[id], \
                                    buf_id, buf, frame_size);
                            if (E_OK != result) {
                                    break;
                            } else {
                                    buf_id += 1;
                                    if (buf_id > 63) {
                                            buf_id = 0;
                                    }

                                    buf += frame_size;
                                    len -= frame_size << 2;
                            }
                    }
                    can_buf[id].rx_last_id = buf_id;
                    break;

           case DEV_XFER_INTERRUPT:
                    cpu_sts = arc_lock_save();
                    if (can_buf[id].rx_len > can_buf[id].rx_ofs) {
#ifdef OS_FREERTOS
                            //result = io_queue_send(can_buf[id].rx_queue, (void *)buf, len, 0);
#else
                            /* TODO: if need, add self-define queue scheme here! */
#endif
                    } else {
                            can_buf[id].rx_buf = buf;
                            can_buf[id].rx_len = len;
                            can_buf[id].rx_ofs = 0;
                            /* re-enable rx interrupt. */
                            result = _can_interrupt_enable(dev_can[id], eCAN_INT_TYPE_RX, eCAN_INT_ENABLE, can_buf[id].mode);
                    }
                    arc_unlock_restore(cpu_sts);
                    break;

           case DEV_XFER_DMA:
                    /* TODO: don't support in current platform! */
                    break;
           case DEV_XFER_TASK_AGENCY:
                    /* nothing to do, taks agency will push data. */
                    break;

           default:
                    result = E_SYS;
            }
        }
        CAN_UNLOCK(can_sema[id]);

        return result;
}

int32_t can_frame_read(uint32_t dev_id, uint32_t *msg_id, uint32_t **data)
{
        int32_t result = E_OK;

        if ((dev_id >= 2) || (NULL == msg_id) || (NULL == data)) {
                result = E_PAR;
        } else {
#ifdef OS_FREERTOS
                DEV_BUFFER message;
                if(pdTRUE != xQueueReceive(can_buf[dev_id].rx_queue,\
                            (void *)&message, 0)) {
                        result = E_SYS;
                } else {
                        result = message.len;
                        *data = (uint32_t *)message.buf;
                        *msg_id = message.ofs;
                }
#endif
        }
        return result;
}

int32_t can_frame_params_setup(uint32_t id, can_frame_params_t *params)
{
        int32_t result = E_OK;

        uint32_t cpu_sts = arc_lock_save();
        if ((id >= 2) || (NULL == params)) {
                result = E_PAR;
        } else {
                frame_params[id].msg_id = params->msg_id;
                frame_params[id].eframe_format = params->eframe_format;
                frame_params[id].eframe_type = params->eframe_type;
                frame_params[id].len = params->len;
                can_buf[id].frame_params = &frame_params[id];
        }
        arc_unlock_restore(cpu_sts);

        return result;
}

void can_send_data(uint32_t bus_id, uint32_t frame_id, uint32_t *data, uint32_t len)
{
        int32_t result = E_OK;
        uint32_t cnt = 0;

        if ((NULL == data)) {
                return;
        }

        if (len > eDATA_LEN_8) {
            if (len % CAN_FRAM_BUF_DATA_SIZE) {
                    /* The parameter len must be an integer multiple of CAN_FRAM_BUF_DATA_SIZE.*/
                    EMBARC_PRINTF("[%s] length is incorrect!\r\n", __func__);
                    return;
            }
        }

        can_frame_params_t params = {
                .msg_id = frame_id, \
                .eframe_format = eSTANDARD_FRAME, \
                .eframe_type = eDATA_FRAME, \
                .len = CAN_FRAM_BUF_DATA_SIZE
        };

        /* save frame params to global varible */
        can_frame_params_setup(bus_id, &params);
        eCAN_FRAME_FORMAT frame_format = params.eframe_format;

        for (cnt = 0; cnt < 20; cnt++) {
                result = can_write(bus_id, data, len, frame_format);
                if (E_DBUSY == result) {
                        chip_hw_udelay(100);
                } else if (E_OK == result) {
                        break;
                } else {
                        EMBARC_PRINTF("can_write error, result: 0x%x!\r\n", result);
                }
        }

        if (result == E_DBUSY) {
                /* E_DBUSY will cause CAN TX FIFO overflow,
                * so call "can_init" function to re-initialize CAN device to free Tx FIFO.
                * */
#if (USE_CAN_FD == 1)
                can_init(bus_id, CAN_BAUDRATE_500KBPS, CAN_BAUDRATE_1MBPS);
#else
                can_init(bus_id, CAN_BAUDRATE_500KBPS, 0);
#endif
                /* Enable the RX interrupt */
                can_interrupt_enable(bus_id, 0, 1);

                /* Enable the error interrupt */
                can_interrupt_enable(bus_id, 3, 1);

#if (CAN_TX_INT_MODE == 1)
                /* Enable the TX interrupt */
                can_interrupt_enable(bus_id, 1, 1);
#endif
                EMBARC_PRINTF("can_send_data reset: %d!\r\n", result);
        }
}

static int32_t can_send_frame(can_t *dev_can, can_ops_t *dev_ops, can_xfer_t *xfer)
{
        int32_t result = E_OK;
        uint32_t header0 = 0, header1 = 0;
        uint32_t idx, tx_cnt = 0, remain_size = 0;
        uint32_t *data_ptr = NULL;
        uint32_t frame_buf[CAN_FRAM_BUF_ELEMENT_NUM];
        int32_t dlc = 0;            /* dlc: data length code */
        int32_t dfw_cnt = 0;        /* dfw: data fileld by word */
        can_ops_t *can_ops = NULL;

        can_ops = (can_ops_t *)dev_can->ops;

        if (xfer->frame_params) {
                header0 = xfer->frame_params->msg_id & 0x1FFFFFFF;
                if (eSTANDARD_FRAME == xfer->frame_params->eframe_format) {
                        /* standard frame. */
                        header0 = (xfer->frame_params->msg_id << 18);
                } else {
                        /* extended frame. */
                        header0 |= (1 << 30);
                }

                if (eREMOTE_FRAME == xfer->frame_params->eframe_type) {
                        /* remote frame. */
                        header0 |= (1 << 29);
                }

                /* send actual bytes number when tx length is less than 8 bytes */
                if (xfer->tx_len < eDATA_LEN_8) {
                        xfer->frame_params->len = xfer->tx_len;
                }

                dlc = can_get_dlc(xfer->frame_params->len);
                if (dlc < 0) {
                        EMBARC_PRINTF("can_send_frame length error!\r\n");
                        return E_PAR;
                }

                header1 |= (((dlc & 0xF) << 16) | (protocol_cfg.bit_rate_switch << 20) | \
                             (protocol_cfg.fd_operation << 21));

                /* Fill frame_buf data with header0 and header1 */
                frame_buf[0] = header0;
                frame_buf[1] = header1;

                /* send one frame at a time */
                /* tx_len is the total length that need to be transferred */
                /* tx_ofs is the offset number to indicate the length that already transferred */
                /* remain_size is the reamining length that still need to be transferred */
                remain_size = xfer->tx_len - xfer->tx_ofs;

                do {
                        if (eDATA_FRAME == xfer->frame_params->eframe_type) {
                                if (remain_size > xfer->frame_params->len) {
                                        tx_cnt = xfer->frame_params->len;
                                } else {
                                        tx_cnt = remain_size;
                                }

                                data_ptr = (uint32_t *)xfer->tx_buf;
                                /* Change the unit length of tx_ofs from uint32_t to uint_8 by ">>2"  */
                                /* data_ptr should be incremented by BYTE unit */
                                data_ptr += (xfer->tx_ofs >> 2);


                                /* Fill frame_buf data after header0 and header1 */
                                /* idx start from 2 because frame_buf[0] and frame_buf[1] are header0 and header1 */
                                /* Change the unit length of tx_cnt from uint32_t to uint_8 by ">>2"  */
                                /* idx should be incremented by BYTE unit */
                                if (tx_cnt < eDATA_LEN_8) {
                                        dfw_cnt = 2;
                                } else {
                                        dfw_cnt = (tx_cnt >> 2);
                                }

                                for (idx = 2; idx < (dfw_cnt + 2); idx++) {
                                        frame_buf[idx] = *data_ptr++;
                                }
                        }

                        uint32_t buf_id = xfer->tx_last_id;

                        uint32_t cpu_status = arc_lock_save();
                        result = can_ops->write_frame(dev_can, buf_id, frame_buf, CAN_FRAM_BUF_ELEMENT_NUM);
                        if (E_OK == result) {
                                /* Increment tx_ofs with tx_cnt */
                                xfer->tx_ofs += tx_cnt;

                                buf_id += 1;
                                if (buf_id > CAN_MAX_TX_BUF_ID) {
                                        buf_id = 0;
                                }
                                xfer->tx_last_id = buf_id;
                        }
                        arc_unlock_restore(cpu_status);

                        if (E_OK != result) {
                            break;
                        }
#if (CAN_TX_INT_MODE == 1)
                        /* For Int mode, only need to send the first frame to trigger can tx interrupt */
                        /* The remaining data send will be implement in can Tx interrupt handler : _can_tx_handler */
                        break;
#endif
                        /* For Polling mode, data send will be implemented frame by frame until all the data have been sent : remain_size == 0 */
                        remain_size = xfer->tx_len - xfer->tx_ofs;

                }while(remain_size);
    }

    return result;
}

int32_t can_write(uint32_t id, uint32_t *buf, uint32_t len, uint32_t flag)
{
        int32_t result = E_OK;
        dev_xfer_mode_t xfer_mode = can_buf[id].data_tx_mode;
        can_ops_t *can_ops = NULL;

        can_ops = (can_ops_t *)dev_can[id]->ops;
        if ((id >= 2) || (NULL == dev_can[id]) || (NULL == can_ops) || \
            (NULL == buf) || (0 == len)) {
                result = E_PAR;
        }

        /* CAN_LOCK here to avoid multi-task access identical can port  */
        CAN_LOCK(can_sema[id]);

        if (E_OK == result) {
                switch (xfer_mode) {
                case DEV_XFER_POLLING:
                        result = can_install_data_buffer(id, buf, len, 1);
                        result = can_send_frame(dev_can[id], can_ops, &can_buf[id]);
                        if (E_OK != result) {
                               // EMBARC_PRINTF("CAN Polling Mode Transfer failed!\r\n");
                        } 
                        break;

                case DEV_XFER_INTERRUPT:
                        if (can_buf[id].tx_len > can_buf[id].tx_ofs) {
                                /* The previous data send has NOT finished    */
                                /* Theoretically, by the increment of "tx_ofs", finally, "tx_ofs" should equle to "tx_len" 
                                   which means the previous data send is finished, then a new round of data send can be started */
                                EMBARC_PRINTF("CAN Int Mode Bus Busy!\r\n");
                                result = E_DBUSY;
                        } else {
                                result = can_install_data_buffer(id, buf, len, 1);
                                /* Enable CAN Tx Int */
                                result = _can_interrupt_enable(dev_can[id], eCAN_INT_TYPE_TX, eCAN_INT_ENABLE, can_buf[id].mode);
                                result = can_send_frame(dev_can[id], can_ops, &can_buf[id]);
                                if (E_DBUSY == result) {
                                        /* the sending status of the first frame is busy, need to disable interrupt */
                                        EMBARC_PRINTF("CAN Int Mode first frame Data Send Busy!\r\n");
                                        _can_interrupt_enable(dev_can[id], eCAN_INT_TYPE_TX, eCAN_INT_DISABLE, can_buf[id].mode);
                                        can_reset_data_buffer(&can_buf[id]);
                                }
                        }
                        break;

                case DEV_XFER_DMA:
                        /* NOT support in current platform! */
                        result = E_SYS;
                        break;

                case DEV_XFER_TASK_AGENCY:
                        /* TODO: if need, add self-define queue scheme at here. */
                        break;

                default:
                        result = E_SYS;
            }
        }

        CAN_UNLOCK(can_sema[id]);

        return result;
}

/* This function will be called only once at the beginning of the CAN data send */
static int32_t can_install_data_buffer(uint32_t id, uint32_t *buf, uint32_t len, uint32_t rx_or_tx)
{
        int32_t result = E_OK;

        if ((id >= 2) || (NULL == dev_can[id]) || (NULL == buf) || (0 == len)) {
                result = E_PAR;
        }

        /* lock cou here since can_buf is a global parameter */
        uint32_t cpu_status = arc_lock_save();

        if (rx_or_tx) {
                /* CAN TX Data Install */
                can_buf[id].tx_buf = buf;
                can_buf[id].tx_len = len;
                can_buf[id].tx_ofs = 0;
        } else {
                /* CAN RX Data Install */
                can_buf[id].rx_buf = buf;
                can_buf[id].rx_len = len;
                can_buf[id].rx_ofs = 0;
        }

        arc_unlock_restore(cpu_status);

        return result;
}

static void can_reset_data_buffer(can_xfer_t *buf)
{
        /* lock cou here since can_buf is a global parameter */
        uint32_t cpu_status = arc_lock_save();

        buf->tx_buf = NULL;
        buf->tx_len = 0;
        buf->tx_ofs = 0;

        arc_unlock_restore(cpu_status);
}

int32_t can_interrupt_enable(uint32_t id, uint32_t type, uint32_t enable)
{
        int32_t result = E_OK;

        CAN_LOCK(can_sema[id]);
        if ((id >= 2) || (NULL == dev_can[id])) {
                result = E_PAR;
        } else {
                result = _can_interrupt_enable(dev_can[id], type, enable, can_buf[id].mode);
        }
        CAN_UNLOCK(can_sema[id]);

        return result;
}

static inline int32_t _can_interrupt_enable(can_t *can_ptr, uint32_t type, uint32_t enable, uint32_t mode)
{
        int32_t result = E_OK;

        uint32_t int_bitmap = 0;
        can_ops_t *can_ops = (can_ops_t *)can_ptr->ops;

        if ((NULL == can_ptr) || (NULL == can_ops)) {
                result = E_PAR;
        } else {
                if (eCAN_BUFFER_MODE == mode) {
                        /* Buffer Mode */
                        if (type == eCAN_INT_TYPE_TX) {
                                int_bitmap = CAN_INT_TX_COMPLISHED;
                        } else if (type == eCAN_INT_TYPE_ERROR){
                                int_bitmap = CAN_INT_ERR_PASSIVE | CAN_INT_BUS_OFF | CAN_INT_PROTOCOL_ERR;
                                can_ptr->int_line_bitmap[3] = int_bitmap;
                        } else {
                                int_bitmap = CAN_INT_RX_NEW_MESSAGE;
                        }
                } else {
                        /* FIFO Mode */
                        if (type == eCAN_INT_TYPE_RX) {
                                int_bitmap = CAN_INT_RX_FIFO_FILLED;
                        } else {
                                int_bitmap = CAN_INT_TX_FIFO_READY;
                        }
                }

                result = can_ops->int_enable(can_ptr, enable, int_bitmap);
        }

        return result;
}

/* Note: called during boot. */
int32_t can_xfer_callback_install(uint32_t id, void (*func)(void *))
{
    int32_t result = E_OK;

    CAN_LOCK(can_sema[id]);
    if ((id >= 2) || (NULL == dev_can[id]) || (NULL == func)) {
            result = E_PAR;
    } else {
            can_buf[id].xfer_callback = func;
    }
    CAN_UNLOCK(can_sema[id]);

    return result;
}

static inline int32_t can_get_dlc(uint32_t len)
{
        int32_t dlc = 0;

#if (USE_CAN_FD == 0)
        if (len > 8) {
#if (CAN_TX_INT_MODE == 1)
                len = 8;
#else
                return E_PAR;
#endif
        }
#else
        if (len > 64)
                return E_PAR;
#endif

        if ((len > 8) && (len <= 12))
                len = eDATA_LEN_12;
        else if ((len > 12) && (len <= 16))
                len = eDATA_LEN_16;
        else if ((len > 16) && (len <= 20))
                len = eDATA_LEN_20;
        else if ((len > 20) && (len <= 24))
                len = eDATA_LEN_24;
        else if ((len > 24) && (len <= 32))
                len = eDATA_LEN_32;
        else if ((len > 32) && (len <= 48))
                len = eDATA_LEN_48;
        else if ((len > 48) && (len <= 64))
                len = eDATA_LEN_64;

        switch (len) {
        case eDATA_LEN_64:
        case eDATA_LEN_48:
        case eDATA_LEN_32:
                dlc = 11 + (len >> 4);
                break;
        case eDATA_LEN_24:
        case eDATA_LEN_20:
        case eDATA_LEN_16:
        case eDATA_LEN_12:
                dlc = 6 + (len >> 2);
                break;
        default:
                dlc = len;
        }

        return dlc;
}

static inline uint32_t can_get_datalen(uint32_t dlc)
{
        uint32_t len = 0;

#if (USE_CAN_FD == 0)
        if (dlc > 8)
                dlc = 8;
#else
        if (dlc > 15)
                dlc = 15;
#endif

        switch (dlc) {
        case eDATA_DLC_15:
        case eDATA_DLC_14:
        case eDATA_DLC_13:
                len = (dlc - 11) << 4;
                break;
        case eDATA_DLC_12:
        case eDATA_DLC_11:
        case eDATA_DLC_10:
        case eDATA_DLC_9:
                len = (dlc - 6) << 2;
                break;
        default:
                len = dlc;
        }

        return len;
}

int32_t can_receive_data(uint32_t dev_id, can_data_message_t *msg)
{
        int32_t result = E_OK;
        uint32_t buf_id = 0, dlc = 0, len = 0;
        can_ops_t *can_ops = (can_ops_t *)dev_can[dev_id]->ops;
        uint32_t frame_size = CAN_DFS_2_FRAMESIZE(can_buf[dev_id].rx_dfs);
        uint32_t rx_buf[18];

        can_frame_params_t *params = msg->frame_params;

        if ((dev_id >= 2) || (NULL == dev_can[dev_id]) || (NULL == can_ops)) {
                result = E_PAR;
        }

        if (can_buf[dev_id].rx_last_id > CAN_MAX_RX_BUF_ID) {
                can_buf[dev_id].rx_last_id = 0;
        }
        buf_id = can_buf[dev_id].rx_last_id;

        result = can_ops->read_frame(dev_can[dev_id], buf_id, rx_buf, frame_size);
        if (E_OK == result) {
                params->msg_id = rx_buf[0] & 0x1FFFFFFF;
                params->eframe_format = (rx_buf[0] & (1 << 30)) ? (1) : (0);
                dlc = (rx_buf[1] >> 16) & 0xF;
                len = can_get_datalen(dlc);
                if (len > 0) {
                        params->len = len;

                        if (params->eframe_format == eEXTENDED_FRAME) {
                                params->msg_id = rx_buf[0] & 0x1FFFFFFF;
                        } else {
                                params->msg_id = (rx_buf[0] & 0x1FFFFFFF) >> 18;
                        }
                        transfer_bytes_stream(&rx_buf[2], msg->data, params->len);
                        can_buf[dev_id].rx_last_id += 1;
                }
        }

        return result;
}

static int32_t _can_rx_buffer_full(can_t *can_ptr, can_xfer_t *buf)
{
        int32_t result = E_OK;

        if ((can_ptr) && (buf)) {
                buf->rx_buf = NULL;
                buf->rx_len = 0;
                buf->rx_ofs = 0;

                result = _can_interrupt_enable(can_ptr, eCAN_INT_TYPE_RX, eCAN_INT_DISABLE, buf->mode);
                if (E_OK == result) {
                    if (buf->xfer_callback) {
                            buf->xfer_callback(NULL);
                    } else {
                            result = E_SYS;
                    }
                }
        } else {
                result = E_PAR;
        }

        return result;
}

static int32_t _can_adjust_buf_id(can_t *can_ptr, can_ops_t *can_ops, uint32_t buf_id)
{
	uint32_t start_id = buf_id;

	while (0 == can_ops->rx_buf_status(can_ptr, buf_id)) {
		buf_id ++;
		if (buf_id > CAN_MAX_RX_BUF_ID) {
			buf_id = 0;
		}
		if (start_id == buf_id) {
			break;
		}
	}

	return buf_id;
}

static int32_t _can_frame_read(can_t *can_ptr, can_ops_t *can_ops, can_xfer_t *buf)
{
	int32_t result = E_OK;
	uint32_t i;
	uint32_t buf_id = 0, len = 0;
	uint32_t frame_size = CAN_DFS_2_FRAMESIZE(buf->rx_dfs);
	uint32_t rx_buf[18];

	/* Adjust FW RX buffer id. */
	buf->rx_last_id = _can_adjust_buf_id(can_ptr, can_ops, buf->rx_last_id);

	/* Here, we read out all received frame from HW buffer,
	 * start from FW RX buffer id.
	 */
	for (i=0;i<CAN_FRAM_MAX_BUF_NUM;i++){
		if (buf->rx_last_id > CAN_MAX_RX_BUF_ID) {
			buf->rx_last_id = 0;
		}
		buf_id = buf->rx_last_id;

		result = can_ops->read_frame(can_ptr, buf_id, rx_buf, frame_size);
		if (E_OK == result) {
			uint32_t msg_id, ide, dlc;
			msg_id = rx_buf[0] & 0x1FFFFFFF;
			ide = (rx_buf[0] & (1 << 30)) ? (1) : (0);
			dlc = (rx_buf[1] >> 16) & 0xF;
			len = can_get_datalen(dlc);
			if (len > 0) {
				if (buf->rx_indication) {
					buf->rx_indication(msg_id, ide, &rx_buf[2], len);
#ifdef OS_FREERTOS
					uint32_t msg = 0;
					xQueueSendFromISR(queue_can_isr, (void *)&msg, 0);
#endif
				}
			}
			buf->rx_last_id += 1;
		} else if (E_NODATA == result) {
			result = E_OK;
			break;
		} else {
			EMBARC_PRINTF("[%s] can_ops->read_frame ret err: %d\r\n", __func__, result);
			break;
		}
	}

	return result;
}

/*
 * rx queue is in can driver layer!
 * */
static int32_t _can_rx_buffer_read(can_t *can_ptr, can_ops_t *can_ops, can_xfer_t *buf)
{
        int32_t result = E_OK;

        uint32_t buf_id = 0;
        uint32_t frame_size = CAN_DFS_2_FRAMESIZE(buf->rx_dfs);

        uint32_t *rx_buf = NULL;

        if (buf->rx_last_id > CAN_MAX_RX_BUF_ID) {
                buf->rx_last_id = 0;
        }
        buf_id = buf->rx_last_id;

        while (buf->rx_len > buf->rx_ofs) {
            if (buf->rx_len - buf->rx_ofs < (frame_size << 2)) {
                    /* TODO: rx buffer unaligned with frame size, Panic! */
                    result = E_SYS;
                    break;
            } else {
                    /* received data. if need, restall rx buffer! */
                    rx_buf = (uint32_t *)((uint32_t)buf->rx_buf + buf->rx_ofs);
                    result = can_ops->read_frame(can_ptr, buf_id, rx_buf, frame_size);
                    if (E_OK == result) {
                            buf_id += 1;
                            if (buf_id > CAN_MAX_RX_BUF_ID) {
                                    buf_id = 0;
                            }

                            buf->rx_ofs += frame_size << 2;
                            if (buf->rx_ofs >= buf->rx_len) {
                                    result = _can_rx_buffer_full(can_ptr, buf);
                                    break;
                            }
                    } else if (E_NODATA == result) {
                            /* hw rx buffer no data! */
                            result = E_OK;
                            break;
                    } else {
                            /* device operation error, Panic! */
                            break;
                    }
                }
        }

        /* record rx buffer id. */
        buf->rx_last_id = buf_id;

        return result;
}

/* rx fifo reaches to watermark. or tx buffer/fifo new message. */
static void _can_rx_handler(can_t *can_ptr, can_xfer_t *buf, uint32_t line, void *param)
{
        int32_t result = E_OK;
        uint32_t int_status = 0;

        can_ops_t *can_ops = NULL;

        if ((NULL == can_ptr) || (line >= 4) || \
                (NULL == buf)) {
                result = E_PAR;
        } else {
                can_ops = (can_ops_t *)can_ptr->ops;
                if (NULL == can_ops) {
                        result = E_NOOPS;
                } else {
                        result = can_ops->int_status(can_ptr, 0xFFFFFFFFU);
                        if (result > 0) {
                                int_status = result;

				                /* We firstly clear the interrupts! */
				                result = can_ops->int_clear(can_ptr, int_status);
                        } else {
                                /* TODO: exception! no status, but interrupt! */
                                result = E_SYS;
				                EMBARC_PRINTF("[%s] exception! no status, but interrupt!\r\n", __func__);
			                    return;
                        }
                }
        }

        if (E_OK == result) {
                if (int_status & CAN_INT_RX_NEW_MESSAGE) {
                        /* received frame. */
                        if (buf->rx_indication) {
                                result = _can_frame_read(can_ptr, can_ops, buf);
                        } else {
                                if ((NULL == buf->rx_buf) || \
                                        (0 == buf->rx_len) || \
                                        (buf->rx_ofs >= buf->rx_len)) {
                                        result = E_PAR;
                                } else {
                                        result = _can_rx_buffer_read(can_ptr, can_ops, buf);
                                }
                        }
                } else if (int_status & CAN_INT_RX_FIFO_FILLED) {
                        /* TODO: FIFO mode. */
                } else {
                        result = E_SYS;
                }
        }

        if (E_OK != result) {
                /* TODO: Error happended in interrupt context, system crash! */
        }
}

/* tx fifo is empty. or tx finished. */
static void _can_tx_handler(can_t *can_ptr, can_xfer_t *buf, uint32_t line, void *param)
{
        int32_t result = E_OK;
        uint32_t int_status = 0;

        can_ops_t *can_ops = NULL;

        if ((NULL == can_ptr) || (line >= 4) || \
                (NULL == buf) || (NULL == buf->tx_buf) || \
                (0 == buf->tx_len)) {
                result = E_PAR;
        } else {
                can_ops = (can_ops_t *)can_ptr->ops;
                if (NULL == can_ops) {
                        result = E_NOOPS;
                } else {
                        /* Check CAN Tx interrupt status */
                        result = can_ops->int_status(can_ptr, 0xFFFFFFFFU);
                        if (result > 0) {
                                int_status = result;
                                result = E_OK;
                        } else {
                                /* TODO: exception! no status, but interrupt! */
                                result = E_SYS;
                        }
                }
        }

        if (E_OK == result) {
                    if (int_status & CAN_INT_TX_COMPLISHED) {
                            /* Clear TX complete interrupt flag */
                            can_ops->int_clear(can_ptr, CAN_INT_TX_COMPLISHED);
                            /* Check whether still have data need to be sent */
                            if(buf->tx_len > buf->tx_ofs) {
                                    /* Continue to send data */
                                    can_send_frame(can_ptr, can_ops, buf);
                            } else {
                                    /* All the data have been sent, disable interrupt and reset can data buffer */
                                    _can_interrupt_enable(can_ptr, 1, 0, 0);
                                    can_reset_data_buffer(buf);
                            }
                    } else if (int_status & CAN_INT_TX_FIFO_READY) {
                            /* TODO: FIFO mode. */
                    } else {
                            result = E_SYS;
                    }
        } else {
                /* TODO: Error happended in interrupt context, system crash! */
                ;
        }
}

static void _can_error_handler(can_t *can_ptr, uint32_t line, void *param)
{
        int32_t result = E_OK;

        uint32_t int_bitmap = 0;
        uint32_t int_status = 0;

        can_ops_t *can_ops = NULL;

        if ((NULL == can_ptr) || (line >= 4)) {
                result = E_PAR;
        } else {
            int_bitmap = can_ptr->int_line_bitmap[line];

            can_ops = (can_ops_t *)can_ptr->ops;
            if (NULL == can_ops) {
                    result = E_NOOPS;
            } else {
                    result = can_ops->int_status(can_ptr, 0xFFFFFFFFU);
                    if (result > 0) {
                            int_status = result;
                            result = E_OK;
                    } else {
                            /* TODO: exception! no status, but interrupt! */
                            result = E_SYS;
                    }
            }
        }

        if (E_OK == result) {
            int_status = int_status & int_bitmap;
            while (int_status) {
                /* Clear the error interrupts */
                can_ops->int_clear(can_ptr, int_status);

                if ((int_status & CAN_INT_ERR_PASSIVE) || (int_status & CAN_INT_PROTOCOL_ERR)) {
                        EMBARC_PRINTF("[%s] protocol err: %d \r\n", __func__, E_PROTOCOL);
                }

                if (int_status & CAN_INT_BUS_OFF) {
                        EMBARC_PRINTF("[%s] bus off: %d \r\n", __func__, E_NBO);
#if 1
                        /* CAN BUS is reset */
#if (USE_CAN_FD == 1)
                        can_init(0, CAN_BAUDRATE_500KBPS, CAN_BAUDRATE_1MBPS);
#else
                        can_init(0, CAN_BAUDRATE_500KBPS, 0);
#endif
                        /* Enable the RX interrupt */
                        can_interrupt_enable(0, 0, 1);

                        /* Enable the error interrupt */
                        can_interrupt_enable(0, 3, 1);
#endif
                }
                result = can_ops->int_status(can_ptr, 0xFFFFFFFFU);
                int_status = result & int_bitmap;
            }
        }
}

static void _can_unused_handler(can_t *can_ptr, uint32_t line, void *param)
{
        /*
         * TODO: this kind of interrupt weren't enabled.
         * but their status are valid? system crash!
         * */
}


/* CAN0 interrupt service routine. */
static void can0_line0_isr(void *param)
{
        _can_rx_handler(dev_can[0], &can_buf[0], 0, param);
}

static void can0_line1_isr(void *param)
{
        _can_tx_handler(dev_can[0], &can_buf[0], 1, param);
}

static void can0_line2_isr(void *param)
{
        _can_unused_handler(dev_can[0], 2, param);
}

static void can0_line3_isr(void *param)
{
        _can_error_handler(dev_can[0], 3, param);
}


/* CAN1 interrupt service routine. */
static void can1_line0_isr(void *param)
{
        _can_rx_handler(dev_can[1], &can_buf[1], 0, param);
}

static void can1_line1_isr(void *param)
{
        _can_tx_handler(dev_can[1], &can_buf[1], 1, param);
}

static void can1_line2_isr(void *param)
{
        _can_unused_handler(dev_can[1], 2, param);
}

static void can1_line3_isr(void *param)
{
        _can_error_handler(dev_can[1], 3, param);
}


static int32_t can_int_install(uint32_t id)
{
        int32_t result = E_OK;

        INT_HANDLER int_info[4];
        uint32_t int_idx;

        can_ops_t *can_ops = (can_ops_t *)dev_can[id]->ops;

        if (0 == id) {
                int_info[0] = can0_line0_isr;
                int_info[1] = can0_line1_isr;
                int_info[2] = can0_line2_isr;
                int_info[3] = can0_line3_isr;
        } else if (1 == id) {
                int_info[0] = can1_line0_isr;
                int_info[1] = can1_line1_isr;
                int_info[2] = can1_line2_isr;
                int_info[3] = can1_line3_isr;
        } else {
                result = E_PAR;
        }

        if (E_OK == result) {
                /* Tx interrupt line select INT1, but it isn't enabled in code now*/
                result = can_ops->int_line_select(dev_can[id], CAN_INT_TX_COMPLISHED, 1);

                /* Error interrupt line select INT3 */
                result = can_ops->int_line_select(dev_can[id], CAN_INT_ERR_PASSIVE, 3);
                result = can_ops->int_line_select(dev_can[id], CAN_INT_BUS_OFF, 3);
                result = can_ops->int_line_select(dev_can[id], CAN_INT_PROTOCOL_ERR, 3);
        }

        for (int_idx = 0; (E_OK == result) && (int_idx < 4); int_idx++) {
                result = int_handler_install(dev_can[id]->int_line[int_idx], \
                        int_info[int_idx]);
                if (E_OK == result) {
                        result = int_enable(dev_can[id]->int_line[int_idx]);
                        if (E_OK == result) {
                                result = can_ops->int_line_enable(dev_can[id], int_idx, 1);
                        }
                        dmu_irq_enable(dev_can[id]->int_line[int_idx], 1);
                }
        }

        return result;
}
#endif

