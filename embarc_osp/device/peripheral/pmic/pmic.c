#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "embARC_toolchain.h"
#include "embARC.h"
#include "embARC_error.h"
#include "embARC_debug.h"
#include "arc_exception.h"
#include "board.h"

#include "i2c_hal.h"

static i2c_params_t xfer_params = {
	.addr_mode = 0,
	.speed_mode = 1,
	.slave_addr = 0xb0 >> 1,
};

void pmic_init(void)
{
	uint32_t ref_clock = clock_frequency(I2C_CLOCK);
	xfer_params.scl_h_cnt = ref_clock / 1000000;
	xfer_params.scl_l_cnt = ref_clock / 1000000;
	i2c_init(0, &xfer_params);
}
