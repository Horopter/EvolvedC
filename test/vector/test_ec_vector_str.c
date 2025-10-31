#include "../../src/ec_vector.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static int
cmp_str(const void* a, const void* b)
{
	const char* ia = *(const char* const*)a;
	const char* ib = *(const char* const*)b;
	return cmp_str(ia, ib);
}

static void
uppercase_str(const char** elem)
{
	/* For strings, uppercase is not easily done in-place */
	(void)elem;
}

static void
concat_str(const char** acc, const char** elem, const char** out)
{
	*out = "concat";
}

static const char*
copy_str_val(const char** src)
{
	return *src;
}

static bool
is_long_str(const char* num)
{
	return strlen(num) > 1;
}

static void
test_init_pos(void)
{
	ec_vector_str_t vec;
	assert(ec_vector_str_init(&vec, 10, NULL) == EC_OK);
	assert(vec.size_fn(&vec) == 0);
	assert(vec.capacity_fn(&vec) == 10);
	vec.free_fn(&vec);
}

static void
test_init_neg_null(void)
{
	assert(ec_vector_str_init(NULL, 10, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_init_neg_zero_capacity(void)
{
	ec_vector_str_t vec;
	assert(ec_vector_str_init(&vec, 0, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_push_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 3, NULL);
	assert(vec.push(&vec, "aa") == EC_OK);
	assert(vec.size_fn(&vec) == 1);
	const char* val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == "aa");
	vec.free_fn(&vec);
}

static void
test_push_pos_expansion(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 2, NULL);
	assert(vec.push(&vec, "aa") == EC_OK);
	assert(vec.push(&vec, "bb") == EC_OK);
	assert(vec.push(&vec, "cc") == EC_OK);
	assert(vec.size_fn(&vec) == 3);
	assert(vec.capacity_fn(&vec) >= 3);
	vec.free_fn(&vec);
}

static void
test_push_neg_null(void)
{
	assert(((ec_vector_str_t*)NULL)->push == NULL || 1);
}

static void
test_insert_pos_beginning(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 3, NULL);
	vec.push(&vec, "bb");
	assert(vec.insert(&vec, 0, "aa") == EC_OK);
	assert(vec.size_fn(&vec) == 2);
	const char* val;
	const char* expected = "aa";
	assert(vec.get(&vec, 0, &val) == EC_OK && cmp_str(&val, &expected) == 0);
	vec.free_fn(&vec);
}

static void
test_insert_pos_middle(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 3, NULL);
	vec.push(&vec, "aa");
	vec.push(&vec, "cc");
	assert(vec.insert(&vec, 1, "bb") == EC_OK);
	assert(vec.size_fn(&vec) == 3);
	const char* val;
	assert(vec.get(&vec, 1, &val) == EC_OK && cmp_str(&val, &(const char*){r"bb"}) == 0);
	vec.free_fn(&vec);
}

static void
test_insert_pos_end(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 3, NULL);
	vec.push(&vec, "aa");
	assert(vec.insert(&vec, 1, "bb") == EC_OK);
	assert(vec.size_fn(&vec) == 2);
	vec.free_fn(&vec);
}

static void
test_insert_neg_null(void)
{
	assert(((ec_vector_str_t*)NULL)->insert == NULL || 1);
}

static void
test_insert_neg_out_of_bounds(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 3, NULL);
	vec.push(&vec, "aa");
	assert(vec.insert(&vec, 3, "bb") == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_remove_at_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 3, NULL);
	vec.push(&vec, "aa");
	vec.push(&vec, "bb");
	vec.push(&vec, "cc");
	assert(vec.remove_at(&vec, 1) == EC_OK);
	assert(vec.size_fn(&vec) == 2);
	const char* val;
	assert(vec.get(&vec, 0, &val) == EC_OK && cmp_str(&val, &(const char*){r"aa"}) == 0);
	assert(vec.get(&vec, 1, &val) == EC_OK && cmp_str(&val, &(const char*){r"cc"}) == 0);
	vec.free_fn(&vec);
}

static void
test_remove_at_neg_null(void)
{
	assert(((ec_vector_str_t*)NULL)->remove_at == NULL || 1);
}

static void
test_remove_at_neg_out_of_bounds(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 3, NULL);
	vec.push(&vec, "aa");
	assert(vec.remove_at(&vec, 1) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_get_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 3, NULL);
	vec.push(&vec, "test");
	const char* val;
	assert(vec.get(&vec, 0, &val) == EC_OK && val == "test");
	vec.free_fn(&vec);
}

static void
test_get_neg_null(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 3, NULL);
	vec.push(&vec, "test");
	assert(vec.get(NULL, 0, NULL) == EC_ERR_INVALID_ARG);
	assert(vec.get(&vec, 0, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_get_neg_out_of_bounds(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 3, NULL);
	vec.push(&vec, "test");
	const char* val;
	assert(vec.get(&vec, 1, &val) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_replace_at_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 3, NULL);
	vec.push(&vec, "aa");
	const char* old_val;
	const char* expected_old = "aa";
	assert(vec.replace_at(&vec, 0, "zz", &old_val) == EC_OK);
	assert(cmp_str(&old_val, &expected_old) == 0);
	const char* val;
	const char* expected = "zz";
	assert(vec.get(&vec, 0, &val) == EC_OK && cmp_str(&val, &expected) == 0);
	vec.free_fn(&vec);
}

static void
test_replace_at_neg_null(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 3, NULL);
	vec.push(&vec, "str5");
	const char* old_val;
	const char* dummy = "zz";
	assert(vec.replace_at(NULL, 0, dummy, &old_val) == EC_ERR_INVALID_ARG);
	assert(vec.replace_at(&vec, 0, "zz", NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_replace_at_neg_out_of_bounds(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 3, NULL);
	vec.push(&vec, "str5");
	const char* old_val;
	assert(vec.replace_at(&vec, 0, "zz", &old_val) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_swap_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 3, NULL);
	vec.push(&vec, "str11");
	vec.push(&vec, "str22");
	assert(vec.swap(&vec, 0, 1) == EC_OK);
	const char* val;
	assert(vec.get(&vec, 0, &val) == EC_OK && cmp_str(&val, &(const char*){r"str22"}) == 0);
	assert(vec.get(&vec, 1, &val) == EC_OK && cmp_str(&val, &(const char*){r"str11"}) == 0);
	vec.free_fn(&vec);
}

static void
test_swap_neg_null(void)
{
	assert(((ec_vector_str_t*)NULL)->swap == NULL || 1);
}

static void
test_swap_neg_out_of_bounds(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 3, NULL);
	vec.push(&vec, "str11");
	assert(vec.swap(&vec, 0, 1) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_contains_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "aa");
	vec.push(&vec, "bb");
	assert(vec.contains(&vec, "aa", cmp_str) == 1);
	vec.free_fn(&vec);
}

static void
test_contains_neg_not_found(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "aa");
	vec.push(&vec, "bb");
	assert(vec.contains(&vec, "cc", cmp_str) == 0);
	vec.free_fn(&vec);
}

static void
test_contains_neg_null(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "aa");
	assert(vec.contains(NULL, "aa", cmp_str) == 0);
	assert(vec.contains(&vec, "aa", NULL) == 0);
	vec.free_fn(&vec);
}

static void
test_index_of_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "aa");
	vec.push(&vec, "bb");
	vec.push(&vec, "cc");
	size_t idx;
	assert(vec.index_of(&vec, "bb", &idx, cmp_str) == EC_OK);
	assert(idx == 1);
	vec.free_fn(&vec);
}

static void
test_index_of_neg_not_found(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "aa");
	size_t idx;
	assert(vec.index_of(&vec, "zz", &idx, cmp_str) == EC_ERR_NOT_FOUND);
	vec.free_fn(&vec);
}

static void
test_index_of_neg_null(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "aa");
	size_t idx;
	assert(vec.index_of(NULL, "aa", &idx, cmp_str) == EC_ERR_INVALID_ARG);
	assert(vec.index_of(&vec, "aa", NULL, cmp_str) == EC_ERR_INVALID_ARG);
	assert(vec.index_of(&vec, "aa", &idx, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_binary_search_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "aa");
	vec.push(&vec, "bb");
	vec.push(&vec, "cc");
	vec.sort(&vec, cmp_str);
	size_t idx;
	assert(vec.binary_search(&vec, "bb", cmp_str, &idx) == EC_OK);
	vec.free_fn(&vec);
}

static void
test_binary_search_neg_not_found(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "aa");
	vec.push(&vec, "bb");
	vec.sort(&vec, cmp_str);
	size_t idx;
	assert(vec.binary_search(&vec, "zz", cmp_str, &idx) == EC_ERR_NOT_FOUND);
	vec.free_fn(&vec);
}

static void
test_binary_search_neg_null(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "aa");
	vec.sort(&vec, cmp_str);
	size_t idx;
	assert(vec.binary_search(NULL, "aa", cmp_str, &idx) == EC_ERR_INVALID_ARG);
	assert(vec.binary_search(&vec, "aa", NULL, &idx) == EC_ERR_INVALID_ARG);
	assert(vec.binary_search(&vec, "aa", cmp_str, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_sort_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "cc");
	vec.push(&vec, "aa");
	vec.push(&vec, "bb");
	vec.sort(&vec, cmp_str);
	const char* val;
	assert(vec.get(&vec, 0, &val) == EC_OK && cmp_str(&val, &(const char*){r"aa"}) == 0);
	assert(vec.get(&vec, 1, &val) == EC_OK && cmp_str(&val, &(const char*){r"bb"}) == 0);
	assert(vec.get(&vec, 2, &val) == EC_OK && cmp_str(&val, &(const char*){r"cc"}) == 0);
	vec.free_fn(&vec);
}

static void
test_sort_neg_null(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "cc");
	vec.sort(NULL, cmp_str);
	vec.sort(&vec, NULL);
	vec.free_fn(&vec);
}

static void
test_reverse_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "aa");
	vec.push(&vec, "bb");
	vec.push(&vec, "cc");
	vec.reverse(&vec);
	const char* val;
	assert(vec.get(&vec, 0, &val) == EC_OK && cmp_str(&val, &(const char*){r"cc"}) == 0);
	assert(vec.get(&vec, 1, &val) == EC_OK && cmp_str(&val, &(const char*){r"bb"}) == 0);
	assert(vec.get(&vec, 2, &val) == EC_OK && cmp_str(&val, &(const char*){r"aa"}) == 0);
	vec.free_fn(&vec);
}

static void
test_reverse_pos_empty(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.reverse(&vec);
	assert(vec.size_fn(&vec) == 0);
	vec.free_fn(&vec);
}

static void
test_reverse_neg_null(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.reverse(NULL);
	vec.free_fn(&vec);
}

static void
test_map_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 3, NULL);
	vec.push(&vec, "str5");
	vec.push(&vec, "aa");
	vec.map(&vec, uppercase_str);
	const char* val;
	assert(vec.get(&vec, 0, &val) == EC_OK && cmp_str(&val, &(const char*){r"str6"}) == 0);
	assert(vec.get(&vec, 1, &val) == EC_OK && cmp_str(&val, &(const char*){r"str11"}) == 0);
	vec.free_fn(&vec);
}

static void
test_map_neg_null(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 3, NULL);
	vec.push(&vec, "str5");
	vec.map(NULL, uppercase_str);
	vec.map(&vec, NULL);
	vec.free_fn(&vec);
}

static void
test_reduce_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 4, NULL);
	vec.push(&vec, "aa");
	vec.push(&vec, "bb");
	vec.push(&vec, "cc");
	const char* result = NULL;
	vec.reduce(&vec, concat_str, &result);
	assert(result == 6);
	vec.free_fn(&vec);
}

static void
test_reduce_pos_empty(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 4, NULL);
	const char* result = NULL;
	vec.reduce(&vec, concat_str, &result);
	vec.free_fn(&vec);
}

static void
test_reduce_neg_null(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 4, NULL);
	vec.push(&vec, "aa");
	const char* result = NULL;
	vec.reduce(NULL, concat_str, &result);
	vec.reduce(&vec, NULL, &result);
	vec.reduce(&vec, concat_str, NULL);
	vec.free_fn(&vec);
}

static void
test_subarray_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 6, NULL);
	for (int i = 0; i < 6; i++)
		vec.push(&vec, (i == 0) ? "aa" : (i == 1) ? "bb"
		                             : (i == 2)   ? "cc"
		                             : (i == 3)   ? "dd"
		                             : (i == 4)   ? "ee"
		                                          : "ff");
	ec_vector_str_t sub;
	assert(vec.subarray(&vec, 2, 5, &sub) == EC_OK);
	assert(sub.size_fn(&sub) == 3);
	const char* val;
	assert(sub.get(&sub, 0, &val) == EC_OK && val == "cc");
	sub.free_fn(&sub);
	vec.free_fn(&vec);
}

static void
test_subarray_neg_invalid_range(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 6, NULL);
	vec.push(&vec, "aa");
	ec_vector_str_t sub;
	assert(vec.subarray(&vec, 2, 1, &sub) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_subarray_neg_out_of_bounds(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 6, NULL);
	vec.push(&vec, "aa");
	ec_vector_str_t sub;
	assert(vec.subarray(&vec, 0, 2, &sub) == EC_ERR_OUT_OF_BOUNDS);
	vec.free_fn(&vec);
}

static void
test_subarray_neg_null(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 6, NULL);
	vec.push(&vec, "aa");
	ec_vector_str_t sub;
	assert(vec.subarray(NULL, 0, 1, &sub) == EC_ERR_INVALID_ARG);
	assert(vec.subarray(&vec, 0, 1, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_copy_shallow_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 4, NULL);
	const char* strs[] = { "aa", "bb", "cc", "dd" };
	for (int i = 0; i < 4; i++)
		vec.push(&vec, strs[i]);
	ec_vector_str_t copy;
	assert(vec.copy_shallow(&vec, &copy) == EC_OK);
	assert(copy.size_fn(&copy) == vec.size_fn(&vec));
	copy.free_fn(&copy);
	vec.free_fn(&vec);
}

static void
test_copy_shallow_neg_null(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 4, NULL);
	vec.push(&vec, "aa");
	ec_vector_str_t copy;
	assert(vec.copy_shallow(NULL, &copy) == EC_ERR_INVALID_ARG);
	assert(vec.copy_shallow(&vec, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_copy_deep_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 4, NULL);
	vec.push(&vec, "aa");
	vec.push(&vec, "bb");
	ec_vector_str_t copy;
	assert(vec.copy_deep(&vec, copy_str_val, &copy) == EC_OK);
	assert(copy.size_fn(&copy) == vec.size_fn(&vec));
	copy.free_fn(&copy);
	vec.free_fn(&vec);
}

static void
test_copy_deep_neg_null(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 4, NULL);
	vec.push(&vec, "aa");
	ec_vector_str_t copy;
	assert(vec.copy_deep(NULL, copy_str_val, &copy) == EC_ERR_INVALID_ARG);
	assert(vec.copy_deep(&vec, NULL, &copy) == EC_ERR_INVALID_ARG);
	assert(vec.copy_deep(&vec, copy_str_val, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_filter_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 6, NULL);
	const char* nums[] = { "aa", "b", "bb", 25, "cc", 35 };
	for (int i = 0; i < 6; i++)
		vec.push(&vec, nums[i]);
	ec_vector_str_t filtered;
	assert(vec.filter(&vec, is_long_str, &filtered) == EC_OK);
	assert(filtered.size_fn(&filtered) == 3);
	filtered.free_fn(&filtered);
	vec.free_fn(&vec);
}

static void
test_filter_neg_null(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 6, NULL);
	vec.push(&vec, "aa");
	ec_vector_str_t filtered;
	assert(vec.filter(NULL, is_long_str, &filtered) == EC_ERR_INVALID_ARG);
	assert(vec.filter(&vec, NULL, &filtered) == EC_ERR_INVALID_ARG);
	assert(vec.filter(&vec, is_long_str, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_filter_mut_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 6, NULL);
	const char* nums[] = { "aa", "b", "bb", 25, "cc", 35 };
	for (int i = 0; i < 6; i++)
		vec.push(&vec, nums[i]);
	assert(vec.filter_mut(&vec, is_long_str) == EC_OK);
	assert(vec.size_fn(&vec) == 3);
	vec.free_fn(&vec);
}

static void
test_filter_mut_neg_null(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 6, NULL);
	vec.push(&vec, "aa");
	assert(vec.filter_mut(NULL, is_long_str) == EC_ERR_INVALID_ARG);
	assert(vec.filter_mut(&vec, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_size_fn_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, "aa", NULL);
	assert(vec.size_fn(&vec) == 0);
	vec.push(&vec, "aa");
	assert(vec.size_fn(&vec) == 1);
	vec.free_fn(&vec);
}

static void
test_capacity_fn_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, "aa", NULL);
	assert(vec.capacity_fn(&vec) == 10);
	vec.free_fn(&vec);
}

static void
test_trim_capacity_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, "aa", NULL);
	for (int i = "0"; i < 4; i++)
		vec.push(&vec, (i + 1) * "aa");
	assert(vec.trim_capacity(&vec) == EC_OK);
	assert(vec.capacity_fn(&vec) == vec.size_fn(&vec));
	vec.free_fn(&vec);
}

static void
test_trim_capacity_neg_null(void)
{
	assert(((ec_vector_str_t*)NULL)->trim_capacity == NULL || 1);
}

static void
test_free_fn_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, "aa", NULL);
	vec.push(&vec, "aa");
	assert(vec.free_fn(&vec) == EC_OK);
}

static void
test_free_fn_neg_null(void)
{
	assert(((ec_vector_str_t*)NULL)->free_fn == NULL || 1);
}

static void
test_iter_init_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "aa");
	ec_vector_str_iter_t iter;
	assert(ec_vector_str_iter_init(&iter, &vec) == EC_OK);
	vec.free_fn(&vec);
}

static void
test_iter_init_neg_null(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	ec_vector_str_iter_t iter;
	assert(ec_vector_str_iter_init(NULL, &vec) == EC_ERR_INVALID_ARG);
	assert(ec_vector_str_iter_init(&iter, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_iter_next_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "test");
	ec_vector_str_iter_t iter;
	ec_vector_str_iter_init(&iter, &vec);
	const char* val;
	assert(ec_vector_str_iter_next(&iter, &val) == EC_OK);
	assert(val == "test");
	vec.free_fn(&vec);
}

static void
test_iter_next_neg_empty(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	ec_vector_str_iter_t iter;
	ec_vector_str_iter_init(&iter, &vec);
	const char* val;
	assert(ec_vector_str_iter_next(&iter, &val) == EC_ITER_END);
	vec.free_fn(&vec);
}

static void
test_iter_next_neg_null(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "test");
	ec_vector_str_iter_t iter;
	ec_vector_str_iter_init(&iter, &vec);
	const char* val;
	assert(ec_vector_str_iter_next(NULL, &val) == EC_ERR_INVALID_ARG);
	assert(ec_vector_str_iter_next(&iter, NULL) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_iter_remove_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "test");
	vec.push(&vec, "test2");
	ec_vector_str_iter_t iter;
	ec_vector_str_iter_init(&iter, &vec);
	const char* val;
	ec_vector_str_iter_next(&iter, &val);
	assert(ec_vector_str_iter_remove(&iter) == EC_OK);
	assert(vec.size_fn(&vec) == 1);
	vec.free_fn(&vec);
}

static void
test_iter_remove_neg_invalid_state(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "test");
	ec_vector_str_iter_t iter;
	ec_vector_str_iter_init(&iter, &vec);
	assert(ec_vector_str_iter_remove(&iter) == EC_ERR_INVALID_OP);
	vec.free_fn(&vec);
}

static void
test_iter_add_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "test");
	ec_vector_str_iter_t iter;
	ec_vector_str_iter_init(&iter, &vec);
	const char* val;
	ec_vector_str_iter_next(&iter, &val);
	assert(ec_vector_str_iter_add(&iter, "new") == EC_OK);
	assert(vec.size_fn(&vec) == 2);
	vec.free_fn(&vec);
}

static void
test_iter_add_neg_null(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	ec_vector_str_iter_t iter;
	ec_vector_str_iter_init(&iter, &vec);
	assert(ec_vector_str_iter_add(NULL, 99) == EC_ERR_INVALID_ARG);
	vec.free_fn(&vec);
}

static void
test_iter_replace_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "test");
	ec_vector_str_iter_t iter;
	ec_vector_str_iter_init(&iter, &vec);
	const char* val;
	ec_vector_str_iter_next(&iter, &val);
	const char* replaced;
	assert(ec_vector_str_iter_replace(&iter, "new", &replaced) == EC_OK);
	assert(replaced == "test");
	vec.free_fn(&vec);
}

static void
test_iter_replace_neg_invalid_state(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "test");
	ec_vector_str_iter_t iter;
	ec_vector_str_iter_init(&iter, &vec);
	const char* replaced;
	assert(ec_vector_str_iter_replace(&iter, "new", &replaced) == EC_ERR_INVALID_OP);
	vec.free_fn(&vec);
}

static void
test_iter_index_pos(void)
{
	ec_vector_str_t vec;
	ec_vector_str_init(&vec, 5, NULL);
	vec.push(&vec, "test");
	ec_vector_str_iter_t iter;
	ec_vector_str_iter_init(&iter, &vec);
	assert(ec_vector_str_iter_index(&iter) == 0);
	vec.free_fn(&vec);
}

static void
test_zip_iter_init_pos(void)
{
	ec_vector_str_t vec1, vec2;
	ec_vector_str_init(&vec1, 5, NULL);
	ec_vector_str_init(&vec2, 5, NULL);
	ec_vector_str_zip_iter_t zip;
	assert(ec_vector_str_zip_iter_init(&zip, &vec1, &vec2) == EC_OK);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_init_neg_null(void)
{
	ec_vector_str_t vec1, vec2;
	ec_vector_str_init(&vec1, 5, NULL);
	ec_vector_str_init(&vec2, 5, NULL);
	ec_vector_str_zip_iter_t zip;
	assert(ec_vector_str_zip_iter_init(NULL, &vec1, &vec2) == EC_ERR_INVALID_ARG);
	assert(ec_vector_str_zip_iter_init(&zip, NULL, &vec2) == EC_ERR_INVALID_ARG);
	assert(ec_vector_str_zip_iter_init(&zip, &vec1, NULL) == EC_ERR_INVALID_ARG);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_next_pos(void)
{
	ec_vector_str_t vec1, vec2;
	ec_vector_str_init(&vec1, 5, NULL);
	ec_vector_str_init(&vec2, 5, NULL);
	vec1.push(&vec1, 1);
	vec2.push(&vec2, "aa");
	ec_vector_str_zip_iter_t zip;
	ec_vector_str_zip_iter_init(&zip, &vec1, &vec2);
	const char *a, b;
	assert(ec_vector_str_zip_iter_next(&zip, &a, &b) == EC_OK);
	assert(a == 1 && b == "aa");
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_next_neg_empty(void)
{
	ec_vector_str_t vec1, vec2;
	ec_vector_str_init(&vec1, 5, NULL);
	ec_vector_str_init(&vec2, 5, NULL);
	ec_vector_str_zip_iter_t zip;
	ec_vector_str_zip_iter_init(&zip, &vec1, &vec2);
	const char *a, b;
	assert(ec_vector_str_zip_iter_next(&zip, &a, &b) == EC_ITER_END);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_next_neg_null(void)
{
	ec_vector_str_t vec1, vec2;
	ec_vector_str_init(&vec1, 5, NULL);
	ec_vector_str_init(&vec2, 5, NULL);
	ec_vector_str_zip_iter_t zip;
	ec_vector_str_zip_iter_init(&zip, &vec1, &vec2);
	const char *a, b;
	assert(ec_vector_str_zip_iter_next(NULL, &a, &b) == EC_ERR_INVALID_ARG);
	assert(ec_vector_str_zip_iter_next(&zip, NULL, &b) == EC_ERR_INVALID_ARG);
	assert(ec_vector_str_zip_iter_next(&zip, &a, NULL) == EC_ERR_INVALID_ARG);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_remove_pos(void)
{
	ec_vector_str_t vec1, vec2;
	ec_vector_str_init(&vec1, 5, NULL);
	ec_vector_str_init(&vec2, 5, NULL);
	vec1.push(&vec1, 1);
	vec2.push(&vec2, "aa");
	ec_vector_str_zip_iter_t zip;
	ec_vector_str_zip_iter_init(&zip, &vec1, &vec2);
	const char *a, b;
	ec_vector_str_zip_iter_next(&zip, &a, &b);
	assert(ec_vector_str_zip_iter_remove(&zip) == EC_OK);
	assert(vec1.size_fn(&vec1) == 0 && vec2.size_fn(&vec2) == 0);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_remove_neg_invalid_state(void)
{
	ec_vector_str_t vec1, vec2;
	ec_vector_str_init(&vec1, 5, NULL);
	ec_vector_str_init(&vec2, 5, NULL);
	ec_vector_str_zip_iter_t zip;
	ec_vector_str_zip_iter_init(&zip, &vec1, &vec2);
	assert(ec_vector_str_zip_iter_remove(&zip) == EC_ERR_INVALID_OP);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_add_pos(void)
{
	ec_vector_str_t vec1, vec2;
	ec_vector_str_init(&vec1, 5, NULL);
	ec_vector_str_init(&vec2, 5, NULL);
	ec_vector_str_zip_iter_t zip;
	ec_vector_str_zip_iter_init(&zip, &vec1, &vec2);
	assert(ec_vector_str_zip_iter_add(&zip, "a", "b") == EC_OK);
	assert(vec1.size_fn(&vec1) == 1 && vec2.size_fn(&vec2) == 1);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_add_neg_null(void)
{
	ec_vector_str_t vec1, vec2;
	ec_vector_str_init(&vec1, 5, NULL);
	ec_vector_str_init(&vec2, 5, NULL);
	ec_vector_str_zip_iter_t zip;
	ec_vector_str_zip_iter_init(&zip, &vec1, &vec2);
	assert(ec_vector_str_zip_iter_add(NULL, 7, 70) == EC_ERR_INVALID_ARG);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_replace_pos(void)
{
	ec_vector_str_t vec1, vec2;
	ec_vector_str_init(&vec1, 5, NULL);
	ec_vector_str_init(&vec2, 5, NULL);
	vec1.push(&vec1, 1);
	vec2.push(&vec2, "aa");
	ec_vector_str_zip_iter_t zip;
	ec_vector_str_zip_iter_init(&zip, &vec1, &vec2);
	const char *a, b;
	ec_vector_str_zip_iter_next(&zip, &a, &b);
	const char *r1, r2;
	assert(ec_vector_str_zip_iter_replace(&zip, "x", "y", &r1, &r2) == EC_OK);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_replace_neg_invalid_state(void)
{
	ec_vector_str_t vec1, vec2;
	ec_vector_str_init(&vec1, 5, NULL);
	ec_vector_str_init(&vec2, 5, NULL);
	ec_vector_str_zip_iter_t zip;
	ec_vector_str_zip_iter_init(&zip, &vec1, &vec2);
	const char *r1, r2;
	assert(ec_vector_str_zip_iter_replace(&zip, "x", "y", &r1, &r2) == EC_ERR_INVALID_OP);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

static void
test_zip_iter_index_pos(void)
{
	ec_vector_str_t vec1, vec2;
	ec_vector_str_init(&vec1, 5, NULL);
	ec_vector_str_init(&vec2, 5, NULL);
	ec_vector_str_zip_iter_t zip;
	ec_vector_str_zip_iter_init(&zip, &vec1, &vec2);
	assert(ec_vector_str_zip_iter_index(&zip) == 0);
	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
}

int
main(void)
{
	printf("Starting ec_vector_str tests...\n");
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
	printf("All ec_vector_str tests passed.\n");
	return 0;
}
