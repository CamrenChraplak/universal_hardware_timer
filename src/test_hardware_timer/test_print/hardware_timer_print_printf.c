/*
	hardware_timer_print_printf.c - prints printf results of tests
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

#include "../hardware_timer_test_priv.h"

#if !defined(USE_UNITY) && !defined(ARDUINO)

#if UHWT_SUPPORT_ESP32
	#if ESP_IDF_VERSION_MAJOR == 5
		#define PRINT_LONG_SPECIFIER
	#endif
#endif

char* timerFuncName = NULL;
int timerFuncLine = -1;

void printMessageType(memCharString *message, const int funcLine, int messType) {
	printf("[");
	if (messType == TEST_FAILED) {
		printf("Failed");
	}
	else if (messType == TEST_IGNORED) {
		printf("Ignored");
	}
	else if (messType == TEST_PASSED) {
		printf("Passed");
	}
	printf("],\t");
	printf(timerFuncName);
	printf(", ");
	if (messType == TEST_PASSED) {
		printf("%d", timerFuncLine);
	}
	else {
		printf("%d", funcLine);
	}
	printf(": ");
	printf(message);
	printf("\n");

	if (timerFuncName != NULL) {
		free(timerFuncName);
		timerFuncName = NULL;
	}
}

bool timerCountWithin(uint32_t buffer, uint32_t targetCount, uint32_t realCount, const int funcLine) {
	if (realCount + buffer < targetCount || realCount - buffer > targetCount) {
		char message[60];
		#ifdef PRINT_LONG_SPECIFIER
			sprintf(message, "Values not within delta %lu, Expected %lu, Got %lu", buffer, targetCount, realCount);
		#else
			sprintf(message, "Values not within delta %u, Expected %u, Got %u", buffer, targetCount, realCount);
		#endif
		
		printMessageType(message, funcLine, TEST_FAILED);
		return false;
	}
	return true;
}

void runTest(const char* funcName, const int funcLine) {
	if (timerFuncName != NULL) {
		free(timerFuncName);
		timerFuncName = NULL;
	}

	timerFuncLine = funcLine;
	int nameLen = strlen(funcName) + 1;
	timerFuncName = (char*)malloc(sizeof(char) * nameLen);

	for (int i = 0; i < nameLen; i++) {
		timerFuncName[i] = funcName[i];
	}
}

#endif