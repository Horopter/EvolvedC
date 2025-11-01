#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../../src/ec_heap.h"
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

// Using standard type from header

#define TEST_ASSERT(cond)                                                                    \
	do                                                                                       \
		{                                                                                    \
			if (!(cond))                                                                     \
				{                                                                            \
					fprintf(stderr, "[ASSERT FAIL] %s:%d: %s\n", __FILE__, __LINE__, #cond); \
					exit(1);                                                                 \
				}                                                                            \
		}                                                                                    \
	while (0)

static void
stamp(void)
{
	time_t t = time(NULL);
	struct tm* m = localtime(&t);
	if (m)
		{
			unsigned long long b[64];
			strftime(b, sizeof(b), "[%Y-%m-%d %H:%M:%S]", m);
			printf("%s ", b);
		}
	else
		{
			printf("[timeerr] ");
		}
}

static int
ull_comparator(const unsigned long long* a, const unsigned long long* b)
{
	return (*a > *b) - (*a < *b);
}

void
test_heap_insert(void* mp)
{
	stamp();
	printf("test_heap_insert\n");
	ec_heap_ull_t heap;
	ec_status_t st = ec_heap_ull_init(&heap, 10, ull_comparator, mp);
	TEST_ASSERT(st == EC_OK);

	unsigned long long values[] = { 5ULL, 3ULL, 8ULL, 1ULL, 7ULL };
	for (int i = 0; i < 5; i++)
		{
			st = heap.insert(&heap, values[i]);
			TEST_ASSERT(st == EC_OK);
		}

	TEST_ASSERT(heap.size_fn(&heap) == 5);
	heap.free_fn(&heap);
	printf("pass: test_heap_insert\n");
}

void
test_heap_extract(void* mp)
{
	stamp();
	printf("test_heap_extract\n");
	ec_heap_ull_t heap;
	ec_status_t st = ec_heap_ull_init(&heap, 10, ull_comparator, mp);
	TEST_ASSERT(st == EC_OK);

	unsigned long long values[] = { 5ULL, 3ULL, 8ULL, 1ULL, 7ULL };
	for (int i = 0; i < 5; i++)
		{
			heap.insert(&heap, values[i]);
		}

	unsigned long long top;
	heap.extract_top(&heap, &top);
	TEST_ASSERT(top == 1ULL);

	heap.extract_top(&heap, &top);
	TEST_ASSERT(top == 3ULL);

	heap.free_fn(&heap);
	printf("pass: test_heap_extract\n");
}

void
test_heap_peek(void* mp)
{
	stamp();
	printf("test_heap_peek\n");
	ec_heap_ull_t heap;
	ec_status_t st = ec_heap_ull_init(&heap, 10, ull_comparator, mp);
	TEST_ASSERT(st == EC_OK);

	unsigned long long value = 42ULL;
	heap.insert(&heap, value);

	unsigned long long top;
	heap.peek_top(&heap, &top);
	TEST_ASSERT(top == 42ULL);
	TEST_ASSERT(heap.size_fn(&heap) == 1);

	heap.free_fn(&heap);
	printf("pass: test_heap_peek\n");
}

void
test_heap_size(void* mp)
{
	stamp();
	printf("test_heap_size\n");
	ec_heap_ull_t heap;
	ec_status_t st = ec_heap_ull_init(&heap, 10, ull_comparator, mp);
	TEST_ASSERT(st == EC_OK);

	TEST_ASSERT(heap.size_fn(&heap) == 0);
	heap.insert(&heap, 42ULL);
	TEST_ASSERT(heap.size_fn(&heap) == 1);
	heap.insert(&heap, 20LL);
	TEST_ASSERT(heap.size_fn(&heap) == 2);

	heap.free_fn(&heap);
	printf("pass: test_heap_size\n");
}

void
test_heap_free(void* mp)
{
	stamp();
	printf("test_heap_free\n");
	ec_heap_ull_t heap;
	ec_status_t st = ec_heap_ull_init(&heap, 10, ull_comparator, mp);
	TEST_ASSERT(st == EC_OK);

	heap.insert(&heap, 42ULL);
	heap.insert(&heap, 20LL);

	heap.free_fn(&heap);
	TEST_ASSERT(heap.size_fn(&heap) == 0);
	printf("pass: test_heap_free\n");
}

void
test_heap_empty_extract_neg(void* mp)
{
	stamp();
	printf("test_heap_empty_extract_neg\n");
	ec_heap_ull_t heap;
	ec_status_t st = ec_heap_ull_init(&heap, 10, ull_comparator, mp);
	TEST_ASSERT(st == EC_OK);

	unsigned long long top;
	st = heap.extract_top(&heap, &top);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);

	heap.free_fn(&heap);
	printf("pass: test_heap_empty_extract_neg\n");
}

void
test_heap_empty_peek_neg(void* mp)
{
	stamp();
	printf("test_heap_empty_peek_neg\n");
	ec_heap_ull_t heap;
	ec_status_t st = ec_heap_ull_init(&heap, 10, ull_comparator, mp);
	TEST_ASSERT(st == EC_OK);

	unsigned long long top;
	st = heap.peek_top(&heap, &top);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);

	heap.free_fn(&heap);
	printf("pass: test_heap_empty_peek_neg\n");
}

void
test_heap_invalid_args_neg(void* mp)
{
	stamp();
	printf("test_heap_invalid_args_neg\n");
	ec_heap_ull_t heap;
	unsigned long long top;

	TEST_ASSERT(ec_heap_ull_init(NULL, 10, ull_comparator, mp) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(ec_heap_ull_init(&heap, 0, ull_comparator, mp) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(ec_heap_ull_init(&heap, 10, NULL, mp) == EC_ERR_INVALID_ARG);

	ec_status_t st = ec_heap_ull_init(&heap, 10, ull_comparator, mp);
	TEST_ASSERT(st == EC_OK);

	TEST_ASSERT(heap.insert(NULL, 42ULL) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(heap.extract_top(NULL, &top) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(heap.extract_top(&heap, NULL) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(heap.peek_top(NULL, &top) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(heap.peek_top(&heap, NULL) == EC_ERR_INVALID_ARG);

	heap.free_fn(&heap);
	printf("pass: test_heap_invalid_args_neg\n");
}

int
main(void)
{
	printf("Starting ec_heap_ull tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_heap_insert(mp);
	test_heap_extract(mp);
	test_heap_peek(mp);
	test_heap_size(mp);
	test_heap_free(mp);
	test_heap_empty_extract_neg(mp);
	test_heap_empty_peek_neg(mp);
	test_heap_invalid_args_neg(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif

	printf("All ec_heap_ull tests passed.\n");
	return 0;
}
