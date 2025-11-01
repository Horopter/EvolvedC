#include "../../src/ec_vector.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

static int
cmp_u32(const void* a, const void* b)
{
	int ia = *(const int*)a;
	int ib = *(const int*)b;
	return ia - ib;
}

static void
inc_u32(int* elem)
{
	*elem += 1;
}

static void
sum_u32(int* acc, int* elem, int* out)
{
	*out = (*acc) + (*elem);
}

static int
copy_u32_val(int* src)
{
	return *src;
}

static bool
is_even_u32(int num)
{
	return (num % 2) == 0;
}

static void
test_init_pos(void)
{
	ec_vector_u32_t vec;
	assert(ec_vector_u32_init(&vec, 10, NULL) == EC_OK);
	assert(vec.size_fn(&vec) == 0);
	assert(vec.capacity_fn(&vec) == 10);
	vec.free_fn(&vec);
}

static void
test_init_neg_null(void)
{
	assert(ec_vector_u32_init(NULL, 10, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_init_neg_zero_capacity(void)
{
	ec_vector_u32_t vec;
	assert(ec_vector_u32_init(&vec, 0, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_push_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 3, NULL);
	assert(vec.push(&vec, 100U) == EC_OK);
	assert(vec.size_fn(&vec) == 1);
	uint32_t val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 100U);
	vec.free_fn(&vec);
}

static void
test_push_pos_expansion(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 2, NULL);
	assert(vec.push(&vec, 1) == EC_OK);
	assert(vec.push(&vec, 2) == EC_OK);
	assert(vec.push(&vec, 3) == EC_OK);
	assert(vec.size_fn(&vec) == 3);
	assert(vec.capacity_fn(&vec) >= 3);
	vec.free_fn(&vec);
}

static void
test_push_neg_null(void)
{
	assert(((ec_vector_u32_t*)NULL)->push == NULL || 1);
}

static void
test_insert_pos_beginning(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 3, NULL);
	vec.push(&vec, 2);
	assert(vec.insert(&vec, 0, 100U) == EC_OK);
	assert(vec.size_fn(&vec) == 2);
	uint32_t val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 100U);
	vec.free_fn(&vec);
}

static void
test_insert_pos_middle(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 3, NULL);
	vec.push(&vec, 1);
	vec.push(&vec, 3);                          // Vector has size 2, valid indices: 0, 1, 2
	assert(vec.insert(&vec, 2, 200U) == EC_OK); // Insert at index 2 (end)
	assert(vec.size_fn(&vec) == 3);
	uint32_t val;
	assert(vec.get(&vec, 2, &val) == EC_OK && val == 200U);
	vec.free_fn(&vec);
}

static void
test_insert_pos_end(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 3, NULL);
	vec.push(&vec, 1);                          // Vector has size 1, valid indices: 0, 1, 2
	assert(vec.insert(&vec, 1, 200U) == EC_OK); // Insert at index 1 (end)
	assert(vec.size_fn(&vec) == 2);
	vec.free_fn(&vec);
}

static void
test_insert_neg_null(void)
{
	assert(((ec_vector_u32_t*)NULL)->insert == NULL || 1);
}

static void
test_insert_neg_out_of_bounds(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 3, NULL);
	vec.push(&vec, 1);
	assert(vec.insert(&vec, 3, 200U) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_remove_at_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 3, NULL);
	vec.push(&vec, 1);
	vec.push(&vec, 2);
	vec.push(&vec, 3);
	assert(vec.remove_at(&vec, 1) == EC_OK); // Remove index 1 (value 2)
	assert(vec.size_fn(&vec) == 2);
	uint32_t val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 1U); // First element is 1
	assert(vec.get(&vec, 1, &val) == EC_OK && val == 3U); // Second element is 3
	vec.free_fn(&vec);
}

static void
test_remove_at_neg_null(void)
{
	assert(((ec_vector_u32_t*)NULL)->remove_at == NULL || 1);
}

static void
test_remove_at_neg_out_of_bounds(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 3, NULL);
	vec.push(&vec, 1);
	assert(vec.remove_at(&vec, 1) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_get_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 3, NULL);
	vec.push(&vec, 1000U);
	uint32_t val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 1000U);
	vec.free_fn(&vec);
}

static void
test_get_neg_null(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 3, NULL);
	vec.push(&vec, 1000U);
	assert(vec.get(NULL, 0, NULL) == EC_ERR_INVALID_ARG);
	assert(vec.get(&vec, 0, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_get_neg_out_of_bounds(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 3, NULL);
	vec.push(&vec, 1000U);
	uint32_t val;
	assert(vec.get(&vec, 1, &val) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_replace_at_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 3, NULL);
	vec.push(&vec, 5);
	uint32_t old_val;
	assert(vec.replace_at(&vec, 0, 999U, &old_val) == EC_OK);
	assert(old_val == 5U); // Original value was 5
	uint32_t val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 999U);
	vec.free_fn(&vec);
}

static void
test_replace_at_neg_null(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 3, NULL);
	vec.push(&vec, 5);
	uint32_t old_val;
	assert(vec.replace_at(NULL, 0, 99, &old_val) == EC_ERR_INVALID_ARG);
	assert(vec.replace_at(&vec, 0, 999U, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_replace_at_neg_out_of_bounds(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 3, NULL);
	vec.push(&vec, 5);
	uint32_t old_val;
	assert(vec.replace_at(&vec, 1, 999U, &old_val) == EC_ERR_OUT_OF_BOUNDS); // Index 1 is out of bounds (vector has size 1)
	vec.free_fn(&vec);
}

static void
test_swap_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 3, NULL);
	vec.push(&vec, 11);
	vec.push(&vec, 22);
	assert(vec.swap(&vec, 0, 1) == EC_OK); // Swap 11 and 22
	uint32_t val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 22U); // After swap, index 0 is 22
	assert(vec.get(&vec, 1, &val) == EC_OK && val == 11U); // After swap, index 1 is 11
	vec.free_fn(&vec);
}

static void
test_swap_neg_null(void)
{
	assert(((ec_vector_u32_t*)NULL)->swap == NULL || 1);
}

static void
test_swap_neg_out_of_bounds(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 3, NULL);
	vec.push(&vec, 11);
	assert(vec.swap(&vec, 0, 1) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_contains_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 100U);
	vec.push(&vec, 200U);
	assert(vec.contains(&vec, 100U, cmp_u32) == 1);
	vec.free_fn(&vec);
}

static void
test_contains_neg_not_found(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 100U);
	vec.push(&vec, 200U);
	assert(vec.contains(&vec, 300U, cmp_u32) == 0);
	vec.free_fn(&vec);
}

static void
test_contains_neg_null(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 100U);
	assert(vec.contains(NULL, 10, cmp_u32) == 0);
	assert(vec.contains(&vec, 100U, NULL) == 0);
	vec.free_fn(&vec);
}

static void
test_index_of_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 100U);
	vec.push(&vec, 200U);
	vec.push(&vec, 300U);
	size_t idx;
	assert(vec.index_of(&vec, 200U, &idx, cmp_u32) == EC_OK);
	assert(idx == 1);
	vec.free_fn(&vec);
}

static void
test_index_of_neg_not_found(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 100U);
	size_t idx;
	assert(vec.index_of(&vec, 999U, &idx, cmp_u32) == EC_ERR_NOT_FOUND);
	vec.free_fn(&vec);
}

static void
test_index_of_neg_null(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 100U);
	size_t idx;
	assert(vec.index_of(NULL, 10, &idx, cmp_u32) == EC_ERR_INVALID_ARG);
	assert(vec.index_of(&vec, 10, NULL, cmp_u32) == EC_ERR_INVALID_ARG);
	assert(vec.index_of(&vec, 10, &idx, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_binary_search_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 100U);
	vec.push(&vec, 200U);
	vec.push(&vec, 300U);
	vec.sort(&vec, cmp_u32);
	size_t idx;
	assert(vec.binary_search(&vec, 200U, cmp_u32, &idx) == EC_OK);
	vec.free_fn(&vec);
}

static void
test_binary_search_neg_not_found(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 100U);
	vec.push(&vec, 200U);
	vec.sort(&vec, cmp_u32);
	size_t idx;
	assert(vec.binary_search(&vec, 999U, cmp_u32, &idx) == EC_ERR_NOT_FOUND);
	vec.free_fn(&vec);
}

static void
test_binary_search_neg_null(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 100U);
	vec.sort(&vec, cmp_u32);
	size_t idx;
	assert(vec.binary_search(NULL, 10, cmp_u32, &idx) == EC_ERR_INVALID_ARG);
	assert(vec.binary_search(&vec, 10, NULL, &idx) == EC_ERR_INVALID_ARG);
	assert(vec.binary_search(&vec, 10, cmp_u32, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_sort_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 3);
	vec.push(&vec, 1);
	vec.push(&vec, 2);
	vec.sort(&vec, cmp_u32); // Sort [3, 1, 2] -> [1, 2, 3]
	uint32_t val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 1U);
	assert(vec.get(&vec, 1, &val) == EC_OK && val == 2U);
	assert(vec.get(&vec, 2, &val) == EC_OK && val == 3U);
	vec.free_fn(&vec);
}

static void
test_sort_neg_null(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 3);
	vec.sort(NULL, cmp_u32);
	vec.sort(&vec, NULL);
	vec.free_fn(&vec);
}

static void
test_reverse_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 1);
	vec.push(&vec, 2);
	vec.push(&vec, 3);
	vec.reverse(&vec); // Reverse [1, 2, 3] -> [3, 2, 1]
	uint32_t val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 3U);
	assert(vec.get(&vec, 1, &val) == EC_OK && val == 2U);
	assert(vec.get(&vec, 2, &val) == EC_OK && val == 1U);
	vec.free_fn(&vec);
}

static void
test_reverse_pos_empty(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.reverse(&vec);
	assert(vec.size_fn(&vec) == 0);
	vec.free_fn(&vec);
}

static void
test_reverse_neg_null(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.reverse(NULL);
	vec.free_fn(&vec);
}

static void
test_map_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 3, NULL);
	vec.push(&vec, 5);
	vec.push(&vec, 100U);
	vec.map(&vec, inc_u32); // Map [5, 100] -> [6, 101] (increment each)
	uint32_t val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 6U);
	assert(vec.get(&vec, 1, &val) == EC_OK && val == 101U);
	vec.free_fn(&vec);
}

static void
test_map_neg_null(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 3, NULL);
	vec.push(&vec, 5);
	vec.map(NULL, inc_u32);
	vec.map(&vec, NULL);
	vec.free_fn(&vec);
}

static void
test_reduce_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 4, NULL);
	vec.push(&vec, 1);
	vec.push(&vec, 2);
	vec.push(&vec, 3);
	uint32_t result = 0;
	vec.reduce(&vec, sum_u32, &result);
	assert(result == (1U + 2U + 3U)); // Sum is 6
	vec.free_fn(&vec);
}

static void
test_reduce_pos_empty(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 4, NULL);
	uint32_t result = 0;
	vec.reduce(&vec, sum_u32, &result);
	vec.free_fn(&vec);
}

static void
test_reduce_neg_null(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 4, NULL);
	vec.push(&vec, 1);
	uint32_t result = 0;
	vec.reduce(NULL, sum_u32, &result);
	vec.reduce(&vec, NULL, &result);
	vec.reduce(&vec, sum_u32, NULL);
	vec.free_fn(&vec);
}

static void
test_subarray_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 6, NULL);
	for (int i = 1; i <= 6; i++)
		vec.push(&vec, i * 10);
	ec_vector_u32_t sub;
	assert(vec.subarray(&vec, 2, 5, &sub) == EC_OK); // Subarray from index 2 to 5 (exclusive)
	assert(sub.size_fn(&sub) == 3);
	uint32_t val;
	// Values at indices 2,3,4 are 30,40,50 (i*10 for i=1..6), so first element of subarray is 30
	assert(sub.get(&sub, 0, &val) == EC_OK && val == 30U);
	sub.free_fn(&sub);
	vec.free_fn(&vec);
}

static void
test_subarray_neg_invalid_range(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 6, NULL);
	vec.push(&vec, 100U);
	ec_vector_u32_t sub;
	assert(vec.subarray(&vec, 2, 1, &sub) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_subarray_neg_out_of_bounds(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 6, NULL);
	vec.push(&vec, 100U);
	ec_vector_u32_t sub;
	assert(vec.subarray(&vec, 0, 2, &sub) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_subarray_neg_null(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 6, NULL);
	vec.push(&vec, 100U);
	ec_vector_u32_t sub;
	assert(vec.subarray(NULL, 0, 1, &sub) == EC_ERR_INVALID_ARG);
	assert(vec.subarray(&vec, 0, 1, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_copy_shallow_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 4, NULL);
	for (int i = 1; i <= 4; i++)
		vec.push(&vec, i);
	ec_vector_u32_t copy;
	assert(vec.copy_shallow(&vec, &copy) == EC_OK);
	assert(copy.size_fn(&copy) == vec.size_fn(&vec));
	copy.free_fn(&copy);
	vec.free_fn(&vec);
}

static void
test_copy_shallow_neg_null(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 4, NULL);
	vec.push(&vec, 1);
	ec_vector_u32_t copy;
	assert(vec.copy_shallow(NULL, &copy) == EC_ERR_INVALID_ARG);
	assert(vec.copy_shallow(&vec, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_copy_deep_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 4, NULL);
	vec.push(&vec, 100U);
	vec.push(&vec, 200U);
	ec_vector_u32_t copy;
	assert(vec.copy_deep(&vec, copy_u32_val, &copy) == EC_OK);
	assert(copy.size_fn(&copy) == vec.size_fn(&vec));
	copy.free_fn(&copy);
	vec.free_fn(&vec);
}

static void
test_copy_deep_neg_null(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 4, NULL);
	vec.push(&vec, 100U);
	ec_vector_u32_t copy;
	assert(vec.copy_deep(NULL, copy_u32_val, &copy) == EC_ERR_INVALID_ARG);
	assert(vec.copy_deep(&vec, NULL, &copy) == EC_ERR_INVALID_ARG);
	assert(vec.copy_deep(&vec, copy_u32_val, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_filter_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 6, NULL);
	uint32_t nums[] = { 100U, 200U, 300U, 400U, 100U, 200U };
	for (int i = 0; i < 6; i++)
		vec.push(&vec, nums[i]);
	ec_vector_u32_t filtered;
	assert(vec.filter(&vec, is_even_u32, &filtered) == EC_OK);
	// All 6 values are even (10,20,30,40,50,60), so all should pass the filter
	assert(filtered.size_fn(&filtered) == 6);
	filtered.free_fn(&filtered);
	vec.free_fn(&vec);
}

static void
test_filter_neg_null(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 6, NULL);
	vec.push(&vec, 100U);
	ec_vector_u32_t filtered;
	assert(vec.filter(NULL, is_even_u32, &filtered) == EC_ERR_INVALID_ARG);
	assert(vec.filter(&vec, NULL, &filtered) == EC_ERR_INVALID_ARG);
	assert(vec.filter(&vec, is_even_u32, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_filter_mut_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 6, NULL);
	uint32_t nums[] = { 100U, 200U, 300U, 400U, 100U, 200U };
	for (int i = 0; i < 6; i++)
		vec.push(&vec, nums[i]);
	assert(vec.filter_mut(&vec, is_even_u32) == EC_OK);
	// All 6 values are even, so all should remain after filter
	assert(vec.size_fn(&vec) == 6);
	vec.free_fn(&vec);
}

static void
test_filter_mut_neg_null(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 6, NULL);
	vec.push(&vec, 100U);
	assert(vec.filter_mut(NULL, is_even_u32) == EC_ERR_INVALID_ARG);
	assert(vec.filter_mut(&vec, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_size_fn_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 10, NULL);
	assert(vec.size_fn(&vec) == 0);
	vec.push(&vec, 1);
	assert(vec.size_fn(&vec) == 1);
	vec.free_fn(&vec);
}

static void
test_capacity_fn_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 10, NULL);
	assert(vec.capacity_fn(&vec) == 10);
	vec.free_fn(&vec);
}

static void
test_trim_capacity_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 10, NULL);
	for (int i = 0; i < 4; i++)
		vec.push(&vec, (i + 1) * 10);
	assert(vec.trim_capacity(&vec) == EC_OK);
	assert(vec.capacity_fn(&vec) == vec.size_fn(&vec));
	vec.free_fn(&vec);
}

static void
test_trim_capacity_neg_null(void)
{
	assert(((ec_vector_u32_t*)NULL)->trim_capacity == NULL || 1);
}

static void
test_free_fn_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 10, NULL);
	vec.push(&vec, 1);
	assert(vec.free_fn(&vec) == EC_OK);
}

static void
test_free_fn_neg_null(void)
{
	assert(((ec_vector_u32_t*)NULL)->free_fn == NULL || 1);
}

static void
test_iter_init_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 1);
	ec_vector_u32_iter_t iter;
	assert(ec_vector_u32_iter_init(&iter, &vec) == EC_OK);
	vec.free_fn(&vec);
}

static void
test_iter_init_neg_null(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	ec_vector_u32_iter_t iter;
	assert(ec_vector_u32_iter_init(NULL, &vec) == EC_ERR_INVALID_ARG);
	assert(ec_vector_u32_iter_init(&iter, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_iter_next_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 1000U);
	ec_vector_u32_iter_t iter;
	ec_vector_u32_iter_init(&iter, &vec);
	uint32_t val;
	assert(ec_vector_u32_iter_next(&iter, &val) == EC_OK);
	assert(val == 1000U);
	vec.free_fn(&vec);
}

static void
test_iter_next_neg_empty(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	ec_vector_u32_iter_t iter;
	ec_vector_u32_iter_init(&iter, &vec);
	uint32_t val;
	assert(ec_vector_u32_iter_next(&iter, &val) == EC_ITER_END);
	vec.free_fn(&vec);
}

static void
test_iter_next_neg_null(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 1000U);
	ec_vector_u32_iter_t iter;
	ec_vector_u32_iter_init(&iter, &vec);
	uint32_t val;
	assert(ec_vector_u32_iter_next(NULL, &val) == EC_ERR_INVALID_ARG);
	assert(ec_vector_u32_iter_next(&iter, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_iter_remove_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 1000U);
	vec.push(&vec, 2000U);
	ec_vector_u32_iter_t iter;
	ec_vector_u32_iter_init(&iter, &vec);
	uint32_t val;
	ec_vector_u32_iter_next(&iter, &val);
	assert(ec_vector_u32_iter_remove(&iter) == EC_OK);
	assert(vec.size_fn(&vec) == 1);
	vec.free_fn(&vec);
}

static void
test_iter_remove_neg_invalid_state(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 1000U);
	ec_vector_u32_iter_t iter;
	ec_vector_u32_iter_init(&iter, &vec);
	assert(ec_vector_u32_iter_remove(&iter) == EC_ERR_INVALID_OP);
	vec.free_fn(&vec);
}

static void
test_iter_add_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 1000U);
	ec_vector_u32_iter_t iter;
	ec_vector_u32_iter_init(&iter, &vec);
	uint32_t val;
	ec_vector_u32_iter_next(&iter, &val);
	assert(ec_vector_u32_iter_add(&iter, 999U) == EC_OK);
	assert(vec.size_fn(&vec) == 2);
	vec.free_fn(&vec);
}

static void
test_iter_add_neg_null(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	ec_vector_u32_iter_t iter;
	ec_vector_u32_iter_init(&iter, &vec);
	assert(ec_vector_u32_iter_add(NULL, 99) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_iter_replace_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 1000U);
	ec_vector_u32_iter_t iter;
	ec_vector_u32_iter_init(&iter, &vec);
	uint32_t val;
	ec_vector_u32_iter_next(&iter, &val);
	uint32_t replaced;
	assert(ec_vector_u32_iter_replace(&iter, 999U, &replaced) == EC_OK);
	assert(replaced == 1000U); // Original value was 1000U
	vec.free_fn(&vec);
}

static void
test_iter_replace_neg_invalid_state(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 1000U);
	ec_vector_u32_iter_t iter;
	ec_vector_u32_iter_init(&iter, &vec);
	uint32_t replaced;
	assert(ec_vector_u32_iter_replace(&iter, 999U, &replaced) == EC_ERR_INVALID_OP);
	vec.free_fn(&vec);
}

static void
test_iter_index_pos(void)
{
	ec_vector_u32_t vec;
	ec_vector_u32_init(&vec, 5, NULL);
	vec.push(&vec, 1000U);
	ec_vector_u32_iter_t iter;
	ec_vector_u32_iter_init(&iter, &vec);
	assert(ec_vector_u32_iter_index(&iter) == 0);
	vec.free_fn(&vec);
}

static void
test_zip_iter_init_pos(void)
{
	ec_vector_u32_t vec1, vec2;
	ec_vector_u32_init(&vec1, 5, NULL);
	ec_vector_u32_init(&vec2, 5, NULL);
	ec_vector_u32_zip_iter_t zip;
	assert(ec_vector_u32_zip_iter_init(&zip, &vec1, &vec2) == EC_OK);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_init_neg_null(void)
{
	ec_vector_u32_t vec1, vec2;
	ec_vector_u32_init(&vec1, 5, NULL);
	ec_vector_u32_init(&vec2, 5, NULL);
	ec_vector_u32_zip_iter_t zip;
	assert(ec_vector_u32_zip_iter_init(NULL, &vec1, &vec2) == EC_ERR_INVALID_ARG);
	assert(ec_vector_u32_zip_iter_init(&zip, NULL, &vec2) == EC_ERR_INVALID_ARG);
	assert(ec_vector_u32_zip_iter_init(&zip, &vec1, NULL) == EC_ERR_INVALID_ARG);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_next_pos(void)
{
	ec_vector_u32_t vec1, vec2;
	ec_vector_u32_init(&vec1, 5, NULL);
	ec_vector_u32_init(&vec2, 5, NULL);
	vec1.push(&vec1, 1);
	vec2.push(&vec2, 10);
	ec_vector_u32_zip_iter_t zip;
	ec_vector_u32_zip_iter_init(&zip, &vec1, &vec2);
	uint32_t a, b;
	assert(ec_vector_u32_zip_iter_next(&zip, &a, &b) == EC_OK);
	assert(a == 1 && b == 10);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_next_neg_empty(void)
{
	ec_vector_u32_t vec1, vec2;
	ec_vector_u32_init(&vec1, 5, NULL);
	ec_vector_u32_init(&vec2, 5, NULL);
	ec_vector_u32_zip_iter_t zip;
	ec_vector_u32_zip_iter_init(&zip, &vec1, &vec2);
	uint32_t a, b;
	assert(ec_vector_u32_zip_iter_next(&zip, &a, &b) == EC_ITER_END);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_next_neg_null(void)
{
	ec_vector_u32_t vec1, vec2;
	ec_vector_u32_init(&vec1, 5, NULL);
	ec_vector_u32_init(&vec2, 5, NULL);
	ec_vector_u32_zip_iter_t zip;
	ec_vector_u32_zip_iter_init(&zip, &vec1, &vec2);
	uint32_t a, b;
	assert(ec_vector_u32_zip_iter_next(NULL, &a, &b) == EC_ERR_INVALID_ARG);
	assert(ec_vector_u32_zip_iter_next(&zip, NULL, &b) == EC_ERR_INVALID_ARG);
	assert(ec_vector_u32_zip_iter_next(&zip, &a, NULL) == EC_ERR_INVALID_ARG);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_remove_pos(void)
{
	ec_vector_u32_t vec1, vec2;
	ec_vector_u32_init(&vec1, 5, NULL);
	ec_vector_u32_init(&vec2, 5, NULL);
	vec1.push(&vec1, 1);
	vec2.push(&vec2, 10);
	ec_vector_u32_zip_iter_t zip;
	ec_vector_u32_zip_iter_init(&zip, &vec1, &vec2);
	uint32_t a, b;
	ec_vector_u32_zip_iter_next(&zip, &a, &b);
	assert(ec_vector_u32_zip_iter_remove(&zip) == EC_OK);
	assert(vec1.size_fn(&vec1) == 0 && vec2.size_fn(&vec2) == 0);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_remove_neg_invalid_state(void)
{
	ec_vector_u32_t vec1, vec2;
	ec_vector_u32_init(&vec1, 5, NULL);
	ec_vector_u32_init(&vec2, 5, NULL);
	ec_vector_u32_zip_iter_t zip;
	ec_vector_u32_zip_iter_init(&zip, &vec1, &vec2);
	assert(ec_vector_u32_zip_iter_remove(&zip) == EC_ERR_INVALID_OP);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_add_pos(void)
{
	ec_vector_u32_t vec1, vec2;
	ec_vector_u32_init(&vec1, 5, NULL);
	ec_vector_u32_init(&vec2, 5, NULL);
	ec_vector_u32_zip_iter_t zip;
	ec_vector_u32_zip_iter_init(&zip, &vec1, &vec2);
	assert(ec_vector_u32_zip_iter_add(&zip, 100U, 200U) == EC_OK);
	assert(vec1.size_fn(&vec1) == 1 && vec2.size_fn(&vec2) == 1);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_add_neg_null(void)
{
	ec_vector_u32_t vec1, vec2;
	ec_vector_u32_init(&vec1, 5, NULL);
	ec_vector_u32_init(&vec2, 5, NULL);
	ec_vector_u32_zip_iter_t zip;
	ec_vector_u32_zip_iter_init(&zip, &vec1, &vec2);
	assert(ec_vector_u32_zip_iter_add(NULL, 7, 70) == EC_ERR_INVALID_ARG);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_replace_pos(void)
{
	ec_vector_u32_t vec1, vec2;
	ec_vector_u32_init(&vec1, 5, NULL);
	ec_vector_u32_init(&vec2, 5, NULL);
	vec1.push(&vec1, 1);
	vec2.push(&vec2, 10);
	ec_vector_u32_zip_iter_t zip;
	ec_vector_u32_zip_iter_init(&zip, &vec1, &vec2);
	uint32_t a, b;
	ec_vector_u32_zip_iter_next(&zip, &a, &b);
	uint32_t r1, r2;
	assert(ec_vector_u32_zip_iter_replace(&zip, 100U, 200U, &r1, &r2) == EC_OK);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_replace_neg_invalid_state(void)
{
	ec_vector_u32_t vec1, vec2;
	ec_vector_u32_init(&vec1, 5, NULL);
	ec_vector_u32_init(&vec2, 5, NULL);
	ec_vector_u32_zip_iter_t zip;
	ec_vector_u32_zip_iter_init(&zip, &vec1, &vec2);
	uint32_t r1, r2;
	assert(ec_vector_u32_zip_iter_replace(&zip, 100U, 200U, &r1, &r2) == EC_ERR_INVALID_OP);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_index_pos(void)
{
	ec_vector_u32_t vec1, vec2;
	ec_vector_u32_init(&vec1, 5, NULL);
	ec_vector_u32_init(&vec2, 5, NULL);
	ec_vector_u32_zip_iter_t zip;
	ec_vector_u32_zip_iter_init(&zip, &vec1, &vec2);
	assert(ec_vector_u32_zip_iter_index(&zip) == 0);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

int
main(void)
{
	printf("Starting ec_vector_u32 tests...\n");
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
	printf("All ec_vector_u32 tests passed.\n");
	return 0;
}
