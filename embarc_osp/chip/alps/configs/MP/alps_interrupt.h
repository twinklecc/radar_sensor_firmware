#ifndef _ALPS_INTERRUPT_H_
#define _ALPS_INTERRUPT_H_

#define INT_ARC_WDG_TMO			(18)

#define INT_RF_ERROR_IRQ		(19)
#define INT_RF_ERROR_SS1		(20)
#define INT_DG_ERROR_IRQ		(21)
#define INT_DG_ERROR_SS1		(22)
#define INT_BB_ERR_ECC_SB_W		(23)
#define INT_FLASH_CTRL_ECC_ERR_SB	(24)

#define INT_MASK_IRQ0			(25)
#define INT_MASK_IRQ1			(26)
#define INT_MASK_IRQ2			(27)
#define INT_MASK_IRQ3			(28)
#define INT_MASK_IRQ4			(29)
#define INT_MASK_IRQ5			(30)
#define INT_MASK_IRQ6			(31)

#define INT_BB_IRQ_W0			(32)
#define INT_BB_IRQ_W1			(33)
#define INT_BB_IRQ_W2			(34)
#define INT_TIMER_INTR_0		(35)
#define INT_TIMER_INTR_1		(36)
#define INT_TIMER_INTR_2		(37)
#define INT_TIMER_INTR_3		(38)
#define INT_CAN_0_IRQ0			(39)
#define INT_CAN_0_IRQ1			(40)
#define INT_CAN_0_IRQ2			(41)
#define INT_CAN_0_IRQ3			(42)
#define INT_CAN_1_IRQ0			(43)
#define INT_CAN_1_IRQ1			(44)
#define INT_CAN_1_IRQ2			(45)
#define INT_CAN_1_IRQ3			(46)

#define INT_DMA_M0_IRQ0			(48)
#define INT_DMA_M0_IRQ1			(49)
#define INT_DMA_M0_IRQ2			(50)
#define INT_DMA_M0_IRQ3			(51)
#define INT_DMA_M0_IRQ4			(52)
#define INT_ASYNC_BRG0			(58)
#define INT_ASYNC_BRG1			(59)

#define INT_CRC_IRQ0			(64)
#define INT_CRC_IRQ1			(65)
#define INT_CRC_IRQ2			(66)
#define INT_UART0_IRQ			(67)
#define INT_UART1_IRQ			(68)
#define INT_SPI_M0_ERR_IRQ		(69)
#define INT_SPI_M0_RXF_IRQ		(70)
#define INT_SPI_M0_TXE_IRQ		(71)
#define INT_SPI_M1_ERR_IRQ		(72)
#define INT_SPI_M1_RXF_IRQ		(73)
#define INT_SPI_M1_TXE_IRQ		(74)
#define INT_SPI_S_ERR_IRQ		(75)
#define INT_SPI_S_RXF_IRQ		(76)
#define INT_SPI_S_TXE_IRQ		(77)
#define INT_QSPI_M_ERR_IRQ		(78)
#define INT_QSPI_M_RXF_IRQ		(79)
#define INT_QSPI_M_TXE_IRQ		(80)
#define INT_I2C_M_IRQ			(81)
#define INT_SW_IRQ0			(82)
#define INT_SW_IRQ1			(83)
#define INT_SW_IRQ2			(84)
#define INT_SW_IRQ3			(85)
#define INT_LVDS_TXO_IRQ		(86)

#define INT_PWM0_IRQ			(104)
#define INT_PWM1_IRQ			(105)
#define INT_PWM2_IRQ			(106)
#define INT_PWM3_IRQ			(107)
#define INT_PWM4_IRQ			(108)
#define INT_PWM5_IRQ			(109)
#define INT_PWM6_IRQ			(110)
#define INT_PWM7_IRQ			(111)
#define INT_GPIO0_IRQ			(112)
#define INT_GPIO1_IRQ			(113)
#define INT_GPIO2_IRQ			(114)
#define INT_GPIO3_IRQ			(115)
#define INT_GPIO4_IRQ			(116)
#define INT_GPIO5_IRQ			(117)
#define INT_GPIO6_IRQ			(118)
#define INT_GPIO7_IRQ			(119)
#define INT_GPIO8_IRQ			(120)
#define INT_GPIO9_IRQ			(121)
#define INT_GPIO10_IRQ			(122)
#define INT_GPIO11_IRQ			(123)
#define INT_GPIO12_IRQ			(124)
#define INT_GPIO13_IRQ			(125)
#define INT_GPIO14_IRQ			(126)
#define INT_GPIO15_IRQ			(127)

#define INTNO_GPIO		INT_GPIO0_IRQ
#define INTNO_GPIO_MAX		INT_GPIO15_IRQ
#define INTNO_DW_TIMER0		INT_TIMER_INTR_0
#define INTNO_DW_TIMER1		INT_TIMER_INTR_1
#define INTNO_DW_TIMER2		INT_TIMER_INTR_2
#define INTNO_DW_TIMER3		INT_TIMER_INTR_3
#define INTNO_CAN0_0		INT_CAN_0_IRQ0
#define INTNO_CAN0_1		INT_CAN_0_IRQ1
#define INTNO_CAN0_2		INT_CAN_0_IRQ2
#define INTNO_CAN0_3		INT_CAN_0_IRQ3
#define INTNO_CAN1_0		INT_CAN_1_IRQ0
#define INTNO_CAN1_1		INT_CAN_1_IRQ1
#define INTNO_CAN1_2		INT_CAN_1_IRQ2
#define INTNO_CAN1_3		INT_CAN_1_IRQ3
#define INTNO_DW_PWM0		INT_PWM0_IRQ
#define INTNO_DW_PWM1		INT_PWM1_IRQ

#define INTNO_CAN0		INT_CAN_0_IRQ1
#define INTNO_CAN1		INT_CAN_1_IRQ1

#define SPI_M0_ERR_INTR		INT_SPI_M0_ERR_IRQ
#define SPI_M0_RXF_INTR		INT_SPI_M0_RXF_IRQ
#define SPI_M0_TXE_INTR		INT_SPI_M0_TXE_IRQ

#define SPI_M1_ERR_INTR		INT_SPI_M1_ERR_IRQ
#define SPI_M1_RXF_INTR		INT_SPI_M1_RXF_IRQ
#define SPI_M1_TXE_INTR		INT_SPI_M1_TXE_IRQ

#define SPI_S_ERR_INTR		INT_SPI_S_ERR_IRQ
#define SPI_S_RXF_INTR		INT_SPI_S_RXF_IRQ
#define SPI_S_TXE_INTR		INT_SPI_S_TXE_IRQ

#define INTNO_UART0		(INT_UART0_IRQ)
#define INTNO_UART1		(INT_UART1_IRQ)

#define INTNO_TIMER0		(16)
#define INTNO_TIMER1		(17)

#define INT_BB_SAM      INT_BB_IRQ_W2
#define INT_BB_RESERVE  INT_BB_IRQ_W1
#define INT_BB_DONE     INT_BB_IRQ_W0

#define INT_WDG_TIMEOUT		INT_ARC_WDG_TMO
#endif
