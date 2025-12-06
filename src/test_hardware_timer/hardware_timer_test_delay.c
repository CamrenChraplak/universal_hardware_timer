/*
	hardware_timer_test_delay.c - delay methods for timer
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

#include "hardware_timer_test_priv.h"

void delaySeconds(uint8_t seconds) __attribute__((weak));

#if defined(ARDUINO) && !defined(OVERRIDE_ARDUINO_TIMER)

#include <Arduino.h>

void delaySeconds(uint8_t seconds) {
	delay(seconds * 1000);
}

#else

#if HARDWARE_TIMER_SUPPORT_AVR
	#include "../avr/hardware_timer_avr.h"
#endif

/**
 * Interrupt function for delay
 * 
 * @param params parameters passed
 */
void timerDelayCounter(void *params) {
	*(volatile uint8_t*)params += 1;
}

void delaySeconds(uint8_t seconds) {
	hard_timer_enum_t timer = HARD_TIMER_INVALID;
	volatile uint8_t delayCount = 0U;

	hard_timer_freq_t freq =
	#ifdef FREQ_MIN_8_COUNTER
		FREQ_MIN_8_COUNTER;
	#else
		1;
	#endif

	if (!setHardTimer(&timer, &freq, &timerDelayCounter, (void*)&delayCount, HARD_TIMER_PRIORITY_DEFAULT)) {
		cancelHardTimer(timer);
		return;
	}

	while (delayCount < seconds * freq) {
		__asm__ __volatile__ ("nop");
	}
	cancelHardTimer(timer);
}

#endif