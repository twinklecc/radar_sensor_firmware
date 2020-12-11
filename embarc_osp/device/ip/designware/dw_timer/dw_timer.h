#ifndef _DW_TIMER_H_
#define _DW_TIMER_H_


typedef struct dw_timer_desc {
	uint32_t base;

	uint32_t timer_cnt;
	uint8_t *int_no;

	void *ops;
} dw_timer_t;

typedef enum dw_timer_mode {
	FREE_RUNNING_MODE = 0,
	USER_DEFINED_MODE,
} timer_mode_t;

typedef struct dw_timer_operation {
	int32_t (*enable)(dw_timer_t *timer, uint32_t id, uint32_t en);
	int32_t (*load_count_update)(dw_timer_t *timer, uint32_t id, uint32_t count);
	int32_t (*load_count2_update)(dw_timer_t *timer, uint32_t id, uint32_t count);
	int32_t (*current_count)(dw_timer_t *timer, uint32_t id, uint32_t *cur);

	/* for each timer. */
	int32_t (*int_mask)(dw_timer_t *timer, uint32_t id, uint32_t mask);
	int32_t (*int_clear)(dw_timer_t *timer, uint32_t id);
	int32_t (*int_status)(dw_timer_t *timer, uint32_t id);

	/* for module. */
	int32_t (*mod_int_clear)(dw_timer_t *timer);
	int32_t (*mod_int_raw_status)(dw_timer_t *timer);
	int32_t (*mod_int_status)(dw_timer_t *timer);

	int32_t (*mode)(dw_timer_t *timer, uint32_t id, uint32_t mode);
	int32_t (*pwm)(dw_timer_t *timer, uint32_t id, uint32_t pwm_en, uint32_t pwm_0n100_en);

	uint32_t (*version)(dw_timer_t *timer);
} dw_timer_ops_t;

int32_t dw_timer_install_ops(dw_timer_t *timer);
#endif
