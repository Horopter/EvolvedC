#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../../src/ec_stack.h"
#include <assert.h>
#include <stdbool.h>
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

// Utility: prlong a stack trace (if available)
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

// Utility: prlong timestamp
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

//
// Test functions for ec_stack_long (a stack of long)
//

void
test_stack_long_push(void* mp)
{
	stamp();
	printf("test_stack_long_push\n");
	ec_stack_long_t s;
	ec_status_t st = ec_stack_long_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	long a = 10L, b = 20L;
	st = s.push(&s, &a);
	TEST_ASSERT(st == EC_OK && s.size_fn(&s) == 1);
	st = s.push(&s, &b);
	TEST_ASSERT(st == EC_OK && s.size_fn(&s) == 2);
	long* peek_val = malloc(sizeof(long));
	TEST_ASSERT(peek_val != NULL);
	st = s.peek(&s, (void**)&peek_val);
	TEST_ASSERT(st == EC_OK && *peek_val == 20L);
	s.free_fn(&s);

	EC_FREE(peek_val);
	printf("pass: test_stack_long_push\n");
}

void
test_stack_long_peek(void* mp)
{
	stamp();
	printf("test_stack_long_peek\n");
	ec_stack_long_t s;
	ec_status_t st = ec_stack_long_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	long a = 42;
	st = s.push(&s, &a);
	TEST_ASSERT(st == EC_OK);
	long* peek_val = malloc(sizeof(long));
	TEST_ASSERT(peek_val != NULL);
	st = s.peek(&s, (void**)&peek_val);
	TEST_ASSERT(st == EC_OK && *peek_val == 42);
	s.free_fn(&s);

	EC_FREE(peek_val);
	printf("pass: test_stack_long_peek\n");
}

void
test_stack_long_pop(void* mp)
{
	stamp();
	printf("test_stack_long_pop\n");
	ec_stack_long_t s;
	ec_status_t st = ec_stack_long_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	long a = 10L, b = 20L, c = 30L;
	st = s.push(&s, &a);
	TEST_ASSERT(st == EC_OK);
	st = s.push(&s, &b);
	TEST_ASSERT(st == EC_OK);
	st = s.push(&s, &c);
	TEST_ASSERT(st == EC_OK);
	// Expected LIFO order: top = c, then b, then a.
	// Allocate memory for the popped value before calling pop.
	long* pop_val = malloc(sizeof(long));
	TEST_ASSERT(pop_val != NULL);

	st = s.pop(&s, (void**)&pop_val);
	TEST_ASSERT(st == EC_OK && *pop_val == 30L && s.size_fn(&s) == 2);

	st = s.pop(&s, (void**)&pop_val);
	TEST_ASSERT(st == EC_OK && *pop_val == 20L && s.size_fn(&s) == 1);

	st = s.pop(&s, (void**)&pop_val);
	TEST_ASSERT(st == EC_OK && *pop_val == 10L && s.size_fn(&s) == 0);

	st = s.pop(&s, (void**)&pop_val);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);
	s.free_fn(&s);

	EC_FREE(pop_val);
	printf("pass: test_stack_long_pop\n");
}

void
test_stack_long_size(void* mp)
{
	stamp();
	printf("test_stack_long_size\n");
	ec_stack_long_t s;
	ec_status_t st = ec_stack_long_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	TEST_ASSERT(s.size_fn(&s) == 0);
	long a = 1, b = 2;
	s.push(&s, &a);
	TEST_ASSERT(s.size_fn(&s) == 1);
	s.push(&s, &b);
	TEST_ASSERT(s.size_fn(&s) == 2);
	s.free_fn(&s);
	printf("pass: test_stack_long_size\n");
}

void
test_stack_long_iterators(void* mp)
{
	stamp();
	printf("test_stack_long_iterators\n");
	ec_stack_long_t s;
	ec_status_t st = ec_stack_long_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	// Push values: 1, 2, 3 (stack top becomes 3)
	long v1 = 1, v2 = 2, v3 = 3;
	s.push(&s, &v1);
	s.push(&s, &v2);
	s.push(&s, &v3);
	ec_stack_long_iter_t it;
	s.iter_init(&it, &s);
	void* out = NULL;
	long expected[] = { 30L, 20L, 10L };
	long idx = 0;
	while (s.iter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(*(long*)out == expected[idx]);
			idx++;
		}
	TEST_ASSERT(idx == 3);
	// Test iterator replacement: replace the element returned in the first iteration.
	s.iter_init(&it, &s);
	s.iter_next(&it, &out); // returns top element (3)
	long new_val = 99;
	st = s.iter_replace(&it, &new_val, NULL);
	TEST_ASSERT(st == EC_OK);
	long* peek_val = malloc(sizeof(long));
	TEST_ASSERT(peek_val != NULL);
	s.peek(&s, (void**)&peek_val);
	TEST_ASSERT(*(long*)peek_val == 99);
	s.free_fn(&s);

	EC_FREE(peek_val);
	printf("pass: test_stack_long_iterators\n");
}

void
test_stack_long_zip_iterators(void* mp)
{
	stamp();
	printf("test_stack_long_zip_iterators\n");
	ec_stack_long_t s1, s2;
	ec_status_t st = ec_stack_long_init(&s1, mp);
	TEST_ASSERT(st == EC_OK);
	st = ec_stack_long_init(&s2, mp);
	TEST_ASSERT(st == EC_OK);
	// For s1: push 10L, 20L, 30L (top becomes 30L)
	// For s2: push 100, 200, 300 (top becomes 300)
	long a = 10L, b = 20L, c = 30L;
	long d = 100, e = 200, f = 300;
	s1.push(&s1, &a);
	s1.push(&s1, &b);
	s1.push(&s1, &c);
	s2.push(&s2, &d);
	s2.push(&s2, &e);
	s2.push(&s2, &f);
	ec_stack_long_zip_iter_t zit;
	s1.zip_iter_init(&zit, &s1, &s2);
	void *out1 = NULL, *out2 = NULL;
	long expected1[] = { 30L, 20L, 10L }; // s1 order (LIFO)
	long expected2[] = { 300, 200, 100 }; // s2 order (LIFO)
	long count = 0;
	while (s1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			TEST_ASSERT(*(long*)out1 == expected1[count]);
			TEST_ASSERT(*(long*)out2 == expected2[count]);
			count++;
		}
	TEST_ASSERT(count == 3);
	// Test zip iterator replace: expected to return not supported.
	st = s1.zip_iter_replace(&zit, &a, &d, NULL, NULL);
	TEST_ASSERT(st == EC_ERR_NOT_SUPPORTED);
	s1.free_fn(&s1);
	s2.free_fn(&s2);
	printf("pass: test_stack_long_zip_iterators\n");
}

void
test_stack_long_free(void* mp)
{
	stamp();
	printf("test_stack_long_free\n");
	ec_stack_long_t s;
	ec_status_t st = ec_stack_long_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	long a = 5L, b = 6L;
	s.push(&s, &a);
	s.push(&s, &b);
	s.free_fn(&s);
	TEST_ASSERT(s.size_fn(&s) == 0);
	printf("pass: test_stack_long_free\n");
}

void
test_stack_long_struct_size(void* mp)
{
	stamp();
	printf("test_stack_long_struct_size\n");
	ec_stack_long_t s;
	ec_status_t st = ec_stack_long_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	size_t sz = s.struct_size(&s);
	TEST_ASSERT(sz == sizeof(ec_stack_long_t));
	s.free_fn(&s);
	printf("pass: test_stack_long_struct_size\n");
}

void
test_stack_long_empty_peek_pop_neg(void* mp)
{
	stamp();
	printf("test_stack_long_empty_peek_pop_neg\n");
	ec_stack_long_t s;
	ec_status_t st = ec_stack_long_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	long* val = malloc(sizeof(long));
	TEST_ASSERT(val != NULL);
	st = s.peek(&s, (void**)&val);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);
	st = s.pop(&s, (void**)&val);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);
	s.free_fn(&s);
	free(val);
	printf("pass: test_stack_long_empty_peek_pop_neg\n");
}

void
test_stack_long_invalid_args_neg(void* mp)
{
	stamp();
	printf("test_stack_long_invalid_args_neg\n");
	ec_stack_long_t s;
	long val = 42L;
	long* out = malloc(sizeof(long));
	TEST_ASSERT(out != NULL);

	TEST_ASSERT(ec_stack_long_init(NULL, mp) == EC_ERR_INVALID_ARG);
	ec_status_t st = ec_stack_long_init(&s, mp);
	TEST_ASSERT(st == EC_OK);

	TEST_ASSERT(s.push(NULL, &val) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.peek(NULL, (void**)&out) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.peek(&s, NULL) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.pop(NULL, (void**)&out) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.pop(&s, NULL) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.size_fn(NULL) == 0);
	TEST_ASSERT(s.struct_size(NULL) == 0);

	s.free_fn(&s);
	free(out);
	printf("pass: test_stack_long_invalid_args_neg\n");
}

int
main(void)
{
	printf("Starting ec_stack_long tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_stack_long_push(mp);
	test_stack_long_peek(mp);
	test_stack_long_pop(mp);
	test_stack_long_size(mp);
	test_stack_long_iterators(mp);
	test_stack_long_zip_iterators(mp);
	test_stack_long_free(mp);
	test_stack_long_struct_size(mp);
	test_stack_long_empty_peek_pop_neg(mp);
	test_stack_long_invalid_args_neg(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif

	printf("All ec_stack_long tests passed.\n");
	return 0;
}
