#ifndef _ALPS_INTERRUPT_H_
#define _ALPS_INTERRUPT_H_

#define DMA_IRQ_FLAG_0		19
#define BB_IRQ_W_0		20
#define UART_0_IRQ_W		21
#define UART_1_IRQ_W		22
#define I2C_M_IRQ_W		23
#define SPI_M0_TXE_INTR		24
#define SPI_M1_TXE_INTR		25
#define SPI_S_RXF_INTR		26
#define QSPI_M_TXE_INTR		27
#define CAN_0_IRQ0		28
#define CAN_1_IRQ0		29
#define RF_ERROR_IRQ_I		30
#define DMU_IRQ_W		31
#define DG_ERROR_IRQ_I		32
#define CRC_IRQ_2		33
#define CRC_IRQ_1		34
#define CRC_IRQ_0		35
#define DMA_M1_IRQ_FLAG_0	36
#define TIMER_INTR_3		37
#define TIMER_INTR_2		38
#define TIMER_INTR_1		39
#define TIMER_INTR_0		40
#define DMA_IRQ_FLAG_1		41
#define DMA_IRQ_FLAG_2		42
#define DMA_IRQ_FLAG_3		43
#define DMA_M1_IRQ_FLAG_1	44
#define DMA_M1_IRQ_FLAG_2	45
#define DMA_M1_IRQ_FLAG_3	46
#define SPI_M0_RXF_INTR		47
#define SPI_M0_ERR_INTR		48
#define SPI_M1_RXF_INTR		49
#define SPI_M1_ERR_INTR		50
#define SPI_S_TXE_INTR		51
#define SPI_S_ERR_INTR		52
#define QSPI_M_RXF_INTR		53
#define QSPI_M_ERR_INTR		54
#define CAN_0_IRQ1		55
#define CAN_0_IRQ2		56
#define CAN_0_IRQ3		57
#define CAN_1_IRQ1		58
#define CAN_1_IRQ2		59
#define CAN_1_IRQ3		60
#define GPIO_IRQ_0		61
#define GPIO_IRQ_1		62
#define GPIO_IRQ_2		63
#define GPIO_IRQ_3		64
#define GPIO_IRQ_4		65
#define GPIO_IRQ_5		66
#define GPIO_IRQ_6		67
#define GPIO_IRQ_7		68
#define GPIO_IRQ_8		69
#define GPIO_IRQ_9		70
#define GPIO_IRQ_10		71
#define GPIO_IRQ_11		72
#define GPIO_IRQ_12		73
#define GPIO_IRQ_13		74
#define GPIO_IRQ_14		75
#define GPIO_IRQ_15		76
#define SW_IRQ_W_0		77
#define SW_IRQ_W_1		78
#define SW_IRQ_W_2		79
#define SW_IRQ_W_3		80
#define PWM_INTR_1		81
#define PWM_INTR_0		82
#define BB_IRQ_W_1		83
#define BB_IRQ_W_2		84

#define INTNO_TIMER0		16			/*!< ARC TIMER0 */
#define INTNO_TIMER1		17			/*!< ARC TIMER1 */

#define INTNO_BB                20                      /*!< BASEBAND IRQ */
#define INTNO_BB_SAM_DONE       BB_IRQ_W_2              /*!< BASEBAND IRQ */
#define INTNO_UART0		21			/*!< UART0 */
#define INTNO_UART1		22			/*!< UART1 */
#define INTNO_I2C0		23			/*!< I2C_0 CONTROLLER */
#define INTNO_SPI_MASTER	24			/*!< SPI MASTER CONTROLLER */
#define INTNO_SPI_MASTER0	24			/*!< SPI MASTER CONTROLLER */
#define INTNO_SPI_MASTER1	25			/*!< SPI MASTER CONTROLLER */
#define INTNO_SPI_SLAVE		26			/*!< SPI SLAVE CONTROLLER */
#define INTNO_QSPI_MASTER	27			/*!< QSPI MASTER CONTROLLER */
#define INTNO_CAN0              28                      /*!< CAN0 */
#define INTNO_CAN1		29			/*!< CAN1 */
#define INTNO_FSM		30			/*!< FUNCTIONAL SAFETY */
#define INTNO_GPIO		GPIO_IRQ_0		/*!< GPIO CONTROLLER */
#define INTNO_DW_TIMER0		TIMER_INTR_0
#define INTNO_DW_TIMER1		TIMER_INTR_1
#define INTNO_DW_TIMER2		TIMER_INTR_2
#define INTNO_DW_TIMER3		TIMER_INTR_3
#define INTNO_CAN0_1            CAN_0_IRQ1              /*!< CAN0 1 */
#define INTNO_CAN0_2		CAN_0_IRQ2		/*!< CAN0 2 */
#define INTNO_CAN0_3		CAN_0_IRQ3		/*!< CAN0 3 */
#define INTNO_CAN1_1            CAN_1_IRQ1              /*!< CAN1 1 */
#define INTNO_CAN1_2		CAN_1_IRQ2		/*!< CAN1 2 */
#define INTNO_CAN1_3		CAN_1_IRQ3		/*!< CAN1 3 */
#define INTNO_DW_PWM0		PWM_INTR_0
#define INTNO_DW_PWM1		PWM_INTR_1
#endif
