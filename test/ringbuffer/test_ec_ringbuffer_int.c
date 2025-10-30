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

/*--- Tests ---*/

static void
test_init(void* mp)
{
	stamp();
	printf("test_init\n");
	ec_ringbuf_int_t rb;
	ec_status_t st = ec_ringbuf_int_init(&rb, mp, 0, NULL);
	TASSERT(st == EC_OK && rb.size_fn(&rb) == 0 && rb.capacity_fn(&rb) == 0);
	rb.clear(&rb);
	printf("pass:test_init\n");
}

static void
test_push_pop(void* mp)
{
	stamp();
	printf("test_push_pop\n");
	ec_ringbuf_int_t rb;
	ec_ringbuf_int_init(&rb, mp, 2, NULL);
	int a = 10, b = 20, out;
	rb.push(&rb, &a);
	rb.push(&rb, &b);
	TASSERT(rb.size_fn(&rb) == 2);
	TASSERT(rb.pop(&rb, &out) == EC_OK && out == 10);
	TASSERT(rb.pop(&rb, &out) == EC_OK && out == 20);
	TASSERT(rb.pop(&rb, &out) == EC_ERR_NOT_FOUND);
	rb.clear(&rb);
	printf("pass:test_push_pop\n");
}

static void
test_front_back(void* mp)
{
	stamp();
	printf("test_front_back\n");
	ec_ringbuf_int_t rb;
	ec_ringbuf_int_init(&rb, mp, 3, NULL);
	int x1 = 100, x2 = 200, x3 = 300, out;
	rb.push(&rb, &x1);
	rb.push(&rb, &x2);
	rb.push(&rb, &x3);
	TASSERT(rb.front(&rb, &out) == EC_OK && out == 100);
	TASSERT(rb.back(&rb, &out) == EC_OK && out == 300);
	rb.clear(&rb);
	TASSERT(rb.front(&rb, &out) == EC_ERR_NOT_FOUND);
	printf("pass:test_front_back\n");
}

static void
test_clear(void* mp)
{
	stamp();
	printf("test_clear\n");
	ec_ringbuf_int_t rb;
	ec_ringbuf_int_init(&rb, mp, 0, NULL);
	int x = 999;
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
	ec_ringbuf_int_t rb;
	ec_ringbuf_int_init(&rb, mp, 3, NULL);
	int a = 10, b = 20, c = 30, d = 40, out;
	rb.push(&rb, &a);
	rb.push(&rb, &b);
	rb.push(&rb, &c);
	// now ring is full
	rb.pop(&rb, &out);
	TASSERT(out == 10); // free one slot
	rb.push(&rb, &d);   // wraps around
	// pop everything
	TASSERT(rb.pop(&rb, &out) == EC_OK && out == 20);
	TASSERT(rb.pop(&rb, &out) == EC_OK && out == 30);
	TASSERT(rb.pop(&rb, &out) == EC_OK && out == 40);
	TASSERT(rb.pop(&rb, &out) == EC_ERR_NOT_FOUND);
	rb.clear(&rb);
	printf("pass:test_wrap_around\n");
}

static void
test_contains(void* mp)
{
	stamp();
	printf("test_contains\n");
	ec_ringbuf_int_t rb;
	ec_ringbuf_int_init(&rb, mp, 2, NULL);
	int k1 = 42, k2 = 55, bad = 999;
	rb.push(&rb, &k1);
	rb.push(&rb, &k2);
	TASSERT(rb.contains(&rb, &k1) == EC_OK);
	TASSERT(rb.contains(&rb, &bad) == EC_ERR_NOT_FOUND);
	rb.clear(&rb);
	printf("pass:test_contains\n");
}

static void
foreach_int(int x)
{
	(void)x;
} // dummy
static void
test_foreach(void* mp)
{
	stamp();
	printf("test_foreach\n");
	ec_ringbuf_int_t rb;
	ec_ringbuf_int_init(&rb, mp, 4, NULL);
	for (int i = 1; i <= 4; i++)
		{
			rb.push(&rb, &i);
		}
	TASSERT(rb.foreach (&rb, foreach_int) == EC_OK);
	rb.clear(&rb);
	printf("pass:test_foreach\n");
}

static void
test_expand(void* mp)
{
	stamp();
	printf("test_expand\n");
	ec_ringbuf_int_t rb;
	ec_ringbuf_int_init(&rb, mp, 1, NULL);
	int out;
	// push more than capacity => triggers grow
	for (int i = 0; i < 8; i++)
		{
			rb.push(&rb, &i);
		}
	TASSERT(rb.capacity_fn(&rb) >= 8);
	// check in FIFO order
	for (int i = 0; i < 8; i++)
		{
			ec_status_t st = rb.pop(&rb, &out);
			TASSERT(st == EC_OK);
			TASSERT(out == i);
		}
	rb.clear(&rb);
	printf("pass:test_expand\n");
}

static int
cmp_desc(const int* a, const int* b)
{
	return (*a > *b) ? -1 : ((*a < *b) ? 1 : 0);
}
static void
test_custom_compare(void* mp)
{
	stamp();
	printf("test_custom_compare\n");
	ec_ringbuf_int_t rb;
	ec_ringbuf_int_init(&rb, mp, 4, cmp_desc);
	// push some items
	int a = 10, b = 5, c = 20;
	rb.push(&rb, &a);
	rb.push(&rb, &b);
	rb.push(&rb, &c);
	// check contains uses compare => "equal" is compare(...)==0
	int val5 = 5;
	TASSERT(rb.contains(&rb, &val5) == EC_OK);
	int val7 = 7;
	TASSERT(rb.contains(&rb, &val7) == EC_ERR_NOT_FOUND);
	rb.clear(&rb);
	printf("pass:test_custom_compare\n");
}

static void
test_big_random(void* mp)
{
	stamp();
	printf("test_big_random\n");
	ec_ringbuf_int_t rb;
	ec_ringbuf_int_init(&rb, mp, 8, NULL);
	srand((unsigned)time(NULL));
	const int N = 2000;
	int* out = (int*)malloc(sizeof(int) * N);
	// push a bunch
	for (int i = 0; i < N; i++)
		{
			int val = rand() % 10000;
			rb.push(&rb, &val);
		}
	// pop half
	for (int i = 0; i < N / 2; i++)
		{
			TASSERT(rb.pop(&rb, &out[i]) == EC_OK);
		}
	free(out);
	rb.clear(&rb);
	printf("pass:test_big_random\n");
}

int
main(void)
{
	printf("Starting ec_ringbuf_int tests...\n");
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
	test_big_random(mp);
#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif
	printf("All ec_ringbuf_int tests passed.\n");
	return 0;
}
