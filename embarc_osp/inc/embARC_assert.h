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
 * \file
 * \ingroup EMBARC_ASSERT
 * \brief necessary definitions of assert
 */

#ifndef _EMBARC_ASSERT_H_
#define _EMBARC_ASSERT_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EMBARC_PRINTF
	#ifdef MID_COMMON
		#include "xprintf.h"
		#define EMBARC_PRINTF xprintf
	#else
		#ifdef IO_STREAM_PRINTF
			#include "io_printf.h"
			#define EMBARC_PRINTF io_exprintf
		#else
			#include <stdio.h>
			#define EMBARC_PRINTF printf
		#endif
	#endif
#endif

#include "embARC_toolchain.h"

extern void _exit_loop();

#define ASSERT_FORMAT "%s: Line %d: assert(%s) failed.\r\n"
#define HALT_FORMAT "%s: Line %d: halt reason:"


Inline void embARC_abort(void)
{
	_exit_loop();
}

Inline void embARC_assert(const char *exptext, const char *file, uint32_t line)
{
	EMBARC_PRINTF(ASSERT_FORMAT, file, line, exptext);
	_exit_loop();
}

Inline void embARC_halt(const char *exptext, const char *file, uint32_t line)
{
	EMBARC_PRINTF(HALT_FORMAT, file, line);
	EMBARC_PRINTF("%s\r\n", exptext);
	_exit_loop();
}

/* check whether the expr is true */
#define EMBARC_ASSERT(expr) \
		((void)(!(expr) ? (embARC_assert(#expr, __FILE__, __LINE__), 0) : 0))

#define EMBARC_HALT(fmt, ...)	({ \
			EMBARC_PRINTF(HALT_FORMAT, __FILE__, __LINE__); \
			EMBARC_PRINTF(fmt, ##__VA_ARGS__); \
			EMBARC_PRINTF("\r\n"); \
			Asm("flag 1"); \
		})

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H_ */
