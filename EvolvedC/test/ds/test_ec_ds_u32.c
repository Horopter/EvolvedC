#include "../../src/ec_ds.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static void
test_make_set_pos(void)
{
	uint32_t a = 100U, b = 200U;
	ec_ds_u32_node_t* na = ec_ds_u32_make_set(&a, NULL);
	ec_ds_u32_node_t* nb = ec_ds_u32_make_set(&b, NULL);
	assert(na && nb);
}

static void
test_find_pos(void)
{
	uint32_t a = 100U;
	ec_ds_u32_node_t* na = ec_ds_u32_make_set(&a, NULL);
	assert(ec_ds_u32_find(na) == na);
}

static void
test_union_by_rank_pos(void)
{
	uint32_t a = 100U, b = 200U;
	ec_ds_u32_node_t* na = ec_ds_u32_make_set(&a, NULL);
	ec_ds_u32_node_t* nb = ec_ds_u32_make_set(&b, NULL);
	ec_ds_u32_union_by_rank(na, nb);
	assert(ec_ds_u32_find(na) == ec_ds_u32_find(nb));
}

static void
test_union_same_set_idempotent(void)
{
	uint32_t a = 100U, b = 200U;
	ec_ds_u32_node_t* na = ec_ds_u32_make_set(&a, NULL);
	ec_ds_u32_node_t* nb = ec_ds_u32_make_set(&b, NULL);
	ec_ds_u32_union_by_rank(na, nb);
	ec_ds_u32_union_by_rank(na, nb);
	assert(ec_ds_u32_find(na) == ec_ds_u32_find(nb));
}

static void
test_union_by_rank_weighted_pos(void)
{
	uint32_t a = 100U, c = 300U;
	ec_ds_u32_node_t* na = ec_ds_u32_make_set(&a, NULL);
	ec_ds_u32_node_t* nc = ec_ds_u32_make_set(&c, NULL);
	ec_ds_u32_union_by_rank_weighted(nc, na, 2.0);
	assert(ec_ds_u32_find(na) == ec_ds_u32_find(nc));
}

static void
test_calc_connected_pos(void)
{
	uint32_t a = 100U, c = 300U;
	ec_ds_u32_node_t* na = ec_ds_u32_make_set(&a, NULL);
	ec_ds_u32_node_t* nc = ec_ds_u32_make_set(&c, NULL);
	ec_ds_u32_union_by_rank_weighted(nc, na, 2.0);
	double v1 = ec_ds_u32_calc(na, nc);
	assert(v1 != -1.0);
}

static void
test_union_by_size_pos(void)
{
	uint32_t a = 100U, b = 200U;
	ec_ds_u32_node_t* na = ec_ds_u32_make_set(&a, NULL);
	ec_ds_u32_node_t* nb = ec_ds_u32_make_set(&b, NULL);
	ec_ds_u32_union_by_size(na, nb);
	assert(ec_ds_u32_find(na) == ec_ds_u32_find(nb));
}

static void
test_union_by_size_idempotent(void)
{
	uint32_t a = 100U, b = 200U;
	ec_ds_u32_node_t* na = ec_ds_u32_make_set(&a, NULL);
	ec_ds_u32_node_t* nb = ec_ds_u32_make_set(&b, NULL);
	ec_ds_u32_union_by_size(na, nb);
	ec_ds_u32_union_by_size(na, nb);
	assert(ec_ds_u32_find(na) == ec_ds_u32_find(nb));
}

static void
test_union_by_size_weighted_pos(void)
{
	uint32_t a = 100U, c = 300U;
	ec_ds_u32_node_t* na = ec_ds_u32_make_set(&a, NULL);
	ec_ds_u32_node_t* nc = ec_ds_u32_make_set(&c, NULL);
	ec_ds_u32_union_by_size_weighted(nc, na, 2.0);
	assert(ec_ds_u32_find(na) == ec_ds_u32_find(nc));
}

static void
test_union_by_size_weighted_idempotent(void)
{
	uint32_t a = 100U, c = 300U;
	ec_ds_u32_node_t* na = ec_ds_u32_make_set(&a, NULL);
	ec_ds_u32_node_t* nc = ec_ds_u32_make_set(&c, NULL);
	ec_ds_u32_union_by_size_weighted(nc, na, 2.0);
	ec_ds_u32_union_by_size_weighted(nc, na, 3.0);
	assert(ec_ds_u32_find(na) == ec_ds_u32_find(nc));
}

static void
test_union_by_rank_weighted_idempotent(void)
{
	uint32_t a = 100U, b = 200U;
	ec_ds_u32_node_t* na = ec_ds_u32_make_set(&a, NULL);
	ec_ds_u32_node_t* nb = ec_ds_u32_make_set(&b, NULL);
	ec_ds_u32_union_by_rank_weighted(na, nb, 2.0);
	ec_ds_u32_union_by_rank_weighted(na, nb, 3.0);
	assert(ec_ds_u32_find(na) == ec_ds_u32_find(nb));
}

static void
test_calc_not_connected_neg(void)
{
	uint32_t a = 100U, d = 400U;
	ec_ds_u32_node_t* na = ec_ds_u32_make_set(&a, NULL);
	ec_ds_u32_node_t* nd = ec_ds_u32_make_set(&d, NULL);
	double v2 = ec_ds_u32_calc(na, nd);
	assert(v2 == -1.0);
}

int
main(void)
{
	printf("Starting ec_ds_u32 tests...\n");
	test_make_set_pos();
	test_find_pos();
	test_union_by_rank_pos();
	test_union_same_set_idempotent();
	test_union_by_rank_weighted_pos();
	test_union_by_rank_weighted_idempotent();
	test_union_by_size_pos();
	test_union_by_size_idempotent();
	test_union_by_size_weighted_pos();
	test_union_by_size_weighted_idempotent();
	test_calc_connected_pos();
	test_calc_not_connected_neg();
	printf("All ec_ds_u32 tests passed.\n");
	return 0;
}
