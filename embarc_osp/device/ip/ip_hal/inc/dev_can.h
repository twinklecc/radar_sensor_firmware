/* ------------------------------------------
 * Copyright (c) 2017, Calterah, Inc. All rights reserved.
 *
--------------------------------------------- */


#ifndef _DEVICE_HAL_CAN_H_
#define _DEVICE_HAL_CAN_H_

#include "dev_common.h"


/*
 * defines for CAN baudrates
 */
#define CAN_BAUDRATE_100KBPS			(100000)
#define CAN_BAUDRATE_200KBPS			(200000)
#define CAN_BAUDRATE_250KBPS			(250000)
#define CAN_BAUDRATE_500KBPS			(500000)
#define CAN_BAUDRATE_1MBPS			(1000000)
#define CAN_BAUDRATE_2MBPS			(2000000)
#define CAN_BAUDRATE_4MBPS			(4000000)
#define CAN_BAUDRATE_5MBPS			(5000000)


typedef struct dev_can_cbs {
	DEV_CALLBACK tx_cb;	/*!< can data transmit callback */
	DEV_CALLBACK rx_cb;	/*!< can data receive callback */
	DEV_CALLBACK err_cb;	/*!< can error callback */
} DEV_CAN_CBS, *DEV_CAN_CBS_PTR;

struct can_frame_format_info {
	uint8_t xtd;
	uint8_t rtr;
	uint8_t fdf;
	uint8_t brs;

	uint8_t dlc;
};

struct can_id_info {
	uint32_t id_mode;
	uint32_t id_l;
	uint32_t id_h;
};

/**
 * \defgroup	DEVICE_HAL_CAN_DEVSTRUCT	CAN Device Interface Definition
 * \ingroup	DEVICE_HAL_CAN
 * \brief	Contains definitions of can device interface structure.
 * \details	This structure will be used in user implemented code, which was called
 *     \ref DEVICE_IMPL "Device Driver Implement Layer" for can to use in implementation code.
 *     Application developer should use the UART API provided here to access to can devices.
 *     BSP developer should follow the API definition to implement can device drivers.
 * @{
 */
/**
 * \brief	can information struct definition
 * \details	informations about can open count, working status,
 *     baudrate, can registers and ctrl structure.
 */
typedef struct dev_can_info {
	void *can_ctrl;	/*!< can control related pointer, implemented by bsp developer, and this should be set during can object implementation */
	uint32_t opn_cnt;	/*!< can open count, open it will increase 1, close it will decrease 1, 0 for close, >0 for open */
	uint32_t status;	/*!< current working status, refer to \ref DEVICE_HAL_COMMON_DEVSTATUS, this should be \ref DEV_ENABLED for first open */
	uint32_t baudrate;	/*!< can baud rate, this should be the value of baud passing by uart_open if first successfully opened */
	uint32_t rx_buf_element_size;
	uint32_t tx_buf_element_size;
	struct can_frame_format_info ff_info;
	struct can_id_info id_info;
	uint32_t data_or_frame; /* send or receive raw data or frame. 0 for frame, 1 for raw data. */
	DEV_BUFFER tx_buf;	/*!< transmit buffer via interrupt, this should be all zero for first open */
	DEV_BUFFER rx_buf;	/*!< receive buffer via interrupt, this should be all zero for first open */
	DEV_CAN_CBS can_cbs;	/*!< can callbacks, callback arguments should be \ref DEV_CAN * or NULL, this should be all NULL for first open */
	void *extra;		/*!< a extra pointer to get hook to applications which should not used by bsp developer,
					this should be NULL for first open and you can \ref DEV_CAN_INFO_SET_EXTRA_OBJECT "set"
					or \ref DEV_CAN_INFO_GET_EXTRA_OBJECT "get" the extra information pointer */
} DEV_CAN_INFO, * DEV_CAN_INFO_PTR;

/** Set extra information pointer of can info */
#define DEV_CAN_INFO_SET_EXTRA_OBJECT(can_info_ptr, extra_info)	(can_info_ptr)->extra = (void *)(extra_info)
/** Get extra information pointer of can info */
#define DEV_CAN_INFO_GET_EXTRA_OBJECT(can_info_ptr)			((can_info_ptr)->extra)

/**
 * \brief	can device interface definition
 * \details	Define can device interface, like can information structure,
 *  provide functions to open/close/control can, send/receive data by can
 * \note	All this details are implemented by user in user porting code
 */
typedef struct dev_can {
	DEV_CAN_INFO can_info;				/*!< can device information */
	int32_t (*can_open) (uint32_t baud);			/*!< Open can device */
	int32_t (*can_close) (void);				/*!< Close can device */
	int32_t (*can_control) (uint32_t ctrl_cmd, void *param);	/*!< Control can device */
	int32_t (*can_write) (const void *data, uint32_t len);	/*!< Send data by can device(blocked) */
	int32_t (*can_read) (void *data, uint32_t len);	/*!< Read data from can device(blocked) */
} DEV_CAN, * DEV_CAN_PTR;



#define DEV_SET_CAN_SYSCMD(cmd)		DEV_SET_SYSCMD((cmd))

#define CAN_CMD_FD_ENABLE			DEV_SET_CAN_SYSCMD(0)
#define CAN_CMD_FD_DISABLE			DEV_SET_CAN_SYSCMD(1)

/* bit rate switch. */
#define CAN_CMD_BRS_ENABLE			DEV_SET_CAN_SYSCMD(2)
#define CAN_CMD_BRS_DISABLE			DEV_SET_CAN_SYSCMD(3)

/* transmission delay conpensation. */
#define CAN_CMD_SET_TDC				DEV_SET_CAN_SYSCMD(4)
#define CAN_CMD_GET_TDC				DEV_SET_CAN_SYSCMD(5)
#define CAN_CMD_TDC_DISABLE			DEV_SET_CAN_SYSCMD(6)

/* auto re-transmission. */
#define CAN_CMD_ART_ENABLE			DEV_SET_CAN_SYSCMD(7)
#define CAN_CMD_ART_DISABLE			DEV_SET_CAN_SYSCMD(8)

#define CAN_CMD_MODE_ENABLE			DEV_SET_CAN_SYSCMD(9)
#define CAN_CMD_MODE_DISABLE			DEV_SET_CAN_SYSCMD(10)

#define CAN_CMD_RESET				DEV_SET_CAN_SYSCMD(11)

/* timestamp counter mode. using enum CAN_TIMESTAMP_COUNTER_MODE. */
#define CAN_CMD_SET_TSCM			DEV_SET_CAN_SYSCMD(12)
#define CAN_CMD_GET_TSC				DEV_SET_CAN_SYSCMD(13)

/* Data (Re)Synchronization Jump Width. */
#define CAN_CMD_SET_DSJW			DEV_SET_CAN_SYSCMD(14)

#define CAN_CMD_SET_BAUD 			DEV_SET_CAN_SYSCMD(15)

/* reject remote frame. */
#define CAN_CMD_SET_RRFE			DEV_SET_CAN_SYSCMD(16)
#define CAN_CMD_UNSET_RRFE 			DEV_SET_CAN_SYSCMD(17)

/* ID filter size.  */
#define CAN_CMD_SET_IDFS			DEV_SET_CAN_SYSCMD(18)

/* extend id mask. */
#define CAN_CMD_SET_XIDAMR 			DEV_SET_CAN_SYSCMD(19)

/* interrupt. */
#define CAN_CMD_INT_ENABLE 			DEV_SET_CAN_SYSCMD(20)
#define CAN_CMD_INT_DISABLE			DEV_SET_CAN_SYSCMD(21)
#define CAN_CMD_SET_RXCB			DEV_SET_CAN_SYSCMD(22)
#define CAN_CMD_SET_TXCB			DEV_SET_CAN_SYSCMD(23)
#define CAN_CMD_SET_ERRCB			DEV_SET_CAN_SYSCMD(24)

#define CAN_CMD_SET_TX_BUF			DEV_SET_CAN_SYSCMD(25)
#define CAN_CMD_SET_RX_BUF			DEV_SET_CAN_SYSCMD(26)

//#define CAN_CMD_SET_FRAME			DEV_SET_CAN_SYSCMD(27)


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	TIMESTAMP_COUNTER_ALWAYS_0 = 0x0,
	TIMESTAMP_COUNTER_INCREMENT,
	TIMESTAMP_COUNTER_EXTERNAL
} CAN_TIMESTAMP_COUNTER_MODE;

/* CMD: CAN_CMD_MODE_ENABLE/DISABLE. */
#define CAN_SLEEP_MODE				(1)
#define CAN_LOOP_BACK_MODE			(2)
#define CAN_BUS_MONITORING_MODE		(3)
#define CAN_RESTRICTED_MODE			(4)

typedef enum {
	BUFFER_DATA_FIELD_8_BYTE = 0,
	BUFFER_DATA_FIELD_12_BYTE,
	BUFFER_DATA_FIELD_16_BYTE,
	BUFFER_DATA_FIELD_20_BYTE,
	BUFFER_DATA_FIELD_24_BYTE,
	BUFFER_DATA_FIELD_32_BYTE,
	BUFFER_DATA_FIELD_48_BYTE,
	BUFFER_DATA_FIELD_64_BYTE
} CAN_BUFFER_DATA_FIELD_SIZE;

/* can_control: CAN_CMD_INT_ENABLE&CAN_CMD_INT_DSIABLE command parameter. */
#define CAN_INTERRUPT_PED		(1 << 10)
#define CAN_INTERRUPT_PEA		(1 << 9)
#define CAN_INTERRUPT_BO		(1 << 8)
#define CAN_INTERRUPT_EW		(1 << 7)
#define CAN_INTERRUPT_EP		(1 << 6)
#define CAN_INTERRUPT_ELO		(1 << 5)
#define CAN_INTERRUPT_BEU		(1 << 4)
#define CAN_INTERRUPT_BEC		(1 << 3)
#define CAN_INTERRUPT_TCF		(1 << 2)
#define CAN_INTERRUPT_TC		(1 << 1)
#define CAN_INTERRUPT_MRX		(1 << 0)
#define CAN_INTERRUPT_ERROR_FLAG		(CAN_INTERRUPT_PED | CAN_INTERRUPT_PEA | CAN_INTERRUPT_EW | \
									 CAN_INTERRUPT_EP | CAN_INTERRUPT_ELO | CAN_INTERRUPT_BEU | \
									 CAN_INTERRUPT_BEC | CAN_INTERRUPT_BO)
#define CAN_INTERRUPT_TRANSMIT_FLAG		(CAN_INTERRUPT_TCF | CAN_INTERRUPT_TC)
#define CAN_INTERRUPT_RECEIVE_FLAG		(CAN_INTERRUPT_MRX)


/*
* CAN_INT_PED: Protocol Error in Data Phase.
* CAN_INT_PEA: Protocol Error in Arbitration Phase.
* CAN_INT_BO: Bus_Off Status.
* CAN_INT_EW: Warning Status.
* CAN_INT_EP: Error Passive.
* CAN_INT_ELO: Error Logging Overflow.
* CAN_INT_BEU: Bit Error Uncorrected.
* CAN_INT_BEC: Bit Error Corrected.
* CAN_INT_TCF: Transmission Cancellation.
* CAN_INT_TC: Transmission Completed.
* CAN_INT_MRX: Message stored to RX Buffer.
******************************************************/
typedef enum {
	CAN_INT_MRX = 0,
	CAN_INT_TC,
	CAN_INT_TCF,
	CAN_INT_BEC,
	CAN_INT_BEU,
	CAN_INT_ELO,
	CAN_INT_EP,
	CAN_INT_EW,
	CAN_INT_BO,
	CAN_INT_PEA,
	CAN_INT_PED
} CAN_INTERRUPT_TYPE;

#ifdef CHIP_ALPS_A
extern DEV_CAN_PTR can_get_dev(int32_t can_id);
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* _DEVICE_HAL_UART_H_ */
