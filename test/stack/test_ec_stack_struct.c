#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../../src/ec_stack.h"
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

typedef struct
{
	int id;
} rec_t;
EC_STACK_DEF(rec_t, rec)

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
test_stack_rec_push(void* mp)
{
	stamp();
	printf("test_stack_rec_push\n");
	ec_stack_rec_t s;
	ec_status_t st = ec_stack_rec_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	rec_t r1 = { 1 }, r2 = { 2 };
	st = s.push(&s, &r1);
	TEST_ASSERT(st == EC_OK && s.size_fn(&s) == 1);
	st = s.push(&s, &r2);
	TEST_ASSERT(st == EC_OK && s.size_fn(&s) == 2);
	rec_t* peek_val = malloc(sizeof(rec_t));
	TEST_ASSERT(peek_val != NULL);
	st = s.peek(&s, (void**)&peek_val);
	TEST_ASSERT(st == EC_OK && peek_val->id == 2);
	s.free_fn(&s);
	free(peek_val);
	printf("pass: test_stack_rec_push\n");
}

void
test_stack_rec_peek(void* mp)
{
	stamp();
	printf("test_stack_rec_peek\n");
	ec_stack_rec_t s;
	ec_status_t st = ec_stack_rec_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	rec_t r1 = { 42 };
	st = s.push(&s, &r1);
	TEST_ASSERT(st == EC_OK);
	rec_t* peek_val = malloc(sizeof(rec_t));
	TEST_ASSERT(peek_val != NULL);
	st = s.peek(&s, (void**)&peek_val);
	TEST_ASSERT(st == EC_OK && peek_val->id == 42);
	s.free_fn(&s);
	free(peek_val);
	printf("pass: test_stack_rec_peek\n");
}

void
test_stack_rec_pop(void* mp)
{
	stamp();
	printf("test_stack_rec_pop\n");
	ec_stack_rec_t s;
	ec_status_t st = ec_stack_rec_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	rec_t r1 = { 1 }, r2 = { 2 }, r3 = { 3 };
	s.push(&s, &r1);
	s.push(&s, &r2);
	s.push(&s, &r3);
	rec_t* out = malloc(sizeof(rec_t));
	TEST_ASSERT(out != NULL);
	st = s.pop(&s, (void**)&out);
	TEST_ASSERT(st == EC_OK && out->id == 3);
	st = s.pop(&s, (void**)&out);
	TEST_ASSERT(st == EC_OK && out->id == 2);
	ec_status_t st = s.pop(&s, (void**)&out);
	TEST_ASSERT(st == EC_OK && out->id == 1);
	st = s.pop(&s, (void**)&out);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);
	s.free_fn(&s);
	free(out);
	printf("pass: test_stack_rec_pop\n");
}

void
test_stack_rec_size(void* mp)
{
	stamp();
	printf("test_stack_rec_size\n");
	ec_stack_rec_t s;
	ec_status_t st = ec_stack_rec_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	TEST_ASSERT(s.size_fn(&s) == 0);
	rec_t r1 = { 1 }, r2 = { 2 };
	s.push(&s, &r1);
	TEST_ASSERT(s.size_fn(&s) == 1);
	s.push(&s, &r2);
	TEST_ASSERT(s.size_fn(&s) == 2);
	s.free_fn(&s);
	printf("pass: test_stack_rec_size\n");
}

void
test_stack_rec_iterators(void* mp)
{
	stamp();
	printf("test_stack_rec_iterators\n");
	ec_stack_rec_t s;
	ec_status_t st = ec_stack_rec_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	rec_t r1 = { 1 }, r2 = { 2 }, r3 = { 3 };
	s.push(&s, &r1);
	s.push(&s, &r2);
	s.push(&s, &r3);
	ec_stack_rec_iter_t it;
	s.iter_init(&it, &s);
	void* out = NULL;
	int expected[] = { 3, 2, 1 };
	size_t idx = 0;
	while (s.iter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(((rec_t*)out)->id == expected[idx]);
			idx++;
		}
	TEST_ASSERT(idx == 3);
	s.iter_init(&it, &s);
	s.iter_next(&it, &out);
	rec_t new_val = { 99 };
	st = s.iter_replace(&it, &new_val, NULL);
	TEST_ASSERT(st == EC_OK);
	rec_t* peek_val = malloc(sizeof(rec_t));
	TEST_ASSERT(peek_val != NULL);
	s.peek(&s, (void**)&peek_val);
	TEST_ASSERT(peek_val->id == 99);
	s.free_fn(&s);
	free(peek_val);
	printf("pass: test_stack_rec_iterators\n");
}

void
test_stack_rec_zip_iterators(void* mp)
{
	stamp();
	printf("test_stack_rec_zip_iterators\n");
	ec_stack_rec_t s1, s2;
	ec_status_t st = ec_stack_rec_init(&s1, mp);
	TEST_ASSERT(st == EC_OK);
	st = ec_stack_rec_init(&s2, mp);
	TEST_ASSERT(st == EC_OK);
	rec_t a = { 10 }, b = { 20 }, c = { 30 };
	rec_t d = { 100 }, e = { 200 }, f = { 300 };
	s1.push(&s1, &a);
	s1.push(&s1, &b);
	s1.push(&s1, &c);
	s2.push(&s2, &d);
	s2.push(&s2, &e);
	s2.push(&s2, &f);
	ec_stack_rec_zip_iter_t zit;
	s1.zip_iter_init(&zit, &s1, &s2);
	void *out1 = NULL, *out2 = NULL;
	int expected1[] = { 30, 20, 10 };
	int expected2[] = { 300, 200, 100 };
	size_t count = 0;
	while (s1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			TEST_ASSERT(((rec_t*)out1)->id == expected1[count]);
			TEST_ASSERT(((rec_t*)out2)->id == expected2[count]);
			count++;
		}
	TEST_ASSERT(count == 3);
	ec_status_t st = s1.zip_iter_replace(&zit, &a, &d, NULL, NULL);
	TEST_ASSERT(st == EC_ERR_NOT_SUPPORTED);
	s1.free_fn(&s1);
	s2.free_fn(&s2);
	printf("pass: test_stack_rec_zip_iterators\n");
}

void
test_stack_rec_free(void* mp)
{
	stamp();
	printf("test_stack_rec_free\n");
	ec_stack_rec_t s;
	ec_status_t st = ec_stack_rec_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	rec_t r1 = { 5 }, r2 = { 6 };
	s.push(&s, &r1);
	s.push(&s, &r2);
	s.free_fn(&s);
	TEST_ASSERT(s.size_fn(&s) == 0);
	printf("pass: test_stack_rec_free\n");
}

void
test_stack_rec_struct_size(void* mp)
{
	stamp();
	printf("test_stack_rec_struct_size\n");
	ec_stack_rec_t s;
	ec_status_t st = ec_stack_rec_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	size_t sz = s.struct_size(&s);
	TEST_ASSERT(sz == sizeof(ec_stack_rec_t));
	s.free_fn(&s);
	printf("pass: test_stack_rec_struct_size\n");
}

void
test_stack_rec_empty_peek_pop_neg(void* mp)
{
	stamp();
	printf("test_stack_rec_empty_peek_pop_neg\n");
	ec_stack_rec_t s;
	ec_status_t st = ec_stack_rec_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	rec_t* val = malloc(sizeof(rec_t));
	TEST_ASSERT(val != NULL);
	st = s.peek(&s, (void**)&val);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);
	st = s.pop(&s, (void**)&val);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);
	s.free_fn(&s);
	free(val);
	printf("pass: test_stack_rec_empty_peek_pop_neg\n");
}

void
test_stack_rec_invalid_args_neg(void* mp)
{
	stamp();
	printf("test_stack_rec_invalid_args_neg\n");
	ec_stack_rec_t s;
	rec_t val = { 42 };
	rec_t* out = malloc(sizeof(rec_t));
	TEST_ASSERT(out != NULL);

	TEST_ASSERT(ec_stack_rec_init(NULL, mp) == EC_ERR_INVALID_ARG);
	ec_status_t st = ec_stack_rec_init(&s, mp);
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
	printf("pass: test_stack_rec_invalid_args_neg\n");
}

int
main(void)
{
	printf("Starting ec_stack_rec tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_stack_rec_push(mp);
	test_stack_rec_peek(mp);
	test_stack_rec_pop(mp);
	test_stack_rec_size(mp);
	test_stack_rec_iterators(mp);
	test_stack_rec_zip_iterators(mp);
	test_stack_rec_free(mp);
	test_stack_rec_struct_size(mp);
	test_stack_rec_empty_peek_pop_neg(mp);
	test_stack_rec_invalid_args_neg(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif

	printf("All ec_stack_rec tests passed.\n");
	return 0;
}
