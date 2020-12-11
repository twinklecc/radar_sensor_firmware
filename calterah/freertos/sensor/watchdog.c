#include "embARC.h"
#include "embARC_debug.h"
#include "embARC_assert.h"
#include "arc_wdg.h"

static void watchdog_isr(void *params)
{
	/* TODO: customer in future! */
	while (1);
}

void watchdog_init(void)
{
	int_disable(INT_WDG_TIMEOUT);

	int_handler_install(INT_WDG_TIMEOUT, watchdog_isr);

	arc_wdg_init(WDG_EVENT_INT, 0x100000);

	int_enable(INT_WDG_TIMEOUT);
}
