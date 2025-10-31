#include "../../src/ec_stack.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void
print_timestamp(void)
{
	time_t now = time(NULL);
	struct tm* local = localtime(&now);
	char buffer[64];
	if (local)
		{
			strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", local);
			printf("%s ", buffer);
		}
	else
		{
			printf("[timestamp error] ");
		}
}

void
test_stack_str_push(void* mp)
{
	print_timestamp();
	printf("test_stack_str_push\n");
	ec_stack_str_t s;
	ec_status_t st = ec_stack_str_init(&s, mp);
	assert(st == EC_OK);
	const char *a = "hello", *b = "world";
	st = s.push(&s, &a);
	assert(st == EC_OK && s.size_fn(&s) == 1);
	st = s.push(&s, &b);
	assert(st == EC_OK && s.size_fn(&s) == 2);
	const char** peek_val = malloc(sizeof(const char*));
	assert(peek_val != NULL);
	st = s.peek(&s, (void**)&peek_val);
	assert(st == EC_OK && strcmp(*peek_val, "world") == 0);
	s.free_fn(&s);
	free(peek_val);
	printf("pass: test_stack_str_push\n");
}

void
test_stack_str_peek(void* mp)
{
	print_timestamp();
	printf("test_stack_str_peek\n");
	ec_stack_str_t s;
	ec_status_t st = ec_stack_str_init(&s, mp);
	assert(st == EC_OK);
	const char* a = "test";
	st = s.push(&s, &a);
	assert(st == EC_OK);
	const char** peek_val = malloc(sizeof(const char*));
	assert(peek_val != NULL);
	st = s.peek(&s, (void**)&peek_val);
	assert(st == EC_OK && strcmp(*peek_val, "test") == 0);
	s.free_fn(&s);
	free(peek_val);
	printf("pass: test_stack_str_peek\n");
}

void
test_stack_str_pop(void* mp)
{
	print_timestamp();
	printf("test_stack_str_pop\n");
	ec_stack_str_t s;
	ec_status_t st = ec_stack_str_init(&s, mp);
	assert(st == EC_OK);
	const char *a = "first", *b = "second", *c = "third";
	st = s.push(&s, &a);
	assert(st == EC_OK);
	st = s.push(&s, &b);
	assert(st == EC_OK);
	st = s.push(&s, &c);
	assert(st == EC_OK);
	const char** pop_val = malloc(sizeof(const char*));
	assert(pop_val != NULL);
	st = s.pop(&s, (void**)&pop_val);
	assert(st == EC_OK && strcmp(*pop_val, "third") == 0 && s.size_fn(&s) == 2);
	st = s.pop(&s, (void**)&pop_val);
	assert(st == EC_OK && strcmp(*pop_val, "second") == 0 && s.size_fn(&s) == 1);
	st = s.pop(&s, (void**)&pop_val);
	assert(st == EC_OK && strcmp(*pop_val, "first") == 0 && s.size_fn(&s) == 0);
	st = s.pop(&s, (void**)&pop_val);
	assert(st == EC_ERR_OUT_OF_BOUNDS);
	s.free_fn(&s);
	free(pop_val);
	printf("pass: test_stack_str_pop\n");
}

void
test_stack_str_size(void* mp)
{
	print_timestamp();
	printf("test_stack_str_size\n");
	ec_stack_str_t s;
	ec_status_t st = ec_stack_str_init(&s, mp);
	assert(st == EC_OK);
	assert(s.size_fn(&s) == 0);
	const char *a = "one", *b = "two";
	s.push(&s, &a);
	assert(s.size_fn(&s) == 1);
	s.push(&s, &b);
	assert(s.size_fn(&s) == 2);
	s.free_fn(&s);
	printf("pass: test_stack_str_size\n");
}

void
test_stack_str_empty_peek_pop(void* mp)
{
	print_timestamp();
	printf("test_stack_str_empty_peek_pop\n");
	ec_stack_str_t s;
	ec_status_t st = ec_stack_str_init(&s, mp);
	assert(st == EC_OK);
	const char** val = malloc(sizeof(const char*));
	assert(val != NULL);
	st = s.peek(&s, (void**)&val);
	assert(st == EC_ERR_OUT_OF_BOUNDS);
	st = s.pop(&s, (void**)&val);
	assert(st == EC_ERR_OUT_OF_BOUNDS);
	s.free_fn(&s);
	free(val);
	printf("pass: test_stack_str_empty_peek_pop\n");
}

void
test_stack_str_iterators(void* mp)
{
	print_timestamp();
	printf("test_stack_str_iterators\n");
	ec_stack_str_t s;
	ec_status_t st = ec_stack_str_init(&s, mp);
	assert(st == EC_OK);
	const char *v1 = "a", *v2 = "b", *v3 = "c";
	s.push(&s, &v1);
	s.push(&s, &v2);
	s.push(&s, &v3);
	ec_stack_str_iter_t it;
	s.iter_init(&it, &s);
	void* out = NULL;
	const char* expected[] = { "c", "b", "a" };
	int idx = 0;
	while (s.iter_next(&it, &out) == EC_OK)
		{
			assert(strcmp(*(const char**)out, expected[idx]) == 0);
			idx++;
		}
	assert(idx == 3);
	s.iter_init(&it, &s);
	s.iter_next(&it, &out);
	const char* new_val = "z";
	st = s.iter_replace(&it, &new_val, NULL);
	assert(st == EC_OK);
	const char** peek_val = malloc(sizeof(const char*));
	assert(peek_val != NULL);
	s.peek(&s, (void**)&peek_val);
	assert(strcmp(*peek_val, "z") == 0);
	s.free_fn(&s);
	free(peek_val);
	printf("pass: test_stack_str_iterators\n");
}

void
test_stack_str_zip_iterators(void* mp)
{
	print_timestamp();
	printf("test_stack_str_zip_iterators\n");
	ec_stack_str_t s1, s2;
	ec_status_t st = ec_stack_str_init(&s1, mp);
	assert(st == EC_OK);
	st = ec_stack_str_init(&s2, mp);
	assert(st == EC_OK);
	const char *a = "x", *b = "y", *c = "z";
	const char *d = "1", *e = "2", *f = "3";
	s1.push(&s1, &a);
	s1.push(&s1, &b);
	s1.push(&s1, &c);
	s2.push(&s2, &d);
	s2.push(&s2, &e);
	s2.push(&s2, &f);
	ec_stack_str_zip_iter_t zit;
	s1.zip_iter_init(&zit, &s1, &s2);
	void *out1 = NULL, *out2 = NULL;
	const char* expected1[] = { "z", "y", "x" };
	const char* expected2[] = { "3", "2", "1" };
	int count = 0;
	while (s1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			assert(strcmp(*(const char**)out1, expected1[count]) == 0);
			assert(strcmp(*(const char**)out2, expected2[count]) == 0);
			count++;
		}
	assert(count == 3);
	st = s1.zip_iter_replace(&zit, &a, &d, NULL, NULL);
	assert(st == EC_ERR_NOT_SUPPORTED);
	s1.free_fn(&s1);
	s2.free_fn(&s2);
	printf("pass: test_stack_str_zip_iterators\n");
}

void
test_stack_str_free(void* mp)
{
	print_timestamp();
	printf("test_stack_str_free\n");
	ec_stack_str_t s;
	ec_status_t st = ec_stack_str_init(&s, mp);
	assert(st == EC_OK);
	const char *a = "alpha", *b = "beta";
	s.push(&s, &a);
	s.push(&s, &b);
	s.free_fn(&s);
	assert(s.size_fn(&s) == 0);
	printf("pass: test_stack_str_free\n");
}

void
test_stack_str_struct_size(void* mp)
{
	print_timestamp();
	printf("test_stack_str_struct_size\n");
	ec_stack_str_t s;
	ec_status_t st = ec_stack_str_init(&s, mp);
	assert(st == EC_OK);
	size_t sz = s.struct_size(&s);
	assert(sz == sizeof(ec_stack_str_t));
	s.free_fn(&s);
	printf("pass: test_stack_str_struct_size\n");
}

void
test_stack_str_invalid_args_neg(void* mp)
{
	print_timestamp();
	printf("test_stack_str_invalid_args_neg\n");
	ec_stack_str_t s;
	const char* val = "test";
	const char** out = malloc(sizeof(const char*));
	assert(out != NULL);

	assert(ec_stack_str_init(NULL, mp) == EC_ERR_INVALID_ARG);
	st = ec_stack_str_init(&s, mp);
	assert(st == EC_OK);

	assert(s.push(NULL, &val) == EC_ERR_INVALID_ARG);
	assert(s.peek(NULL, (void**)&out) == EC_ERR_INVALID_ARG);
	assert(s.peek(&s, NULL) == EC_ERR_INVALID_ARG);
	assert(s.pop(NULL, (void**)&out) == EC_ERR_INVALID_ARG);
	assert(s.pop(&s, NULL) == EC_ERR_INVALID_ARG);
	assert(s.size_fn(NULL) == 0);
	assert(s.struct_size(NULL) == 0);

	s.free_fn(&s);
	free(out);
	printf("pass: test_stack_str_invalid_args_neg\n");
}

int
main(void)
{
	printf("Starting ec_stack_str tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	assert(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_stack_str_push(mp);
	test_stack_str_peek(mp);
	test_stack_str_pop(mp);
	test_stack_str_size(mp);
	test_stack_str_empty_peek_pop(mp);
	test_stack_str_iterators(mp);
	test_stack_str_zip_iterators(mp);
	test_stack_str_free(mp);
	test_stack_str_struct_size(mp);
	test_stack_str_invalid_args_neg(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif

	printf("All ec_stack_str tests passed.\n");
	return 0;
}
