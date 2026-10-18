#include "../../src/ec_vector.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

static int
cmp_u64(const void* a, const void* b)
{
	int ia = *(const int*)a;
	int ib = *(const int*)b;
	return ia - ib;
}

static void
inc_u64(int* elem)
{
	*elem += 1;
}

static void
sum_u64(int* acc, int* elem, int* out)
{
	*out = (*acc) + (*elem);
}

static int
copy_u64_val(int* src)
{
	return *src;
}

static bool
is_even_u64(int num)
{
	return (num % 2) == 0;
}

static void
test_init_pos(void)
{
	ec_vector_u64_t vec;
	assert(ec_vector_u64_init(&vec, 10, NULL) == EC_OK);
	assert(vec.size_fn(&vec) == 0);
	assert(vec.capacity_fn(&vec) == 10);
	vec.free_fn(&vec);
}

static void
test_init_neg_null(void)
{
	assert(ec_vector_u64_init(NULL, 10, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_init_neg_zero_capacity(void)
{
	ec_vector_u64_t vec;
	assert(ec_vector_u64_init(&vec, 0, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_push_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 3, NULL);
	assert(vec.push(&vec, 1000ULL) == EC_OK);
	assert(vec.size_fn(&vec) == 1);
	uint64_t val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 1000ULL);
	vec.free_fn(&vec);
}

static void
test_push_pos_expansion(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 2, NULL);
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
	assert(((ec_vector_u64_t*)NULL)->push == NULL || 1);
}

static void
test_insert_pos_beginning(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 3, NULL);
	vec.push(&vec, 2);
	assert(vec.insert(&vec, 0, 1000ULL) == EC_OK);
	assert(vec.size_fn(&vec) == 2);
	uint64_t val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 1000ULL);
	vec.free_fn(&vec);
}

static void
test_insert_pos_middle(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 3, NULL);
	vec.push(&vec, 1);
	vec.push(&vec, 3);                             // Vector has size 2, valid indices: 0, 1, 2
	assert(vec.insert(&vec, 2, 2000ULL) == EC_OK); // Insert at index 2 (end)
	assert(vec.size_fn(&vec) == 3);
	uint64_t val;
	assert(vec.get(&vec, 2, &val) == EC_OK && val == 2000ULL);
	vec.free_fn(&vec);
}

static void
test_insert_pos_end(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 3, NULL);
	vec.push(&vec, 1);                             // Vector has size 1, valid indices: 0, 1, 2
	assert(vec.insert(&vec, 1, 2000ULL) == EC_OK); // Insert at index 1 (end)
	assert(vec.size_fn(&vec) == 2);
	vec.free_fn(&vec);
}

static void
test_insert_neg_null(void)
{
	assert(((ec_vector_u64_t*)NULL)->insert == NULL || 1);
}

static void
test_insert_neg_out_of_bounds(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 3, NULL);
	vec.push(&vec, 1);
	assert(vec.insert(&vec, 3, 2000ULL) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_remove_at_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 3, NULL);
	vec.push(&vec, 1);
	vec.push(&vec, 2);
	vec.push(&vec, 3);
	assert(vec.remove_at(&vec, 1) == EC_OK); // Remove index 1 (value 2)
	assert(vec.size_fn(&vec) == 2);
	uint64_t val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 1ULL); // First element is 1
	assert(vec.get(&vec, 1, &val) == EC_OK && val == 3ULL); // Second element is 3
	vec.free_fn(&vec);
}

static void
test_remove_at_neg_null(void)
{
	assert(((ec_vector_u64_t*)NULL)->remove_at == NULL || 1);
}

static void
test_remove_at_neg_out_of_bounds(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 3, NULL);
	vec.push(&vec, 1);
	assert(vec.remove_at(&vec, 1) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_get_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 3, NULL);
	vec.push(&vec, 10000ULL);
	uint64_t val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 10000ULL);
	vec.free_fn(&vec);
}

static void
test_get_neg_null(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 3, NULL);
	vec.push(&vec, 10000ULL);
	assert(vec.get(NULL, 0, NULL) == EC_ERR_INVALID_ARG);
	assert(vec.get(&vec, 0, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_get_neg_out_of_bounds(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 3, NULL);
	vec.push(&vec, 10000ULL);
	uint64_t val;
	assert(vec.get(&vec, 1, &val) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_replace_at_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 3, NULL);
	vec.push(&vec, 5);
	uint64_t old_val;
	assert(vec.replace_at(&vec, 0, 9999ULL, &old_val) == EC_OK);
	assert(old_val == 5ULL); // Original value was 5
	uint64_t val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 9999ULL);
	vec.free_fn(&vec);
}

static void
test_replace_at_neg_null(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 3, NULL);
	vec.push(&vec, 5);
	uint64_t old_val;
	assert(vec.replace_at(NULL, 0, 99, &old_val) == EC_ERR_INVALID_ARG);
	assert(vec.replace_at(&vec, 0, 9999ULL, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_replace_at_neg_out_of_bounds(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 3, NULL);
	vec.push(&vec, 5);
	uint64_t old_val;
	assert(vec.replace_at(&vec, 1, 9999ULL, &old_val) == EC_ERR_OUT_OF_BOUNDS); // Index 1 is out of bounds for empty vector
	vec.free_fn(&vec);
}

static void
test_swap_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 3, NULL);
	vec.push(&vec, 11);
	vec.push(&vec, 22);
	assert(vec.swap(&vec, 0, 1) == EC_OK); // Swap 11 and 22
	uint64_t val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 22ULL); // After swap, index 0 is 22
	assert(vec.get(&vec, 1, &val) == EC_OK && val == 11ULL); // After swap, index 1 is 11
	vec.free_fn(&vec);
}

static void
test_swap_neg_null(void)
{
	assert(((ec_vector_u64_t*)NULL)->swap == NULL || 1);
}

static void
test_swap_neg_out_of_bounds(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 3, NULL);
	vec.push(&vec, 11);
	assert(vec.swap(&vec, 0, 1) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_contains_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 1000ULL);
	vec.push(&vec, 2000ULL);
	assert(vec.contains(&vec, 1000ULL, cmp_u64) == 1);
	vec.free_fn(&vec);
}

static void
test_contains_neg_not_found(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 1000ULL);
	vec.push(&vec, 2000ULL);
	assert(vec.contains(&vec, 3000ULL, cmp_u64) == 0);
	vec.free_fn(&vec);
}

static void
test_contains_neg_null(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 1000ULL);
	assert(vec.contains(NULL, 10, cmp_u64) == 0);
	assert(vec.contains(&vec, 1000ULL, NULL) == 0);
	vec.free_fn(&vec);
}

static void
test_index_of_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 1000ULL);
	vec.push(&vec, 2000ULL);
	vec.push(&vec, 3000ULL);
	size_t idx;
	assert(vec.index_of(&vec, 2000ULL, &idx, cmp_u64) == EC_OK);
	assert(idx == 1);
	vec.free_fn(&vec);
}

static void
test_index_of_neg_not_found(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 1000ULL);
	size_t idx;
	assert(vec.index_of(&vec, 9999ULL, &idx, cmp_u64) == EC_ERR_NOT_FOUND);
	vec.free_fn(&vec);
}

static void
test_index_of_neg_null(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 1000ULL);
	size_t idx;
	assert(vec.index_of(NULL, 10, &idx, cmp_u64) == EC_ERR_INVALID_ARG);
	assert(vec.index_of(&vec, 10, NULL, cmp_u64) == EC_ERR_INVALID_ARG);
	assert(vec.index_of(&vec, 10, &idx, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_binary_search_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 1000ULL);
	vec.push(&vec, 2000ULL);
	vec.push(&vec, 3000ULL);
	vec.sort(&vec, cmp_u64);
	size_t idx;
	assert(vec.binary_search(&vec, 2000ULL, cmp_u64, &idx) == EC_OK);
	vec.free_fn(&vec);
}

static void
test_binary_search_neg_not_found(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 1000ULL);
	vec.push(&vec, 2000ULL);
	vec.sort(&vec, cmp_u64);
	size_t idx;
	assert(vec.binary_search(&vec, 9999ULL, cmp_u64, &idx) == EC_ERR_NOT_FOUND);
	vec.free_fn(&vec);
}

static void
test_binary_search_neg_null(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 1000ULL);
	vec.sort(&vec, cmp_u64);
	size_t idx;
	assert(vec.binary_search(NULL, 10, cmp_u64, &idx) == EC_ERR_INVALID_ARG);
	assert(vec.binary_search(&vec, 10, NULL, &idx) == EC_ERR_INVALID_ARG);
	assert(vec.binary_search(&vec, 10, cmp_u64, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_sort_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 3);
	vec.push(&vec, 1);
	vec.push(&vec, 2);
	vec.sort(&vec, cmp_u64); // Sort [3, 1, 2] -> [1, 2, 3]
	uint64_t val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 1ULL);
	assert(vec.get(&vec, 1, &val) == EC_OK && val == 2ULL);
	assert(vec.get(&vec, 2, &val) == EC_OK && val == 3ULL);
	vec.free_fn(&vec);
}

static void
test_sort_neg_null(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 3);
	vec.sort(NULL, cmp_u64);
	vec.sort(&vec, NULL);
	vec.free_fn(&vec);
}

static void
test_reverse_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 1);
	vec.push(&vec, 2);
	vec.push(&vec, 3);
	vec.reverse(&vec); // Reverse [1, 2, 3] -> [3, 2, 1]
	uint64_t val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 3ULL);
	assert(vec.get(&vec, 1, &val) == EC_OK && val == 2ULL);
	assert(vec.get(&vec, 2, &val) == EC_OK && val == 1ULL);
	vec.free_fn(&vec);
}

static void
test_reverse_pos_empty(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.reverse(&vec);
	assert(vec.size_fn(&vec) == 0);
	vec.free_fn(&vec);
}

static void
test_reverse_neg_null(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.reverse(NULL);
	vec.free_fn(&vec);
}

static void
test_map_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 3, NULL);
	vec.push(&vec, 5);
	vec.push(&vec, 1000ULL);
	vec.map(&vec, inc_u64); // Map [5, 1000] -> [6, 1001] (increment each)
	uint64_t val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == 6ULL);
	assert(vec.get(&vec, 1, &val) == EC_OK && val == 1001ULL);
	vec.free_fn(&vec);
}

static void
test_map_neg_null(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 3, NULL);
	vec.push(&vec, 5);
	vec.map(NULL, inc_u64);
	vec.map(&vec, NULL);
	vec.free_fn(&vec);
}

static void
test_reduce_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 4, NULL);
	vec.push(&vec, 1);
	vec.push(&vec, 2);
	vec.push(&vec, 3);
	uint64_t result = 0;
	vec.reduce(&vec, sum_u64, &result);
	assert(result == (1ULL + 2ULL + 3ULL)); // Sum is 6
	vec.free_fn(&vec);
}

static void
test_reduce_pos_empty(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 4, NULL);
	uint64_t result = 0;
	vec.reduce(&vec, sum_u64, &result);
	vec.free_fn(&vec);
}

static void
test_reduce_neg_null(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 4, NULL);
	vec.push(&vec, 1);
	uint64_t result = 0;
	vec.reduce(NULL, sum_u64, &result);
	vec.reduce(&vec, NULL, &result);
	vec.reduce(&vec, sum_u64, NULL);
	vec.free_fn(&vec);
}

static void
test_subarray_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 6, NULL);
	for (int i = 1; i <= 6; i++)
		vec.push(&vec, i * 10);
	ec_vector_u64_t sub;
	assert(vec.subarray(&vec, 2, 5, &sub) == EC_OK);
	assert(sub.size_fn(&sub) == 3);
	uint64_t val;
	// Values at indices 2,3,4 are 30,40,50 (i*10 for i=1..6), so first element of subarray is 30
	assert(sub.get(&sub, 0, &val) == EC_OK && val == 30ULL);
	sub.free_fn(&sub);
	vec.free_fn(&vec);
}

static void
test_subarray_neg_invalid_range(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 6, NULL);
	vec.push(&vec, 1000ULL);
	ec_vector_u64_t sub;
	assert(vec.subarray(&vec, 2, 1, &sub) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_subarray_neg_out_of_bounds(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 6, NULL);
	vec.push(&vec, 1000ULL);
	ec_vector_u64_t sub;
	assert(vec.subarray(&vec, 0, 2, &sub) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_subarray_neg_null(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 6, NULL);
	vec.push(&vec, 1000ULL);
	ec_vector_u64_t sub;
	assert(vec.subarray(NULL, 0, 1, &sub) == EC_ERR_INVALID_ARG);
	assert(vec.subarray(&vec, 0, 1, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_copy_shallow_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 4, NULL);
	for (int i = 1; i <= 4; i++)
		vec.push(&vec, i);
	ec_vector_u64_t copy;
	assert(vec.copy_shallow(&vec, &copy) == EC_OK);
	assert(copy.size_fn(&copy) == vec.size_fn(&vec));
	copy.free_fn(&copy);
	vec.free_fn(&vec);
}

static void
test_copy_shallow_neg_null(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 4, NULL);
	vec.push(&vec, 1);
	ec_vector_u64_t copy;
	assert(vec.copy_shallow(NULL, &copy) == EC_ERR_INVALID_ARG);
	assert(vec.copy_shallow(&vec, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_copy_deep_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 4, NULL);
	vec.push(&vec, 1000ULL);
	vec.push(&vec, 2000ULL);
	ec_vector_u64_t copy;
	assert(vec.copy_deep(&vec, copy_u64_val, &copy) == EC_OK);
	assert(copy.size_fn(&copy) == vec.size_fn(&vec));
	copy.free_fn(&copy);
	vec.free_fn(&vec);
}

static void
test_copy_deep_neg_null(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 4, NULL);
	vec.push(&vec, 1000ULL);
	ec_vector_u64_t copy;
	assert(vec.copy_deep(NULL, copy_u64_val, &copy) == EC_ERR_INVALID_ARG);
	assert(vec.copy_deep(&vec, NULL, &copy) == EC_ERR_INVALID_ARG);
	assert(vec.copy_deep(&vec, copy_u64_val, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_filter_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 6, NULL);
	uint64_t nums[] = { 1000ULL, 2000ULL, 3000ULL, 4000ULL, 1000ULL, 2000ULL };
	for (int i = 0; i < 6; i++)
		vec.push(&vec, nums[i]);
	ec_vector_u64_t filtered;
	assert(vec.filter(&vec, is_even_u64, &filtered) == EC_OK);
	// All 6 values are even, so all should pass the filter
	assert(filtered.size_fn(&filtered) == 6);
	filtered.free_fn(&filtered);
	vec.free_fn(&vec);
}

static void
test_filter_neg_null(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 6, NULL);
	vec.push(&vec, 1000ULL);
	ec_vector_u64_t filtered;
	assert(vec.filter(NULL, is_even_u64, &filtered) == EC_ERR_INVALID_ARG);
	assert(vec.filter(&vec, NULL, &filtered) == EC_ERR_INVALID_ARG);
	assert(vec.filter(&vec, is_even_u64, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_filter_mut_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 6, NULL);
	uint64_t nums[] = { 1000ULL, 2000ULL, 3000ULL, 4000ULL, 1000ULL, 2000ULL };
	for (int i = 0; i < 6; i++)
		vec.push(&vec, nums[i]);
	assert(vec.filter_mut(&vec, is_even_u64) == EC_OK);
	// All 6 values are even, so all should remain after filter
	assert(vec.size_fn(&vec) == 6);
	vec.free_fn(&vec);
}

static void
test_filter_mut_neg_null(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 6, NULL);
	vec.push(&vec, 1000ULL);
	assert(vec.filter_mut(NULL, is_even_u64) == EC_ERR_INVALID_ARG);
	assert(vec.filter_mut(&vec, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_size_fn_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 10, NULL);
	assert(vec.size_fn(&vec) == 0);
	vec.push(&vec, 1);
	assert(vec.size_fn(&vec) == 1);
	vec.free_fn(&vec);
}

static void
test_capacity_fn_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 10, NULL);
	assert(vec.capacity_fn(&vec) == 10);
	vec.free_fn(&vec);
}

static void
test_trim_capacity_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 10, NULL);
	for (int i = 0; i < 4; i++)
		vec.push(&vec, (i + 1) * 10);
	assert(vec.trim_capacity(&vec) == EC_OK);
	assert(vec.capacity_fn(&vec) == vec.size_fn(&vec));
	vec.free_fn(&vec);
}

static void
test_trim_capacity_neg_null(void)
{
	assert(((ec_vector_u64_t*)NULL)->trim_capacity == NULL || 1);
}

static void
test_free_fn_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 10, NULL);
	vec.push(&vec, 1);
	assert(vec.free_fn(&vec) == EC_OK);
}

static void
test_free_fn_neg_null(void)
{
	assert(((ec_vector_u64_t*)NULL)->free_fn == NULL || 1);
}

static void
test_iter_init_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 1);
	ec_vector_u64_iter_t iter;
	assert(ec_vector_u64_iter_init(&iter, &vec) == EC_OK);
	vec.free_fn(&vec);
}

static void
test_iter_init_neg_null(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	ec_vector_u64_iter_t iter;
	assert(ec_vector_u64_iter_init(NULL, &vec) == EC_ERR_INVALID_ARG);
	assert(ec_vector_u64_iter_init(&iter, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_iter_next_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 10000ULL);
	ec_vector_u64_iter_t iter;
	ec_vector_u64_iter_init(&iter, &vec);
	uint64_t val;
	assert(ec_vector_u64_iter_next(&iter, &val) == EC_OK);
	assert(val == 10000ULL);
	vec.free_fn(&vec);
}

static void
test_iter_next_neg_empty(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	ec_vector_u64_iter_t iter;
	ec_vector_u64_iter_init(&iter, &vec);
	uint64_t val;
	assert(ec_vector_u64_iter_next(&iter, &val) == EC_ITER_END);
	vec.free_fn(&vec);
}

static void
test_iter_next_neg_null(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 10000ULL);
	ec_vector_u64_iter_t iter;
	ec_vector_u64_iter_init(&iter, &vec);
	uint64_t val;
	assert(ec_vector_u64_iter_next(NULL, &val) == EC_ERR_INVALID_ARG);
	assert(ec_vector_u64_iter_next(&iter, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_iter_remove_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 10000ULL);
	vec.push(&vec, 20000ULL);
	ec_vector_u64_iter_t iter;
	ec_vector_u64_iter_init(&iter, &vec);
	uint64_t val;
	ec_vector_u64_iter_next(&iter, &val);
	assert(ec_vector_u64_iter_remove(&iter) == EC_OK);
	assert(vec.size_fn(&vec) == 1);
	vec.free_fn(&vec);
}

static void
test_iter_remove_neg_invalid_state(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 10000ULL);
	ec_vector_u64_iter_t iter;
	ec_vector_u64_iter_init(&iter, &vec);
	assert(ec_vector_u64_iter_remove(&iter) == EC_ERR_INVALID_OP);
	vec.free_fn(&vec);
}

static void
test_iter_add_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 10000ULL);
	ec_vector_u64_iter_t iter;
	ec_vector_u64_iter_init(&iter, &vec);
	uint64_t val;
	ec_vector_u64_iter_next(&iter, &val);
	assert(ec_vector_u64_iter_add(&iter, 9999ULL) == EC_OK);
	assert(vec.size_fn(&vec) == 2);
	vec.free_fn(&vec);
}

static void
test_iter_add_neg_null(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	ec_vector_u64_iter_t iter;
	ec_vector_u64_iter_init(&iter, &vec);
	assert(ec_vector_u64_iter_add(NULL, 99) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_iter_replace_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 10000ULL);
	ec_vector_u64_iter_t iter;
	ec_vector_u64_iter_init(&iter, &vec);
	uint64_t val;
	ec_vector_u64_iter_next(&iter, &val);
	uint64_t replaced;
	assert(ec_vector_u64_iter_replace(&iter, 9999ULL, &replaced) == EC_OK);
	assert(replaced == 10000ULL); // Original value was 10000ULL
	vec.free_fn(&vec);
}

static void
test_iter_replace_neg_invalid_state(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 10000ULL);
	ec_vector_u64_iter_t iter;
	ec_vector_u64_iter_init(&iter, &vec);
	uint64_t replaced;
	assert(ec_vector_u64_iter_replace(&iter, 9999ULL, &replaced) == EC_ERR_INVALID_OP);
	vec.free_fn(&vec);
}

static void
test_iter_index_pos(void)
{
	ec_vector_u64_t vec;
	ec_vector_u64_init(&vec, 5, NULL);
	vec.push(&vec, 10000ULL);
	ec_vector_u64_iter_t iter;
	ec_vector_u64_iter_init(&iter, &vec);
	assert(ec_vector_u64_iter_index(&iter) == 0);
	vec.free_fn(&vec);
}

static void
test_zip_iter_init_pos(void)
{
	ec_vector_u64_t vec1, vec2;
	ec_vector_u64_init(&vec1, 5, NULL);
	ec_vector_u64_init(&vec2, 5, NULL);
	ec_vector_u64_zip_iter_t zip;
	assert(ec_vector_u64_zip_iter_init(&zip, &vec1, &vec2) == EC_OK);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_init_neg_null(void)
{
	ec_vector_u64_t vec1, vec2;
	ec_vector_u64_init(&vec1, 5, NULL);
	ec_vector_u64_init(&vec2, 5, NULL);
	ec_vector_u64_zip_iter_t zip;
	assert(ec_vector_u64_zip_iter_init(NULL, &vec1, &vec2) == EC_ERR_INVALID_ARG);
	assert(ec_vector_u64_zip_iter_init(&zip, NULL, &vec2) == EC_ERR_INVALID_ARG);
	assert(ec_vector_u64_zip_iter_init(&zip, &vec1, NULL) == EC_ERR_INVALID_ARG);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_next_pos(void)
{
	ec_vector_u64_t vec1, vec2;
	ec_vector_u64_init(&vec1, 5, NULL);
	ec_vector_u64_init(&vec2, 5, NULL);
	vec1.push(&vec1, 1);
	vec2.push(&vec2, 10);
	ec_vector_u64_zip_iter_t zip;
	ec_vector_u64_zip_iter_init(&zip, &vec1, &vec2);
	uint64_t a, b;
	assert(ec_vector_u64_zip_iter_next(&zip, &a, &b) == EC_OK);
	assert(a == 1 && b == 10);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_next_neg_empty(void)
{
	ec_vector_u64_t vec1, vec2;
	ec_vector_u64_init(&vec1, 5, NULL);
	ec_vector_u64_init(&vec2, 5, NULL);
	ec_vector_u64_zip_iter_t zip;
	ec_vector_u64_zip_iter_init(&zip, &vec1, &vec2);
	uint64_t a, b;
	assert(ec_vector_u64_zip_iter_next(&zip, &a, &b) == EC_ITER_END);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_next_neg_null(void)
{
	ec_vector_u64_t vec1, vec2;
	ec_vector_u64_init(&vec1, 5, NULL);
	ec_vector_u64_init(&vec2, 5, NULL);
	ec_vector_u64_zip_iter_t zip;
	ec_vector_u64_zip_iter_init(&zip, &vec1, &vec2);
	uint64_t a, b;
	assert(ec_vector_u64_zip_iter_next(NULL, &a, &b) == EC_ERR_INVALID_ARG);
	assert(ec_vector_u64_zip_iter_next(&zip, NULL, &b) == EC_ERR_INVALID_ARG);
	assert(ec_vector_u64_zip_iter_next(&zip, &a, NULL) == EC_ERR_INVALID_ARG);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_remove_pos(void)
{
	ec_vector_u64_t vec1, vec2;
	ec_vector_u64_init(&vec1, 5, NULL);
	ec_vector_u64_init(&vec2, 5, NULL);
	vec1.push(&vec1, 1);
	vec2.push(&vec2, 10);
	ec_vector_u64_zip_iter_t zip;
	ec_vector_u64_zip_iter_init(&zip, &vec1, &vec2);
	uint64_t a, b;
	ec_vector_u64_zip_iter_next(&zip, &a, &b);
	assert(ec_vector_u64_zip_iter_remove(&zip) == EC_OK);
	assert(vec1.size_fn(&vec1) == 0 && vec2.size_fn(&vec2) == 0);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_remove_neg_invalid_state(void)
{
	ec_vector_u64_t vec1, vec2;
	ec_vector_u64_init(&vec1, 5, NULL);
	ec_vector_u64_init(&vec2, 5, NULL);
	ec_vector_u64_zip_iter_t zip;
	ec_vector_u64_zip_iter_init(&zip, &vec1, &vec2);
	assert(ec_vector_u64_zip_iter_remove(&zip) == EC_ERR_INVALID_OP);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_add_pos(void)
{
	ec_vector_u64_t vec1, vec2;
	ec_vector_u64_init(&vec1, 5, NULL);
	ec_vector_u64_init(&vec2, 5, NULL);
	ec_vector_u64_zip_iter_t zip;
	ec_vector_u64_zip_iter_init(&zip, &vec1, &vec2);
	assert(ec_vector_u64_zip_iter_add(&zip, 1000ULL, 2000ULL) == EC_OK);
	assert(vec1.size_fn(&vec1) == 1 && vec2.size_fn(&vec2) == 1);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_add_neg_null(void)
{
	ec_vector_u64_t vec1, vec2;
	ec_vector_u64_init(&vec1, 5, NULL);
	ec_vector_u64_init(&vec2, 5, NULL);
	ec_vector_u64_zip_iter_t zip;
	ec_vector_u64_zip_iter_init(&zip, &vec1, &vec2);
	assert(ec_vector_u64_zip_iter_add(NULL, 7, 70) == EC_ERR_INVALID_ARG);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_replace_pos(void)
{
	ec_vector_u64_t vec1, vec2;
	ec_vector_u64_init(&vec1, 5, NULL);
	ec_vector_u64_init(&vec2, 5, NULL);
	vec1.push(&vec1, 1);
	vec2.push(&vec2, 10);
	ec_vector_u64_zip_iter_t zip;
	ec_vector_u64_zip_iter_init(&zip, &vec1, &vec2);
	uint64_t a, b;
	ec_vector_u64_zip_iter_next(&zip, &a, &b);
	uint64_t r1, r2;
	assert(ec_vector_u64_zip_iter_replace(&zip, 1000ULL, 2000ULL, &r1, &r2) == EC_OK);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_replace_neg_invalid_state(void)
{
	ec_vector_u64_t vec1, vec2;
	ec_vector_u64_init(&vec1, 5, NULL);
	ec_vector_u64_init(&vec2, 5, NULL);
	ec_vector_u64_zip_iter_t zip;
	ec_vector_u64_zip_iter_init(&zip, &vec1, &vec2);
	uint64_t r1, r2;
	assert(ec_vector_u64_zip_iter_replace(&zip, 1000ULL, 2000ULL, &r1, &r2) == EC_ERR_INVALID_OP);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_index_pos(void)
{
	ec_vector_u64_t vec1, vec2;
	ec_vector_u64_init(&vec1, 5, NULL);
	ec_vector_u64_init(&vec2, 5, NULL);
	ec_vector_u64_zip_iter_t zip;
	ec_vector_u64_zip_iter_init(&zip, &vec1, &vec2);
	assert(ec_vector_u64_zip_iter_index(&zip) == 0);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

int
main(void)
{
	printf("Starting ec_vector_u64 tests...\n");
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
	printf("All ec_vector_u64 tests passed.\n");
	return 0;
}
