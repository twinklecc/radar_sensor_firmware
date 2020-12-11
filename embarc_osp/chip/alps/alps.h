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
 * \brief	alps resource definitions
 */

/**
 * \addtogroup CHIP_ALPS_COMMON_INIT
 * @{
 */
#ifndef _ALPS_H_
#define _ALPS_H_

#include "arc_em.h"
#include "arc_builtin.h"

#ifdef CHIP_ALPS_A
#include "drivers/ip/designware/iic/dw_iic_obj.h"
#include "drivers/ip/designware/spi/dw_spi_obj.h"
#include "drivers/ip/designware/uart/dw_uart_obj.h"
#else
#include "drivers/ip/designware/ssi/dw_ssi_obj.h"
#include "drivers/ip/designware/dw_timer/dw_timer_obj.h"
#include "drivers/ip/designware/dw_uart/dw_uart_obj.h"
#endif
#ifdef CHIP_ALPS_A
#include "drivers/ip/gpio/gpio_obj.h"
#include "drivers/ip/can/can_obj.h"
#include "drivers/ip/designware/qspi/dw_qspi_obj.h"
#else
#include "drivers/ip/designware/gpio/dw_gpio_obj.h"
#include "drivers/ip/can_r2p0/can_obj.h"
#ifdef CHIP_ALPS_B
#include "drivers/ip/designware/dw_qspi/dw_qspi_obj.h"
#endif
#include "alps_otp_mmap.h"
#endif
#include "drivers/ip/designware/i2c/dw_i2c_obj.h"
#include "drivers/ip/xip/xip_obj.h"
#include "drivers/ip/crc/crc_obj.h"
#include "drivers/ip/designware/ahb_dmac/dw_dma_obj.h"

#include "common/alps_timer.h"

#include "alps_hardware.h"
#include "alps_module_list.h"
#include "alps_clock.h"
#include "alps_reset_api.h"
#include "clkgen.h"
#include "mux.h"
#include "alps_dmu.h"
#include "alps_emu.h"
#include "dmu.h"


#ifdef ARC_FEATURE_DMP_PERIPHERAL
#define PERIPHERAL_BASE		ARC_FEATURE_DMP_PERIPHERAL
#else
#define PERIPHERAL_BASE		_arc_aux_read(AUX_DMP_PERIPHERAL)
#endif

/* common macros must be defined by all chips */

#define CHIP_CONSOLE_UART_ID		DW_UART_0_ID

#ifdef PLAT_ENV_FPGA
#ifdef CHIP_CONSOLE_UART_BAUD
#undef CHIP_CONSOLE_UART_BAUD
#endif
#define CHIP_CONSOLE_UART_BAUD		(115200)
#else
#ifndef CHIP_CONSOLE_UART_BAUD
#define CHIP_CONSOLE_UART_BAUD		UART_BAUDRATE_921600
#endif
#endif


#ifndef CHIP_SPI_FREQ
#define CHIP_SPI_FREQ			(1000000)
#endif


#define CHIP_SYS_TIMER_ID		TIMER_0
#define CHIP_SYS_TIMER_INTNO		INTNO_TIMER0
#define CHIP_SYS_TIMER_HZ		(1000)

/** chip timer count frequency (HZ) */
#define CHIP_SYS_TIMER_MS_HZ		(1000)
/** chip timer count frequency convention based on the global timer counter */
#define CHIP_SYS_TIMER_MS_CONV		(CHIP_SYS_TIMER_MS_HZ/CHIP_SYS_TIMER_HZ)

#define CHIP_OS_TIMER_ID		TIMER_0
#define CHIP_OS_TIMER_INTNO		INTNO_TIMER0

#define CHIP_CPU_CLOCK			clock_frequency(CPU_CLOCK)

#define OSP_DELAY_OS_COMPAT_ENABLE	(1)
#define OSP_DELAY_OS_COMPAT_DISABLE	(0)

#define OSP_GET_CUR_SYSHZ()		(gl_alps_sys_hz_cnt)
#define OSP_GET_CUR_MS()		(gl_alps_ms_cnt)
#define OSP_GET_CUR_US()		chip_get_cur_us()
#define OSP_GET_CUR_HWTICKS()		chip_get_hwticks()

#ifdef __cplusplus
extern "C" {
#endif

extern void chip_init(void);
extern void chip_timer_update(uint32_t precision);
extern void chip_delay_ms(uint32_t ms, uint8_t os_compat);
extern uint64_t chip_get_hwticks(void);
extern uint64_t chip_get_cur_us(void);
extern void chip_hw_mdelay(uint32_t ms);
void chip_hw_udelay(uint32_t us);

#ifdef __cplusplus
}
#endif

#endif	/* _ALPS_H_ */

/** @} end of group CHIP_ALPS_COMMON_INIT */
