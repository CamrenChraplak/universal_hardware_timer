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

#if UHWT_TIMER_FREQ_MAX <= 0
	memCharString noMaxFreqIgnore[] = {"Skipping tests that require >0 UHWT_TIMER_FREQ_MAX"};
#endif
#if UHWT_TIMER_COUNT <= 0
	memCharString noTimerIgnore[] = {"Skipping tests that require >0 UHWT_TIMER_COUNT"};
#endif

/**
 * Tests 'uhwtValidFrequency'
 */
void testValidFreq() {

	// invalid inputs
	if (uhwtValidFrequency(0)) {
		TEST_FAIL();
	}
	if (uhwtValidFrequency(UHWT_TIMER_FREQ_MAX + 1)) {
		TEST_FAIL();
	}

	#if UHWT_TIMER_FREQ_MAX > 0

		// valid inputs
		if (!uhwtValidFrequency(1)) {
			TEST_FAIL();
		}
		if (!uhwtValidFrequency(UHWT_TIMER_FREQ_MAX)) {
			TEST_FAIL();
		}
		TEST_PASS();
	#else
		TEST_IGNORE_MESSAGE(noMaxFreqIgnore);
	#endif
}

/**
 * Tests 'uhwtValidTimer'
 */
void testValidTimer() {

	// invalid inputs
	if (uhwtValidTimer(UHWT_TIMER_INVALID)) {
		TEST_FAIL();
	}
	if (uhwtValidTimer(UHWT_TIMER_COUNT)) {
		TEST_FAIL();
	}
	if (uhwtValidTimer(-2)) {
		TEST_FAIL();
	}
	
	#if UHWT_TIMER_COUNT > 0

		// valid inputs
		if (!uhwtValidTimer(0)) {
			TEST_FAIL();
		}
		TEST_PASS();
	#else
		TEST_IGNORE_MESSAGE(noTimerIgnore);
	#endif
}

/**
 * Tests 'uhwtInitTimer'
 * Doesn't test with platform
 */
void testInitTimer() {

	// invalid inputs
	if (uhwtInitTimer(UHWT_TIMER_INVALID)) {
		TEST_FAIL();
	}
	if (uhwtInitTimer(UHWT_TIMER_COUNT)) {
		TEST_FAIL();
	}

	#if UHWT_TIMER_COUNT > 0

		// valid inputs
		uhwtSetTimerInitialized(0);
		if (uhwtInitTimer(0)) {
			TEST_FAIL();
		}
		uhwtSetTimerDeconstructed(0);
		TEST_PASS();
	#else
		TEST_IGNORE_MESSAGE(noTimerIgnore);
	#endif
}

/**
 * Tests 'uhwtDeconstructTimer'
 * Doesn't test with platform
 */
void testDeconstructTimer() {

	// invalid inputs
	if (uhwtDeconstructTimer(UHWT_TIMER_INVALID)) {
		TEST_FAIL();
	}
	if (uhwtDeconstructTimer(UHWT_TIMER_COUNT)) {
		TEST_FAIL();
	}

	#if UHWT_TIMER_COUNT > 0

		// not initialized
		if (uhwtDeconstructTimer(0)) {
			TEST_FAIL();
		}
		
		TEST_PASS();
	#else
		TEST_IGNORE_MESSAGE(noTimerIgnore);
	#endif
}

/**
 * Tests 'uhwtTimerInitialized'
 * Doesn't test with platform
 */
void testInitialized() {

	// invalid inputs
	if (uhwtTimerInitialized(UHWT_TIMER_INVALID)) {
		TEST_FAIL();
	}
	if (uhwtTimerInitialized(UHWT_TIMER_COUNT)) {
		TEST_FAIL();
	}

	#if UHWT_TIMER_COUNT > 0

		// not initialized
		if (uhwtTimerInitialized(0)) {
			TEST_FAIL();
		}

		// valid inputs
		uhwtSetTimerInitialized(0);
		if (!uhwtTimerInitialized(0)) {
			TEST_FAIL();
		}
		uhwtSetTimerDeconstructed(0);
		if (uhwtTimerInitialized(0)) {
			TEST_FAIL();
		}
		
		TEST_PASS();
	#else
		TEST_IGNORE_MESSAGE(noTimerIgnore);
	#endif
}

/**
 * Tests 'uhwtClaimTimer'
 */
void testClaimTimer() {

	// invalid inputs
	if (uhwtClaimTimer(NULL)) {
		TEST_FAIL();
	}

	#if UHWT_TIMER_COUNT > 0

		uhwt_timer_t timer = UHWT_TIMER_COUNT;

		// timer value out of bounds
		if (!uhwtClaimTimer(&timer)) {
			TEST_FAIL();
		}
		if (!uhwtValidTimer(timer)) {
			TEST_FAIL();
		}
		if (!uhwtUnclaimTimer(timer)) {
			TEST_FAIL();
		}

		// timer invalid
		timer = UHWT_TIMER_INVALID;
		if (!uhwtClaimTimer(&timer)) {
			TEST_FAIL();
		}
		if (!uhwtValidTimer(timer)) {
			TEST_FAIL();
		}
		if (!uhwtUnclaimTimer(timer)) {
			TEST_FAIL();
		}

		// valid timer
		timer = 0;
		if (!uhwtClaimTimer(&timer)) {
			TEST_FAIL();
		}
		if (!uhwtValidTimer(timer)) {
			TEST_FAIL();
		}

		// repeat claim
		if (uhwtClaimTimer(&timer)) {
			TEST_FAIL();
		}
		if (!uhwtValidTimer(timer)) {
			TEST_FAIL();
		}
		if (!uhwtUnclaimTimer(timer)) {
			TEST_FAIL();
		}

		// timer started
		timer = 0;
		uhwtSetTimerInitialized(timer);
		uhwtSetTimerStarted(timer);

		if (uhwtClaimTimer(&timer)) {
			TEST_FAIL();
		}
		if (uhwtUnclaimTimer(timer)) {
			TEST_FAIL();
		}

		uhwtSetTimerStopped(timer);
		uhwtSetTimerDeconstructed(timer);

		// claim all
		for (uint8_t i = 0; i < UHWT_TIMER_COUNT; i++) {
			timer = UHWT_TIMER_INVALID;

			if (!uhwtClaimTimer(&timer)) {
				TEST_FAIL();
			}
			if (!uhwtValidTimer(timer)) {
				TEST_FAIL();
			}
		}

		// all are claimed
		timer = UHWT_TIMER_INVALID;
		if (uhwtClaimTimer(&timer)) {
			TEST_FAIL();
		}
		if (uhwtValidTimer(timer)) {
			TEST_FAIL();
		}

		// release all
		for (uint8_t i = 0; i < UHWT_TIMER_COUNT; i++) {
			timer = i;

			if (!uhwtUnclaimTimer(timer)) {
				TEST_FAIL();
			}
		}
		
		TEST_PASS();
	#else
		TEST_IGNORE_MESSAGE(noTimerIgnore);
	#endif
}

/**
 * Tests 'uhwtUnclaimTimer'
 */
void testUnclaimTimer() {

	// invalid inputs
	if (uhwtUnclaimTimer(UHWT_TIMER_INVALID)) {
		TEST_FAIL();
	}
	if (uhwtUnclaimTimer(UHWT_TIMER_COUNT)) {
		TEST_FAIL();
	}

	#if UHWT_TIMER_COUNT > 0

		uhwt_timer_t timer = UHWT_TIMER_INVALID;

		// timer not claimed
		if (uhwtUnclaimTimer(0)) {
			TEST_FAIL();
		}

		// claim valid
		if (!uhwtClaimTimer(&timer)) {
			TEST_FAIL();
		}
		if (!uhwtValidTimer(timer)) {
			TEST_FAIL();
		}
		if (!uhwtUnclaimTimer(timer)) {
			TEST_FAIL();
		}

		// repeat unclaim
		if (uhwtUnclaimTimer(timer)) {
			TEST_FAIL();
		}
		
		TEST_PASS();
	#else
		TEST_IGNORE_MESSAGE(noTimerIgnore);
	#endif
}

/**
 * Tests 'uhwtTimerClaimed'
 */
void testTimerClaimed() {

	// invalid inputs
	if (uhwtTimerClaimed(UHWT_TIMER_INVALID)) {
		TEST_FAIL();
	}
	if (uhwtTimerClaimed(UHWT_TIMER_COUNT)) {
		TEST_FAIL();
	}

	#if UHWT_TIMER_COUNT > 0

		if (uhwtTimerClaimed(0)) {
			TEST_FAIL();
		}

		uhwt_timer_t timer = UHWT_TIMER_INVALID;

		// claim valid
		if (!uhwtClaimTimer(&timer)) {
			TEST_FAIL();
		}
		if (!uhwtValidTimer(timer)) {
			TEST_FAIL();
		}
		if (!uhwtTimerClaimed(timer)) {
			TEST_FAIL();
		}

		// unclaim valid
		if (!uhwtUnclaimTimer(timer)) {
			TEST_FAIL();
		}
		if (uhwtTimerClaimed(timer)) {
			TEST_FAIL();
		}
		
		TEST_PASS();
	#else
		TEST_IGNORE_MESSAGE(noTimerIgnore);
	#endif
}

/**
 * Tests 'uhwtStartTimer'
 * Doesn't test with platform
 */
void testStartTimerArgs() {

	// invalid inputs
	if (uhwtStartTimer(UHWT_TIMER_INVALID)) {
		TEST_FAIL();
	}
	if (uhwtStartTimer(UHWT_TIMER_COUNT)) {
		TEST_FAIL();
	}

	#if UHWT_TIMER_COUNT > 0

		// timer not configured
		if (uhwtStartTimer(0)) {
			TEST_FAIL();
		}

		// timer started
		uhwtSetTimerInitialized(0);
		uhwtSetTimerStarted(0);
		if (uhwtStartTimer(0)) {
			TEST_FAIL();
		}

		uhwtSetTimerStopped(0);
		uhwtSetTimerDeconstructed(0);
		
		TEST_PASS();
	#else
		TEST_IGNORE_MESSAGE(noTimerIgnore);
	#endif
}

/**
 * Tests 'uhwtStopTimer'
 * Doesn't test with platform
 */
void testStopTimerArgs() {

	// invalid inputs
	if (uhwtStopTimer(UHWT_TIMER_INVALID)) {
		TEST_FAIL();
	}
	if (uhwtStopTimer(UHWT_TIMER_COUNT)) {
		TEST_FAIL();
	}

	#if UHWT_TIMER_COUNT > 0

		// timer not configured
		if (uhwtStopTimer(0)) {
			TEST_FAIL();
		}

		// timer initialized
		uhwtSetTimerInitialized(0);
		if (uhwtStopTimer(0)) {
			TEST_FAIL();
		}
		uhwtSetTimerDeconstructed(0);
		
		TEST_PASS();
	#else
		TEST_IGNORE_MESSAGE(noTimerIgnore);
	#endif
}

/**
 * Tests 'uhwtTimerStarted'
 */
void testTimerStarted() {
	// invalid inputs
	if (uhwtTimerStarted(UHWT_TIMER_INVALID)) {
		TEST_FAIL();
	}
	if (uhwtTimerStarted(UHWT_TIMER_COUNT)) {
		TEST_FAIL();
	}

	#if UHWT_TIMER_COUNT > 0

		// timer not configured
		if (uhwtTimerStarted(0)) {
			TEST_FAIL();
		}

		// timer initialized
		uhwtSetTimerInitialized(0);
		if (uhwtTimerStarted(0)) {
			TEST_FAIL();
		}

		// timer started
		uhwtSetTimerStarted(0);
		if (!uhwtTimerStarted(0)) {
			TEST_FAIL();
		}

		// timer stopped
		uhwtSetTimerStopped(0);
		if (uhwtTimerStarted(0)) {
			TEST_FAIL();
		}

		uhwtSetTimerDeconstructed(0);
		
		TEST_PASS();
	#else
		TEST_IGNORE_MESSAGE(noTimerIgnore);
	#endif
}

void uhwtTestCommon() {
	UHWT_SET_FILE_NAME("hardware_timer_test_common.c");

	// static checks
	RUN_TEST(&testValidFreq);
	RUN_TEST(&testValidTimer);

	// init
	RUN_TEST(&testInitTimer);
	RUN_TEST(&testDeconstructTimer);
	RUN_TEST(&testInitialized);

	// claim
	RUN_TEST(&testClaimTimer);
	RUN_TEST(&testUnclaimTimer);
	RUN_TEST(&testTimerClaimed);

	// start
	RUN_TEST(&testStartTimerArgs);
	RUN_TEST(&testStopTimerArgs);
	RUN_TEST(&testTimerStarted);
}