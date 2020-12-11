#ifndef _DATA_STREAM_H_
#define _DATA_STREAM_H_


#ifdef OS_FREERTOS
#include "FreeRTOS.h"
#include "queue.h"
#endif

enum io_stream_data_source {
    DATA_FROM_HOST = 0,
    DATA_FROM_MASTER,
    DATA_FROM_SLAVER,
    DATA_FROM_EXTRA_SENSOR
};

enum io_stream_data_target {
    LOG_TO_HOST = 0,
    DATA_TO_HOST,
    DATA_TO_MASTER,
    DATA_TO_SLAVER,
    DATA_TO_EXTRA_SENSOR
};

enum data_stream_type {
    COMMAND_STREAM = 0,
    DATA_STREAM,
    UNKOWN_STREAM
};

enum chip_cascade_mode {
    NO_CASCADE = 0,
    CASCADE_MASTER,
    CASCADE_SLAVER_ONLY,
    CASCADE_SLAVER_MASTER
};

enum frame_id_mode {
    FRAME_ID_FIX = 0,
    FREAM_ID_INC
};

enum frame_type {
    CAN_STD_RTR_FRAME = 0,
    CAN_STD_FRAME,
    CAN_XTD_RTR_FRAME,
    CAN_XTD_FRAME,
    CAN_FD_RTR_FRAME,
    CAN_FD_FRAME,
    CAN_FD_BRS_FRAME
};

struct io_transmit_flag {
	uint8_t data_or_frame;
	uint8_t malloc;
};

int32_t io_stream_init(void);
int32_t io_stream_receive_data(enum io_stream_data_source source, void *data_buf, uint32_t count);

/**
 * io_stream_send_data: send raw or naked data.
 * @return: < 0, error code.
 * @target: <IN>, indicate where the data will be sent to.
 * @data_buf: <IN>, container stores the data.
 * @count: <IN>, the amount of the data.
 * @flag: reserved.
 **/
int32_t io_stream_send_data(enum io_stream_data_target target, void *data_buf, uint32_t count);
int32_t io_stream_malloc_send_data(enum io_stream_data_target target, void *data_buf, uint32_t count);

#if 0
/**
 * io_stream_send_frame: send special frame made by user.
 * @return: < 0, error code.
 * @target: <IN>, indicate where the data will be sent to.
 * @data_buf: <IN>, container stores the data.
 * @count: <IN>, the amount of the data.
 * @flag: reserved.
 **/
int32_t io_stream_send_frame(enum io_stream_data_target target, void *data_buf, uint32_t count);
int32_t io_stream_malloc_send_frame(enum io_stream_data_target target, void *data_buf, uint32_t count);
#endif

#ifdef OS_FREERTOS
QueueHandle_t io_stream_get_cascade_queuehandle(enum io_stream_data_source source);
QueueHandle_t io_stream_get_cli_queuehandle(void);
#endif


#endif
