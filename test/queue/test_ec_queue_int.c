#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../../src/ec_queue.h"
#include <assert.h>
#include <stdbool.h>
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

// Utility: print a stack trace (if available)
static void
print_stacktrace(void)
{
#ifdef __linux__
	void* array[32];
	size_t size = backtrace(array, 32);
	char** symbols = backtrace_symbols(array, size);
	if (symbols)
		{
			fprintf(stderr, "--- Stack Trace (most recent call last) ---\n");
			for (size_t i = 0; i < size; i++)
				fprintf(stderr, "%s\n", symbols[i]);
			free(symbols);
		}
#else
	fprintf(stderr, "(Stack trace not supported on this platform)\n");
#endif
}

// Macro for assertions in tests
#define TEST_ASSERT(cond)                                                                    \
	do                                                                                       \
		{                                                                                    \
			if (!(cond))                                                                     \
				{                                                                            \
					fprintf(stderr, "[ASSERT FAIL] %s:%d: %s\n", __FILE__, __LINE__, #cond); \
					print_stacktrace();                                                      \
					exit(3);                                                                 \
				}                                                                            \
		}                                                                                    \
	while (0)

// Utility: print timestamp
static void
stamp(void)
{
	time_t t = time(NULL);
	struct tm* m = localtime(&t);
	if (m)
		{
			char b[64];
			strftime(b, sizeof(b), "[%Y-%m-%d %H:%M:%S]", m);
			printf("%s ", b);
		}
	else
		printf("[timeerr] ");
}

// Test functions for ec_queue_int (a queue of int)

void
test_queue_int_enqueue(void* mp)
{
	stamp();
	printf("test_queue_int_enqueue\n");
	ec_queue_int_t q;
	ec_status_t st = ec_queue_int_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	int a = 10, b = 20;
	st = q.enqueue(&q, &a);
	TEST_ASSERT(st == EC_OK && q.size_fn(&q) == 1);
	st = q.enqueue(&q, &b);
	TEST_ASSERT(st == EC_OK && q.size_fn(&q) == 2);
	int* peek_val = malloc(sizeof(int));
	TEST_ASSERT(peek_val != NULL);
	st = q.peek(&q, (void**)&peek_val);
	TEST_ASSERT(st == EC_OK && *peek_val == 10);
	q.free_fn(&q);
	EC_FREE(peek_val);
	printf("pass: test_queue_int_enqueue\n");
}

void
test_queue_int_peek(void* mp)
{
	stamp();
	printf("test_queue_int_peek\n");
	ec_queue_int_t q;
	ec_status_t st = ec_queue_int_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	int a = 42;
	st = q.enqueue(&q, &a);
	TEST_ASSERT(st == EC_OK);
	int* peek_val = malloc(sizeof(int));
	TEST_ASSERT(peek_val != NULL);
	st = q.peek(&q, (void**)&peek_val);
	TEST_ASSERT(st == EC_OK && *peek_val == 42);
	q.free_fn(&q);
	EC_FREE(peek_val);
	printf("pass: test_queue_int_peek\n");
}

void
test_queue_int_dequeue(void* mp)
{
	stamp();
	printf("test_queue_int_dequeue\n");
	ec_queue_int_t q;
	ec_status_t st = ec_queue_int_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	int a = 10, b = 20, c = 30;
	st = q.enqueue(&q, &a);
	TEST_ASSERT(st == EC_OK);
	st = q.enqueue(&q, &b);
	TEST_ASSERT(st == EC_OK);
	st = q.enqueue(&q, &c);
	TEST_ASSERT(st == EC_OK);

	int* dequeue_val = malloc(sizeof(int));
	TEST_ASSERT(dequeue_val != NULL);
	st = q.dequeue(&q, (void**)&dequeue_val);
	TEST_ASSERT(st == EC_OK && *dequeue_val == 10 && q.size_fn(&q) == 2);

	st = q.dequeue(&q, (void**)&dequeue_val);
	TEST_ASSERT(st == EC_OK && *dequeue_val == 20 && q.size_fn(&q) == 1);

	st = q.dequeue(&q, (void**)&dequeue_val);
	TEST_ASSERT(st == EC_OK && *dequeue_val == 30 && q.size_fn(&q) == 0);

	st = q.dequeue(&q, (void**)&dequeue_val);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);

	q.free_fn(&q);
	EC_FREE(dequeue_val);
	printf("pass: test_queue_int_dequeue\n");
}

void
test_queue_int_size(void* mp)
{
	stamp();
	printf("test_queue_int_size\n");
	ec_queue_int_t q;
	ec_status_t st = ec_queue_int_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	TEST_ASSERT(q.size_fn(&q) == 0);
	int a = 1, b = 2;
	q.enqueue(&q, &a);
	TEST_ASSERT(q.size_fn(&q) == 1);
	q.enqueue(&q, &b);
	TEST_ASSERT(q.size_fn(&q) == 2);
	q.free_fn(&q);
	printf("pass: test_queue_int_size\n");
}

void
test_queue_int_free(void* mp)
{
	stamp();
	printf("test_queue_int_free\n");
	ec_queue_int_t q;
	ec_status_t st = ec_queue_int_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	int a = 5, b = 6;
	q.enqueue(&q, &a);
	q.enqueue(&q, &b);
	q.free_fn(&q);
	TEST_ASSERT(q.size_fn(&q) == 0);
	printf("pass: test_queue_int_free\n");
}

// Iterator Test for ec_queue_int
void
test_queue_int_iterators(void* mp)
{
	stamp();
	printf("test_queue_int_iterators\n");
	ec_queue_int_t q;
	ec_status_t st = ec_queue_int_init(&q, mp);
	TEST_ASSERT(st == EC_OK);

	int v1 = 1, v2 = 2, v3 = 3;
	q.enqueue(&q, &v1);
	q.enqueue(&q, &v2);
	q.enqueue(&q, &v3);

	ec_queue_int_iter_t it;
	q.iter_init(&it, &q);
	void* out = malloc(sizeof(int));
	TEST_ASSERT(out != NULL);
	int expected[] = { 1, 2, 3 };
	int idx = 0;

	while (q.iter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(*(int*)out == expected[idx]);
			idx++;
		}
	TEST_ASSERT(idx == 3);

	// Test iterator replacement
	q.iter_init(&it, &q);
	q.iter_next(&it, &out); // first element (1)
	int new_val = 99;
	st = q.iter_replace(&it, &new_val, NULL);
	TEST_ASSERT(st == EC_OK);

	int* peek_val = malloc(sizeof(int));
	TEST_ASSERT(peek_val != NULL);
	q.peek(&q, (void**)&peek_val);
	TEST_ASSERT(*peek_val == 99);

	q.free_fn(&q);
	EC_FREE(peek_val);
	EC_FREE(out);
	printf("pass: test_queue_int_iterators\n");
}

// Zip Iterator Test for ec_queue_int
void
test_queue_int_zip_iterators(void* mp)
{
	stamp();
	printf("test_queue_int_zip_iterators\n");
	ec_queue_int_t q1, q2;
	ec_status_t st = ec_queue_int_init(&q1, mp);
	TEST_ASSERT(st == EC_OK);
	st = ec_queue_int_init(&q2, mp);
	TEST_ASSERT(st == EC_OK);

	int a = 10, b = 20, c = 30;
	int d = 100, e = 200, f = 300;
	q1.enqueue(&q1, &a);
	q1.enqueue(&q1, &b);
	q1.enqueue(&q1, &c);
	q2.enqueue(&q2, &d);
	q2.enqueue(&q2, &e);
	q2.enqueue(&q2, &f);

	ec_queue_int_zip_iter_t zit;
	q1.zip_iter_init(&zit, &q1, &q2);
	void* out1 = malloc(sizeof(int));
	void* out2 = malloc(sizeof(int));
	TEST_ASSERT(out1 != NULL && out2 != NULL);
	int expected1[] = { 10, 20, 30 };
	int expected2[] = { 100, 200, 300 };
	int count = 0;

	while (q1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			TEST_ASSERT(*(int*)out1 == expected1[count]);
			TEST_ASSERT(*(int*)out2 == expected2[count]);
			count++;
		}
	TEST_ASSERT(count == 3);

	// Test zip iterator replace (expected to return not supported)
	st = q1.zip_iter_replace(&zit, &a, &d, NULL, NULL);
	TEST_ASSERT(st == EC_ERR_NOT_SUPPORTED);

	q1.free_fn(&q1);
	q2.free_fn(&q2);
	EC_FREE(out1);
	EC_FREE(out2);
	printf("pass: test_queue_int_zip_iterators\n");
}

int
main(void)
{
	printf("Starting ec_queue_int tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_queue_int_enqueue(mp);
	test_queue_int_peek(mp);
	test_queue_int_dequeue(mp);
	test_queue_int_size(mp);
	test_queue_int_free(mp);
	test_queue_int_iterators(mp);
	test_queue_int_zip_iterators(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif

	printf("All ec_queue_int tests passed.\n");
	return 0;
}
