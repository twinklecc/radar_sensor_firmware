#include "arc.h"
#include "arc_builtin.h"
#include "arc_wdg.h"


void arc_wdg_init(wdg_event_t event, uint32_t period)
{
	_arc_aux_write(AUX_WDG_PASSWD, AUX_WDG_PERIOD_WEN_MASK);
	_arc_aux_write(AUX_WDG_PERIOD, period);

	_arc_aux_write(AUX_WDG_PASSWD, AUX_WDG_CTRL_WEN_MASK);
	_arc_aux_write(AUX_WDG_CTRL, ((event & 0x3) << 1) | AUX_WDG_ENABLE);

	_arc_aux_write(AUX_WDG_PASSWD, AUX_WDG_WR_DIS_MASK);
}

void arc_wdg_deinit(void)
{
	_arc_aux_write(AUX_WDG_PASSWD, AUX_WDG_CTRL_WEN_MASK);
	_arc_aux_write(AUX_WDG_CTRL, AUX_WDG_DISABLE);
}

void arc_wdg_update(uint32_t period)
{
	if (_arc_aux_read(AUX_WDG_PASSWD) == AUX_WDG_PERIOD_AUTO_WEN_MASK) {
		_arc_aux_write(AUX_WDG_PERIOD, period);
	} else {
		_arc_aux_write(AUX_WDG_PASSWD, AUX_WDG_PERIOD_WEN_MASK);
		_arc_aux_write(AUX_WDG_PERIOD, period);
		_arc_aux_write(AUX_WDG_PASSWD, AUX_WDG_WR_DIS_MASK);
	}
}

void arc_wdg_status_clear(void)
{
	uint32_t val = _arc_aux_read(AUX_WDG_CTRL);
	val &= ~(1 << 3);
	_arc_aux_write(AUX_WDG_CTRL, val);
}

uint32_t arc_wdg_count(void)
{
	return _arc_aux_read(AUX_WDG_COUNT);
}
