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
#include "embARC.h"
#include "embARC_debug.h"
#include "embARC_error.h"
//#include "calterah_error.h"
//#include "clkgen.h"
#include "uart_hal.h"
#include "gpio_hal.h"
#include "dma_hal.h"


typedef struct main_args {
	int argc;
	char *argv[];
} MAIN_ARGS;

/** Change this to pass your own arguments to main functions */
MAIN_ARGS s_main_args = {1, {"main"}};

#if defined(MID_FATFS)
static FATFS sd_card_fs;	/* File system object for each logical drive */
#endif /* MID_FATFS */

/*--- When new embARC Startup process is used----*/
#define MIN_CALC(x, y)		(((x)<(y))?(x):(y))
#define MAX_CALC(x, y)		(((x)>(y))?(x):(y))

#ifdef OS_FREERTOS
/* Note: Task size in unit of StackType_t */
/* Note: Stack size should be small than 65536, since the stack size unit is uint16_t */
#define MIN_STACKSZ(size)	(MIN_CALC((size)*sizeof(StackType_t), configTOTAL_HEAP_SIZE>>3)/sizeof(StackType_t))


#ifndef TASK_STACK_SIZE_MAIN
#define TASK_STACK_SIZE_MAIN	MIN_STACKSZ(65535)
#endif

#ifndef TASK_PRI_MAIN
#define TASK_PRI_MAIN		1	/* Main task priority */
#endif
static TaskHandle_t task_handle_main;

#endif /* OS_FREERTOS */


static void task_main(void *par)
{
	int ercd;

	if ((par == NULL) || (((int)par) & 0x3)) {
	/* null or aligned not to 4 bytes */
		ercd = _arc_goto_main(0, NULL);
	} else {
		MAIN_ARGS *main_arg = (MAIN_ARGS *)par;
		ercd = _arc_goto_main(main_arg->argc, main_arg->argv);
	}

#if defined(OS_FREERTOS)
	EMBARC_PRINTF("Exit from main function, error code:%d....\r\n", ercd);
	while (1) {
		vTaskSuspend(NULL);
	}
#else
	while (1);
#endif
}

void init_hardware_hook(void)
{
	dcache_non_cached_region_base(REL_NON_CACHED_BASE);
}

void board_main(void)
{
	/* Set the interrupt priority operating
	 * level of ARC processor to 0.
	 * */
	arc_int_ipm_set(0);
	uint32_t freq = chip_clock_early();
	if (0 == freq) {
		freq = clock_frequency(CPU_CLOCK);
	}
	set_current_cpu_freq(freq);

	io_mux_init();

        if (E_OK != gpio_init()) {
            EMBARC_PRINTF("gpio_init error!\r\n");
        }

	chip_init();

	system_clock_init();

	if (E_OK != dma_init()) {
            EMBARC_PRINTF("dma_init error!\r\n");
	}

	uart_init(CHIP_CONSOLE_UART_ID, CHIP_CONSOLE_UART_BAUD);

#ifdef MID_COMMON
	xprintf_setup();
#endif


#ifdef MID_FATFS
	if(f_mount(&sd_card_fs, "", 0) != FR_OK) {
		EMBARC_PRINTF("FatFS failed to initialize!\r\n");
	} else {
		EMBARC_PRINTF("FatFS initialized successfully!\r\n");
	}
#endif

#ifdef ENABLE_OS
	os_hal_exc_init();
#endif


#ifdef OS_FREERTOS
	xTaskCreate((TaskFunction_t)task_main, "main", TASK_STACK_SIZE_MAIN,
			(void *)(&s_main_args), TASK_PRI_MAIN, &task_handle_main);
#else /* No os and ntshell */
	cpu_unlock();	/* unlock cpu to let interrupt work */
#endif


#ifdef OS_FREERTOS
	vTaskStartScheduler();
#endif

	task_main((void *)(&s_main_args));
/* board level exit */
}

#ifdef USE_SW_TRACE
void trace_info(uint32_t value)
{
	static uint32_t idx = 0;
	raw_writel(SW_TRACE_BASE + (idx++ << 2), value);
	if (idx >= (SW_TRACE_LEN >> 2)) {
		idx = 0;
	}
}
#endif
