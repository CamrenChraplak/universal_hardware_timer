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
typedef uint16_t prescalar_t; // scalar type
typedef uint16_t timertick_t; // timer tick type

#if UHWT_TIMER_COUNT <= 4
	uint8_t timerStates = 0U; // started states and claimed states
#elif UHWT_TIMER_COUNT <= 8
	uint16_t timerStates = 0U; // started states and claimed states
#elif UHWT_TIMER_COUNT <= 16
	uint32_t timerStates = 0U; // started states and claimed states
#elif UHWT_TIMER_COUNT <= 32
	uint64_t timerStates = 0U; // started states and claimed states
#else
	#error "Too many timers"
#endif

const prescalar_t scalarMask[] PROGMEM = {
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

#define SCALAR_MASK_SIZE (sizeof(scalarMask) / sizeof(prescalar_t)) // size of scalarMask

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

/****************************
 * Timer Functions
****************************/

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

/**
 * Calculates timer ticks from scalar value and frequency
 * 
 * @param scalar scalar enum
 * @param freq frequency to get ticks for
 * 
 * @return calculated timer ticks
 */
timertick_t calculateTicks(prescalar_enum_t scalar, uhwt_freq_t freq) {
	return (F_CPU / (getMask(scalar) * freq)) - 1;
}

/**
 * Calculates timer frequency from scalar value and timer tick value
 * 
 * @param scalar scalar enum
 * @param timerTicks timer ticks to count for
 * 
 * @return calculated frequency
 */
uhwt_freq_t calculateFreq(prescalar_enum_t scalar, timertick_t timerTicks) {
	return F_CPU / ((uhwt_freq_t)getMask(scalar) * (timerTicks + 1));
}

/**
 * Sets timer started state
 * 
 * @param timer timer to set
 * @param state whether or not timer is started
 */
void setTimerStarted(uhwt_timer_t timer, bool state) {

	if (timer == UHWT_TIMER_INVALID) {
		return;
	}
	if (state) {
		timerStates |= (1 << timer);
	}
	else {
		timerStates &= (~(1 << timer));
	}
}

/**
 * Sets timer claimed state
 * 
 * @param timer timer to set
 * @param state whether or not timer is claimed
 */
void setTimerClaimed(uhwt_timer_t timer, bool state) {

	if (timer == UHWT_TIMER_INVALID) {
		return;
	}
	if (state) {
		timerStates |= (1 << (timer + UHWT_TIMER_COUNT));
	}
	else {
		timerStates &= (~(1 << (timer + UHWT_TIMER_COUNT)));
	}
}

/**
 * Tests if given scalar and timer ticks equal a given frequency
 * 
 * @param freq target frequency
 * @param scalar scalar value
 * @param ticks timer tick count
 * 
 * @return if parameters generate frequency
 */
bool sameFreq(uhwt_freq_t freq, prescalar_enum_t scalar, timertick_t ticks) {

	if (F_CPU % ((uhwt_freq_t)getMask(scalar) * (ticks + 1)) != 0) {
		return false;
	}
	if (calculateFreq(scalar, ticks) != freq) {
		return false;
	}
	return true;
}

/**
 * Gets stats for any timer
 * 
 * @param freq pointer to target frequency
 * @param timer timer to test for
 * @param scalar pointer to scalar value
 * @param timerTicks pointer to timer tick count
 */
void getStats(uhwt_freq_t *freq, uhwt_timer_t timer, prescalar_enum_t *scalar, timertick_t *timerTicks) {

	*scalar = SCALAR_1;
	*timerTicks = 0;

	uhwt_freq_t closestFreq = 0;

	for (uint8_t i = SCALAR_MASK_SIZE - 1; i < SCALAR_MASK_SIZE; i--) {

		// ignore invalid scalars
		if ((i == SCALAR_32 || i == SCALAR_128) && timer != TIMER_2_ALIAS) {
			continue;
		}

		// ignore invalid ticks
		if ((timertick_t)(F_CPU / (getMask(i) * *freq)) < 1) {
			continue;
		}
		if (calculateTicks(i, *freq) > UINT16_MAX) {
			continue;
		}
		if (calculateTicks(i, *freq) > UINT8_MAX && timer != TIMER_1_ALIAS) {
			continue;
		}
		timertick_t calcTicks = calculateTicks(i, *freq);

		// frequency is exact value
		if (sameFreq(*freq, i, calcTicks)) {
			*scalar = i;
			*timerTicks = calcTicks;
			return;
		}

		// test if newly calculated frequency is closer
		if (abs(*freq - closestFreq) > abs(*freq - calculateFreq(i, calcTicks))) {
			*scalar = (prescalar_enum_t)i;
			*timerTicks = calcTicks;
			closestFreq = calculateFreq(i, calcTicks);
		}
	}
	*freq = closestFreq;
}

bool hardTimerClaimed(uhwt_timer_t timer) {

	if (timer == UHWT_TIMER_INVALID) {
		return false;
	}
	return (!!((1 << (UHWT_TIMER_COUNT + timer)) & timerStates));
}

/**
 * Tests if given timer is available to claim
 * 
 * @param timer timer to test
 * 
 * @return if timer is available
 */
bool availableClaim(uhwt_timer_t timer) {
	if (!hardTimerClaimed(timer) && !hardTimerStarted(timer)) {
		setTimerClaimed(timer, true);
		return true;
	}
	return false;
}

uhwt_timer_t claimTimer(uhwt_claim_s *priority) {

	// checks priorities
	if (priority -> slowestTimer) {
		if (availableClaim(TIMER_1_ALIAS)) {
			return TIMER_1_ALIAS;
		}
	}
	if (priority -> mostAccurateTimer) {
		if (availableClaim(TIMER_2_ALIAS)) {
			return TIMER_2_ALIAS;
		}
	}

	// uses default order if no priority matched
	for (uint8_t i = 0; i < UHWT_TIMER_COUNT; i++) {
		if (availableClaim(i)) {
			return i;
		}
	}

	return UHWT_TIMER_INVALID;
}

bool unclaimTimer(uhwt_timer_t timer) {
	if (hardTimerClaimed(timer)) {
		setTimerClaimed(timer, false);
		return true;
	}
	return false;
}

/**
 * Sets frequency for first timer to set
 * 
 * @warning all parameters are NON POINTERS
 * 
 * @param origFreq value of user given target frequency
 * @param origTimer timer to set
 * @param newFreq frequency to store calculation and compare against
 * @param newTimer value to set origTimer to
 * @param newTicks ticks to store calculation
 * @param newScalar scalar to store calculation
 */
#define SET_FIRST_FREQ(origFreq, origTimer, newFreq, newTimer, newTicks, newScalar) \
	(origTimer) = (newTimer); \
	getStats(&(newFreq), (origTimer), &(newScalar), &(newTicks)); \
	if (sameFreq((origFreq), (newScalar), (newTicks))) { \
		return HARD_TIMER_OK; \
	}

/**
 * Sets frequency for next timer to set
 * 
 * @warning all parameters are NON POINTERS
 * 
 * @param origFreq value of user given target frequency
 * @param origTimer timer to set
 * @param origTicks ticks to set
 * @param origScalar scalar to set
 * @param newFreq frequency to store calculation and compare against
 * @param newTimer value to set origTimer to
 * @param newTicks ticks to store calculation
 * @param newScalar scalar to store calculation
 */
#define SET_NEXT_FREQ(origFreq, origTimer, origTicks, origScalar, newFreq, newTimer, newTicks, newScalar) \
	uhwt_freq_t calcFreq = (origFreq); \
	getStats(&calcFreq, (newTimer), &(newScalar), &(newTicks)); \
	if (abs((origFreq) - (newFreq)) > abs((origFreq) - calcFreq)) { \
		(newFreq) = calcFreq; \
		(origTimer) = (newTimer); \
		(origScalar) = (newScalar); \
		(origTicks) = (newTicks); \
		if (sameFreq((origFreq), (origScalar), (origTicks))) { \
			(origFreq) = (newFreq); \
			return HARD_TIMER_OK; \
		} \
	}

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
uhwt_status_t getHardTimerStats(uhwt_freq_t *freq, uhwt_timer_t *timer, prescalar_enum_t *scalar, timertick_t *timerTicks) {

	// returns started timer early
	if (hardTimerStarted(*timer) && hardTimerClaimed(*timer)) {
		return HARD_TIMER_FAIL;
	}

	// gets stats for current timer
	if (!hardTimerStarted(*timer) && *timer != UHWT_TIMER_INVALID) {
		SET_FIRST_FREQ(*freq, *timer, *freq, *timer, *timerTicks, *scalar);
		return HARD_TIMER_SLIGHTLY_OFF;
	}

	// gets best available timer
	*timer = UHWT_TIMER_INVALID;

	if (*freq < FREQ_MIN_8_COUNTER) {
		// calculates slow frequencies for timer 1

		if (hardTimerStarted(TIMER_1_ALIAS) || hardTimerClaimed(TIMER_1_ALIAS)) {
			// slow timer unavailable
			return HARD_TIMER_FAIL;
		}

		SET_FIRST_FREQ(*freq, *timer, *freq, TIMER_1_ALIAS, *timerTicks, *scalar);
		return HARD_TIMER_SLIGHTLY_OFF;
	}
	else {
		/**
		 * calculates frequencies for remaining timers
		 * 
		 * first checks timer 0 since its configuration is valid for all timers
		 * then checks timer 1 since it can do slower frequencies than the others
		 * finally checks timer 2 since it can be more accurate
		 */

		prescalar_enum_t tempScalar = SCALAR_1;
		timertick_t tempTicks = 0;
		uhwt_freq_t tempFreq = *freq;

		// gets timer 0
		if (!hardTimerStarted(TIMER_0_ALIAS) && !hardTimerClaimed(TIMER_0_ALIAS)) {
			SET_FIRST_FREQ(*freq, *timer, tempFreq, TIMER_0_ALIAS, *timerTicks, *scalar);
		}

		// gets timer 1
		if (!hardTimerStarted(TIMER_1_ALIAS) && !hardTimerClaimed(TIMER_1_ALIAS)) {

			if (*timer == UHWT_TIMER_INVALID) {
				// timer 0 unavailable
				SET_FIRST_FREQ(*freq, *timer, tempFreq, TIMER_1_ALIAS, *timerTicks, *scalar);
			}
			else {
				// timer 0 available
				SET_NEXT_FREQ(*freq, *timer, *timerTicks, *scalar, tempFreq, TIMER_1_ALIAS, tempTicks, tempScalar);
			}
		}

		// gets timer 2
		if (!hardTimerStarted(TIMER_2_ALIAS) && !hardTimerClaimed(TIMER_2_ALIAS)) {

			if (*timer == UHWT_TIMER_INVALID) {
				// timer 0 and 1 unavailable
				SET_FIRST_FREQ(*freq, *timer, tempFreq, TIMER_2_ALIAS, *timerTicks, *scalar);
			}
			else {
				// timer 0 and/or 1 available
				SET_NEXT_FREQ(*freq, *timer, *timerTicks, *scalar, tempFreq, TIMER_2_ALIAS, tempTicks, tempScalar);
			}
		}

		if (*timer == UHWT_TIMER_INVALID) {
			return HARD_TIMER_FAIL;
		}

		*freq = tempFreq;

		return HARD_TIMER_SLIGHTLY_OFF;
	}
}

bool hardTimerStarted(uhwt_timer_t timer) {

	if (timer == UHWT_TIMER_INVALID) {
		return false;
	}
	return !!((1 << timer) & timerStates);
}

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

	if (hardTimerStarted(timer)) {
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
		setTimerStarted(timer, false);
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
	timertick_t timerTicks;

	if (getHardTimerStats(freq, timer, &scalar, &timerTicks) == HARD_TIMER_FAIL) {
		return false;
	}

	if (!hardTimerStarted(*timer)) {

		setHardTimerFunction(*timer, function, params);

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

		setTimerStarted(*timer, true);
		return true;
	}

	return false;
}

#endif