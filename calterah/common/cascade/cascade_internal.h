#ifndef _CASCADE_INTERNAL_H_
#define _CASCADE_INTERNAL_H_

#include "frame.h"

#define CASCADE_BUFFER_SIZE		(20000 >> 2)
#define CASCADE_QUEUE_LEN		(3)

typedef enum {
	CS_XFER_INVALID = 0,
	CS_RX,
	CS_TX,
	CS_SLV_TX_DONE,
	CS_XFER
} cs_xfer_type_t;

typedef enum {
	CS_XFER_IDLE = 0,
	CS_XFER_LOCK,
	CS_XFER_BUSY,
	CS_XFER_CRC,
	CS_XFER_DONE,
	CS_XFER_PENDING
} cs_xfer_state_t;

typedef struct cascade_comm_config {
	uint8_t tx_mode;
	uint8_t rx_mode;
	//uint8_t frame_format;
	uint8_t hw_if_id;

	void *xfer_desc;
} cs_com_cfg_t;

typedef struct cascade_xfer_manager {
	//uint32_t buffer[CASCADE_BUFFER_SIZE];
	DEV_BUFFER devbuf;

	uint8_t crc_dma_chn_id;
	uint8_t crc_unmatch_cnt;

	cs_frame_t cur_frame;

	uint32_t *data;
	uint32_t xfer_size;

	uint8_t nof_frame;
	uint8_t cur_frame_id;
	uint8_t crc_done_cnt;
	uint8_t hw_if_mode;

	cs_xfer_state_t state;

	QueueHandle_t queue_handle;
	void (*done)(uint32_t *data, uint32_t len, int32_t result);
} cs_xfer_mng_t;


int32_t cascade_if_init(void);
int32_t cadcade_if_slave_init(uint8_t id);
int32_t cadcade_if_master_init(uint8_t id);

int32_t cascade_if_master_sync_init(void);
int32_t cascade_if_slave_sync_init(void);
int32_t cascade_spi_s2m_sync(void);

void cascade_receiver_init(uint32_t length);

void cascade_rx_indication(cs_xfer_mng_t *xfer);
void cascade_tx_confirmation(cs_xfer_mng_t *xfer);
void cascade_sync_callback(void *params);
void cascade_if_callback(void *params);

void cascade_if_master_package_init(cs_xfer_mng_t *xfer);
void cascade_if_slave_package_init(cs_xfer_mng_t *xfer);
int32_t cascade_sync_send_callback(cs_xfer_mng_t *xfer);
int32_t cascade_sync_receive_callback(cs_xfer_mng_t *xfer);
void cascade_if_master_send_callback(cs_xfer_mng_t *xfer);
void cascade_if_master_receive_callback(cs_xfer_mng_t *xfer);
void cascade_if_slave_send_callback(cs_xfer_mng_t *xfer);
void cascade_if_slave_receive_callback(cs_xfer_mng_t *xfer);

void cascade_session_state(uint8_t state);
int32_t cascade_in_xfer_session(cs_xfer_mng_t **xfer);
void cascade_if_xfer_resume(cs_xfer_mng_t *xfer, uint32_t length);
void cascade_if_master_xfer_resume(cs_xfer_mng_t *xfer, uint32_t length);
void cascade_if_slave_xfer_resume(cs_xfer_mng_t *xfer, uint32_t length);

void cascade_crc_init(void);
void cascade_crc_compute_done(uint32_t crc32);
void cascade_rx_crc_handle(cs_xfer_mng_t *xfer, uint32_t crc32);
void cascade_tx_crc_handle(cs_xfer_mng_t *xfer, uint32_t crc32);
void cascade_if_slave_rx_crc_done(cs_xfer_mng_t *xfer, uint32_t crc32);
void cascade_if_slave_tx_crc_done(cs_xfer_mng_t *xfer, uint32_t crc32);
void cascade_if_master_rx_crc_done(cs_xfer_mng_t *xfer, uint32_t crc32);
void cascade_if_master_tx_crc_done(cs_xfer_mng_t *xfer, uint32_t crc32);


int32_t cascade_if_master_write(cs_xfer_mng_t *xfer);
int32_t cascade_if_slave_write(cs_xfer_mng_t *xfer);

int32_t cascade_package_init(cs_xfer_mng_t *xfer, uint32_t *data, uint16_t len);
void cascade_rx_first_frame_init(cs_xfer_mng_t *xfer);
void cascade_rx_next_frame_init(cs_xfer_mng_t *xfer, uint32_t frame_len);
void cascade_package_done(cs_xfer_mng_t *xfer);
void cascade_tx_first_frame_init(cs_frame_t *frame, uint32_t *data, uint16_t len);
void cascade_tx_next_frame_init(cs_xfer_mng_t *xfer);

int32_t cascade_crc_dmacopy(uint32_t *data, uint32_t len);
void cascade_crc_request(uint32_t *data, uint32_t length);


#endif
