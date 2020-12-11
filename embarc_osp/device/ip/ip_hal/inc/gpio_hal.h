#ifndef _GPIO_HAL_H_
#define _GPIO_HAL_H_

typedef void (*callback)(void *);

typedef enum gpio_interrupt_triggle_type {
	GPIO_INT_HIGH_LEVEL_ACTIVE = 0,
	GPIO_INT_LOW_LEVEL_ACTIVE,
	GPIO_INT_RISING_EDGE_ACTIVE,
	GPIO_INT_FALLING_EDGE_ACTIVE,
	GPIO_INT_BOTH_EDGE_ACTIVE
} gpio_int_active_t;


int32_t gpio_init(void);

/*
 * Description:
 *   set the direction of @gpio_no.
 * @dir: 0, input(DW_GPIO_DIR_INPUT); 1, output(DW_GPIO_DIR_OUTPUT)
 * */
int32_t gpio_set_direct(uint32_t gpio_no, uint32_t dir);

int32_t gpio_write(uint32_t gpio_no, uint32_t level);

int32_t gpio_read(uint32_t gpio_no);

void gpio_interrupt_clear(uint32_t gpio_no);
int32_t gpio_interrupt_enable(uint32_t gpio_no, uint32_t en);
int32_t gpio_int_unregister(uint32_t gpio_no);
int32_t gpio_int_register(uint32_t gpio_no, callback func, gpio_int_active_t type);
#endif
