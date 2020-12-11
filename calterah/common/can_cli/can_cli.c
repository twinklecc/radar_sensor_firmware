#include <string.h>
#include "embARC_toolchain.h"
#include "FreeRTOS.h"
#include "task.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "can_hal.h"
#include "can_cli.h"
#include "gpio_hal.h"
#include "baseband.h"
#include "dev_can.h"

/* TODO: move to can transciever driver in future. */
#define CAN_TRANS_EN_IO                 (22)
#define CAN_TRANS_SB_IO                 (23)

#define CAN_CLI_ITEM_CNT_MAX            (32)
#define CAN_CLI_RX_BUFFER_LEN           (64)

typedef enum {
        CAN_CLI_ITEM_IDLE = 0,
        CAN_CLI_ITEM_BUSY
} can_cli_item_state_t;


typedef struct can_cli_item {
        uint32_t lock;

        uint32_t msg_id;
        can_cli_callback callback;
} can_cli_item_t;

static uint8_t rx_msg_fill_top = 0;
static uint8_t rx_msg_pop_top = 0;
static can_frame_params_t rx_frame_params_buffer[CAN_CLI_RX_BUFFER_LEN];
static can_data_message_t rx_msg_buffer[CAN_CLI_RX_BUFFER_LEN];
static can_cli_item_t can_cli_cmd_list[CAN_CLI_ITEM_CNT_MAX];
static TaskHandle_t can_cli_task_handle = NULL;

#ifdef OS_FREERTOS
/* Task Communication */
QueueHandle_t queue_can_isr;
#endif

static int32_t can_cli_buffer_alloc(void);
static int32_t can_cli_buffer_ready(void);
static void can_cli_buffer_free(can_data_message_t *rx_msg);
static void can_cli_handle(void *params);
static void can_cli_rx_indication(uint32_t msg_id, uint32_t ide, uint32_t *data, uint32_t len);

static void can_transceiver_init(void)
{
        /* Setup CAN transceiver */
        gpio_set_direct(CAN_TRANS_EN_IO, 1);
        gpio_set_direct(CAN_TRANS_SB_IO, 1);
        gpio_write(CAN_TRANS_EN_IO, 1);
        gpio_write(CAN_TRANS_SB_IO, 1);
}

int32_t can_cli_init(void)
{
        int32_t result = E_OK;

#if (USE_CAN_FD == 1)
        result = can_init(0, CAN_BAUDRATE_500KBPS, CAN_BAUDRATE_1MBPS);
#else
        result = can_init(0, CAN_BAUDRATE_500KBPS, 0);
#endif
        if (E_OK != result) {
                EMBARC_PRINTF("CAN init failed.\r\n");
        } else {
                can_transceiver_init();
                /* Enable the RX interrupt */
                can_interrupt_enable(0, 0, 1);
                /* Enable the error interrupt */
                can_interrupt_enable(0, 3, 1);
#ifdef OS_FREERTOS
                /* Creat a queue of the CAN ISR */
                queue_can_isr = xQueueCreate(CAN_ISR_QUEUE_LENGTH, sizeof(uint32_t));
#endif
                if (pdPASS != xTaskCreate(can_cli_handle, \
                                        "can_cli_task", \
                                        256, \
                                        (void *)1, \
                                        (configMAX_PRIORITIES - 1), \
                                        &can_cli_task_handle)) {
                        EMBARC_PRINTF("create can_cli task error\r\n");
                }

                result = can_indication_register(0, can_cli_rx_indication);
                if (E_OK != result) {
                        EMBARC_PRINTF("indication register failed\r\n");
                }
        }

        for(uint32_t i = 0; i < CAN_CLI_RX_BUFFER_LEN; i++)
                rx_msg_buffer[i].frame_params = &rx_frame_params_buffer[i];

        return result;
}

int32_t can_cli_register(uint32_t msg_id, can_cli_callback func)
{
        int32_t result = E_OK;

        can_cli_item_t *item = NULL;

        uint32_t idx = 0;
        for (; idx < CAN_CLI_ITEM_CNT_MAX; idx++) {
                if (raw_spin_trylock(&can_cli_cmd_list[idx].lock)) {
                        item = &can_cli_cmd_list[idx];
                        break;
                }
        }

        if (NULL != item) {
                item->msg_id = msg_id;
                item->callback = func;
        } else {
                result = E_BOVR;
        }

        return result;
}


static void can_cli_rx_indication(uint32_t msg_id, uint32_t ide, uint32_t *data, uint32_t len)
{
        can_data_message_t *rx_msg = NULL;

        int32_t idx = can_cli_buffer_alloc();
        if ((idx >= 0)) {
                rx_msg = &rx_msg_buffer[idx];
        }

        if (NULL != rx_msg) {
                rx_msg->frame_params->eframe_format = ide;
                rx_msg->frame_params->len = (uint8_t)len;
                if (rx_msg->frame_params->eframe_format == eEXTENDED_FRAME) {
                        rx_msg->frame_params->msg_id = msg_id;
                } else {
                        rx_msg->frame_params->msg_id = msg_id >> 18;
                }
                transfer_bytes_stream(data, rx_msg->data, len);
        } else {
                /* buffer overflow: ignore the message. */
        }

}


static void can_cli_handle(void *params)
{
        can_data_message_t *rx_msg = NULL;
        can_cli_item_t *item = NULL;
#ifdef OS_FREERTOS
        uint32_t msg = 0;
#endif

        while (1) {
#ifdef OS_FREERTOS
                /* wait queue */
                if (xQueueReceive(queue_can_isr, &msg, portMAX_DELAY) == pdTRUE) {
#endif
                        /* get message. */
                        int32_t idx = can_cli_buffer_ready();

                        if (idx >= 0) {
                                rx_msg = &rx_msg_buffer[idx];
                        } else {
                                rx_msg = NULL;
                        }

                        if (NULL != rx_msg) {
                                item = NULL;
                                for (idx = 0; idx < CAN_CLI_ITEM_CNT_MAX; idx++) {
                                        if (rx_msg->frame_params->msg_id == can_cli_cmd_list[idx].msg_id) {
                                                item = &can_cli_cmd_list[idx];
                                                break;
                                        }
                                }

                                if (NULL != item) {
                                        if (item->callback) {
                                                item->callback(rx_msg->data, rx_msg->frame_params->len);
                                        }
                                } else {
                                        EMBARC_PRINTF("msg_id : 0x%x is not registered.\r\n", \
                                                        rx_msg->frame_params->msg_id);
                                }

                                can_cli_buffer_free(rx_msg);
                        } else {
                                taskYIELD();
                        }
#ifdef OS_FREERTOS
                }
#endif
        }
}

static int32_t can_cli_buffer_alloc(void)
{
        int32_t result = E_OK;

        if (rx_msg_fill_top >= CAN_CLI_RX_BUFFER_LEN) {
                rx_msg_fill_top = 0;
        }

        uint32_t cpu_status = arc_lock_save();
        if (rx_msg_buffer[rx_msg_fill_top].lock) {
                result = E_BOVR;
        } else {
                rx_msg_buffer[rx_msg_fill_top].lock = 1;
                result = rx_msg_fill_top++;
        }
        arc_unlock_restore(cpu_status);

        return result;
}

static int32_t can_cli_buffer_ready(void)
{
        int32_t result = -1;

        if (rx_msg_pop_top >= CAN_CLI_RX_BUFFER_LEN) {
                rx_msg_pop_top = 0;
        }
        if (rx_msg_buffer[rx_msg_pop_top].lock) {
                result = rx_msg_pop_top++;
        }

        return result;
}

static void can_cli_buffer_free(can_data_message_t *rx_msg)
{
        if (rx_msg) {
                rx_msg->lock = 0;
        }
}

