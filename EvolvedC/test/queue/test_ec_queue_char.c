#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../../src/ec_queue.h"

EC_QUEUE_DEF(char, ch)
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

// Test functions for ec_queue_ch (a queue of int)

void
test_queue_ch_enqueue(void* mp)
{
	stamp();
	printf("test_queue_ch_enqueue\n");
	ec_queue_ch_t q;
	ec_status_t st = ec_queue_ch_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	char a = 'a', b = 'b';
	st = q.enqueue(&q, &a);
	TEST_ASSERT(st == EC_OK && q.size_fn(&q) == 1);
	st = q.enqueue(&q, &b);
	TEST_ASSERT(st == EC_OK && q.size_fn(&q) == 2);
	char* peek_val = malloc(sizeof(char));
	TEST_ASSERT(peek_val != NULL);
	st = q.peek(&q, (void**)&peek_val);
	TEST_ASSERT(st == EC_OK && *peek_val == 'a');
	q.free_fn(&q);
	EC_FREE(peek_val);
	printf("pass: test_queue_ch_enqueue\n");
}

void
test_queue_ch_peek(void* mp)
{
	stamp();
	printf("test_queue_ch_peek\n");
	ec_queue_ch_t q;
	ec_status_t st = ec_queue_ch_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	char a = 'a';
	st = q.enqueue(&q, &a);
	TEST_ASSERT(st == EC_OK);
	char* peek_val = malloc(sizeof(char));
	TEST_ASSERT(peek_val != NULL);
	st = q.peek(&q, (void**)&peek_val);
	TEST_ASSERT(st == EC_OK && *peek_val == 'a');
	q.free_fn(&q);
	EC_FREE(peek_val);
	printf("pass: test_queue_ch_peek\n");
}

void
test_queue_ch_dequeue(void* mp)
{
	stamp();
	printf("test_queue_ch_dequeue\n");
	ec_queue_ch_t q;
	ec_status_t st = ec_queue_ch_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	char a = 'a', b = 'b', c = 'c';
	st = q.enqueue(&q, &a);
	TEST_ASSERT(st == EC_OK);
	st = q.enqueue(&q, &b);
	TEST_ASSERT(st == EC_OK);
	st = q.enqueue(&q, &c);
	TEST_ASSERT(st == EC_OK);

	char* dequeue_val = malloc(sizeof(char));
	TEST_ASSERT(dequeue_val != NULL);
	ec_status_t st2 = q.dequeue(&q, (void**)&dequeue_val);
	TEST_ASSERT(st2 == EC_OK && *dequeue_val == 'a' && q.size_fn(&q) == 2);

	st = q.dequeue(&q, (void**)&dequeue_val);
	TEST_ASSERT(st == EC_OK && *dequeue_val == 'b' && q.size_fn(&q) == 1);

	st = q.dequeue(&q, (void**)&dequeue_val);
	TEST_ASSERT(st == EC_OK && *dequeue_val == 'c' && q.size_fn(&q) == 0);

	st = q.dequeue(&q, (void**)&dequeue_val);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);

	q.free_fn(&q);
	EC_FREE(dequeue_val);
	printf("pass: test_queue_ch_dequeue\n");
}

void
test_queue_ch_size(void* mp)
{
	stamp();
	printf("test_queue_ch_size\n");
	ec_queue_ch_t q;
	ec_status_t st = ec_queue_ch_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	TEST_ASSERT(q.size_fn(&q) == 0);
	char a = 'x', b = 2;
	q.enqueue(&q, &a);
	TEST_ASSERT(q.size_fn(&q) == 1);
	q.enqueue(&q, &b);
	TEST_ASSERT(q.size_fn(&q) == 2);
	q.free_fn(&q);
	printf("pass: test_queue_ch_size\n");
}

void
test_queue_ch_free(void* mp)
{
	stamp();
	printf("test_queue_ch_free\n");
	ec_queue_ch_t q;
	ec_status_t st = ec_queue_ch_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	char a = 'x', b = 6;
	q.enqueue(&q, &a);
	q.enqueue(&q, &b);
	q.free_fn(&q);
	TEST_ASSERT(q.size_fn(&q) == 0);
	printf("pass: test_queue_ch_free\n");
}

// Iterator Test for ec_queue_ch
void
test_queue_ch_iterators(void* mp)
{
	stamp();
	printf("test_queue_ch_iterators\n");
	ec_queue_ch_t q;
	ec_status_t st = ec_queue_ch_init(&q, mp);
	TEST_ASSERT(st == EC_OK);

	char v1 = 'x', v2 = 'y', v3 = 'z';
	q.enqueue(&q, &v1);
	q.enqueue(&q, &v2);
	q.enqueue(&q, &v3);

	ec_queue_ch_iter_t it;
	q.iter_init(&it, &q);
	void* out = malloc(sizeof(char));
	TEST_ASSERT(out != NULL);
	char expected[] = { 'x', 'y', 'z' };
	int idx = 0;

	while (q.iter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(*(char*)out == expected[idx]);
			idx++;
		}
	TEST_ASSERT(idx == 3);

	// Test iterator replacement
	q.iter_init(&it, &q);
	q.iter_next(&it, &out); // first element (1)
	char new_val = 'c';
	st = q.iter_replace(&it, &new_val, NULL);
	TEST_ASSERT(st == EC_OK);

	char* peek_val = malloc(sizeof(char));
	TEST_ASSERT(peek_val != NULL);
	q.peek(&q, (void**)&peek_val);
	TEST_ASSERT(*peek_val == 'c');

	q.free_fn(&q);
	EC_FREE(peek_val);
	EC_FREE(out);
	printf("pass: test_queue_ch_iterators\n");
}

// Zip Iterator Test for ec_queue_ch
void
test_queue_ch_zip_iterators(void* mp)
{
	stamp();
	printf("test_queue_ch_zip_iterators\n");
	ec_queue_ch_t q1, q2;
	ec_status_t st = ec_queue_ch_init(&q1, mp);
	TEST_ASSERT(st == EC_OK);
	st = ec_queue_ch_init(&q2, mp);
	TEST_ASSERT(st == EC_OK);

	char a = 'a', b = 'b', c = 'c';
	char d = 'a', e = 'b', f = 'c';
	q1.enqueue(&q1, &a);
	q1.enqueue(&q1, &b);
	q1.enqueue(&q1, &c);
	q2.enqueue(&q2, &d);
	q2.enqueue(&q2, &e);
	q2.enqueue(&q2, &f);

	ec_queue_ch_zip_iter_t zit;
	q1.zip_iter_init(&zit, &q1, &q2);
	void* out1 = malloc(sizeof(char));
	void* out2 = malloc(sizeof(char));
	TEST_ASSERT(out1 != NULL && out2 != NULL);
	char expected1[] = { 'a', 'b', 'c' };
	char expected2[] = { 'a', 'b', 'c' };
	int count = 0;

	while (q1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			TEST_ASSERT(*(char*)out1 == expected1[count]);
			TEST_ASSERT(*(char*)out2 == expected2[count]);
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
	printf("pass: test_queue_ch_zip_iterators\n");
}

void
test_queue_ch_init_neg_null(void* mp)
{
	stamp();
	printf("test_queue_ch_init_neg_null\n");
	ec_status_t st = ec_queue_ch_init(NULL, mp);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	printf("pass: test_queue_ch_init_neg_null\n");
}

void
test_queue_ch_enqueue_neg_null(void* mp)
{
	stamp();
	printf("test_queue_ch_enqueue_neg_null\n");
	ec_queue_ch_t q;
	ec_status_t st = ec_queue_ch_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	char a = 'a';
	st = q.enqueue(NULL, &a);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	q.free_fn(&q);
	printf("pass: test_queue_ch_enqueue_neg_null\n");
}

void
test_queue_ch_peek_neg_empty(void* mp)
{
	stamp();
	printf("test_queue_ch_peek_neg_empty\n");
	ec_queue_ch_t q;
	ec_status_t st = ec_queue_ch_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	char* peek_val = malloc(sizeof(char));
	TEST_ASSERT(peek_val != NULL);
	st = q.peek(&q, (void**)&peek_val);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);
	q.free_fn(&q);
	EC_FREE(peek_val);
	printf("pass: test_queue_ch_peek_neg_empty\n");
}

void
test_queue_ch_peek_neg_null(void* mp)
{
	stamp();
	printf("test_queue_ch_peek_neg_null\n");
	ec_queue_ch_t q;
	ec_status_t st = ec_queue_ch_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	char a = 'a';
	q.enqueue(&q, &a);
	st = q.peek(NULL, NULL);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	q.free_fn(&q);
	printf("pass: test_queue_ch_peek_neg_null\n");
}

void
test_queue_ch_dequeue_neg_null(void* mp)
{
	stamp();
	printf("test_queue_ch_dequeue_neg_null\n");
	char* out = malloc(sizeof(char));
	TEST_ASSERT(out != NULL);
	ec_status_t st = ec_queue_ch_dequeue(NULL, (void**)&out);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	EC_FREE(out);
	printf("pass: test_queue_ch_dequeue_neg_null\n");
}

void
test_queue_ch_free_neg_null(void* mp)
{
	stamp();
	printf("test_queue_ch_free_neg_null\n");
	ec_status_t st = ec_queue_ch_free_fn(NULL);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	printf("pass: test_queue_ch_free_neg_null\n");
}

void
test_queue_ch_iter_replace_neg_no_prev(void* mp)
{
	stamp();
	printf("test_queue_ch_iter_replace_neg_no_prev\n");
	ec_queue_ch_t q;
	ec_status_t st = ec_queue_ch_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	ec_queue_ch_iter_t it;
	q.iter_init(&it, &q);
	char new_val = 'c';
	st = q.iter_replace(&it, &new_val, NULL);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	q.free_fn(&q);
	printf("pass: test_queue_ch_iter_replace_neg_no_prev\n");
}

void
test_queue_ch_iter_replace_neg_null(void* mp)
{
	stamp();
	printf("test_queue_ch_iter_replace_neg_null\n");
	char new_val = 'c';
	ec_status_t st = ec_queue_ch_iter_replace(NULL, &new_val, NULL);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	printf("pass: test_queue_ch_iter_replace_neg_null\n");
}

void
test_queue_ch_zip_iter_next_neg_mismatch(void* mp)
{
	stamp();
	printf("test_queue_ch_zip_iter_next_neg_mismatch\n");
	ec_queue_ch_t q1, q2;
	ec_status_t st = ec_queue_ch_init(&q1, mp);
	TEST_ASSERT(st == EC_OK);
	st = ec_queue_ch_init(&q2, mp);
	TEST_ASSERT(st == EC_OK);
	char a = 'a', b = 'b';
	q1.enqueue(&q1, &a);
	q1.enqueue(&q1, &b);
	char d = 'a';
	q2.enqueue(&q2, &d);
	ec_queue_ch_zip_iter_t zit;
	q1.zip_iter_init(&zit, &q1, &q2);
	void* out1 = malloc(sizeof(char));
	void* out2 = malloc(sizeof(char));
	TEST_ASSERT(out1 != NULL && out2 != NULL);
	st = q1.zip_iter_next(&zit, &out1, &out2);
	TEST_ASSERT(st == EC_OK);
	st = q1.zip_iter_next(&zit, &out1, &out2);
	TEST_ASSERT(st == EC_ITER_END);
	q1.free_fn(&q1);
	q2.free_fn(&q2);
	EC_FREE(out1);
	EC_FREE(out2);
	printf("pass: test_queue_ch_zip_iter_next_neg_mismatch\n");
}

void
test_queue_ch_zip_iter_next_neg_null(void* mp)
{
	stamp();
	printf("test_queue_ch_zip_iter_next_neg_null\n");
	ec_status_t st = ec_queue_ch_zip_iter_next(NULL, NULL, NULL);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	printf("pass: test_queue_ch_zip_iter_next_neg_null\n");
}

int
main(void)
{
	printf("Starting ec_queue_ch tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_queue_ch_init_neg_null(mp);
	test_queue_ch_enqueue(mp);
	test_queue_ch_enqueue_neg_null(mp);
	test_queue_ch_peek(mp);
	test_queue_ch_peek_neg_empty(mp);
	test_queue_ch_peek_neg_null(mp);
	test_queue_ch_dequeue(mp);
	test_queue_ch_dequeue_neg_null(mp);
	test_queue_ch_size(mp);
	test_queue_ch_free(mp);
	test_queue_ch_free_neg_null(mp);
	test_queue_ch_iterators(mp);
	test_queue_ch_iter_replace_neg_no_prev(mp);
	test_queue_ch_iter_replace_neg_null(mp);
	test_queue_ch_zip_iterators(mp);
	test_queue_ch_zip_iter_next_neg_mismatch(mp);
	test_queue_ch_zip_iter_next_neg_null(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif

	printf("All ec_queue_ch tests passed.\n");
	return 0;
}
