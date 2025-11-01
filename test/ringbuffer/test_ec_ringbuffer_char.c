#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../../src/ec_ringbuffer.h"
#include <stdbool.h>
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

/*
 * Test file for ec_ringbuffer with char type.
 */
static void
stktrc(void)
{
#ifdef __linux__
	void* arr[32];
	size_t n = backtrace(arr, 32);
	char** syms = backtrace_symbols(arr, n);
	if (syms)
		{
			fprintf(stderr, "---StackTrace---\n");
			for (size_t i = 0; i < n; i++)
				{
					fprintf(stderr, "%s\n", syms[i]);
				}
			free(syms);
		}
#else
	fprintf(stderr, "(NoStackTrace)\n");
#endif
}
#define TASSERT(COND)                                                               \
	do                                                                              \
		{                                                                           \
			if (!(COND))                                                            \
				{                                                                   \
					fprintf(stderr, "[FAIL]%s:%d:%s\n", __FILE__, __LINE__, #COND); \
					stktrc();                                                       \
					exit(1);                                                        \
				}                                                                   \
		}                                                                           \
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

static void
test_init(void* mp)
{
	stamp();
	printf("test_init\n");
	ec_ringbuf_ch_t rb;
	ec_status_t st = ec_ringbuf_ch_init(&rb, mp, 0, NULL);
	TASSERT(st == EC_OK && rb.size_fn(&rb) == 0 && rb.capacity_fn(&rb) == 0);
	rb.clear(&rb);
	printf("pass:test_init\n");
}

static void
test_push_pop(void* mp)
{
	stamp();
	printf("test_push_pop\n");
	ec_ringbuf_ch_t rb;
	ec_ringbuf_ch_init(&rb, mp, 2, NULL);
	char a = 'a', b = 'b', out;
	rb.push(&rb, &a);
	rb.push(&rb, &b);
	TASSERT(rb.size_fn(&rb) == 2);
	TASSERT(rb.pop(&rb, &out) == EC_OK && out == 'a');
	TASSERT(rb.pop(&rb, &out) == EC_OK && out == 'b');
	TASSERT(rb.pop(&rb, &out) == EC_ERR_NOT_FOUND);
	rb.clear(&rb);
	printf("pass:test_push_pop\n");
}

static void
test_front_back(void* mp)
{
	stamp();
	printf("test_front_back\n");
	ec_ringbuf_ch_t rb;
	ec_ringbuf_ch_init(&rb, mp, 3, NULL);
	char x1 = 'x', x2 = 'y', x3 = 'z', out;
	rb.push(&rb, &x1);
	rb.push(&rb, &x2);
	rb.push(&rb, &x3);
	TASSERT(rb.front(&rb, &out) == EC_OK && out == 'x');
	TASSERT(rb.back(&rb, &out) == EC_OK && out == 'z');
	rb.clear(&rb);
	TASSERT(rb.front(&rb, &out) == EC_ERR_NOT_FOUND);
	printf("pass:test_front_back\n");
}

static void
test_clear(void* mp)
{
	stamp();
	printf("test_clear\n");
	ec_ringbuf_ch_t rb;
	ec_ringbuf_ch_init(&rb, mp, 0, NULL);
	char x = 'X';
	rb.push(&rb, &x);
	TASSERT(rb.size_fn(&rb) == 1);
	rb.clear(&rb);
	TASSERT(rb.size_fn(&rb) == 0);
	rb.clear(&rb);
	TASSERT(rb.size_fn(&rb) == 0);
	printf("pass:test_clear\n");
}

static void
test_wrap_around(void* mp)
{
	stamp();
	printf("test_wrap_around\n");
	ec_ringbuf_ch_t rb;
	ec_ringbuf_ch_init(&rb, mp, 3, NULL);
	char a = 'a', b = 'b', c = 'c', d = 'd', out;
	rb.push(&rb, &a);
	rb.push(&rb, &b);
	rb.push(&rb, &c);
	rb.pop(&rb, &out);
	TASSERT(out == 'a');
	rb.push(&rb, &d);
	TASSERT(rb.pop(&rb, &out) == EC_OK && out == 'b');
	TASSERT(rb.pop(&rb, &out) == EC_OK && out == 'c');
	TASSERT(rb.pop(&rb, &out) == EC_OK && out == 'd');
	TASSERT(rb.pop(&rb, &out) == EC_ERR_NOT_FOUND);
	rb.clear(&rb);
	printf("pass:test_wrap_around\n");
}

static void
test_contains(void* mp)
{
	stamp();
	printf("test_contains\n");
	ec_ringbuf_ch_t rb;
	ec_ringbuf_ch_init(&rb, mp, 2, NULL);
	char k1 = 'A', k2 = 'B', bad = 'Z';
	rb.push(&rb, &k1);
	rb.push(&rb, &k2);
	TASSERT(rb.contains(&rb, &k1) == EC_OK);
	TASSERT(rb.contains(&rb, &bad) == EC_ERR_NOT_FOUND);
	rb.clear(&rb);
	printf("pass:test_contains\n");
}

static void
foreach_ch(char x)
{
	(void)x;
}
static void
foreach_ch_test(char x)
{
	(void)x;
}

static void
test_foreach(void* mp)
{
	stamp();
	printf("test_foreach\n");
	ec_ringbuf_ch_t rb;
	ec_ringbuf_ch_init(&rb, mp, 4, NULL);
	char vals[4] = { 'a', 'b', 'c', 'd' };
	for (int i = 0; i < 4; i++)
		{
			rb.push(&rb, &vals[i]);
		}
	TASSERT(rb.foreach (&rb, foreach_ch) == EC_OK);
	rb.clear(&rb);
	printf("pass:test_foreach\n");
}

static void
test_expand(void* mp)
{
	stamp();
	printf("test_expand\n");
	ec_ringbuf_ch_t rb;
	ec_ringbuf_ch_init(&rb, mp, 1, NULL);
	char out;
	for (char i = '0'; i < '8'; i++)
		{
			rb.push(&rb, &i);
		}
	TASSERT(rb.capacity_fn(&rb) >= 8);
	for (char i = '0'; i < '8'; i++)
		{
			ec_status_t st = rb.pop(&rb, &out);
			TASSERT(st == EC_OK);
			TASSERT(out == i);
		}
	rb.clear(&rb);
	printf("pass:test_expand\n");
}

static int
cmp_desc(const char* a, const char* b)
{
	return (*a > *b) ? -1 : ((*a < *b) ? 1 : 0);
}
static void
test_custom_compare(void* mp)
{
	stamp();
	printf("test_custom_compare\n");
	ec_ringbuf_ch_t rb;
	ec_ringbuf_ch_init(&rb, mp, 4, cmp_desc);
	char a = 'a', b = 'b', c = 'c';
	rb.push(&rb, &a);
	rb.push(&rb, &b);
	rb.push(&rb, &c);
	char val = 'b';
	TASSERT(rb.contains(&rb, &val) == EC_OK);
	char val2 = 'z';
	TASSERT(rb.contains(&rb, &val2) == EC_ERR_NOT_FOUND);
	rb.clear(&rb);
	printf("pass:test_custom_compare\n");
}

static void
test_invalid_args(void* mp)
{
	stamp();
	printf("test_invalid_args\n");
	ec_ringbuf_ch_t rb;
	ec_ringbuf_ch_init(&rb, mp, 4, NULL);
	char val = 'X', out;

	TASSERT(ec_ringbuf_ch_init(NULL, mp, 4, NULL) == EC_ERR_INVALID_ARG);
	TASSERT(rb.push(NULL, &val) == EC_ERR_INVALID_ARG);
	TASSERT(rb.push(&rb, NULL) == EC_ERR_INVALID_ARG);
	TASSERT(rb.pop(NULL, &out) == EC_ERR_INVALID_ARG);
	TASSERT(rb.pop(&rb, NULL) == EC_ERR_INVALID_ARG);
	TASSERT(rb.front(NULL, &out) == EC_ERR_INVALID_ARG);
	TASSERT(rb.front(&rb, NULL) == EC_ERR_INVALID_ARG);
	TASSERT(rb.back(NULL, &out) == EC_ERR_INVALID_ARG);
	TASSERT(rb.back(&rb, NULL) == EC_ERR_INVALID_ARG);
	TASSERT(rb.clear(NULL) == EC_ERR_INVALID_ARG);
	TASSERT(rb.contains(NULL, &val) == EC_ERR_INVALID_ARG);
	TASSERT(rb.contains(&rb, NULL) == EC_ERR_INVALID_ARG);

	TASSERT(rb.foreach (NULL, foreach_ch_test) == EC_ERR_INVALID_ARG);
	TASSERT(rb.foreach (&rb, NULL) == EC_ERR_INVALID_ARG);

	TASSERT(rb.size_fn(NULL) == 0);
	TASSERT(rb.capacity_fn(NULL) == 0);

	rb.clear(&rb);
	printf("pass:test_invalid_args\n");
}

int
main(void)
{
	printf("Starting ec_ringbuf_ch tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif
	test_init(mp);
	test_push_pop(mp);
	test_front_back(mp);
	test_clear(mp);
	test_wrap_around(mp);
	test_contains(mp);
	test_foreach(mp);
	test_expand(mp);
	test_custom_compare(mp);
	test_invalid_args(mp);
#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif
	printf("All ec_ringbuf_ch tests passed.\n");
	return 0;
}
