/*
	universal_hardware_timer.h - hardware timer interface for microcontrollers
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

#ifndef UNIVERSAL_HARDWARE_TIMER_H
#define UNIVERSAL_HARDWARE_TIMER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

/****************************
 * Library types and constants
****************************/

typedef uint32_t uhwt_freq_t; // hard timer frequency variable
typedef uint8_t uhwt_priority_t; // hard timer execute priority variable
typedef void (*uhwt_function_ptr_t) (void*); // timer callback function pointer
typedef void* uhwt_params_ptr_t; // timer callback parameter type

// hardware timer priority for claiming timers
typedef struct {
	bool slowestTimer: 1; // whether to use slowest timer or not
	bool mostAccurateTimer: 1; // whether to use most accurate timer or not
} uhwt_claim_s;

#define UHWT_PRIORITY_DEFAULT 0 // default hard timer priority
#define UHWT_TIMER_MAX_COUNT 16 // max number of hardware timers supported in library
#define UHWT_TIMER_INVALID_LIT -1 // literal number version of UHWT_TIMER_INVALID

/****************************
 * Platform detection
****************************/

// if a supported avr platform is selected
#define UHWT_SUPPORT_AVR ( \
	defined(__AVR_ATmega328P__) \
)

// if a supported esp32 platform is selected
#define UHWT_SUPPORT_ESP32 ( \
	defined(ESP32) \
)

// if a supported pico platform is selected
#define UHWT_SUPPORT_PICO ( \
	defined(PICO_RP2040) || \
	defined(PICO_RP2350) \
)

// if a supported platform is selected
#define UHWT_SUPPORT ( \
	UHWT_SUPPORT_AVR || \
	UHWT_SUPPORT_ESP32 || \
	UHWT_SUPPORT_PICO \
)

/****************************
 * Platform defines
****************************/

#if UHWT_SUPPORT_ESP32

	/****************************
	 * Timer Config
	 * 
	 * Only 4 hardware timers available
	****************************/

	#define UHWT_TIMER_FREQ_MAX 200000 // max frequency user set timer can be
	#define UHWT_TIMER_COUNT 4 // amount of hardware timers to use

#elif UHWT_SUPPORT_PICO

	/****************************
	 * Timer Config
	 * 
	 * Only 16 hardware alarm timers available
	 * 
	 * 2 are used for system timing on each core
	 * 
	 * Only 14 timers after api usage
	****************************/

	#define UHWT_TIMER_FREQ_MAX 250000 // max frequency user set timer can be
	#define UHWT_TIMER_COUNT 14 // amount of hardware timers to use

#elif UHWT_SUPPORT_AVR

	/****************************
	 * Timer Config
	 * 
	 * Only 3 hardware timers available
	 * 
	 * 1 is used for Arduino timing
	 * 
	 * Only 2 timers after api usage, unless overriden
	****************************/

	#if F_CPU == 16000000L
		#define UHWT_TIMER_FREQ_MAX 120000 // max frequency user set timer can be
	#else
		#error "F_CPU must be 16 (MHz)"
	#endif

	#ifdef UHWT_OVERRIDE_ARDUINO_TIMER
		#define UHWT_TIMER_COUNT 3 // amount of hardware timers to use
	#else
		#define UHWT_TIMER_COUNT 2 // amount of hardware timers to use
	#endif

#else

	/****************************
	 * Timer Config
	 * 
	 * No hardware timers available
	****************************/

	#define UHWT_TIMER_FREQ_MAX 0 // max frequency user set timer can be

	#ifndef UHWT_TIMER_COUNT
		#define UHWT_TIMER_COUNT 0 // amount of hardware timers to use
	#endif

#endif

typedef enum { // hardware timer enum type
	UHWT_TIMER_INVALID = -1, // invalid counter
	#if UHWT_TIMER_COUNT >= 1
		UHWT_TIMER0,
	#endif
	#if UHWT_TIMER_COUNT >= 2
		UHWT_TIMER1,
	#endif
	#if UHWT_TIMER_COUNT >= 3
		UHWT_TIMER2,
	#endif
	#if UHWT_TIMER_COUNT >= 4
		UHWT_TIMER3,
	#endif
	#if UHWT_TIMER_COUNT >= 5
		UHWT_TIMER4,
	#endif
	#if UHWT_TIMER_COUNT >= 6
		UHWT_TIMER5,
	#endif
	#if UHWT_TIMER_COUNT >= 7
		UHWT_TIMER6,
	#endif
	#if UHWT_TIMER_COUNT >= 8
		UHWT_TIMER7,
	#endif
	#if UHWT_TIMER_COUNT >= 9
		UHWT_TIMER8,
	#endif
	#if UHWT_TIMER_COUNT >= 10
		UHWT_TIMER9,
	#endif
	#if UHWT_TIMER_COUNT >= 11
		UHWT_TIMER10,
	#endif
	#if UHWT_TIMER_COUNT >= 12
		UHWT_TIMER11,
	#endif
	#if UHWT_TIMER_COUNT >= 13
		UHWT_TIMER12,
	#endif
	#if UHWT_TIMER_COUNT >= 14
		UHWT_TIMER13,
	#endif
	#if UHWT_TIMER_COUNT >= 15
		UHWT_TIMER14,
	#endif
	#if UHWT_TIMER_COUNT >= 16
		UHWT_TIMER15,
	#endif
} uhwt_timer_t;

/****************************
 * Library functions
****************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Claims a timer based on functional priorities
 * 
 * @param priority priority of timer to get
 * 
 * @return timer claimed
 */
uhwt_timer_t claimTimer(uhwt_claim_s *priority);

/**
 * Releases claim on a timer
 * 
 * @param timer timer to release
 * 
 * @return if unclaim was successful
 */
bool unclaimTimer(uhwt_timer_t timer);

/**
 * Tests if timer is claimed already or not
 * 
 * @param timer timer to check
 * 
 * @return if timer is claimed or not
 */
bool hardTimerClaimed(uhwt_timer_t timer);

/**
 * Stops hardware timer from executing
 * 
 * @param timer timer to stop
 * 
 * @return if timer was successfully canceled
 */
bool cancelHardTimer(uhwt_timer_t timer);

/**
 * Starts hardware timer execution
 * 
 * @param timer pointer to timer to start
 * @param freq pointer to desired frequency in Hz
 * @param function pointer to function to call back
 * @param params parameters to pass to callback function
 * @param priority priority to run timer at (0 min, 255 max)
 * 
 * setHardTimer(UHWT_TIMER_INVALID, ...):
 * 
 *                Claimed:     Unclaimed:
 * 
 * Started:           -,           -
 * 
 * Not Started:       -,       Best Timer
 * 
 * setHardTimer(UHWT_TIMER#, ...):
 * 
 *                Claimed:     Unclaimed:
 * 
 * Started:         Fail,      Best Timer
 * 
 * Not Started:  UHWT_TIMER#,  UHWT_TIMER#
 * 
 * @return if timer was successfully set
 */
bool setHardTimer(uhwt_timer_t *timer, uhwt_freq_t *freq,
		uhwt_function_ptr_t function, uhwt_params_ptr_t params,
		uhwt_priority_t priority);

/**
 * Gets if selected timer was started
 * 
 * @param timer timer to check
 * 
 * @return if timer was started
 */
bool hardTimerStarted(uhwt_timer_t timer);

/**
 * Sets function to execute for timer ISR
 * 
 * @param timer timer to set
 * @param function function to set
 * 
 * @return if successfully set
 */
bool setHardTimerFunction(uhwt_timer_t timer,
		uhwt_function_ptr_t function, uhwt_params_ptr_t params);

/**
 * Tests if timer is claimed
 * 
 * @param timer timer to test
 * 
 * @return claim status
 */
bool uhwtTimerClaimed(uhwt_timer_t timer);

/**
 * Tests if timer is started
 * 
 * @param timer timer to test
 * 
 * @return start status
 */
bool uhwtTimerStarted(uhwt_timer_t timer);

/**
 * Claims next available timer
 * 
 * @param timer pointer to timer to set
 * 
 * @return if successful
 */
bool uhwtClaimTimer(uhwt_timer_t *timer);

/**
 * Claims next available timer
 * 
 * @param timer pointer to timer to set
 * @param claimArgs claim arguments for selecting timer
 * 
 * @return if successful
 */
bool uhwtClaimTimerStats(uhwt_timer_t *timer, uhwt_claim_s claimArgs);

/**
 * Sets timer as unclaimed
 * 
 * @param timer timer to unclaim
 * 
 * @return if unclaim was successful
 */
bool uhwtUnclaimTimer(uhwt_timer_t timer);

#ifdef __cplusplus
}
#endif

#endif