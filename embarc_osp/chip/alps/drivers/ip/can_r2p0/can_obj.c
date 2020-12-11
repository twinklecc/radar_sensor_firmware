#include "embARC_toolchain.h"
#include "alps/alps.h"
#include "can.h"
#include "embARC_error.h"
#include "embARC.h"
#include "embARC_debug.h"

static can_t can_r2p0_0 = {
	.base = REL_REGBASE_CAN0,
	.int_line = {INTNO_CAN0_0, INTNO_CAN0_1, INTNO_CAN0_2, INTNO_CAN0_3},
};

static can_t can_r2p0_1 = {
	.base = REL_REGBASE_CAN1,
	.int_line = {INTNO_CAN1_0, INTNO_CAN1_1, INTNO_CAN1_2, INTNO_CAN1_3},
};

static void can_resource_install(void)
{
	can_install_ops(&can_r2p0_0);
	can_install_ops(&can_r2p0_1);
}

void *can_get_dev(uint32_t id)
{
	static uint32_t can_install_flag = 0;
	void *return_ptr;

	if (0 == can_install_flag) {
		can_resource_install();
		can_install_flag = 1;
	}

	switch (id) {
		case 0:
			return_ptr = (void *)&can_r2p0_0;
			break;
		case 1:
			return_ptr = (void *)&can_r2p0_1;
			break;
		default:
			return_ptr = NULL;
	}

	return return_ptr;
}
