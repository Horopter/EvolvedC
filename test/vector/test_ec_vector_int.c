/*
 * test_vector_int.c
 *
 * Unit tests for ec_vector_int operations.
 * Each test function targets one operation defined in ec_vector.h.
 * This file tests the vector of int functions, and works whether or not
 * flags such as EC_USE_MEMPOOL and EC_THREADSAFE are defined.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../../src/ec_vector.h"
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

/*---------------------------------------------------------------------------
  Helper Function: print_timestamp
  Prints the current date/time in a formatted string.
---------------------------------------------------------------------------*/
void
print_timestamp(void)
{
	time_t now = time(NULL);
	struct tm* local = localtime(&now);
	char buffer[64];
	if (local)
		{
			strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", local);
			printf("%s ", buffer);
		}
	else
		{
			printf("[timestamp error] ");
		}
}

/*---------------------------------------------------------------------------
  Helper Comparator and Functions for int
---------------------------------------------------------------------------*/
int
cmp_int(const void* a, const void* b)
{
	int ia = *(const int*)a;
	int ib = *(const int*)b;
	return ia - ib;
}

void
inc_int(int* elem)
{
	*elem += 1;
}

void
sum_int(int* acc, int* elem, int* out)
{
	*out = (*acc) + (*elem);
}

/*---------------------------------------------------------------------------
  Test Functions for ec_vector_int
---------------------------------------------------------------------------*/

/* Test push() */
void
test_vector_int_push(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_push\n");

	ec_vector_int_t vec;
	ec_status_t status = ec_vector_int_init(&vec, 3, mptr);
	assert(status == EC_OK);

	status = vec.push(&vec, 10);
	assert(status == EC_OK);
	status = vec.push(&vec, 20);
	assert(status == EC_OK);
	status = vec.push(&vec, 30);
	assert(status == EC_OK);

	assert(vec.size == 3);
	int val;
	status = vec.get(&vec, 0, &val);
	assert(status == EC_OK && val == 10);
	status = vec.get(&vec, 1, &val);
	assert(status == EC_OK && val == 20);
	status = vec.get(&vec, 2, &val);
	assert(status == EC_OK && val == 30);

	vec.free_fn(&vec);
	printf("Passed test: vector_int_push\n");
}

/* Test insert() */
void
test_vector_int_insert(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_insert\n");

	ec_vector_int_t vec;
	ec_status_t status = ec_vector_int_init(&vec, 3, mptr);
	assert(status == EC_OK);

	// Push initial values [1, 3]
	status = vec.push(&vec, 1);
	assert(status == EC_OK);
	status = vec.push(&vec, 3);
	assert(status == EC_OK);

	// Insert 2 at index 1: expected vector -> [1, 2, 3]
	status = vec.insert(&vec, 1, 2);
	assert(status == EC_OK && vec.size == 3);
	int val;
	status = vec.get(&vec, 0, &val);
	assert(status == EC_OK && val == 1);
	status = vec.get(&vec, 1, &val);
	assert(status == EC_OK && val == 2);
	status = vec.get(&vec, 2, &val);
	assert(status == EC_OK && val == 3);

	vec.free_fn(&vec);
	printf("Passed test: vector_int_insert\n");
}

/* Test remove_at() */
void
test_vector_int_remove_at(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_remove_at\n");

	ec_vector_int_t vec;
	ec_status_t status = ec_vector_int_init(&vec, 5, mptr);
	assert(status == EC_OK);

	// Create vector [10, 20, 30, 40]
	status = vec.push(&vec, 10);
	assert(status == EC_OK);
	status = vec.push(&vec, 20);
	assert(status == EC_OK);
	status = vec.push(&vec, 30);
	assert(status == EC_OK);
	status = vec.push(&vec, 40);
	assert(status == EC_OK);

	// Remove element at index 2 (value 30) => expected [10, 20, 40]
	status = vec.remove_at(&vec, 2);
	assert(status == EC_OK && vec.size == 3);
	int val;
	status = vec.get(&vec, 0, &val);
	assert(status == EC_OK && val == 10);
	status = vec.get(&vec, 1, &val);
	assert(status == EC_OK && val == 20);
	status = vec.get(&vec, 2, &val);
	assert(status == EC_OK && val == 40);

	vec.free_fn(&vec);
	printf("Passed test: vector_int_remove_at\n");
}

/* Test get() */
void
test_vector_int_get(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_get\n");

	ec_vector_int_t vec;
	ec_status_t status = ec_vector_int_init(&vec, 3, mptr);
	assert(status == EC_OK);

	status = vec.push(&vec, 100);
	assert(status == EC_OK);
	status = vec.push(&vec, 200);
	assert(status == EC_OK);

	int val;
	status = vec.get(&vec, 1, &val);
	assert(status == EC_OK && val == 200);
	// Test out-of-bound access
	status = vec.get(&vec, 2, &val);
	assert(status == EC_ERR_OUT_OF_BOUNDS);

	vec.free_fn(&vec);
	printf("Passed test: vector_int_get\n");
}

/* Test replace_at() */
void
test_vector_int_replace_at(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_replace_at\n");

	ec_vector_int_t vec;
	ec_status_t status = ec_vector_int_init(&vec, 3, mptr);
	assert(status == EC_OK);

	status = vec.push(&vec, 5);
	assert(status == EC_OK);
	status = vec.push(&vec, 6);
	assert(status == EC_OK);

	int old_val;
	status = vec.replace_at(&vec, 1, 99, &old_val);
	assert(status == EC_OK && old_val == 6);

	int val;
	status = vec.get(&vec, 1, &val);
	assert(status == EC_OK && val == 99);

	vec.free_fn(&vec);
	printf("Passed test: vector_int_replace_at\n");
}

/* Test swap() */
void
test_vector_int_swap(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_swap\n");

	ec_vector_int_t vec;
	ec_status_t status = ec_vector_int_init(&vec, 3, mptr);
	assert(status == EC_OK);

	status = vec.push(&vec, 11);
	assert(status == EC_OK);
	status = vec.push(&vec, 22);
	assert(status == EC_OK);
	status = vec.push(&vec, 33);
	assert(status == EC_OK);

	status = vec.swap(&vec, 0, 2);
	assert(status == EC_OK);

	int a, b;
	status = vec.get(&vec, 0, &a);
	assert(status == EC_OK && a == 33);
	status = vec.get(&vec, 2, &b);
	assert(status == EC_OK && b == 11);

	vec.free_fn(&vec);
	printf("Passed test: vector_int_swap\n");
}

/* Test contains() and index_of() */
void
test_vector_int_contains_and_index_of(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_contains and index_of\n");

	ec_vector_int_t vec;
	ec_status_t status = ec_vector_int_init(&vec, 5, mptr);
	assert(status == EC_OK);

	// Create vector [10, 20, 30, 40, 50]
	for (int i = 1; i <= 5; i++)
		{
			status = vec.push(&vec, i * 10);
			assert(status == EC_OK);
		}
	size_t result = vec.contains(&vec, 30, cmp_int);
	assert(result == 1);

	size_t idx;
	status = vec.index_of(&vec, 40, &idx, cmp_int);
	assert(status == EC_OK && idx == 3);

	vec.free_fn(&vec);
	printf("Passed test: vector_int_contains and index_of\n");
}

/* Test binary_search() */
void
test_vector_int_binary_search(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_binary_search\n");

	ec_vector_int_t vec;
	ec_status_t status = ec_vector_int_init(&vec, 5, mptr);
	assert(status == EC_OK);

	// Push unsorted elements, then sort
	status = vec.push(&vec, 30);
	assert(status == EC_OK);
	status = vec.push(&vec, 10);
	assert(status == EC_OK);
	status = vec.push(&vec, 20);
	assert(status == EC_OK);

	vec.sort(&vec, cmp_int);
	size_t idx;
	status = vec.binary_search(&vec, 20, cmp_int, &idx);
	assert(status == EC_OK);

	vec.free_fn(&vec);
	printf("Passed test: vector_int_binary_search\n");
}

/* Test sort() and reverse() */
void
test_vector_int_sort_and_reverse(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_sort and reverse\n");

	ec_vector_int_t vec;
	ec_status_t status = ec_vector_int_init(&vec, 5, mptr);
	assert(status == EC_OK);

	status = vec.push(&vec, 3);
	assert(status == EC_OK);
	status = vec.push(&vec, 1);
	assert(status == EC_OK);
	status = vec.push(&vec, 2);
	assert(status == EC_OK);

	vec.sort(&vec, cmp_int);
	int val;
	status = vec.get(&vec, 0, &val);
	assert(status == EC_OK && val == 1);
	status = vec.get(&vec, 1, &val);
	assert(status == EC_OK && val == 2);
	status = vec.get(&vec, 2, &val);
	assert(status == EC_OK && val == 3);

	vec.reverse(&vec);
	status = vec.get(&vec, 0, &val);
	assert(status == EC_OK && val == 3);
	status = vec.get(&vec, 1, &val);
	assert(status == EC_OK && val == 2);
	status = vec.get(&vec, 2, &val);
	assert(status == EC_OK && val == 1);

	vec.free_fn(&vec);
	printf("Passed test: vector_int_sort and reverse\n");
}

// Multiply each element by 2
void
times_two(int* elem)
{
	*elem *= 2;
}

/* Test map() */
void
test_vector_int_map(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_map\n");

	ec_vector_int_t vec;
	ec_status_t status = ec_vector_int_init(&vec, 3, mptr);
	assert(status == EC_OK);

	status = vec.push(&vec, 5);
	assert(status == EC_OK);
	status = vec.push(&vec, 10);
	assert(status == EC_OK);

	vec.map(&vec, times_two);

	int val;
	status = vec.get(&vec, 0, &val);
	assert(status == EC_OK && val == 10);
	status = vec.get(&vec, 1, &val);
	assert(status == EC_OK && val == 20);

	vec.free_fn(&vec);
	printf("Passed test: vector_int_map\n");
}

// Multiply all elements: 1 * 2 * 3 * 4 = 24
void
multiply(int* acc, int* elem, int* out)
{
	*out = (*acc) * (*elem);
}

/* Test reduce() */
void
test_vector_int_reduce(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_reduce\n");

	ec_vector_int_t vec;
	ec_status_t status = ec_vector_int_init(&vec, 4, mptr);
	assert(status == EC_OK);

	status = vec.push(&vec, 1);
	assert(status == EC_OK);
	status = vec.push(&vec, 2);
	assert(status == EC_OK);
	status = vec.push(&vec, 3);
	assert(status == EC_OK);
	status = vec.push(&vec, 4);
	assert(status == EC_OK);

	int product = 0;
	vec.reduce(&vec, multiply, &product);
	assert(product == 24);

	vec.free_fn(&vec);
	printf("Passed test: vector_int_reduce\n");
}

/* Test subarray() */
void
test_vector_int_subarray(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_subarray\n");

	ec_vector_int_t vec;
	ec_status_t status = ec_vector_int_init(&vec, 6, mptr);
	assert(status == EC_OK);

	for (int i = 1; i <= 6; i++)
		{
			status = vec.push(&vec, i * 10);
			assert(status == EC_OK);
		}
	ec_vector_int_t sub;
	status = vec.subarray(&vec, 2, 5, &sub);
	assert(status == EC_OK && sub.size == 3);
	int val;
	status = sub.get(&sub, 0, &val);
	assert(status == EC_OK && val == 30);
	status = sub.get(&sub, 2, &val);
	assert(status == EC_OK && val == 50);

	sub.free_fn(&sub);
	vec.free_fn(&vec);
	printf("Passed test: vector_int_subarray\n");
}

/* Test copy_shallow() */
void
test_vector_int_copy_shallow(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_copy_shallow\n");

	ec_vector_int_t vec;
	ec_status_t status = ec_vector_int_init(&vec, 4, mptr);
	assert(status == EC_OK);

	for (int i = 1; i <= 4; i++)
		{
			status = vec.push(&vec, i);
			assert(status == EC_OK);
		}
	ec_vector_int_t copy;
	status = vec.copy_shallow(&vec, &copy);
	assert(status == EC_OK && copy.size == vec.size);

	for (size_t i = 0; i < copy.size; i++)
		{
			int a, b;
			status = vec.get(&vec, i, &a);
			status = copy.get(&copy, i, &b);
			assert(status == EC_OK && a == b);
		}

	copy.free_fn(&copy);
	vec.free_fn(&vec);
	printf("Passed test: vector_int_copy_shallow\n");
}

// Deep copy function returning value (matches copy_deep signature TYPE (*cp)(TYPE *))
int
copy_int_val(int* src)
{
	return *src;
}

/* Test copy_deep() */
void
test_vector_int_copy_deep(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_copy_deep\n");

	ec_vector_int_t vec;
	ec_status_t status = ec_vector_int_init(&vec, 4, mptr);
	assert(status == EC_OK);

	for (int i = 1; i <= 4; i++)
		{
			status = vec.push(&vec, i * 10);
			assert(status == EC_OK);
		}

	ec_vector_int_t copy;
	status = vec.copy_deep(&vec, copy_int_val, &copy);
	assert(status == EC_OK && copy.size == vec.size);

	for (size_t i = 0; i < copy.size; i++)
		{
			int a, b;
			status = vec.get(&vec, i, &a);
			status = copy.get(&copy, i, &b);
			assert(status == EC_OK && a == b);
		}

	copy.free_fn(&copy);
	vec.free_fn(&vec);
	printf("Passed test: vector_int_copy_deep\n");
}

// Filter: retain only even numbers.
bool
is_even(int num)
{
	return (num % 2) == 0;
}

/* Test filter() */
void
test_vector_int_filter(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_filter\n");

	ec_vector_int_t vec;
	ec_status_t status = ec_vector_int_init(&vec, 6, mptr);
	assert(status == EC_OK);
	int nums[] = { 10, 15, 20, 25, 30, 35 };
	for (int i = 0; i < 6; i++)
		{
			status = vec.push(&vec, nums[i]);
			assert(status == EC_OK);
		}

	ec_vector_int_t filtered;
	status = vec.filter(&vec, is_even, &filtered);
	assert(status == EC_OK && filtered.size == 3);

	filtered.free_fn(&filtered);
	vec.free_fn(&vec);
	printf("Passed test: vector_int_filter\n");
}

/* Test filter_mut() */
void
test_vector_int_filter_mut(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_filter_mut\n");

	ec_vector_int_t vec;
	ec_status_t status = ec_vector_int_init(&vec, 6, mptr);
	assert(status == EC_OK);
	int nums[] = { 10, 15, 20, 25, 30, 35 };
	for (int i = 0; i < 6; i++)
		{
			status = vec.push(&vec, nums[i]);
			assert(status == EC_OK);
		}
	status = vec.filter_mut(&vec, is_even);
	assert(status == EC_OK && vec.size == 3);

	vec.free_fn(&vec);
	printf("Passed test: vector_int_filter_mut\n");
}

/* Test size_fn() and capacity_fn() */
void
test_vector_int_size_and_capacity(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_size and capacity_fn\n");

	ec_vector_int_t vec;
	ec_status_t status = ec_vector_int_init(&vec, 10, mptr);
	assert(status == EC_OK);
	assert(vec.size_fn(&vec) == 0);
	assert(vec.capacity_fn(&vec) == 10);

	for (int i = 0; i < 5; i++)
		{
			status = vec.push(&vec, i + 1);
			assert(status == EC_OK);
		}
	assert(vec.size_fn(&vec) == 5);

	vec.free_fn(&vec);
	printf("Passed test: vector_int_size and capacity_fn\n");
}

/* Test trim_capacity() */
void
test_vector_int_trim_capacity(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_trim_capacity\n");

	ec_vector_int_t vec;
	ec_status_t status = ec_vector_int_init(&vec, 10, mptr);
	assert(status == EC_OK);
	for (int i = 0; i < 4; i++)
		{
			status = vec.push(&vec, (i + 1) * 10);
			assert(status == EC_OK);
		}
	status = vec.trim_capacity(&vec);
	assert(status == EC_OK && vec.capacity == vec.size);

	vec.free_fn(&vec);
	printf("Passed test: vector_int_trim_capacity\n");
}

/* Test iterator functions */
void
test_vector_int_iterators(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_iterators\n");

	ec_vector_int_t vec;
	ec_status_t status = ec_vector_int_init(&vec, 5, mptr);
	assert(status == EC_OK);
	for (int i = 0; i < 5; i++)
		{
			status = vec.push(&vec, i + 100);
			assert(status == EC_OK);
		}

	ec_vector_int_iter_t iter;
	status = ec_vector_int_iter_init(&iter, &vec);
	assert(status == EC_OK);
	int val;
	size_t count = 0;
	while ((status = ec_vector_int_iter_next(&iter, &val)) == EC_OK)
		{
			assert(val >= 100);
			count++;
		}
	assert(count == vec.size);

	// Test iterator removal: remove last element returned.
	status = ec_vector_int_iter_remove(&iter);
	assert(status == EC_OK);
	assert(vec.size == 4);

	// Test iterator add: add 99 at the current iterator position.
	status = ec_vector_int_iter_add(&iter, 99);
	assert(status == EC_OK);

	// Test iterator replace: replace previous element with 77.
	int replaced;
	status = ec_vector_int_iter_replace(&iter, 77, &replaced);
	assert(status == EC_OK);

	vec.free_fn(&vec);
	printf("Passed test: vector_int_iterators\n");
}

/* Test zip iterator functions */
void
test_vector_int_zip_iterators(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_int_zip_iterators\n");

	ec_vector_int_t vec1, vec2;
	ec_status_t status = ec_vector_int_init(&vec1, 5, mptr);
	assert(status == EC_OK);
	status = ec_vector_int_init(&vec2, 5, mptr);
	assert(status == EC_OK);
	for (int i = 0; i < 5; i++)
		{
			status = vec1.push(&vec1, i + 1);
			assert(status == EC_OK);
			status = vec2.push(&vec2, (i + 1) * 10);
			assert(status == EC_OK);
		}

	ec_vector_int_zip_iter_t zip;
	status = ec_vector_int_zip_iter_init(&zip, &vec1, &vec2);
	assert(status == EC_OK);
	int a, b;
	size_t zip_count = 0;
	while ((status = ec_vector_int_zip_iter_next(&zip, &a, &b)) == EC_OK)
		{
			assert(b == a * 10);
			zip_count++;
		}
	assert(zip_count == vec1.size);

	// Test zip iterator removal: remove the last pair.
	status = ec_vector_int_zip_iter_remove(&zip);
	assert(status == EC_OK);
	assert(vec1.size == 4 && vec2.size == 4);

	// Test zip iterator add: add pair (7,70)
	status = ec_vector_int_zip_iter_add(&zip, 7, 70);
	assert(status == EC_OK);

	// Test zip iterator replace: replace the last pair with (8,80)
	int r1, r2;
	status = ec_vector_int_zip_iter_replace(&zip, 8, 80, &r1, &r2);
	assert(status == EC_OK);

	vec1.free_fn(&vec1);
	vec2.free_fn(&vec2);
	printf("Passed test: vector_int_zip_iterators\n");
}

/*---------------------------------------------------------------------------
  Main Function
---------------------------------------------------------------------------*/
int
main(void)
{
	printf("Starting ec_vector_int function tests...\n\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t pool_conf = {
		.block_size = 64,
		.num_blocks = 100,
		.pool_type = EC_MEMPOOL_DYNAMIC,
		.thread_mode = EC_THREAD_UNSAFE,
		.alignment = 8,
		.max_chunks = 0,
		.expansion_factor = 2.0f,
		.mem_alloc = NULL,
		.mem_realloc = NULL,
		.mem_free = NULL
	};
	ec_mempool_t pool;
	ec_status_t status = ec_mempool_init(&pool, &pool_conf);
	assert(status == EC_OK);
	void* mptr = &pool;
#else
	void* mptr = NULL;
#endif

	test_vector_int_push(mptr);
	test_vector_int_insert(mptr);
	test_vector_int_remove_at(mptr);
	test_vector_int_get(mptr);
	test_vector_int_replace_at(mptr);
	test_vector_int_swap(mptr);
	test_vector_int_contains_and_index_of(mptr);
	test_vector_int_binary_search(mptr);
	test_vector_int_sort_and_reverse(mptr);
	test_vector_int_map(mptr);
	test_vector_int_reduce(mptr);
	test_vector_int_subarray(mptr);
	test_vector_int_copy_shallow(mptr);
	test_vector_int_copy_deep(mptr);
	test_vector_int_filter(mptr);
	test_vector_int_filter_mut(mptr);
	test_vector_int_size_and_capacity(mptr);
	test_vector_int_trim_capacity(mptr);
	test_vector_int_iterators(mptr);
	test_vector_int_zip_iterators(mptr);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mptr);
#endif

	printf("\nAll ec_vector_int function tests passed successfully.\n");
	return 0;
}
