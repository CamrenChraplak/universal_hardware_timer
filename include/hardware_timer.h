/*
	hardware_timer.h - hardware timer interface for microcontrollers
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

#ifndef HARDWARE_TIMER_H
#define HARDWARE_TIMER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

/****************************
 * Library types and constants
****************************/

typedef uint32_t hard_timer_freq_t; // hard timer frequency variable
typedef uint8_t hard_timer_priority_t; // hard timer execute priority variable

// hardware timer priority for claiming timers
typedef struct {
	bool slowestTimer: 1; // whether to use slowest timer or not
	bool mostAccurateTimer: 1; // whether to use most accurate timer or not
} hard_timer_claim_s;

// codes when getting hard timer stats
typedef enum {
	HARD_TIMER_OK, // hard timer stats retrieved
	HARD_TIMER_SLIGHTLY_OFF, // retrieved values that aren't completely accurate
	HARD_TIMER_FAIL, // failed to get timer values
} hard_timer_status_t;

typedef void (*hard_timer_function_ptr_t) (void*); // timer callback function pointer

#define HARD_TIMER_PRIORITY_DEFAULT 0 // default hard timer priority
#define HARD_TIMER_MAX_COUNT 16 // max number of hardware timers supported in library
#define HARD_TIMER_INVALID_LIT -1 // literal number version of HARD_TIMER_INVALID

/****************************
 * Platform detection
****************************/

// if a supported avr platform is selected
#define HARDWARE_TIMER_SUPPORT_AVR ( \
	defined(__AVR_ATmega328P__) \
)

// if a supported esp32 platform is selected
#define HARDWARE_TIMER_SUPPORT_ESP32 ( \
	defined(ESP32) \
)

// if a supported pico platform is selected
#define HARDWARE_TIMER_SUPPORT_PICO ( \
	defined(PICO_RP2040) || \
	defined(PICO_RP2350) \
)

// if a supported platform is selected
#define HARDWARE_TIMER_SUPPORT ( \
	HARDWARE_TIMER_SUPPORT_AVR || \
	HARDWARE_TIMER_SUPPORT_ESP32 || \
	HARDWARE_TIMER_SUPPORT_PICO \
)

/****************************
 * Platform defines
****************************/

#if HARDWARE_TIMER_SUPPORT_ESP32

	/****************************
	 * Timer Config
	 * 
	 * Only 4 hardware timers available
	****************************/

	#include <esp_idf_version.h>

	#define HARD_TIMER_FREQ_MAX 200000 // max frequency user set timer can be
	#define HARD_TIMER_COUNT 4 // amount of hardware timers to use

	#if ESP_IDF_VERSION_MAJOR == 4
		#include <driver/timer.h>
		typedef timer_isr_t hard_timer_callback_ptr_t; // callback pointer type
	#elif ESP_IDF_VERSION_MAJOR == 5
		#include <driver/gptimer.h>
		typedef gptimer_alarm_cb_t hard_timer_callback_ptr_t; // callback pointer type
	#else
		#error "Must use esp-idf version 4.X.X - 5.X.X"
	#endif

#elif HARDWARE_TIMER_SUPPORT_PICO

	/****************************
	 * Timer Config
	 * 
	 * Only 16 hardware alarm timers available
	 * 
	 * 2 are used for system timing on each core
	 * 
	 * Only 14 timers after api usage
	****************************/

	#define HARD_TIMER_FREQ_MAX 333333 // max frequency user set timer can be
	#define HARD_TIMER_COUNT 14 // amount of hardware timers to use

	#include <pico.h>
	#include <pico/time.h>
	typedef repeating_timer_callback_t hard_timer_callback_ptr_t; // callback pointer type

#elif HARDWARE_TIMER_SUPPORT_AVR

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
		#define HARD_TIMER_FREQ_MAX 120000 // max frequency user set timer can be
	#else
		#error "F_CPU must be (MHz) 16"
	#endif

	#ifdef OVERRIDE_ARDUINO_TIMER
		#define HARD_TIMER_COUNT 3 // amount of hardware timers to use
	#else
		#define HARD_TIMER_COUNT 2 // amount of hardware timers to use
	#endif

	typedef void* hard_timer_callback_ptr_t; // callback pointer type
	#define NO_TIMER_CALLBACK_SUPPORT // hardware timer doesn't use callbacks

#else

	/****************************
	 * Timer Config
	 * 
	 * No hardware timers available
	****************************/

	#define HARD_TIMER_FREQ_MAX 0 // max frequency user set timer can be

	#ifndef HARD_TIMER_COUNT
		#define HARD_TIMER_COUNT 0 // amount of hardware timers to use
	#endif

#endif

typedef enum { // hardware timer enum type
	HARD_TIMER_INVALID = -1, // invalid counter
	#if HARD_TIMER_COUNT >= 1
		HARD_TIMER0,
	#endif
	#if HARD_TIMER_COUNT >= 2
		HARD_TIMER1,
	#endif
	#if HARD_TIMER_COUNT >= 3
		HARD_TIMER2,
	#endif
	#if HARD_TIMER_COUNT >= 4
		HARD_TIMER3,
	#endif
	#if HARD_TIMER_COUNT >= 5
		HARD_TIMER4,
	#endif
	#if HARD_TIMER_COUNT >= 6
		HARD_TIMER5,
	#endif
	#if HARD_TIMER_COUNT >= 7
		HARD_TIMER6,
	#endif
	#if HARD_TIMER_COUNT >= 8
		HARD_TIMER7,
	#endif
	#if HARD_TIMER_COUNT >= 9
		HARD_TIMER8,
	#endif
	#if HARD_TIMER_COUNT >= 10
		HARD_TIMER9,
	#endif
	#if HARD_TIMER_COUNT >= 11
		HARD_TIMER10,
	#endif
	#if HARD_TIMER_COUNT >= 12
		HARD_TIMER11,
	#endif
	#if HARD_TIMER_COUNT >= 13
		HARD_TIMER12,
	#endif
	#if HARD_TIMER_COUNT >= 14
		HARD_TIMER13,
	#endif
	#if HARD_TIMER_COUNT >= 15
		HARD_TIMER14,
	#endif
	#if HARD_TIMER_COUNT >= 16
		HARD_TIMER15,
	#endif
} hard_timer_enum_t;

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
hard_timer_enum_t claimTimer(hard_timer_claim_s *priority);

/**
 * Releases claim on a timer
 * 
 * @param timer timer to release
 * 
 * @return if unclaim was successful
 */
bool unclaimTimer(hard_timer_enum_t timer);

/**
 * Tests if timer is claimed already or not
 * 
 * @param timer timer to check
 * 
 * @return if timer is claimed or not
 */
bool hardTimerClaimed(hard_timer_enum_t timer);

/**
 * Stops hardware timer from executing
 * 
 * @param timer timer to stop
 * 
 * @return if timer was successfully canceled
 */
bool cancelHardTimer(hard_timer_enum_t timer);

/**
 * Starts hardware timer execution
 * 
 * @param timer pointer to timer to start
 * @param freq pointer to desired frequency in Hz
 * @param function pointer to function to call back
 * @param params parameters to pass to callback function
 * @param priority priority to run timer at (0 min, 255 max)
 * 
 * setHardTimer(HARD_TIMER_INVALID, ...):
 * 
 *                Claimed:     Unclaimed:
 * 
 * Started:           -,           -
 * 
 * Not Started:       -,       Best Timer
 * 
 * setHardTimer(HARD_TIMER#, ...):
 * 
 *                Claimed:     Unclaimed:
 * 
 * Started:         Fail,      Best Timer
 * 
 * Not Started:  HARD_TIMER#,  HARD_TIMER#
 * 
 * @return if timer was successfully set
 */
bool setHardTimer(hard_timer_enum_t *timer, hard_timer_freq_t *freq,
		hard_timer_function_ptr_t function, void* params,
		hard_timer_priority_t priority);

/**
 * Gets if selected timer was started
 * 
 * @param timer timer to check
 * 
 * @return if timer was started
 */
bool hardTimerStarted(hard_timer_enum_t timer);

/**
 * Sets function to execute for timer ISR
 * 
 * @param timer timer to set
 * @param function function to set
 * 
 * @return if successfully set
 */
bool setHardTimerFunction(hard_timer_enum_t timer,
		hard_timer_function_ptr_t function, void* params);

/**
 * Gets callback function used for setting timer
 * 
 * @param timer timer to get
 * 
 * @return pointer to callback function
 */
hard_timer_callback_ptr_t getHardTimerCallback(hard_timer_enum_t timer);

#ifdef __cplusplus
}
#endif

#endif