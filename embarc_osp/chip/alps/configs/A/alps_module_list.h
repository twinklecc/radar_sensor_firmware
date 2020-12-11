#ifndef ALPS_B_MODULE_LIST_H_
#define ALPS_B_MODULE_LIST_H_

typedef enum alps_module_id {
	XIP = 0,
	BASEBAND,
	UART0,
	UART1,
	I2C,
	SPI_M0,
	SPI_M1,
	SPI_S,
	QSPI,
	GPIO,
	CAN0,
	CAN1,
	DMA,
	CRC,
	TIMER,
	DMU,
	PWM
} module_id_t;

#endif
