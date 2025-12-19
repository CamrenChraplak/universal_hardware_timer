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

typedef enum {
	SCALAR_0, // timer prescalar of 0, represents invalid
	#ifdef SCALAR_1_ENABLE
		SCALAR_1, // timer prescalar of 1
	#endif
	#ifdef SCALAR_8_ENABLE
		SCALAR_8, // timer prescalar of 8
	#endif
	#ifdef SCALAR_32_ENABLE
		SCALAR_32, // timer prescalar of 32
	#endif
	#ifdef SCALAR_64_ENABLE
		SCALAR_64, // timer prescalar of 64
	#endif
	#ifdef SCALAR_128_ENABLE
		SCALAR_128, // timer prescalar of 128
	#endif
	#ifdef SCALAR_256_ENABLE
		SCALAR_256, // timer prescalar of 256
	#endif
	#ifdef SCALAR_1024_ENABLE
		SCALAR_1024, // timer prescalar of 1024
	#endif
} prescalar_enum_t; // pre scalar type

const uhwt_prescalar_t scalarMask[] PROGMEM = {
	0, // SCALAR_0
	#ifdef SCALAR_1_ENABLE
		1, // SCALAR_1
	#endif
	#ifdef SCALAR_8_ENABLE
		8, // SCALAR_8
	#endif
	#ifdef SCALAR_32_ENABLE
		32, // SCALAR_32
	#endif
	#ifdef SCALAR_64_ENABLE
		64, // SCALAR_64
	#endif
	#ifdef SCALAR_128_ENABLE
		128, // SCALAR_128
	#endif
	#ifdef SCALAR_256_ENABLE
		256, // SCALAR_256
	#endif
	#ifdef SCALAR_1024_ENABLE
		1024, // SCALAR_1024
	#endif
};

#ifndef SKIP_TIMER_INDEX
	#define SKIP_TIMER_INDEX UHWT_TIMER_COUNT
#endif

#ifdef UHWT_OVERRIDE_ARDUINO_TIMER
	#define TIMER_COUNT UHWT_TIMER_COUNT
#else
	#define TIMER_COUNT UHWT_TIMER_COUNT + 1
#endif

#if TIMER_COUNT > 0
	#if SKIP_TIMER_INDEX == 0
		#define TIMER_0_ALIAS UHWT_TIMER_INVALID_LIT
	#else
		#define TIMER_0_ALIAS UHWT_TIMER0
	#endif
#else
	#define TIMER_0_ALIAS UHWT_TIMER_INVALID_LIT
#endif
#if TIMER_COUNT > 1
	#if SKIP_TIMER_INDEX == 1
		#define TIMER_1_ALIAS UHWT_TIMER_INVALID_LIT
	#elif SKIP_TIMER_INDEX < 1
		#define TIMER_1_ALIAS UHWT_TIMER0
	#else
		#define TIMER_1_ALIAS UHWT_TIMER1
	#endif
#else
	#define TIMER_1_ALIAS UHWT_TIMER_INVALID_LIT
#endif
#if TIMER_COUNT > 2
	#if SKIP_TIMER_INDEX == 2
		#define TIMER_2_ALIAS UHWT_TIMER_INVALID_LIT
	#elif SKIP_TIMER_INDEX < 2
		#define TIMER_2_ALIAS UHWT_TIMER1
	#else
		#define TIMER_2_ALIAS UHWT_TIMER2
	#endif
#else
	#define TIMER_2_ALIAS UHWT_TIMER_INVALID_LIT
#endif

#define SCALAR_MASK_SIZE (sizeof(scalarMask) / sizeof(uhwt_prescalar_t)) // size of scalarMask

/****************************
 * Timer 0
****************************/

#if TIMER_0_ALIAS != UHWT_TIMER_INVALID_LIT

#define TIMER_0_COMP TCCR0A // sets compare mode
#define TIMER_0_WAVEFORM TCCR0B // sets waveform mode
#define TIMER_0_COUNTER TCNT0 // current counter value
#define TIMER_0_TARGET OCR0A // target tick value
#define TIMER_0_INCR TCCR0A // sets increment mode
#define TIMER_0_SCAL TCCR0B // sets scalar mode
#define TIMER_0_INTERR TIMSK0 // sets interrupt

#define TIMER_0_SCALAR_ENABLE ((1 << CS00) | (1 << CS01) | (1 << CS02)) // flags for timer 0 scalar
#define TIMER_0_INTERR_ENABLE (1 << OCIE0A) // flags for timer 0 interrupt
#define TIMER_0_INCREM_ENABLE (1 << WGM01) // flags for timer 0 increment

/**
 * sets scalar for timer 0
 * 
 * @param scalar prescalar_enum_t type for scalar value
 */
#define TIMER_0_SET_SCALAR(scalar) \
	if (scalar == SCALAR_1 || scalar == SCALAR_64 || scalar == SCALAR_1024) { \
		TIMER_0_SCAL |= (1 << CS00); \
	} \
	if (scalar == SCALAR_8 || scalar == SCALAR_64) { \
		TIMER_0_SCAL |= (1 << CS01); \
	} \
	if (scalar == SCALAR_256 || scalar == SCALAR_1024) { \
		TIMER_0_SCAL |= (1 << CS02); \
	}

ISR(TIMER0_COMPA_vect) {
	#if SKIP_TIMER_INDEX != 0
		((void(*)())hardTimerFunctions[0])(hardTimerParams[0]);
	#endif
}

#endif

/****************************
 * Timer 1
****************************/

#if TIMER_1_ALIAS != UHWT_TIMER_INVALID_LIT

#define TIMER_1_COMP TCCR1A // sets compare mode
#define TIMER_1_WAVEFORM TCCR1B // sets waveform mode
#define TIMER_1_COUNTER TCNT1 // current counter value
#define TIMER_1_TARGET OCR1A // target tick value
#define TIMER_1_INCR TCCR1B // sets increment mode
#define TIMER_1_SCAL TCCR1B // sets scalar mode
#define TIMER_1_INTERR TIMSK1 // sets interrupt

#define TIMER_1_SCALAR_ENABLE ((1 << CS10) | (1 << CS11) | (1 << CS12)) // flags for timer 1 scalar
#define TIMER_1_INTERR_ENABLE (1 << OCIE1A) // flags for timer 1 interrupt
#define TIMER_1_INCREM_ENABLE (1 << WGM12) // flags for timer 1 increment

/**
 * sets scalar for timer 1
 * 
 * @param scalar prescalar_enum_t type for scalar value
 */
#define TIMER_1_SET_SCALAR(scalar) \
	if (scalar == SCALAR_1 || scalar == SCALAR_64 || scalar == SCALAR_1024) { \
		TIMER_1_SCAL |= (1 << CS10); \
	} \
	if (scalar == SCALAR_8 || scalar == SCALAR_64) { \
		TIMER_1_SCAL |= (1 << CS11); \
	} \
	if (scalar == SCALAR_256 || scalar == SCALAR_1024) { \
		TIMER_1_SCAL |= (1 << CS12); \
	}

ISR(TIMER1_COMPA_vect) {
	#if SKIP_TIMER_INDEX != 1
		#if SKIP_TIMER_INDEX < 1
			((void(*)())hardTimerFunctions[0])(hardTimerParams[0]);
		#else
			((void(*)())hardTimerFunctions[1])(hardTimerParams[1]);
		#endif
	#endif
}

#endif

/****************************
 * Timer 2
****************************/

#if TIMER_2_ALIAS != UHWT_TIMER_INVALID_LIT

#define TIMER_2_COMP TCCR2A // sets compare mode
#define TIMER_2_WAVEFORM TCCR2B // sets waveform mode
#define TIMER_2_COUNTER TCNT2 // current counter value
#define TIMER_2_TARGET OCR2A // target tick value
#define TIMER_2_INCR TCCR2A // sets increment mode
#define TIMER_2_SCAL TCCR2B // sets scalar mode
#define TIMER_2_INTERR TIMSK2 // sets interrupt

#define TIMER_2_SCALAR_ENABLE ((1 << CS20) | (1 << CS21) | (1 << CS22)) // flags for timer 2 scalar
#define TIMER_2_INTERR_ENABLE (1 << OCIE2A) // flags for timer 2 interrupt
#define TIMER_2_INCREM_ENABLE (1 << WGM21) // flags for timer 2 increment

/**
 * sets scalar for timer 2
 * 
 * @param scalar prescalar_enum_t type for scalar value
 */
#define TIMER_2_SET_SCALAR(scalar) \
	if (scalar == SCALAR_1 || scalar == SCALAR_32 || scalar == SCALAR_128 || scalar == SCALAR_1024) { \
		TIMER_2_SCAL |= (1 << CS20); \
	} \
	if (scalar == SCALAR_8 || scalar == SCALAR_32 || scalar == SCALAR_256 || scalar == SCALAR_1024) { \
		TIMER_2_SCAL |= (1 << CS21); \
	} \
	if (scalar == SCALAR_64 || scalar == SCALAR_128 || scalar == SCALAR_256 || scalar == SCALAR_1024) { \
		TIMER_2_SCAL |= (1 << CS22); \
	}

ISR(TIMER2_COMPA_vect) {
	#if SKIP_TIMER_INDEX != 2
		#if SKIP_TIMER_INDEX < 2
			((void(*)())hardTimerFunctions[1])(hardTimerParams[1]);
		#else
			((void(*)())hardTimerFunctions[2])(hardTimerParams[2]);
		#endif
	#endif
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
	HARD_TIMER_CONCATENATE3(TIMER_, num, _SCAL) &= ~HARD_TIMER_CONCATENATE3(TIMER_, num, _SCALAR_ENABLE); \
	HARD_TIMER_CONCATENATE3(TIMER_, num, _INTERR) &= ~HARD_TIMER_CONCATENATE3(TIMER_, num, _INTERR_ENABLE); \
	sei()

bool cancelHardTimer(uhwt_timer_t timer) {

	if (uhwtTimerStarted(timer)) {
		#if UHWT_TIMER_COUNT > 0
			if (timer == UHWT_TIMER0) {
				#if SKIP_TIMER_INDEX != 0
					CANCEL_HARD_TIMER(0);
				#else
					CANCEL_HARD_TIMER(1);
				#endif
			}
		#endif
		#if UHWT_TIMER_COUNT > 1
			else if (timer == UHWT_TIMER1) {
				#if SKIP_TIMER_INDEX < 1
					CANCEL_HARD_TIMER(2);
				#else
					CANCEL_HARD_TIMER(1);
				#endif
			}
		#endif
		#if UHWT_TIMER_COUNT > 2
			else if (timer == UHWT_TIMER2) {
				#if SKIP_TIMER_INDEX < 2
					CANCEL_HARD_TIMER(3);
				#else
					CANCEL_HARD_TIMER(2);
				#endif
			}
		#endif
		uhwtStopTimer(timer);
		return true;
	}

	return false;
}

/**
 * Sets hard timer
 * 
 * @param num timer number
 */
#define SET_HARD_TIMER(num, scalar, timerTicks, function, params) \
	cli(); \
	HARD_TIMER_CONCATENATE3(TIMER_, num, _COMP) = 0; \
	HARD_TIMER_CONCATENATE3(TIMER_, num, _WAVEFORM) = 0; \
	HARD_TIMER_CONCATENATE3(TIMER_, num, _COUNTER) = 0; \
	HARD_TIMER_CONCATENATE3(TIMER_, num, _TARGET) = timerTicks; \
	HARD_TIMER_CONCATENATE3(TIMER_, num, _INCR) |= HARD_TIMER_CONCATENATE3(TIMER_, num, _INCREM_ENABLE); \
	HARD_TIMER_CONCATENATE3(TIMER_, num, _SET_SCALAR)(scalar); \
	HARD_TIMER_CONCATENATE3(TIMER_, num, _INTERR) |= HARD_TIMER_CONCATENATE3(TIMER_, num, _INTERR_ENABLE); \
	sei()

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

		uhwtSetCallbackParams(*timer, function, params);

		#if UHWT_TIMER_COUNT > 0
			if (*timer == UHWT_TIMER0) {
				#if SKIP_TIMER_INDEX != 0
					SET_HARD_TIMER(0, scalar, timerTicks, function, params);
				#else
					SET_HARD_TIMER(1, scalar, timerTicks, function, params);
				#endif
			}
		#endif
		#if UHWT_TIMER_COUNT > 1
			else if (*timer == UHWT_TIMER1) {
				#if SKIP_TIMER_INDEX < 1
					SET_HARD_TIMER(2, scalar, timerTicks, function, params);
				#else
					SET_HARD_TIMER(1, scalar, timerTicks, function, params);
				#endif
			}
		#endif
		#if UHWT_TIMER_COUNT > 2
			else if (*timer == UHWT_TIMER2) {
				#if SKIP_TIMER_INDEX < 2
					SET_HARD_TIMER(3, scalar, timerTicks, function, params);
				#else
					SET_HARD_TIMER(2, scalar, timerTicks, function, params);
				#endif
			}
		#endif

		uhwtStartTimer(*timer);
		return true;
	}

	return false;
}

#endif