#ifndef _CASCADE_H_
#define _CASCADE_H_

typedef enum {
	CHIP_CASCADE_MASTER = 1,
	CHIP_CASCADE_SLAVE
} chip_cascade_status_t;

typedef enum {
	DEFAULT_FRAME = 0,
} cascade_frame_type_t;

/*
 * TODO: remove!
 * */
typedef enum {
	CASCADE_COM_2SPI = 0,
	CASCADE_COM_SPI_S2M_SYNC,
	CASCADE_COM_SPI_2SYNC,
	CASCADE_COM_SPI_DUPLEX
} cascade_com_type_t;

#define CASCADE_IF_2SPI_SIMPLEX
/*
#define CASCADE_IF_SPI_S2M_SYNC
#define CASCADE_IF_SPI_2SYNC
*/

#define CASCADE_TX_POLLING
#define CASCADE_TX_INT
#define CASCADE_TX_DMA


#if 0
typedef struct cascade_frame {
	//cascade_frame_type_t type;

	uint32_t chn_no:4;
	uint32_t reserved:4;
	uint32_t size:16;
	uint32_t sn:4;
	uint32_t no:4;

	uint32_t *payload;
	uint32_t crc;
} cascade_frame_t;
#endif

int32_t cascade_init(void);
void cascade_sync_bb_init(void);

//int32_t cascade_read(uint32_t **data, uint32_t *len);
int32_t cascade_read(uint32_t **data, uint32_t *len, TickType_t wait_ticks);
int32_t cascade_write(uint32_t *data, uint32_t len);
int32_t cascade_transmit_done(void);
void cascade_process_done(void);


#ifdef OS_FREERTOS
int32_t cascade_frame_receive(uint32_t **payload, uint32_t *length);
int32_t cascade_frame_update(uint32_t *payload, uint32_t length);
#endif

int32_t chip_cascade_status(void);
int32_t cascade_if_master_sync_bb_init(void);
int32_t cascade_if_slave_sync_bb_init(void);
int32_t cascade_s2m_sync_bb(void);

void cascade_write_buf_req();
void cascade_write_buf(uint32_t value);
void cascade_write_buf_done();
int32_t cascade_read_buf_req(TickType_t xTicksToWait); /* 1 tick 1ms*/
uint32_t cascade_read_buf(uint32_t offset);
void cascade_read_buf_done();

// command string tx/rx
void cascade_write_buf_str(const char *pcCommandString);
void cascade_read_buf_str();
char * cascade_spi_cmd_info(uint32_t *len);

#endif
