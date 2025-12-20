/*
	hardware_timer_priv.h - private hardware timer interface for microcontrollers
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

#ifndef HARDWARE_TIMER_PRIV_H
#define HARDWARE_TIMER_PRIV_H

#include "../universal_hardware_timer.h"

/**
 * Macro for contatenating flags as numbers
 * 
 * @param a first input
 * @param b second input
 * 
 * @return contatenated value
 * 
 * @note Example:
 * @note #define NUMBER 1
 * @note #define FUNCTION(id) CONCATENATE(function, id)
 * @note FUNCTION(NUMBER)
 * @note //returns 'function1' instead of 'functionNUMBER'
 */
#define HARD_TIMER_CONCATENATE(a, b) a ## b

/**
 * Macro for contatenating 3 flags as numbers
 * 
 * @param a first input
 * @param b second input
 * @param c third input
 * 
 * @return contatenated value
 * 
 * @note Example:
 * @note #define NUMBER 1
 * @note #define FUNCTION(id) CONCATENATE(function, id, name)
 * @note FUNCTION(NUMBER)
 * @note //returns 'function1name' instead of 'functionNUMBERname'
 */
#define HARD_TIMER_CONCATENATE3(a, b, c) a ## b ## c

#if UHWT_SUPPORT_ESP32
	#include <esp_idf_version.h>
	#if ESP_IDF_VERSION_MAJOR == 4
		#include <driver/timer.h>
		typedef timer_isr_t uhwt_platform_callback_ptr_t; // specific platform callback pointer type
	#elif ESP_IDF_VERSION_MAJOR == 5
		#include <driver/gptimer.h>
		typedef gptimer_alarm_cb_t uhwt_platform_callback_ptr_t; // specific platform callback pointer type
	#else
		#error "Must use esp-idf version 4.X.X - 5.X.X"
	#endif
	#define UHWT_PRIORITY_SUPPORT // hardware timer allows setting execution priority
#elif UHWT_SUPPORT_PICO
	#include <pico.h>
	#include <pico/time.h>
	typedef repeating_timer_callback_t uhwt_platform_callback_ptr_t; // specific platform callback pointer type
#elif UHWT_SUPPORT_AVR
	typedef void* uhwt_platform_callback_ptr_t; // specific platform callback pointer type
	#define UHWT_NO_CALLBACK_SUPPORT // hardware timer doesn't use callbacks
	#define UHWT_CONFIGS_NOT_EQUAL // hardware timers don't have equal configurations
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Gets callback function used for setting timer
 * 
 * @param timer timer to get
 * 
 * @return pointer to callback function
 */
uhwt_platform_callback_ptr_t uhwtGetCallback(uhwt_timer_t timer);

/**
 * Sets timer as started
 * 
 * @param timer timer to start
 * 
 * @return if start was successful
 */
bool uhwtStartTimer(uhwt_timer_t timer);

/**
 * Sets timer as stopped
 * 
 * @param timer timer to stop
 * 
 * @return if stop was successful
 */
bool uhwtStopTimer(uhwt_timer_t timer);

/**
 * Gets next timer according to claim arguments
 * 
 * @param claimArgs arguments for getting timer
 * 
 * @return timer retrieved
 */
uhwt_timer_t uwhtPlatformGetNextTimerStats(uhwt_claim_s claimArgs);

/**
 * Tests if timer presets equal the target frequency
 * 
 * @param targetFreq frequency to test
 * @param scalar pre scalar for timer
 * @param ticks timer ticks per cycle
 * 
 * @return if presets set equal to timer
 */
bool uhwtPlatformEqualFreq(uhwt_freq_t targetFreq, uhwt_prescalar_t scalar, uhwt_timertick_t ticks);

/**
 * Tests if given pre scalar is valid for timer
 * 
 * @param timer timer to test
 * @param scalar pre scalar to test
 * 
 * @return if pre scalar is valid
 */
bool uhwtValidPreScalar(uhwt_timer_t timer, uhwt_prescalar_t scalar);

/**
 * Tests if given timer ticks is valid for timer
 * 
 * @param timer timer to test
 * @param ticks timer ticks to test
 * 
 * @return if timer ticks is valid
 */
bool uhwtValidTimerTicks(uhwt_timer_t timer, uhwt_timertick_t ticks);

/**
 * Gets next pre scalar value
 * 
 * @param prevScalar pre scalar to increment
 * 
 * @return next pre scalar
 */
uhwt_prescalar_t uhwtGetNextPreScalar(uhwt_prescalar_t prevScalar);

/**
 * Calculates timer ticks
 * 
 * @param targetFreq target frequency to achieve
 * @param scalar preset pre scalar
 * 
 * @return computed timer ticks
 */
uhwt_timertick_t uhwtCalcTicks(uhwt_freq_t targetFreq, uhwt_prescalar_t scalar);

/**
 * Calculates timer pre scalar
 * 
 * @param targetFreq target frequency to achieve
 * @param ticks preset timer ticks
 * 
 * @return computed pre scalar
 */
uhwt_prescalar_t uhwtCalcScalar(uhwt_freq_t targetFreq, uhwt_timertick_t ticks);

/**
 * Sets function to execute for timer ISR
 * 
 * @param timer timer to set
 * @param function function to set
 * @param params params to set
 * 
 * @return if successfully set
 */
bool uhwtPlatformSetCallbackParams(uhwt_timer_t timer,
		uhwt_function_ptr_t function, uhwt_params_ptr_t params);

#ifdef __cplusplus
}
#endif

#ifdef UHWT_NO_CALLBACK_SUPPORT
	extern uhwt_function_ptr_t hardTimerFunctions[UHWT_TIMER_COUNT];
	extern uhwt_params_ptr_t hardTimerParams[UHWT_TIMER_COUNT];
#endif

#ifdef UHWT_PRIORITY_SUPPORT
	extern uhwt_priority_t uhwtPriorities[UHWT_TIMER_COUNT]; // priorities of timer execution
#endif

#endif