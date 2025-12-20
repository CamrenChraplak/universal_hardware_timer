/*
	board_pico_timer.c - timer configuration for all Raspberry Pi Picos
	Copyright (C) 2025 Camren Chraplak

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/**
 * timerTicks: time in ms or us
 * scalar: SCALAR_MS (millis) or SCALAR_US (micros)
 */

#include "../private/hardware_timer_priv.h"

#if UHWT_SUPPORT_PICO

#include <pico/time.h>

#define THOUSAND 1000

// hardware timers
struct repeating_timer timers[UHWT_TIMER_COUNT];

/**
 * Gets timer based on desired timer
 * 
 * @param timer timer to select
 * 
 * @return pointer to timer selected
 */
struct repeating_timer* getTimer(uhwt_timer_t timer) {
	if (timer >= 0 && timer < UHWT_TIMER_COUNT) {
		return &timers[timer];
	}
	return NULL;
}

#define PICO_SDK_TIMER_MAX 1000000

/****************************
 * Platform Functions
****************************/

uhwt_freq_t uhwtCalcFreq(uhwt_prescalar_t scalar, uhwt_timertick_t ticks) {
	return PICO_SDK_TIMER_MAX / ticks;
}

bool uhwtPlatformEqualFreq(uhwt_freq_t targetFreq, uhwt_prescalar_t scalar, uhwt_timertick_t ticks) {
	if (PICO_SDK_TIMER_MAX % ticks != 0) {
		return false;
	}
	return true;
}

uhwt_prescalar_t uhwtGetNextPreScalar(uhwt_prescalar_t prevScalar) {
	// u64 bit max
	// always constant

	// 0: start of request
	if (!prevScalar) {
		return 1;
	}
	// end of request
	return 0;
}

uhwt_timertick_t uhwtCalcTicks(uhwt_freq_t targetFreq, uhwt_prescalar_t scalar) {
	return PICO_SDK_TIMER_MAX / targetFreq;
}

uhwt_prescalar_t uhwtCalcScalar(uhwt_freq_t targetFreq, uhwt_timertick_t ticks) {
	return 1;
}

bool uhwtValidPreScalar(uhwt_timer_t timer, uhwt_prescalar_t scalar) {
	if (scalar == 1) {
		return true;
	}
	return false;
}

/****************************
 * Timer Functions
****************************/

bool cancelHardTimer(uhwt_timer_t timer) {

	if (uhwtTimerStarted(timer)) {
		struct repeating_timer* timerPtr = getTimer(timer);
		if (!cancel_repeating_timer(timerPtr)) {
			return false;
		}
		uhwtStopTimer(timer);
		return true;
	}

	return false;
}

bool setHardTimer(uhwt_timer_t *timer, uhwt_freq_t *freq, uhwt_function_ptr_t function, uhwt_params_ptr_t params, uhwt_priority_t priority) {

	if (function == NULL || freq == NULL || timer == NULL) {
		return false;
	}
	if (*freq == (uhwt_freq_t)0 || *freq > UHWT_TIMER_FREQ_MAX) {
		return false;
	}

	uhwt_prescalar_t scalar;
	uhwt_timertick_t timerTicks;
	
	if (!uhwtGetStats(timer, *freq, &scalar, &timerTicks)) {
		return false;
	}
	if (!uhwtValidPreScalar(*timer, scalar) || !uhwtValidTimerTicks(*timer, timerTicks)) {
		return false;
	}
	*freq = uhwtCalcFreq(scalar, timerTicks);

	if (!uhwtTimerStarted(*timer)) {
		struct repeating_timer* timerPtr = getTimer(*timer);

		uhwtSetCallbackParams(*timer, function, params);

		if (add_repeating_timer_us(-timerTicks, uhwtGetCallback(*timer), NULL, timerPtr)) {
			uhwtStartTimer(*timer);
			return true;
		}
	}

	return false;
}

#endif