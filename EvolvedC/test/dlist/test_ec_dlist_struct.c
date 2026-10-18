#include "../../src/ec_dlist.h"

typedef struct
{
	int id;
} rec_t;
EC_DLIST_DEF(rec_t*, recp)
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

static int
cmp_recp(const void* a, const void* b)
{
	return ((*(rec_t**)a)->id - (*(rec_t**)b)->id);
}

static bool
is_valid_recp(const void* x)
{
	return (((*(rec_t**)x)->id) % 2) == 0;
}

static void
inc_recp(void* x)
{
	((*(rec_t**)x)->id)++;
}

static void
add_recps(void* a, void* b, void* out)
{
	(*(rec_t**)out)->id = ((*(rec_t**)a)->id) + ((*(rec_t**)b)->id);
}

static void
mul_recps(void* a, void* b, void* out)
{
	(*(rec_t**)out)->id = ((*(rec_t**)a)->id) * ((*(rec_t**)b)->id);
}

static void*
dupe_recp(void* src)
{
	rec_t** ss = (rec_t**)src;
	rec_t** xx = (rec_t**)malloc(sizeof(rec_t*));
	*xx = *ss;
	return xx;
}

static int callback_counter = 0;

static void
callback_recp(void* x)
{
	(void)x;
	callback_counter++;
}

static void
test_init_free_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	assert(l.size_fn(&l) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_add_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 10 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 20 };
	rec_t* b = &rec_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.size_fn(&l) == 2);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_add_at_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_x = { 1 };
	rec_t* x = &rec_x;
	rec_t rec_y = { 2 };
	rec_t* y = &rec_y;
	rec_t rec_z = { 3 };
	rec_t* z = &rec_z;
	assert(l.add(&l, &x) == EC_OK);
	assert(l.add(&l, &z) == EC_OK);
	assert(l.add_at(&l, &y, 1) == EC_OK);
	rec_t** out;
	assert(l.get_at(&l, 1, (void**)&out) == EC_OK && (*(rec_t**)out)->id == 2);
	assert(l.size_fn(&l) == 3);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_add_at_neg_oob(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_x = { 1 };
	rec_t rec_y = { 2 };
	rec_t *x = &rec_x, *y = &rec_y;
	assert(l.add(&l, &x) == EC_OK);
	assert(l.add_at(&l, &y, 5) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 10 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 20 };
	rec_t* b = &rec_b;
	rec_t rec_c = { 30 };
	rec_t* c = &rec_c;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	// remove_fn uses pointer equality, so we need to pass the same pointer that was added
	assert(l.remove_fn(&l, &b) == EC_OK);
	assert(l.size_fn(&l) == 2);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_neg_not_found(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 10 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 20 };
	rec_t* b = &rec_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	rec_t rec_v = { 99 };
	rec_t* v = &rec_v;
	assert(l.remove_fn(&l, &v) == EC_ERR_NOT_FOUND);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_first_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 11 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 22 };
	rec_t* b = &rec_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.remove_first(&l) == EC_OK);
	assert(l.size_fn(&l) == 1);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_first_neg_empty(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	assert(l.remove_first(&l) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_last_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 33 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 44 };
	rec_t* b = &rec_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.remove_last(&l) == EC_OK);
	assert(l.size_fn(&l) == 1);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_last_neg_empty(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	assert(l.remove_last(&l) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_at_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 10 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 20 };
	rec_t* b = &rec_b;
	rec_t rec_c = { 30 };
	rec_t* c = &rec_c;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	assert(l.remove_at(&l, 1) == EC_OK);
	assert(l.size_fn(&l) == 2);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_at_neg_oob(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 10 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 20 };
	rec_t* b = &rec_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.remove_at(&l, 5) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_at_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 10 };
	rec_t rec_b = { 200 };
	rec_t *a = &rec_a, *b = &rec_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	rec_t** out = NULL;
	assert(l.get_at(&l, 1, (void**)&out) == EC_OK && (*(rec_t**)out)->id == 200);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_at_neg_oob(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 10 };
	rec_t rec_b = { 200 };
	rec_t *a = &rec_a, *b = &rec_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	rec_t** out = NULL;
	assert(l.get_at(&l, 2, (void**)&out) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_first_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 777 };
	rec_t* a = &rec_a;
	assert(l.add(&l, &a) == EC_OK);
	rec_t** out = NULL;
	assert(l.get_first(&l, (void**)&out) == EC_OK && (*(rec_t**)out)->id == 777);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_first_neg_empty(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t** out = NULL;
	assert(l.get_first(&l, (void**)&out) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_last_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 10 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 20 };
	rec_t* b = &rec_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	rec_t** out;
	assert(l.get_last(&l, (void**)&out) == EC_OK && (*(rec_t**)out)->id == 20);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_last_neg_empty(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t** out;
	assert(l.get_last(&l, (void**)&out) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_splice_pos(void)
{
	ec_dlist_recp_t l1, l2;
	assert(ec_dlist_recp_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_recp_init(&l2, NULL) == EC_OK);
	rec_t rec_a = { 10 };
	rec_t rec_b = { 20 }, rec_c = { 30 }, rec_d = { 40 };
	rec_t *a = &rec_a, *b = &rec_b, *c = &rec_c, *d = &rec_d;
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l1.add(&l1, &b) == EC_OK);
	assert(l2.add(&l2, &c) == EC_OK);
	assert(l2.add(&l2, &d) == EC_OK);
	assert(l1.splice(&l1, &l2) == EC_OK);
	assert(l1.size_fn(&l1) == 4);
	assert(l2.size_fn(&l2) == 0);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

static void
test_splice_neg_null(void)
{
	ec_dlist_recp_t l1;
	assert(ec_dlist_recp_init(&l1, NULL) == EC_OK);
	assert(l1.splice(&l1, NULL) == EC_ERR_INVALID_ARG);
	assert(l1.free_fn(&l1) == EC_OK);
}

static void
test_splice_at_pos(void)
{
	ec_dlist_recp_t l1, l2;
	assert(ec_dlist_recp_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_recp_init(&l2, NULL) == EC_OK);
	int a = 1, b = 2, c = 3, d = 4, e = 5;
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l1.add(&l1, &b) == EC_OK);
	assert(l1.add(&l1, &c) == EC_OK);
	assert(l2.add(&l2, &d) == EC_OK);
	assert(l2.add(&l2, &e) == EC_OK);
	assert(l1.splice_at(&l1, &l2, 1) == EC_OK);
	assert(l1.size_fn(&l1) == 5);
	assert(l2.size_fn(&l2) == 0);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

static void
test_splice_at_neg_oob(void)
{
	ec_dlist_recp_t l1, l2;
	assert(ec_dlist_recp_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_recp_init(&l2, NULL) == EC_OK);
	int a = 1, d = 4;
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l2.add(&l2, &d) == EC_OK);
	assert(l1.splice_at(&l1, &l2, 5) == EC_ERR_OUT_OF_BOUNDS);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

static void
test_add_all_pos(void)
{
	ec_dlist_recp_t l1, l2;
	assert(ec_dlist_recp_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_recp_init(&l2, NULL) == EC_OK);
	rec_t rec_a = { 10 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 20 };
	rec_t* b = &rec_b;
	assert(l2.add(&l2, &a) == EC_OK);
	assert(l2.add(&l2, &b) == EC_OK);
	assert(l1.add_all(&l1, &l2) == EC_OK);
	assert(l1.size_fn(&l1) == 2);
	assert(l2.size_fn(&l2) == 2);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

static void
test_add_all_at_pos(void)
{
	ec_dlist_recp_t l1, l2;
	assert(ec_dlist_recp_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_recp_init(&l2, NULL) == EC_OK);
	rec_t rec_a = { 10 };
	rec_t rec_b = { 30 };
	rec_t rec_c = { 20 };
	rec_t rec_d = { 25 };
	rec_t *a = &rec_a, *b = &rec_b, *c = &rec_c, *d = &rec_d;
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l1.add(&l1, &b) == EC_OK);
	assert(l2.add(&l2, &c) == EC_OK);
	assert(l2.add(&l2, &d) == EC_OK);
	assert(l1.add_all_at(&l1, &l2, 1) == EC_OK);
	assert(l1.size_fn(&l1) == 4);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

static void
test_sublist_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	for (int i = 0; i < 5; i++)
		{
			int v = (i + 1) * 10;
			assert(l.add(&l, &v) == EC_OK);
		}
	ec_dlist_recp_t* sub = NULL;
	assert(l.sublist(&l, 1, 4, &sub) == EC_OK);
	assert(sub->size_fn(sub) == 3);
	rec_t exp_recs[] = { { 20 }, { 30 }, { 40 } };
	rec_t* exp[3];
	for (int i = 0; i < 3; i++)
		exp[i] = &exp_recs[i];
	for (size_t i = 0; i < 3; i++)
		{
			rec_t** out = NULL;
			assert(sub->get_at(sub, i, (void**)&out) == EC_OK && (*(rec_t**)out)->id == exp[i]->id);
		}
	assert(sub->free_fn(sub) == EC_OK);
	free(sub);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_sublist_neg_oob(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 10 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 20 };
	rec_t* b = &rec_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	ec_dlist_recp_t* sub = NULL;
	assert(l.sublist(&l, 1, 10, &sub) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_copy_shallow_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	for (int i = 1; i <= 3; i++)
		{
			int v = i;
			assert(l.add(&l, &v) == EC_OK);
		}
	ec_dlist_recp_t* cp = NULL;
	assert(l.copy_shallow(&l, &cp) == EC_OK);
	assert(cp->size_fn(cp) == 3);
	assert(cp->free_fn(cp) == EC_OK);
	free(cp);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_copy_deep_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	for (int i = 1; i <= 3; i++)
		{
			int v = i * 10;
			assert(l.add(&l, &v) == EC_OK);
		}
	ec_dlist_recp_t* cp = NULL;
	assert(l.copy_deep(&l, dupe_recp, &cp) == EC_OK);
	assert(cp->size_fn(cp) == 3);
	assert(cp->free_fn(cp) == EC_OK);
	free(cp);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_replace_at_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 5 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 10 };
	rec_t* b = &rec_b;
	rec_t rec_c = { 99 };
	rec_t* c = &rec_c;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.replace_at(&l, &c, 1) == EC_OK);
	rec_t** out = NULL;
	assert(l.get_at(&l, 1, (void**)&out) == EC_OK && (*(rec_t**)out)->id == 99);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_replace_at_neg_oob(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 5 };
	rec_t rec_c = { 99 };
	rec_t *a = &rec_a, *c = &rec_c;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.replace_at(&l, &c, 5) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_contains_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 10 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 20 };
	rec_t* b = &rec_b;
	rec_t rec_c = { 30 };
	rec_t* c = &rec_c;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	int x = 20;
	assert(l.contains(&l, &x) == 1);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_contains_neg_not_found(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 10 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 20 };
	rec_t* b = &rec_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	int x = 99;
	assert(l.contains(&l, &x) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_contains_value_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 10 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 20 };
	rec_t* b = &rec_b;
	rec_t rec_c = { 30 };
	rec_t* c = &rec_c;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	int x = 30;
	assert(l.contains_value(&l, &x, cmp_recp) == 1);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_index_of_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	for (int i = 1; i <= 5; i++)
		{
			int v = i * 10;
			assert(l.add(&l, &v) == EC_OK);
		}
	int x = 40;
	size_t idx;
	assert(l.index_of(&l, &x, cmp_recp, &idx) == EC_OK);
	assert(idx == 3);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_index_of_neg_not_found(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	for (int i = 1; i <= 5; i++)
		{
			int v = i * 10;
			assert(l.add(&l, &v) == EC_OK);
		}
	int x = 99;
	size_t idx;
	assert(l.index_of(&l, &x, cmp_recp, &idx) == EC_ERR_NOT_FOUND);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_to_array_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	int a = 1, b = 2, c = 3;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	void** arr = NULL;
	assert(l.to_array(&l, &arr) == EC_OK);
	assert((*(rec_t**)arr)[0].id == 1 && (*(rec_t**)arr)[1].id == 2 && (*(rec_t**)arr)[2].id == 3);
	free(arr);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_reverse_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 10 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 20 };
	rec_t* b = &rec_b;
	rec_t rec_c = { 30 };
	rec_t* c = &rec_c;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	l.reverse(&l);
	rec_t** out;
	assert(l.get_at(&l, 0, (void**)&out) == EC_OK && (*(rec_t**)out)->id == 30);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_sort_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	int a = 30, b = 10, c = 20;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	assert(l.sort(&l, cmp_recp) == EC_OK);
	rec_t** out;
	assert(l.get_at(&l, 0, (void**)&out) == EC_OK && (*(rec_t**)out)->id == 10);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_foreach_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 5 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 6 };
	rec_t* b = &rec_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	l.foreach (&l, inc_recp);
	rec_t** out;
	assert(l.get_at(&l, 0, (void**)&out) == EC_OK && (*(rec_t**)out)->id == 6);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_reduce_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	for (int i = 1; i <= 4; i++)
		{
			int v = i;
			assert(l.add(&l, &v) == EC_OK);
		}
	rec_t rec_sum = { 0 };
	rec_t* sum = &rec_sum;
	assert(l.reduce(&l, add_recps, &sum) == EC_OK);
	assert(sum->id == 10);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_filter_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t recs[6] = { { 10 }, { 15 }, { 20 }, { 25 }, { 30 }, { 35 } };
	rec_t* arr[6];
	for (int i = 0; i < 6; i++)
		arr[i] = &recs[i];
	for (int i = 0; i < 6; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_recp_t* fl = NULL;
	assert(l.filter(&l, is_valid_recp, &fl) == EC_OK);
	assert(fl->size_fn(fl) == 3);
	assert(fl->free_fn(fl) == EC_OK);
	free(fl);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_filter_mut_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t recs[6] = { { 10 }, { 15 }, { 20 }, { 25 }, { 30 }, { 35 } };
	rec_t* arr[6];
	for (int i = 0; i < 6; i++)
		arr[i] = &recs[i];
	for (int i = 0; i < 6; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	assert(l.filter_mut(&l, is_valid_recp) == EC_OK);
	assert(l.size_fn(&l) == 3);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_next_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	for (int i = 0; i < 5; i++)
		{
			int v = i + 100;
			assert(l.add(&l, &v) == EC_OK);
		}
	ec_dlist_recp_iter_t it;
	l.iter_init(&it, &l);
	rec_t** out;
	size_t count = 0;
	while (l.iter_next(&it, &out) == EC_OK)
		{
			assert((*(rec_t**)out)->id >= 100);
			count++;
		}
	assert(count == 5);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_remove_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	for (int i = 0; i < 5; i++)
		{
			int v = i + 100;
			assert(l.add(&l, &v) == EC_OK);
		}
	ec_dlist_recp_iter_t it;
	l.iter_init(&it, &l);
	rec_t** out;
	assert(l.iter_next(&it, &out) == EC_OK);
	assert(l.iter_remove(&it) == EC_OK);
	assert(l.size_fn(&l) == 4);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_add_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	for (int i = 0; i < 3; i++)
		{
			int v = i + 100;
			assert(l.add(&l, &v) == EC_OK);
		}
	ec_dlist_recp_iter_t it;
	l.iter_init(&it, &l);
	rec_t** out;
	assert(l.iter_next(&it, &out) == EC_OK);
	int x = 999;
	assert(l.iter_add(&it, &x) == EC_OK);
	assert(l.size_fn(&l) == 4);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_replace_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	for (int i = 0; i < 3; i++)
		{
			int v = i + 100;
			assert(l.add(&l, &v) == EC_OK);
		}
	ec_dlist_recp_iter_t it;
	l.iter_init(&it, &l);
	rec_t** out;
	assert(l.iter_next(&it, &out) == EC_OK);
	rec_t rec_y = { 777 };
	rec_t* y = &rec_y;
	assert(l.iter_replace(&it, &y, NULL) == EC_OK);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_zip_iter_next_pos(void)
{
	ec_dlist_recp_t l1, l2;
	assert(ec_dlist_recp_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_recp_init(&l2, NULL) == EC_OK);
	for (int i = 0; i < 5; i++)
		{
			int aa = i + 1, bb = (i + 1) * 10;
			assert(l1.add(&l1, &aa) == EC_OK);
			assert(l2.add(&l2, &bb) == EC_OK);
		}
	ec_dlist_recp_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	size_t c = 0;
	while (l1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			int *r1 = (int*)out1, *r2 = (int*)out2;
			assert((*r2) == ((*r1) * 10));
			c++;
		}
	assert(c == 5);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

static void
test_remove_all_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 10 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 20 };
	rec_t* b = &rec_b;
	rec_t rec_c = { 30 };
	rec_t* c = &rec_c;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	assert(l.size_fn(&l) == 3);
	assert(l.remove_all(&l) == EC_OK);
	assert(l.size_fn(&l) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_all_neg_empty(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	assert(l.remove_all(&l) == EC_OK);
	assert(l.size_fn(&l) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_all_cb_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	rec_t rec_a = { 10 };
	rec_t* a = &rec_a;
	rec_t rec_b = { 20 };
	rec_t* b = &rec_b;
	rec_t rec_c = { 30 };
	rec_t* c = &rec_c;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	assert(l.size_fn(&l) == 3);
	callback_counter = 0;
	assert(l.remove_all_cb(&l, callback_recp) == EC_OK);
	assert(callback_counter == 3);
	assert(l.size_fn(&l) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_all_cb_neg_empty(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	callback_counter = 0;
	assert(l.remove_all_cb(&l, callback_recp) == EC_OK);
	assert(callback_counter == 0);
	assert(l.size_fn(&l) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_sort_in_place_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	int a = 30, b = 10, c = 20;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	l.sort_in_place(&l, cmp_recp);
	rec_t** out;
	assert(l.get_at(&l, 0, (void**)&out) == EC_OK && (*(rec_t**)out)->id == 10);
	assert(l.get_at(&l, 1, (void**)&out) == EC_OK && (*(rec_t**)out)->id == 20);
	assert(l.get_at(&l, 2, (void**)&out) == EC_OK && (*(rec_t**)out)->id == 30);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_index_pos(void)
{
	ec_dlist_recp_t l;
	assert(ec_dlist_recp_init(&l, NULL) == EC_OK);
	for (int i = 0; i < 5; i++)
		{
			int v = i + 100;
			assert(l.add(&l, &v) == EC_OK);
		}
	ec_dlist_recp_iter_t it;
	l.iter_init(&it, &l);
	rec_t** out;
	size_t idx = 0;
	while (l.iter_next(&it, &out) == EC_OK)
		{
			assert(l.iter_index(&it) == idx);
			idx++;
		}
	assert(idx == 5);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_zip_iter_init_pos(void)
{
	ec_dlist_recp_t l1, l2;
	assert(ec_dlist_recp_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_recp_init(&l2, NULL) == EC_OK);
	int a = 1, b = 2;
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l2.add(&l2, &b) == EC_OK);
	ec_dlist_recp_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	assert(zit.l1 == &l1);
	assert(zit.l2 == &l2);
	assert(zit.idx == 0);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

static void
test_zip_iter_index_pos(void)
{
	ec_dlist_recp_t l1, l2;
	assert(ec_dlist_recp_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_recp_init(&l2, NULL) == EC_OK);
	for (int i = 0; i < 5; i++)
		{
			int aa = i + 1, bb = (i + 1) * 10;
			assert(l1.add(&l1, &aa) == EC_OK);
			assert(l2.add(&l2, &bb) == EC_OK);
		}
	ec_dlist_recp_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	size_t idx = 0;
	while (l1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			assert(l1.zip_iter_index(&zit) == idx);
			idx++;
		}
	assert(idx == 5);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

static void
test_zip_iter_add_neg_not_supported(void)
{
	ec_dlist_recp_t l1, l2;
	assert(ec_dlist_recp_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_recp_init(&l2, NULL) == EC_OK);
	int a = 1, b = 2;
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l2.add(&l2, &b) == EC_OK);
	ec_dlist_recp_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	rec_t rec_x = { 1 };
	rec_t rec_y = { 20 };
	rec_t *x = &rec_x, *y = &rec_y;
	assert(l1.zip_iter_add(&zit, &x, &y) == EC_ERR_NOT_SUPPORTED);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

static void
test_zip_iter_remove_neg_not_supported(void)
{
	ec_dlist_recp_t l1, l2;
	assert(ec_dlist_recp_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_recp_init(&l2, NULL) == EC_OK);
	int a = 1, b = 2;
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l2.add(&l2, &b) == EC_OK);
	ec_dlist_recp_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	assert(l1.zip_iter_next(&zit, &out1, &out2) == EC_OK);
	assert(l1.zip_iter_remove(&zit, &out1, &out2) == EC_ERR_NOT_SUPPORTED);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

static void
test_zip_iter_replace_neg_not_supported(void)
{
	ec_dlist_recp_t l1, l2;
	assert(ec_dlist_recp_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_recp_init(&l2, NULL) == EC_OK);
	int a = 1, b = 2;
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l2.add(&l2, &b) == EC_OK);
	ec_dlist_recp_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	assert(l1.zip_iter_next(&zit, &out1, &out2) == EC_OK);
	rec_t rec_x = { 1 };
	rec_t rec_y = { 20 };
	rec_t *x = &rec_x, *y = &rec_y;
	void *old1, *old2;
	assert(l1.zip_iter_replace(&zit, &x, &y, &old1, &old2) == EC_ERR_NOT_SUPPORTED);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

int
main(void)
{
	printf("Starting ec_dlist_recp tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	assert(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif
	test_init_free_pos();
	test_add_pos();
	test_add_at_pos();
	test_add_at_neg_oob();
	test_remove_pos();
	test_remove_neg_not_found();
	test_remove_first_pos();
	test_remove_first_neg_empty();
	test_remove_last_pos();
	test_remove_last_neg_empty();
	test_remove_at_pos();
	test_remove_at_neg_oob();
	test_get_at_pos();
	test_get_at_neg_oob();
	test_get_first_pos();
	test_get_first_neg_empty();
	test_get_last_pos();
	test_get_last_neg_empty();
	test_splice_pos();
	test_splice_neg_null();
	test_splice_at_pos();
	test_splice_at_neg_oob();
	test_add_all_pos();
	test_add_all_at_pos();
	test_sublist_pos();
	test_sublist_neg_oob();
	test_copy_shallow_pos();
	test_copy_deep_pos();
	test_replace_at_pos();
	test_replace_at_neg_oob();
	test_contains_pos();
	test_contains_neg_not_found();
	test_contains_value_pos();
	test_index_of_pos();
	test_index_of_neg_not_found();
	test_to_array_pos();
	test_reverse_pos();
	test_sort_pos();
	test_foreach_pos();
	test_reduce_pos();
	test_filter_pos();
	test_filter_mut_pos();
	test_iter_next_pos();
	test_iter_remove_pos();
	test_iter_add_pos();
	test_iter_replace_pos();
	test_iter_index_pos();
	test_remove_all_pos();
	test_remove_all_neg_empty();
	test_remove_all_cb_pos();
	test_remove_all_cb_neg_empty();
	test_sort_in_place_pos();
	test_zip_iter_next_pos();
	test_zip_iter_init_pos();
	test_zip_iter_index_pos();
	test_zip_iter_add_neg_not_supported();
	test_zip_iter_remove_neg_not_supported();
	test_zip_iter_replace_neg_not_supported();
#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif
	printf("All ec_dlist_recp tests passed.\n");
	return 0;
}
