#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../../src/ec_ds.h"
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

static void
print_stacktrace(void)
{
#ifdef __linux__
	void* arr[32];
	size_t n = backtrace(arr, 32);
	char** sym = backtrace_symbols(arr, n);
	if (sym)
		{
			for (size_t i = 0; i < n; i++)
				fprintf(stderr, "%s\n", sym[i]);
			free(sym);
		}
#else
	fprintf(stderr, "(NoStackTrace)\n");
#endif
}
#define TEST_ASSERT(cond)                                                             \
	do                                                                                \
		{                                                                             \
			if (!(cond))                                                              \
				{                                                                     \
					fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #cond); \
					print_stacktrace();                                               \
					exit(3);                                                          \
				}                                                                     \
		}                                                                             \
	while (0)
static void
stamp(void)
{
	time_t t = time(NULL);
	struct tm* m = localtime(&t);
	if (m)
		{
			char b[64];
			strftime(b, sizeof(b), "[ %Y-%m-%d %H:%M:%S ]", m);
			printf("%s ", b);
		}
	else
		printf("[timeerr] ");
}

/* Plain DS tests */
static void
test_ds_make_find_plain(void* mp)
{
	stamp();
	printf("test_ds_make_find_plain\n");
	int a = 42, b = 100;
	ec_ds_int_node_t* n1 = ec_ds_int_make_set(&a, mp);
	ec_ds_int_node_t* n2 = ec_ds_int_make_set(&b, mp);
	TEST_ASSERT(n1 && n2);
	TEST_ASSERT(ec_ds_int_find(n1) == n1);
	TEST_ASSERT(ec_ds_int_find(n2) == n2);
	printf("pass:test_ds_make_find_plain\n");
}
static void
test_ds_union_by_rank_plain(void* mp)
{
	stamp();
	printf("test_ds_union_by_rank_plain\n");
	int a = 1, b = 2, c = 3;
	ec_ds_int_node_t* n1 = ec_ds_int_make_set(&a, mp);
	ec_ds_int_node_t* n2 = ec_ds_int_make_set(&b, mp);
	ec_ds_int_node_t* n3 = ec_ds_int_make_set(&c, mp);
	ec_ds_int_union_by_rank(n1, n2);
	TEST_ASSERT(ec_ds_int_find(n1) == ec_ds_int_find(n2));
	ec_ds_int_union_by_rank(n2, n3);
	TEST_ASSERT(ec_ds_int_find(n1) == ec_ds_int_find(n3));
	printf("pass:test_ds_union_by_rank_plain\n");
}
static void
test_ds_union_by_size_plain(void* mp)
{
	stamp();
	printf("test_ds_union_by_size_plain\n");
	int a = 10, b = 20, c = 30;
	ec_ds_int_node_t* n1 = ec_ds_int_make_set(&a, mp);
	ec_ds_int_node_t* n2 = ec_ds_int_make_set(&b, mp);
	ec_ds_int_node_t* n3 = ec_ds_int_make_set(&c, mp);
	ec_ds_int_union_by_size(n1, n2);
	TEST_ASSERT(ec_ds_int_find(n1) == ec_ds_int_find(n2));
	ec_ds_int_union_by_size(n1, n3);
	TEST_ASSERT(ec_ds_int_find(n1) == ec_ds_int_find(n3));
	printf("pass:test_ds_union_by_size_plain\n");
}

/* Weighted DS tests */
static void
test_ds_make_find_weighted(void* mp)
{
	stamp();
	printf("test_ds_make_find_weighted\n");
	int a = 42, b = 100;
	ec_ds_int_node_t* w1 = ec_ds_int_make_set(&a, mp);
	ec_ds_int_node_t* w2 = ec_ds_int_make_set(&b, mp);
	TEST_ASSERT(w1 && w2);
	TEST_ASSERT(ec_ds_int_find(w1) == w1);
	TEST_ASSERT(ec_ds_int_find(w2) == w2);
	printf("pass:test_ds_make_find_weighted\n");
}
static void
test_ds_union_by_rank_weighted(void* mp)
{
	stamp();
	printf("test_ds_union_by_rank_weighted\n");
	int a = 1, b = 2, c = 3;
	ec_ds_int_node_t* w1 = ec_ds_int_make_set(&a, mp);
	ec_ds_int_node_t* w2 = ec_ds_int_make_set(&b, mp);
	ec_ds_int_node_t* w3 = ec_ds_int_make_set(&c, mp);
	ec_ds_int_union_by_rank_weighted(w1, w2, 2.0);
	TEST_ASSERT(ec_ds_int_find(w1) == ec_ds_int_find(w2));
	double r = ec_ds_int_calc(w1, w2);
	TEST_ASSERT(r > 1.99 && r < 2.01);
	ec_ds_int_union_by_rank_weighted(w2, w3, 3.0);
	r = ec_ds_int_calc(w1, w3);
	TEST_ASSERT(r > 5.99 && r < 6.01);
	printf("pass:test_ds_union_by_rank_weighted\n");
}
static void
test_ds_union_by_size_weighted(void* mp)
{
	stamp();
	printf("test_ds_union_by_size_weighted\n");
	int a = 10, b = 20, c = 30;
	ec_ds_int_node_t* w1 = ec_ds_int_make_set(&a, mp);
	ec_ds_int_node_t* w2 = ec_ds_int_make_set(&b, mp);
	ec_ds_int_node_t* w3 = ec_ds_int_make_set(&c, mp);
	ec_ds_int_union_by_size_weighted(w1, w2, 4.0);
	TEST_ASSERT(ec_ds_int_find(w1) == ec_ds_int_find(w2));
	double r = ec_ds_int_calc(w1, w2);
	TEST_ASSERT(r > 3.99 && r < 4.01);
	ec_ds_int_union_by_size_weighted(w1, w3, 5.0);
	r = ec_ds_int_calc(w1, w3);
	TEST_ASSERT(r > 4.99 && r < 5.01);
	printf("pass:test_ds_union_by_size_weighted\n");
}
static void
test_ds_calc_not_connected(void* mp)
{
	stamp();
	printf("test_ds_calc_not_connected\n");
	int a = 1, b = 2, c = 3;
	ec_ds_int_node_t* w1 = ec_ds_int_make_set(&a, mp);
	ec_ds_int_node_t* w2 = ec_ds_int_make_set(&b, mp);
	ec_ds_int_node_t* w3 = ec_ds_int_make_set(&c, mp);
	ec_ds_int_union_by_rank_weighted(w1, w2, 2.0);
	double r = ec_ds_int_calc(w1, w3);
	TEST_ASSERT(r == -1.0);
	printf("pass:test_ds_calc_not_connected\n");
}
static void
test_ds_chain_weighted(void* mp)
{
	stamp();
	printf("test_ds_chain_weighted\n");
	int a = 1, b = 2, c = 3, d = 4;
	ec_ds_int_node_t* w1 = ec_ds_int_make_set(&a, mp);
	ec_ds_int_node_t* w2 = ec_ds_int_make_set(&b, mp);
	ec_ds_int_node_t* w3 = ec_ds_int_make_set(&c, mp);
	ec_ds_int_node_t* w4 = ec_ds_int_make_set(&d, mp);
	ec_ds_int_union_by_rank_weighted(w1, w2, 2.0);
	ec_ds_int_union_by_rank_weighted(w2, w3, 3.0);
	ec_ds_int_union_by_rank_weighted(w3, w4, 4.0);
	double r = ec_ds_int_calc(w1, w4);
	TEST_ASSERT(r > 23.99 && r < 24.01);
	printf("pass:test_ds_chain_weighted\n");
}

int
main(void)
{
	printf("Starting ec_ds tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif
	test_ds_make_find_plain(mp);
	test_ds_union_by_rank_plain(mp);
	test_ds_union_by_size_plain(mp);
	test_ds_make_find_weighted(mp);
	test_ds_union_by_rank_weighted(mp);
	test_ds_union_by_size_weighted(mp);
	test_ds_calc_not_connected(mp);
	test_ds_chain_weighted(mp);
#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif
	printf("All ec_ds tests passed.\n");
	return 0;
}
