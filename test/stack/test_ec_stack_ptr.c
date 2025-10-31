#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include <stdint.h>
#include "../../src/ec_stack.h"
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

void
test_stack_ptr_push(void* mp)
{
	stamp();
	printf("test_stack_ptr_push\n");
	ec_stack_ptr_t s;
	ec_status_t st = ec_stack_ptr_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	int a = 1, b = 2;
	void *p1 = &a, *p2 = &b;
	st = s.push(&s, &p1);
	TEST_ASSERT(st == EC_OK && s.size_fn(&s) == 1);
	st = s.push(&s, &p2);
	TEST_ASSERT(st == EC_OK && s.size_fn(&s) == 2);
	void** peek_val = malloc(sizeof(void*));
	TEST_ASSERT(peek_val != NULL);
	st = s.peek(&s, (void**)&peek_val);
	TEST_ASSERT(st == EC_OK && *peek_val == p2);
	s.free_fn(&s);
	free(peek_val);
	printf("pass: test_stack_ptr_push\n");
}

void
test_stack_ptr_peek(void* mp)
{
	stamp();
	printf("test_stack_ptr_peek\n");
	ec_stack_ptr_t s;
	ec_status_t st = ec_stack_ptr_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	int a = 42;
	void* p1 = &a;
	st = s.push(&s, &p1);
	TEST_ASSERT(st == EC_OK);
	void** peek_val = malloc(sizeof(void*));
	TEST_ASSERT(peek_val != NULL);
	st = s.peek(&s, (void**)&peek_val);
	TEST_ASSERT(st == EC_OK && *peek_val == p1);
	s.free_fn(&s);
	free(peek_val);
	printf("pass: test_stack_ptr_peek\n");
}

void
test_stack_ptr_pop(void* mp)
{
	stamp();
	printf("test_stack_ptr_pop\n");
	ec_stack_ptr_t s;
	ec_status_t st = ec_stack_ptr_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	int a = 1, b = 2, c = 3;
	void *p1 = &a, *p2 = &b, *p3 = &c;
	s.push(&s, &p1);
	s.push(&s, &p2);
	s.push(&s, &p3);
	void** out = malloc(sizeof(void*));
	TEST_ASSERT(out != NULL);
	st = s.pop(&s, (void**)&out);
	TEST_ASSERT(st == EC_OK && *out == p3);
	st = s.pop(&s, (void**)&out);
	TEST_ASSERT(st == EC_OK && *out == p2);
	ec_status_t st = s.pop(&s, (void**)&out);
	TEST_ASSERT(st == EC_OK && *out == p1);
	st = s.pop(&s, (void**)&out);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);
	s.free_fn(&s);
	free(out);
	printf("pass: test_stack_ptr_pop\n");
}

void
test_stack_ptr_size(void* mp)
{
	stamp();
	printf("test_stack_ptr_size\n");
	ec_stack_ptr_t s;
	ec_status_t st = ec_stack_ptr_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	TEST_ASSERT(s.size_fn(&s) == 0);
	int a = 1, b = 2;
	void *p1 = &a, *p2 = &b;
	s.push(&s, &p1);
	TEST_ASSERT(s.size_fn(&s) == 1);
	s.push(&s, &p2);
	TEST_ASSERT(s.size_fn(&s) == 2);
	s.free_fn(&s);
	printf("pass: test_stack_ptr_size\n");
}

void
test_stack_ptr_iterators(void* mp)
{
	stamp();
	printf("test_stack_ptr_iterators\n");
	ec_stack_ptr_t s;
	ec_status_t st = ec_stack_ptr_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	int v1 = 1, v2 = 2, v3 = 3;
	void *p1 = &v1, *p2 = &v2, *p3 = &v3;
	s.push(&s, &p1);
	s.push(&s, &p2);
	s.push(&s, &p3);
	ec_stack_ptr_iter_t it;
	s.iter_init(&it, &s);
	void* out = NULL;
	void* expected[] = { p3, p2, p1 };
	size_t idx = 0;
	while (s.iter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(*(void**)out == expected[idx]);
			idx++;
		}
	TEST_ASSERT(idx == 3);
	s.iter_init(&it, &s);
	s.iter_next(&it, &out);
	int new_val = 99;
	void* new_ptr = &new_val;
	st = s.iter_replace(&it, &new_ptr, NULL);
	TEST_ASSERT(st == EC_OK);
	void** peek_val = malloc(sizeof(void*));
	TEST_ASSERT(peek_val != NULL);
	s.peek(&s, (void**)&peek_val);
	TEST_ASSERT(*peek_val == new_ptr);
	s.free_fn(&s);
	free(peek_val);
	printf("pass: test_stack_ptr_iterators\n");
}

void
test_stack_ptr_zip_iterators(void* mp)
{
	stamp();
	printf("test_stack_ptr_zip_iterators\n");
	ec_stack_ptr_t s1, s2;
	ec_status_t st = ec_stack_ptr_init(&s1, mp);
	TEST_ASSERT(st == EC_OK);
	st = ec_stack_ptr_init(&s2, mp);
	TEST_ASSERT(st == EC_OK);
	int a = 10, b = 20, c = 30;
	int d = 100, e = 200, f = 300;
	void *p1 = &a, *p2 = &b, *p3 = &c;
	void *p4 = &d, *p5 = &e, *p6 = &f;
	s1.push(&s1, &p1);
	s1.push(&s1, &p2);
	s1.push(&s1, &p3);
	s2.push(&s2, &p4);
	s2.push(&s2, &p5);
	s2.push(&s2, &p6);
	ec_stack_ptr_zip_iter_t zit;
	s1.zip_iter_init(&zit, &s1, &s2);
	void *out1 = NULL, *out2 = NULL;
	void* expected1[] = { p3, p2, p1 };
	void* expected2[] = { p6, p5, p4 };
	size_t count = 0;
	while (s1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			TEST_ASSERT(*(void**)out1 == expected1[count]);
			TEST_ASSERT(*(void**)out2 == expected2[count]);
			count++;
		}
	TEST_ASSERT(count == 3);
	ec_status_t st = s1.zip_iter_replace(&zit, &p1, &p4, NULL, NULL);
	TEST_ASSERT(st == EC_ERR_NOT_SUPPORTED);
	s1.free_fn(&s1);
	s2.free_fn(&s2);
	printf("pass: test_stack_ptr_zip_iterators\n");
}

void
test_stack_ptr_free(void* mp)
{
	stamp();
	printf("test_stack_ptr_free\n");
	ec_stack_ptr_t s;
	ec_status_t st = ec_stack_ptr_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	int a = 5, b = 6;
	void *p1 = &a, *p2 = &b;
	s.push(&s, &p1);
	s.push(&s, &p2);
	s.free_fn(&s);
	TEST_ASSERT(s.size_fn(&s) == 0);
	printf("pass: test_stack_ptr_free\n");
}

void
test_stack_ptr_struct_size(void* mp)
{
	stamp();
	printf("test_stack_ptr_struct_size\n");
	ec_stack_ptr_t s;
	ec_status_t st = ec_stack_ptr_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	size_t sz = s.struct_size(&s);
	TEST_ASSERT(sz == sizeof(ec_stack_ptr_t));
	s.free_fn(&s);
	printf("pass: test_stack_ptr_struct_size\n");
}

void
test_stack_ptr_empty_peek_pop_neg(void* mp)
{
	stamp();
	printf("test_stack_ptr_empty_peek_pop_neg\n");
	ec_stack_ptr_t s;
	ec_status_t st = ec_stack_ptr_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	void** val = malloc(sizeof(void*));
	TEST_ASSERT(val != NULL);
	st = s.peek(&s, (void**)&val);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);
	st = s.pop(&s, (void**)&val);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);
	s.free_fn(&s);
	free(val);
	printf("pass: test_stack_ptr_empty_peek_pop_neg\n");
}

void
test_stack_ptr_invalid_args_neg(void* mp)
{
	stamp();
	printf("test_stack_ptr_invalid_args_neg\n");
	ec_stack_ptr_t s;
	int a = 42;
	void* val = &a;
	void** out = malloc(sizeof(void*));
	TEST_ASSERT(out != NULL);

	TEST_ASSERT(ec_stack_ptr_init(NULL, mp) == EC_ERR_INVALID_ARG);
	ec_status_t st = ec_stack_ptr_init(&s, mp);
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
	printf("pass: test_stack_ptr_invalid_args_neg\n");
}

int
main(void)
{
	printf("Starting ec_stack_ptr tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_stack_ptr_push(mp);
	test_stack_ptr_peek(mp);
	test_stack_ptr_pop(mp);
	test_stack_ptr_size(mp);
	test_stack_ptr_iterators(mp);
	test_stack_ptr_zip_iterators(mp);
	test_stack_ptr_free(mp);
	test_stack_ptr_struct_size(mp);
	test_stack_ptr_empty_peek_pop_neg(mp);
	test_stack_ptr_invalid_args_neg(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif

	printf("All ec_stack_ptr tests passed.\n");
	return 0;
}
