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

#include "hardware_timer.h"

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

#if HARDWARE_TIMER_SUPPORT_AVR
	#define HARDWARE_TIMER_NO_CALLBACK_SUPPORT // hardware timer doesn't use callbacks
#endif

#ifdef HARDWARE_TIMER_NO_CALLBACK_SUPPORT
	extern hard_timer_function_ptr_t hardTimerFunctions[HARD_TIMER_COUNT];
	extern void* hardTimerParams[HARD_TIMER_COUNT];
#endif

#endif