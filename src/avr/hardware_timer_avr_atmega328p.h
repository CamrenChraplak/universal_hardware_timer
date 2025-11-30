/*
	hardware_timer_avr_atmega328p.h - timer configuration for atmega328p
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
 * F_CPU = 16,000,000
 * 
 * F_DER = desired frequency (Hz)
 * 
 * timerTicks = [F_CPU / (scalar * F_DER)] - 1
 * F_DER = F_CPU / [scalar * (timerTicks + 1)]
 * 
 * 8-bit counter for timer 0,2 and 16-bit for timer 1
 * scalars: 1, 8, 64, 256, 1024. timer 2 additional scalars: 32, 128
 */

#define SCALAR_1_ENABLE
#define SCALAR_8_ENABLE
#define SCALAR_32_ENABLE
#define SCALAR_64_ENABLE
#define SCALAR_128_ENABLE
#define SCALAR_256_ENABLE
#define SCALAR_1024_ENABLE

#ifndef OVERRIDE_ARDUINO_TIMER
	#define SKIP_TIMER_INDEX 0
#endif