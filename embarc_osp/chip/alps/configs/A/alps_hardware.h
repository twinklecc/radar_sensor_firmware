/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */
/**
 *
 * \file
 * \ingroup	CHIP_ALPS_COMMON_INIT
 * \brief	alps hardware resource definitions
 */

/**
 * \addtogroup BOARD_ALPS_COMMON_INIT
 * @{
 */
#ifndef _ALPS_HARDWARE_H_
#define _ALPS_HARDWARE_H_

#include "arc_feature_config.h"

/** Initial CPU Clock Frequency definition */
#if defined(BOARD_INIT_CPU_FREQ)
    #define INIT_CLK_CPU  (BOARD_INIT_CPU_FREQ)
#else

#if (FLASH_XIP_EN)
    #define INIT_CLK_CPU  (300000000U)
#else
    #define INIT_CLK_CPU  (50000000U)
#endif

#endif

/** Initial Peripheral Bus Reference Clock definition */
#ifdef BOARD_INIT_DEV_FREQ
	/*!< Get peripheral bus reference clock defintion from build system */
	#define INIT_CLK_BUS_APB		(BOARD_INIT_DEV_FREQ)
#else
	/*!< Default peripheral bus reference clock defintion */
#if (FLASH_XIP_EN)
	#define INIT_CLK_BUS_APB		(400000000U)
#else
	#define INIT_CLK_BUS_APB		(50000000U)
#endif

#endif


/** CPU Clock Frequency definition */
#if defined(BOARD_CPU_FREQ)
	/*!< Get cpu clock frequency definition from build system */
	#define CLK_CPU			(BOARD_CPU_FREQ)
#elif defined(ARC_FEATURE_CPU_CLOCK_FREQ)
	/*!< Get cpu clock frequency definition from tcf file */
	#define CLK_CPU			(ARC_FEATURE_CPU_CLOCK_FREQ)
#else
	/*!< Default cpu clock frequency */
	#define CLK_CPU			(20000000)
#endif

/** Peripheral Bus Reference Clock definition */
#ifdef BOARD_DEV_FREQ
	/*!< Get peripheral bus reference clock defintion from build system */
	#define CLK_BUS_APB		(BOARD_DEV_FREQ)
#else
	/*!< Default peripheral bus reference clock defintion */
	#define CLK_BUS_APB		(50000000U)
#endif

#define ARC_FEATURE_DMP_PERIPHERAL     (0x00000000U)

/* Device Register Base Address */
#define REL_REGBASE_PINMUX		(0x00000000U)		/*!< PINMUX */
#define REL_REGBASE_SPI_MST_CS_CTRL	(0x00000014U)		/*!< SPI Master Select Ctrl */
#define REL_REGBASE_TIMERS		(0x00003000U)		/*!< DW TIMER */

#define REL_REGBASE_HDMA		(0x00200000)

#define REL_REGBASE_CLKGEN		(0x00B20000U)
#define REL_REGBASE_UART0		(0x00B30000U)		/*!< UART0 is connected to PMOD */
#define REL_REGBASE_UART1		(0x00B40000U)		/*!< UART1 is USB-UARTï¼?use UART1 as default */
#define REL_REGBASE_I2C0		(0x00B50000U)		/*!< I2C 0 */
#define REL_REGBASE_SPI0		(0x00B60000U)		/*!< SPI Master  */
#define REL_REGBASE_SPI1		(0x00B70000U)		/*!< SPI Master  */
#define REL_REGBASE_SPI2		(0x00B80000U)		/*!< SPI Slave  */
#define REL_REGBASE_QSPI		(0x00B90000U)		/*!< QSPI Master  */
#define REL_REGBASE_GPIO		(0x00BA0000U)		/*!< GPIO */
#define REL_REGBASE_GPIO0		(0x00BA0000U)		/*!< GPIO */
#define REL_REGBASE_CAN0		(0x00BB0000U)		/*!< CAN0 */
#define REL_REGBASE_CAN1		(0x00BC0000U)		/*!< CAN1 */
#define REL_REGBASE_FLASH		(0x00BD0000U)		/*!< FLASH mapping */
#define REL_REGBASE_WDT			(0x00BE0000U)		/*!< WDT  */

/* Interrupt Connection */
#define INTNO_TIMER0			16			/*!< ARC Timer0 */
#define INTNO_TIMER1			17			/*!< ARC Timer1 */
#define INTNO_SECURE_TIMER0		20			/*!< Core Secure Timer 0 */
#define INTNO_DMA_START			22			/*!< Core DMA Controller */
#define INTNO_DMA_COMPLETE		22			/*!< Core DMA Controller Complete */
#define INTNO_DMA_ERROR			23			/*!< Core DMA Controller Error */

#define INTNO_BB                        20                      /*!< Baseband IRQ */
#define INTNO_UART0			21			/*!< UART0 */
#define INTNO_UART1			22			/*!< UART1 */
#define INTNO_I2C0			23			/*!< I2C_0 controller */
#define INTNO_SPI_MASTER		24			/*!< SPI Master controller */
#define INTNO_SPI_MASTER0		24			/*!< SPI Master controller */
#define INTNO_SPI_MASTER1		25			/*!< SPI Master controller */
#define INTNO_SPI_SLAVE			26			/*!< SPI Slave controller */
#define INTNO_QSPI_MASTER		27			/*!< QSPI Master controller */
#define INTNO_CAN0                      28                      /*!< CAN0 */
#define INTNO_CAN1			29			/*!< CAN1 */
#define INTNO_FSM			30			/*!< Functional Safety */
#define INTNO_GPIO			31			/*!< GPIO controller */

/* SPI Mater Signals Usage */
#define	ALPS_SPI_LINE_0			0			/*!< CS0 -- Pmod 6 pin1 */
#define	ALPS_SPI_LINE_1			1			/*!< CS1 -- Pmod 5 pin1 or Pmod 6 pin 7 */
#define	ALPS_SPI_LINE_2			2			/*!< CS2 -- Pmod 6 pin8 */
#define	ALPS_SPI_LINE_SDCARD		3			/*!< CS3 -- On-board SD card  */
#define	ALPS_SPI_LINE_SPISLAVE		4			/*!< CS4 -- Internal SPI slave */
#define	ALPS_SPI_LINE_SFLASH		5			/*!< CS5 -- On-board SPI Flash memory */

#endif	/* _ALPS_HARDWARE_H_ */

/** @} end of group BOARD_ALPS_COMMON_INIT */
