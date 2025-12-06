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

#if defined(ARDUINO) && !defined(OVERRIDE_ARDUINO_TIMER)

#include <Arduino.h>

void delaySeconds(uint8_t seconds) {
	delay(seconds * 1000);
}

#else

void delaySeconds(uint8_t seconds) {
	//TODO: delay for non Arduino
}

#endif