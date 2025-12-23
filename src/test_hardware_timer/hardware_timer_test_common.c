/*
	hardware_timer_test_common.c - tests common timer methods
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

/**
 * Tests to run:
 * 
 * uhwtTimerInitialized
 * uhwtTimerStarted
 * uhwtClaimTimer
 * uhwtUnclaimTimer
 * uhwtTimerClaimed
 * uwhtGetNextTimer
 * uhwtValidFrequency
 * uhwtValidTimer
 * uhwtSetTimerStarted
 * uhwtSetTimerStopped
 * uhwtSetTimerInitialized
 * uhwtSetTimerDeconstructed
 * 
 * Common invalid args:
 * 
 * uhwtInitTimer
 * uhwtDeconstructTimer
 * uhwtStopTimer
 * uhwtStartTimer
 * uhwtClaimTimerStats
 * uwhtGetNextTimerStats
 * uhwtGetStats
 * uhwtGetClosestStats
 * uhwtSetCallbackParams
 * uhwtSetStats
 * uhwtSetupTimer
 * uhwtSetupComplexTimer
 * uhwtCalcFreq
 * uhwtGetPreScalar
 * uhwtGetTimerTicks
 * uhwtEqualFreq
 * uhwtSetPriority
 * uhwtValidPreScalar
 * uhwtValidTimerTicks
 * uhwtGetCallback
 * uhwtGetNextPreScalar
 * uhwtCalcTicks
 * uhwtCalcScalar
 */

void testValidFreq() {
	if (uhwtValidFrequency(0)) {
		TEST_FAIL();
	}
	if (uhwtValidFrequency(UHWT_TIMER_FREQ_MAX + 1)) {
		TEST_FAIL();
	}
	if (!uhwtValidFrequency(1)) {
		TEST_FAIL();
	}
	if (!uhwtValidFrequency(UHWT_TIMER_FREQ_MAX)) {
		TEST_FAIL();
	}
	TEST_PASS();
}

void testValidTimer() {
	if (uhwtValidTimer(UHWT_TIMER_INVALID)) {
		TEST_FAIL();
	}
	if (uhwtValidTimer(UHWT_TIMER_COUNT)) {
		TEST_FAIL();
	}
	
	#if UHWT_TIMER_COUNT > 0
		if (!uhwtValidTimer(0)) {
			TEST_FAIL();
		}
		if (!uhwtValidTimer(UHWT_TIMER_COUNT - 1)) {
			TEST_FAIL();
		}
		TEST_PASS();
	#else
		TEST_IGNORE();
	#endif
}

void uhwtTestCommon() {
	UHWT_SET_FILE_NAME("hardware_timer_test_common.c");
	RUN_TEST(&testValidFreq);
	RUN_TEST(&testValidTimer);
}