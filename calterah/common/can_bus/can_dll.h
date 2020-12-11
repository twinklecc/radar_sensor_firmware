#ifndef _CAN_DLL_H_
#define _CAN_DLL_H_

typedef enum can_frame_format {
	CAN_CBFF = 0,
	CAN_CEFF,
	CAN_FBFF,
	CAN_FEFF
} can_frame_format_t;

typedef enum can_dll_tx_status {
	CAN_XFER_DONE = 0,
	CAN_XFER_ERROR,
	/* TODO: dll need timer ? CAN_XFER_TIMEOUT, */
	can_XFER_PREPARE,
	CAN_XFER_BUF_FILLING,
	CAL_XFER_TRANSFERING
} can_dll_xfer_sts_t;

typedef int32_t (*dll_indication)(uint32_t id, uint32_t dlc, uint8_t *data);
typedef int32_t (*dll_confirm)(int32_t id, uint32_t xfer_status);
typedef int32_t (*dll_remote_indication)(uint32_t id, uint32_t dlc);
typedef int32_t (*dll_remote_confirm)(int32_t id, uint32_t xfer_status);

int32_t can_dll_init(void);
int32_t can_dll_request(uint32_t id, uint32_t dlc, uint8_t *data);
int32_t can_dll_remote_request(uint32_t id, uint32_t dlc);

int32_t can_dll_indication_register(dll_indication func);
int32_t can_dll_confirm_register(dll_confirm func);

int32_t can_dll_remote_indication_register(dll_remote_indication func);
int32_t can_dll_remote_confirm_register(dll_remote_confirm func);

#endif
