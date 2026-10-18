#include "../../src/ec_ds.h"
#include <assert.h>
#include <stdio.h>

static void
test_make_set_pos(void)
{
	int a = 1, b = 2, c = 3;
	void *pa = &a, *pb = &b, *pc = &c;
	ec_ds_ptr_node_t* na = ec_ds_ptr_make_set(&pa, NULL);
	ec_ds_ptr_node_t* nb = ec_ds_ptr_make_set(&pb, NULL);
	ec_ds_ptr_node_t* nc = ec_ds_ptr_make_set(&pc, NULL);
	assert(na && nb && nc);
}

static void
test_union_by_rank_pos(void)
{
	int a = 1, b = 2;
	void *pa = &a, *pb = &b;
	ec_ds_ptr_node_t* na = ec_ds_ptr_make_set(&pa, NULL);
	ec_ds_ptr_node_t* nb = ec_ds_ptr_make_set(&pb, NULL);
	ec_ds_ptr_union_by_rank(na, nb);
	assert(ec_ds_ptr_find(na) == ec_ds_ptr_find(nb));
}

static void
test_calc_not_connected_neg(void)
{
	int a = 1, b = 2, c = 3;
	void *pa = &a, *pb = &b, *pc = &c;
	ec_ds_ptr_node_t* na = ec_ds_ptr_make_set(&pa, NULL);
	ec_ds_ptr_node_t* nb = ec_ds_ptr_make_set(&pb, NULL);
	ec_ds_ptr_node_t* nc = ec_ds_ptr_make_set(&pc, NULL);
	ec_ds_ptr_union_by_rank(na, nb);
	assert(ec_ds_ptr_calc(na, nc) == -1.0);
}

static void
test_find_pos(void)
{
	int a = 1;
	void* pa = &a;
	ec_ds_ptr_node_t* na = ec_ds_ptr_make_set(&pa, NULL);
	assert(ec_ds_ptr_find(na) == na);
}

static void
test_union_by_rank_idempotent(void)
{
	int a = 1, b = 2;
	void *pa = &a, *pb = &b;
	ec_ds_ptr_node_t* na = ec_ds_ptr_make_set(&pa, NULL);
	ec_ds_ptr_node_t* nb = ec_ds_ptr_make_set(&pb, NULL);
	ec_ds_ptr_union_by_rank(na, nb);
	ec_ds_ptr_union_by_rank(na, nb);
	assert(ec_ds_ptr_find(na) == ec_ds_ptr_find(nb));
}

static void
test_union_by_rank_weighted_pos(void)
{
	int a = 1, c = 3;
	void *pa = &a, *pc = &c;
	ec_ds_ptr_node_t* na = ec_ds_ptr_make_set(&pa, NULL);
	ec_ds_ptr_node_t* nc = ec_ds_ptr_make_set(&pc, NULL);
	ec_ds_ptr_union_by_rank_weighted(nc, na, 2.5);
	assert(ec_ds_ptr_find(na) == ec_ds_ptr_find(nc));
}

static void
test_union_by_rank_weighted_idempotent(void)
{
	int a = 1, b = 2;
	void *pa = &a, *pb = &b;
	ec_ds_ptr_node_t* na = ec_ds_ptr_make_set(&pa, NULL);
	ec_ds_ptr_node_t* nb = ec_ds_ptr_make_set(&pb, NULL);
	ec_ds_ptr_union_by_rank_weighted(na, nb, 2.0);
	ec_ds_ptr_union_by_rank_weighted(na, nb, 3.0);
	assert(ec_ds_ptr_find(na) == ec_ds_ptr_find(nb));
}

static void
test_union_by_size_pos(void)
{
	int a = 1, b = 2;
	void *pa = &a, *pb = &b;
	ec_ds_ptr_node_t* na = ec_ds_ptr_make_set(&pa, NULL);
	ec_ds_ptr_node_t* nb = ec_ds_ptr_make_set(&pb, NULL);
	ec_ds_ptr_union_by_size(na, nb);
	assert(ec_ds_ptr_find(na) == ec_ds_ptr_find(nb));
}

static void
test_union_by_size_idempotent(void)
{
	int a = 1, b = 2;
	void *pa = &a, *pb = &b;
	ec_ds_ptr_node_t* na = ec_ds_ptr_make_set(&pa, NULL);
	ec_ds_ptr_node_t* nb = ec_ds_ptr_make_set(&pb, NULL);
	ec_ds_ptr_union_by_size(na, nb);
	ec_ds_ptr_union_by_size(na, nb);
	assert(ec_ds_ptr_find(na) == ec_ds_ptr_find(nb));
}

static void
test_union_by_size_weighted_pos(void)
{
	int a = 1, c = 3;
	void *pa = &a, *pc = &c;
	ec_ds_ptr_node_t* na = ec_ds_ptr_make_set(&pa, NULL);
	ec_ds_ptr_node_t* nc = ec_ds_ptr_make_set(&pc, NULL);
	ec_ds_ptr_union_by_size_weighted(nc, na, 2.5);
	assert(ec_ds_ptr_find(na) == ec_ds_ptr_find(nc));
}

static void
test_union_by_size_weighted_idempotent(void)
{
	int a = 1, c = 3;
	void *pa = &a, *pc = &c;
	ec_ds_ptr_node_t* na = ec_ds_ptr_make_set(&pa, NULL);
	ec_ds_ptr_node_t* nc = ec_ds_ptr_make_set(&pc, NULL);
	ec_ds_ptr_union_by_size_weighted(nc, na, 2.0);
	ec_ds_ptr_union_by_size_weighted(nc, na, 3.0);
	assert(ec_ds_ptr_find(na) == ec_ds_ptr_find(nc));
}

static void
test_calc_connected_pos(void)
{
	int a = 1, c = 3;
	void *pa = &a, *pc = &c;
	ec_ds_ptr_node_t* na = ec_ds_ptr_make_set(&pa, NULL);
	ec_ds_ptr_node_t* nc = ec_ds_ptr_make_set(&pc, NULL);
	ec_ds_ptr_union_by_rank_weighted(nc, na, 2.5);
	assert(ec_ds_ptr_calc(na, nc) != -1.0);
}

int
main(void)
{
	printf("Starting ec_ds_ptr tests...\n");
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
	printf("All ec_ds_ptr tests passed.\n");
	return 0;
}
