#ifndef _ALPS_IO_MUX_H_
#define _ALPS_IO_MUX_H_

#define IO_MUX_FUNC0		(0)
#define IO_MUX_FUNC1		(1)
#define IO_MUX_FUNC2		(2)
#define IO_MUX_FUNC3		(3)
#define IO_MUX_FUNC4		(4)
#define IO_MUX_FUNC_INVALID	(0xFF)


#define PIN_NAME_LEN_MAX	(16)
typedef struct pin_multiplex_descriptor {
	char group_name[PIN_NAME_LEN_MAX];
	uint8_t dev_id;
	uint8_t func;

	uint32_t reg_offset;
} io_mux_t;

#endif
