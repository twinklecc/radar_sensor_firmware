#ifndef _CRC_HAL_H_
#define _CRC_HAL_H_

#include "hw_crc.h"

int32_t crc_init(crc_poly_t poly, uint32_t mode);
int32_t crc32_update(uint32_t crc, uint32_t *data, uint32_t len);
int32_t crc16_update(uint32_t crc, uint16_t *data, uint32_t len);
uint32_t crc_output(void);
uint32_t crc_init_value(void);

#endif
