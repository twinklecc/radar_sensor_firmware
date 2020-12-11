#ifndef _CAN_HAL_H_
#define _CAN_HAL_H_

#include "dev_common.h"

#include "can.h"

/* Possible Values: 8, 12, 16, 20, 24, 32, 48, 64. In Bytes.*/
#if (USE_CAN_FD == 1)
#define CAN_FRAM_BUF_DATA_SIZE (16)
#else
#define CAN_FRAM_BUF_DATA_SIZE (8)
#endif

typedef struct {
        uint32_t msg_id;					/* message identifier */
        eCAN_FRAME_FORMAT eframe_format;	/* frame format: 0 is standard frame, 1 is extended frame */
        eCAN_FRAME_TYPE eframe_type;		/* frame type:   0 is data frame, 1 is remote frame */
        uint32_t len;						/* Length of Message Data Field , in Bytes. */
} can_frame_params_t;

typedef struct can_data_message {
        can_frame_params_t *frame_params;
        uint8_t  data[CAN_FRAM_BUF_DATA_SIZE];
        uint32_t lock;
} can_data_message_t;

typedef enum{
        eDATA_LEN_8  = 8,
        eDATA_LEN_12 = 12,
        eDATA_LEN_16 = 16,
        eDATA_LEN_20 = 20,
        eDATA_LEN_24 = 24,
        eDATA_LEN_32 = 32,
        eDATA_LEN_48 = 48,
        eDATA_LEN_64 = 64
} eCAN_DATA_LEN;


typedef enum{
        eDATA_DLC_8  = 8,
        eDATA_DLC_9,
        eDATA_DLC_10,
        eDATA_DLC_11,
        eDATA_DLC_12,
        eDATA_DLC_13,
        eDATA_DLC_14,
        eDATA_DLC_15
} eCAN_DLC;


#ifdef OS_FREERTOS
#define CAN_ISR_QUEUE_LENGTH 64
#endif

typedef void (*rx_indication_callback)(uint32_t msg_id, uint32_t ide, uint32_t *data, uint32_t len);
int32_t can_init(uint32_t id, uint32_t nomi_baud, uint32_t data_baud);
int32_t can_config(uint32_t id, void *param);

int32_t can_read(uint32_t id, uint32_t *buf, uint32_t len);
int32_t can_write(uint32_t id, uint32_t *buf, uint32_t len, uint32_t flag);

int32_t can_interrupt_enable(uint32_t id, uint32_t type, uint32_t enable);

int32_t can_xfer_callback_install(uint32_t id, void (*func)(void *));

#ifdef OS_FREERTOS
int32_t can_queue_install(uint32_t id, QueueHandle_t queue, uint32_t rx_or_tx);
#endif
void *can_xfer_buffer(uint32_t id, uint32_t rx_or_tx);

int32_t can_indication_register(uint32_t dev_id, rx_indication_callback func);
int32_t can_receive_data(uint32_t dev_id, can_data_message_t *msg);
void can_send_data(uint32_t bus_id, uint32_t frame_id, uint32_t *data, uint32_t len);

static inline int32_t can_get_dlc(uint32_t len);
static inline uint32_t can_get_datalen(uint32_t dlc);


/*************************Function Begin*************************
* Function Name: swap_hlbyte
*
* Description: Firstly, the uint32_t data will be exchanged high and low byte, 
*              then it will be stored as uint8_t data.
*
* Inputs: Parameter 1 is a pointer to the data type uint32_t
*         Parameter 3 is the number of data in parameter 1
*         That is how many uint32_t data needs to be stored as uint8_t data.
* Outputs: Parameter 2 is a pointer to the data type uint8_t
*
*************************Function End***************************/
static inline void swap_hlbyte(uint32_t *data, uint8_t *msg_data, uint32_t len)
{
        int32_t i = 0, j = 0, n = (len / 4);
        int32_t temp = 0;

        for (i = 0; i < n / 2; i++)
        {
                temp = data[i];
                data[i] = data[n - i - 1];
                data[n - i - 1] = temp;
        }

        while (n--) {
                for (i = 3; i >= 0; i--) {
                        msg_data[j] = (uint8_t)(*data >> (i << 3));
                        j++;
                }
                data++;
        }
}


/*************************Function Begin*************************
* Function Name: transfer_bytes_stream
*
* Description: The uint32_t data will be stored as uint8_t data.
*
* Inputs: Parameter 1 is a pointer to the data type uint32_t
*         Parameter 3 is the number of data in parameter 2
*         That is how many uint32_t data needs to be stored as uint8_t data.
* Outputs: Parameter 2 is a pointer to the data type uint8_t
*
*************************Function End***************************/
static inline void transfer_bytes_stream(uint32_t *src, uint8_t *dst, uint32_t len)
{
        int32_t i = 0, j = 0, n = (len / sizeof(uint32_t));

        while (n--) {
                for (i = 0; i < sizeof(uint32_t); i++) {
                        dst[j] = (uint8_t)(*src >> (i << 3));
                        j++;
                }
                src++;
        }
}


/*************************Function Begin*************************
* Function Name: transfer_word_stream
*
* Description: The uint8_t data will be stored as uint32_t data.
*
* Inputs: Parameter 1 is a pointer to the data type uint8_t
*         Parameter 3 is the number of data in parameter 1
*         That is how many uint8_t data needs to be stored as uint32_t data.
* Outputs: Parameter 2 is a pointer to the data type uint32_t
*
*************************Function End***************************/

static inline void transfer_word_stream(uint8_t *src, uint32_t *dst, uint32_t len)
{
        int32_t i = 0, n = (len / sizeof(uint32_t));

        while (n--) {
                *dst = 0;
                for (i = 0; i < sizeof(uint32_t); i++) {
                        *dst |= (uint32_t)(*src++ << (i << 3));
                }
                dst++;
        }
}
#endif
