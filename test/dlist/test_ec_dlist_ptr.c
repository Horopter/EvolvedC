#include <stdint.h>
#include "../../src/ec_dlist.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

static int
cmp_ptr(const void* a, const void* b)
{
	return (*(void**)a - *(void**)b);
}

static bool
is_non_null(const void* x)
{
	return ((*(void**)x) % 2) == 0;
}

static void
inc_ptr(void* x)
{
	(*(void**)x)++;
}

static void
add_ptrs(void* a, void* b, void* out)
{
	*(void**)out = (*(void**)a) + (*(void**)b);
}

static void
mul_ptrs(void* a, void* b, void* out)
{
	*(void**)out = (*(void**)a) * (*(void**)b);
}

static void*
dupe_ptr(void* src)
{
	void** ss = (void**)src;
	void** xx = (void**)malloc(sizeof(void*));
	*xx = *ss;
	return xx;
}

static int callback_counter = 0;

static void
callback_ptr(void* x)
{
	(void)x;
	callback_counter++;
}

static void
test_init_free_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	assert(l.size_fn(&l) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_add_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 10;
	void* a = &val_a;
	int val_b = 20;
	void* b = &val_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.size_fn(&l) == 2);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_add_at_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_x = 1;
	void* x = &val_x;
	int val_y = 2;
	void* y = &val_y;
	int val_z = 3;
	void* z = &val_z;
	assert(l.add(&l, &x) == EC_OK);
	assert(l.add(&l, &z) == EC_OK);
	assert(l.add_at(&l, &y, 1) == EC_OK);
	void** out;
	assert(l.get_at(&l, 1, (void**)&out) == EC_OK && *out == y);
	assert(l.size_fn(&l) == 3);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_add_at_neg_oob(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_x = 1;
	void *x = &val_x, y = 2;
	assert(l.add(&l, &x) == EC_OK);
	assert(l.add_at(&l, &y, 5) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 10;
	void* a = &val_a;
	int val_b = 20;
	void* b = &val_b;
	int val_c = 30;
	void* c = &val_c;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	int val_v = 20;
	void* v = &val_v;
	assert(l.remove_fn(&l, &v) == EC_OK);
	assert(l.size_fn(&l) == 2);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_neg_not_found(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 10;
	void* a = &val_a;
	int val_b = 20;
	void* b = &val_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	int val_v = 99;
	void* v = &val_v;
	assert(l.remove_fn(&l, &v) == EC_ERR_NOT_FOUND);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_first_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 11;
	void* a = &val_a;
	int val_b = 22;
	void* b = &val_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.remove_first(&l) == EC_OK);
	assert(l.size_fn(&l) == 1);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_first_neg_empty(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	assert(l.remove_first(&l) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_last_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 33;
	void* a = &val_a;
	int val_b = 44;
	void* b = &val_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.remove_last(&l) == EC_OK);
	assert(l.size_fn(&l) == 1);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_last_neg_empty(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	assert(l.remove_last(&l) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_at_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 10;
	void* a = &val_a;
	int val_b = 20;
	void* b = &val_b;
	int val_c = 30;
	void* c = &val_c;
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
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 10;
	void* a = &val_a;
	int val_b = 20;
	void* b = &val_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.remove_at(&l, 5) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_at_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 10;
	void* a = &val_a;
	int val_b = 200;
	void* b = &val_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	void* out = NULL;
	assert(l.get_at(&l, 1, (void**)&out) == EC_OK && *out == a);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_at_neg_oob(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 10;
	void* a = &val_a;
	int val_b = 200;
	void* b = &val_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	void* out = NULL;
	assert(l.get_at(&l, 2, (void**)&out) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_first_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 777;
	void* a = &val_a;
	assert(l.add(&l, &a) == EC_OK);
	void* out = NULL;
	assert(l.get_first(&l, (void**)&out) == EC_OK && *out == a);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_first_neg_empty(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	void* out = NULL;
	assert(l.get_first(&l, (void**)&out) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_last_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 10;
	void* a = &val_a;
	int val_b = 20;
	void* b = &val_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	void** out;
	assert(l.get_last(&l, (void**)&out) == EC_OK && *out == a);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_last_neg_empty(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	void** out;
	assert(l.get_last(&l, (void**)&out) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_splice_pos(void)
{
	ec_dlist_ptr_t l1, l2;
	assert(ec_dlist_ptr_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ptr_init(&l2, NULL) == EC_OK);
	int val_a = 10;
	void *a = &val_a, b = 20, c = 30, d = 40;
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
	ec_dlist_ptr_t l1;
	assert(ec_dlist_ptr_init(&l1, NULL) == EC_OK);
	assert(l1.splice(&l1, NULL) == EC_ERR_INVALID_ARG);
	assert(l1.free_fn(&l1) == EC_OK);
}

static void
test_splice_at_pos(void)
{
	ec_dlist_ptr_t l1, l2;
	assert(ec_dlist_ptr_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ptr_init(&l2, NULL) == EC_OK);
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
	ec_dlist_ptr_t l1, l2;
	assert(ec_dlist_ptr_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ptr_init(&l2, NULL) == EC_OK);
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
	ec_dlist_ptr_t l1, l2;
	assert(ec_dlist_ptr_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ptr_init(&l2, NULL) == EC_OK);
	int val_a = 10;
	void* a = &val_a;
	int val_b = 20;
	void* b = &val_b;
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
	ec_dlist_ptr_t l1, l2;
	assert(ec_dlist_ptr_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ptr_init(&l2, NULL) == EC_OK);
	int val_a = 10;
	void *a = &val_a, b = 30, c = 20, d = 25;
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
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	for (int i = 0; i < 5; i++)
		{
			int v = (i + 1) * 10;
			assert(l.add(&l, &v) == EC_OK);
		}
	ec_dlist_ptr_t* sub = NULL;
	assert(l.sublist(&l, 1, 4, &sub) == EC_OK);
	assert(sub->size_fn(sub) == 3);
	int exp_vals[] = { 20, 30, 40 };
	void* exp[3];
	for (int i = 0; i < 3; i++)
		exp[i] = &exp_vals[i];
	for (size_t i = 0; i < 3; i++)
		{
			void* out = NULL;
			assert(sub->get_at(sub, i, &out) == EC_OK && *(void**)out == exp[i]);
		}
	assert(sub->free_fn(sub) == EC_OK);
	free(sub);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_sublist_neg_oob(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 10;
	void* a = &val_a;
	int val_b = 20;
	void* b = &val_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	ec_dlist_ptr_t* sub = NULL;
	assert(l.sublist(&l, 1, 10, &sub) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_copy_shallow_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	for (int i = 1; i <= 3; i++)
		{
			int v = i;
			assert(l.add(&l, &v) == EC_OK);
		}
	ec_dlist_ptr_t* cp = NULL;
	assert(l.copy_shallow(&l, &cp) == EC_OK);
	assert(cp->size_fn(cp) == 3);
	assert(cp->free_fn(cp) == EC_OK);
	free(cp);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_copy_deep_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	for (int i = 1; i <= 3; i++)
		{
			int v = i * 10;
			assert(l.add(&l, &v) == EC_OK);
		}
	ec_dlist_ptr_t* cp = NULL;
	assert(l.copy_deep(&l, dupe_ptr, &cp) == EC_OK);
	assert(cp->size_fn(cp) == 3);
	assert(cp->free_fn(cp) == EC_OK);
	free(cp);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_replace_at_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 5;
	void* a = &val_a;
	int val_b = 10;
	void* b = &val_b;
	int val_c = 99;
	void* c = &val_c;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.replace_at(&l, &c, 1) == EC_OK);
	void* out = NULL;
	assert(l.get_at(&l, 1, (void**)&out) == EC_OK && *out == a);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_replace_at_neg_oob(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 5;
	void *a = &val_a, c = 99;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.replace_at(&l, &c, 5) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_contains_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 10;
	void* a = &val_a;
	int val_b = 20;
	void* b = &val_b;
	int val_c = 30;
	void* c = &val_c;
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
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 10;
	void* a = &val_a;
	int val_b = 20;
	void* b = &val_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	int x = 99;
	assert(l.contains(&l, &x) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_contains_value_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 10;
	void* a = &val_a;
	int val_b = 20;
	void* b = &val_b;
	int val_c = 30;
	void* c = &val_c;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	int x = 30;
	assert(l.contains_value(&l, &x, cmp_ptr) == 1);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_index_of_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	for (int i = 1; i <= 5; i++)
		{
			int v = i * 10;
			assert(l.add(&l, &v) == EC_OK);
		}
	int val_x = 40;
	void* x = &val_x;
	size_t idx;
	assert(l.index_of(&l, &x, cmp_ptr, &idx) == EC_OK);
	assert(idx == 3);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_index_of_neg_not_found(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	for (int i = 1; i <= 5; i++)
		{
			int v = i * 10;
			assert(l.add(&l, &v) == EC_OK);
		}
	int x = 99;
	size_t idx;
	assert(l.index_of(&l, &x, cmp_ptr, &idx) == EC_ERR_NOT_FOUND);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_to_array_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int a = 1, b = 2, c = 3;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	void** arr = NULL;
	assert(l.to_array(&l, &arr) == EC_OK);
	assert(*(void**)arr[0] == 1 && *(void**)arr[1] == 2 && *(void**)arr[2] == 3);
	free(arr);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_reverse_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 10;
	void* a = &val_a;
	int val_b = 20;
	void* b = &val_b;
	int val_c = 30;
	void* c = &val_c;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	l.reverse(&l);
	void** out;
	assert(l.get_at(&l, 0, (void**)&out) == EC_OK && *out == c);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_sort_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int a = 30, b = 10, c = 20;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	assert(l.sort(&l, cmp_ptr) == EC_OK);
	void** out;
	assert(l.get_at(&l, 0, (void**)&out) == EC_OK && *out == c);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_foreach_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 5;
	void* a = &val_a;
	int val_b = 6;
	void* b = &val_b;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	l.foreach (&l, inc_ptr);
	void** out;
	assert(l.get_at(&l, 0, (void**)&out) == EC_OK && *out != NULL);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_reduce_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	for (int i = 1; i <= 4; i++)
		{
			int v = i;
			assert(l.add(&l, &v) == EC_OK);
		}
	void* sum = NULL;
	assert(l.reduce(&l, add_ptrs, &sum) == EC_OK);
	assert(sum == &val_a);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_filter_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int vals[6] = { 10, 15, 20, 25, 30, 35 };
	void* arr[6];
	for (int i = 0; i < 6; i++)
		arr[i] = &vals[i];
	for (int i = 0; i < 6; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_ptr_t* fl = NULL;
	assert(l.filter(&l, is_non_null, &fl) == EC_OK);
	assert(fl->size_fn(fl) == 3);
	assert(fl->free_fn(fl) == EC_OK);
	free(fl);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_filter_mut_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int vals[6] = { 10, 15, 20, 25, 30, 35 };
	void* arr[6];
	for (int i = 0; i < 6; i++)
		arr[i] = &vals[i];
	for (int i = 0; i < 6; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	assert(l.filter_mut(&l, is_non_null) == EC_OK);
	assert(l.size_fn(&l) == 3);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_next_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	for (int i = 0; i < 5; i++)
		{
			int v = i + 100;
			assert(l.add(&l, &v) == EC_OK);
		}
	ec_dlist_ptr_iter_t it;
	l.iter_init(&it, &l);
	void** out;
	size_t count = 0;
	while (l.iter_next(&it, &out) == EC_OK)
		{
			assert(*(void**)out >= 100);
			count++;
		}
	assert(count == 5);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_remove_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	for (int i = 0; i < 5; i++)
		{
			int v = i + 100;
			assert(l.add(&l, &v) == EC_OK);
		}
	ec_dlist_ptr_iter_t it;
	l.iter_init(&it, &l);
	void** out;
	assert(l.iter_next(&it, &out) == EC_OK);
	assert(l.iter_remove(&it) == EC_OK);
	assert(l.size_fn(&l) == 4);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_add_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	for (int i = 0; i < 3; i++)
		{
			int v = i + 100;
			assert(l.add(&l, &v) == EC_OK);
		}
	ec_dlist_ptr_iter_t it;
	l.iter_init(&it, &l);
	void** out;
	assert(l.iter_next(&it, &out) == EC_OK);
	int x = 999;
	assert(l.iter_add(&it, &x) == EC_OK);
	assert(l.size_fn(&l) == 4);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_replace_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	for (int i = 0; i < 3; i++)
		{
			int v = i + 100;
			assert(l.add(&l, &v) == EC_OK);
		}
	ec_dlist_ptr_iter_t it;
	l.iter_init(&it, &l);
	void** out;
	assert(l.iter_next(&it, &out) == EC_OK);
	int val_y = 777;
	void* y = &val_y;
	assert(l.iter_replace(&it, &y, NULL) == EC_OK);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_zip_iter_next_pos(void)
{
	ec_dlist_ptr_t l1, l2;
	assert(ec_dlist_ptr_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ptr_init(&l2, NULL) == EC_OK);
	for (int i = 0; i < 5; i++)
		{
			int aa = i + 1, bb = (i + 1) * 10;
			assert(l1.add(&l1, &aa) == EC_OK);
			assert(l2.add(&l2, &bb) == EC_OK);
		}
	ec_dlist_ptr_zip_iter_t zit;
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
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 10;
	void* a = &val_a;
	int val_b = 20;
	void* b = &val_b;
	int val_c = 30;
	void* c = &val_c;
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
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	assert(l.remove_all(&l) == EC_OK);
	assert(l.size_fn(&l) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_all_cb_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int val_a = 10;
	void* a = &val_a;
	int val_b = 20;
	void* b = &val_b;
	int val_c = 30;
	void* c = &val_c;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	assert(l.size_fn(&l) == 3);
	callback_counter = 0;
	assert(l.remove_all_cb(&l, callback_ptr) == EC_OK);
	assert(callback_counter == 3);
	assert(l.size_fn(&l) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_all_cb_neg_empty(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	callback_counter = 0;
	assert(l.remove_all_cb(&l, callback_ptr) == EC_OK);
	assert(callback_counter == 0);
	assert(l.size_fn(&l) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_sort_in_place_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	int a = 30, b = 10, c = 20;
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	l.sort_in_place(&l, cmp_ptr);
	void** out;
	assert(l.get_at(&l, 0, (void**)&out) == EC_OK && *out == c);
	assert(l.get_at(&l, 1, (void**)&out) == EC_OK && *out == a);
	assert(l.get_at(&l, 2, (void**)&out) == EC_OK && *out == a);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_index_pos(void)
{
	ec_dlist_ptr_t l;
	assert(ec_dlist_ptr_init(&l, NULL) == EC_OK);
	for (int i = 0; i < 5; i++)
		{
			int v = i + 100;
			assert(l.add(&l, &v) == EC_OK);
		}
	ec_dlist_ptr_iter_t it;
	l.iter_init(&it, &l);
	void** out;
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
	ec_dlist_ptr_t l1, l2;
	assert(ec_dlist_ptr_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ptr_init(&l2, NULL) == EC_OK);
	int a = 1, b = 2;
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l2.add(&l2, &b) == EC_OK);
	ec_dlist_ptr_zip_iter_t zit;
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
	ec_dlist_ptr_t l1, l2;
	assert(ec_dlist_ptr_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ptr_init(&l2, NULL) == EC_OK);
	for (int i = 0; i < 5; i++)
		{
			int aa = i + 1, bb = (i + 1) * 10;
			assert(l1.add(&l1, &aa) == EC_OK);
			assert(l2.add(&l2, &bb) == EC_OK);
		}
	ec_dlist_ptr_zip_iter_t zit;
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
	ec_dlist_ptr_t l1, l2;
	assert(ec_dlist_ptr_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ptr_init(&l2, NULL) == EC_OK);
	int a = 1, b = 2;
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l2.add(&l2, &b) == EC_OK);
	ec_dlist_ptr_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	int val_x = 1;
	void *x = &val_x0, y = 20;
	assert(l1.zip_iter_add(&zit, &x, &y) == EC_ERR_NOT_SUPPORTED);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

static void
test_zip_iter_remove_neg_not_supported(void)
{
	ec_dlist_ptr_t l1, l2;
	assert(ec_dlist_ptr_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ptr_init(&l2, NULL) == EC_OK);
	int a = 1, b = 2;
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l2.add(&l2, &b) == EC_OK);
	ec_dlist_ptr_zip_iter_t zit;
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
	ec_dlist_ptr_t l1, l2;
	assert(ec_dlist_ptr_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ptr_init(&l2, NULL) == EC_OK);
	int a = 1, b = 2;
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l2.add(&l2, &b) == EC_OK);
	ec_dlist_ptr_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	assert(l1.zip_iter_next(&zit, &out1, &out2) == EC_OK);
	int val_x = 1;
	void *x = &val_x0, y = 20;
	void *old1, *old2;
	assert(l1.zip_iter_replace(&zit, &x, &y, &old1, &old2) == EC_ERR_NOT_SUPPORTED);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

int
main(void)
{
	printf("Starting ec_dlist_ptr tests...\n");
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
	printf("All ec_dlist_ptr tests passed.\n");
	return 0;
}
