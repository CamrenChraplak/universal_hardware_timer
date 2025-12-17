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

typedef enum {
	SCALAR_MS, // timer prescalar for milli seconds
	SCALAR_US, // timer prescalar micro seconds
} prescalar_t; // pre scalar type
typedef int64_t timertick_t; // timer tick type

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

/**
 * Gets next unstarted and unclaimed timer
 * 
 * @return available timer
 */
uhwt_timer_t getNextTimer(void) {
	for (uint8_t i = 0; i < UHWT_TIMER_COUNT; i++) {
		if (!uhwtTimerStarted(i) && !uhwtTimerClaimed(i)) {
			return (uhwt_timer_t)i;
		}
	}
	return UHWT_TIMER_INVALID;
}

uhwt_timer_t claimTimer(uhwt_claim_s *priority) {
	uhwt_timer_t timer = UHWT_TIMER_INVALID;

	if (priority != NULL) {
		uhwtClaimTimerStats(&timer, *priority);
	}
	else {
		uhwtClaimTimer(&timer);
	}

	return timer;
}

bool unclaimTimer(uhwt_timer_t timer) {
	return uhwtUnclaimTimer(timer);
}

bool hardTimerClaimed(uhwt_timer_t timer) {
	return uhwtTimerClaimed(timer);
}

#define PICO_SDK_TIMER_MAX 1000000

/**
 * Gets hard timer stats for target frequency
 * 
 * @param freq pointer to desired frequency in Hz
 * @param timer pointer to timer ID
 * @param scalar pointer to scalar value
 * @param timerTicks pointer to desired tick count
 * 
 * @return result of getting timer stats
 * 
 * @note freq value is changed to actual freq if values are slightly off
 */
uhwt_status_t getHardTimerStats(uhwt_freq_t *freq, uhwt_timer_t *timer, prescalar_t *scalar, timertick_t *timerTicks) {

	uhwt_status_t status = HARD_TIMER_OK;

	// freq doesn't divide evenly with us
	if (PICO_SDK_TIMER_MAX % *freq != 0) {
		status = HARD_TIMER_SLIGHTLY_OFF;
	}

	//target in us
	uhwt_freq_t targetUS = PICO_SDK_TIMER_MAX / *freq;

	if (targetUS % THOUSAND == 0 && status == HARD_TIMER_OK) {
		*scalar = SCALAR_MS;
		*timerTicks = targetUS / THOUSAND;
	}
	else {
		*scalar = SCALAR_US;
		*timerTicks = targetUS;
	}

	if (*scalar == SCALAR_MS) {
		*freq = PICO_SDK_TIMER_MAX / (*timerTicks * THOUSAND);
	}
	else if (*scalar == SCALAR_US) {
		*freq = PICO_SDK_TIMER_MAX / *timerTicks;
	}

	if ((!uhwtTimerClaimed(*timer) && uhwtTimerStarted(*timer)) || *timer == UHWT_TIMER_INVALID) {
		*timer = getNextTimer();
	}
	
	if (*timer == UHWT_TIMER_INVALID) {
		return HARD_TIMER_FAIL;
	}

	return status;
}

bool hardTimerStarted(uhwt_timer_t timer) {
	return uhwtTimerStarted(timer);
}

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

	prescalar_t scalar;
	timertick_t timerTicks;
	
	if (getHardTimerStats(freq, timer, &scalar, &timerTicks) == HARD_TIMER_FAIL) {
		return false;
	}

	if (!uhwtTimerStarted(*timer)) {
		struct repeating_timer* timerPtr = getTimer(*timer);

		setHardTimerFunction(*timer, function, params);

		if (scalar == SCALAR_MS) {
			if (add_repeating_timer_ms(-timerTicks, getHardTimerCallback(*timer), NULL, timerPtr)) {
				uhwtStartTimer(*timer);
				return true;
			}
		}
		else if (scalar == SCALAR_US) {
			if (add_repeating_timer_us(-timerTicks, getHardTimerCallback(*timer), NULL, timerPtr)) {
				uhwtStartTimer(*timer);
				return true;
			}
		}
	}

	return false;
}

#endif