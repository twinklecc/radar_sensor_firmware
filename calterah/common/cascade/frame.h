#ifndef _CASCADE_FRAME_H_
#define _CASCADE_FRAME_H_

#define CASCADE_HEADER_MAGIC			(0x4373)

#define CASCADE_PACKAGE_LEN_MAX			(0x5000)
#define CASCADE_FRAME_LEN_MAX			(0x1000)
#define CASCADE_FRAME_PART_LEN_MAX		(0x20)

#define CASCADE_DEF_FRAME_SIZE(header)		(((header) >> 8) & 0xFFFF)
#define CASCADE_DEF_FRAME_NUMBER(header)	(((header) >> 28) & 0xF)
#define CASCADE_DEF_FRAME_SN(header)		(((header) >> 24) & 0xF)

/* word. */
#define CASCADE_CRC32_LEN			(1)
#define CASCADE_HEADER_LEN			(1)

#define CASCADE_FRAME_CNT_MAX			(5)
typedef struct {
	uint32_t header;
	uint32_t *payload;
	uint32_t length;
	uint32_t crc32;
} cascade_frame_format_t;

typedef struct cascade_frame {
	uint32_t header;
	uint32_t *payload;
	uint16_t len;
	uint16_t handled_size;

	uint32_t crc32;
	uint8_t crc_valid;
	uint8_t crc_done;
	uint8_t header_done;
	uint8_t payload_done;
} cs_frame_t;

#define FRAME_PART_BASE(frame)	\
	((frame)->payload + (frame)->handled_size)
#define FRAME_PART_SIZE(frame) \
	((((frame)->len - (frame)->handled_size) > CASCADE_FRAME_PART_LEN_MAX) ? (CASCADE_FRAME_PART_LEN_MAX) : ((frame)->len - (frame)->handled_size))

typedef struct {
	uint32_t nof_frame;
	uint32_t cur_frame_no;

	cascade_frame_format_t frame[CASCADE_FRAME_CNT_MAX];
} cascade_tx_runtime_t;

typedef struct {
	/* idle, in-progress, full. */
	//uint32_t status;

	uint32_t handled_size;

	uint32_t data[(CASCADE_FRAME_LEN_MAX >> 2) + 2];
} frame_buffer_t;

typedef struct {
	uint32_t length;
	uint32_t received_size;
	uint32_t *payload;
	uint32_t crc32;
} cascade_frame_info_t;

typedef struct {
	uint32_t total_len;
	//uint32_t nof_frame;
	uint32_t received_size;

	cascade_frame_info_t cur_frame;

	frame_buffer_t buffer[2];
} cascade_rx_runtime_t;

#define CASCADE_HEADER_VALID(header)	(((header) >> 16) == (CASCADE_HEADER_MAGIC))
#define CASCADE_FRAME_LEN(header)	((header) & 0xFFFF)
#define CASCADE_PACKAGE_LEN(header)	((header) & 0xFFFF)


void cascade_frame_part_update(cs_frame_t *frame, uint32_t split_cnt);
void cascade_frame_reset(cs_frame_t *frame);


#endif
