#include "../../src/ec_ds.h"
#include <assert.h>
#include <stdio.h>

static void
test_make_set_pos(void)
{
	const char *ka = "a", *kb = "b", *kc = "c";
	ec_ds_str_node_t* na = ec_ds_str_make_set(ka, NULL);
	ec_ds_str_node_t* nb = ec_ds_str_make_set(kb, NULL);
	ec_ds_str_node_t* nc = ec_ds_str_make_set(kc, NULL);
	assert(na && nb && nc);
}

static void
test_find_pos(void)
{
	const char* ka = "a";
	ec_ds_str_node_t* na = ec_ds_str_make_set(ka, NULL);
	assert(ec_ds_str_find(na) == na);
}

static void
test_union_by_size_pos(void)
{
	const char *ka = "a", *kb = "b";
	ec_ds_str_node_t* na = ec_ds_str_make_set(ka, NULL);
	ec_ds_str_node_t* nb = ec_ds_str_make_set(kb, NULL);
	ec_ds_str_union_by_size(na, nb);
	assert(ec_ds_str_find(na) == ec_ds_str_find(nb));
}

static void
test_union_by_rank_pos(void)
{
	const char *ka = "a", *kb = "b";
	ec_ds_str_node_t* na = ec_ds_str_make_set(ka, NULL);
	ec_ds_str_node_t* nb = ec_ds_str_make_set(kb, NULL);
	ec_ds_str_union_by_rank(na, nb);
	assert(ec_ds_str_find(na) == ec_ds_str_find(nb));
}

static void
test_union_by_rank_idempotent(void)
{
	const char *ka = "a", *kb = "b";
	ec_ds_str_node_t* na = ec_ds_str_make_set(ka, NULL);
	ec_ds_str_node_t* nb = ec_ds_str_make_set(kb, NULL);
	ec_ds_str_union_by_rank(na, nb);
	ec_ds_str_union_by_rank(na, nb);
	assert(ec_ds_str_find(na) == ec_ds_str_find(nb));
}

static void
test_union_by_rank_weighted_pos(void)
{
	const char *ka = "a", *kc = "c";
	ec_ds_str_node_t* na = ec_ds_str_make_set(ka, NULL);
	ec_ds_str_node_t* nc = ec_ds_str_make_set(kc, NULL);
	ec_ds_str_union_by_rank_weighted(nc, na, 2.0);
	assert(ec_ds_str_find(na) == ec_ds_str_find(nc));
}

static void
test_union_by_rank_weighted_idempotent(void)
{
	const char *ka = "a", *kb = "b";
	ec_ds_str_node_t* na = ec_ds_str_make_set(ka, NULL);
	ec_ds_str_node_t* nb = ec_ds_str_make_set(kb, NULL);
	ec_ds_str_union_by_rank_weighted(na, nb, 2.0);
	ec_ds_str_union_by_rank_weighted(na, nb, 3.0);
	assert(ec_ds_str_find(na) == ec_ds_str_find(nb));
}

static void
test_union_by_size_idempotent(void)
{
	const char *ka = "a", *kb = "b";
	ec_ds_str_node_t* na = ec_ds_str_make_set(ka, NULL);
	ec_ds_str_node_t* nb = ec_ds_str_make_set(kb, NULL);
	ec_ds_str_union_by_size(na, nb);
	ec_ds_str_union_by_size(na, nb);
	assert(ec_ds_str_find(na) == ec_ds_str_find(nb));
}

static void
test_union_by_size_weighted_pos(void)
{
	const char *ka = "a", *kc = "c";
	ec_ds_str_node_t* na = ec_ds_str_make_set(ka, NULL);
	ec_ds_str_node_t* nc = ec_ds_str_make_set(kc, NULL);
	ec_ds_str_union_by_size_weighted(nc, na, 2.0);
	assert(ec_ds_str_find(na) == ec_ds_str_find(nc));
}

static void
test_union_by_size_weighted_idempotent(void)
{
	const char *ka = "a", *kc = "c";
	ec_ds_str_node_t* na = ec_ds_str_make_set(ka, NULL);
	ec_ds_str_node_t* nc = ec_ds_str_make_set(kc, NULL);
	ec_ds_str_union_by_size_weighted(nc, na, 2.0);
	ec_ds_str_union_by_size_weighted(nc, na, 3.0);
	assert(ec_ds_str_find(na) == ec_ds_str_find(nc));
}

static void
test_calc_connected_pos(void)
{
	const char *ka = "a", *kc = "c";
	ec_ds_str_node_t* na = ec_ds_str_make_set(ka, NULL);
	ec_ds_str_node_t* nc = ec_ds_str_make_set(kc, NULL);
	ec_ds_str_union_by_rank_weighted(nc, na, 2.0);
	double v1 = ec_ds_str_calc(na, nc);
	assert(v1 != -1.0);
}

static void
test_calc_not_connected_neg(void)
{
	const char *ka = "a", *kb = "b", *kc = "c";
	ec_ds_str_node_t* na = ec_ds_str_make_set(ka, NULL);
	ec_ds_str_node_t* nb = ec_ds_str_make_set(kb, NULL);
	ec_ds_str_node_t* nc = ec_ds_str_make_set(kc, NULL);
	ec_ds_str_union_by_size(na, nb);
	double r = ec_ds_str_calc(na, nc);
	assert(r == -1.0);
}

int
main(void)
{
	printf("Starting ec_ds_str tests...\n");
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
	printf("All ec_ds_str tests passed.\n");
	return 0;
}
