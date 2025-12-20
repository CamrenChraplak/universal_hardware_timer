/*
	hardware_timer_avr.h - timer configuration for avr
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
 * Arduino functionality that uses hardware timers
 * 
 * wiring.c
 * 		millis()
 * 			Uses one timer
 * wiring_analog.c
 * 		PWM/AnalogWrite
 * 			Uses all timers if all PWM pins used
 * Tone.cpp
 * 		Tone generation
 * 			Uses one timer
 */

#include "../private/hardware_timer_priv.h"

#if UHWT_SUPPORT_AVR

#if F_CPU == 16000000L
	#define FREQ_MIN_8_COUNTER 62 // min frequency for 8 bit counter
#else
	#error "Frequency not supported"
#endif

#if defined(__AVR_ATmega328P__)
	#include "hardware_timer_avr_atmega328p.h"
#endif

typedef enum {
	SCALAR_0, // timer prescalar of 0, represents invalid
	SCALAR_1, // timer prescalar of 1
	SCALAR_8, // timer prescalar of 8
	SCALAR_32, // timer prescalar of 32
	SCALAR_64, // timer prescalar of 64
	SCALAR_128, // timer prescalar of 128
	SCALAR_256, // timer prescalar of 256
	SCALAR_1024, // timer prescalar of 1024
} prescalar_enum_t; // pre scalar type

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

#endif

#endif