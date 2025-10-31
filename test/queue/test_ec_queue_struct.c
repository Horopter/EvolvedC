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

typedef struct
{
	int id;
} rec_t;
EC_QUEUE_DEF(rec_t*, recp)

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

// Test functions for ec_queue_recp (a queue of rec_t*)

void
test_queue_recp_enqueue(void* mp)
{
	stamp();
	printf("test_queue_recp_enqueue\n\n");
	ec_queue_recp_t q;
	ec_status_t st = ec_queue_recp_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	rec_t rec_a = { 10 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 20 };
	rec_t* b = &rec_b;
	st = q.enqueue(&q, &a);
	TEST_ASSERT(st == EC_OK && q.size_fn(&q) == 1);
	st = q.enqueue(&q, &b);
	TEST_ASSERT(st == EC_OK && q.size_fn(&q) == 2);
	rec_t** peek_val = malloc(sizeof(rec_t*));
	TEST_ASSERT(peek_val != NULL);
	ec_status_t st = q.peek(&q, (void**)&peek_val);
	TEST_ASSERT(st == EC_OK && (*peek_val)->id == 10);
	q.free_fn(&q);
	EC_FREE(peek_val);
	printf("pass: test_queue_recp_enqueue\n\n");
}

void
test_queue_recp_peek(void* mp)
{
	stamp();
	printf("test_queue_recp_peek\n\n");
	ec_queue_recp_t q;
	ec_status_t st = ec_queue_recp_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	rec_t rec_a = { 42 };
	rec_t* a = &rec_a;
	st = q.enqueue(&q, &a);
	TEST_ASSERT(st == EC_OK);
	rec_t** peek_val = malloc(sizeof(rec_t*));
	TEST_ASSERT(peek_val != NULL);
	st = q.peek(&q, (void**)&peek_val);
	TEST_ASSERT(st == EC_OK && (*peek_val)->id == 42);
	q.free_fn(&q);
	EC_FREE(peek_val);
	printf("pass: test_queue_recp_peek\n\n");
}

void
test_queue_recp_dequeue(void* mp)
{
	stamp();
	printf("test_queue_recp_dequeue\n\n");
	ec_queue_recp_t q;
	ec_status_t st = ec_queue_recp_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	rec_t rec_a = { 10 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 20 };
	rec_t* b = &rec_b;
	rec_t rec_c = { 30 };
	rec_t* c = &rec_c;
	st = q.enqueue(&q, &a);
	TEST_ASSERT(st == EC_OK);
	st = q.enqueue(&q, &b);
	TEST_ASSERT(st == EC_OK);
	ec_status_t st = q.enqueue(&q, &c);
	TEST_ASSERT(st == EC_OK);

	rec_t** dequeue_val = malloc(sizeof(rec_t*));
	TEST_ASSERT(dequeue_val != NULL);
	st = q.dequeue(&q, (void**)&dequeue_val);
	TEST_ASSERT(st == EC_OK && (*dequeue_val)->id == 10 && q.size_fn(&q) == 2);

	st = q.dequeue(&q, (void**)&dequeue_val);
	TEST_ASSERT(st == EC_OK && (*dequeue_val)->id == 20 && q.size_fn(&q) == 1);

	st = q.dequeue(&q, (void**)&dequeue_val);
	TEST_ASSERT(st == EC_OK && (*dequeue_val)->id == 30 && q.size_fn(&q) == 0);

	st = q.dequeue(&q, (void**)&dequeue_val);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);

	q.free_fn(&q);
	EC_FREE(dequeue_val);
	printf("pass: test_queue_recp_dequeue\n\n");
}

void
test_queue_recp_size(void* mp)
{
	stamp();
	printf("test_queue_recp_size\n\n");
	ec_queue_recp_t q;
	ec_status_t st = ec_queue_recp_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	TEST_ASSERT(q.size_fn(&q) == 0);
	rec_t rec_a = { 1 };
	rec_t *a = &rec_a, b = 2;
	q.enqueue(&q, &a);
	TEST_ASSERT(q.size_fn(&q) == 1);
	q.enqueue(&q, &b);
	TEST_ASSERT(q.size_fn(&q) == 2);
	q.free_fn(&q);
	printf("pass: test_queue_recp_size\n\n");
}

void
test_queue_recp_free(void* mp)
{
	stamp();
	printf("test_queue_recp_free\n\n");
	ec_queue_recp_t q;
	ec_status_t st = ec_queue_recp_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	rec_t rec_a = { 5 };
	rec_t *a = &rec_a, b = 6;
	q.enqueue(&q, &a);
	q.enqueue(&q, &b);
	q.free_fn(&q);
	TEST_ASSERT(q.size_fn(&q) == 0);
	printf("pass: test_queue_recp_free\n\n");
}

// Iterator Test for ec_queue_recp
void
test_queue_recp_iterators(void* mp)
{
	stamp();
	printf("test_queue_recp_iterators\n\n");
	ec_queue_recp_t q;
	ec_status_t st = ec_queue_recp_init(&q, mp);
	TEST_ASSERT(st == EC_OK);

	rec_t rec_v1 = { 1 };
	rec_t* v1 = &rec_v1;
	rec_t rec_v2 = { 2 };
	rec_t* v2 = &rec_v2;
	rec_t rec_v3 = { 3 };
	rec_t* v3 = &rec_v3;
	q.enqueue(&q, &v1);
	q.enqueue(&q, &v2);
	q.enqueue(&q, &v3);

	ec_queue_recp_iter_t it;
	q.iter_init(&it, &q);
	rec_t** out = malloc(sizeof(rec_t*));
	TEST_ASSERT(out != NULL);
	rec_t recs_exp[] = { { 1 }, { 2 }, { 3 } };
	rec_t* expected[] = { &recs_exp[0], &recs_exp[1], &recs_exp[2] };
	int idx = 0;

	while (q.iter_next(&it, (void**)&out) == EC_OK)
		{
			TEST_ASSERT((*out)->id == expected[idx]->id);
			idx++;
		}
	TEST_ASSERT(idx == 3);

	// Test iterator replacement
	q.iter_init(&it, &q);
	q.iter_next(&it, (void**)&out); // first element (1)
	rec_t rec_new = { 99 };
	rec_t* new_val = &rec_new;
	st = q.iter_replace(&it, &new_val, NULL);
	TEST_ASSERT(st == EC_OK);

	rec_t** peek_val = malloc(sizeof(rec_t*));
	TEST_ASSERT(peek_val != NULL);
	q.peek(&q, (void**)&peek_val);
	TEST_ASSERT((*peek_val)->id == 99);

	q.free_fn(&q);
	EC_FREE(peek_val);
	EC_FREE(out);
	printf("pass: test_queue_recp_iterators\n\n");
}

// Zip Iterator Test for ec_queue_recp
void
test_queue_recp_zip_iterators(void* mp)
{
	stamp();
	printf("test_queue_recp_zip_iterators\n\n");
	ec_queue_recp_t q1, q2;
	ec_status_t st = ec_queue_recp_init(&q1, mp);
	TEST_ASSERT(st == EC_OK);
	st = ec_queue_recp_init(&q2, mp);
	TEST_ASSERT(st == EC_OK);

	rec_t rec_a = { 10 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 20 };
	rec_t* b = &rec_b;
	rec_t rec_c = { 30 };
	rec_t* c = &rec_c;
	rec_t rec_d = { 100 };
	rec_t* d = &rec_d;
	rec_t rec_e = { 200 };
	rec_t* e = &rec_e;
	rec_t rec_f = { 300 };
	rec_t* f = &rec_f;
	q1.enqueue(&q1, &a);
	q1.enqueue(&q1, &b);
	q1.enqueue(&q1, &c);
	q2.enqueue(&q2, &d);
	q2.enqueue(&q2, &e);
	q2.enqueue(&q2, &f);

	ec_queue_recp_zip_iter_t zit;
	q1.zip_iter_init(&zit, &q1, &q2);
	rec_t** out1 = malloc(sizeof(rec_t*));
	rec_t** out2 = malloc(sizeof(rec_t*));
	TEST_ASSERT(out1 != NULL && out2 != NULL);
	rec_t recs_exp1[] = { { 10 }, { 20 }, { 30 } };
	rec_t* expected1[] = { &recs_exp1[0], &recs_exp1[1], &recs_exp1[2] };
	rec_t recs_exp2[] = { { 100 }, { 200 }, { 300 } };
	rec_t* expected2[] = { &recs_exp2[0], &recs_exp2[1], &recs_exp2[2] };
	int count = 0;

	while (q1.zip_iter_next(&zit, (void**)&out1, (void**)&out2) == EC_OK)
		{
			TEST_ASSERT((*out1)->id == expected1[count]->id);
			TEST_ASSERT((*out2)->id == expected2[count]->id);
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
	printf("pass: test_queue_recp_zip_iterators\n\n");
}

void
test_queue_recp_init_neg_null(void* mp)
{
	stamp();
	printf("test_queue_recp_init_neg_null
\n");
	ec_status_t st = ec_queue_recp_init(NULL, mp);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	printf("pass: test_queue_recp_init_neg_null
\n");
}

void
test_queue_recp_enqueue_neg_null(void* mp)
{
	stamp();
	printf("test_queue_recp_enqueue_neg_null
\n");
	ec_queue_recp_t q;
	ec_status_t st = ec_queue_recp_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	rec_t rec_a = {10};
	rec_t* a = &rec_a;
	st = q.enqueue(NULL, &a);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	q.free_fn(&q);
	printf("pass: test_queue_recp_enqueue_neg_null
\n");
}

void
test_queue_recp_peek_neg_empty(void* mp)
{
	stamp();
	printf("test_queue_recp_peek_neg_empty
\n");
	ec_queue_recp_t q;
	ec_status_t st = ec_queue_recp_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	rec_t** peek_val = malloc(sizeof(rec_t*));
	TEST_ASSERT(peek_val != NULL);
	st = q.peek(&q, (void**)&peek_val);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);
	q.free_fn(&q);
	EC_FREE(peek_val);
	printf("pass: test_queue_recp_peek_neg_empty
\n");
}

void
test_queue_recp_peek_neg_null(void* mp)
{
	stamp();
	printf("test_queue_recp_peek_neg_null
\n");
	ec_queue_recp_t q;
	ec_status_t st = ec_queue_recp_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	rec_t rec_a = {10};
	rec_t* a = &rec_a;
	q.enqueue(&q, &a);
	st = q.peek(NULL, NULL);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	q.free_fn(&q);
	printf("pass: test_queue_recp_peek_neg_null
\n");
}

void
test_queue_recp_dequeue_neg_null(void* mp)
{
	stamp();
	printf("test_queue_recp_dequeue_neg_null
\n");
	rec_t** out = malloc(sizeof(rec_t*));
	TEST_ASSERT(out != NULL);
	ec_status_t ec_status_t st = ec_queue_recp_dequeue(NULL, (void**)&out);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	EC_FREE(out);
	printf("pass: test_queue_recp_dequeue_neg_null
\n");
}

void
test_queue_recp_free_neg_null(void* mp)
{
	stamp();
	printf("test_queue_recp_free_neg_null
\n");
	ec_status_t st = ec_queue_recp_free_fn(NULL);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	printf("pass: test_queue_recp_free_neg_null
\n");
}

void
test_queue_recp_iter_replace_neg_no_prev(void* mp)
{
	stamp();
	printf("test_queue_recp_iter_replace_neg_no_prev
\n");
	ec_queue_recp_t q;
	ec_status_t st = ec_queue_recp_init(&q, mp);
	TEST_ASSERT(st == EC_OK);
	ec_queue_recp_iter_t it;
	q.iter_init(&it, &q);
	rec_t rec_new = {99};
	rec_t* new_val = &rec_new;
	st = q.iter_replace(&it, &new_val, NULL);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	q.free_fn(&q);
	printf("pass: test_queue_recp_iter_replace_neg_no_prev
\n");
}

void
test_queue_recp_iter_replace_neg_null(void* mp)
{
	stamp();
	printf("test_queue_recp_iter_replace_neg_null
\n");
	rec_t rec_new = {99};
	rec_t* new_val = &rec_new;
	ec_status_t st = ec_queue_recp_iter_replace(NULL, &new_val, NULL);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	printf("pass: test_queue_recp_iter_replace_neg_null
\n");
}

void
test_queue_recp_zip_iter_next_neg_mismatch(void* mp)
{
	stamp();
	printf("test_queue_recp_zip_iter_next_neg_mismatch
\n");
	ec_queue_recp_t q1, q2;
	ec_status_t st = ec_queue_recp_init(&q1, mp);
	TEST_ASSERT(st == EC_OK);
	st = ec_queue_recp_init(&q2, mp);
	TEST_ASSERT(st == EC_OK);
	rec_t rec_a = {10};
	rec_t* a = &rec_a;
	rec_t rec_b = {20};
	rec_t* b = &rec_b;
	q1.enqueue(&q1, &a);
	q1.enqueue(&q1, &b);
	rec_t rec_d = {100};
	rec_t* d = &rec_d;
	q2.enqueue(&q2, &d);
	ec_queue_recp_zip_iter_t zit;
	q1.zip_iter_init(&zit, &q1, &q2);
	rec_t** out1 = malloc(sizeof(rec_t*));
	rec_t** out2 = malloc(sizeof(rec_t*));
	TEST_ASSERT(out1 != NULL && out2 != NULL);
	ec_status_t st = q1.zip_iter_next(&zit, (void**)&out1, (void**)&out2);
	TEST_ASSERT(st == EC_OK);
	st = q1.zip_iter_next(&zit, (void**)&out1, (void**)&out2);
	TEST_ASSERT(st == EC_ITER_END);
	q1.free_fn(&q1);
	q2.free_fn(&q2);
	EC_FREE(out1);
	EC_FREE(out2);
	printf("pass: test_queue_recp_zip_iter_next_neg_mismatch
\n");
}

void
test_queue_recp_zip_iter_next_neg_null(void* mp)
{
	stamp();
	printf("test_queue_recp_zip_iter_next_neg_null
\n");
	ec_status_t st = ec_queue_recp_zip_iter_next(NULL, NULL, NULL);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	printf("pass: test_queue_recp_zip_iter_next_neg_null
\n");
}

int
main(void)
{
	printf("Starting ec_queue_recp tests...\n\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_queue_recp_init_neg_null(mp);
	test_queue_recp_enqueue(mp);
	test_queue_recp_enqueue_neg_null(mp);
	test_queue_recp_peek(mp);
	test_queue_recp_peek_neg_empty(mp);
	test_queue_recp_peek_neg_null(mp);
	test_queue_recp_dequeue(mp);
	test_queue_recp_dequeue_neg_null(mp);
	test_queue_recp_size(mp);
	test_queue_recp_free(mp);
	test_queue_recp_free_neg_null(mp);
	test_queue_recp_iterators(mp);
	test_queue_recp_iter_replace_neg_no_prev(mp);
	test_queue_recp_iter_replace_neg_null(mp);
	test_queue_recp_zip_iterators(mp);
	test_queue_recp_zip_iter_next_neg_mismatch(mp);
	test_queue_recp_zip_iter_next_neg_null(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif

	printf("All ec_queue_recp tests passed.\n\n");
	return 0;
}
