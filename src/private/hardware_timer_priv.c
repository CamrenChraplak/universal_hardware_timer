/*
	hardware_timer_priv.c - hardware timer interface for microcontrollers
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

// functions to execute at end of ISR
uhwt_function_ptr_t hardTimerFunctions[UHWT_TIMER_COUNT];

// function parameters to pass
uhwt_params_ptr_t hardTimerParams[UHWT_TIMER_COUNT];

#ifndef UHWT_NO_CALLBACK_SUPPORT

	// callback functions for linking to ISR
	uhwt_platform_callback_ptr_t hardTimerCallbacks[UHWT_TIMER_COUNT];

	#if UHWT_SUPPORT_ESP32

		typedef bool uhwt_callback_ret_t;
		#define CALLBACK_RETURN() return false

		#if ESP_IDF_VERSION_MAJOR == 4
			#define CALL_PARAMS void *params
		#elif ESP_IDF_VERSION_MAJOR == 5
			#define CALL_PARAMS gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *params
		#endif

	#elif UHWT_SUPPORT_PICO

		typedef bool uhwt_callback_ret_t;
		#define CALLBACK_RETURN() return true
		#define CALL_PARAMS repeating_timer_t *rt

	#endif

	/**
	 * Creates callback functions for each timer
	 * 
	 * @param num timer number to set
	 */
	#define TIMER_CALLBACK_PROTOTYPE(num) \
		static uhwt_callback_ret_t HARD_TIMER_CONCATENATE(timerCallback, num)(CALL_PARAMS) { \
			((void(*)())hardTimerFunctions[num])(hardTimerParams[num]); \
			CALLBACK_RETURN(); \
		}

	#if UHWT_TIMER_COUNT >= 1
		TIMER_CALLBACK_PROTOTYPE(0)
	#endif
	#if UHWT_TIMER_COUNT >= 2
		TIMER_CALLBACK_PROTOTYPE(1)
	#endif
	#if UHWT_TIMER_COUNT >= 3
		TIMER_CALLBACK_PROTOTYPE(2)
	#endif
	#if UHWT_TIMER_COUNT >= 4
		TIMER_CALLBACK_PROTOTYPE(3)
	#endif
	#if UHWT_TIMER_COUNT >= 5
		TIMER_CALLBACK_PROTOTYPE(4)
	#endif
	#if UHWT_TIMER_COUNT >= 6
		TIMER_CALLBACK_PROTOTYPE(5)
	#endif
	#if UHWT_TIMER_COUNT >= 7
		TIMER_CALLBACK_PROTOTYPE(6)
	#endif
	#if UHWT_TIMER_COUNT >= 8
		TIMER_CALLBACK_PROTOTYPE(7)
	#endif
	#if UHWT_TIMER_COUNT >= 9
		TIMER_CALLBACK_PROTOTYPE(8)
	#endif
	#if UHWT_TIMER_COUNT >= 10
		TIMER_CALLBACK_PROTOTYPE(9)
	#endif
	#if UHWT_TIMER_COUNT >= 11
		TIMER_CALLBACK_PROTOTYPE(10)
	#endif
	#if UHWT_TIMER_COUNT >= 12
		TIMER_CALLBACK_PROTOTYPE(11)
	#endif
	#if UHWT_TIMER_COUNT >= 13
		TIMER_CALLBACK_PROTOTYPE(12)
	#endif
	#if UHWT_TIMER_COUNT >= 14
		TIMER_CALLBACK_PROTOTYPE(13)
	#endif
	#if UHWT_TIMER_COUNT >= 15
		TIMER_CALLBACK_PROTOTYPE(14)
	#endif
	#if UHWT_TIMER_COUNT >= 16
		TIMER_CALLBACK_PROTOTYPE(15)
	#endif

#endif

/**
 * Simplifies callback test cases
 * 
 * @param num timer number
 */
#define CALLBACK_TEST_CASE(num) \
	case(HARD_TIMER_CONCATENATE(UHWT_TIMER, num)): \
		hardTimerCallbacks[num] = HARD_TIMER_CONCATENATE(timerCallback, num); \
	break;

bool uhwtSetCallbackParams(uhwt_timer_t timer,
		uhwt_function_ptr_t function, uhwt_params_ptr_t params) {
	if (timer == UHWT_TIMER_INVALID) {
		return false;
	}
	if (!uhwtPlatformSetCallbackParams(timer, function, params)) {
		return false;
	}
	hardTimerFunctions[timer] = function;
	hardTimerParams[timer] = params;

	#ifndef UHWT_NO_CALLBACK_SUPPORT

		switch(timer) {
			#if UHWT_TIMER_COUNT >= 1
				CALLBACK_TEST_CASE(0)
			#endif
			#if UHWT_TIMER_COUNT >= 2
				CALLBACK_TEST_CASE(1)
			#endif
			#if UHWT_TIMER_COUNT >= 3
				CALLBACK_TEST_CASE(2)
			#endif
			#if UHWT_TIMER_COUNT >= 4
				CALLBACK_TEST_CASE(3)
			#endif
			#if UHWT_TIMER_COUNT >= 5
				CALLBACK_TEST_CASE(4)
			#endif
			#if UHWT_TIMER_COUNT >= 6
				CALLBACK_TEST_CASE(5)
			#endif
			#if UHWT_TIMER_COUNT >= 7
				CALLBACK_TEST_CASE(6)
			#endif
			#if UHWT_TIMER_COUNT >= 8
				CALLBACK_TEST_CASE(7)
			#endif
			#if UHWT_TIMER_COUNT >= 9
				CALLBACK_TEST_CASE(8)
			#endif
			#if UHWT_TIMER_COUNT >= 10
				CALLBACK_TEST_CASE(9)
			#endif
			#if UHWT_TIMER_COUNT >= 11
				CALLBACK_TEST_CASE(10)
			#endif
			#if UHWT_TIMER_COUNT >= 12
				CALLBACK_TEST_CASE(11)
			#endif
			#if UHWT_TIMER_COUNT >= 13
				CALLBACK_TEST_CASE(12)
			#endif
			#if UHWT_TIMER_COUNT >= 14
				CALLBACK_TEST_CASE(13)
			#endif
			#if UHWT_TIMER_COUNT >= 15
				CALLBACK_TEST_CASE(14)
			#endif
			#if UHWT_TIMER_COUNT >= 16
				CALLBACK_TEST_CASE(15)
			#endif
			default:
			break;
		}

	#endif

	return true;
}

uhwt_platform_callback_ptr_t uhwtGetCallback(uhwt_timer_t timer) {
	#ifndef UHWT_NO_CALLBACK_SUPPORT
		if (timer == UHWT_TIMER_INVALID) {
			return NULL;
		}
		return hardTimerCallbacks[timer];
	#else
		return NULL;
	#endif
}

#if UHWT_TIMER_COUNT <= 8
	typedef uint8_t uhwt_stat_t; // type for timer stats
#elif UHWT_TIMER_COUNT <= 16
	typedef uint16_t uhwt_stat_t; // type for timer stats
#elif UHWT_TIMER_COUNT <= 32
	typedef uint32_t uhwt_stat_t; // type for timer stats
#elif UHWT_TIMER_COUNT <= 64
	typedef uint64_t uhwt_stat_t; // type for timer stats
#endif

// stores status of hardware timers
typedef struct {
	uhwt_stat_t uhwtClaimed: UHWT_TIMER_COUNT; // whether timers were claimed or not
	uhwt_stat_t uhwtStarted: UHWT_TIMER_COUNT; // whether timers were started or not
} uhwt_stat_s;

// stores status of all hardware timers
uhwt_stat_s uhwtStats;

#ifdef UHWT_PRIORITY_SUPPORT
	uhwt_priority_t uhwtPriorities[UHWT_TIMER_COUNT]; // priorities of timer execution
#endif

bool uhwtTimerClaimed(uhwt_timer_t timer) {
	if (timer == UHWT_TIMER_INVALID) {
		return false;
	}
	return !!(uhwtStats.uhwtClaimed & (((uhwt_stat_t)1) << (timer)));
}

bool uhwtTimerStarted(uhwt_timer_t timer) {
	if (timer == UHWT_TIMER_INVALID) {
		return false;
	}
	return !!(uhwtStats.uhwtStarted & (((uhwt_stat_t)1) << (timer)));
}

bool uhwtClaimTimerStats(uhwt_timer_t *timer, uhwt_claim_s claimArgs) {

	if (*timer == UHWT_TIMER_INVALID) {
		uhwt_timer_t statTimer = uwhtPlatformGetNextTimerStats(claimArgs);

		if (statTimer != UHWT_TIMER_INVALID) {
			*timer = statTimer;
			return true;
		}
	}

	return uhwtClaimTimer(timer);
}

bool uhwtClaimTimer(uhwt_timer_t *timer) {

	if (timer == NULL) {
		return false;
	}

	if (*timer != UHWT_TIMER_INVALID) {
		if (!uhwtTimerClaimed(*timer) && !uhwtTimerStarted(*timer)) {
			uhwtStats.uhwtClaimed |= (1 << (*timer));
			return true;
		}
	}
	else {
		*timer = uwhtGetNextTimer();
		if (*timer != UHWT_TIMER_INVALID) {
			uhwtStats.uhwtClaimed |= (1 << (*timer));
			return true;
		}
	}
	
	return false;
}

bool uhwtUnclaimTimer(uhwt_timer_t timer) {
	if (uhwtTimerClaimed(timer)) {
		uhwtStats.uhwtClaimed &= (~(1 << (timer)));
		return true;
	}
	return false;
}

bool uhwtStartTimer(uhwt_timer_t timer) {
	if (!uhwtTimerStarted(timer)) {
		uhwtStats.uhwtStarted |= (1 << (timer));
		return true;
	}
	return false;
}

bool uhwtStopTimer(uhwt_timer_t timer) {
	if (uhwtTimerStarted(timer)) {
		uhwtStats.uhwtStarted &= (~(1 << (timer)));
		return true;
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

/**
 * Gets difference between given frequencies
 * 
 * @param freq1 first frequency to test
 * @param freq2 second frequency to test
 * 
 * @result difference between frequencies
 */
uhwt_freq_t uhwtFreqDelta(uhwt_freq_t freq1, uhwt_freq_t freq2) {
	if (freq1 > freq2) {
		return freq1 - freq2;
	}
	return freq2 - freq1;
}

/**
 * Calculates closest stat for given timer
 * 
 * @param timer timer to calculate for
 * @param targetFreq target frequency to achieve
 * @param scalar pointer to pre scalar to set
 * @param timerTicks pointer to timer ticks to set
 * 
 * @return calculated frequency
 */
uhwt_freq_t uhwtGetClosestFreq(uhwt_timer_t timer, uhwt_freq_t targetFreq, uhwt_prescalar_t *scalar, uhwt_timertick_t *timerTicks) {

	// get tick of 1 and corresponding scalar
	*timerTicks = 1;
	*scalar = uhwtCalcScalar(targetFreq, *timerTicks);

	// test if tick of 1 works
	if (uhwtValidPreScalar(timer, *scalar)) {
		if (uhwtEqualFreq(targetFreq, *scalar, *timerTicks)) {
			return targetFreq;
		}
	}

	*timerTicks = 0;
	*scalar = 0;

	uhwt_freq_t closestFreq = 0;
	uhwt_prescalar_t tempScalar = uhwtGetNextPreScalar(0);

	while (tempScalar != 0) {
		// ignore invalid scalars
		if (!uhwtValidPreScalar(timer, tempScalar)) {
			tempScalar = uhwtGetNextPreScalar(tempScalar);
			continue;
		}

		// ignore invalid ticks
		uhwt_timertick_t calcTicks = uhwtCalcTicks(targetFreq, tempScalar);
		if (!uhwtValidTimerTicks(timer, calcTicks)) {
			tempScalar = uhwtGetNextPreScalar(tempScalar);
			continue;
		}

		uhwt_freq_t tempFreq = uhwtCalcFreq(tempScalar, calcTicks);

		// frequency is exact value
		if (uhwtEqualFreq(targetFreq, tempScalar, calcTicks)) {
			*scalar = tempScalar;
			*timerTicks = calcTicks;
			closestFreq = tempFreq;
			break;
		}

		// test if newly calculated frequency is closer
		if (uhwtFreqDelta(targetFreq, closestFreq) > uhwtFreqDelta(targetFreq, tempFreq)) {
			*scalar = tempScalar;
			*timerTicks = calcTicks;
			closestFreq = tempFreq;
		}

		tempScalar = uhwtGetNextPreScalar(tempScalar);
	}

	return closestFreq;
}

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

bool uhwtGetStats(uhwt_timer_t *timer, uhwt_freq_t targetFreq, uhwt_prescalar_t *scalar, uhwt_timertick_t *timerTicks) {
	if (timer == NULL || scalar == NULL || timerTicks == NULL) {
		return false;
	}

	if (uhwtTimerClaimed(*timer) && uhwtTimerStarted(*timer)) {
		return false;
	}

	// gets next free timer if one isn't selected
	if ((!uhwtTimerClaimed(*timer) && uhwtTimerStarted(*timer)) || *timer == UHWT_TIMER_INVALID) {
		*timer = uwhtGetNextTimer();
	}
	if (*timer == UHWT_TIMER_INVALID) {
		return false;
	}

	// gets closest frequency
	uhwtGetClosestFreq(*timer, targetFreq, scalar, timerTicks);

	if (!uhwtValidPreScalar(*timer, *scalar) || !uhwtValidTimerTicks(*timer, *timerTicks)) {
		return false;
	}

	return true;
}

bool uhwtGetClosestStats(uhwt_timer_t *timer, uhwt_freq_t targetFreq,
		uhwt_prescalar_t *scalar, uhwt_timertick_t *timerTicks) {

	#ifndef UHWT_CONFIGS_NOT_EQUAL
		return uhwtGetStats(timer, targetFreq, scalar, timerTicks);
	#else

		if (timer == NULL || scalar == NULL || timerTicks == NULL) {
			return false;
		}

		uhwt_freq_t closestFreq = 0;
		*scalar = 0;
		*timerTicks = 0;
		*timer = UHWT_TIMER_INVALID;

		for (uint8_t i = 0; i < UHWT_TIMER_COUNT; i++) {
			uhwt_prescalar_t tempScalar;
			uhwt_timertick_t tempTicks;

			if (uhwtTimerClaimed(i) || uhwtTimerStarted(i)) {
				continue;
			}

			// gets closest frequency
			uhwt_freq_t calcFreq = uhwtGetClosestFreq(i, targetFreq, &tempScalar, &tempTicks);

			// frequency is exact value
			if (uhwtEqualFreq(targetFreq, tempScalar, tempTicks)) {
				*scalar = tempScalar;
				*timerTicks = tempTicks;
				*timer = i;
				return true;
			}

			// test if newly calculated frequency is closer
			if (uhwtFreqDelta(targetFreq, closestFreq) > uhwtFreqDelta(targetFreq, calcFreq)) {
				*scalar = tempScalar;
				*timerTicks = tempTicks;
				closestFreq = calcFreq;
				*timer = i;
			}
		}

		if (!uhwtValidPreScalar(*timer, *scalar) || !uhwtValidTimerTicks(*timer, *timerTicks)) {
			return false;
		}

		return true;

	#endif
}

bool uhwtValidFrequency(uhwt_freq_t freq) {
	if (freq > UHWT_TIMER_FREQ_MAX) {
		return false;
	}
	return true;
}

bool uhwtSetStats(uhwt_timer_t timer, uhwt_prescalar_t scalar, uhwt_timertick_t timerTicks) {
	if (!uhwtTimerStarted(timer) && timer != UHWT_TIMER_INVALID) {
		return uhwtPlatformSetStats(timer, scalar, timerTicks);
	}
	return false;
}

#ifndef UHWT_PRIORITY_SUPPORT
	void uhwtSetPriority(uhwt_timer_t timer, uhwt_priority_t priority) {}
#endif

uhwt_timer_t uwhtPlatformGetNextTimerStats(uhwt_claim_s claimArgs) __attribute__((weak));
uhwt_timer_t uwhtPlatformGetNextTimerStats(uhwt_claim_s claimArgs) {
	return UHWT_TIMER_INVALID;
}

bool uhwtValidPreScalar(uhwt_timer_t timer, uhwt_prescalar_t scalar) __attribute__((weak));
bool uhwtValidPreScalar(uhwt_timer_t timer, uhwt_prescalar_t scalar) {
	if (scalar == 0) {
		return false;
	}
	return true;
}

bool uhwtValidTimerTicks(uhwt_timer_t timer, uhwt_timertick_t ticks) __attribute__((weak));
bool uhwtValidTimerTicks(uhwt_timer_t timer, uhwt_timertick_t ticks) {
	if (ticks == 0) {
		return false;
	}
	return true;
}

bool uhwtPlatformSetCallbackParams(uhwt_timer_t timer,
		uhwt_function_ptr_t function, uhwt_params_ptr_t params) __attribute__((weak));
bool uhwtPlatformSetCallbackParams(uhwt_timer_t timer,
		uhwt_function_ptr_t function, uhwt_params_ptr_t params) {
	return true;
}