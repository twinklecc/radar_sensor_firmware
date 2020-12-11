#ifndef _CAN_SIGNAL_INTERFACE_H_
#define _CAN_SIGNAL_INTERFACE_H_

#include "track.h"

#define SENSOR_ID                           0

/* CAN Message ID */
#define SCAN_FRAME_ID                       (0x200 + SENSOR_ID * 0x10)
#define HEAD_FRAME_ID0                      (0x300 + SENSOR_ID * 0x10)
#define HEAD_FRAME_ID1                      (0x301 + SENSOR_ID * 0x10)
#define BK_FRAME_ID0                        (0x400 + SENSOR_ID * 0x10)
#define BK_FRAME_ID1                        (0x401 + SENSOR_ID * 0x10)
#define BK_FRAME_ID2                        (0x402 + SENSOR_ID * 0x10)    /* Reserved */
#define AK_FRAME_ID0                        (0x500 + SENSOR_ID * 0x10)
#define AK_FRAME_ID1                        (0x501 + SENSOR_ID * 0x10)
#define AK_FRAME_ID2                        (0x502 + SENSOR_ID * 0x10)    /* Reserved */

/* CAN OTA Message ID */
#define CAN_OTA_ID                          (0x111 + SENSOR_ID * 0x10)
/* CAN OTA Magic code and HS code */
#define CAN_OTA_COM_MAGIC_NUM               (0x341200ff)
#define CAN_OTA_COM_HS_CODE                 (0xff002143)

void track_pkg_can_print(track_t *track);
int32_t can_scan_signal(uint8_t *data, uint32_t len);
int32_t can_cli_commands(void);

#endif /* _CAN_SIGNAL_INTERFACE_H_ */

