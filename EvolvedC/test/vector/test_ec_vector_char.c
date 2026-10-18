#include "../../src/ec_vector.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

EC_VECTOR_DEF(char, ch)

static int
cmp_ch(const void* a, const void* b)
{
	char ia = *(const char*)a;
	char ib = *(const char*)b;
	return (int)(ia - ib);
}

static void
inc_ch(char* elem)
{
	*elem += 1;
}

static void
sum_ch(char* acc, char* elem, char* out)
{
	*out = (*acc) + (*elem);
}

static char
copy_ch_val(char* src)
{
	return *src;
}

static bool
is_even_ch(char num)
{
	return (num % 2) == 0;
}

static void
test_init_pos(void)
{
	ec_vector_ch_t vec;
	assert(ec_vector_ch_init(&vec, 10, NULL) == EC_OK);
	assert(vec.size_fn(&vec) == 0);
	assert(vec.capacity_fn(&vec) == 10);
	vec.free_fn(&vec);
}

static void
test_init_neg_null(void)
{
	assert(ec_vector_ch_init(NULL, 10, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_init_neg_zero_capacity(void)
{
	ec_vector_ch_t vec;
	assert(ec_vector_ch_init(&vec, 0, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_push_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 3, NULL);
	assert(vec.push(&vec, 'a') == EC_OK);
	assert(vec.size_fn(&vec) == 1);
	char val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 'a');
	vec.free_fn(&vec);
}

static void
test_push_pos_expansion(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 2, NULL);
	assert(vec.push(&vec, 'a') == EC_OK);
	assert(vec.push(&vec, 'b') == EC_OK);
	assert(vec.push(&vec, 'c') == EC_OK);
	assert(vec.size_fn(&vec) == 3);
	assert(vec.capacity_fn(&vec) >= 3);
	vec.free_fn(&vec);
}

static void
test_push_neg_null(void)
{
	assert(((ec_vector_ch_t*)NULL)->push == NULL || 1);
}

static void
test_insert_pos_beginning(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 3, NULL);
	vec.push(&vec, 'b');
	assert(vec.insert(&vec, 0, 'a') == EC_OK);
	assert(vec.size_fn(&vec) == 2);
	char val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 'a');
	vec.free_fn(&vec);
}

static void
test_insert_pos_middle(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 3, NULL);
	vec.push(&vec, 'a');
	vec.push(&vec, 'c');
	assert(vec.insert(&vec, 1, 'b') == EC_OK);
	assert(vec.size_fn(&vec) == 3);
	char val;
	assert(vec.get(&vec, 1, &val) == EC_OK && val == 'b');
	vec.free_fn(&vec);
}

static void
test_insert_pos_end(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 3, NULL);
	vec.push(&vec, 'a');
	assert(vec.insert(&vec, 1, 'b') == EC_OK);
	assert(vec.size_fn(&vec) == 2);
	vec.free_fn(&vec);
}

static void
test_insert_neg_null(void)
{
	assert(((ec_vector_ch_t*)NULL)->insert == NULL || 1);
}

static void
test_insert_neg_out_of_bounds(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 3, NULL);
	vec.push(&vec, 'a');
	assert(vec.insert(&vec, 3, 'b') == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_remove_at_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 3, NULL);
	vec.push(&vec, 'a');
	vec.push(&vec, 'b');
	vec.push(&vec, 'c');
	assert(vec.remove_at(&vec, 1) == EC_OK);
	assert(vec.size_fn(&vec) == 2);
	char val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 'a');
	assert(vec.get(&vec, 1, &val) == EC_OK && val == 'c');
	vec.free_fn(&vec);
}

static void
test_remove_at_neg_null(void)
{
	assert(((ec_vector_ch_t*)NULL)->remove_at == NULL || 1);
}

static void
test_remove_at_neg_out_of_bounds(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 3, NULL);
	vec.push(&vec, 'a');
	assert(vec.remove_at(&vec, 1) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_get_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 3, NULL);
	vec.push(&vec, 'x');
	char val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 'x');
	vec.free_fn(&vec);
}

static void
test_get_neg_null(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 3, NULL);
	vec.push(&vec, 'x');
	assert(vec.get(NULL, 0, NULL) == EC_ERR_INVALID_ARG);
	assert(vec.get(&vec, 0, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_get_neg_out_of_bounds(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 3, NULL);
	vec.push(&vec, 'x');
	char val;
	assert(vec.get(&vec, 1, &val) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_replace_at_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 3, NULL);
	vec.push(&vec, 'a');
	char old_val;
	assert(vec.replace_at(&vec, 0, 'z', &old_val) == EC_OK);
	assert(old_val == 'a');
	char val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 'z');
	vec.free_fn(&vec);
}

static void
test_replace_at_neg_null(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 3, NULL);
	vec.push(&vec, 'a');
	char old_val;
	assert(vec.replace_at(NULL, 0, 'z', &old_val) == EC_ERR_INVALID_ARG);
	assert(vec.replace_at(&vec, 0, 'z', NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_replace_at_neg_out_of_bounds(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 3, NULL);
	vec.push(&vec, 'a');
	char old_val;
	assert(vec.replace_at(&vec, 1, 'z', &old_val) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_swap_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 3, NULL);
	vec.push(&vec, 'a');
	vec.push(&vec, 'b');
	assert(vec.swap(&vec, 0, 1) == EC_OK);
	char val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 'b');
	assert(vec.get(&vec, 1, &val) == EC_OK && val == 'a');
	vec.free_fn(&vec);
}

static void
test_swap_neg_null(void)
{
	assert(((ec_vector_ch_t*)NULL)->swap == NULL || 1);
}

static void
test_swap_neg_out_of_bounds(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 3, NULL);
	vec.push(&vec, 'a');
	assert(vec.swap(&vec, 0, 1) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_contains_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'a');
	vec.push(&vec, 'b');
	assert(vec.contains(&vec, 'a', cmp_ch) == 1);
	vec.free_fn(&vec);
}

static void
test_contains_neg_not_found(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'a');
	vec.push(&vec, 'b');
	assert(vec.contains(&vec, 'z', cmp_ch) == 0);
	vec.free_fn(&vec);
}

static void
test_contains_neg_null(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'a');
	assert(vec.contains(NULL, 'a', cmp_ch) == 0);
	assert(vec.contains(&vec, 'a', NULL) == 0);
	vec.free_fn(&vec);
}

static void
test_index_of_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'a');
	vec.push(&vec, 'b');
	vec.push(&vec, 'c');
	size_t idx;
	assert(vec.index_of(&vec, 'b', &idx, cmp_ch) == EC_OK);
	assert(idx == 1);
	vec.free_fn(&vec);
}

static void
test_index_of_neg_not_found(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'a');
	size_t idx;
	assert(vec.index_of(&vec, 'z', &idx, cmp_ch) == EC_ERR_NOT_FOUND);
	vec.free_fn(&vec);
}

static void
test_index_of_neg_null(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'a');
	size_t idx;
	assert(vec.index_of(NULL, 'a', &idx, cmp_ch) == EC_ERR_INVALID_ARG);
	assert(vec.index_of(&vec, 'a', NULL, cmp_ch) == EC_ERR_INVALID_ARG);
	assert(vec.index_of(&vec, 'a', &idx, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_binary_search_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'a');
	vec.push(&vec, 'b');
	vec.push(&vec, 'c');
	vec.sort(&vec, cmp_ch);
	size_t idx;
	assert(vec.binary_search(&vec, 'b', cmp_ch, &idx) == EC_OK);
	vec.free_fn(&vec);
}

static void
test_binary_search_neg_not_found(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'a');
	vec.push(&vec, 'b');
	vec.sort(&vec, cmp_ch);
	size_t idx;
	assert(vec.binary_search(&vec, 'z', cmp_ch, &idx) == EC_ERR_NOT_FOUND);
	vec.free_fn(&vec);
}

static void
test_binary_search_neg_null(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'a');
	vec.sort(&vec, cmp_ch);
	size_t idx;
	assert(vec.binary_search(NULL, 'a', cmp_ch, &idx) == EC_ERR_INVALID_ARG);
	assert(vec.binary_search(&vec, 'a', NULL, &idx) == EC_ERR_INVALID_ARG);
	assert(vec.binary_search(&vec, 'a', cmp_ch, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_sort_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'c');
	vec.push(&vec, 'a');
	vec.push(&vec, 'b');
	vec.sort(&vec, cmp_ch);
	char val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 'a');
	assert(vec.get(&vec, 1, &val) == EC_OK && val == 'b');
	assert(vec.get(&vec, 2, &val) == EC_OK && val == 'c');
	vec.free_fn(&vec);
}

static void
test_sort_neg_null(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'a');
	vec.sort(NULL, cmp_ch);
	vec.sort(&vec, NULL);
	vec.free_fn(&vec);
}

static void
test_reverse_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'a');
	vec.push(&vec, 'b');
	vec.push(&vec, 'c');
	vec.reverse(&vec);
	char val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 'c');
	assert(vec.get(&vec, 1, &val) == EC_OK && val == 'b');
	assert(vec.get(&vec, 2, &val) == EC_OK && val == 'a');
	vec.free_fn(&vec);
}

static void
test_reverse_pos_empty(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.reverse(&vec);
	assert(vec.size_fn(&vec) == 0);
	vec.free_fn(&vec);
}

static void
test_reverse_neg_null(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.reverse(NULL);
	vec.free_fn(&vec);
}

static void
test_map_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 3, NULL);
	vec.push(&vec, 'a');
	vec.push(&vec, 'b');
	vec.map(&vec, inc_ch);
	char val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 'b');
	assert(vec.get(&vec, 1, &val) == EC_OK && val == 'c');
	vec.free_fn(&vec);
}

static void
test_map_neg_null(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 3, NULL);
	vec.push(&vec, 'a');
	vec.map(NULL, inc_ch);
	vec.map(&vec, NULL);
	vec.free_fn(&vec);
}

static void
test_reduce_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 4, NULL);
	vec.push(&vec, 'a');
	vec.push(&vec, 'b');
	vec.push(&vec, 'c');
	char result = ' ';
	vec.reduce(&vec, sum_ch, &result);
	vec.free_fn(&vec);
}

static void
test_reduce_pos_empty(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 4, NULL);
	char result = ' ';
	vec.reduce(&vec, sum_ch, &result);
	vec.free_fn(&vec);
}

static void
test_reduce_neg_null(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 4, NULL);
	vec.push(&vec, 'a');
	char result = ' ';
	vec.reduce(NULL, sum_ch, &result);
	vec.reduce(&vec, NULL, &result);
	vec.reduce(&vec, sum_ch, NULL);
	vec.free_fn(&vec);
}

static void
test_subarray_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 6, NULL);
	for (int i = 0; i < 6; i++)
		vec.push(&vec, 'a' + i);
	ec_vector_ch_t sub;
	assert(vec.subarray(&vec, 2, 5, &sub) == EC_OK);
	assert(sub.size_fn(&sub) == 3);
	char val;
	assert(vec.get(&sub, 0, &val) == EC_OK && val == 'c');
	sub.free_fn(&sub);
	vec.free_fn(&vec);
}

static void
test_subarray_neg_invalid_range(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 6, NULL);
	vec.push(&vec, 'a');
	ec_vector_ch_t sub;
	assert(vec.subarray(&vec, 2, 1, &sub) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_subarray_neg_out_of_bounds(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 6, NULL);
	vec.push(&vec, 'a');
	ec_vector_ch_t sub;
	assert(vec.subarray(&vec, 0, 2, &sub) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_subarray_neg_null(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 6, NULL);
	vec.push(&vec, 'a');
	ec_vector_ch_t sub;
	assert(vec.subarray(NULL, 0, 1, &sub) == EC_ERR_INVALID_ARG);
	assert(vec.subarray(&vec, 0, 1, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_copy_shallow_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 4, NULL);
	for (int i = 0; i < 4; i++)
		vec.push(&vec, 'a' + i);
	ec_vector_ch_t copy;
	assert(vec.copy_shallow(&vec, &copy) == EC_OK);
	assert(copy.size_fn(&copy) == vec.size_fn(&vec));
	copy.free_fn(&copy);
	vec.free_fn(&vec);
}

static void
test_copy_shallow_neg_null(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 4, NULL);
	vec.push(&vec, 'a');
	ec_vector_ch_t copy;
	assert(vec.copy_shallow(NULL, &copy) == EC_ERR_INVALID_ARG);
	assert(vec.copy_shallow(&vec, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_copy_deep_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 4, NULL);
	vec.push(&vec, 'a');
	vec.push(&vec, 'b');
	ec_vector_ch_t copy;
	assert(vec.copy_deep(&vec, copy_ch_val, &copy) == EC_OK);
	assert(copy.size_fn(&copy) == vec.size_fn(&vec));
	copy.free_fn(&copy);
	vec.free_fn(&vec);
}

static void
test_copy_deep_neg_null(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 4, NULL);
	vec.push(&vec, 'a');
	ec_vector_ch_t copy;
	assert(vec.copy_deep(NULL, copy_ch_val, &copy) == EC_ERR_INVALID_ARG);
	assert(vec.copy_deep(&vec, NULL, &copy) == EC_ERR_INVALID_ARG);
	assert(vec.copy_deep(&vec, copy_ch_val, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_filter_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 6, NULL);
	char nums[] = { 'a', 'b', 'c', 'd', 'e', 'f' };
	for (int i = 0; i < 6; i++)
		vec.push(&vec, nums[i]);
	ec_vector_ch_t filtered;
	assert(vec.filter(&vec, is_even_ch, &filtered) == EC_OK);
	filtered.free_fn(&filtered);
	vec.free_fn(&vec);
}

static void
test_filter_neg_null(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 6, NULL);
	vec.push(&vec, 'a');
	ec_vector_ch_t filtered;
	assert(vec.filter(NULL, is_even_ch, &filtered) == EC_ERR_INVALID_ARG);
	assert(vec.filter(&vec, NULL, &filtered) == EC_ERR_INVALID_ARG);
	assert(vec.filter(&vec, is_even_ch, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_filter_mut_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 6, NULL);
	char nums[] = { 'a', 'b', 'c', 'd', 'e', 'f' };
	for (int i = 0; i < 6; i++)
		vec.push(&vec, nums[i]);
	assert(vec.filter_mut(&vec, is_even_ch) == EC_OK);
	vec.free_fn(&vec);
}

static void
test_filter_mut_neg_null(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 6, NULL);
	vec.push(&vec, 'a');
	assert(vec.filter_mut(NULL, is_even_ch) == EC_ERR_INVALID_ARG);
	assert(vec.filter_mut(&vec, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_size_fn_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 10, NULL);
	assert(vec.size_fn(&vec) == 0);
	vec.push(&vec, 'a');
	assert(vec.size_fn(&vec) == 1);
	vec.free_fn(&vec);
}

static void
test_capacity_fn_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 10, NULL);
	assert(vec.capacity_fn(&vec) == 10);
	vec.free_fn(&vec);
}

static void
test_trim_capacity_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 10, NULL);
	for (int i = 0; i < 4; i++)
		vec.push(&vec, 'a' + i);
	assert(vec.trim_capacity(&vec) == EC_OK);
	assert(vec.capacity_fn(&vec) == vec.size_fn(&vec));
	vec.free_fn(&vec);
}

static void
test_trim_capacity_neg_null(void)
{
	assert(((ec_vector_ch_t*)NULL)->trim_capacity == NULL || 1);
}

static void
test_free_fn_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 10, NULL);
	vec.push(&vec, 'a');
	assert(vec.free_fn(&vec) == EC_OK);
}

static void
test_free_fn_neg_null(void)
{
	assert(((ec_vector_ch_t*)NULL)->free_fn == NULL || 1);
}

static void
test_iter_init_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'a');
	ec_vector_ch_iter_t iter;
	assert(ec_vector_ch_iter_init(&iter, &vec) == EC_OK);
	vec.free_fn(&vec);
}

static void
test_iter_init_neg_null(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	ec_vector_ch_iter_t iter;
	assert(ec_vector_ch_iter_init(NULL, &vec) == EC_ERR_INVALID_ARG);
	assert(ec_vector_ch_iter_init(&iter, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_iter_next_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'x');
	ec_vector_ch_iter_t iter;
	ec_vector_ch_iter_init(&iter, &vec);
	char val;
	assert(ec_vector_ch_iter_next(&iter, &val) == EC_OK);
	assert(val == 'x');
	vec.free_fn(&vec);
}

static void
test_iter_next_neg_empty(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	ec_vector_ch_iter_t iter;
	ec_vector_ch_iter_init(&iter, &vec);
	char val;
	assert(ec_vector_ch_iter_next(&iter, &val) == EC_ITER_END);
	vec.free_fn(&vec);
}

static void
test_iter_next_neg_null(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'x');
	ec_vector_ch_iter_t iter;
	ec_vector_ch_iter_init(&iter, &vec);
	char val;
	assert(ec_vector_ch_iter_next(NULL, &val) == EC_ERR_INVALID_ARG);
	assert(ec_vector_ch_iter_next(&iter, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_iter_remove_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'a');
	vec.push(&vec, 'b');
	ec_vector_ch_iter_t iter;
	ec_vector_ch_iter_init(&iter, &vec);
	char val;
	ec_vector_ch_iter_next(&iter, &val);
	assert(ec_vector_ch_iter_remove(&iter) == EC_OK);
	assert(vec.size_fn(&vec) == 1);
	vec.free_fn(&vec);
}

static void
test_iter_remove_neg_invalid_state(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'a');
	ec_vector_ch_iter_t iter;
	ec_vector_ch_iter_init(&iter, &vec);
	assert(ec_vector_ch_iter_remove(&iter) == EC_ERR_INVALID_OP);
	vec.free_fn(&vec);
}

static void
test_iter_add_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'a');
	ec_vector_ch_iter_t iter;
	ec_vector_ch_iter_init(&iter, &vec);
	char val;
	ec_vector_ch_iter_next(&iter, &val);
	assert(ec_vector_ch_iter_add(&iter, 'z') == EC_OK);
	assert(vec.size_fn(&vec) == 2);
	vec.free_fn(&vec);
}

static void
test_iter_add_neg_null(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	ec_vector_ch_iter_t iter;
	ec_vector_ch_iter_init(&iter, &vec);
	assert(ec_vector_ch_iter_add(NULL, 'z') == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_iter_replace_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'a');
	ec_vector_ch_iter_t iter;
	ec_vector_ch_iter_init(&iter, &vec);
	char val;
	ec_vector_ch_iter_next(&iter, &val);
	char replaced;
	assert(ec_vector_ch_iter_replace(&iter, 'z', &replaced) == EC_OK);
	assert(replaced == 'a');
	vec.free_fn(&vec);
}

static void
test_iter_replace_neg_invalid_state(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'a');
	ec_vector_ch_iter_t iter;
	ec_vector_ch_iter_init(&iter, &vec);
	char replaced;
	assert(ec_vector_ch_iter_replace(&iter, 'z', &replaced) == EC_ERR_INVALID_OP);
	vec.free_fn(&vec);
}

static void
test_iter_index_pos(void)
{
	ec_vector_ch_t vec;
	ec_vector_ch_init(&vec, 5, NULL);
	vec.push(&vec, 'a');
	ec_vector_ch_iter_t iter;
	ec_vector_ch_iter_init(&iter, &vec);
	assert(ec_vector_ch_iter_index(&iter) == 0);
	vec.free_fn(&vec);
}

static void
test_zip_iter_init_pos(void)
{
	ec_vector_ch_t vec1, vec2;
	ec_vector_ch_init(&vec1, 5, NULL);
	ec_vector_ch_init(&vec2, 5, NULL);
	ec_vector_ch_zip_iter_t zip;
	assert(ec_vector_ch_zip_iter_init(&zip, &vec1, &vec2) == EC_OK);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_init_neg_null(void)
{
	ec_vector_ch_t vec1, vec2;
	ec_vector_ch_init(&vec1, 5, NULL);
	ec_vector_ch_init(&vec2, 5, NULL);
	ec_vector_ch_zip_iter_t zip;
	assert(ec_vector_ch_zip_iter_init(NULL, &vec1, &vec2) == EC_ERR_INVALID_ARG);
	assert(ec_vector_ch_zip_iter_init(&zip, NULL, &vec2) == EC_ERR_INVALID_ARG);
	assert(ec_vector_ch_zip_iter_init(&zip, &vec1, NULL) == EC_ERR_INVALID_ARG);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_next_pos(void)
{
	ec_vector_ch_t vec1, vec2;
	ec_vector_ch_init(&vec1, 5, NULL);
	ec_vector_ch_init(&vec2, 5, NULL);
	vec1.push(&vec1, 'a');
	vec2.push(&vec2, 'b');
	ec_vector_ch_zip_iter_t zip;
	ec_vector_ch_zip_iter_init(&zip, &vec1, &vec2);
	char a, b;
	assert(ec_vector_ch_zip_iter_next(&zip, &a, &b) == EC_OK);
	assert(a == 'a' && b == 'b');
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_next_neg_empty(void)
{
	ec_vector_ch_t vec1, vec2;
	ec_vector_ch_init(&vec1, 5, NULL);
	ec_vector_ch_init(&vec2, 5, NULL);
	ec_vector_ch_zip_iter_t zip;
	ec_vector_ch_zip_iter_init(&zip, &vec1, &vec2);
	char a, b;
	assert(ec_vector_ch_zip_iter_next(&zip, &a, &b) == EC_ITER_END);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_next_neg_null(void)
{
	ec_vector_ch_t vec1, vec2;
	ec_vector_ch_init(&vec1, 5, NULL);
	ec_vector_ch_init(&vec2, 5, NULL);
	ec_vector_ch_zip_iter_t zip;
	ec_vector_ch_zip_iter_init(&zip, &vec1, &vec2);
	char a, b;
	assert(ec_vector_ch_zip_iter_next(NULL, &a, &b) == EC_ERR_INVALID_ARG);
	assert(ec_vector_ch_zip_iter_next(&zip, NULL, &b) == EC_ERR_INVALID_ARG);
	assert(ec_vector_ch_zip_iter_next(&zip, &a, NULL) == EC_ERR_INVALID_ARG);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_remove_pos(void)
{
	ec_vector_ch_t vec1, vec2;
	ec_vector_ch_init(&vec1, 5, NULL);
	ec_vector_ch_init(&vec2, 5, NULL);
	vec1.push(&vec1, 'a');
	vec2.push(&vec2, 'b');
	ec_vector_ch_zip_iter_t zip;
	ec_vector_ch_zip_iter_init(&zip, &vec1, &vec2);
	char a, b;
	ec_vector_ch_zip_iter_next(&zip, &a, &b);
	assert(ec_vector_ch_zip_iter_remove(&zip) == EC_OK);
	assert(vec1.size_fn(&vec1) == 0 && vec2.size_fn(&vec2) == 0);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_remove_neg_invalid_state(void)
{
	ec_vector_ch_t vec1, vec2;
	ec_vector_ch_init(&vec1, 5, NULL);
	ec_vector_ch_init(&vec2, 5, NULL);
	ec_vector_ch_zip_iter_t zip;
	ec_vector_ch_zip_iter_init(&zip, &vec1, &vec2);
	assert(ec_vector_ch_zip_iter_remove(&zip) == EC_ERR_INVALID_OP);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_add_pos(void)
{
	ec_vector_ch_t vec1, vec2;
	ec_vector_ch_init(&vec1, 5, NULL);
	ec_vector_ch_init(&vec2, 5, NULL);
	ec_vector_ch_zip_iter_t zip;
	ec_vector_ch_zip_iter_init(&zip, &vec1, &vec2);
	assert(ec_vector_ch_zip_iter_add(&zip, 'a', 'b') == EC_OK);
	assert(vec1.size_fn(&vec1) == 1 && vec2.size_fn(&vec2) == 1);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_add_neg_null(void)
{
	ec_vector_ch_t vec1, vec2;
	ec_vector_ch_init(&vec1, 5, NULL);
	ec_vector_ch_init(&vec2, 5, NULL);
	ec_vector_ch_zip_iter_t zip;
	ec_vector_ch_zip_iter_init(&zip, &vec1, &vec2);
	assert(ec_vector_ch_zip_iter_add(NULL, 'a', 'b') == EC_ERR_INVALID_ARG);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_replace_pos(void)
{
	ec_vector_ch_t vec1, vec2;
	ec_vector_ch_init(&vec1, 5, NULL);
	ec_vector_ch_init(&vec2, 5, NULL);
	vec1.push(&vec1, 'a');
	vec2.push(&vec2, 'b');
	ec_vector_ch_zip_iter_t zip;
	ec_vector_ch_zip_iter_init(&zip, &vec1, &vec2);
	char a, b;
	ec_vector_ch_zip_iter_next(&zip, &a, &b);
	char r1, r2;
	assert(ec_vector_ch_zip_iter_replace(&zip, 'x', 'y', &r1, &r2) == EC_OK);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_replace_neg_invalid_state(void)
{
	ec_vector_ch_t vec1, vec2;
	ec_vector_ch_init(&vec1, 5, NULL);
	ec_vector_ch_init(&vec2, 5, NULL);
	ec_vector_ch_zip_iter_t zip;
	ec_vector_ch_zip_iter_init(&zip, &vec1, &vec2);
	char r1, r2;
	assert(ec_vector_ch_zip_iter_replace(&zip, 'x', 'y', &r1, &r2) == EC_ERR_INVALID_OP);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_index_pos(void)
{
	ec_vector_ch_t vec1, vec2;
	ec_vector_ch_init(&vec1, 5, NULL);
	ec_vector_ch_init(&vec2, 5, NULL);
	ec_vector_ch_zip_iter_t zip;
	ec_vector_ch_zip_iter_init(&zip, &vec1, &vec2);
	assert(ec_vector_ch_zip_iter_index(&zip) == 0);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

int
main(void)
{
	printf("Starting ec_vector_char tests...\n");
	test_init_pos();
	test_init_neg_null();
	test_init_neg_zero_capacity();
	test_push_pos();
	test_push_pos_expansion();
	test_push_neg_null();
	test_insert_pos_beginning();
	test_insert_pos_middle();
	test_insert_pos_end();
	test_insert_neg_null();
	test_insert_neg_out_of_bounds();
	test_remove_at_pos();
	test_remove_at_neg_null();
	test_remove_at_neg_out_of_bounds();
	test_get_pos();
	test_get_neg_null();
	test_get_neg_out_of_bounds();
	test_replace_at_pos();
	test_replace_at_neg_null();
	test_replace_at_neg_out_of_bounds();
	test_swap_pos();
	test_swap_neg_null();
	test_swap_neg_out_of_bounds();
	test_contains_pos();
	test_contains_neg_not_found();
	test_contains_neg_null();
	test_index_of_pos();
	test_index_of_neg_not_found();
	test_index_of_neg_null();
	test_binary_search_pos();
	test_binary_search_neg_not_found();
	test_binary_search_neg_null();
	test_sort_pos();
	test_sort_neg_null();
	test_reverse_pos();
	test_reverse_pos_empty();
	test_reverse_neg_null();
	test_map_pos();
	test_map_neg_null();
	test_reduce_pos();
	test_reduce_pos_empty();
	test_reduce_neg_null();
	test_subarray_pos();
	test_subarray_neg_invalid_range();
	test_subarray_neg_out_of_bounds();
	test_subarray_neg_null();
	test_copy_shallow_pos();
	test_copy_shallow_neg_null();
	test_copy_deep_pos();
	test_copy_deep_neg_null();
	test_filter_pos();
	test_filter_neg_null();
	test_filter_mut_pos();
	test_filter_mut_neg_null();
	test_size_fn_pos();
	test_capacity_fn_pos();
	test_trim_capacity_pos();
	test_trim_capacity_neg_null();
	test_free_fn_pos();
	test_free_fn_neg_null();
	test_iter_init_pos();
	test_iter_init_neg_null();
	test_iter_next_pos();
	test_iter_next_neg_empty();
	test_iter_next_neg_null();
	test_iter_remove_pos();
	test_iter_remove_neg_invalid_state();
	test_iter_add_pos();
	test_iter_add_neg_null();
	test_iter_replace_pos();
	test_iter_replace_neg_invalid_state();
	test_iter_index_pos();
	test_zip_iter_init_pos();
	test_zip_iter_init_neg_null();
	test_zip_iter_next_pos();
	test_zip_iter_next_neg_empty();
	test_zip_iter_next_neg_null();
	test_zip_iter_remove_pos();
	test_zip_iter_remove_neg_invalid_state();
	test_zip_iter_add_pos();
	test_zip_iter_add_neg_null();
	test_zip_iter_replace_pos();
	test_zip_iter_replace_neg_invalid_state();
	test_zip_iter_index_pos();
	printf("All ec_vector_char tests passed.\n");
	return 0;
}
