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
#ifndef _TARGET_MEM_CONFIG_H_
#define _TARGET_MEM_CONFIG_H_

#include "arc_feature_config.h"

#ifdef USE_APPL_MEM_CONFIG
#include "appl_mem_config.h"
#endif

/**
 * The unit of XXXX_SIZE is Byte
 * For REGION_ROM, ICCM, EXT_ROM and EXT_RAM are available
 * For REGION_RAM, DCCM and EXT_RAM are available
 */
#ifdef ARC_FEATURE_ICCM_PRESENT
#ifndef ICCM_SIZE
#define ICCM_SIZE	ARC_FEATURE_ICCM_SIZE
#endif
#ifndef ICCM_START
#define ICCM_START	ARC_FEATURE_ICCM_BASE
#endif
#else
#ifndef ICCM_SIZE
#define ICCM_SIZE	0x40000
#endif
#ifndef ICCM_START
#define ICCM_START	0x100000
#endif
#endif

#ifdef ARC_FEATURE_DCCM_PRESENT
#ifndef DCCM_SIZE
#define DCCM_SIZE	ARC_FEATURE_DCCM_SIZE
#endif
#ifndef DCCM_START
#define DCCM_START	ARC_FEATURE_DCCM_BASE
#endif
#else
#ifndef DCCM_SIZE
#define DCCM_SIZE	0x20000
#endif
#ifndef DCCM_START
#define DCCM_START	0xA00000
#endif
#endif

#if (FLASH_XIP_EN)
#if (SYSTEM_3_STAGES_BOOT)
#ifndef EXT_ROM_START
#define EXT_ROM_START	0x330100
#endif
#ifndef EXT_ROM_SIZE
#define EXT_ROM_SIZE	0x3FFF00
#endif
#else
#ifndef EXT_ROM_START
#define EXT_ROM_START	0x330000
#endif
#ifndef EXT_ROM_SIZE
#define EXT_ROM_SIZE	0x400000
#endif
#endif
#endif

#ifndef EXT_RAM_START
#define EXT_RAM_START	0x770000
#endif

#ifndef EXT_RAM_SIZE
#define EXT_RAM_SIZE	0x80000
#endif

#ifndef REGION_ROM
#ifdef ARC_FEATURE_ICACHE_PRESENT
#if (FLASH_XIP_EN)
#define REGION_ROM	EXT_ROM
#else
#define REGION_ROM	EXT_RAM
#endif /* FALSH_XIP_EN */
#else
#define REGION_ROM	ICCM
#endif /* ARC_FEATURE_ICACHE_PRESENT */
#endif /* REGION_ROM */

#ifndef REGION_RAM
#ifdef ARC_FEATURE_DCACHE_PRESENT
#define REGION_RAM	EXT_RAM
#else
#define REGION_RAM	DCCM
#endif
#endif

#endif /* _TARGET_MEM_CONFIG_H_ */
