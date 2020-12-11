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


static inline uint32_t chip_clock_early(void)
{
#ifdef PLAT_ENV_FPGA
    return (10000000U);
#else
    /* pls get by clock tree API. */
    return (0);
#endif
}

static inline uint32_t chip_peri_clock_early(void)
{
#ifdef PLAT_ENV_FPGA
	return (10000000U);
#else
	/* pls get by clock tree API. */
	return 0;
#endif
}

#define ARC_FEATURE_DMP_PERIPHERAL     (0x00000000U)

/* Device Register Base Address */
#include "alps_mmap.h"

/* Interrupt Connection */
#include "alps_interrupt.h"

#endif	/* _ALPS_HARDWARE_H_ */

/** @} end of group BOARD_ALPS_COMMON_INIT */
