#ifndef _ARC_WDG_H_
#define _ARC_WDG_H_

typedef enum {
	WDG_EVENT_TIMEOUT = 0,
	WDG_EVENT_INT,
	WDG_EVENT_RESET
} wdg_event_t;

void arc_wdg_init(wdg_event_t event, uint32_t period);
void arc_wdg_deinit(void);
void arc_wdg_update(uint32_t period);
uint32_t arc_wdg_count(void);

#endif
