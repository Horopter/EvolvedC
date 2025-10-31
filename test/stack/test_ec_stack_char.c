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

EC_STACK_DEF(char, ch)

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
test_stack_char_push(void* mp)
{
	stamp();
	printf("test_stack_char_push\n");
	ec_stack_ch_t s;
	ec_status_t st = ec_stack_ch_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	char a = 'a', b = 'b';
	st = s.push(&s, &a);
	TEST_ASSERT(st == EC_OK && s.size_fn(&s) == 1);
	st = s.push(&s, &b);
	TEST_ASSERT(st == EC_OK && s.size_fn(&s) == 2);
	char* peek_val = malloc(sizeof(char));
	TEST_ASSERT(peek_val != NULL);
	st = s.peek(&s, (void**)&peek_val);
	TEST_ASSERT(st == EC_OK && *peek_val == 'b');
	s.free_fn(&s);
	free(peek_val);
	printf("pass: test_stack_char_push\n");
}

void
test_stack_char_peek(void* mp)
{
	stamp();
	printf("test_stack_char_peek\n");
	ec_stack_ch_t s;
	ec_status_t st = ec_stack_ch_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	char a = 'x';
	st = s.push(&s, &a);
	TEST_ASSERT(st == EC_OK);
	char* peek_val = malloc(sizeof(char));
	TEST_ASSERT(peek_val != NULL);
	st = s.peek(&s, (void**)&peek_val);
	TEST_ASSERT(st == EC_OK && *peek_val == 'x');
	s.free_fn(&s);
	free(peek_val);
	printf("pass: test_stack_char_peek\n");
}

void
test_stack_char_pop(void* mp)
{
	stamp();
	printf("test_stack_char_pop\n");
	ec_stack_ch_t s;
	ec_status_t st = ec_stack_ch_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	char a = 'x', b = 'y', c = 'z';
	s.push(&s, &a);
	s.push(&s, &b);
	s.push(&s, &c);
	char* out = malloc(sizeof(char));
	TEST_ASSERT(out != NULL);
	st = s.pop(&s, (void**)&out);
	TEST_ASSERT(st == EC_OK && *out == 'z');
	st = s.pop(&s, (void**)&out);
	TEST_ASSERT(st == EC_OK && *out == 'y');
	ec_status_t st = s.pop(&s, (void**)&out);
	TEST_ASSERT(st == EC_OK && *out == 'x');
	st = s.pop(&s, (void**)&out);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);
	s.free_fn(&s);
	free(out);
	printf("pass: test_stack_char_pop\n");
}

void
test_stack_char_size(void* mp)
{
	stamp();
	printf("test_stack_char_size\n");
	ec_stack_ch_t s;
	ec_status_t st = ec_stack_ch_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	TEST_ASSERT(s.size_fn(&s) == 0);
	char a = 'a', b = 'b';
	s.push(&s, &a);
	TEST_ASSERT(s.size_fn(&s) == 1);
	s.push(&s, &b);
	TEST_ASSERT(s.size_fn(&s) == 2);
	s.free_fn(&s);
	printf("pass: test_stack_char_size\n");
}

void
test_stack_char_iterators(void* mp)
{
	stamp();
	printf("test_stack_char_iterators\n");
	ec_stack_ch_t s;
	ec_status_t st = ec_stack_ch_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	char v1 = 'a', v2 = 'b', v3 = 'c';
	s.push(&s, &v1);
	s.push(&s, &v2);
	s.push(&s, &v3);
	ec_stack_ch_iter_t it;
	s.iter_init(&it, &s);
	void* out = NULL;
	char expected[] = { 'c', 'b', 'a' };
	size_t idx = 0;
	while (s.iter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(*(char*)out == expected[idx]);
			idx++;
		}
	TEST_ASSERT(idx == 3);
	s.iter_init(&it, &s);
	s.iter_next(&it, &out);
	char new_val = 'z';
	st = s.iter_replace(&it, &new_val, NULL);
	TEST_ASSERT(st == EC_OK);
	char* peek_val = malloc(sizeof(char));
	TEST_ASSERT(peek_val != NULL);
	s.peek(&s, (void**)&peek_val);
	TEST_ASSERT(*peek_val == 'z');
	s.free_fn(&s);
	free(peek_val);
	printf("pass: test_stack_char_iterators\n");
}

void
test_stack_char_zip_iterators(void* mp)
{
	stamp();
	printf("test_stack_char_zip_iterators\n");
	ec_stack_ch_t s1, s2;
	ec_status_t st = ec_stack_ch_init(&s1, mp);
	TEST_ASSERT(st == EC_OK);
	st = ec_stack_ch_init(&s2, mp);
	TEST_ASSERT(st == EC_OK);
	char a = 'x', b = 'y', c = 'z';
	char d = '1', e = '2', f = '3';
	s1.push(&s1, &a);
	s1.push(&s1, &b);
	s1.push(&s1, &c);
	s2.push(&s2, &d);
	s2.push(&s2, &e);
	s2.push(&s2, &f);
	ec_stack_ch_zip_iter_t zit;
	s1.zip_iter_init(&zit, &s1, &s2);
	void *out1 = NULL, *out2 = NULL;
	char expected1[] = { 'z', 'y', 'x' };
	char expected2[] = { '3', '2', '1' };
	size_t count = 0;
	while (s1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			TEST_ASSERT(*(char*)out1 == expected1[count]);
			TEST_ASSERT(*(char*)out2 == expected2[count]);
			count++;
		}
	TEST_ASSERT(count == 3);
	ec_status_t st = s1.zip_iter_replace(&zit, &a, &d, NULL, NULL);
	TEST_ASSERT(st == EC_ERR_NOT_SUPPORTED);
	s1.free_fn(&s1);
	s2.free_fn(&s2);
	printf("pass: test_stack_char_zip_iterators\n");
}

void
test_stack_char_free(void* mp)
{
	stamp();
	printf("test_stack_char_free\n");
	ec_stack_ch_t s;
	ec_status_t st = ec_stack_ch_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	char a = 'a', b = 'b';
	s.push(&s, &a);
	s.push(&s, &b);
	s.free_fn(&s);
	TEST_ASSERT(s.size_fn(&s) == 0);
	printf("pass: test_stack_char_free\n");
}

void
test_stack_char_struct_size(void* mp)
{
	stamp();
	printf("test_stack_char_struct_size\n");
	ec_stack_ch_t s;
	ec_status_t st = ec_stack_ch_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	size_t sz = s.struct_size(&s);
	TEST_ASSERT(sz == sizeof(ec_stack_ch_t));
	s.free_fn(&s);
	printf("pass: test_stack_char_struct_size\n");
}

void
test_stack_char_empty_peek_pop_neg(void* mp)
{
	stamp();
	printf("test_stack_char_empty_peek_pop_neg\n");
	ec_stack_ch_t s;
	ec_status_t st = ec_stack_ch_init(&s, mp);
	TEST_ASSERT(st == EC_OK);
	char* val = malloc(sizeof(char));
	TEST_ASSERT(val != NULL);
	st = s.peek(&s, (void**)&val);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);
	st = s.pop(&s, (void**)&val);
	TEST_ASSERT(st == EC_ERR_OUT_OF_BOUNDS);
	s.free_fn(&s);
	free(val);
	printf("pass: test_stack_char_empty_peek_pop_neg\n");
}

void
test_stack_char_invalid_args_neg(void* mp)
{
	stamp();
	printf("test_stack_char_invalid_args_neg\n");
	ec_stack_ch_t s;
	char val = 'x';
	char* out = malloc(sizeof(char));
	TEST_ASSERT(out != NULL);

	TEST_ASSERT(ec_stack_ch_init(NULL, mp) == EC_ERR_INVALID_ARG);
	ec_status_t st = ec_stack_ch_init(&s, mp);
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
	printf("pass: test_stack_char_invalid_args_neg\n");
}

int
main(void)
{
	printf("Starting ec_stack_char tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_stack_char_push(mp);
	test_stack_char_peek(mp);
	test_stack_char_pop(mp);
	test_stack_char_size(mp);
	test_stack_char_iterators(mp);
	test_stack_char_zip_iterators(mp);
	test_stack_char_free(mp);
	test_stack_char_struct_size(mp);
	test_stack_char_empty_peek_pop_neg(mp);
	test_stack_char_invalid_args_neg(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif

	printf("All ec_stack_char tests passed.\n");
	return 0;
}
