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
		printf("[timeerr] \n");
}

// Test functions for ec_queue_long (a queue of int)

void
test_queue_long_enqueue(void* mp)
{
	stamp();
	printf("test_queue_long_enqueue\n\n");
	ec_queue_long_t q;
	ec_status_t st = ec_queue_long_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	long a = 10L, b = 20;
	st = q.enqueue(&q, &a);
	TEST_ASSERT(st == EC_OK && q.size_fn(&q) == 1);
	st = q.enqueue(&q, &b);
	TEST_ASSERT(st == EC_OK && q.size_fn(&q) == 2);
	long* peek_val = malloc(sizeof(long));
	TEST_ASSERT(peek_val != NULL);
	st = q.peek(&q, (void**)&peek_val);
	TEST_ASSERT(st == EC_OK && *peek_val == 10);
	q.free_fn(&q);
	EC_FREE(peek_val);
	printf("pass: test_queue_long_enqueue\n\n");
}

void
test_queue_long_peek(void* mp)
{
	stamp();
	printf("test_queue_long_peek\n\n");
	ec_queue_long_t q;
	ec_status_t st = ec_queue_long_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	long a = 42L;
	st = q.enqueue(&q, &a);
	TEST_ASSERT(st == EC_OK);
	long* peek_val = malloc(sizeof(long));
	TEST_ASSERT(peek_val != NULL);
	st = q.peek(&q, (void**)&peek_val);
	TEST_ASSERT(st == EC_OK && *peek_val == 42);
	q.free_fn(&q);
	EC_FREE(peek_val);
	printf("pass: test_queue_long_peek\n\n");
}

void
test_queue_long_dequeue(void* mp)
{
	stamp();
	printf("test_queue_long_dequeue\n\n");
	ec_queue_long_t q;
	ec_status_t st = ec_queue_long_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	long a = 10L, b = 20, c = 30;
	st = q.enqueue(&q, &a);
	TEST_ASSERT(st == EC_OK);
	st = q.enqueue(&q, &b);
	TEST_ASSERT(st == EC_OK);
	st = q.enqueue(&q, &c);
	TEST_ASSERT(st == EC_OK);

	long* dequeue_val = malloc(sizeof(long));
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
	printf("pass: test_queue_long_dequeue\n\n");
}

void
test_queue_long_size(void* mp)
{
	stamp();
	printf("test_queue_long_size\n\n");
	ec_queue_long_t q;
	ec_status_t st = ec_queue_long_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	TEST_ASSERT(q.size_fn(&q) == 0);
	long a = 1L, b = 2;
	q.enqueue(&q, &a);
	TEST_ASSERT(q.size_fn(&q) == 1);
	q.enqueue(&q, &b);
	TEST_ASSERT(q.size_fn(&q) == 2);
	q.free_fn(&q);
	printf("pass: test_queue_long_size\n\n");
}

void
test_queue_long_free(void* mp)
{
	stamp();
	printf("test_queue_long_free\n\n");
	ec_queue_long_t q;
	ec_status_t st = ec_queue_long_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	long a = 1L, b = 6;
	q.enqueue(&q, &a);
	q.enqueue(&q, &b);
	q.free_fn(&q);
	TEST_ASSERT(q.size_fn(&q) == 0);
	printf("pass: test_queue_long_free\n\n");
}

// Iterator Test for ec_queue_long
void
test_queue_long_iterators(void* mp)
{
	stamp();
	printf("test_queue_long_iterators\n\n");
	ec_queue_long_t q;
	ec_status_t st = ec_queue_long_init(&q, mp);
	TEST_ASSERT(st == EC_OK);

	long v1 = 1L, v2 = 2, v3 = 3;
	q.enqueue(&q, &v1);
	q.enqueue(&q, &v2);
	q.enqueue(&q, &v3);

	ec_queue_long_iter_t it;
	q.iter_init(&it, &q);
	void* out = malloc(sizeof(long));
	TEST_ASSERT(out != NULL);
	long expected[] = { 1, 2, 3 };
	int idx = 0;

	while (q.iter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(*(long*)out == expected[idx]);
			idx++;
		}
	TEST_ASSERT(idx == 3);

	// Test iterator replacement
	q.iter_init(&it, &q);
	q.iter_next(&it, &out); // first element (1)
	long new_val = 99L;
	st = q.iter_replace(&it, &new_val, NULL);
	TEST_ASSERT(st == EC_OK);

	long* peek_val = malloc(sizeof(long));
	TEST_ASSERT(peek_val != NULL);
	q.peek(&q, (void**)&peek_val);
	TEST_ASSERT(*peek_val == 99);

	q.free_fn(&q);
	EC_FREE(peek_val);
	EC_FREE(out);
	printf("pass: test_queue_long_iterators\n\n");
}

// Zip Iterator Test for ec_queue_long
void
test_queue_long_zip_iterators(void* mp)
{
	stamp();
	printf("test_queue_long_zip_iterators\n\n");
	ec_queue_long_t q1, q2;
	ec_status_t st = ec_queue_long_init(&q1, mp);
	TEST_ASSERT(st == EC_OK);
	st = ec_queue_long_init(&q2, mp);
	TEST_ASSERT(st == EC_OK);

	long a = 10L, b = 20, c = 30;
	long d = 100L, e = 200, f = 300;
	q1.enqueue(&q1, &a);
	q1.enqueue(&q1, &b);
	q1.enqueue(&q1, &c);
	q2.enqueue(&q2, &d);
	q2.enqueue(&q2, &e);
	q2.enqueue(&q2, &f);

	ec_queue_long_zip_iter_t zit;
	q1.zip_iter_init(&zit, &q1, &q2);
	void* out1 = malloc(sizeof(long));
	void* out2 = malloc(sizeof(long));
	TEST_ASSERT(out1 != NULL && out2 != NULL);
	long expected1[] = { 10, 20, 30 };
	long expected2[] = { 100, 200, 300 };
	int count = 0;

	while (q1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			TEST_ASSERT(*(long*)out1 == expected1[count]);
			TEST_ASSERT(*(long*)out2 == expected2[count]);
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
	printf("pass: test_queue_long_zip_iterators\n\n");
}

void
test_queue_long_init_neg_null(void* mp)
{
	stamp();
	printf("test_queue_long_init_neg_null\n");
	ec_status_t st = ec_queue_long_init(NULL, mp);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	printf("pass: test_queue_long_init_neg_null\n");
}

void
test_queue_long_enqueue_neg_null(void* mp)
{
	stamp();
	printf("test_queue_long_enqueue_neg_null\n");
	ec_queue_long_t q;
	ec_status_t st = ec_queue_long_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	long a = 10L;
	st = q.enqueue(NULL, &a);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	q.free_fn(&q);
	printf("pass: test_queue_long_enqueue_neg_null\n");
}

void
test_queue_long_peek_neg_empty(void* mp)
{
	stamp();
	printf("test_queue_long_peek_neg_empty\n");
	ec_queue_long_t q;
	ec_status_t st = ec_queue_long_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	long* peek_val = malloc(sizeof(long));
	TEST_ASSERT(peek_val != NULL);
	st = q.peek(&q, (void**)&peek_val);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);
	q.free_fn(&q);
	EC_FREE(peek_val);
	printf("pass: test_queue_long_peek_neg_empty\n");
}

void
test_queue_long_peek_neg_null(void* mp)
{
	stamp();
	printf("test_queue_long_peek_neg_null\n");
	ec_queue_long_t q;
	ec_status_t st = ec_queue_long_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	long a = 10L;
	q.enqueue(&q, &a);
	st = q.peek(NULL, NULL);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	q.free_fn(&q);
	printf("pass: test_queue_long_peek_neg_null\n");
}

void
test_queue_long_dequeue_neg_null(void* mp)
{
	stamp();
	printf("test_queue_long_dequeue_neg_null\n");
	long* out = malloc(sizeof(long));
	TEST_ASSERT(out != NULL);
	ec_status_t st = ec_queue_long_dequeue(NULL, (void**)&out);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	EC_FREE(out);
	printf("pass: test_queue_long_dequeue_neg_null\n");
}

void
test_queue_long_free_neg_null(void* mp)
{
	stamp();
	printf("test_queue_long_free_neg_null\n");
	ec_status_t st = ec_queue_long_free_fn(NULL);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	printf("pass: test_queue_long_free_neg_null\n");
}

void
test_queue_long_iter_replace_neg_no_prev(void* mp)
{
	stamp();
	printf("test_queue_long_iter_replace_neg_no_prev\n");
	ec_queue_long_t q;
	ec_status_t st = ec_queue_long_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	ec_queue_long_iter_t it;
	q.iter_init(&it, &q);
	long new_val = 30L;
	st = q.iter_replace(&it, &new_val, NULL);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	q.free_fn(&q);
	printf("pass: test_queue_long_iter_replace_neg_no_prev\n");
}

void
test_queue_long_iter_replace_neg_null(void* mp)
{
	stamp();
	printf("test_queue_long_iter_replace_neg_null\n");
	long new_val = 30L;
	ec_status_t st = ec_queue_long_iter_replace(NULL, &new_val, NULL);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	printf("pass: test_queue_long_iter_replace_neg_null\n");
}

void
test_queue_long_zip_iter_next_neg_mismatch(void* mp)
{
	stamp();
	printf("test_queue_long_zip_iter_next_neg_mismatch\n");
	ec_queue_long_t q1, q2;
	ec_status_t st = ec_queue_long_init(&q1, mp);
	TEST_ASSERT(st == EC_OK);
	st = ec_queue_long_init(&q2, mp);
	TEST_ASSERT(st == EC_OK);
	long a = 10L, b = 20;
	q1.enqueue(&q1, &a);
	q1.enqueue(&q1, &b);
	long d = 100L;
	q2.enqueue(&q2, &d);
	ec_queue_long_zip_iter_t zit;
	q1.zip_iter_init(&zit, &q1, &q2);
	void* out1 = malloc(sizeof(long));
	void* out2 = malloc(sizeof(long));
	TEST_ASSERT(out1 != NULL && out2 != NULL);
	st = q1.zip_iter_next(&zit, &out1, &out2);
	TEST_ASSERT(st == EC_OK);
	st = q1.zip_iter_next(&zit, &out1, &out2);
	TEST_ASSERT(st == EC_ITER_END);
	q1.free_fn(&q1);
	q2.free_fn(&q2);
	EC_FREE(out1);
	EC_FREE(out2);
	printf("pass: test_queue_long_zip_iter_next_neg_mismatch\n");
}

void
test_queue_long_zip_iter_next_neg_null(void* mp)
{
	stamp();
	printf("test_queue_long_zip_iter_next_neg_null\n");
	ec_status_t st = ec_queue_long_zip_iter_next(NULL, NULL, NULL);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	printf("pass: test_queue_long_zip_iter_next_neg_null\n");
}

int
main(void)
{
	printf("Starting ec_queue_long tests...\n\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_queue_long_init_neg_null(mp);
	test_queue_long_enqueue(mp);
	test_queue_long_enqueue_neg_null(mp);
	test_queue_long_peek(mp);
	test_queue_long_peek_neg_empty(mp);
	test_queue_long_peek_neg_null(mp);
	test_queue_long_dequeue(mp);
	test_queue_long_dequeue_neg_null(mp);
	test_queue_long_size(mp);
	test_queue_long_free(mp);
	test_queue_long_free_neg_null(mp);
	test_queue_long_iterators(mp);
	test_queue_long_iter_replace_neg_no_prev(mp);
	test_queue_long_iter_replace_neg_null(mp);
	test_queue_long_zip_iterators(mp);
	test_queue_long_zip_iter_next_neg_mismatch(mp);
	test_queue_long_zip_iter_next_neg_null(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif

	printf("All ec_queue_long tests passed.\n\n");
	return 0;
}
