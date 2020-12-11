#include "embARC.h"
#include "can_hal.h"

//#define __udstext	__attribute__ ((section(".uds_text")))
//void can_ota_main(void) __udstext;
void can_ota_main(void)
{
	volatile uint32_t test;
	for (test = 0; test < 10000; test++) {
		;
	}
}
