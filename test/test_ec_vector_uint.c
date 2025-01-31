/*
 * test_vector_uint.c
 *
 * Unit tests for ec_vector_uint operations.
 * This file tests all functions (push, insert, remove_at, get, replace_at, swap,
 * contains, index_of, binary_search, sort, reverse, map, reduce, subarray,
 * copy_shallow, copy_deep, filter, filter_mut, size/capacity, trim_capacity,
 * iterators, and zip iterators) for a vector of unsigned int.
 * Corner cases (invalid indices, empty vector operations, etc.) are also checked.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "../src/ec_vector.h"

#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

/*---------------------------------------------------------------------------
  Helper Function: print_timestamp
  Prints the current date/time in a formatted string.
---------------------------------------------------------------------------*/

void print_timestamp(void) {
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    char buffer[64];
    if (local) {
        strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", local);
        printf("%s ", buffer);
    } else {
        printf("[timestamp error] ");
    }
}

/*---------------------------------------------------------------------------
  Helper Functions for unsigned int
---------------------------------------------------------------------------*/
// Comparator for unsigned int.
int cmp_uint(const void *a, const void *b) {
    unsigned int ua = *(const unsigned int *)a;
    unsigned int ub = *(const unsigned int *)b;
    if (ua < ub)
        return -1;
    else if (ua > ub)
        return 1;
    else
        return 0;
}

// Increment function.
void inc_uint(unsigned int *elem) {
    (*elem)++;
}

// Sum function for reduce (summing elements).
void sum_uint(unsigned int *acc, unsigned int *elem, unsigned int *out) {
    *out = (*acc) + (*elem);
}

// Multiply an element by two (for map).
void times_two_uint(unsigned int *elem) {
    *elem *= 2;
}

// Predicate for filter: returns true if the value is even.
bool is_even_uint(unsigned int value) {
    return (value % 2) == 0;
}

/*---------------------------------------------------------------------------
  Test Functions for ec_vector_uint
---------------------------------------------------------------------------*/

/* Test push() and out-of-bound get() */
void test_vector_uint_push(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_push\n");

    ec_vector_uint_t vec;
    ec_status_t status = ec_vector_uint_init(&vec, 3, mptr);
    assert(status == EC_OK);

    status = vec.push(&vec, 1);  assert(status == EC_OK);
    status = vec.push(&vec, 2);  assert(status == EC_OK);
    status = vec.push(&vec, 3);  assert(status == EC_OK);

    assert(vec.size == 3);
    unsigned int val;
    status = vec.get(&vec, 0, &val); assert(status == EC_OK && val == 1);
    status = vec.get(&vec, 1, &val); assert(status == EC_OK && val == 2);
    status = vec.get(&vec, 2, &val); assert(status == EC_OK && val == 3);

    // Corner case: Attempt to get an element at index equal to size.
    status = vec.get(&vec, 3, &val);
    assert(status == EC_ERR_OUT_OF_BOUNDS);

    vec.free_fn(&vec);
    printf("Passed test: vector_uint_push\n");
}

/* Test insert() including boundary and error cases */
void test_vector_uint_insert(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_insert\n");

    ec_vector_uint_t vec;
    ec_status_t status = ec_vector_uint_init(&vec, 5, mptr);
    assert(status == EC_OK);

    // Corner case: Insert into an empty vector at index 0.
    status = vec.insert(&vec, 0, 10);
    assert(status == EC_OK && vec.size == 1);
    unsigned int val;
    status = vec.get(&vec, 0, &val);
    assert(status == EC_OK && val == 10);

    // Insert at the end.
    status = vec.insert(&vec, 1, 20);
    assert(status == EC_OK && vec.size == 2);
    status = vec.get(&vec, 1, &val);
    assert(status == EC_OK && val == 20);

    // Insert in the middle.
    status = vec.insert(&vec, 1, 15);
    assert(status == EC_OK && vec.size == 3);
    status = vec.get(&vec, 1, &val);
    assert(status == EC_OK && val == 15);

    // Corner case: Insert at an index greater than size.
    status = vec.insert(&vec, 5, 100); // valid indices: 0..3; index 5 is invalid.
    assert(status == EC_ERR_OUT_OF_BOUNDS);

    vec.free_fn(&vec);
    printf("Passed test: vector_uint_insert\n");
}

/* Test remove_at() including corner cases */
void test_vector_uint_remove_at(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_remove_at\n");

    ec_vector_uint_t vec;
    ec_status_t status = ec_vector_uint_init(&vec, 5, mptr);
    assert(status == EC_OK);

    // Build vector: [10, 20, 30, 40]
    status = vec.push(&vec, 10); assert(status == EC_OK);
    status = vec.push(&vec, 20); assert(status == EC_OK);
    status = vec.push(&vec, 30); assert(status == EC_OK);
    status = vec.push(&vec, 40); assert(status == EC_OK);

    // Remove element at index 2 (30).
    status = vec.remove_at(&vec, 2);
    assert(status == EC_OK && vec.size == 3);
    unsigned int val;
    status = vec.get(&vec, 0, &val); assert(status == EC_OK && val == 10);
    status = vec.get(&vec, 1, &val); assert(status == EC_OK && val == 20);
    status = vec.get(&vec, 2, &val); assert(status == EC_OK && val == 40);

    // Corner case: Remove at an index that does not exist.
    status = vec.remove_at(&vec, 3);
    assert(status == EC_ERR_OUT_OF_BOUNDS);

    // Corner case: Remove from an empty vector.
    while (vec.size > 0) {
        status = vec.remove_at(&vec, 0);
        assert(status == EC_OK);
    }
    status = vec.remove_at(&vec, 0);
    assert(status == EC_ERR_OUT_OF_BOUNDS);

    vec.free_fn(&vec);
    printf("Passed test: vector_uint_remove_at\n");
}

/* Test get() including error on out-of-bound access */
void test_vector_uint_get(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_get\n");

    ec_vector_uint_t vec;
    ec_status_t status = ec_vector_uint_init(&vec, 3, mptr);
    assert(status == EC_OK);

    status = vec.push(&vec, 100); assert(status == EC_OK);
    status = vec.push(&vec, 200); assert(status == EC_OK);

    unsigned int val;
    status = vec.get(&vec, 1, &val);
    assert(status == EC_OK && val == 200);

    // Corner case: Accessing an index that does not exist.
    status = vec.get(&vec, 2, &val);
    assert(status == EC_ERR_OUT_OF_BOUNDS);

    vec.free_fn(&vec);
    printf("Passed test: vector_uint_get\n");
}

/* Test replace_at() including error checking */
void test_vector_uint_replace_at(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_replace_at\n");

    ec_vector_uint_t vec;
    ec_status_t status = ec_vector_uint_init(&vec, 3, mptr);
    assert(status == EC_OK);

    status = vec.push(&vec, 5);  assert(status == EC_OK);
    status = vec.push(&vec, 6);  assert(status == EC_OK);

    unsigned int old_val;
    status = vec.replace_at(&vec, 1, 99, &old_val);
    assert(status == EC_OK && old_val == 6);
    unsigned int val;
    status = vec.get(&vec, 1, &val);
    assert(status == EC_OK && val == 99);

    // Corner case: Replace at an invalid index.
    status = vec.replace_at(&vec, 2, 100, &old_val);
    assert(status == EC_ERR_OUT_OF_BOUNDS);

    vec.free_fn(&vec);
    printf("Passed test: vector_uint_replace_at\n");
}

/* Test swap() including error on invalid indices */
void test_vector_uint_swap(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_swap\n");

    ec_vector_uint_t vec;
    ec_status_t status = ec_vector_uint_init(&vec, 3, mptr);
    assert(status == EC_OK);

    status = vec.push(&vec, 11);  assert(status == EC_OK);
    status = vec.push(&vec, 22);  assert(status == EC_OK);
    status = vec.push(&vec, 33);  assert(status == EC_OK);

    status = vec.swap(&vec, 0, 2);
    assert(status == EC_OK);
    unsigned int a, b;
    status = vec.get(&vec, 0, &a); assert(status == EC_OK && a == 33);
    status = vec.get(&vec, 2, &b); assert(status == EC_OK && b == 11);

    // Corner case: Swap using an invalid index.
    status = vec.swap(&vec, 0, 3);
    assert(status == EC_ERR_OUT_OF_BOUNDS);

    vec.free_fn(&vec);
    printf("Passed test: vector_uint_swap\n");
}

/* Test contains() and index_of() */
void test_vector_uint_contains_and_index_of(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_contains and index_of\n");

    ec_vector_uint_t vec;
    ec_status_t status = ec_vector_uint_init(&vec, 5, mptr);
    assert(status == EC_OK);

    // Create vector: [10, 20, 30, 40, 50]
    for (unsigned int i = 1; i <= 5; i++) {
        status = vec.push(&vec, i * 10);
        assert(status == EC_OK);
    }
    size_t found = vec.contains(&vec, 30, cmp_uint);
    assert(found == 1);
    found = vec.contains(&vec, 35, cmp_uint);
    assert(found == 0);

    size_t idx;
    status = vec.index_of(&vec, 40, &idx, cmp_uint);
    assert(status == EC_OK && idx == 3);

    // Corner case: index_of for a non-existent element.
    status = vec.index_of(&vec, 35, &idx, cmp_uint);
    assert(status == EC_ERR_OUT_OF_BOUNDS);

    vec.free_fn(&vec);
    printf("Passed test: vector_uint_contains and index_of\n");
}

/* Test binary_search() including not-found condition */
void test_vector_uint_binary_search(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_binary_search\n");

    ec_vector_uint_t vec;
    ec_status_t status = ec_vector_uint_init(&vec, 5, mptr);
    assert(status == EC_OK);

    // Insert unsorted elements then sort.
    status = vec.push(&vec, 30); assert(status == EC_OK);
    status = vec.push(&vec, 10); assert(status == EC_OK);
    status = vec.push(&vec, 20); assert(status == EC_OK);

    vec.sort(&vec, cmp_uint);
    size_t idx;
    status = vec.binary_search(&vec, 20, cmp_uint, &idx);
    assert(status == EC_OK);

    // Corner case: Search for an element not in the vector.
    status = vec.binary_search(&vec, 25, cmp_uint, &idx);
    assert(status == EC_ERR_OUT_OF_BOUNDS);

    vec.free_fn(&vec);
    printf("Passed test: vector_uint_binary_search\n");
}

/* Test sort() and reverse() */
void test_vector_uint_sort_and_reverse(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_sort and reverse\n");

    ec_vector_uint_t vec;
    ec_status_t status = ec_vector_uint_init(&vec, 5, mptr);
    assert(status == EC_OK);

    status = vec.push(&vec, 3); assert(status == EC_OK);
    status = vec.push(&vec, 1); assert(status == EC_OK);
    status = vec.push(&vec, 2); assert(status == EC_OK);

    vec.sort(&vec, cmp_uint);
    unsigned int val;
    status = vec.get(&vec, 0, &val); assert(status == EC_OK && val == 1);
    status = vec.get(&vec, 1, &val); assert(status == EC_OK && val == 2);
    status = vec.get(&vec, 2, &val); assert(status == EC_OK && val == 3);

    vec.reverse(&vec);
    status = vec.get(&vec, 0, &val); assert(status == EC_OK && val == 3);
    status = vec.get(&vec, 1, &val); assert(status == EC_OK && val == 2);
    status = vec.get(&vec, 2, &val); assert(status == EC_OK && val == 1);

    vec.free_fn(&vec);
    printf("Passed test: vector_uint_sort and reverse\n");
}

/* Test map() */
void test_vector_uint_map(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_map\n");

    ec_vector_uint_t vec;
    ec_status_t status = ec_vector_uint_init(&vec, 3, mptr);
    assert(status == EC_OK);

    status = vec.push(&vec, 5);  assert(status == EC_OK);
    status = vec.push(&vec, 10); assert(status == EC_OK);

    vec.map(&vec, times_two_uint);

    unsigned int val;
    status = vec.get(&vec, 0, &val); assert(status == EC_OK && val == 10);
    status = vec.get(&vec, 1, &val); assert(status == EC_OK && val == 20);

    vec.free_fn(&vec);
    printf("Passed test: vector_uint_map\n");
}

/* Test reduce() */
void test_vector_uint_reduce(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_reduce\n");

    ec_vector_uint_t vec;
    ec_status_t status = ec_vector_uint_init(&vec, 4, mptr);
    assert(status == EC_OK);

    status = vec.push(&vec, 1); assert(status == EC_OK);
    status = vec.push(&vec, 2); assert(status == EC_OK);
    status = vec.push(&vec, 3); assert(status == EC_OK);
    status = vec.push(&vec, 4); assert(status == EC_OK);

    unsigned int sum = 0;
    vec.reduce(&vec, sum_uint, &sum);
    assert(sum == 10); // 1 + 2 + 3 + 4 = 10

    vec.free_fn(&vec);
    printf("Passed test: vector_uint_reduce\n");
}

/* Test subarray() including invalid range */
void test_vector_uint_subarray(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_subarray\n");

    ec_vector_uint_t vec;
    ec_status_t status = ec_vector_uint_init(&vec, 6, mptr);
    assert(status == EC_OK);

    for (unsigned int i = 1; i <= 6; i++) {
        status = vec.push(&vec, i * 10);
        assert(status == EC_OK);
    }
    ec_vector_uint_t sub;
    // Valid subarray extraction: indices 2 to 4 (3 elements)
    status = vec.subarray(&vec, 2, 5, &sub);
    assert(status == EC_OK && sub.size == 3);
    unsigned int val;
    status = sub.get(&sub, 0, &val); assert(status == EC_OK && val == 30);
    status = sub.get(&sub, 2, &val); assert(status == EC_OK && val == 50);

    // Corner case: Request a subarray with start > end.
    status = vec.subarray(&vec, 4, 2, &sub);
    assert(status != EC_OK);

    sub.free_fn(&sub);
    vec.free_fn(&vec);
    printf("Passed test: vector_uint_subarray\n");
}

/* Test copy_shallow() */
void test_vector_uint_copy_shallow(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_copy_shallow\n");

    ec_vector_uint_t vec;
    ec_status_t status = ec_vector_uint_init(&vec, 4, mptr);
    assert(status == EC_OK);

    for (unsigned int i = 1; i <= 4; i++) {
        status = vec.push(&vec, i * 10);
        assert(status == EC_OK);
    }
    ec_vector_uint_t copy;
    status = vec.copy_shallow(&vec, &copy);
    assert(status == EC_OK && copy.size == vec.size);

    for (size_t i = 0; i < copy.size; i++) {
        unsigned int a, b;
        status = vec.get(&vec, i, &a);
        status = copy.get(&copy, i, &b);
        assert(status == EC_OK && a == b);
    }

    copy.free_fn(&copy);
    vec.free_fn(&vec);
    printf("Passed test: vector_uint_copy_shallow\n");
}

/* Deep copy helper for unsigned int */
unsigned int* copy_uint(unsigned int *src) {
    unsigned int *dest = (unsigned int*)malloc(sizeof(unsigned int));
    *dest = *src;
    return dest;
}

/* Test copy_deep() */
void test_vector_uint_copy_deep(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_copy_deep\n");

    ec_vector_uint_t vec;
    ec_status_t status = ec_vector_uint_init(&vec, 4, mptr);
    assert(status == EC_OK);

    for (unsigned int i = 1; i <= 4; i++) {
        status = vec.push(&vec, i * 10);
        assert(status == EC_OK);
    }

    ec_vector_uint_t copy;
    status = vec.copy_deep(&vec, (unsigned int*(*)(unsigned int*))copy_uint, &copy);
    assert(status == EC_OK && copy.size == vec.size);

    for (size_t i = 0; i < copy.size; i++) {
        unsigned int a, b;
        status = vec.get(&vec, i, &a);
        status = copy.get(&copy, i, &b);
        assert(status == EC_OK && a == b);
    }

    copy.free_fn(&copy);
    vec.free_fn(&vec);
    printf("Passed test: vector_uint_copy_deep\n");
}

/* Test filter() */
void test_vector_uint_filter(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_filter\n");

    ec_vector_uint_t vec;
    ec_status_t status = ec_vector_uint_init(&vec, 6, mptr);
    assert(status == EC_OK);
    unsigned int nums[] = {10, 15, 20, 25, 30, 35};
    for (unsigned int i = 0; i < 6; i++) {
        status = vec.push(&vec, nums[i]);
        assert(status == EC_OK);
    }

    ec_vector_uint_t filtered;
    status = vec.filter(&vec, is_even_uint, &filtered);
    assert(status == EC_OK && filtered.size == 3);

    // Verify that every element in the filtered vector is even.
    for (size_t i = 0; i < filtered.size; i++) {
        unsigned int num;
        status = filtered.get(&filtered, i, &num);
        assert(status == EC_OK && (num % 2 == 0));
    }

    filtered.free_fn(&filtered);
    vec.free_fn(&vec);
    printf("Passed test: vector_uint_filter\n");
}

/* Test filter_mut() */
void test_vector_uint_filter_mut(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_filter_mut\n");

    ec_vector_uint_t vec;
    ec_status_t status = ec_vector_uint_init(&vec, 6, mptr);
    assert(status == EC_OK);
    unsigned int nums[] = {10, 15, 20, 25, 30, 35};
    for (unsigned int i = 0; i < 6; i++) {
        status = vec.push(&vec, nums[i]);
        assert(status == EC_OK);
    }
    status = vec.filter_mut(&vec, is_even_uint);
    assert(status == EC_OK && vec.size == 3);

    // Verify remaining elements are even.
    for (size_t i = 0; i < vec.size; i++) {
        unsigned int num;
        status = vec.get(&vec, i, &num);
        assert(status == EC_OK && (num % 2 == 0));
    }

    vec.free_fn(&vec);
    printf("Passed test: vector_uint_filter_mut\n");
}

/* Test size_fn() and capacity_fn() */
void test_vector_uint_size_and_capacity(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_size and capacity_fn\n");

    ec_vector_uint_t vec;
    ec_status_t status = ec_vector_uint_init(&vec, 10, mptr);
    assert(status == EC_OK);
    assert(vec.size_fn(&vec) == 0);
    assert(vec.capacity_fn(&vec) == 10);

    for (unsigned int i = 0; i < 5; i++) {
        status = vec.push(&vec, i + 1);
        assert(status == EC_OK);
    }
    assert(vec.size_fn(&vec) == 5);

    vec.free_fn(&vec);
    printf("Passed test: vector_uint_size and capacity_fn\n");
}

/* Test trim_capacity() */
void test_vector_uint_trim_capacity(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_trim_capacity\n");

    ec_vector_uint_t vec;
    ec_status_t status = ec_vector_uint_init(&vec, 10, mptr);
    assert(status == EC_OK);
    for (unsigned int i = 0; i < 4; i++) {
        status = vec.push(&vec, (i + 1) * 10);
        assert(status == EC_OK);
    }
    status = vec.trim_capacity(&vec);
    assert(status == EC_OK && vec.capacity == vec.size);

    vec.free_fn(&vec);
    printf("Passed test: vector_uint_trim_capacity\n");
}

/* Test iterator functions */
void test_vector_uint_iterators(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_iterators\n");

    ec_vector_uint_t vec;
    ec_status_t status = ec_vector_uint_init(&vec, 5, mptr);
    assert(status == EC_OK);
    for (unsigned int i = 0; i < 5; i++) {
        status = vec.push(&vec, i + 100);
        assert(status == EC_OK);
    }

    ec_vector_uint_iter_t iter;
    status = ec_vector_uint_iter_init(&iter, &vec);
    assert(status == EC_OK);
    unsigned int val;
    size_t count = 0;
    while ((status = ec_vector_uint_iter_next(&iter, &val)) == EC_OK) {
        assert(val >= 100);
        count++;
    }
    assert(count == vec.size);

    // Test iterator removal: remove last returned element.
    status = ec_vector_uint_iter_remove(&iter);
    assert(status == EC_OK);
    assert(vec.size == 4);

    // Test iterator add: add an element at the iterator’s current position.
    status = ec_vector_uint_iter_add(&iter, 99);
    assert(status == EC_OK);

    // Test iterator replace: replace the previous element.
    unsigned int replaced;
    status = ec_vector_uint_iter_replace(&iter, 77, &replaced);
    assert(status == EC_OK);

    vec.free_fn(&vec);
    printf("Passed test: vector_uint_iterators\n");
}

/* Test zip iterator functions */
void test_vector_uint_zip_iterators(void *mptr) {
    print_timestamp();
    printf("Running test: vector_uint_zip_iterators\n");

    ec_vector_uint_t vec1, vec2;
    ec_status_t status = ec_vector_uint_init(&vec1, 5, mptr);
    assert(status == EC_OK);
    status = ec_vector_uint_init(&vec2, 5, mptr);
    assert(status == EC_OK);
    for (unsigned int i = 0; i < 5; i++) {
        status = vec1.push(&vec1, i + 1);
        assert(status == EC_OK);
        status = vec2.push(&vec2, (i + 1) * 10);
        assert(status == EC_OK);
    }

    ec_vector_uint_zip_iter_t zip;
    status = ec_vector_uint_zip_iter_init(&zip, &vec1, &vec2);
    assert(status == EC_OK);
    unsigned int a, b;
    size_t zip_count = 0;
    while ((status = ec_vector_uint_zip_iter_next(&zip, &a, &b)) == EC_OK) {
        assert(b == a * 10);
        zip_count++;
    }
    assert(zip_count == vec1.size);

    // Test zip iterator removal: remove the last pair.
    status = ec_vector_uint_zip_iter_remove(&zip);
    assert(status == EC_OK);
    assert(vec1.size == 4 && vec2.size == 4);

    // Test zip iterator add: add a new pair.
    status = ec_vector_uint_zip_iter_add(&zip, 7, 70);
    assert(status == EC_OK);

    // Test zip iterator replace: replace the last pair.
    unsigned int r1, r2;
    status = ec_vector_uint_zip_iter_replace(&zip, 8, 80, &r1, &r2);
    assert(status == EC_OK);

    vec1.free_fn(&vec1);
    vec2.free_fn(&vec2);
    printf("Passed test: vector_uint_zip_iterators\n");
}

/*---------------------------------------------------------------------------
  Main Function: Run all tests
---------------------------------------------------------------------------*/
int main(void) {
    printf("Starting ec_vector_uint function tests...\n\n");
#ifdef EC_USE_MEMPOOL
    ec_mempool_conf_t pool_conf = {
         .block_size        = 64,
         .num_blocks        = 100,
         .pool_type         = EC_MEMPOOL_DYNAMIC,
         .thread_mode       = EC_THREAD_UNSAFE,
         .alignment         = 8,
         .max_chunks        = 0,
         .expansion_factor  = 2.0f,
         .mem_alloc         = NULL,
         .mem_realloc       = NULL,
         .mem_free          = NULL
    };
    ec_mempool_t pool;
    ec_status_t status = ec_mempool_init(&pool, &pool_conf);
    assert(status == EC_OK);
    void *mptr = &pool;
#else
    void *mptr = NULL;
#endif

    test_vector_uint_push(mptr);
    test_vector_uint_insert(mptr);
    test_vector_uint_remove_at(mptr);
    test_vector_uint_get(mptr);
    test_vector_uint_replace_at(mptr);
    test_vector_uint_swap(mptr);
    test_vector_uint_contains_and_index_of(mptr);
    test_vector_uint_binary_search(mptr);
    test_vector_uint_sort_and_reverse(mptr);
    test_vector_uint_map(mptr);
    test_vector_uint_reduce(mptr);
    test_vector_uint_subarray(mptr);
    test_vector_uint_copy_shallow(mptr);
    test_vector_uint_copy_deep(mptr);
    test_vector_uint_filter(mptr);
    test_vector_uint_filter_mut(mptr);
    test_vector_uint_size_and_capacity(mptr);
    test_vector_uint_trim_capacity(mptr);
    test_vector_uint_iterators(mptr);
    test_vector_uint_zip_iterators(mptr);

#ifdef EC_USE_MEMPOOL
    ec_mempool_destroy((ec_mempool_t *)mptr);
#endif

    printf("\nAll ec_vector_uint function tests passed successfully.\n");
    return 0;
}
