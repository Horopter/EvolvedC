#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../../src/ec_heap.h"
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

typedef struct person_s
{
	int id;
	char name[16];
} person_t;

EC_HEAP_DEF(person_t, person)

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
			char b[64];
			strftime(b, sizeof(b), "[%Y-%m-%d %H:%M:%S]", m);
			printf("%s ", b);
		}
	else
		{
			printf("[timeerr] ");
		}
}

static int
person_comparator(const person_t* a, const person_t* b)
{
	return (a->id > b->id) - (a->id < b->id);
}

void
test_heap_insert(void* mp)
{
	stamp();
	printf("test_heap_insert\n");
	ec_heap_person_t heap;
	ec_status_t st = ec_heap_person_init(&heap, 10, person_comparator, mp);
	TEST_ASSERT(st == EC_OK);

	person_t values[] = { { 5, "eve" }, { 3, "bob" }, { 8, "hal" }, { 1, "alice" }, { 7, "gina" } };
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
	ec_heap_person_t heap;
	ec_status_t st = ec_heap_person_init(&heap, 10, person_comparator, mp);
	TEST_ASSERT(st == EC_OK);

	person_t values[] = { { 5, "eve" }, { 3, "bob" }, { 8, "hal" }, { 1, "alice" }, { 7, "gina" } };
	for (int i = 0; i < 5; i++)
		{
			heap.insert(&heap, values[i]);
		}

	person_t top;
	heap.extract_top(&heap, &top);
	TEST_ASSERT(top.id == 1 && strcmp(top.name, "alice") == 0);

	heap.extract_top(&heap, &top);
	TEST_ASSERT(top.id == 3 && strcmp(top.name, "bob") == 0);

	heap.free_fn(&heap);
	printf("pass: test_heap_extract\n");
}

void
test_heap_peek(void* mp)
{
	stamp();
	printf("test_heap_peek\n");
	ec_heap_person_t heap;
	ec_status_t st = ec_heap_person_init(&heap, 10, person_comparator, mp);
	TEST_ASSERT(st == EC_OK);

	person_t value = { 42, "test" };
	heap.insert(&heap, value);

	person_t top;
	heap.peek_top(&heap, &top);
	TEST_ASSERT(top.id == 42 && strcmp(top.name, "test") == 0);
	TEST_ASSERT(heap.size_fn(&heap) == 1);

	heap.free_fn(&heap);
	printf("pass: test_heap_peek\n");
}

void
test_heap_size(void* mp)
{
	stamp();
	printf("test_heap_size\n");
	ec_heap_person_t heap;
	ec_status_t st = ec_heap_person_init(&heap, 10, person_comparator, mp);
	TEST_ASSERT(st == EC_OK);

	person_t a = { 10, "first" }, b = { 20, "second" };
	TEST_ASSERT(heap.size_fn(&heap) == 0);
	heap.insert(&heap, a);
	TEST_ASSERT(heap.size_fn(&heap) == 1);
	heap.insert(&heap, b);
	TEST_ASSERT(heap.size_fn(&heap) == 2);

	heap.free_fn(&heap);
	printf("pass: test_heap_size\n");
}

void
test_heap_free(void* mp)
{
	stamp();
	printf("test_heap_free\n");
	ec_heap_person_t heap;
	ec_status_t st = ec_heap_person_init(&heap, 10, person_comparator, mp);
	TEST_ASSERT(st == EC_OK);

	person_t a = { 5, "one" }, b = { 15, "two" };
	heap.insert(&heap, a);
	heap.insert(&heap, b);

	heap.free_fn(&heap);
	TEST_ASSERT(heap.size_fn(&heap) == 0);
	printf("pass: test_heap_free\n");
}

void
test_heap_empty_extract_neg(void* mp)
{
	stamp();
	printf("test_heap_empty_extract_neg\n");
	ec_heap_person_t heap;
	ec_status_t st = ec_heap_person_init(&heap, 10, person_comparator, mp);
	TEST_ASSERT(st == EC_OK);

	person_t top;
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
	ec_heap_person_t heap;
	ec_status_t st = ec_heap_person_init(&heap, 10, person_comparator, mp);
	TEST_ASSERT(st == EC_OK);

	person_t top;
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
	ec_heap_person_t heap;
	person_t top;
	person_t x = { 5, "test" };

	TEST_ASSERT(ec_heap_person_init(NULL, 10, person_comparator, mp) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(ec_heap_person_init(&heap, 0, person_comparator, mp) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(ec_heap_person_init(&heap, 10, NULL, mp) == EC_ERR_INVALID_ARG);

	ec_status_t st = ec_heap_person_init(&heap, 10, person_comparator, mp);
	TEST_ASSERT(st == EC_OK);

	TEST_ASSERT(heap.insert(NULL, x) == EC_ERR_INVALID_ARG);
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
	printf("Starting ec_heap_person tests...\n");
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

	printf("All ec_heap_person tests passed.\n");
	return 0;
}
