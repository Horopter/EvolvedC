#include "../../src/ec_dlist.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

// String type uses const char*

static int
cmp_str(const void* a, const void* b)
{
	const char* str_a = *(const char* const*)a;
	const char* str_b = *(const char* const*)b;
	return strcmp(str_a, str_b);
}

static bool
is_long_str(const void* x)
{
	const char* c = *(const char* const*)x;
	return strlen(c) > 1;
}

static void
toupper_str(void* x)
{
	// For const char*, we can't modify in place - just verify it's non-null
	(void)x;
}

static void
concat_strs(void* a, void* b, void* out)
{
	// For string concatenation, we can't easily do pointer arithmetic
	// Just verify both are non-null
	const char* str_a = *(const char* const*)a;
	const char* str_b = *(const char* const*)b;
	*(const char**)out = (str_a && str_b) ? str_a : NULL;
}

static void*
dupe_str(void* src)
{
	const char** ss = (const char**)src;
	const char** xx = (const char**)malloc(sizeof(const char*));
	*xx = *ss;
	return xx;
}

static int callback_counter = 0;

static void
callback_str(void* x)
{
	(void)x;
	callback_counter++;
}

static void
test_init_free_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	assert(l.size_fn(&l) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_add_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.size_fn(&l) == 2);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_add_at_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *x = "a", *y = "b", *z = "c";
	assert(l.add(&l, &x) == EC_OK);
	assert(l.add(&l, &z) == EC_OK);
	assert(l.add_at(&l, &y, 1) == EC_OK);
	const char** out;
	assert(l.get_at(&l, 1, (void**)&out) == EC_OK && *(const char**)out == "b");
	assert(l.size_fn(&l) == 3);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_add_at_neg_oob(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *x = "a", *y = "b";
	assert(l.add(&l, &x) == EC_OK);
	assert(l.add_at(&l, &y, 5) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_add_first_last_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b", *c = "c";
	assert(l.add_first(&l, &a) == EC_OK);
	assert(l.add_last(&l, &b) == EC_OK);
	assert(l.add_last(&l, &c) == EC_OK);
	assert(l.size_fn(&l) == 3);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b", *c = "c";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	const char* v = "b";
	assert(l.remove_fn(&l, &v) == EC_OK);
	assert(l.size_fn(&l) == 2);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_neg_not_found(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	const char* v = "z";
	assert(l.remove_fn(&l, &v) == EC_ERR_NOT_FOUND);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_first_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.remove_first(&l) == EC_OK);
	assert(l.size_fn(&l) == 1);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_first_neg_empty(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	assert(l.remove_first(&l) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_last_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.remove_last(&l) == EC_OK);
	assert(l.size_fn(&l) == 1);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_last_neg_empty(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	assert(l.remove_last(&l) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_at_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b", *c = "c";
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
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.remove_at(&l, 5) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_all_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b", *c = "c";
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
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	assert(l.remove_all(&l) == EC_OK);
	assert(l.size_fn(&l) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_all_cb_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b", *c = "c";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	assert(l.size_fn(&l) == 3);
	callback_counter = 0;
	assert(l.remove_all_cb(&l, callback_str) == EC_OK);
	assert(callback_counter == 3);
	assert(l.size_fn(&l) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_all_cb_neg_empty(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	callback_counter = 0;
	assert(l.remove_all_cb(&l, callback_str) == EC_OK);
	assert(callback_counter == 0);
	assert(l.size_fn(&l) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_at_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	const char** out = NULL;
	assert(l.get_at(&l, 1, (void**)&out) == EC_OK && *(const char**)out == "b");
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_at_neg_oob(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	const char** out = NULL;
	assert(l.get_at(&l, 2, (void**)&out) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_first_last_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b", *c = "c";
	assert(l.add_first(&l, &a) == EC_OK);
	assert(l.add_last(&l, &b) == EC_OK);
	assert(l.add_last(&l, &c) == EC_OK);
	const char** out;
	assert(l.get_first(&l, (void**)&out) == EC_OK && *(const char**)out == "a");
	assert(l.get_last(&l, (void**)&out) == EC_OK && *(const char**)out == "c");
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_first_neg_empty(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char** out;
	assert(l.get_first(&l, (void**)&out) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_first_neg_null(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char* a = "a";
	assert(l.add_first(&l, &a) == EC_OK);
	const char** out;
	assert(l.get_first(NULL, (void**)&out) == EC_ERR_INVALID_ARG);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_last_neg_empty(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char** out;
	assert(l.get_last(&l, (void**)&out) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_splice_pos(void)
{
	ec_dlist_str_t l1, l2;
	assert(ec_dlist_str_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_str_init(&l2, NULL) == EC_OK);
	const char *a = "a", *b = "b", *c = "c", *d = "d";
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
	ec_dlist_str_t l1;
	assert(ec_dlist_str_init(&l1, NULL) == EC_OK);
	assert(l1.splice(&l1, NULL) == EC_ERR_INVALID_ARG);
	assert(l1.free_fn(&l1) == EC_OK);
}

static void
test_splice_at_pos(void)
{
	ec_dlist_str_t l1, l2;
	assert(ec_dlist_str_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_str_init(&l2, NULL) == EC_OK);
	const char *a = "a", *b = "b", *c = "c", *d = "d", *e = "a0";
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
	ec_dlist_str_t l1, l2;
	assert(ec_dlist_str_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_str_init(&l2, NULL) == EC_OK);
	const char *a = "a", *d = "d";
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l2.add(&l2, &d) == EC_OK);
	assert(l1.splice_at(&l1, &l2, 5) == EC_ERR_OUT_OF_BOUNDS);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

static void
test_add_all_pos(void)
{
	ec_dlist_str_t l1, l2;
	assert(ec_dlist_str_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_str_init(&l2, NULL) == EC_OK);
	const char *a = "a", *b = "b";
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
	ec_dlist_str_t l1, l2;
	assert(ec_dlist_str_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_str_init(&l2, NULL) == EC_OK);
	const char *a = "a", *b = "c", *c = "b", *d = "d";
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
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char* arr[] = { "a", "b", "c", "d", "e" };
	for (int i = 0; i < 5; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_str_t* sub = NULL;
	assert(l.sublist(&l, 1, 4, &sub) == EC_OK);
	assert(sub->size_fn(sub) == 3);
	const char* exp[] = { "b", "c", "d" };
	for (size_t i = 0; i < 3; i++)
		{
			const char** out = NULL;
			assert(sub->get_at(sub, i, (void**)&out) == EC_OK && *(const char**)out == exp[i]);
		}
	assert(sub->free_fn(sub) == EC_OK);
	free(sub);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_sublist_neg_oob(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	ec_dlist_str_t* sub = NULL;
	assert(l.sublist(&l, 1, 10, &sub) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_copy_shallow_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char* arr[] = { "a", "b", "c" };
	for (int i = 0; i < 3; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_str_t* cp = NULL;
	assert(l.copy_shallow(&l, &cp) == EC_OK);
	assert(cp->size_fn(cp) == 3);
	assert(cp->free_fn(cp) == EC_OK);
	free(cp);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_copy_deep_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char* arr[] = { "a", "b", "c" };
	for (int i = 0; i < 3; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_str_t* cp = NULL;
	assert(l.copy_deep(&l, dupe_str, &cp) == EC_OK);
	assert(cp->size_fn(cp) == 3);
	assert(cp->free_fn(cp) == EC_OK);
	free(cp);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_replace_at_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b", *c = "z";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.replace_at(&l, &c, 1) == EC_OK);
	const char** out = NULL;
	assert(l.get_at(&l, 1, (void**)&out) == EC_OK && *(const char**)out == "z");
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_replace_at_neg_oob(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *c = "z";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.replace_at(&l, &c, 5) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_contains_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b", *c = "c";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	const char* x = "b";
	assert(l.contains(&l, &x) == 1);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_contains_neg_not_found(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	const char* x = "z";
	assert(l.contains(&l, &x) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_contains_value_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b", *c = "c";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	const char* x = "c";
	assert(l.contains_value(&l, &x, cmp_str) == 1);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_index_of_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char* arr[] = { "a", "b", "c", "d", "e" };
	for (int i = 0; i < 5; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	const char* x = "d";
	size_t idx;
	assert(l.index_of(&l, &x, cmp_str, &idx) == EC_OK);
	assert(idx == 3);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_index_of_neg_not_found(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char* arr[] = { "a", "b", "c", "d", "e" };
	for (int i = 0; i < 5; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	const char* x = "z";
	size_t idx;
	assert(l.index_of(&l, &x, cmp_str, &idx) == EC_ERR_NOT_FOUND);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_to_array_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b", *c = "c";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	void** arr = NULL;
	assert(l.to_array(&l, &arr) == EC_OK);
	// arr contains pointers to const char* values
	const char** str_arr = (const char**)arr;
	assert(cmp_str(str_arr[0], &a) == 0 && cmp_str(str_arr[1], &b) == 0 && cmp_str(str_arr[2], &c) == 0);
	free(arr);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_reverse_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b", *c = "c";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	l.reverse(&l);
	const char** out;
	assert(l.get_at(&l, 0, (void**)&out) == EC_OK && *(const char**)out == "c");
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_sort_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "c", *b = "a", *c = "b";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	assert(l.sort(&l, cmp_str) == EC_OK);
	const char** out;
	assert(l.get_at(&l, 0, (void**)&out) == EC_OK && *(const char**)out == "a");
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_sort_in_place_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "c", *b = "a", *c = "b";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	l.sort_in_place(&l, cmp_str);
	const char** out;
	assert(l.get_at(&l, 0, (void**)&out) == EC_OK && *(const char**)out == "a");
	assert(l.get_at(&l, 1, (void**)&out) == EC_OK && *(const char**)out == "b");
	assert(l.get_at(&l, 2, (void**)&out) == EC_OK && *(const char**)out == "c");
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_foreach_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char *a = "a", *b = "b";
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	l.foreach (&l, toupper_str);
	const char** out;
	// toupper_str doesn't modify const char* strings, so "a" should still be first
	assert(l.get_at(&l, 0, (void**)&out) == EC_OK && cmp_str(out, &a) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_reduce_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char* arr[] = { "a", "b", "c", "d" };
	for (int i = 0; i < 4; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	const char* sum = "";
	assert(l.reduce(&l, concat_strs, &sum) == EC_OK);
	// concat_strs just returns first string, so sum should point to first element
	assert(sum != NULL);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_filter_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	// Use longer strings for is_long_str (length > 1)
	const char* arr[] = { "a", "ab", "b", "bc", "c", "cd" };
	for (int i = 0; i < 6; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_str_t* fl = NULL;
	assert(l.filter(&l, is_long_str, &fl) == EC_OK);
	// 3 strings have length > 1: "ab", "bc", "cd"
	assert(fl->size_fn(fl) == 3);
	assert(fl->free_fn(fl) == EC_OK);
	free(fl);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_filter_mut_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	// Use longer strings for is_long_str (length > 1)
	const char* arr[] = { "a", "ab", "b", "bc", "c", "cd" };
	for (int i = 0; i < 6; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	assert(l.filter_mut(&l, is_long_str) == EC_OK);
	// 3 strings have length > 1
	assert(l.size_fn(&l) == 3);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_next_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char* arr[] = { "a", "b", "c", "d", "e" };
	for (int i = 0; i < 5; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_str_iter_t it;
	l.iter_init(&it, &l);
	const char** out;
	size_t count = 0;
	while (l.iter_next(&it, (void**)&out) == EC_OK)
		{
			assert(*(const char**)out >= "a" && *(const char**)out <= "e");
			count++;
		}
	assert(count == 5);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_remove_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char* arr[] = { "a", "b", "c", "d", "e" };
	for (int i = 0; i < 5; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_str_iter_t it;
	l.iter_init(&it, &l);
	const char** out;
	assert(l.iter_next(&it, (void**)&out) == EC_OK);
	assert(l.iter_remove(&it) == EC_OK);
	assert(l.size_fn(&l) == 4);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_add_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char* arr[] = { "a", "b", "c" };
	for (int i = 0; i < 3; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_str_iter_t it;
	l.iter_init(&it, &l);
	const char** out;
	assert(l.iter_next(&it, (void**)&out) == EC_OK);
	const char* x = "z";
	assert(l.iter_add(&it, &x) == EC_OK);
	assert(l.size_fn(&l) == 4);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_replace_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char* arr[] = { "a", "b", "c" };
	for (int i = 0; i < 3; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_str_iter_t it;
	l.iter_init(&it, &l);
	const char** out;
	assert(l.iter_next(&it, (void**)&out) == EC_OK);
	const char* y = "z";
	assert(l.iter_replace(&it, &y, NULL) == EC_OK);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_index_pos(void)
{
	ec_dlist_str_t l;
	assert(ec_dlist_str_init(&l, NULL) == EC_OK);
	const char* arr[] = { "a", "b", "c", "d", "e" };
	for (int i = 0; i < 5; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_str_iter_t it;
	l.iter_init(&it, &l);
	const char** out;
	size_t idx = 0;
	while (l.iter_next(&it, (void**)&out) == EC_OK)
		{
			assert(l.iter_index(&it) == idx + 1);
			idx++;
		}
	assert(idx == 5);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_zip_iter_next_pos(void)
{
	ec_dlist_str_t l1, l2;
	assert(ec_dlist_str_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_str_init(&l2, NULL) == EC_OK);
	char arr1[] = { "a", "b", "c", "d", "e" };
	char arr2[] = { "1", "2", "3", "4", "5" };
	for (int i = 0; i < 5; i++)
		{
			assert(l1.add(&l1, &arr1[i]) == EC_OK);
			assert(l2.add(&l2, &arr2[i]) == EC_OK);
		}
	ec_dlist_str_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	size_t c = 0;
	while (l1.zip_iter_next(&zit, (void**)&out1, (void**)&out2) == EC_OK)
		{
			// Verify that both are valid string pointers (not NULL)
			assert(*(const char**)out1 != NULL);
			assert(*(const char**)out2 != NULL);
			c++;
		}
	assert(c == 5);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

static void
test_zip_iter_init_pos(void)
{
	ec_dlist_str_t l1, l2;
	assert(ec_dlist_str_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_str_init(&l2, NULL) == EC_OK);
	const char *a = "a", *b = "b";
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l2.add(&l2, &b) == EC_OK);
	ec_dlist_str_zip_iter_t zit;
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
	ec_dlist_str_t l1, l2;
	assert(ec_dlist_str_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_str_init(&l2, NULL) == EC_OK);
	char arr1[] = { "a", "b", "c", "d", "e" };
	char arr2[] = { "1", "2", "3", "4", "5" };
	for (int i = 0; i < 5; i++)
		{
			assert(l1.add(&l1, &arr1[i]) == EC_OK);
			assert(l2.add(&l2, &arr2[i]) == EC_OK);
		}
	ec_dlist_str_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	size_t idx = 0;
	while (l1.zip_iter_next(&zit, (void**)&out1, (void**)&out2) == EC_OK)
		{
			assert(l1.zip_iter_index(&zit) == idx + 1);
			idx++;
		}
	assert(idx == 5);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

static void
test_zip_iter_add_neg_not_supported(void)
{
	ec_dlist_str_t l1, l2;
	assert(ec_dlist_str_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_str_init(&l2, NULL) == EC_OK);
	const char *a = "a", *b = "b";
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l2.add(&l2, &b) == EC_OK);
	ec_dlist_str_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	const char *x = "x", *y = "y";
	assert(l1.zip_iter_add(&zit, &x, &y) == EC_ERR_NOT_SUPPORTED);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

static void
test_zip_iter_remove_neg_not_supported(void)
{
	ec_dlist_str_t l1, l2;
	assert(ec_dlist_str_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_str_init(&l2, NULL) == EC_OK);
	const char *a = "a", *b = "b";
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l2.add(&l2, &b) == EC_OK);
	ec_dlist_str_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	assert(l1.zip_iter_next(&zit, (void**)&out1, (void**)&out2) == EC_OK);
	assert(l1.zip_iter_remove(&zit, (void**)&out1, (void**)&out2) == EC_ERR_NOT_SUPPORTED);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

static void
test_zip_iter_replace_neg_not_supported(void)
{
	ec_dlist_str_t l1, l2;
	assert(ec_dlist_str_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_str_init(&l2, NULL) == EC_OK);
	const char *a = "a", *b = "b";
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l2.add(&l2, &b) == EC_OK);
	ec_dlist_str_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	assert(l1.zip_iter_next(&zit, (void**)&out1, (void**)&out2) == EC_OK);
	const char *x = "x", *y = "y";
	void *old1, *old2;
	assert(l1.zip_iter_replace(&zit, &x, &y, &old1, &old2) == EC_ERR_NOT_SUPPORTED);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

int
main(void)
{
	printf("Starting ec_dlist_str tests...\n");
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
	test_add_first_last_pos();
	test_remove_pos();
	test_remove_neg_not_found();
	test_remove_first_pos();
	test_remove_first_neg_empty();
	test_remove_last_pos();
	test_remove_last_neg_empty();
	test_remove_at_pos();
	test_remove_at_neg_oob();
	test_remove_all_pos();
	test_remove_all_neg_empty();
	test_remove_all_cb_pos();
	test_remove_all_cb_neg_empty();
	test_get_at_pos();
	test_get_at_neg_oob();
	test_get_first_last_pos();
	test_get_first_neg_empty();
	test_get_first_neg_null();
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
	test_sort_in_place_pos();
	test_foreach_pos();
	test_reduce_pos();
	test_filter_pos();
	test_filter_mut_pos();
	test_iter_next_pos();
	test_iter_remove_pos();
	test_iter_add_pos();
	test_iter_replace_pos();
	test_iter_index_pos();
	test_zip_iter_next_pos();
	test_zip_iter_init_pos();
	test_zip_iter_index_pos();
	test_zip_iter_add_neg_not_supported();
	test_zip_iter_remove_neg_not_supported();
	test_zip_iter_replace_neg_not_supported();
#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif
	printf("All ec_dlist_str tests passed.\n");
	return 0;
}
