#include "../../src/ec_ds.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

#define TEST_ASSERT(cond)                                                             \
	do                                                                                \
		{                                                                             \
			if (!(cond))                                                              \
				{                                                                     \
					fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #cond); \
					exit(3);                                                          \
				}                                                                     \
		}                                                                             \
	while (0)

static void
test_ds_make_find_plain(void* mp)
{
	int a = 42, b = 100;
	ec_ds_int_node_t* n1 = ec_ds_int_make_set(&a, mp);
	ec_ds_int_node_t* n2 = ec_ds_int_make_set(&b, mp);
	TEST_ASSERT(n1 && n2);
	TEST_ASSERT(ec_ds_int_find(n1) == n1);
	TEST_ASSERT(ec_ds_int_find(n2) == n2);
}

static void
test_ds_union_by_rank_plain(void* mp)
{
	int a = 1, b = 2, c = 3;
	ec_ds_int_node_t* n1 = ec_ds_int_make_set(&a, mp);
	ec_ds_int_node_t* n2 = ec_ds_int_make_set(&b, mp);
	ec_ds_int_node_t* n3 = ec_ds_int_make_set(&c, mp);
	ec_ds_int_union_by_rank(n1, n2);
	TEST_ASSERT(ec_ds_int_find(n1) == ec_ds_int_find(n2));
	ec_ds_int_union_by_rank(n2, n3);
	TEST_ASSERT(ec_ds_int_find(n1) == ec_ds_int_find(n3));
}

static void
test_ds_union_by_size_plain(void* mp)
{
	int a = 10, b = 20, c = 30;
	ec_ds_int_node_t* n1 = ec_ds_int_make_set(&a, mp);
	ec_ds_int_node_t* n2 = ec_ds_int_make_set(&b, mp);
	ec_ds_int_node_t* n3 = ec_ds_int_make_set(&c, mp);
	ec_ds_int_union_by_size(n1, n2);
	TEST_ASSERT(ec_ds_int_find(n1) == ec_ds_int_find(n2));
	ec_ds_int_union_by_size(n1, n3);
	TEST_ASSERT(ec_ds_int_find(n1) == ec_ds_int_find(n3));
}

static void
test_ds_make_find_weighted(void* mp)
{
	int a = 42, b = 100;
	ec_ds_int_node_t* w1 = ec_ds_int_make_set(&a, mp);
	ec_ds_int_node_t* w2 = ec_ds_int_make_set(&b, mp);
	TEST_ASSERT(w1 && w2);
	TEST_ASSERT(ec_ds_int_find(w1) == w1);
	TEST_ASSERT(ec_ds_int_find(w2) == w2);
}

static void
test_ds_union_by_rank_weighted(void* mp)
{
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
}

static void
test_ds_union_by_size_weighted(void* mp)
{
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
}

static void
test_ds_calc_not_connected(void* mp)
{
	int a = 1, b = 2, c = 3;
	ec_ds_int_node_t* w1 = ec_ds_int_make_set(&a, mp);
	ec_ds_int_node_t* w2 = ec_ds_int_make_set(&b, mp);
	ec_ds_int_node_t* w3 = ec_ds_int_make_set(&c, mp);
	ec_ds_int_union_by_rank_weighted(w1, w2, 2.0);
	double r = ec_ds_int_calc(w1, w3);
	TEST_ASSERT(r == -1.0);
}

static void
test_ds_union_by_rank_idempotent(void* mp)
{
	int a = 1, b = 2;
	ec_ds_int_node_t* n1 = ec_ds_int_make_set(&a, mp);
	ec_ds_int_node_t* n2 = ec_ds_int_make_set(&b, mp);
	ec_ds_int_union_by_rank(n1, n2);
	ec_ds_int_union_by_rank(n1, n2);
	TEST_ASSERT(ec_ds_int_find(n1) == ec_ds_int_find(n2));
}

static void
test_ds_union_by_size_idempotent(void* mp)
{
	int a = 10, b = 20;
	ec_ds_int_node_t* n1 = ec_ds_int_make_set(&a, mp);
	ec_ds_int_node_t* n2 = ec_ds_int_make_set(&b, mp);
	ec_ds_int_union_by_size(n1, n2);
	ec_ds_int_union_by_size(n1, n2);
	TEST_ASSERT(ec_ds_int_find(n1) == ec_ds_int_find(n2));
}

static void
test_ds_union_by_rank_weighted_idempotent(void* mp)
{
	int a = 1, b = 2;
	ec_ds_int_node_t* w1 = ec_ds_int_make_set(&a, mp);
	ec_ds_int_node_t* w2 = ec_ds_int_make_set(&b, mp);
	ec_ds_int_union_by_rank_weighted(w1, w2, 2.0);
	ec_ds_int_union_by_rank_weighted(w1, w2, 3.0);
	TEST_ASSERT(ec_ds_int_find(w1) == ec_ds_int_find(w2));
}

static void
test_ds_union_by_size_weighted_idempotent(void* mp)
{
	int a = 10, b = 20;
	ec_ds_int_node_t* w1 = ec_ds_int_make_set(&a, mp);
	ec_ds_int_node_t* w2 = ec_ds_int_make_set(&b, mp);
	ec_ds_int_union_by_size_weighted(w1, w2, 4.0);
	ec_ds_int_union_by_size_weighted(w1, w2, 5.0);
	TEST_ASSERT(ec_ds_int_find(w1) == ec_ds_int_find(w2));
}

static void
test_ds_calc_connected_pos(void* mp)
{
	int a = 1, b = 2;
	ec_ds_int_node_t* w1 = ec_ds_int_make_set(&a, mp);
	ec_ds_int_node_t* w2 = ec_ds_int_make_set(&b, mp);
	ec_ds_int_union_by_rank_weighted(w1, w2, 2.0);
	double r = ec_ds_int_calc(w1, w2);
	TEST_ASSERT(r != -1.0);
}

static void
test_ds_chain_weighted(void* mp)
{
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
	test_ds_union_by_rank_idempotent(mp);
	test_ds_union_by_size_plain(mp);
	test_ds_union_by_size_idempotent(mp);
	test_ds_make_find_weighted(mp);
	test_ds_union_by_rank_weighted(mp);
	test_ds_union_by_rank_weighted_idempotent(mp);
	test_ds_union_by_size_weighted(mp);
	test_ds_union_by_size_weighted_idempotent(mp);
	test_ds_calc_connected_pos(mp);
	test_ds_calc_not_connected(mp);
	test_ds_chain_weighted(mp);
#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif
	printf("All ec_ds tests passed.\n");
	return 0;
}
