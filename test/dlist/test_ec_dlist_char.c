#include "../../src/ec_dlist.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

EC_DLIST_DEF(char, ch)

static int
cmp_char(const void* a, const void* b)
{
	return (*(char*)a - *(char*)b);
}

static bool
is_upper(const void* x)
{
	char c = *(char*)x;
	return c >= 'A' && c <= 'Z';
}

static void
inc_char(void* x)
{
	(*(char*)x)++;
}

static void
add_chars(void* a, void* b, void* out)
{
	*(char*)out = (*(char*)a) + (*(char*)b);
}

static void*
dupe_char(void* src)
{
	char* ss = (char*)src;
	char* xx = (char*)malloc(sizeof(char));
	*xx = *ss;
	return xx;
}

static int callback_counter = 0;

static void
callback_char(void* x)
{
	(void)x;
	callback_counter++;
}

static void
test_init_free_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	assert(l.size_fn(&l) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_add_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.size_fn(&l) == 2);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_add_at_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char x = 'a', y = 'b', z = 'c';
	assert(l.add(&l, &x) == EC_OK);
	assert(l.add(&l, &z) == EC_OK);
	assert(l.add_at(&l, &y, 1) == EC_OK);
	void* out;
	assert(l.get_at(&l, 1, &out) == EC_OK && *(char*)out == 'b');
	assert(l.size_fn(&l) == 3);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_add_at_neg_oob(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char x = 'a', y = 'b';
	assert(l.add(&l, &x) == EC_OK);
	assert(l.add_at(&l, &y, 5) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_add_first_last_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b', c = 'c';
	assert(l.add_first(&l, &a) == EC_OK);
	assert(l.add_last(&l, &b) == EC_OK);
	assert(l.add_last(&l, &c) == EC_OK);
	assert(l.size_fn(&l) == 3);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b', c = 'c';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	char v = 'b';
	assert(l.remove_fn(&l, &v) == EC_OK);
	assert(l.size_fn(&l) == 2);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_neg_not_found(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	char v = 'z';
	assert(l.remove_fn(&l, &v) == EC_ERR_NOT_FOUND);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_first_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.remove_first(&l) == EC_OK);
	assert(l.size_fn(&l) == 1);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_first_neg_empty(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	assert(l.remove_first(&l) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_last_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.remove_last(&l) == EC_OK);
	assert(l.size_fn(&l) == 1);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_last_neg_empty(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	assert(l.remove_last(&l) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_at_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b', c = 'c';
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
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.remove_at(&l, 5) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_all_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b', c = 'c';
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
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	assert(l.remove_all(&l) == EC_OK);
	assert(l.size_fn(&l) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_all_cb_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b', c = 'c';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	assert(l.size_fn(&l) == 3);
	callback_counter = 0;
	assert(l.remove_all_cb(&l, callback_char) == EC_OK);
	assert(callback_counter == 3);
	assert(l.size_fn(&l) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_all_cb_neg_empty(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	callback_counter = 0;
	assert(l.remove_all_cb(&l, callback_char) == EC_OK);
	assert(callback_counter == 0);
	assert(l.size_fn(&l) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_at_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	void* out = NULL;
	assert(l.get_at(&l, 1, &out) == EC_OK && *(char*)out == 'b');
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_at_neg_oob(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	void* out = NULL;
	assert(l.get_at(&l, 2, &out) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_first_last_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b', c = 'c';
	assert(l.add_first(&l, &a) == EC_OK);
	assert(l.add_last(&l, &b) == EC_OK);
	assert(l.add_last(&l, &c) == EC_OK);
	void* out;
	assert(l.get_first(&l, &out) == EC_OK && *(char*)out == 'a');
	assert(l.get_last(&l, &out) == EC_OK && *(char*)out == 'c');
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_first_neg_empty(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	void* out;
	assert(l.get_first(&l, &out) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_first_neg_null(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a';
	assert(l.add_first(&l, &a) == EC_OK);
	void* out;
	assert(l.get_first(NULL, &out) == EC_ERR_INVALID_ARG);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_last_neg_empty(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	void* out;
	assert(l.get_last(&l, &out) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_splice_pos(void)
{
	ec_dlist_ch_t l1, l2;
	assert(ec_dlist_ch_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ch_init(&l2, NULL) == EC_OK);
	char a = 'a', b = 'b', c = 'c', d = 'd';
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
	ec_dlist_ch_t l1;
	assert(ec_dlist_ch_init(&l1, NULL) == EC_OK);
	assert(l1.splice(&l1, NULL) == EC_ERR_INVALID_ARG);
	assert(l1.free_fn(&l1) == EC_OK);
}

static void
test_splice_at_pos(void)
{
	ec_dlist_ch_t l1, l2;
	assert(ec_dlist_ch_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ch_init(&l2, NULL) == EC_OK);
	char a = 'a', b = 'b', c = 'c', d = 'd', e = 'e';
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
	ec_dlist_ch_t l1, l2;
	assert(ec_dlist_ch_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ch_init(&l2, NULL) == EC_OK);
	char a = 'a', d = 'd';
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l2.add(&l2, &d) == EC_OK);
	assert(l1.splice_at(&l1, &l2, 5) == EC_ERR_OUT_OF_BOUNDS);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

static void
test_add_all_pos(void)
{
	ec_dlist_ch_t l1, l2;
	assert(ec_dlist_ch_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ch_init(&l2, NULL) == EC_OK);
	char a = 'a', b = 'b';
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
	ec_dlist_ch_t l1, l2;
	assert(ec_dlist_ch_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ch_init(&l2, NULL) == EC_OK);
	char a = 'a', b = 'c', c = 'b', d = 'd';
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
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char arr[] = { 'a', 'b', 'c', 'd', 'e' };
	for (int i = 0; i < 5; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_ch_t* sub = NULL;
	assert(l.sublist(&l, 1, 4, &sub) == EC_OK);
	assert(sub->size_fn(sub) == 3);
	char exp[] = { 'b', 'c', 'd' };
	for (size_t i = 0; i < 3; i++)
		{
			void* out = NULL;
			assert(sub->get_at(sub, i, &out) == EC_OK && *(char*)out == exp[i]);
		}
	assert(sub->free_fn(sub) == EC_OK);
	free(sub);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_sublist_neg_oob(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	ec_dlist_ch_t* sub = NULL;
	assert(l.sublist(&l, 1, 10, &sub) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_copy_shallow_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char arr[] = { 'a', 'b', 'c' };
	for (int i = 0; i < 3; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_ch_t* cp = NULL;
	assert(l.copy_shallow(&l, &cp) == EC_OK);
	assert(cp->size_fn(cp) == 3);
	assert(cp->free_fn(cp) == EC_OK);
	free(cp);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_copy_deep_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char arr[] = { 'a', 'b', 'c' };
	for (int i = 0; i < 3; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_ch_t* cp = NULL;
	assert(l.copy_deep(&l, dupe_char, &cp) == EC_OK);
	assert(cp->size_fn(cp) == 3);
	assert(cp->free_fn(cp) == EC_OK);
	free(cp);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_replace_at_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b', c = 'z';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.replace_at(&l, &c, 1) == EC_OK);
	void* out = NULL;
	assert(l.get_at(&l, 1, &out) == EC_OK && *(char*)out == 'z');
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_replace_at_neg_oob(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', c = 'z';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.replace_at(&l, &c, 5) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_contains_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b', c = 'c';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	char x = 'b';
	assert(l.contains(&l, &x) == 1);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_contains_neg_not_found(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	char x = 'z';
	assert(l.contains(&l, &x) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_contains_value_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b', c = 'c';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	char x = 'c';
	assert(l.contains_value(&l, &x, cmp_char) == 1);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_index_of_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char arr[] = { 'a', 'b', 'c', 'd', 'e' };
	for (int i = 0; i < 5; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	char x = 'd';
	size_t idx;
	assert(l.index_of(&l, &x, cmp_char, &idx) == EC_OK);
	assert(idx == 3);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_index_of_neg_not_found(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char arr[] = { 'a', 'b', 'c', 'd', 'e' };
	for (int i = 0; i < 5; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	char x = 'z';
	size_t idx;
	assert(l.index_of(&l, &x, cmp_char, &idx) == EC_ERR_NOT_FOUND);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_to_array_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b', c = 'c';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	void** arr = NULL;
	assert(l.to_array(&l, &arr) == EC_OK);
	assert(*(char*)arr[0] == 'a' && *(char*)arr[1] == 'b' && *(char*)arr[2] == 'c');
	free(arr);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_reverse_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b', c = 'c';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	l.reverse(&l);
	void* out;
	assert(l.get_at(&l, 0, &out) == EC_OK && *(char*)out == 'c');
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_sort_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'c', b = 'a', c = 'b';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	assert(l.sort(&l, cmp_char) == EC_OK);
	void* out;
	assert(l.get_at(&l, 0, &out) == EC_OK && *(char*)out == 'a');
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_sort_in_place_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'c', b = 'a', c = 'b';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	assert(l.add(&l, &c) == EC_OK);
	l.sort_in_place(&l, cmp_char);
	void* out;
	assert(l.get_at(&l, 0, &out) == EC_OK && *(char*)out == 'a');
	assert(l.get_at(&l, 1, &out) == EC_OK && *(char*)out == 'b');
	assert(l.get_at(&l, 2, &out) == EC_OK && *(char*)out == 'c');
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_foreach_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char a = 'a', b = 'b';
	assert(l.add(&l, &a) == EC_OK);
	assert(l.add(&l, &b) == EC_OK);
	l.foreach (&l, inc_char);
	void* out;
	assert(l.get_at(&l, 0, &out) == EC_OK && *(char*)out == 'b');
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_reduce_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char arr[] = { 'a', 'b', 'c', 'd' };
	for (int i = 0; i < 4; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	char sum = 0;
	assert(l.reduce(&l, add_chars, &sum) == EC_OK);
	// Note: char addition may overflow, check if sum equals expected (modulo 256)
	int expected = ('a' + 'b' + 'c' + 'd') & 0xFF;
	assert((unsigned char)sum == (unsigned char)expected);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_filter_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char arr[] = { 'a', 'A', 'b', 'B', 'c', 'C' };
	for (int i = 0; i < 6; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_ch_t* fl = NULL;
	assert(l.filter(&l, is_upper, &fl) == EC_OK);
	assert(fl->size_fn(fl) == 3);
	assert(fl->free_fn(fl) == EC_OK);
	free(fl);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_filter_mut_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char arr[] = { 'a', 'A', 'b', 'B', 'c', 'C' };
	for (int i = 0; i < 6; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	assert(l.filter_mut(&l, is_upper) == EC_OK);
	assert(l.size_fn(&l) == 3);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_next_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char arr[] = { 'a', 'b', 'c', 'd', 'e' };
	for (int i = 0; i < 5; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_ch_iter_t it;
	l.iter_init(&it, &l);
	void* out;
	size_t count = 0;
	while (l.iter_next(&it, &out) == EC_OK)
		{
			assert(*(char*)out >= 'a' && *(char*)out <= 'e');
			count++;
		}
	assert(count == 5);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_remove_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char arr[] = { 'a', 'b', 'c', 'd', 'e' };
	for (int i = 0; i < 5; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_ch_iter_t it;
	l.iter_init(&it, &l);
	void* out;
	assert(l.iter_next(&it, &out) == EC_OK);
	assert(l.iter_remove(&it) == EC_OK);
	assert(l.size_fn(&l) == 4);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_add_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char arr[] = { 'a', 'b', 'c' };
	for (int i = 0; i < 3; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_ch_iter_t it;
	l.iter_init(&it, &l);
	void* out;
	assert(l.iter_next(&it, &out) == EC_OK);
	char x = 'z';
	assert(l.iter_add(&it, &x) == EC_OK);
	assert(l.size_fn(&l) == 4);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_replace_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char arr[] = { 'a', 'b', 'c' };
	for (int i = 0; i < 3; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_ch_iter_t it;
	l.iter_init(&it, &l);
	void* out;
	assert(l.iter_next(&it, &out) == EC_OK);
	char y = 'z';
	assert(l.iter_replace(&it, &y, NULL) == EC_OK);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_iter_index_pos(void)
{
	ec_dlist_ch_t l;
	assert(ec_dlist_ch_init(&l, NULL) == EC_OK);
	char arr[] = { 'a', 'b', 'c', 'd', 'e' };
	for (int i = 0; i < 5; i++)
		assert(l.add(&l, &arr[i]) == EC_OK);
	ec_dlist_ch_iter_t it;
	l.iter_init(&it, &l);
	void* out;
	size_t idx = 0;
	while (l.iter_next(&it, &out) == EC_OK)
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
	ec_dlist_ch_t l1, l2;
	assert(ec_dlist_ch_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ch_init(&l2, NULL) == EC_OK);
	char arr1[] = { 'a', 'b', 'c', 'd', 'e' };
	char arr2[] = { '1', '2', '3', '4', '5' };
	for (int i = 0; i < 5; i++)
		{
			assert(l1.add(&l1, &arr1[i]) == EC_OK);
			assert(l2.add(&l2, &arr2[i]) == EC_OK);
		}
	ec_dlist_ch_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	size_t c = 0;
	while (l1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			assert(*(char*)out1 >= 'a' && *(char*)out1 <= 'e');
			assert(*(char*)out2 >= '1' && *(char*)out2 <= '5');
			c++;
		}
	assert(c == 5);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

static void
test_zip_iter_init_pos(void)
{
	ec_dlist_ch_t l1, l2;
	assert(ec_dlist_ch_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ch_init(&l2, NULL) == EC_OK);
	char a = 'a', b = 'b';
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l2.add(&l2, &b) == EC_OK);
	ec_dlist_ch_zip_iter_t zit;
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
	ec_dlist_ch_t l1, l2;
	assert(ec_dlist_ch_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ch_init(&l2, NULL) == EC_OK);
	char arr1[] = { 'a', 'b', 'c', 'd', 'e' };
	char arr2[] = { '1', '2', '3', '4', '5' };
	for (int i = 0; i < 5; i++)
		{
			assert(l1.add(&l1, &arr1[i]) == EC_OK);
			assert(l2.add(&l2, &arr2[i]) == EC_OK);
		}
	ec_dlist_ch_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	size_t idx = 0;
	while (l1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
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
	ec_dlist_ch_t l1, l2;
	assert(ec_dlist_ch_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ch_init(&l2, NULL) == EC_OK);
	char a = 'a', b = 'b';
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l2.add(&l2, &b) == EC_OK);
	ec_dlist_ch_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	char x = 'x', y = 'y';
	assert(l1.zip_iter_add(&zit, &x, &y) == EC_ERR_NOT_SUPPORTED);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

static void
test_zip_iter_remove_neg_not_supported(void)
{
	ec_dlist_ch_t l1, l2;
	assert(ec_dlist_ch_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ch_init(&l2, NULL) == EC_OK);
	char a = 'a', b = 'b';
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l2.add(&l2, &b) == EC_OK);
	ec_dlist_ch_zip_iter_t zit;
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
	ec_dlist_ch_t l1, l2;
	assert(ec_dlist_ch_init(&l1, NULL) == EC_OK);
	assert(ec_dlist_ch_init(&l2, NULL) == EC_OK);
	char a = 'a', b = 'b';
	assert(l1.add(&l1, &a) == EC_OK);
	assert(l2.add(&l2, &b) == EC_OK);
	ec_dlist_ch_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	assert(l1.zip_iter_next(&zit, &out1, &out2) == EC_OK);
	char x = 'x', y = 'y';
	void *old1, *old2;
	assert(l1.zip_iter_replace(&zit, &x, &y, &old1, &old2) == EC_ERR_NOT_SUPPORTED);
	assert(l1.free_fn(&l1) == EC_OK);
	assert(l2.free_fn(&l2) == EC_OK);
}

int
main(void)
{
	printf("Starting ec_dlist_char tests...\n");
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
	printf("All ec_dlist_char tests passed.\n");
	return 0;
}
