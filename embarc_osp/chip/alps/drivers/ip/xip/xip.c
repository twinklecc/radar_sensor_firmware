#include "embARC_toolchain.h"
#include "alps/alps.h"
#include "xip.h"

static xip_t flash_xip = {
	.base = REL_REGBASE_XIP,
};

void *xip_get_dev(void)
{
	return (void *)&flash_xip;
}
