#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h> /* For backtrace() and backtrace_symbols() on Linux/Mac */
#endif
#include "../../src/ec_slist.h"
#include <assert.h>
#include <stdbool.h>

#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

/*-----------------------------------------------------------------------------*
  Simple stack trace on assertion failure (POSIX/Linux/Mac). On other OS, stub.
 *-----------------------------------------------------------------------------*/
static void
print_stacktrace(void)
{
#ifdef __linux__
	void* array[32U];
	size_t size = backtrace(array, 32U);
	char** symbols = backtrace_symbols(array, size);
	if (symbols)
		{
			fprintf(stderr, "--- Stack Trace (most recent call last) ---\n");
			for (size_t i = 0U; i < size; i++)
				{
					fprintf(stderr, "%s\n", symbols[i]);
				}
			free(symbols);
		}
#else
	fprintf(stderr, "(Stack trace not supported on this platform)\n");
#endif
}

/*-----------------------------------------------------------------------------*
  A custom assertion macro that prints stack trace on failure, then exits(3U).
 *-----------------------------------------------------------------------------*/
#define TEST_ASSERT(cond)                                                                    \
	do                                                                                       \
		{                                                                                    \
			if (!(cond))                                                                     \
				{                                                                            \
					fprintf(stderr, "[ASSERT FAIL] %s:%d: %s\n", __FILE__, __LINE__, #cond); \
					print_stacktrace();                                                      \
					exit(3U);                                                                \
				}                                                                            \
		}                                                                                    \
	while (0U)

/*-----------------------------------------------------------------------------*
  Time stamp for logging
 *-----------------------------------------------------------------------------*/
static void
stamp(void)
{
	time_t t = time(NULL);
	struct tm* m = localtime(&t);
	if (m)
		{
			char b[64U];
			strftime(b, sizeof(b), "[%Y-%m-%d %H:%M:%S]", m);
			printf("%s ", b);
		}
	else
		{
			printf("[timeerr] ");
		}
}

static int
cmp_uint(const void* a, const void* b)
{
	return (*(unsigned int*)a - *(unsigned int*)b);
}
static bool
is_even(const void* x)
{
	return ((*(unsigned int*)x) % 2U) == 0U;
}
static void
inc_uint(void* x)
{
	(*(unsigned int*)x)++;
}
static void
add_uints(void* a, void* b, void* out)
{
	*(unsigned int*)out = (*(unsigned int*)a) + (*(unsigned int*)b);
}
static void
mul_uints(void* a, void* b, void* out)
{
	*(unsigned int*)out = (*(unsigned int*)a) * (*(unsigned int*)b);
}

/*-----------------------------------------------------------------------------*
  Tests
 *-----------------------------------------------------------------------------*/

void
test_slist_uint_add(void* mp)
{
	stamp();
	printf("test_slist_uint_add\n");
	ec_slist_uint_t l;
	ec_status_t s = ec_slist_uint_init(&l, mp);
	TEST_ASSERT(s == EC_OK);
	unsigned int a = 10U, b = 20U;
	l.add(&l, &a);
	l.add(&l, &b);
	TEST_ASSERT(l.size_fn(&l) == 2U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_add\n");
}

void
test_slist_uint_add_at(void* mp)
{
	stamp();
	printf("test_slist_uint_add_at\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int x = 1U, y = 2U, z = 3U;
	l.add(&l, &x);
	l.add(&l, &z);
	ec_status_t st = l.add_at(&l, &y, 1U);
	TEST_ASSERT(st == EC_OK);
	void* out;
	l.get_at(&l, 1U, &out);
	TEST_ASSERT(*(unsigned int*)out == 2U);
	TEST_ASSERT(l.size_fn(&l) == 3U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_add_at\n");
}

void
test_slist_uint_remove(void* mp)
{
	stamp();
	printf("test_slist_uint_remove\n");

	ec_slist_uint_t l;
	ec_status_t init_status = ec_slist_uint_init(&l, mp);
	TEST_ASSERT(init_status == EC_OK);

	unsigned int a = 10U, b = 20U, c = 30U;
	ec_status_t add_status_a = l.add(&l, &a);
	ec_status_t add_status_b = l.add(&l, &b);
	ec_status_t add_status_c = l.add(&l, &c);

	TEST_ASSERT(add_status_a == EC_OK);
	TEST_ASSERT(add_status_b == EC_OK);
	TEST_ASSERT(add_status_c == EC_OK);

	unsigned int v = 20U;

	ec_status_t s = l.remove_fn(&l, &v);

	// Verify status code and pointer before dereferencing
	TEST_ASSERT(s == EC_OK);
	TEST_ASSERT(l.size_fn(&l) == 2U); // List should now have 2U elements

	l.free_fn(&l);
	printf("pass: test_slist_uint_remove\n");
}

void
test_slist_uint_remove_first(void* mp)
{
	stamp();
	printf("test_slist_uint_remove_first\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 11U, b = 22U;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_status_t s = l.remove_first(&l);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 1U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_remove_first\n");
}

void
test_slist_uint_remove_last(void* mp)
{
	stamp();
	printf("test_slist_uint_remove_last\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 33U, b = 44U;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_status_t s = l.remove_last(&l);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 1U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_remove_last\n");
}

void
test_slist_uint_remove_at(void* mp)
{
	stamp();
	printf("test_slist_uint_remove_at\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 10U, b = 20U, c = 30U;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	ec_status_t s = l.remove_at(&l, 1U);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 2U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_remove_at\n");
}

void
test_slist_uint_get_at(void* mp)
{
	stamp();
	printf("test_slist_uint_get_at\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 100U, b = 200U;
	l.add(&l, &a);
	l.add(&l, &b);
	void* out = NULL;
	ec_status_t s = l.get_at(&l, 1U, &out);
	TEST_ASSERT(s == EC_OK && *(unsigned int*)out == 200U);
	s = l.get_at(&l, 2U, &out);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_uint_get_at\n");
}

void
test_slist_uint_get_first(void* mp)
{
	stamp();
	printf("test_slist_uint_get_first\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 777U;
	l.add(&l, &a);
	void* out = NULL;
	ec_status_t s = l.get_first(&l, &out);
	TEST_ASSERT(s == EC_OK && *(unsigned int*)out == 777U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_get_first\n");
}

void
test_slist_uint_get_last(void* mp)
{
	stamp();
	printf("test_slist_uint_get_last\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 10U, b = 20U;
	l.add(&l, &a);
	l.add(&l, &b);
	void* out;
	ec_status_t s = l.get_last(&l, &out);
	TEST_ASSERT(s == EC_OK && *(unsigned int*)out == 20U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_get_last\n");
}

void
test_slist_uint_splice(void* mp)
{
	stamp();
	printf("test_slist_uint_splice\n");
	ec_slist_uint_t l1, l2;
	ec_slist_uint_init(&l1, mp);
	ec_slist_uint_init(&l2, mp);
	unsigned int a = 10U, b = 20U, c = 30U, d = 40U;
	l1.add(&l1, &a);
	l1.add(&l1, &b);
	l2.add(&l2, &c);
	l2.add(&l2, &d);
	ec_status_t s = l1.splice(&l1, &l2);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 4U && l2.size_fn(&l2) == 0U);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_uint_splice\n");
}

void
test_slist_uint_splice_at(void* mp)
{
	stamp();
	printf("test_slist_uint_splice_at\n");
	ec_slist_uint_t l1, l2;
	ec_slist_uint_init(&l1, mp);
	ec_slist_uint_init(&l2, mp);
	unsigned int a = 1U, b = 2U, c = 3U, d = 4U, e = 5U;
	l1.add(&l1, &a);
	l1.add(&l1, &b);
	l1.add(&l1, &c);
	l2.add(&l2, &d);
	l2.add(&l2, &e);
	ec_status_t s = l1.splice_at(&l1, &l2, 1U);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 5U && l2.size_fn(&l2) == 0U);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_uint_splice_at\n");
}

void
test_slist_uint_add_all(void* mp)
{
	stamp();
	printf("test_slist_uint_add_all\n");
	ec_slist_uint_t l1, l2;
	ec_slist_uint_init(&l1, mp);
	ec_slist_uint_init(&l2, mp);
	unsigned int a = 10U, b = 20U;
	l2.add(&l2, &a);
	l2.add(&l2, &b);
	ec_status_t s = l1.add_all(&l1, &l2);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 2U && l2.size_fn(&l2) == 2U);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_uint_add_all\n");
}

void
test_slist_uint_add_all_at(void* mp)
{
	stamp();
	printf("test_slist_uint_add_all_at\n");
	ec_slist_uint_t l1, l2;
	ec_slist_uint_init(&l1, mp);
	ec_slist_uint_init(&l2, mp);
	unsigned int a = 10U, b = 30U, c = 20U, d = 25U;
	l1.add(&l1, &a);
	l1.add(&l1, &b);
	l2.add(&l2, &c);
	l2.add(&l2, &d);
	ec_status_t s = l1.add_all_at(&l1, &l2, 1U);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 4U);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_uint_add_all_at\n");
}

void
test_slist_uint_sublist(void* mp)
{
	stamp();
	printf("test_slist_uint_sublist\n");

	ec_slist_uint_t l;
	ec_status_t init_status = ec_slist_uint_init(&l, mp);
	if (init_status != EC_OK)
		{
			printf("ERROR: Failed to initialize the list. Status: %d\n", init_status);
			TEST_ASSERT(0U); // Force failure
		}

	// Adding elements: 10U, 20U, 30U, 40U, 50U
	for (unsigned int i = 0U; i < 5U; i++)
		{
			unsigned int v = (i + 1U) * 10U;
			ec_status_t add_status = l.add(&l, &v);
			if (add_status != EC_OK)
				{
					printf("ERROR: Failed to add value %d to list. Status: %d\n", v, add_status);
					TEST_ASSERT(0U); // Force failure
				}
		}

	// Creating sublist from index 1U to 4U (should contain 20U, 30U, 40U)
	ec_slist_uint_t* sub = NULL;
	ec_status_t sublist_status = l.sublist(&l, 1U, 4U, &sub);
	if (sublist_status != EC_OK)
		{
			printf("ERROR: Failed to create sublist from index 1U to 4. Status: %d\n", sublist_status);
			TEST_ASSERT(0U); // Force failure
		}

	// Verify sublist size
	size_t sublist_size = sub->size_fn(sub);
	if (sublist_size != 3U)
		{
			printf("ERROR: Sublist size incorrect. Expected 3U, got %zu\n", sublist_size);
			TEST_ASSERT(0U);
		}

	// Verify sublist content
	unsigned int expected_values[] = { 20U, 30U, 40U };
	for (size_t i = 0U; i < 3U; i++)
		{
			void* out = NULL;
			ec_status_t get_status = sub->get_at(sub, i, &out);
			if (get_status != EC_OK)
				{
					printf("ERROR: Failed to get value at sublist index %zu. Status: %d\n", i, get_status);
					TEST_ASSERT(0U);
				}
			if (*(unsigned int*)out != expected_values[i])
				{
					printf("ERROR: Sublist value mismatch at index %zu. Expected %d, got %d\n", i, expected_values[i], *(unsigned int*)out);
					TEST_ASSERT(0U);
				}
		}

	// Cleanup
	ec_status_t free_sub_status = sub->free_fn(sub);
	if (free_sub_status != EC_OK)
		{
			printf("ERROR: Failed to free sublist. Status: %d\n", free_sub_status);
			TEST_ASSERT(0U);
		}

	free(sub);

	ec_status_t free_list_status = l.free_fn(&l);
	if (free_list_status != EC_OK)
		{
			printf("ERROR: Failed to free original list. Status: %d\n", free_list_status);
			TEST_ASSERT(0U);
		}

	printf("pass: test_slist_uint_sublist\n");
}

void
test_slist_uint_copy_shallow(void* mp)
{
	stamp();
	printf("test_slist_uint_copy_shallow\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	for (unsigned int i = 1U; i <= 3U; i++)
		{
			unsigned int v = i;
			l.add(&l, &v);
		}
	ec_slist_uint_t* cp = NULL;
	ec_status_t s = l.copy_shallow(&l, &cp);
	TEST_ASSERT(s == EC_OK && cp->size == 3U);
	cp->free_fn(cp);
	free(cp);
	l.free_fn(&l);
	printf("pass: test_slist_uint_copy_shallow\n");
}

static void*
dupe_uint(void* src)
{
	int* ss = (unsigned int*)src;
	int* xx = (unsigned int*)malloc(sizeof(int));
	*xx = *ss;
	return xx;
}

void
test_slist_uint_copy_deep(void* mp)
{
	stamp();
	printf("test_slist_uint_copy_deep\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	for (unsigned int i = 1U; i <= 3U; i++)
		{
			unsigned int v = i * 10U;
			l.add(&l, &v);
		}
	ec_slist_uint_t* cp = NULL;
	ec_status_t s = l.copy_deep(&l, dupe_uint, &cp);
	TEST_ASSERT(s == EC_OK && cp->size_fn(cp) == 3U);
	cp->free_fn(cp);
	free(cp);
	l.free_fn(&l);
	printf("pass: test_slist_uint_copy_deep\n");
}

void
test_slist_uint_replace_at(void* mp)
{
	stamp();
	printf("test_slist_uint_replace_at\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 5U, b = 10U, c = 99U;
	void* out = NULL;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_status_t s = l.replace_at(&l, &c, 1U);
	TEST_ASSERT(s == EC_OK);
	l.get_at(&l, 1U, &out);
	TEST_ASSERT(*(unsigned int*)out == 99U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_replace_at\n");
}

void
test_slist_uint_contains(void* mp)
{
	stamp();
	printf("test_slist_uint_contains\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 10U, b = 20U, c = 30U;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	unsigned int x = 20U;
	size_t r = l.contains(&l, &x);
	TEST_ASSERT(r == 1U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_contains\n");
}

void
test_slist_uint_contains_value(void* mp)
{
	stamp();
	printf("test_slist_uint_contains_value\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 10U, b = 20U, c = 30U;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	unsigned int x = 30U;
	size_t r = l.contains_value(&l, &x, cmp_uint);
	TEST_ASSERT(r == 1U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_contains_value\n");
}

void
test_slist_uint_index_of(void* mp)
{
	stamp();
	printf("test_slist_uint_index_of\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	for (unsigned int i = 1U; i <= 5U; i++)
		{
			unsigned int v = i * 10U;
			l.add(&l, &v);
		}
	unsigned int x = 40U;
	size_t idx;
	ec_status_t s = l.index_of(&l, &x, cmp_uint, &idx);
	TEST_ASSERT(s == EC_OK && idx == 3U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_index_of\n");
}

void
test_slist_uint_to_array(void* mp)
{
	stamp();
	printf("test_slist_uint_to_array\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 1U, b = 2U, c = 3U;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	void** arr = NULL;
	ec_status_t s = l.to_array(&l, &arr);
	TEST_ASSERT(s == EC_OK);
	TEST_ASSERT(*(unsigned int*)arr[0U] == 1U && *(unsigned int*)arr[1U] == 2U && *(unsigned int*)arr[2U] == 3U);
	free(arr);
	l.free_fn(&l);
	printf("pass: test_slist_uint_to_array\n");
}

void
test_slist_uint_reverse(void* mp)
{
	stamp();
	printf("test_slist_uint_reverse\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 10U, b = 20U, c = 30U;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	l.reverse(&l);
	void* out;
	l.get_at(&l, 0U, &out);
	TEST_ASSERT(*(unsigned int*)out == 30U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_reverse\n");
}

void
test_slist_uint_sort(void* mp)
{
	stamp();
	printf("test_slist_uint_sort\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 30U, b = 10U, c = 20U;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	ec_status_t s = l.sort(&l, cmp_uint);
	TEST_ASSERT(s == EC_OK);
	void* out;
	l.get_at(&l, 0U, &out);
	TEST_ASSERT(*(unsigned int*)out == 10U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_sort\n");
}

void
test_slist_uint_foreach(void* mp)
{
	stamp();
	printf("test_slist_uint_foreach\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 5U, b = 6U;
	l.add(&l, &a);
	l.add(&l, &b);
	l.foreach (&l, inc_uint);
	void* out;
	l.get_at(&l, 0U, &out);
	TEST_ASSERT(*(unsigned int*)out == 6U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_foreach\n");
}

void
test_slist_uint_reduce(void* mp)
{
	stamp();
	printf("test_slist_uint_reduce\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	for (unsigned int i = 1U; i <= 4U; i++)
		{
			unsigned int v = i;
			l.add(&l, &v);
		}
	unsigned int sum = 0U;
	ec_status_t s = l.reduce(&l, add_uints, &sum);
	TEST_ASSERT(s == EC_OK && sum == 10U);
	unsigned int prod = 0U;
	s = l.reduce(&l, mul_uints, &prod);
	TEST_ASSERT(s == EC_OK && prod == 24U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_reduce\n");
}

void
test_slist_uint_filter(void* mp)
{
	stamp();
	printf("test_slist_uint_filter\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int arr[6U] = { 10U, 15U, 20U, 25U, 30U, 35U };
	for (unsigned int i = 0U; i < 6U; i++)
		l.add(&l, &arr[i]);
	ec_slist_uint_t* fl = NULL;
	ec_status_t s = l.filter(&l, is_even, &fl);
	TEST_ASSERT(s == EC_OK && fl->size_fn(fl) == 3U);
	fl->free_fn(fl);
	free(fl);
	l.free_fn(&l);
	printf("pass: test_slist_uint_filter\n");
}

void
test_slist_uint_filter_mut(void* mp)
{
	stamp();
	printf("test_slist_uint_filter_mut\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int arr[6U] = { 10U, 15U, 20U, 25U, 30U, 35U };
	for (unsigned int i = 0U; i < 6U; i++)
		l.add(&l, &arr[i]);
	ec_status_t s = l.filter_mut(&l, is_even);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 3U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_filter_mut\n");
}

void
test_slist_uint_iterators(void* mp)
{
	stamp();
	printf("test_slist_uint_iterators\n");

	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);

	// Add initial values to the list: 100U, 101U, 102U, 103U, 104U
	for (unsigned int i = 0U; i < 5U; i++)
		{
			unsigned int v = i + 100U;
			l.add(&l, &v);
		}

	ec_slist_uint_iter_t it;
	l.iter_init(&it, &l); // Initialize the iterator

	void* out;
	size_t count = 0U;

	// Iterate through the list to verify all elements are >= 100U
	while (l.iter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(*(unsigned int*)out >= 100U);
			count++;
		}
	TEST_ASSERT(count == 5U); // Ensure all 5U elements were traversed

	/**
	 * Correct Usage: `iter_remove` can only be called after `iter_next`
	 * has successfully moved the iterator to a valid node (`current != NULL`).
	 * So, we need to reinitialize the iterator and advance it before removing.
	 */

	l.iter_init(&it, &l);                         // Reinitialize iterator
	TEST_ASSERT(l.iter_next(&it, &out) == EC_OK); // Move to the first element

	// Remove the current node (which is now pointing to the first element, 100U)
	ec_status_t s = l.iter_remove(&it);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 4U); // List size should be reduced to 4U

	// Add a new element (999U) at the current iterator position
	unsigned int x = 999U;
	s = l.iter_add(&it, &x);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 5U); // List size should now be back to 5U

	// Replace the current node's value with 777U
	unsigned int y = 777U;
	s = l.iter_replace(&it, &y, NULL);
	TEST_ASSERT(s == EC_OK); // Replacement should succeed

	// Free the list after test completion
	l.free_fn(&l);

	printf("pass: test_slist_uint_iterators\n");
}

void
test_slist_uint_zip_iterators(void* mp)
{
	stamp();
	printf("test_slist_uint_zip_iterators\n");
	ec_slist_uint_t l1, l2;
	ec_slist_uint_init(&l1, mp);
	ec_slist_uint_init(&l2, mp);
	for (unsigned int i = 0U; i < 5U; i++)
		{
			unsigned int aa = i + 1U, bb = (i + 1U) * 10U;
			l1.add(&l1, &aa);
			l2.add(&l2, &bb);
		}
	ec_slist_uint_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	size_t c = 0U;
	while (l1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			unsigned int *r1 = (unsigned int*)out1, *r2 = (unsigned int*)out2;
			TEST_ASSERT((*r2) == ((*r1) * 10U));
			c++;
		}
	TEST_ASSERT(c == 5U);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_uint_zip_iterators\n");
}

// Negative test cases
void
test_slist_uint_add_at_neg(void* mp)
{
	stamp();
	printf("test_slist_uint_add_at_neg\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int x = 1U, y = 2U;
	l.add(&l, &x);
	ec_status_t s = l.add_at(&l, &y, 10U);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_uint_add_at_neg\n");
}

void
test_slist_uint_remove_neg(void* mp)
{
	stamp();
	printf("test_slist_uint_remove_neg\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 10U, b = 20U;
	l.add(&l, &a);
	l.add(&l, &b);
	unsigned int v = 99U;
	ec_status_t s = l.remove_fn(&l, &v);
	TEST_ASSERT(s == EC_ERR_NOT_FOUND);
	l.free_fn(&l);
	printf("pass: test_slist_uint_remove_neg\n");
}

void
test_slist_uint_remove_first_neg(void* mp)
{
	stamp();
	printf("test_slist_uint_remove_first_neg\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	ec_status_t s = l.remove_first(&l);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_uint_remove_first_neg\n");
}

void
test_slist_uint_remove_last_neg(void* mp)
{
	stamp();
	printf("test_slist_uint_remove_last_neg\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	ec_status_t s = l.remove_last(&l);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_uint_remove_last_neg\n");
}

void
test_slist_uint_remove_at_neg(void* mp)
{
	stamp();
	printf("test_slist_uint_remove_at_neg\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 10U;
	l.add(&l, &a);
	ec_status_t s = l.remove_at(&l, 5U);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_uint_remove_at_neg\n");
}

void
test_slist_uint_get_first_neg(void* mp)
{
	stamp();
	printf("test_slist_uint_get_first_neg\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	void* out = NULL;
	ec_status_t s = l.get_first(&l, &out);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_uint_get_first_neg\n");
}

void
test_slist_uint_get_last_neg(void* mp)
{
	stamp();
	printf("test_slist_uint_get_last_neg\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	void* out = NULL;
	ec_status_t s = l.get_last(&l, &out);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_uint_get_last_neg\n");
}

void
test_slist_uint_splice_neg(void* mp)
{
	stamp();
	printf("test_slist_uint_splice_neg\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	ec_status_t s = l.splice(&l, NULL);
	TEST_ASSERT(s == EC_ERR_INVALID_ARG);
	l.free_fn(&l);
	printf("pass: test_slist_uint_splice_neg\n");
}

void
test_slist_uint_splice_at_neg(void* mp)
{
	stamp();
	printf("test_slist_uint_splice_at_neg\n");
	ec_slist_uint_t l1, l2;
	ec_slist_uint_init(&l1, mp);
	ec_slist_uint_init(&l2, mp);
	unsigned int a = 10U;
	l1.add(&l1, &a);
	ec_status_t s = l1.splice_at(&l1, &l2, 10U);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_uint_splice_at_neg\n");
}

void
test_slist_uint_sublist_neg(void* mp)
{
	stamp();
	printf("test_slist_uint_sublist_neg\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 10U, b = 20U;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_slist_uint_t* sub = NULL;
	ec_status_t s = l.sublist(&l, 1U, 10U, &sub);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_uint_sublist_neg\n");
}

void
test_slist_uint_replace_at_neg(void* mp)
{
	stamp();
	printf("test_slist_uint_replace_at_neg\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 10U, b = 99U;
	l.add(&l, &a);
	ec_status_t s = l.replace_at(&l, &b, 10U);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_uint_replace_at_neg\n");
}

void
test_slist_uint_contains_neg(void* mp)
{
	stamp();
	printf("test_slist_uint_contains_neg\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 10U, b = 20U;
	l.add(&l, &a);
	l.add(&l, &b);
	unsigned int x = 99U;
	size_t r = l.contains(&l, &x);
	TEST_ASSERT(r == 0U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_contains_neg\n");
}

void
test_slist_uint_index_of_neg(void* mp)
{
	stamp();
	printf("test_slist_uint_index_of_neg\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	for (unsigned int i = 1U; i <= 5U; i++)
		{
			unsigned int v = i * 10U;
			l.add(&l, &v);
		}
	unsigned int x = 99U;
	size_t idx;
	ec_status_t s = l.index_of(&l, &x, cmp_uint, &idx);
	TEST_ASSERT(s == EC_ERR_NOT_FOUND);
	l.free_fn(&l);
	printf("pass: test_slist_uint_index_of_neg\n");
}

// Additional methods
void
test_slist_uint_add_first(void* mp)
{
	stamp();
	printf("test_slist_uint_add_first\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 10U, b = 20U;
	l.add(&l, &b);
	l.add_first(&l, &a);
	void* out;
	l.get_at(&l, 0U, &out);
	TEST_ASSERT(*(unsigned int*)out == 10U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_add_first\n");
}

void
test_slist_uint_add_last(void* mp)
{
	stamp();
	printf("test_slist_uint_add_last\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 10U, b = 20U;
	l.add(&l, &a);
	l.add_last(&l, &b);
	void* out;
	l.get_last(&l, &out);
	TEST_ASSERT(*(unsigned int*)out == 20U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_add_last\n");
}

void
test_slist_uint_remove_all(void* mp)
{
	stamp();
	printf("test_slist_uint_remove_all\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	for (unsigned int i = 0U; i < 5U; i++)
		{
			unsigned int v = i + 10U;
			l.add(&l, &v);
		}
	ec_status_t s = l.remove_all(&l);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 0U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_remove_all\n");
}

static void
cb_uint(void* x)
{
	(void)x;
}

void
test_slist_uint_remove_all_cb(void* mp)
{
	stamp();
	printf("test_slist_uint_remove_all_cb\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	for (unsigned int i = 0U; i < 5U; i++)
		{
			unsigned int v = i + 10U;
			l.add(&l, &v);
		}
	ec_status_t s = l.remove_all_cb(&l, cb_uint);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 0U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_remove_all_cb\n");
}

void
test_slist_uint_sort_in_place(void* mp)
{
	stamp();
	printf("test_slist_uint_sort_in_place\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	unsigned int a = 30U, b = 10U, c = 20U;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	l.sort_in_place(&l, cmp_uint);
	void* out;
	l.get_at(&l, 0U, &out);
	TEST_ASSERT(*(unsigned int*)out == 10U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_sort_in_place\n");
}

void
test_slist_uint_iter_index(void* mp)
{
	stamp();
	printf("test_slist_uint_iter_index\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	for (unsigned int i = 0U; i < 5U; i++)
		{
			unsigned int v = i + 10U;
			l.add(&l, &v);
		}
	ec_slist_uint_iter_t it;
	l.iter_init(&it, &l);
	void* out;
	size_t idx = 1U; // iter_index is 1-based after first iter_next
	while (l.iter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(l.iter_index(&it) == idx);
			idx++;
		}
	l.free_fn(&l);
	printf("pass: test_slist_uint_iter_index\n");
}

void
test_slist_uint_diter(void* mp)
{
	stamp();
	printf("test_slist_uint_diter\n");
	ec_slist_uint_t l;
	ec_slist_uint_init(&l, mp);
	for (unsigned int i = 0U; i < 5U; i++)
		{
			unsigned int v = i + 10U;
			l.add(&l, &v);
		}
	ec_slist_uint_iter_t it;
	l.diter_init(&it, &l);
	void* out;
	size_t count = 0U;
	while (l.diter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(*(unsigned int*)out >= 10U);
			count++;
		}
	TEST_ASSERT(count == 5U);
	TEST_ASSERT(l.diter_index(&it) == 5U);
	l.free_fn(&l);
	printf("pass: test_slist_uint_diter\n");
}

void
test_slist_uint_zip_iter_index(void* mp)
{
	stamp();
	printf("test_slist_uint_zip_iter_index\n");
	ec_slist_uint_t l1, l2;
	ec_slist_uint_init(&l1, mp);
	ec_slist_uint_init(&l2, mp);
	for (unsigned int i = 0U; i < 5U; i++)
		{
			unsigned int aa = i + 1U, bb = (i + 1U) * 10U;
			l1.add(&l1, &aa);
			l2.add(&l2, &bb);
		}
	ec_slist_uint_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	size_t idx = 0U;
	while (l1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			TEST_ASSERT(l1.zip_iter_index(&zit) == idx + 1U);
			idx++;
		}
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_uint_zip_iter_index\n");
}

void
test_slist_uint_zip_iter_not_supported(void* mp)
{
	stamp();
	printf("test_slist_uint_zip_iter_not_supported\n");
	ec_slist_uint_t l1, l2;
	ec_slist_uint_init(&l1, mp);
	ec_slist_uint_init(&l2, mp);
	unsigned int a = 10U, b = 20U;
	l1.add(&l1, &a);
	l2.add(&l2, &b);
	ec_slist_uint_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	l1.zip_iter_next(&zit, &out1, &out2);
	unsigned int x = 99U, y = 88U;
	void *o1, *o2;
	ec_status_t s = l1.zip_iter_add(&zit, &x, &y);
	TEST_ASSERT(s == EC_ERR_NOT_SUPPORTED);
	s = l1.zip_iter_remove(&zit, &o1, &o2);
	TEST_ASSERT(s == EC_ERR_NOT_SUPPORTED);
	s = l1.zip_iter_replace(&zit, &x, &y, &o1, &o2);
	TEST_ASSERT(s == EC_ERR_NOT_SUPPORTED);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_uint_zip_iter_not_supported\n");
}

int
main(void)
{
	printf("Starting ec_slist_uint tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64U, 100U, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8U, 0U, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_slist_uint_add(mp);
	test_slist_uint_add_at(mp);
	test_slist_uint_remove(mp);
	test_slist_uint_remove_first(mp);
	test_slist_uint_remove_last(mp);
	test_slist_uint_remove_at(mp);
	test_slist_uint_get_at(mp);
	test_slist_uint_get_first(mp);
	test_slist_uint_get_last(mp);
	test_slist_uint_splice(mp);
	test_slist_uint_splice_at(mp);
	test_slist_uint_add_all(mp);
	test_slist_uint_add_all_at(mp);
	test_slist_uint_sublist(mp);
	test_slist_uint_copy_shallow(mp);
	test_slist_uint_copy_deep(mp);
	test_slist_uint_replace_at(mp);
	test_slist_uint_contains(mp);
	test_slist_uint_contains_value(mp);
	test_slist_uint_index_of(mp);
	test_slist_uint_to_array(mp);
	test_slist_uint_reverse(mp);
	test_slist_uint_sort(mp);
	test_slist_uint_foreach(mp);
	test_slist_uint_reduce(mp);
	test_slist_uint_filter(mp);
	test_slist_uint_filter_mut(mp);
	test_slist_uint_iterators(mp);
	test_slist_uint_zip_iterators(mp);
	// Negative tests
	test_slist_uint_add_at_neg(mp);
	test_slist_uint_remove_neg(mp);
	test_slist_uint_remove_first_neg(mp);
	test_slist_uint_remove_last_neg(mp);
	test_slist_uint_remove_at_neg(mp);
	test_slist_uint_get_first_neg(mp);
	test_slist_uint_get_last_neg(mp);
	test_slist_uint_splice_neg(mp);
	test_slist_uint_splice_at_neg(mp);
	test_slist_uint_sublist_neg(mp);
	test_slist_uint_replace_at_neg(mp);
	test_slist_uint_contains_neg(mp);
	test_slist_uint_index_of_neg(mp);
	// Additional methods
	test_slist_uint_add_first(mp);
	test_slist_uint_add_last(mp);
	test_slist_uint_remove_all(mp);
	test_slist_uint_remove_all_cb(mp);
	test_slist_uint_sort_in_place(mp);
	test_slist_uint_iter_index(mp);
	test_slist_uint_diter(mp);
	test_slist_uint_zip_iter_index(mp);
	test_slist_uint_zip_iter_not_supported(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif
	printf("All ec_slist_uint tests passed.\n");
	return 0;
}
