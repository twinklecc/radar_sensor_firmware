#include <string.h>
#include "embARC.h"

#define TRACE_INFO_SIZE		(4096)

typedef struct {
	uint8_t mod_id;
	uint8_t func_id;
	uint8_t pos;
	uint8_t error;

	uint32_t timestamp;
} trace_info_t;

static uint32_t trace_info_index = 0;
__attribute__ ((section (".trace_record"))) trace_info_t trace_ram[TRACE_INFO_SIZE];

void trace_record_error(uint8_t mod, uint8_t func, uint8_t pos, uint8_t error)
{
	uint32_t cpu_sts = arc_lock_save();
	if (trace_info_index < TRACE_INFO_SIZE) {
		trace_ram[trace_info_index].mod_id = mod;
		trace_ram[trace_info_index].func_id = func;
		trace_ram[trace_info_index].pos = pos;
		trace_ram[trace_info_index].error = error;
		trace_ram[trace_info_index].timestamp = _arc_aux_read(AUX_RTC_LOW);

		trace_info_index += 1;
	}

	if (trace_info_index >= TRACE_INFO_SIZE) {
		trace_info_index = 0;
	}

	arc_unlock_restore(cpu_sts);
}

void trace_init(void)
{
	memset(trace_ram, 0, TRACE_INFO_SIZE * sizeof(trace_info_t));
}
