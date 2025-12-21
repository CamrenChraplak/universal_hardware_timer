/*
	hardware_timer_avr.c - timer configuration for avr
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

#include "hardware_timer_avr.h"

#if UHWT_TIMER_COUNT > 0 && UHWT_SUPPORT_AVR

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

const uhwt_prescalar_t scalarMask[] PROGMEM = {
	0, // SCALAR_0
	1, // SCALAR_1
	8, // SCALAR_8
	32, // SCALAR_32
	64, // SCALAR_64
	128, // SCALAR_128
	256, // SCALAR_256
	1024, // SCALAR_1024
};

#if TIMER_0_ALIAS != UHWT_TIMER_INVALID_LIT
	ISR(TIMER0_COMPA_vect) {
		((void(*)())hardTimerFunctions[TIMER_0_ALIAS])(hardTimerParams[TIMER_0_ALIAS]);
	}
#endif

#if TIMER_1_ALIAS != UHWT_TIMER_INVALID_LIT
	ISR(TIMER1_COMPA_vect) {
		((void(*)())hardTimerFunctions[TIMER_1_ALIAS])(hardTimerParams[TIMER_1_ALIAS]);
	}
#endif

#if TIMER_2_ALIAS != UHWT_TIMER_INVALID_LIT
	ISR(TIMER2_COMPA_vect) {
		((void(*)())hardTimerFunctions[TIMER_2_ALIAS])(hardTimerParams[TIMER_2_ALIAS]);
	}
#endif

/**
 * Gets scalar mask value from enum
 * 
 * @param i scalar enum
 * 
 * @return literal int value
 */
uint16_t getMask(prescalar_enum_t scalar) {
	return ((uint16_t)pgm_read_word_near(scalarMask + scalar));
}

/****************************
 * Platform Functions
****************************/

uhwt_freq_t uhwtCalcFreq(uhwt_prescalar_t scalar, uhwt_timertick_t ticks) {
	return F_CPU / ((uhwt_freq_t)getMask(scalar) * (ticks + 1));
}

bool uhwtPlatformEqualFreq(uhwt_freq_t targetFreq, uhwt_prescalar_t scalar, uhwt_timertick_t ticks) {
	if (F_CPU % ((uhwt_freq_t)getMask(scalar) * (ticks + 1)) != 0) {
		return false;
	}
	return true;
}

uhwt_timer_t uwhtPlatformGetNextTimerStats(uhwt_claim_s claimArgs) {
	uhwt_timer_t timer = TIMER_1_ALIAS;

	// checks priorities
	if (claimArgs.slowestTimer) {
		if (uhwtClaimTimer(&timer)) {
			return timer;
		}
	}
	timer = TIMER_2_ALIAS;
	if (claimArgs.mostAccurateTimer) {
		if (uhwtClaimTimer(&timer)) {
			return timer;
		}
	}

	return UHWT_TIMER_INVALID;
}

bool uhwtValidPreScalar(uhwt_timer_t timer, uhwt_prescalar_t scalar) {
	if (scalar == 0) {
		return false;
	}
	if ((scalar == SCALAR_32 || scalar == SCALAR_128) && timer != TIMER_2_ALIAS) {
		return false;
	}
	return true;
}

bool uhwtValidTimerTicks(uhwt_timer_t timer, uhwt_timertick_t ticks) {
	if (ticks == 0) {
		return false;
	}
	if (ticks > UINT8_MAX && timer != TIMER_1_ALIAS) {
		return false;
	}
	return true;
}

uhwt_prescalar_t uhwtGetNextPreScalar(uhwt_prescalar_t prevScalar) {
	switch(prevScalar) {
		case(SCALAR_0):
			return SCALAR_1024;
		break;
		case(SCALAR_1024):
			return SCALAR_256;
		break;
		case(SCALAR_256):
			return SCALAR_128;
		break;
		case(SCALAR_128):
			return SCALAR_64;
		break;
		case(SCALAR_64):
			return SCALAR_32;
		break;
		case(SCALAR_32):
			return SCALAR_8;
		break;
		case(SCALAR_8):
			return SCALAR_1;
		break;
		case(SCALAR_1):
			return SCALAR_0;
		break;
		default:
			return SCALAR_0;
		break;
	};
}

uhwt_timertick_t uhwtCalcTicks(uhwt_freq_t targetFreq, uhwt_prescalar_t scalar) {
	return (F_CPU / (getMask(scalar) * targetFreq)) - 1;
}

uhwt_prescalar_t uhwtCalcScalar(uhwt_freq_t targetFreq, uhwt_timertick_t ticks) {
	return F_CPU / (targetFreq * (ticks + 1));
}

/**
 * Sets timer stats
 * 
 * @param num timer number
 * @param scalar scalar to set
 * @param timerTicks ticks to set
 */
#define UHWT_SET_STATS(num, scalar, timerTicks) \
	cli(); \
	HARD_TIMER_CONCATENATE3(TIMER_, num, _TARGET) = timerTicks; \
	HARD_TIMER_CONCATENATE3(TIMER_, num, _SET_SCALAR)(scalar); \
	sei()

bool uhwtPlatformSetStats(uhwt_timer_t timer, uhwt_prescalar_t scalar, uhwt_timertick_t timerTicks) {
	
	switch(timer) {
		#if TIMER_0_ALIAS != UHWT_TIMER_INVALID_LIT
			case(TIMER_0_ALIAS):
				UHWT_SET_STATS(0, scalar, timerTicks);
			break;
		#endif
		#if TIMER_1_ALIAS != UHWT_TIMER_INVALID_LIT
			case(TIMER_1_ALIAS):
				UHWT_SET_STATS(1, scalar, timerTicks);
			break;
		#endif
		#if TIMER_2_ALIAS != UHWT_TIMER_INVALID_LIT
			case(TIMER_2_ALIAS):
				UHWT_SET_STATS(2, scalar, timerTicks);
			break;
		#endif
		default:
			return false;
		break;
	}
	return true;
}

/**
 * Inits hardware timer
 * 
 * @param num timer id
 */
#define UHWT_INIT_TIMER(num) \
	cli(); \
	HARD_TIMER_CONCATENATE3(TIMER_, num, _COMP) = 0; \
	HARD_TIMER_CONCATENATE3(TIMER_, num, _WAVEFORM) = 0; \
	HARD_TIMER_CONCATENATE3(TIMER_, num, _COUNTER) = 0; \
	sei()

bool uhwtInitTimer(uhwt_timer_t timer) {
	switch(timer) {
		#if TIMER_0_ALIAS != UHWT_TIMER_INVALID_LIT
			case(TIMER_0_ALIAS):
				UHWT_INIT_TIMER(0);
			break;
		#endif
		#if TIMER_1_ALIAS != UHWT_TIMER_INVALID_LIT
			case(TIMER_1_ALIAS):
				UHWT_INIT_TIMER(1);
			break;
		#endif
		#if TIMER_2_ALIAS != UHWT_TIMER_INVALID_LIT
			case(TIMER_2_ALIAS):
				UHWT_INIT_TIMER(2);
			break;
		#endif
		default:
			return false;
		break;
	}
	return true;
}

/**
 * Stops timer
 * 
 * @param num timer id
 */
#define UHWT_STOP_TIMER(num) \
	cli(); \
	HARD_TIMER_CONCATENATE3(TIMER_, num, _INTERR) &= ~HARD_TIMER_CONCATENATE3(TIMER_, num, _INTERR_ENABLE); \
	sei()

bool uhwtPlatformStopTimer(uhwt_timer_t timer) {
	switch(timer) {
		#if TIMER_0_ALIAS != UHWT_TIMER_INVALID_LIT
			case(TIMER_0_ALIAS):
				UHWT_STOP_TIMER(0);
			break;
		#endif
		#if TIMER_1_ALIAS != UHWT_TIMER_INVALID_LIT
			case(TIMER_1_ALIAS):
				UHWT_STOP_TIMER(1);
			break;
		#endif
		#if TIMER_2_ALIAS != UHWT_TIMER_INVALID_LIT
			case(TIMER_2_ALIAS):
				UHWT_STOP_TIMER(2);
			break;
		#endif
		default:
			return false;
		break;
	}
	return true;
}

/****************************
 * Timer Functions
****************************/

/**
 * Cancels hard timer
 * 
 * @param num timer id
 */
#define CANCEL_HARD_TIMER(num) \
	cli(); \
	HARD_TIMER_CONCATENATE3(TIMER_, num, _INTERR) &= ~HARD_TIMER_CONCATENATE3(TIMER_, num, _INTERR_ENABLE); \
	sei()

bool cancelHardTimer(uhwt_timer_t timer) {
	return uhwtStopTimer(timer);
}

/**
 * Starts hardware timer
 * 
 * @param num timer id
 */
#define START_HARD_TIMER(num) \
	cli(); \
	HARD_TIMER_CONCATENATE3(TIMER_, num, _INCR) |= HARD_TIMER_CONCATENATE3(TIMER_, num, _INCREM_ENABLE); \
	HARD_TIMER_CONCATENATE3(TIMER_, num, _INTERR) |= HARD_TIMER_CONCATENATE3(TIMER_, num, _INTERR_ENABLE); \
	sei()

bool startHardTimer(uhwt_timer_t timer) {
	switch(timer) {
		#if TIMER_0_ALIAS != UHWT_TIMER_INVALID_LIT
			case(TIMER_0_ALIAS):
				START_HARD_TIMER(0);
			break;
		#endif
		#if TIMER_1_ALIAS != UHWT_TIMER_INVALID_LIT
			case(TIMER_1_ALIAS):
				START_HARD_TIMER(1);
			break;
		#endif
		#if TIMER_2_ALIAS != UHWT_TIMER_INVALID_LIT
			case(TIMER_2_ALIAS):
				START_HARD_TIMER(2);
			break;
		#endif
		default:
			return false;
		break;
	}
	return true;
}

bool setHardTimer(uhwt_timer_t *timer, uhwt_freq_t *freq, uhwt_function_ptr_t function, uhwt_params_ptr_t params, uhwt_priority_t priority) {

	if (function == NULL || freq == NULL || timer == NULL) {
		return false;
	}
	if (*freq == (uhwt_freq_t)0 || *freq > UHWT_TIMER_FREQ_MAX) {
		return false;
	}

	prescalar_enum_t scalar;
	uhwt_timertick_t timerTicks;

	if (!uhwtGetStats(timer, *freq, &scalar, &timerTicks)) {
		return false;
	}
	if (!uhwtValidPreScalar(*timer, scalar) || !uhwtValidTimerTicks(*timer, timerTicks)) {
		return false;
	}
	*freq = uhwtCalcFreq(scalar, timerTicks);

	if (!uhwtTimerStarted(*timer)) {

		

		uhwtInitTimer(*timer);
		uhwtSetCallbackParams(*timer, function, params);
		uhwtPlatformSetStats(*timer, scalar, timerTicks);
		startHardTimer(*timer);

		uhwtSetTimerStarted(*timer);
		return true;
	}

	return false;
}

#endif