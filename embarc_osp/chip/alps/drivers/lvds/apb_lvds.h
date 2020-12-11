#ifndef _APB_LVDS_H_
#define _APB_LVDS_H_

#include "alps_clock.h"

#define LVDS_BASE 0xD10000


void lvds_dump_config(uint8_t dump_src);
void lvds_dump_start(uint8_t dump_src);
void lvds_dump_stop(void);

#endif
