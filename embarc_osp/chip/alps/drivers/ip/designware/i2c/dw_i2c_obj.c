#include "embARC_toolchain.h"
#include "alps/alps.h"
#include "dw_i2c.h"
#include "dw_i2c_obj.h"

static dw_i2c_t i2c_m = {
	.base = REL_REGBASE_I2C0,
	.int_no = INT_I2C_M_IRQ
};

void *i2c_get_dev(uint32_t id)
{
	static uint32_t i2c_install_flag = 0;

	if (0 == i2c_install_flag) {

		dw_i2c_install_ops(&i2c_m);
		i2c_install_flag = 1;

	}
	return (void *)&i2c_m;
}
