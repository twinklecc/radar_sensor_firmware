#ifndef BASEBAND_CAS_H
#define BASEBAND_CAS_H

#include "cascade.h"
#include "baseband.h"

#define SHIFT_L16(data)                   (data << 16)
// cascade
#define CMD_HDR                           0xFCFCFCFC /* command header code for spi tx/rx */
#define CMD_RX_WAIT                       1
#define CMD_SCAN_STOP                     0xAA01
#define WAIT_TICK_NUM                     30     /* ms*/

#define MAX_TX_OBJ_NUM                    256  // tx buf size for one transmit

#define BUF_SHIFT_INFO                    0
#define BUF_SIZE_INFO                     1
#define BUF_SIZE_CAFR                     2     /* rng 16bits + vel 16bits + amb_idx + noi = 2 words */
#define BUF_SIZE_FFT                      16    /* maximal 16 channels, 16 words */

#define BUF_SIZE_DUMMY                    15    /* FIXME, may be useless */

#define BUF_SIZE_ONE_OBJ                  (BUF_SIZE_CAFR + BUF_SIZE_FFT)
#define BUF_SIZE_CASCADE                  (MAX_TX_OBJ_NUM * BUF_SIZE_ONE_OBJ + BUF_SIZE_INFO + BUF_SIZE_DUMMY)
#define CMD_SIZE_CASCADE                  (16)  /* 16 words */



#ifdef CHIP_CASCADE
void baseband_scan_stop_tx(uint16_t cascade_cmd, uint16_t param);
void baseband_scan_stop_rx(TickType_t xTicksToWait);
void baseband_write_cascade_ctrl(void);
void baseband_merge_cascade(baseband_t *bb);
void baseband_write_cascade_cmd(const char *pcCommandString);
void baseband_read_cascade_cmd(TickType_t xTicksToWait);
uint32_t cascade_spi_cmd_wait(void);
#endif // CHIP_CASCADE

#endif
