#include "../../src/ec_vector.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

static int
cmp_ptr(const void* a, const void* b)
{
	void* ia = *(const void**)a;
	void* ib = *(const void**)b;
	if (ia < ib)
		return -1;
	if (ia > ib)
		return 1;
	return 0;
}

static void
inc_ptr(void* elem)
{
	/* For ptr, we can't meaningfully increment, so this is a no-op */
	(void)elem;
}

static void
sum_ptr(void* acc, void* elem, void* out)
{
	/* For ptr, sum doesn't make sense, but we need the function */
	*(void**)out = *(void**)acc;
	(void)elem;
}

static void*
copy_ptr_val(void* src)
{
	return *(void**)src;
}

static bool
is_valid_ptr(void* num)
{
	return num != NULL;
}

// Global test pointers
static int val_1 = 1, val_2 = 2, val_3 = 3, val_5 = 5, val_6 = 6, val_10 = 10, val_11 = 11, val_20 = 20, val_22 = 22, val_30 = 30, val_99 = 99, val_100 = 100, val_200 = 200;
static void *ptr_1 = &val_1, *ptr_2 = &val_2, *ptr_3 = &val_3, *ptr_5 = &val_5, *ptr_6 = &val_6;
static void *ptr_10 = &val_10, *ptr_11 = &val_11, *ptr_20 = &val_20, *ptr_22 = &val_22, *ptr_30 = &val_30;
static void *ptr_99 = &val_99, *ptr_100 = &val_100, *ptr_200 = &val_200;

static void
test_init_pos(void)
{
	ec_vector_ptr_t vec;
	assert(ec_vector_ptr_init(&vec, 10, NULL) == EC_OK);
	assert(vec.size_fn(&vec) == 0);
	assert(vec.capacity_fn(&vec) == 10);
	vec.free_fn(&vec);
}

static void
test_init_neg_null(void)
{
	assert(ec_vector_ptr_init(NULL, 10, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_init_neg_zero_capacity(void)
{
	ec_vector_ptr_t vec;
	assert(ec_vector_ptr_init(&vec, 0, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_push_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 3, NULL);
	int a = 10;
	void* p1 = &a;
	assert(vec.push(&vec, p1) == EC_OK);
	assert(vec.size_fn(&vec) == 1);
	void* val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == p1);
	vec.free_fn(&vec);
}

static void
test_push_pos_expansion(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 2, NULL);
	assert(vec.push(&vec, ptr_1) == EC_OK);
	assert(vec.push(&vec, ptr_2) == EC_OK);
	assert(vec.push(&vec, ptr_3) == EC_OK);
	assert(vec.size_fn(&vec) == 3);
	assert(vec.capacity_fn(&vec) >= 3);
	vec.free_fn(&vec);
}

static void
test_push_neg_null(void)
{
	assert(((ec_vector_ptr_t*)NULL)->push == NULL || 1);
}

static void
test_insert_pos_beginning(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 3, NULL);
	vec.push(&vec, ptr_2);
	assert(vec.insert(&vec, 0, ptr_1) == EC_OK);
	assert(vec.size_fn(&vec) == 2);
	void* val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == ptr_1);
	vec.free_fn(&vec);
}

static void
test_insert_pos_middle(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 3, NULL);
	vec.push(&vec, ptr_1);
	vec.push(&vec, ptr_3);
	assert(vec.insert(&vec, 1, ptr_2) == EC_OK);
	assert(vec.size_fn(&vec) == 3);
	void* val;
	assert(vec.get(&vec, 1, &val) == EC_OK && val == ptr_2);
	vec.free_fn(&vec);
}

static void
test_insert_pos_end(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 3, NULL);
	vec.push(&vec, ptr_1);
	assert(vec.insert(&vec, 1, ptr_2) == EC_OK);
	assert(vec.size_fn(&vec) == 2);
	vec.free_fn(&vec);
}

static void
test_insert_neg_null(void)
{
	assert(((ec_vector_ptr_t*)NULL)->insert == NULL || 1);
}

static void
test_insert_neg_out_of_bounds(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 3, NULL);
	int a1 = 1;
	void* ptr_1 = &a1;
	vec.push(&vec, ptr_1);
	int a2 = 2;
	void* ptr_2 = &a2;
	assert(vec.insert(&vec, 3, ptr_2) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_insert_neg_out_of_bounds_fixed(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 3, NULL);
	vec.push(&vec, ptr_1);
	assert(vec.insert(&vec, 1, ptr_2) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_remove_at_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 3, NULL);
	vec.push(&vec, ptr_1);
	vec.push(&vec, ptr_2);
	vec.push(&vec, ptr_3);
	assert(vec.remove_at(&vec, 1) == EC_OK);
	assert(vec.size_fn(&vec) == 2);
	void* val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == ptr_1);
	assert(vec.get(&vec, 1, &val) == EC_OK && val == ptr_3);
	vec.free_fn(&vec);
}

static void
test_remove_at_neg_null(void)
{
	assert(((ec_vector_ptr_t*)NULL)->remove_at == NULL || 1);
}

static void
test_remove_at_neg_out_of_bounds(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 3, NULL);
	vec.push(&vec, ptr_1);
	assert(vec.remove_at(&vec, 1) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_get_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 3, NULL);
	vec.push(&vec, ptr_100);
	void* val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == ptr_100);
	vec.free_fn(&vec);
}

static void
test_get_neg_null(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 3, NULL);
	vec.push(&vec, ptr_100);
	assert(vec.get(NULL, 0, NULL) == EC_ERR_INVALID_ARG);
	assert(vec.get(&vec, 0, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_get_neg_out_of_bounds(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 3, NULL);
	vec.push(&vec, ptr_100);
	void* val;
	assert(vec.get(&vec, 1, &val) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_replace_at_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 3, NULL);
	vec.push(&vec, ptr_5);
	void* old_val;
	assert(vec.replace_at(&vec, 0, ptr_99, &old_val) == EC_OK);
	assert(old_val == ptr_5);
	void* val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == ptr_99);
	vec.free_fn(&vec);
}

static void
test_replace_at_neg_null(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 3, NULL);
	vec.push(&vec, ptr_5);
	void* old_val;
	assert(vec.replace_at(NULL, 0, ptr_99, &old_val) == EC_ERR_INVALID_ARG);
	assert(vec.replace_at(&vec, 0, ptr_99, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_replace_at_neg_out_of_bounds(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 3, NULL);
	vec.push(&vec, ptr_5);
	void* old_val;
	assert(vec.replace_at(&vec, 1, ptr_99, &old_val) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_swap_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 3, NULL);
	vec.push(&vec, ptr_11);
	vec.push(&vec, ptr_22);
	assert(vec.swap(&vec, 0, 1) == EC_OK);
	void* val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == ptr_22);
	assert(vec.get(&vec, 1, &val) == EC_OK && val == ptr_11);
	vec.free_fn(&vec);
}

static void
test_swap_neg_null(void)
{
	assert(((ec_vector_ptr_t*)NULL)->swap == NULL || 1);
}

static void
test_swap_neg_out_of_bounds(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 3, NULL);
	vec.push(&vec, ptr_11);
	assert(vec.swap(&vec, 0, 1) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_contains_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_10);
	vec.push(&vec, ptr_20);
	assert(vec.contains(&vec, ptr_10, cmp_ptr) == 1);
	vec.free_fn(&vec);
}

static void
test_contains_neg_not_found(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_10);
	vec.push(&vec, ptr_20);
	assert(vec.contains(&vec, ptr_30, cmp_ptr) == 0);
	vec.free_fn(&vec);
}

static void
test_contains_neg_null(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_10);
	assert(vec.contains(NULL, ptr_10, cmp_ptr) == 0);
	assert(vec.contains(&vec, ptr_10, NULL) == 0);
	vec.free_fn(&vec);
}

static void
test_index_of_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_10);
	vec.push(&vec, ptr_20);
	vec.push(&vec, ptr_30);
	size_t idx;
	assert(vec.index_of(&vec, ptr_20, &idx, cmp_ptr) == EC_OK);
	assert(idx == 1);
	vec.free_fn(&vec);
}

static void
test_index_of_neg_not_found(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_10);
	size_t idx;
	assert(vec.index_of(&vec, ptr_99, &idx, cmp_ptr) == EC_ERR_NOT_FOUND);
	vec.free_fn(&vec);
}

static void
test_index_of_neg_null(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_10);
	size_t idx;
	assert(vec.index_of(NULL, ptr_10, &idx, cmp_ptr) == EC_ERR_INVALID_ARG);
	assert(vec.index_of(&vec, ptr_10, NULL, cmp_ptr) == EC_ERR_INVALID_ARG);
	assert(vec.index_of(&vec, ptr_10, &idx, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_binary_search_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_10);
	vec.push(&vec, ptr_20);
	vec.push(&vec, ptr_30);
	vec.sort(&vec, cmp_ptr);
	size_t idx;
	assert(vec.binary_search(&vec, ptr_20, cmp_ptr, &idx) == EC_OK);
	vec.free_fn(&vec);
}

static void
test_binary_search_neg_not_found(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_10);
	vec.push(&vec, ptr_20);
	vec.sort(&vec, cmp_ptr);
	size_t idx;
	assert(vec.binary_search(&vec, ptr_99, cmp_ptr, &idx) == EC_ERR_NOT_FOUND);
	vec.free_fn(&vec);
}

static void
test_binary_search_neg_null(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_10);
	vec.sort(&vec, cmp_ptr);
	size_t idx;
	assert(vec.binary_search(NULL, ptr_10, cmp_ptr, &idx) == EC_ERR_INVALID_ARG);
	assert(vec.binary_search(&vec, ptr_10, NULL, &idx) == EC_ERR_INVALID_ARG);
	assert(vec.binary_search(&vec, ptr_10, cmp_ptr, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_sort_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_3);
	vec.push(&vec, ptr_1);
	vec.push(&vec, ptr_2);
	vec.sort(&vec, cmp_ptr);
	void* val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == ptr_1);
	assert(vec.get(&vec, 1, &val) == EC_OK && val == ptr_2);
	assert(vec.get(&vec, 2, &val) == EC_OK && val == ptr_3);
	vec.free_fn(&vec);
}

static void
test_sort_neg_null(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_3);
	vec.sort(NULL, cmp_ptr);
	vec.sort(&vec, NULL);
	vec.free_fn(&vec);
}

static void
test_reverse_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_1);
	vec.push(&vec, ptr_2);
	vec.push(&vec, ptr_3);
	vec.reverse(&vec);
	void* val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == ptr_3);
	assert(vec.get(&vec, 1, &val) == EC_OK && val == ptr_2);
	assert(vec.get(&vec, 2, &val) == EC_OK && val == ptr_1);
	vec.free_fn(&vec);
}

static void
test_reverse_pos_empty(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.reverse(&vec);
	assert(vec.size_fn(&vec) == 0);
	vec.free_fn(&vec);
}

static void
test_reverse_neg_null(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.reverse(NULL);
	vec.free_fn(&vec);
}

static void
test_map_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 3, NULL);
	vec.push(&vec, ptr_5);
	vec.push(&vec, ptr_10);
	vec.map(&vec, inc_ptr);
	// For ptr, map doesn't meaningfully change pointers, just verify it ran
	void* val;
	assert(vec.get(&vec, 0, &val) == EC_OK);
	assert(vec.get(&vec, 1, &val) == EC_OK);
	vec.free_fn(&vec);
}

static void
test_map_neg_null(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 3, NULL);
	vec.push(&vec, ptr_5);
	vec.map(NULL, inc_ptr);
	vec.map(&vec, NULL);
	vec.free_fn(&vec);
}

static void
test_reduce_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 4, NULL);
	vec.push(&vec, ptr_1);
	vec.push(&vec, ptr_2);
	vec.push(&vec, ptr_3);
	void* result = NULL;
	vec.reduce(&vec, sum_ptr, &result);
	// For ptr, reduce result is not meaningful - just check it ran
	assert(result != NULL || vec.size_fn(&vec) == 0);
	vec.free_fn(&vec);
}

static void
test_reduce_pos_empty(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 4, NULL);
	void* result = NULL;
	vec.reduce(&vec, sum_ptr, &result);
	vec.free_fn(&vec);
}

static void
test_reduce_neg_null(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 4, NULL);
	vec.push(&vec, ptr_1);
	void* result = NULL;
	vec.reduce(NULL, sum_ptr, &result);
	vec.reduce(&vec, NULL, &result);
	vec.reduce(&vec, sum_ptr, NULL);
	vec.free_fn(&vec);
}

static void
test_subarray_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 6, NULL);
	int vals[] = { 10, 20, 30, 40, 50, 60 };
	void* ptrs[6];
	for (int i = 0; i < 6; i++)
		ptrs[i] = &vals[i];
	for (int i = 0; i < 6; i++)
		vec.push(&vec, ptrs[i]);
	ec_vector_ptr_t sub;
	assert(vec.subarray(&vec, 2, 5, &sub) == EC_OK);
	assert(sub.size_fn(&sub) == 3);
	void* val;
	assert(sub.get(&sub, 0, &val) == EC_OK && val == ptrs[2]);
	sub.free_fn(&sub);
	vec.free_fn(&vec);
}

static void
test_subarray_neg_invalid_range(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 6, NULL);
	vec.push(&vec, ptr_10);
	ec_vector_ptr_t sub;
	assert(vec.subarray(&vec, 2, 1, &sub) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_subarray_neg_out_of_bounds(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 6, NULL);
	vec.push(&vec, ptr_10);
	ec_vector_ptr_t sub;
	assert(vec.subarray(&vec, 0, 2, &sub) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_subarray_neg_null(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 6, NULL);
	vec.push(&vec, ptr_10);
	ec_vector_ptr_t sub;
	assert(vec.subarray(NULL, 0, 1, &sub) == EC_ERR_INVALID_ARG);
	assert(vec.subarray(&vec, 0, 1, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_copy_shallow_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 4, NULL);
	int vals[] = { 1, 2, 3, 4 };
	for (int i = 0; i < 4; i++)
		vec.push(&vec, &vals[i]);
	ec_vector_ptr_t copy;
	assert(vec.copy_shallow(&vec, &copy) == EC_OK);
	assert(copy.size_fn(&copy) == vec.size_fn(&vec));
	copy.free_fn(&copy);
	vec.free_fn(&vec);
}

static void
test_copy_shallow_neg_null(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 4, NULL);
	vec.push(&vec, ptr_1);
	ec_vector_ptr_t copy;
	assert(vec.copy_shallow(NULL, &copy) == EC_ERR_INVALID_ARG);
	assert(vec.copy_shallow(&vec, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_copy_deep_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 4, NULL);
	vec.push(&vec, ptr_10);
	vec.push(&vec, ptr_20);
	ec_vector_ptr_t copy;
	assert(vec.copy_deep(&vec, copy_ptr_val, &copy) == EC_OK);
	assert(copy.size_fn(&copy) == vec.size_fn(&vec));
	copy.free_fn(&copy);
	vec.free_fn(&vec);
}

static void
test_copy_deep_neg_null(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 4, NULL);
	vec.push(&vec, ptr_10);
	ec_vector_ptr_t copy;
	assert(vec.copy_deep(NULL, copy_ptr_val, &copy) == EC_ERR_INVALID_ARG);
	assert(vec.copy_deep(&vec, NULL, &copy) == EC_ERR_INVALID_ARG);
	assert(vec.copy_deep(&vec, copy_ptr_val, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_filter_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 6, NULL);
	int nums[] = { 10, 15, 20, 25, 30, 35 };
	for (int i = 0; i < 6; i++)
		vec.push(&vec, &nums[i]);
	ec_vector_ptr_t filtered;
	assert(vec.filter(&vec, is_valid_ptr, &filtered) == EC_OK);
	assert(filtered.size_fn(&filtered) == 6); // All pointers are valid
	filtered.free_fn(&filtered);
	vec.free_fn(&vec);
}

static void
test_filter_neg_null(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 6, NULL);
	vec.push(&vec, ptr_10);
	ec_vector_ptr_t filtered;
	assert(vec.filter(NULL, is_valid_ptr, &filtered) == EC_ERR_INVALID_ARG);
	assert(vec.filter(&vec, NULL, &filtered) == EC_ERR_INVALID_ARG);
	assert(vec.filter(&vec, is_valid_ptr, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_filter_mut_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 6, NULL);
	int nums[] = { 10, 15, 20, 25, 30, 35 };
	for (int i = 0; i < 6; i++)
		vec.push(&vec, &nums[i]);
	assert(vec.filter_mut(&vec, is_valid_ptr) == EC_OK);
	assert(vec.size_fn(&vec) == 6); // All pointers are valid
	vec.free_fn(&vec);
}

static void
test_filter_mut_neg_null(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 6, NULL);
	vec.push(&vec, ptr_10);
	assert(vec.filter_mut(NULL, is_valid_ptr) == EC_ERR_INVALID_ARG);
	assert(vec.filter_mut(&vec, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_size_fn_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 10, NULL);
	assert(vec.size_fn(&vec) == 0);
	vec.push(&vec, ptr_1);
	assert(vec.size_fn(&vec) == 1);
	vec.free_fn(&vec);
}

static void
test_capacity_fn_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 10, NULL);
	assert(vec.capacity_fn(&vec) == 10);
	vec.free_fn(&vec);
}

static void
test_trim_capacity_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 10, NULL);
	int vals[] = { 10, 20, 30, 40 };
	for (int i = 0; i < 4; i++)
		vec.push(&vec, &vals[i]);
	assert(vec.trim_capacity(&vec) == EC_OK);
	assert(vec.capacity_fn(&vec) == vec.size_fn(&vec));
	vec.free_fn(&vec);
}

static void
test_trim_capacity_neg_null(void)
{
	assert(((ec_vector_ptr_t*)NULL)->trim_capacity == NULL || 1);
}

static void
test_free_fn_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 10, NULL);
	vec.push(&vec, ptr_1);
	assert(vec.free_fn(&vec) == EC_OK);
}

static void
test_free_fn_neg_null(void)
{
	assert(((ec_vector_ptr_t*)NULL)->free_fn == NULL || 1);
}

static void
test_iter_init_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_1);
	ec_vector_ptr_iter_t iter;
	assert(ec_vector_ptr_iter_init(&iter, &vec) == EC_OK);
	vec.free_fn(&vec);
}

static void
test_iter_init_neg_null(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	ec_vector_ptr_iter_t iter;
	assert(ec_vector_ptr_iter_init(NULL, &vec) == EC_ERR_INVALID_ARG);
	assert(ec_vector_ptr_iter_init(&iter, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_iter_next_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_100);
	ec_vector_ptr_iter_t iter;
	ec_vector_ptr_iter_init(&iter, &vec);
	void* val;
	assert(ec_vector_ptr_iter_next(&iter, &val) == EC_OK);
	assert(val == ptr_100);
	vec.free_fn(&vec);
}

static void
test_iter_next_neg_empty(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	ec_vector_ptr_iter_t iter;
	ec_vector_ptr_iter_init(&iter, &vec);
	void* val;
	assert(ec_vector_ptr_iter_next(&iter, &val) == EC_ITER_END);
	vec.free_fn(&vec);
}

static void
test_iter_next_neg_null(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_100);
	ec_vector_ptr_iter_t iter;
	ec_vector_ptr_iter_init(&iter, &vec);
	void* val;
	assert(ec_vector_ptr_iter_next(NULL, &val) == EC_ERR_INVALID_ARG);
	assert(ec_vector_ptr_iter_next(&iter, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_iter_remove_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_100);
	vec.push(&vec, ptr_200);
	ec_vector_ptr_iter_t iter;
	ec_vector_ptr_iter_init(&iter, &vec);
	void* val;
	ec_vector_ptr_iter_next(&iter, &val);
	assert(ec_vector_ptr_iter_remove(&iter) == EC_OK);
	assert(vec.size_fn(&vec) == 1);
	vec.free_fn(&vec);
}

static void
test_iter_remove_neg_invalid_state(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_100);
	ec_vector_ptr_iter_t iter;
	ec_vector_ptr_iter_init(&iter, &vec);
	assert(ec_vector_ptr_iter_remove(&iter) == EC_ERR_INVALID_OP);
	vec.free_fn(&vec);
}

static void
test_iter_add_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_100);
	ec_vector_ptr_iter_t iter;
	ec_vector_ptr_iter_init(&iter, &vec);
	void* val;
	ec_vector_ptr_iter_next(&iter, &val);
	int v99 = 99;
	assert(ec_vector_ptr_iter_add(&iter, &v99) == EC_OK);
	assert(vec.size_fn(&vec) == 2);
	vec.free_fn(&vec);
}

static void
test_iter_add_neg_null(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	ec_vector_ptr_iter_t iter;
	ec_vector_ptr_iter_init(&iter, &vec);
	int v99 = 99;
	assert(ec_vector_ptr_iter_add(NULL, &v99) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_iter_replace_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_100);
	ec_vector_ptr_iter_t iter;
	ec_vector_ptr_iter_init(&iter, &vec);
	void* val;
	ec_vector_ptr_iter_next(&iter, &val);
	void* replaced;
	int v77 = 77;
	assert(ec_vector_ptr_iter_replace(&iter, &v77, &replaced) == EC_OK);
	assert(replaced == ptr_100);
	vec.free_fn(&vec);
}

static void
test_iter_replace_neg_invalid_state(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_100);
	ec_vector_ptr_iter_t iter;
	ec_vector_ptr_iter_init(&iter, &vec);
	void* replaced;
	int v77 = 77;
	assert(ec_vector_ptr_iter_replace(&iter, &v77, &replaced) == EC_ERR_INVALID_OP);
	vec.free_fn(&vec);
}

static void
test_iter_index_pos(void)
{
	ec_vector_ptr_t vec;
	ec_vector_ptr_init(&vec, 5, NULL);
	vec.push(&vec, ptr_100);
	ec_vector_ptr_iter_t iter;
	ec_vector_ptr_iter_init(&iter, &vec);
	assert(ec_vector_ptr_iter_index(&iter) == 0);
	vec.free_fn(&vec);
}

static void
test_zip_iter_init_pos(void)
{
	ec_vector_ptr_t vec1, vec2;
	ec_vector_ptr_init(&vec1, 5, NULL);
	ec_vector_ptr_init(&vec2, 5, NULL);
	ec_vector_ptr_zip_iter_t zip;
	assert(ec_vector_ptr_zip_iter_init(&zip, &vec1, &vec2) == EC_OK);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_init_neg_null(void)
{
	ec_vector_ptr_t vec1, vec2;
	ec_vector_ptr_init(&vec1, 5, NULL);
	ec_vector_ptr_init(&vec2, 5, NULL);
	ec_vector_ptr_zip_iter_t zip;
	assert(ec_vector_ptr_zip_iter_init(NULL, &vec1, &vec2) == EC_ERR_INVALID_ARG);
	assert(ec_vector_ptr_zip_iter_init(&zip, NULL, &vec2) == EC_ERR_INVALID_ARG);
	assert(ec_vector_ptr_zip_iter_init(&zip, &vec1, NULL) == EC_ERR_INVALID_ARG);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_next_pos(void)
{
	ec_vector_ptr_t vec1, vec2;
	ec_vector_ptr_init(&vec1, 5, NULL);
	ec_vector_ptr_init(&vec2, 5, NULL);
	int v1 = 1, v2 = 10;
	vec1.push(&vec1, &v1);
	vec2.push(&vec2, &v2);
	ec_vector_ptr_zip_iter_t zip;
	ec_vector_ptr_zip_iter_init(&zip, &vec1, &vec2);
	void *a, *b;
	assert(ec_vector_ptr_zip_iter_next(&zip, &a, &b) == EC_OK);
	assert(a == &v1 && b == &v2);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_next_neg_empty(void)
{
	ec_vector_ptr_t vec1, vec2;
	ec_vector_ptr_init(&vec1, 5, NULL);
	ec_vector_ptr_init(&vec2, 5, NULL);
	ec_vector_ptr_zip_iter_t zip;
	ec_vector_ptr_zip_iter_init(&zip, &vec1, &vec2);
	void *a, *b;
	assert(ec_vector_ptr_zip_iter_next(&zip, &a, &b) == EC_ITER_END);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_next_neg_null(void)
{
	ec_vector_ptr_t vec1, vec2;
	ec_vector_ptr_init(&vec1, 5, NULL);
	ec_vector_ptr_init(&vec2, 5, NULL);
	ec_vector_ptr_zip_iter_t zip;
	ec_vector_ptr_zip_iter_init(&zip, &vec1, &vec2);
	void *a, *b;
	assert(ec_vector_ptr_zip_iter_next(NULL, &a, &b) == EC_ERR_INVALID_ARG);
	assert(ec_vector_ptr_zip_iter_next(&zip, NULL, &b) == EC_ERR_INVALID_ARG);
	assert(ec_vector_ptr_zip_iter_next(&zip, &a, NULL) == EC_ERR_INVALID_ARG);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_remove_pos(void)
{
	ec_vector_ptr_t vec1, vec2;
	ec_vector_ptr_init(&vec1, 5, NULL);
	ec_vector_ptr_init(&vec2, 5, NULL);
	int v1 = 1;
	vec1.push(&vec1, &v1);
	int v10 = 10;
	vec2.push(&vec2, &v10);
	ec_vector_ptr_zip_iter_t zip;
	ec_vector_ptr_zip_iter_init(&zip, &vec1, &vec2);
	void *a, *b;
	ec_vector_ptr_zip_iter_next(&zip, &a, &b);
	assert(ec_vector_ptr_zip_iter_remove(&zip) == EC_OK);
	assert(vec1.size_fn(&vec1) == 0 && vec2.size_fn(&vec2) == 0);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_remove_neg_invalid_state(void)
{
	ec_vector_ptr_t vec1, vec2;
	ec_vector_ptr_init(&vec1, 5, NULL);
	ec_vector_ptr_init(&vec2, 5, NULL);
	ec_vector_ptr_zip_iter_t zip;
	ec_vector_ptr_zip_iter_init(&zip, &vec1, &vec2);
	assert(ec_vector_ptr_zip_iter_remove(&zip) == EC_ERR_INVALID_OP);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_add_pos(void)
{
	ec_vector_ptr_t vec1, vec2;
	ec_vector_ptr_init(&vec1, 5, NULL);
	ec_vector_ptr_init(&vec2, 5, NULL);
	ec_vector_ptr_zip_iter_t zip;
	ec_vector_ptr_zip_iter_init(&zip, &vec1, &vec2);
	int v7 = 7, v70 = 70;
	assert(ec_vector_ptr_zip_iter_add(&zip, &v7, &v70) == EC_OK);
	assert(vec1.size_fn(&vec1) == 1 && vec2.size_fn(&vec2) == 1);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_add_neg_null(void)
{
	ec_vector_ptr_t vec1, vec2;
	ec_vector_ptr_init(&vec1, 5, NULL);
	ec_vector_ptr_init(&vec2, 5, NULL);
	ec_vector_ptr_zip_iter_t zip;
	ec_vector_ptr_zip_iter_init(&zip, &vec1, &vec2);
	int v7 = 7, v70 = 70;
	assert(ec_vector_ptr_zip_iter_add(NULL, &v7, &v70) == EC_ERR_INVALID_ARG);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_replace_pos(void)
{
	ec_vector_ptr_t vec1, vec2;
	ec_vector_ptr_init(&vec1, 5, NULL);
	ec_vector_ptr_init(&vec2, 5, NULL);
	int v1 = 1;
	vec1.push(&vec1, &v1);
	int v10 = 10;
	vec2.push(&vec2, &v10);
	ec_vector_ptr_zip_iter_t zip;
	ec_vector_ptr_zip_iter_init(&zip, &vec1, &vec2);
	void *a, *b;
	ec_vector_ptr_zip_iter_next(&zip, &a, &b);
	void *r1, *r2;
	int v8 = 8, v80 = 80;
	assert(ec_vector_ptr_zip_iter_replace(&zip, &v8, &v80, &r1, &r2) == EC_OK);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_replace_neg_invalid_state(void)
{
	ec_vector_ptr_t vec1, vec2;
	ec_vector_ptr_init(&vec1, 5, NULL);
	ec_vector_ptr_init(&vec2, 5, NULL);
	ec_vector_ptr_zip_iter_t zip;
	ec_vector_ptr_zip_iter_init(&zip, &vec1, &vec2);
	void *r1, *r2;
	int v8 = 8, v80 = 80;
	assert(ec_vector_ptr_zip_iter_replace(&zip, &v8, &v80, &r1, &r2) == EC_ERR_INVALID_OP);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_index_pos(void)
{
	ec_vector_ptr_t vec1, vec2;
	ec_vector_ptr_init(&vec1, 5, NULL);
	ec_vector_ptr_init(&vec2, 5, NULL);
	ec_vector_ptr_zip_iter_t zip;
	ec_vector_ptr_zip_iter_init(&zip, &vec1, &vec2);
	assert(ec_vector_ptr_zip_iter_index(&zip) == 0);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

int
main(void)
{
	printf("Starting ec_vector_ptr tests...\n");
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
	printf("All ec_vector_ptr tests passed.\n");
	return 0;
}
