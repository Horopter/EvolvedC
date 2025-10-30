#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#include "../../src/ec_timer.h"

#define TEST_ASSERT(cond)                                                             \
	do                                                                                \
		{                                                                             \
			if (!(cond))                                                              \
				{                                                                     \
					fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #cond); \
					exit(EXIT_FAILURE);                                               \
				}                                                                     \
		}                                                                             \
	while (0)

/* Time Functions */
uint64_t
time_ms(void)
{
#if defined(_WIN32) || defined(_WIN64)
	static int64_t frequency = 0;
	if (frequency == 0)
		{
			LARGE_INTEGER freq;
			QueryPerformanceFrequency(&freq);
			assert(freq.QuadPart != 0);
			frequency = freq.QuadPart;
		}
	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	return (int64_t)(count.QuadPart * 1000) / frequency;
#else
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
#endif
}

void
sleep_ms(uint64_t ms)
{
#if defined(_WIN32) || defined(_WIN64)
	Sleep((DWORD)ms);
#else
	struct timespec t = { ms / 1000, (ms % 1000) * 1000000 };
	nanosleep(&t, NULL);
#endif
}

/* Timer Test Variables */
int count;
uint64_t ids[EC_TIMER_MAX_EVENTS];

void
callback(void* arg)
{
	(void)arg;
	count--;
}

/* Basic Timer Test */
void
test_timer_basic(void)
{
	printf("[RUN] test_timer_basic\n");

	ec_timer_t timer;
	ec_timer_init(&timer);

	for (int i = 0; i < 1000; i++)
		{
			ids[i] = ec_timer_start(&timer, rand() % 100, 0, EC_TIMER_ONESHOT, callback, NULL);
			count++;
			TEST_ASSERT(ids[i] != EC_TIMER_INVALID);
		}

	uint64_t t = 10000;
	while (t > 0 && count > 0)
		{
			ec_timer_process(&timer);
			t -= 10;
			sleep_ms(10);
		}

	TEST_ASSERT(count == 0);
	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_basic\n");
}

/* Timer Cancellation Test */
void
test_timer_cancel(void)
{
	printf("[RUN] test_timer_cancel\n");

	ec_timer_t timer;
	ec_timer_init(&timer);

	for (int i = 0; i < 1000; i++)
		{
			ids[i] = ec_timer_start(&timer, rand() % 100, 0, EC_TIMER_ONESHOT, callback, NULL);
			count++;
		}

	for (int i = 0; i < 500; i++)
		{
			ec_timer_cancel(&timer, ids[i]);
			count--;
		}

	uint64_t t = 10000;
	while (t > 0 && count > 0)
		{
			ec_timer_process(&timer);
			t -= 10;
			sleep_ms(10);
		}

	TEST_ASSERT(count == 0);
	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_cancel\n");
}

/* Periodic Timer Test */
void
test_timer_periodic(void)
{
	printf("[RUN] test_timer_periodic\n");

	ec_timer_t timer;
	ec_timer_init(&timer);

	ids[0] = ec_timer_start(&timer, 50, 50, EC_TIMER_PERIODIC, callback, NULL);
	count = 5;

	uint64_t t = 300;
	while (t > 0 && count > 0)
		{
			ec_timer_process(&timer);
			t -= 10;
			sleep_ms(10);
		}

	TEST_ASSERT(count == 0);
	ec_timer_cancel(&timer, ids[0]);

	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_periodic\n");
}

/* Multi-Timer Test */
void
test_timer_multiple(void)
{
	printf("[RUN] test_timer_multiple\n");

	ec_timer_t timer;
	ec_timer_init(&timer);

	for (int i = 0; i < 10; i++)
		{
			ids[i] = ec_timer_start(&timer, (i + 1) * 20, 0, EC_TIMER_ONESHOT, callback, NULL);
			count++;
		}

	uint64_t t = 300;
	while (t > 0 && count > 0)
		{
			ec_timer_process(&timer);
			t -= 10;
			sleep_ms(10);
		}

	TEST_ASSERT(count == 0);
	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_multiple\n");
}

/* Stress Test */
void
test_timer_stress(void)
{
	printf("[RUN] test_timer_stress\n");

	ec_timer_t timer;
	ec_timer_init(&timer);

	for (int i = 0; i < EC_TIMER_MAX_EVENTS; i++)
		{
			ids[i] = ec_timer_start(&timer, rand() % 50, 0, EC_TIMER_ONESHOT, callback, NULL);
			count++;
		}

	uint64_t t = 1000;
	while (t > 0 && count > 0)
		{
			ec_timer_process(&timer);
			t -= 10;
			sleep_ms(10);
		}

	TEST_ASSERT(count == 0);
	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_stress\n");
}

/* Edge Case Test */
void
test_timer_edge_cases(void)
{
	printf("[RUN] test_timer_edge_cases\n");

	ec_timer_t timer;
	ec_timer_init(&timer);

	count = 2; // Expect two timers

	/* Timer with 0 delay (should execute immediately) */
	ids[0] = ec_timer_start(&timer, 0, 0, EC_TIMER_ONESHOT, callback, NULL);
	TEST_ASSERT(ids[0] != EC_TIMER_INVALID);

	/* Timer with extreme delay (should NOT execute during this test) */
	ids[1] = ec_timer_start(&timer, UINT64_MAX, 0, EC_TIMER_ONESHOT, callback, NULL);
	TEST_ASSERT(ids[1] != EC_TIMER_INVALID);

	/* Process timers multiple times to ensure immediate execution */
	uint64_t t = 50;
	while (t > 0)
		{
			ec_timer_process(&timer);
			if (count == 1)
				break; // Stop early if immediate timer executed
			t -= 10;
			sleep_ms(10);
		}

	/* Check if immediate timer executed */
	if (count != 1)
		{
			fprintf(stderr, "[FAIL] %s:%d: Expected count = 1, but got count = %d\n", __FILE__, __LINE__, count);
			exit(EXIT_FAILURE);
		}

	/* Ensure extreme delay timer is still active */
	if (!timer.events[ids[1]].active)
		{
			fprintf(stderr, "[FAIL] %s:%d: Max delay timer should still be active!\n", __FILE__, __LINE__);
			exit(EXIT_FAILURE);
		}

	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_edge_cases\n");
}

/* Run All Tests */
int
main(void)
{
	printf("\nStarting `ec_timer` Tests...\n");

	test_timer_basic();
	test_timer_cancel();
	test_timer_periodic();
	test_timer_multiple();
	test_timer_stress();
	test_timer_edge_cases();

	printf("\nAll `ec_timer` tests passed successfully!\n");
	return 0;
}
