/*
	hardware_timer_config.c - hardware timer config functionality
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

#include "hardware_timer_priv.h"

#if !UHWT_SUPPORT
	#error "hardware_timer.h library not supported!"
#endif

#ifdef UHWT_PRIORITY_SUPPORT
	uhwt_priority_t uhwtPriorities[UHWT_TIMER_COUNT]; // priorities of timer execution
#else
	void uhwtSetPriority(uhwt_timer_t timer, uhwt_priority_t priority) {}
#endif

bool uhwtEqualFreq(uhwt_freq_t targetFreq, uhwt_prescalar_t scalar, uhwt_timertick_t ticks) {
	uhwt_freq_t calcFreq = uhwtCalcFreq(scalar, ticks);
	if (!uhwtValidFrequency(targetFreq) || !uhwtValidFrequency(calcFreq)) {
		return false;
	}
	if (calcFreq == targetFreq) {
		return uhwtPlatformEqualFreq(targetFreq, scalar, ticks);
	}
	return false;
}

bool uhwtValidFrequency(uhwt_freq_t freq) {
	if (freq > UHWT_TIMER_FREQ_MAX) {
		return false;
	}
	return true;
}

bool uhwtStopTimer(uhwt_timer_t timer) {
	if (!uhwtTimerStarted(timer)) {
		return false;
	}
	if (uhwtPlatformStopTimer(timer)) {
		return uhwtSetTimerStopped(timer);
	}
	return false;
}

bool uhwtStartTimer(uhwt_timer_t timer) {
	if (uhwtTimerStarted(timer)) {
		return false;
	}
	if (uhwtPlatformStartTimer(timer)) {
		return uhwtSetTimerStarted(timer);
	}
	return false;
}

uhwt_timer_t uwhtGetNextTimer() {
	for (uhwt_timer_t i = 0; i < UHWT_TIMER_COUNT; i++) {
		if (!uhwtTimerStarted(i) && !uhwtTimerClaimed(i)) {
			return i;
		}
	}
	return UHWT_TIMER_INVALID;
}

uhwt_timer_t uwhtGetNextTimerStats(uhwt_claim_s claimArgs) {

	uhwt_timer_t timer = uwhtPlatformGetNextTimerStats(claimArgs);

	if (timer == UHWT_TIMER_INVALID) {
		return uwhtGetNextTimer();
	}

	return timer;
}

// TODO: remove

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
	
	if (!uhwtTimerStarted(*timer)) {
		*freq = uhwtCalcFreq(scalar, timerTicks);
		uhwtInitTimer(*timer);
		uhwtSetPriority(*timer, priority);
		uhwtSetCallbackParams(*timer, function, params);
		uhwtSetStats(*timer, scalar, timerTicks);
		uhwtStartTimer(*timer);
		return true;
	}

	return false;
}

// TODO: remove

bool cancelHardTimer(uhwt_timer_t timer) {
	return uhwtStopTimer(timer);
}