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
 * \ingroup	ARC_HAL_MISC_TIMER
 * \brief  implementation of internal timer related functions
 * \todo RTC support should be improved if RTC is enabled
 */
#include "arc_timer.h"
#include "arc.h"
#include "arc_builtin.h"

/**
 * \brief  check whether the specific timer present
 * \param[in] no timer number
 * \return 1 present, 0 not present
 */
int32_t timer_present(const uint32_t no)
{
	uint32_t bcr = _arc_aux_read(AUX_BCR_TIMERS);
	switch (no) {
		case TIMER_0:
			bcr = (bcr >> 8) & 1;
			break;
		case TIMER_1:
			bcr = (bcr >> 9) & 1;
			break;
		case TIMER_RTC:
			bcr = (bcr >> 10) & 1;
			break;
		default:
			bcr = 0;
			/* illegal argument so return false */
			break;
		}

	return (int)bcr;
}

/**
 * \brief  start the specific timer
 * \param[in] no	timer number
 * \param[in] mode	timer mode
 * \param[in] val	timer limit value (not for RTC)
 * \return 0 success, -1 failure
 */
int32_t timer_start(const uint32_t no, const uint32_t mode, const uint32_t val)
{
	if (timer_present(no) == 0) {
		return -1;
	}

	switch (no) {
		case TIMER_0:
			_arc_aux_write(AUX_TIMER0_CTRL, 0);
			_arc_aux_write(AUX_TIMER0_LIMIT, val);
			_arc_aux_write(AUX_TIMER0_CTRL, mode);
			_arc_aux_write(AUX_TIMER0_CNT, 0);
			break;
		case TIMER_1:
			_arc_aux_write(AUX_TIMER1_CTRL, 0);
			_arc_aux_write(AUX_TIMER1_LIMIT, val);
			_arc_aux_write(AUX_TIMER1_CTRL, mode);
			_arc_aux_write(AUX_TIMER1_CNT, 0);
			break;
		case TIMER_RTC:
			_arc_aux_write(AUX_RTC_CTRL, mode);
			break;
		default:
			return -1;
	}

	return 0;
}

/**
 * \brief  stop and clear the specific timer
 *
 * \param[in] no timer number
 * \return 0 success, -1 failure
 */
int32_t timer_stop(const uint32_t no)
{
	if (timer_present(no) == 0) {
		return -1;
	}

	switch (no) {
		case TIMER_0 :
			_arc_aux_write(AUX_TIMER0_CTRL, 0);
			_arc_aux_write(AUX_TIMER0_LIMIT,0);
			_arc_aux_write(AUX_TIMER0_CNT, 0);
			break;
		case TIMER_1:
			_arc_aux_write(AUX_TIMER1_CTRL, 0);
			_arc_aux_write(AUX_TIMER1_LIMIT,0);
			_arc_aux_write(AUX_TIMER1_CNT, 0);
			break;
		case TIMER_RTC:
			_arc_aux_write(AUX_RTC_CTRL, TIMER_RTC_CLEAR);
			break;
		default:
			return -1;
	}

	return 0;
}

/**
 * \brief  get timer current tick
 *
 * \param[in] no timer number
 * \param[out] val, timer value
 * \return 0 success, -1 failure
 */
int32_t timer_current(const uint32_t no, void *val)
{
	if (timer_present(no) == 0) {
		return -1;
	}

	switch (no) {
		case TIMER_0 :
			*((uint32_t *)val) = _arc_aux_read(AUX_TIMER0_CNT);
			break;
		case TIMER_1 :
			*((uint32_t *)val) = _arc_aux_read(AUX_TIMER1_CNT);
			break;
		case TIMER_RTC:
			*((uint64_t *)val) = _arc_aux_read(AUX_RTC_LOW);
			break;

		default :
			return -1;
	}

	return 0;
}

int32_t timer_current_high(const uint32_t no, void *val)
{
	if (timer_present(no) == 0) {
		return -1;
	}

	switch (no) {
		case TIMER_0 :
		case TIMER_1 :
			*((uint32_t *)val) = 0;
			break;
		case TIMER_RTC:
			*((uint32_t *)val) = _arc_aux_read(AUX_RTC_HIGH);
			break;
		default :
			return -1;
	}

	return 0;
}

/**
 * \brief  clear the interrupt pending bit of timer
 *
 * \param[in] no timer number
 * \return 0 success, -1 failure
 */
int32_t timer_int_clear(const uint32_t no)
{
	uint32_t val;

	if (timer_present(no) == 0) {
		return -1;
	}

	switch (no) {
		case TIMER_0 :
			val = _arc_aux_read(AUX_TIMER0_CTRL);
			val &= ~TIMER_CTRL_IP;
			_arc_aux_write(AUX_TIMER0_CTRL, val);
			break;
		case TIMER_1 :
			val = _arc_aux_read(AUX_TIMER1_CTRL);
			val &= ~TIMER_CTRL_IP;
			_arc_aux_write(AUX_TIMER1_CTRL, val);
			break;
		default :
			return -1;
	}

	return 0;
}

/**
 * \brief  init internal timer
 */
void timer_init(void)
{
	timer_stop(TIMER_0);
	timer_stop(TIMER_1);
	timer_start(TIMER_RTC, TIMER_RTC_ENABLE, 0);
}
uint64_t timer_rtc_count(void)
{
	uint64_t cycles = 0;
	cycles = _arc_aux_read(AUX_RTC_HIGH);
	cycles <<= 32;
	cycles |= _arc_aux_read(AUX_RTC_LOW);
	return cycles;
}
