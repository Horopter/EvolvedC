#include "../../src/ec_ds.h"
#include <assert.h>
#include <stdio.h>

typedef struct
{
	int id;
} key_t;
EC_DS_DEF(key_t, key)

static void
test_make_set_pos(void)
{
	key_t a = { 1 }, b = { 2 }, c = { 3 };
	ec_ds_key_node_t* na = ec_ds_key_make_set(&a, NULL);
	ec_ds_key_node_t* nb = ec_ds_key_make_set(&b, NULL);
	ec_ds_key_node_t* nc = ec_ds_key_make_set(&c, NULL);
	assert(na && nb && nc);
}

static void
test_union_by_size_pos(void)
{
	key_t a = { 1 }, b = { 2 };
	ec_ds_key_node_t* na = ec_ds_key_make_set(&a, NULL);
	ec_ds_key_node_t* nb = ec_ds_key_make_set(&b, NULL);
	ec_ds_key_union_by_size(na, nb);
	assert(ec_ds_key_find(na) == ec_ds_key_find(nb));
}

static void
test_calc_not_connected_neg(void)
{
	key_t a = { 1 }, b = { 2 }, c = { 3 };
	ec_ds_key_node_t* na = ec_ds_key_make_set(&a, NULL);
	ec_ds_key_node_t* nb = ec_ds_key_make_set(&b, NULL);
	ec_ds_key_node_t* nc = ec_ds_key_make_set(&c, NULL);
	ec_ds_key_union_by_size(na, nb);
	assert(ec_ds_key_calc(na, nc) == -1.0);
}

static void
test_find_pos(void)
{
	key_t a = { 1 };
	ec_ds_key_node_t* na = ec_ds_key_make_set(&a, NULL);
	assert(ec_ds_key_find(na) == na);
}

static void
test_union_by_rank_pos(void)
{
	key_t a = { 1 }, b = { 2 };
	ec_ds_key_node_t* na = ec_ds_key_make_set(&a, NULL);
	ec_ds_key_node_t* nb = ec_ds_key_make_set(&b, NULL);
	ec_ds_key_union_by_rank(na, nb);
	assert(ec_ds_key_find(na) == ec_ds_key_find(nb));
}

static void
test_union_by_rank_idempotent(void)
{
	key_t a = { 1 }, b = { 2 };
	ec_ds_key_node_t* na = ec_ds_key_make_set(&a, NULL);
	ec_ds_key_node_t* nb = ec_ds_key_make_set(&b, NULL);
	ec_ds_key_union_by_rank(na, nb);
	ec_ds_key_union_by_rank(na, nb);
	assert(ec_ds_key_find(na) == ec_ds_key_find(nb));
}

static void
test_union_by_rank_weighted_pos(void)
{
	key_t a = { 1 }, c = { 3 };
	ec_ds_key_node_t* na = ec_ds_key_make_set(&a, NULL);
	ec_ds_key_node_t* nc = ec_ds_key_make_set(&c, NULL);
	ec_ds_key_union_by_rank_weighted(nc, na, 2.0);
	assert(ec_ds_key_find(na) == ec_ds_key_find(nc));
}

static void
test_union_by_rank_weighted_idempotent(void)
{
	key_t a = { 1 }, b = { 2 };
	ec_ds_key_node_t* na = ec_ds_key_make_set(&a, NULL);
	ec_ds_key_node_t* nb = ec_ds_key_make_set(&b, NULL);
	ec_ds_key_union_by_rank_weighted(na, nb, 2.0);
	ec_ds_key_union_by_rank_weighted(na, nb, 3.0);
	assert(ec_ds_key_find(na) == ec_ds_key_find(nb));
}

static void
test_union_by_size_idempotent(void)
{
	key_t a = { 1 }, b = { 2 };
	ec_ds_key_node_t* na = ec_ds_key_make_set(&a, NULL);
	ec_ds_key_node_t* nb = ec_ds_key_make_set(&b, NULL);
	ec_ds_key_union_by_size(na, nb);
	ec_ds_key_union_by_size(na, nb);
	assert(ec_ds_key_find(na) == ec_ds_key_find(nb));
}

static void
test_union_by_size_weighted_pos(void)
{
	key_t a = { 1 }, c = { 3 };
	ec_ds_key_node_t* na = ec_ds_key_make_set(&a, NULL);
	ec_ds_key_node_t* nc = ec_ds_key_make_set(&c, NULL);
	ec_ds_key_union_by_size_weighted(nc, na, 4.0);
	assert(ec_ds_key_find(na) == ec_ds_key_find(nc));
}

static void
test_union_by_size_weighted_idempotent(void)
{
	key_t a = { 1 }, c = { 3 };
	ec_ds_key_node_t* na = ec_ds_key_make_set(&a, NULL);
	ec_ds_key_node_t* nc = ec_ds_key_make_set(&c, NULL);
	ec_ds_key_union_by_size_weighted(nc, na, 2.0);
	ec_ds_key_union_by_size_weighted(nc, na, 3.0);
	assert(ec_ds_key_find(na) == ec_ds_key_find(nc));
}

static void
test_calc_connected_pos(void)
{
	key_t a = { 1 }, c = { 3 };
	ec_ds_key_node_t* na = ec_ds_key_make_set(&a, NULL);
	ec_ds_key_node_t* nc = ec_ds_key_make_set(&c, NULL);
	ec_ds_key_union_by_rank_weighted(nc, na, 2.0);
	double r = ec_ds_key_calc(na, nc);
	assert(r != -1.0);
}

int
main(void)
{
	printf("Starting ec_ds_key tests...\n");
	test_make_set_pos();
	test_find_pos();
	test_union_by_rank_pos();
	test_union_by_rank_idempotent();
	test_union_by_rank_weighted_pos();
	test_union_by_rank_weighted_idempotent();
	test_union_by_size_pos();
	test_union_by_size_idempotent();
	test_union_by_size_weighted_pos();
	test_union_by_size_weighted_idempotent();
	test_calc_connected_pos();
	test_calc_not_connected_neg();
	printf("All ec_ds_key tests passed.\n");
	return 0;
}
