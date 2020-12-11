#ifndef _DW_GPIO_H_
#define _DW_GPIO_H_

typedef struct dw_gpio_descriptor {
	uint32_t base;

	uint32_t int_no;

	void *ops;
} dw_gpio_t;

typedef struct dw_gpio_operations {
	int32_t (*mode)(dw_gpio_t *dw_gpio, uint32_t gpio_id, uint32_t mode);
	int32_t (*set_direct)(dw_gpio_t *dw_gpio, uint32_t gpio_id, uint32_t direct);
	int32_t (*get_direct)(dw_gpio_t *dw_gpio, uint32_t gpio_id);
	int32_t (*write)(dw_gpio_t *dw_gpio, uint32_t gpio_id, uint32_t level);
	int32_t (*read)(dw_gpio_t *dw_gpio, uint32_t gpio_id);

	int32_t (*debounce)(dw_gpio_t *dw_gpio, uint32_t gpio_id, uint32_t enable);

	int32_t (*int_enable)(dw_gpio_t *dw_gpio, uint32_t gpio_id, uint32_t enable, uint32_t mask);
	int32_t (*int_polarity)(dw_gpio_t *dw_gpio, uint32_t gpio_id, uint32_t polarity);
	int32_t (*int_all_status)(dw_gpio_t *dw_gpio);
	int32_t (*int_status)(dw_gpio_t *dw_gpio, uint32_t gpio_id);
	int32_t (*int_raw_status)(dw_gpio_t *dw_gpio, uint32_t gpio_id);
	int32_t (*int_clear)(dw_gpio_t *dw_gpio, uint32_t gpio_id);

	int32_t (*version)(dw_gpio_t *dw_gpio);
} dw_gpio_ops_t;

typedef enum dw_gpio_port {
	DW_GPIO_PORT_A = 0,
	DW_GPIO_PORT_B,
	DW_GPIO_PORT_C,
	DW_GPIO_PORT_D
} dw_gpio_port_t;

typedef enum dw_gpio_direction {
	DW_GPIO_DIR_INPUT = 0,
	DW_GPIO_DIR_OUTPUT
} dw_gpio_direct_t;

typedef enum dw_gpio_mode {
	DW_GPIO_SW_MODE = 0,
	DW_GPIO_HW_MODE
} dw_gpio_mode_t;

typedef enum dw_gpio_interrupt_triggle_type {
	DW_GPIO_INT_HIGH_LEVEL_ACTIVE = 0,
	DW_GPIO_INT_LOW_LEVEL_ACTIVE,
	DW_GPIO_INT_RISING_EDGE_ACTIVE,
	DW_GPIO_INT_FALLING_EDGE_ACTIVE,
	DW_GPIO_INT_BOTH_EDGE_ACTIVE
} dw_gpio_int_active_t;


int32_t dw_gpio_install_ops(dw_gpio_t *dw_gpio);

#endif
