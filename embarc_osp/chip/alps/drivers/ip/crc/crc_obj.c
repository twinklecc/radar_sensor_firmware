#include "embARC_toolchain.h"
#include "alps/alps.h"
#include "hw_crc.h"

static hw_crc_t crc_dev = {
	.base = REL_REGBASE_CRC,
	.err_int = INT_CRC_IRQ0,
	.complete_int = INT_CRC_IRQ1
};

void *crc_get_dev(uint32_t id)
{
	return (void *)&crc_dev;
}
