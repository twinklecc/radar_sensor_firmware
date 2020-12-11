#ifndef _CAN_BUF_CONFIG_H_
#define _CAN_BUF_CONFIG_H_

#define CAN_BUS_DEV_ID			(0)

#define CAN_BUS_FD			(0)

#define CAN_BUS_FD_BRS			(0)

/* 500 Kbps. */
#define CAN_BUS_BUAD_RATE		(500)

/* xxx bytes. */
#define CAN_BUS_FRAME_SIZE		(8)

#define CAN_BUS_FRAME_BUF_CNT_MAX	(64)

/* 0->CBFF, 1->CEFF, 2->FBFF, 3->FEFF. */
#define CAN_BUS_FRAME_FORMAT		(0)

#endif
