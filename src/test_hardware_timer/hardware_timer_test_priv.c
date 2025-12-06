/*
	hardware_timer_test_priv.c - methods to test timer methods
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

#if HARD_TIMER_COUNT > 0

#define TEST_SLOW_FREQ 10 // frequency for testing slow timing
#ifndef TEST_FAST_FREQ
	#define TEST_FAST_FREQ HARD_TIMER_FREQ_MAX // target frequency
#endif

/**
 * frequency for testing setting timer
 * 
 * @note included for microcontrollers where
 * @note all timers can't run TEST_SLOW_FREQ
 */
#define TEST_CASES_FREQ 100

#define SLOW_TIMER_PRIORITY 0 // slow timer priority
#define FAST_TIMER_PRIORITY 255 // fast timer priority

#define TEST_DELAY_ELLAPSE_S 1 // time for timer to run for

#ifndef SLOW_TEST_BUFFER
	#define SLOW_TEST_BUFFER 0 // amount slow timer can be off of goal
#endif
#ifndef FAST_TEST_BUFFER
	#define FAST_TEST_BUFFER 0 // amount fast timer can be off of goal
#endif

#define HARD_TIMER_RAM_ATTR(funcName)

/**
 * Tests if timer is invalid timer or not
 */
#define TIMER_NOT_INVALID(timer) \
	if (timer != HARD_TIMER_INVALID) { \
		TEST_FAIL_MESSAGE(setTimerFail); \
	}

memCharString invalidStartFail[] PROG_FLASH = {"Start State"};
memCharString startFail[] PROG_FLASH = {"Start"};
memCharString noSetTimerFail[] PROG_FLASH = {"No Set Timer"};
memCharString restartFail[] PROG_FLASH = {"Restart"};
memCharString cancelFail[] PROG_FLASH = {"Stop"};
memCharString recancelFail[] PROG_FLASH = {"Restop"};
memCharString allNullFail[] PROG_FLASH = {"All Null"};
memCharString freqNullFail[] PROG_FLASH = {"Freq Null"};
memCharString funcNullFail[] PROG_FLASH = {"Func Null"};

memCharString unclaimInvalidFail[] PROG_FLASH = {"Unclaim Invalid"};
memCharString unclaimNotClaimedFail[] PROG_FLASH = {"Unclaim Unclaimed"};
memCharString claimLoopFail[] PROG_FLASH = {"Claim Loop"};
memCharString allClaimedFail[] PROG_FLASH = {"All Claimed"};
memCharString unclaimLoopFail[] PROG_FLASH = {"Unclaim Loop"};
memCharString notClaimedFail[] PROG_FLASH = {"Not Claimed"};
memCharString isClaimedFail[] PROG_FLASH = {"Is Claimed"};
memCharString didntUnclaimFail[] PROG_FLASH = {"Didn't Unclaim"};
memCharString noStartFail[] PROG_FLASH = {"No Start"};
memCharString claimedActiveFail[] PROG_FLASH = {"Claimed Active"};
memCharString sameTimerFail[] PROG_FLASH = {"Same Timer"};

memCharString freq0Fail[] PROG_FLASH = {"Freq 0"};
memCharString freqMaxFail[] PROG_FLASH = {"Freq Max"};
memCharString notSameTimerFail[] PROG_FLASH = {"Not Same Timer"};
memCharString isStartFail[] PROG_FLASH = {"Is Start"};
memCharString notStartFail[] PROG_FLASH = {"Not Start"};
memCharString setTimerFail[] PROG_FLASH = {"Set Timer"};
memCharString setLoopFail[] PROG_FLASH = {"Set Loop"};
memCharString maxTimerFail[] PROG_FLASH = {"Max Timer"};
memCharString startLoopFail[] PROG_FLASH = {"Start Loop"};
memCharString cancelLoopFail[] PROG_FLASH = {"Cancel Loop"};
memCharString cancelInvalidFail[] PROG_FLASH = {"Cancel Invalid"};
memCharString startedLoopFail[] PROG_FLASH = {"Started Loop"};
memCharString didntStopFail[] PROG_FLASH = {"Didn't Stop"};

/**
 * Priority claim statements
 * P: Priority
 * S: Started
 * N: Not Started
 * C: Claimed
 * U: Unclaimed
 */

memCharString pNotStartedClaimedFail[] PROG_FLASH = {"PNC"};
memCharString pNotStartedClaimedUnequalFail[] PROG_FLASH = {"PNC Unequal"};
memCharString pStartedClaimedFail[] PROG_FLASH = {"PSC"};
memCharString pStartedClaimedUnequalFail[] PROG_FLASH = {"PSC Unequal"};
memCharString pStartedUnclaimedFail[] PROG_FLASH = {"PSU"};
memCharString pStartedUnclaimedUnequalFail[] PROG_FLASH = {"PSU Unequal"};

volatile uint32_t hardTimerCount = 0U;

/**
 * Testing function
 */
void HARD_TIMER_RAM_ATTR(testTimingFunction) testTimingFunction(void *params) {
	hardTimerCount++;
}

/**
 * Testing function
 */
void HARD_TIMER_RAM_ATTR(testTimingFunctionParams) testTimingFunctionParams(void *params) {
	hardTimerCount += *(uint32_t*)params;
}

/**
 * Resets all timers to off and unclaimed
 */
void resetTimers() {
	for (int i = 0; i < HARD_TIMER_COUNT; i++) {
		unclaimTimer((hard_timer_enum_t)i);
		cancelHardTimer((hard_timer_enum_t)i);
	}
}

/**
 * Tests timer start state
 * 
 * @param timer timer to test
 * @param start whether timer should or shouldn't be started
 */
void testGetStartState(hard_timer_enum_t timer, bool start) {
	if (hardTimerStarted(timer) != start) {
		TEST_FAIL_MESSAGE(invalidStartFail);
	}
}

/**
 * Tests that no timers are set when starting
 * 
 * @warning Only call when timers aren't started nor initialized
 */
void testProgramStart() {
	resetTimers();
	for (int i = 0; i < HARD_TIMER_COUNT; i++) {
		testGetStartState((hard_timer_enum_t)i, false);
	}
	TEST_PASS();
}

/**
 * Tests correct status for repeat starts and cancels
 */
void testRepeat() {
	resetTimers();
	hard_timer_freq_t freq = TEST_CASES_FREQ;
	hard_timer_enum_t timer = HARD_TIMER_INVALID;

	if (!setHardTimer(&timer, &freq, &testTimingFunction, NULL, HARD_TIMER_PRIORITY_DEFAULT)) {
		TEST_FAIL_MESSAGE(startFail);
	}
	if (timer == HARD_TIMER_INVALID) {
		TEST_FAIL_MESSAGE(noSetTimerFail);
	}

	hard_timer_enum_t secondTimer = timer;
	freq = TEST_CASES_FREQ;
	if (!setHardTimer(&secondTimer, &freq, &testTimingFunction, NULL, HARD_TIMER_PRIORITY_DEFAULT)) {
		TEST_FAIL_MESSAGE(startFail);
	}
	if (timer == secondTimer) {
		TEST_FAIL_MESSAGE(restartFail);
	}

	if (!cancelHardTimer(timer)) {
		TEST_FAIL_MESSAGE(cancelFail);
	}
	if (cancelHardTimer(timer)) {
		TEST_FAIL_MESSAGE(recancelFail);
	}
	TEST_PASS();
}

/**
 * Tests claim functionality
 */
void testClaims(void) {

	resetTimers();

	hard_timer_enum_t timer = HARD_TIMER_INVALID;
	hard_timer_freq_t freq = TEST_CASES_FREQ;

	// unclaim
	if (unclaimTimer(timer)) {
		TEST_FAIL_MESSAGE(unclaimInvalidFail);
	}
	if (unclaimTimer(HARD_TIMER0)) {
		TEST_FAIL_MESSAGE(unclaimNotClaimedFail);
	}

	// none claimed
	for (uint8_t i = 0; i < HARD_TIMER_COUNT; i++) {
		if (hardTimerClaimed(i)) {
			TEST_FAIL_MESSAGE(notClaimedFail);
		}
	}

	// claim
	for (uint8_t i = 0; i < HARD_TIMER_COUNT; i++) {
		if (claimTimer(NULL) == HARD_TIMER_INVALID) {
			TEST_FAIL_MESSAGE(claimLoopFail);
		}
	}
	if (claimTimer(NULL) != HARD_TIMER_INVALID) {
		TEST_FAIL_MESSAGE(allClaimedFail);
	}

	// all claimed
	for (uint8_t i = 0; i < HARD_TIMER_COUNT; i++) {
		if (!hardTimerClaimed(i)) {
			TEST_FAIL_MESSAGE(isClaimedFail);
		}
	}

	// remove all claims and test unclaimed
	for (uint8_t i = 0; i < HARD_TIMER_COUNT; i++) {
		if (!unclaimTimer(i)) {
			TEST_FAIL_MESSAGE(unclaimLoopFail);
		}
		if (hardTimerClaimed(i)) {
			TEST_FAIL_MESSAGE(didntUnclaimFail);
		}
	}

	// claim active timer
	if (!setHardTimer(&timer, &freq, testTimingFunction, NULL, HARD_TIMER_PRIORITY_DEFAULT)) {
		TEST_FAIL_MESSAGE(noStartFail);
	}
	for (uint8_t i = 0; i < HARD_TIMER_COUNT - 1; i++) {
		if (claimTimer(NULL) == HARD_TIMER_INVALID) {
			TEST_FAIL_MESSAGE(claimLoopFail);
		}
	}
	if (claimTimer(NULL) != HARD_TIMER_INVALID) {
		TEST_FAIL_MESSAGE(claimedActiveFail);
	}

	// remove all claims
	bool claimed = false;
	for (uint8_t i = 0; i < HARD_TIMER_COUNT; i++) {
		if (!unclaimTimer(i)) {
			if (!claimed) {
				claimed = true;
			}
			else {
				TEST_FAIL_MESSAGE(unclaimLoopFail);
			}
		}
	}

	if (!cancelHardTimer(timer)) {
		TEST_FAIL_MESSAGE(cancelFail);
	}
	timer = HARD_TIMER_INVALID;
	TEST_PASS();
}

/**
 * Tests start parameters
 */
void testStart(void) {

	resetTimers();

	hard_timer_enum_t timer = HARD_TIMER_INVALID;
	hard_timer_freq_t freq = TEST_CASES_FREQ;

	// null parameters
	if (setHardTimer(NULL, NULL, NULL, NULL, HARD_TIMER_PRIORITY_DEFAULT)) {
		TEST_FAIL_MESSAGE(allNullFail);
	}
	TIMER_NOT_INVALID(timer);
	if (setHardTimer(&timer, NULL, testTimingFunction, NULL, HARD_TIMER_PRIORITY_DEFAULT)) {
		TEST_FAIL_MESSAGE(freqNullFail);
	}
	TIMER_NOT_INVALID(timer);
	if (setHardTimer(&timer, &freq, NULL, NULL, HARD_TIMER_PRIORITY_DEFAULT)) {
		TEST_FAIL_MESSAGE(funcNullFail);
	}
	TIMER_NOT_INVALID(timer);
	if (cancelHardTimer(HARD_TIMER_INVALID)) {
		TEST_FAIL_MESSAGE(cancelInvalidFail);
	}

	// cancel timer
	if (cancelHardTimer(timer)) {
		TEST_FAIL_MESSAGE(cancelFail);
	}
	if (cancelHardTimer(HARD_TIMER0)) {
		TEST_FAIL_MESSAGE(cancelFail);
	}

	// set all timers
	for (uint8_t i = 0; i < HARD_TIMER_COUNT; i++) {
		hard_timer_enum_t loopTimer = HARD_TIMER_INVALID;
		freq = TEST_CASES_FREQ;
		if (!setHardTimer(&loopTimer, &freq, testTimingFunction, NULL, HARD_TIMER_PRIORITY_DEFAULT)) {
			TEST_FAIL_MESSAGE(setLoopFail);
		}
		if (!hardTimerStarted(loopTimer)) {
			TEST_FAIL_MESSAGE(startedLoopFail);
		}
	}
	if (setHardTimer(NULL, &freq, testTimingFunction, NULL, HARD_TIMER_PRIORITY_DEFAULT)) {
		TEST_FAIL_MESSAGE(maxTimerFail);
	}
	for (uint8_t i = 0; i < HARD_TIMER_COUNT; i++) {
		if (!cancelHardTimer((hard_timer_enum_t)i)) {
			TEST_FAIL_MESSAGE(cancelLoopFail);
		}
		if (hardTimerStarted((hard_timer_enum_t)i)) {
			TEST_FAIL_MESSAGE(didntStopFail);
		}
	}

	// 0 frequency
	freq = 0;
	if (setHardTimer(&timer, &freq, testTimingFunction, NULL, HARD_TIMER_PRIORITY_DEFAULT)) {
		TEST_FAIL_MESSAGE(freq0Fail);
	}
	TIMER_NOT_INVALID(timer);
	if (hardTimerStarted(timer)) {
		TEST_FAIL_MESSAGE(isStartFail);
	}
	if (cancelHardTimer(timer)) {
		TEST_FAIL_MESSAGE(cancelFail);
	}
	if (hardTimerStarted(timer)) {
		TEST_FAIL_MESSAGE(notStartFail);
	}
	timer = HARD_TIMER_INVALID;

	// over max frequency
	freq = HARD_TIMER_FREQ_MAX + 1;
	if (setHardTimer(&timer, &freq, testTimingFunction, NULL, HARD_TIMER_PRIORITY_DEFAULT)) {
		TEST_FAIL_MESSAGE(freqMaxFail);
	}
	TIMER_NOT_INVALID(timer);
	if (hardTimerStarted(timer)) {
		TEST_FAIL_MESSAGE(isStartFail);
	}
	if (cancelHardTimer(timer)) {
		TEST_FAIL_MESSAGE(cancelFail);
	}
	if (hardTimerStarted(timer)) {
		TEST_FAIL_MESSAGE(notStartFail);
	}
	timer = HARD_TIMER_INVALID;
	TEST_PASS();
}

/**
 * Tests timer priorities are set according
 * to 'hard_timer.h'. Best Timer ignored
 */
void testTimerPriority() {
	
	resetTimers();

	hard_timer_enum_t timer = HARD_TIMER_INVALID;
	hard_timer_enum_t secondTimer = HARD_TIMER_INVALID;
	hard_timer_freq_t freq = TEST_CASES_FREQ;

	// test timer claimed and unstarted
	timer = claimTimer(NULL);
	secondTimer = timer;
	if (!setHardTimer(&secondTimer, &freq, testTimingFunction, NULL, HARD_TIMER_PRIORITY_DEFAULT)) {
		TEST_FAIL_MESSAGE(pNotStartedClaimedFail);
	}

	if (timer != secondTimer) {
		TEST_FAIL_MESSAGE(pNotStartedClaimedUnequalFail);
	}

	// test timer claimed and started
	freq = TEST_CASES_FREQ;
	if (setHardTimer(&secondTimer, &freq, testTimingFunction, NULL, HARD_TIMER_PRIORITY_DEFAULT)) {
		TEST_FAIL_MESSAGE(pStartedClaimedFail);
	}

	if (timer != secondTimer) {
		TEST_FAIL_MESSAGE(pStartedClaimedUnequalFail);
	}

	// test timer unclaimed and started
	unclaimTimer(secondTimer);

	if (!setHardTimer(&secondTimer, &freq, testTimingFunction, NULL, HARD_TIMER_PRIORITY_DEFAULT)) {
		TEST_FAIL_MESSAGE(pStartedUnclaimedFail);
	}

	if (timer == secondTimer) {
		TEST_FAIL_MESSAGE(pStartedUnclaimedUnequalFail);
	}
	TEST_PASS();
}

/**
 * Tests timing for hardware timer
 * 
 * @param freq target frequency to run at
 * @param buffer amount actual freq can be off
 * @param priority priority to run timer function at
 */
void testTiming(hard_timer_freq_t freq, uint8_t buffer, hard_timer_priority_t priority) {
	resetTimers();
	hard_timer_enum_t functionTimer = HARD_TIMER_INVALID;

	testGetStartState(functionTimer, false);
	hardTimerCount = 0U;

	uint32_t counter = 1;

	if (!setHardTimer(&functionTimer, &freq, &testTimingFunctionParams, &counter, priority)) {
		TEST_FAIL_MESSAGE(startFail);
	}

	delaySeconds(TEST_DELAY_ELLAPSE_S);

	if (!cancelHardTimer(functionTimer)) {
		TEST_FAIL_MESSAGE(cancelFail);
	}

	TEST_ASSERT_UINT32_WITHIN(buffer, freq, hardTimerCount);
	TEST_PASS();
}

/**
 * Tests slow timing accuracy
 */
void testSlowTiming() {
	testTiming(TEST_SLOW_FREQ, SLOW_TEST_BUFFER, SLOW_TIMER_PRIORITY);
}

/**
 * Tests fast timing accuracy
 */
void testFastTiming() {
	testTiming(TEST_FAST_FREQ, FAST_TEST_BUFFER, FAST_TIMER_PRIORITY);
}

void testTimers() {
	RUN_TEST(&testProgramStart);
	RUN_TEST(&testRepeat);
	RUN_TEST(&testClaims);
	RUN_TEST(&testStart);
	RUN_TEST(&testTimerPriority);
	RUN_TEST(&testSlowTiming);
	RUN_TEST(&testFastTiming);
	resetTimers();
}

#else

memCharString passTimerIgnore[] PROG_FLASH = {"Ignoring timer tests"};

void passTimers() {
	TEST_IGNORE_MESSAGE(passTimerIgnore);
}

void testTimers() {
	RUN_TEST(&passTimers);
}
#endif