/*
	hardware_timer_esp32.c - timer configuration for Espressif ESP32
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
 * APB_CLK = 80,000,000Hz
 * 
 * freq = desired frequency (Hz)
 * 
 * freq = APB_CLK / (scalar * timerTicks)
 * 
 * 64-bit counter
 * 16-bit scalar
 */

#include "../private/hardware_timer_priv.h"

#if UHWT_SUPPORT_ESP32

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>

#define TIMER_COUNT_ZERO 0U // value for setting timer tick count to 0
#define SCALAR_MAX UINT16_MAX // max value for timer scalar

#if ESP_IDF_VERSION_MAJOR == 4

#include <driver/timer.h>
#include <esp_intr_alloc.h>

typedef struct hw_timer_s {
	uint8_t group; // timer group
	uint8_t num; // timer number
} hard_timer_group_t;

// stores timer groups and numbers
static hard_timer_group_t timerGroups[4] = {
	{.group=0, .num=0}, // timer0
	{.group=1, .num=0}, // timer1
	{.group=0, .num=1}, // timer2
	{.group=1, .num=1}, // timer3
};

// hardware timer pointers
hard_timer_group_t *timers[] = {
	#if UHWT_TIMER_COUNT >= 1
		NULL,
	#endif
	#if UHWT_TIMER_COUNT >= 2
		NULL,
	#endif
	#if UHWT_TIMER_COUNT >= 3
		NULL,
	#endif
	#if UHWT_TIMER_COUNT >= 4
		NULL,
	#endif
};

typedef hard_timer_group_t** timer_ptr_t;

#elif ESP_IDF_VERSION_MAJOR == 5

#include <soc/clk_tree_defs.h>
#include <driver/gptimer_types.h>
#include <hal/timer_types.h>
#include <driver/gptimer.h>

#if UHWT_TIMER_COUNT >= 1
	gptimer_handle_t handler0 = NULL;
#endif
#if UHWT_TIMER_COUNT >= 2
	gptimer_handle_t handler1 = NULL;
#endif
#if UHWT_TIMER_COUNT >= 3
	gptimer_handle_t handler2 = NULL;
#endif
#if UHWT_TIMER_COUNT >= 4
	gptimer_handle_t handler3 = NULL;
#endif

gptimer_handle_t *timers[] = {
	#if UHWT_TIMER_COUNT >= 1
		&handler0,
	#endif
	#if UHWT_TIMER_COUNT >= 2
		&handler1,
	#endif
	#if UHWT_TIMER_COUNT >= 3
		&handler2,
	#endif
	#if UHWT_TIMER_COUNT >= 4
		&handler3,
	#endif
};

typedef gptimer_handle_t** timer_ptr_t;

// gptimer requires this as minimum freq
#define HARD_TIMER_FREQ_MIN ((APB_CLK_FREQ / SCALAR_MAX) + 1)
//#define HARD_TIMER_FREQ_MIN 1221

#endif

/****************************
 * Platform Functions
****************************/

uhwt_freq_t uhwtCalcFreq(uhwt_prescalar_t scalar, uhwt_timertick_t ticks) {
	if (scalar * ticks == 0) {
		return 0;
	}
	return APB_CLK_FREQ / (scalar * ticks);
}

bool uhwtPlatformEqualFreq(uhwt_freq_t targetFreq, uhwt_prescalar_t scalar, uhwt_timertick_t ticks) {
	if (APB_CLK_FREQ % (scalar * ticks) != 0) {
		return false;
	}
	return true;
}

bool uhwtValidTimerTicks(uhwt_timer_t timer, uhwt_timertick_t ticks) {
	
	/**
	 * Pre Scalar counts (and amount of timers)
	 * 
	 * General Timers: https://docs.espressif.com/projects/arduino-esp32/en/latest/api/timer.html
	 * 
	 * ESP32: 64 (2x2) https://documentation.espressif.com/esp32_datasheet_en.pdf
	 * ESP32-S2: 64 (4? general vs 1? datasheet) https://documentation.espressif.com/esp32-s2_datasheet_en.pdf
	 * ESP32-S3: 54? datasheet vs 64? general (4) https://documentation.espressif.com/esp32-s3_datasheet_en.pdf
	 * ESP32-C2 or ESP-8684: 54 datasheet only (1 datasheet only) https://documentation.espressif.com/esp8684_datasheet_en.pdf
	 * ESP32-C3: 54 (2) https://documentation.espressif.com/esp32-c3_datasheet_en.pdf
	 * ESP32-C5: 54 datasheet only (2 datasheet only) https://documentation.espressif.com/esp32-c5_datasheet_en.pdf
	 * ESP32-C6: 54? datasheet vs 64? general (2) https://documentation.espressif.com/esp32-c6_datasheet_en.pdf
	 * ESP32-C61: 54 datasheet only (2 datasheet only) https://documentation.espressif.com/esp32-c61_datasheet_en.pdf
	 * ESP32-H2: 54? datasheet vs 64? general (2) https://documentation.espressif.com/esp32-h2_datasheet_en.pdf
	 * ESP32-P4: 54 datasheet only (4 datasheet only) https://documentation.espressif.com/esp32-p4-chip-revision-v1.3_datasheet_en.pdf
	 */

	// TODO: 54 bit ticks
	
	return true;
}

uhwt_prescalar_t uhwtGetNextPreScalar(uhwt_prescalar_t prevScalar) {
	// u16 bit max
	// 2^16 - 1 -> 2^15 -> 2^14 -> ... -> 2^2 -> 2^1 -> 1 -> 0 -> 2^16 - 1

	#define MAX_PRESCALAR_BITS 16
	
	// 0: start of request
	if (prevScalar == 0) {
		return UINT16_MAX;
	}
	// 2^16 - 1
	else if (prevScalar == UINT16_MAX) {
		return 0b1000000000000000;
	}
	else {
		prevScalar = prevScalar >> (1);
		for (uint8_t i = 0; i < MAX_PRESCALAR_BITS; i++) {
			if (!(prevScalar ^ (1 << i))) {
				return prevScalar;
			}
		}
	}
	return 0;
}

uhwt_timertick_t uhwtCalcTicks(uhwt_freq_t targetFreq, uhwt_prescalar_t scalar) {
	if (targetFreq * scalar == 0) {
		return 0;
	}
	return APB_CLK_FREQ / (targetFreq * scalar);
}

uhwt_prescalar_t uhwtCalcScalar(uhwt_freq_t targetFreq, uhwt_timertick_t ticks) {
	if (targetFreq * ticks == 0) {
		return 0;
	}
	return APB_CLK_FREQ / (targetFreq * ticks);
}

/****************************
 * Timer Functions
****************************/

/**
 * Scales input priority
 * 
 * @param priority of type uhwt_priority_t
 * 
 * @note function can only run up to priority 'ESP_INTR_FLAG_LEVEL3' since functions are in c
 * 
 * @return priority flag for 'intr_alloc_flags' when calling 'timer_isr_callback_add'
 */
int setPriority(uhwt_priority_t priority) {
	#ifdef PLATFORMIO
		// use input priority for PlatformIO
		int adjustment = priority / (UINT8_MAX / 3);
		#if ESP_IDF_VERSION_MAJOR == 4
			return (1 << adjustment);
		#elif ESP_IDF_VERSION_MAJOR == 5
			return adjustment;
		#else
			return 0;
		#endif
	#else
		// use default priority with esp-idf
		return 0;
	#endif
}

/**
 * Gets timer based on desired timer
 * 
 * @param timer timer to select
 * 
 * @return pointer to timer selected
 */
timer_ptr_t getTimer(uhwt_timer_t timer) {

	if (timer == UHWT_TIMER_INVALID) {
		return NULL;
	}
	return &timers[timer];
}

bool cancelHardTimer(uhwt_timer_t timer) {
	
	if (uhwtTimerStarted(timer)) {

		timer_ptr_t timerPtr = getTimer(timer);

		#if ESP_IDF_VERSION_MAJOR == 4
			// cancels timer
			timer_set_alarm((*timerPtr) -> group, (*timerPtr) -> num, false);
			timer_pause((*timerPtr) -> group, (*timerPtr) -> num);
			timer_set_counter_value((*timerPtr) -> group, (*timerPtr) -> num, TIMER_COUNT_ZERO);

			// deconstructs timer
			timer_isr_callback_remove((*timerPtr) -> group, (*timerPtr) -> num);
			timer_deinit((*timerPtr) -> group, (*timerPtr) -> num);

			*timerPtr = NULL;
			uhwtStopTimer(timer);
			return true;
		#elif ESP_IDF_VERSION_MAJOR == 5

			gptimer_stop(**timerPtr);
			gptimer_disable(**timerPtr);
			gptimer_del_timer(**timerPtr);

			**timerPtr = NULL;
			uhwtStopTimer(timer);
			return true;
		#endif
	}

	return false;
}

bool setHardTimer(uhwt_timer_t *timer, uhwt_freq_t *freq, uhwt_function_ptr_t function, uhwt_params_ptr_t params, uhwt_priority_t priority) {
	
	if (function == NULL || freq == NULL || timer == NULL) {
		return false;
	}
	if (*freq == (uhwt_freq_t)0 || *freq > UHWT_TIMER_FREQ_MAX) {
		return false;
	}

	uhwt_prescalar_t scalar;
	uhwt_timertick_t timerTicks;

	if (!uhwtGetStats(timer, *freq, &scalar, &timerTicks)) {
		return false;
	}
	if (!uhwtValidPreScalar(*timer, scalar) || !uhwtValidTimerTicks(*timer, timerTicks)) {
		return false;
	}
	*freq = uhwtCalcFreq(scalar, timerTicks);

	if (!uhwtTimerStarted(*timer)) {

		timer_ptr_t timerPtr = getTimer(*timer);

		uhwtSetCallbackParams(*timer, function, params);
		
		#if ESP_IDF_VERSION_MAJOR == 4
			// init timer
			timer_config_t config = {
				.divider = scalar,
				.counter_dir = true,
				.counter_en = TIMER_PAUSE,
				.alarm_en = TIMER_ALARM_DIS,
				.auto_reload = false,
			};
			*timerPtr = &timerGroups[*timer];
			
			timer_init((*timerPtr) -> group, (*timerPtr) -> num, &config);
			timer_set_counter_value((*timerPtr) -> group, (*timerPtr) -> num, TIMER_COUNT_ZERO);
			timer_start((*timerPtr) -> group, (*timerPtr) -> num);
			timer_isr_callback_add((*timerPtr) -> group, (*timerPtr) -> num, getHardTimerCallback(*timer), params, setPriority(priority));

			// run timer
			timer_set_alarm_value((*timerPtr) -> group, (*timerPtr) -> num, timerTicks);
			timer_set_auto_reload((*timerPtr) -> group, (*timerPtr) -> num, true);
			timer_set_alarm((*timerPtr) -> group, (*timerPtr) -> num, true);
			timer_start((*timerPtr) -> group, (*timerPtr) -> num);

			uhwtStartTimer(*timer);

			return true;

		#elif ESP_IDF_VERSION_MAJOR == 5

			uint64_t count = 1;
			uhwt_freq_t tempFreq = *freq;

			// adjusts frequency to be above min
			while (tempFreq < HARD_TIMER_FREQ_MIN) {
				tempFreq *= 2;
				count *= 2;
			}

			// timer config
			gptimer_config_t config = {
				.clk_src = GPTIMER_CLK_SRC_DEFAULT,
				.direction = GPTIMER_COUNT_UP,
				.resolution_hz = tempFreq,
				.intr_priority = setPriority(priority),
			};

			// function config
			gptimer_alarm_config_t configAlarm = {
				.reload_count = 0,
				.alarm_count = count,
				.flags.auto_reload_on_alarm = true,
			};

			// callback config
			gptimer_event_callbacks_t configCallback = {
				.on_alarm = getHardTimerCallback(*timer),
			};

			// creates new timer
			gptimer_new_timer(&config, *timerPtr);

			// sets up callback function
			gptimer_set_alarm_action(**timerPtr, &configAlarm);
			gptimer_register_event_callbacks(**timerPtr, &configCallback, params);

			// starts timer
			gptimer_enable(**timerPtr);
			gptimer_start(**timerPtr);

			uhwtStartTimer(*timer);

			return true;

		#endif
	}

	return false;
}

#endif