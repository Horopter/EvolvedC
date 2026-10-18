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

static void
test_init(void* mp)
{
	stamp();
	printf("test_init\n");
	ec_ringbuf_ptr_t rb;
	ec_status_t st = ec_ringbuf_ptr_init(&rb, mp, 0, NULL);
	TASSERT(st == EC_OK && rb.size_fn(&rb) == 0 && rb.capacity_fn(&rb) == 0);
	rb.clear(&rb);
	printf("pass:test_init\n");
}

static void
test_push_pop(void* mp)
{
	stamp();
	printf("test_push_pop\n");
	ec_ringbuf_ptr_t rb;
	ec_ringbuf_ptr_init(&rb, mp, 2, NULL);
	int x = 10, y = 20;
	void *a = &x, *b = &y, *out;
	rb.push(&rb, &a);
	rb.push(&rb, &b);
	TASSERT(rb.size_fn(&rb) == 2);
	TASSERT(rb.pop(&rb, &out) == EC_OK && out == &x);
	TASSERT(rb.pop(&rb, &out) == EC_OK && out == &y);
	TASSERT(rb.pop(&rb, &out) == EC_ERR_NOT_FOUND);
	rb.clear(&rb);
	printf("pass:test_push_pop\n");
}

static void
test_front_back(void* mp)
{
	stamp();
	printf("test_front_back\n");
	ec_ringbuf_ptr_t rb;
	ec_ringbuf_ptr_init(&rb, mp, 3, NULL);
	int x1 = 100, x2 = 200, x3 = 300;
	void *ptr1 = &x1, *ptr2 = &x2, *ptr3 = &x3, *out;
	rb.push(&rb, &ptr1);
	rb.push(&rb, &ptr2);
	rb.push(&rb, &ptr3);
	TASSERT(rb.front(&rb, &out) == EC_OK && out == &x1);
	TASSERT(rb.back(&rb, &out) == EC_OK && out == &x3);
	rb.clear(&rb);
	TASSERT(rb.front(&rb, &out) == EC_ERR_NOT_FOUND);
	printf("pass:test_front_back\n");
}

static void
test_clear(void* mp)
{
	stamp();
	printf("test_clear\n");
	ec_ringbuf_ptr_t rb;
	ec_ringbuf_ptr_init(&rb, mp, 0, NULL);
	int x = 999;
	void* ptr = &x;
	rb.push(&rb, &ptr);
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
	ec_ringbuf_ptr_t rb;
	ec_ringbuf_ptr_init(&rb, mp, 3, NULL);
	int arr[4] = { 10, 20, 30, 40 };
	void *a = &arr[0], *b = &arr[1], *c = &arr[2], *d = &arr[3], *out;
	rb.push(&rb, &a);
	rb.push(&rb, &b);
	rb.push(&rb, &c);
	rb.pop(&rb, &out);
	TASSERT(out == &arr[0]);
	rb.push(&rb, &d);
	TASSERT(rb.pop(&rb, &out) == EC_OK && out == &arr[1]);
	TASSERT(rb.pop(&rb, &out) == EC_OK && out == &arr[2]);
	TASSERT(rb.pop(&rb, &out) == EC_OK && out == &arr[3]);
	TASSERT(rb.pop(&rb, &out) == EC_ERR_NOT_FOUND);
	rb.clear(&rb);
	printf("pass:test_wrap_around\n");
}

static void
test_contains(void* mp)
{
	stamp();
	printf("test_contains\n");
	ec_ringbuf_ptr_t rb;
	ec_ringbuf_ptr_init(&rb, mp, 2, NULL);
	int x = 42, y = 55, z = 999;
	void *k1 = &x, *k2 = &y, *bad = &z;
	rb.push(&rb, &k1);
	rb.push(&rb, &k2);
	TASSERT(rb.contains(&rb, &k1) == EC_OK);
	TASSERT(rb.contains(&rb, &bad) == EC_ERR_NOT_FOUND);
	rb.clear(&rb);
	printf("pass:test_contains\n");
}

static void
foreach_ptr(void* x)
{
	(void)x;
}
static void
foreach_ptr_test(void* x)
{
	(void)x;
}

static void
test_foreach(void* mp)
{
	stamp();
	printf("test_foreach\n");
	ec_ringbuf_ptr_t rb;
	ec_ringbuf_ptr_init(&rb, mp, 4, NULL);
	int arr[4] = { 1, 2, 3, 4 };
	void* ptrs[4] = { &arr[0], &arr[1], &arr[2], &arr[3] };
	for (int i = 0; i < 4; i++)
		{
			rb.push(&rb, &ptrs[i]);
		}
	TASSERT(rb.foreach (&rb, foreach_ptr) == EC_OK);
	rb.clear(&rb);
	printf("pass:test_foreach\n");
}

static void
test_expand(void* mp)
{
	stamp();
	printf("test_expand\n");
	ec_ringbuf_ptr_t rb;
	ec_ringbuf_ptr_init(&rb, mp, 1, NULL);
	void* out;
	int arr[8];
	void* ptrs[8];
	for (int i = 0; i < 8; i++)
		{
			arr[i] = i;
			ptrs[i] = &arr[i];
			rb.push(&rb, &ptrs[i]);
		}
	TASSERT(rb.capacity_fn(&rb) >= 8);
	for (int i = 0; i < 8; i++)
		{
			ec_status_t st = rb.pop(&rb, &out);
			TASSERT(st == EC_OK);
			TASSERT(out == &arr[i]);
		}
	rb.clear(&rb);
	printf("pass:test_expand\n");
}

static int
ptr_cmp(const void* a, const void* b)
{
	// Compare the void* values stored in the buffer
	// a and b are const void* pointers to the buffer elements
	const void* va = *(const void**)a;
	const void* vb = *(const void**)b;
	return (va == vb) ? 0 : ((va < vb) ? -1 : 1);
}
static void
test_custom_compare(void* mp)
{
	stamp();
	printf("test_custom_compare\n");
	ec_ringbuf_ptr_t rb;
	ec_ringbuf_ptr_init(&rb, mp, 4, ptr_cmp);
	int x = 10, y = 5, z = 20;
	void *a = &x, *b = &y, *c = &z;
	rb.push(&rb, &a);
	rb.push(&rb, &b);
	rb.push(&rb, &c);
	void* val = b;  // b = &y, so b is in the buffer
	TASSERT(rb.contains(&rb, &val) == EC_OK);
	int w = 99;
	void* bad = &w;
	TASSERT(rb.contains(&rb, &bad) == EC_ERR_NOT_FOUND);
	rb.clear(&rb);
	printf("pass:test_custom_compare\n");
}

static void
test_invalid_args(void* mp)
{
	stamp();
	printf("test_invalid_args\n");
	ec_ringbuf_ptr_t rb;
	ec_ringbuf_ptr_init(&rb, mp, 4, NULL);
	int x = 42;
	void *val = &x, *out;

	TASSERT(ec_ringbuf_ptr_init(NULL, mp, 4, NULL) == EC_ERR_INVALID_ARG);
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

	TASSERT(rb.foreach (NULL, foreach_ptr_test) == EC_ERR_INVALID_ARG);
	TASSERT(rb.foreach (&rb, NULL) == EC_ERR_INVALID_ARG);

	TASSERT(rb.size_fn(NULL) == 0);
	TASSERT(rb.capacity_fn(NULL) == 0);

	rb.clear(&rb);
	printf("pass:test_invalid_args\n");
}

int
main(void)
{
	printf("Starting ec_ringbuf_ptr tests...\n");
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
	printf("All ec_ringbuf_ptr tests passed.\n");
	return 0;
}
