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

/* --- Positive Tests --- */

static void
test_timer_init_pos(void)
{
	printf("[RUN] test_timer_init_pos\n");
	ec_timer_t timer;
	ec_timer_init(&timer);
	TEST_ASSERT(timer.timestamp == 0);
	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_init_pos\n");
}

static void
test_timer_start_pos(void)
{
	printf("[RUN] test_timer_start_pos\n");
	ec_timer_t timer;
	ec_timer_init(&timer);
	count = 0;
	uint64_t id = ec_timer_start(&timer, 10, 0, EC_TIMER_ONESHOT, callback, NULL);
	TEST_ASSERT(id != EC_TIMER_INVALID);
	TEST_ASSERT(timer.events[id].active == true);
	ec_timer_cancel(&timer, id);
	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_start_pos\n");
}

static void
test_timer_start_periodic_pos(void)
{
	printf("[RUN] test_timer_start_periodic_pos\n");
	ec_timer_t timer;
	ec_timer_init(&timer);
	count = 0;
	uint64_t id = ec_timer_start(&timer, 10, 50, EC_TIMER_PERIODIC, callback, NULL);
	TEST_ASSERT(id != EC_TIMER_INVALID);
	TEST_ASSERT(timer.events[id].active == true);
	TEST_ASSERT(timer.events[id].type == EC_TIMER_PERIODIC);
	ec_timer_cancel(&timer, id);
	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_start_periodic_pos\n");
}

static void
test_timer_cancel_pos(void)
{
	printf("[RUN] test_timer_cancel_pos\n");
	ec_timer_t timer;
	ec_timer_init(&timer);
	uint64_t id = ec_timer_start(&timer, 100, 0, EC_TIMER_ONESHOT, callback, NULL);
	TEST_ASSERT(id != EC_TIMER_INVALID);
	TEST_ASSERT(timer.events[id].active == true);
	ec_timer_cancel(&timer, id);
	TEST_ASSERT(timer.events[id].active == false);
	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_cancel_pos\n");
}

static void
test_timer_process_pos(void)
{
	printf("[RUN] test_timer_process_pos\n");
	ec_timer_t timer;
	ec_timer_init(&timer);
	count = 1;
	uint64_t id = ec_timer_start(&timer, 10, 0, EC_TIMER_ONESHOT, callback, NULL);
	TEST_ASSERT(id != EC_TIMER_INVALID);
	uint64_t t = 100;
	while (t > 0 && count > 0)
		{
			ec_timer_process(&timer);
			sleep_ms(10);
			t -= 10;
		}
	TEST_ASSERT(count == 0);
	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_process_pos\n");
}

static void
test_timer_process_periodic_pos(void)
{
	printf("[RUN] test_timer_process_periodic_pos\n");
	ec_timer_t timer;
	ec_timer_init(&timer);
	count = 3;
	uint64_t id = ec_timer_start(&timer, 20, 30, EC_TIMER_PERIODIC, callback, NULL);
	TEST_ASSERT(id != EC_TIMER_INVALID);
	uint64_t t = 200;
	while (t > 0 && count > 0)
		{
			ec_timer_process(&timer);
			sleep_ms(10);
			t -= 10;
		}
	ec_timer_cancel(&timer, id);
	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_process_periodic_pos\n");
}

static void
test_timer_destroy_pos(void)
{
	printf("[RUN] test_timer_destroy_pos\n");
	ec_timer_t timer;
	ec_timer_init(&timer);
	uint64_t id = ec_timer_start(&timer, 100, 0, EC_TIMER_ONESHOT, callback, NULL);
	TEST_ASSERT(id != EC_TIMER_INVALID);
	ec_timer_destroy(&timer);
	/* Destroy should not crash even with active timers */
	printf("[PASS] test_timer_destroy_pos\n");
}

static void
test_timer_now_pos(void)
{
	printf("[RUN] test_timer_now_pos\n");
	uint64_t t1 = ec_timer_now();
	sleep_ms(10);
	uint64_t t2 = ec_timer_now();
	TEST_ASSERT(t2 > t1);
	printf("[PASS] test_timer_now_pos\n");
}

static void
test_timer_multiple_timers_pos(void)
{
	printf("[RUN] test_timer_multiple_timers_pos\n");
	ec_timer_t timer;
	ec_timer_init(&timer);
	count = 10;
	for (int i = 0; i < 10; i++)
		{
			ids[i] = ec_timer_start(&timer, (i + 1) * 20, 0, EC_TIMER_ONESHOT, callback, NULL);
			TEST_ASSERT(ids[i] != EC_TIMER_INVALID);
		}
	uint64_t t = 300;
	while (t > 0 && count > 0)
		{
			ec_timer_process(&timer);
			sleep_ms(10);
			t -= 10;
		}
	TEST_ASSERT(count == 0);
	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_multiple_timers_pos\n");
}

static void
test_timer_full_capacity_pos(void)
{
	printf("[RUN] test_timer_full_capacity_pos\n");
	ec_timer_t timer;
	ec_timer_init(&timer);
	count = 0;
	/* Fill all slots */
	for (int i = 0; i < EC_TIMER_MAX_EVENTS; i++)
		{
			ids[i] = ec_timer_start(&timer, 100, 0, EC_TIMER_ONESHOT, callback, NULL);
			TEST_ASSERT(ids[i] != EC_TIMER_INVALID);
			count++;
		}
	/* Try to add one more - should fail */
	uint64_t id = ec_timer_start(&timer, 100, 0, EC_TIMER_ONESHOT, callback, NULL);
	TEST_ASSERT(id == EC_TIMER_INVALID);
	/* Cancel all */
	for (int i = 0; i < EC_TIMER_MAX_EVENTS; i++)
		{
			ec_timer_cancel(&timer, ids[i]);
		}
	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_full_capacity_pos\n");
}

static void
test_timer_zero_delay_pos(void)
{
	printf("[RUN] test_timer_zero_delay_pos\n");
	ec_timer_t timer;
	ec_timer_init(&timer);
	count = 1;
	uint64_t id = ec_timer_start(&timer, 0, 0, EC_TIMER_ONESHOT, callback, NULL);
	TEST_ASSERT(id != EC_TIMER_INVALID);
	/* Process should trigger immediately */
	ec_timer_process(&timer);
	sleep_ms(5);
	ec_timer_process(&timer);
	TEST_ASSERT(count == 0);
	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_zero_delay_pos\n");
}

/* --- Negative Tests --- */

static void
test_timer_init_neg(void)
{
	printf("[RUN] test_timer_init_neg\n");
	/* NULL pointer should be handled gracefully */
	ec_timer_init(NULL);
	/* Should not crash */
	printf("[PASS] test_timer_init_neg\n");
}

static void
test_timer_start_neg_null_timer(void)
{
	printf("[RUN] test_timer_start_neg_null_timer\n");
	uint64_t id = ec_timer_start(NULL, 10, 0, EC_TIMER_ONESHOT, callback, NULL);
	TEST_ASSERT(id == EC_TIMER_INVALID);
	printf("[PASS] test_timer_start_neg_null_timer\n");
}

static void
test_timer_start_neg_null_callback(void)
{
	printf("[RUN] test_timer_start_neg_null_callback\n");
	ec_timer_t timer;
	ec_timer_init(&timer);
	uint64_t id = ec_timer_start(&timer, 10, 0, EC_TIMER_ONESHOT, NULL, NULL);
	TEST_ASSERT(id == EC_TIMER_INVALID);
	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_start_neg_null_callback\n");
}

static void
test_timer_cancel_neg_null_timer(void)
{
	printf("[RUN] test_timer_cancel_neg_null_timer\n");
	/* NULL pointer should be handled gracefully */
	ec_timer_cancel(NULL, 0);
	/* Should not crash */
	printf("[PASS] test_timer_cancel_neg_null_timer\n");
}

static void
test_timer_cancel_neg_invalid_id(void)
{
	printf("[RUN] test_timer_cancel_neg_invalid_id\n");
	ec_timer_t timer;
	ec_timer_init(&timer);
	/* Invalid ID should be handled gracefully */
	ec_timer_cancel(&timer, EC_TIMER_MAX_EVENTS);
	ec_timer_cancel(&timer, UINT64_MAX);
	/* Should not crash */
	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_cancel_neg_invalid_id\n");
}

static void
test_timer_process_neg(void)
{
	printf("[RUN] test_timer_process_neg\n");
	/* NULL pointer should be handled gracefully */
	ec_timer_process(NULL);
	/* Should not crash */
	printf("[PASS] test_timer_process_neg\n");
}

static void
test_timer_destroy_neg(void)
{
	printf("[RUN] test_timer_destroy_neg\n");
	/* NULL pointer should be handled gracefully */
	ec_timer_destroy(NULL);
	/* Should not crash */
	printf("[PASS] test_timer_destroy_neg\n");
}

/* --- Edge Cases and Stress Tests --- */

static void
test_timer_stress(void)
{
	printf("[RUN] test_timer_stress\n");
	ec_timer_t timer;
	ec_timer_init(&timer);
	srand((unsigned)time(NULL));
	count = 0;
	for (int i = 0; i < EC_TIMER_MAX_EVENTS; i++)
		{
			ids[i] = ec_timer_start(&timer, rand() % 50, 0, EC_TIMER_ONESHOT, callback, NULL);
			if (ids[i] != EC_TIMER_INVALID)
				count++;
		}
	uint64_t t = 1000;
	while (t > 0 && count > 0)
		{
			ec_timer_process(&timer);
			sleep_ms(10);
			t -= 10;
		}
	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_stress\n");
}

static void
test_timer_large_delay_pos(void)
{
	printf("[RUN] test_timer_large_delay_pos\n");
	ec_timer_t timer;
	ec_timer_init(&timer);
	count = 1;
	/* Timer with very large delay should not execute immediately */
	uint64_t id = ec_timer_start(&timer, UINT64_MAX / 2, 0, EC_TIMER_ONESHOT, callback, NULL);
	TEST_ASSERT(id != EC_TIMER_INVALID);
	/* Process should not trigger it */
	ec_timer_process(&timer);
	sleep_ms(10);
	ec_timer_process(&timer);
	TEST_ASSERT(count == 1);
	TEST_ASSERT(timer.events[id].active == true);
	ec_timer_cancel(&timer, id);
	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_large_delay_pos\n");
}

static void
test_timer_cancel_and_restart_pos(void)
{
	printf("[RUN] test_timer_cancel_and_restart_pos\n");
	ec_timer_t timer;
	ec_timer_init(&timer);
	count = 1;
	uint64_t id = ec_timer_start(&timer, 100, 0, EC_TIMER_ONESHOT, callback, NULL);
	TEST_ASSERT(id != EC_TIMER_INVALID);
	ec_timer_cancel(&timer, id);
	TEST_ASSERT(timer.events[id].active == false);
	/* Restart on same slot */
	count = 1;
	uint64_t id2 = ec_timer_start(&timer, 10, 0, EC_TIMER_ONESHOT, callback, NULL);
	TEST_ASSERT(id2 != EC_TIMER_INVALID);
	uint64_t t = 100;
	while (t > 0 && count > 0)
		{
			ec_timer_process(&timer);
			sleep_ms(10);
			t -= 10;
		}
	TEST_ASSERT(count == 0);
	ec_timer_destroy(&timer);
	printf("[PASS] test_timer_cancel_and_restart_pos\n");
}

/* Run All Tests */
int
main(void)
{
	printf("\nStarting `ec_timer` Tests...\n");

	/* Positive Tests */
	test_timer_init_pos();
	test_timer_start_pos();
	test_timer_start_periodic_pos();
	test_timer_cancel_pos();
	test_timer_process_pos();
	test_timer_process_periodic_pos();
	test_timer_destroy_pos();
	test_timer_now_pos();
	test_timer_multiple_timers_pos();
	test_timer_full_capacity_pos();
	test_timer_zero_delay_pos();
	test_timer_large_delay_pos();
	test_timer_cancel_and_restart_pos();

	/* Negative Tests */
	test_timer_init_neg();
	test_timer_start_neg_null_timer();
	test_timer_start_neg_null_callback();
	test_timer_cancel_neg_null_timer();
	test_timer_cancel_neg_invalid_id();
	test_timer_process_neg();
	test_timer_destroy_neg();

	/* Stress Tests */
	test_timer_stress();

	printf("\nAll `ec_timer` tests passed successfully!\n");
	return 0;
}
