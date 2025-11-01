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
	void* array[32L];
	size_t size = backtrace(array, 32L);
	char** symbols = backtrace_symbols(array, size);
	if (symbols)
		{
			fprintf(stderr, "--- Stack Trace (most recent call last) ---\n");
			for (size_t i = 0L; i < size; i++)
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
  A custom assertion macro that prints stack trace on failure, then exits(3L).
 *-----------------------------------------------------------------------------*/
#define TEST_ASSERT(cond)                                                                    \
	do                                                                                       \
		{                                                                                    \
			if (!(cond))                                                                     \
				{                                                                            \
					fprintf(stderr, "[ASSERT FAIL] %s:%d: %s\n", __FILE__, __LINE__, #cond); \
					print_stacktrace();                                                      \
					exit(3L);                                                                \
				}                                                                            \
		}                                                                                    \
	while (0L)

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
			char b[64L];
			strftime(b, sizeof(b), "[%Y-%m-%d %H:%M:%S]", m);
			printf("%s ", b);
		}
	else
		{
			printf("[timeerr] ");
		}
}

static int
cmp_ll(const void* a, const void* b)
{
	return (*(long long*)a - *(long long*)b);
}
static bool
is_even(const void* x)
{
	return ((*(long long*)x) % 2L) == 0L;
}
static void
inc_ll(void* x)
{
	(*(long long*)x)++;
}
static void
add_lls(void* a, void* b, void* out)
{
	*(long long*)out = (*(long long*)a) + (*(long long*)b);
}
static void
mul_lls(void* a, void* b, void* out)
{
	*(long long*)out = (*(long long*)a) * (*(long long*)b);
}

/*-----------------------------------------------------------------------------*
  Tests
 *-----------------------------------------------------------------------------*/

void
test_slist_ll_add(void* mp)
{
	stamp();
	printf("test_slist_ll_add\n");
	ec_slist_ll_t l;
	ec_status_t s = ec_slist_ll_init(&l, mp);
	TEST_ASSERT(s == EC_OK);
	long long a = 10L, b = 20L;
	l.add(&l, &a);
	l.add(&l, &b);
	TEST_ASSERT(l.size_fn(&l) == 2L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_add\n");
}

void
test_slist_ll_add_at(void* mp)
{
	stamp();
	printf("test_slist_ll_add_at\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long x = 1L, y = 2L, z = 3L;
	l.add(&l, &x);
	l.add(&l, &z);
	ec_status_t st = l.add_at(&l, &y, 1L);
	TEST_ASSERT(st == EC_OK);
	void* out;
	l.get_at(&l, 1L, &out);
	TEST_ASSERT(*(long long*)out == 2L);
	TEST_ASSERT(l.size_fn(&l) == 3L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_add_at\n");
}

void
test_slist_ll_remove(void* mp)
{
	stamp();
	printf("test_slist_ll_remove\n");

	ec_slist_ll_t l;
	ec_status_t init_status = ec_slist_ll_init(&l, mp);
	TEST_ASSERT(init_status == EC_OK);

	long long a = 10L, b = 20L, c = 30L;
	ec_status_t add_status_a = l.add(&l, &a);
	ec_status_t add_status_b = l.add(&l, &b);
	ec_status_t add_status_c = l.add(&l, &c);

	TEST_ASSERT(add_status_a == EC_OK);
	TEST_ASSERT(add_status_b == EC_OK);
	TEST_ASSERT(add_status_c == EC_OK);

	long long v = 20L;

	ec_status_t s = l.remove_fn(&l, &v);

	// Verify status code and pointer before dereferencing
	TEST_ASSERT(s == EC_OK);
	TEST_ASSERT(l.size_fn(&l) == 2L); // List should now have 2L elements

	l.free_fn(&l);
	printf("pass: test_slist_ll_remove\n");
}

void
test_slist_ll_remove_first(void* mp)
{
	stamp();
	printf("test_slist_ll_remove_first\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 11L, b = 22L;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_status_t s = l.remove_first(&l);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 1L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_remove_first\n");
}

void
test_slist_ll_remove_last(void* mp)
{
	stamp();
	printf("test_slist_ll_remove_last\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 33L, b = 44L;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_status_t s = l.remove_last(&l);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 1L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_remove_last\n");
}

void
test_slist_ll_remove_at(void* mp)
{
	stamp();
	printf("test_slist_ll_remove_at\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 10L, b = 20L, c = 30L;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	ec_status_t s = l.remove_at(&l, 1L);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 2L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_remove_at\n");
}

void
test_slist_ll_get_at(void* mp)
{
	stamp();
	printf("test_slist_ll_get_at\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 100L, b = 200L;
	l.add(&l, &a);
	l.add(&l, &b);
	void* out = NULL;
	ec_status_t s = l.get_at(&l, 1L, &out);
	TEST_ASSERT(s == EC_OK && *(long long*)out == 200L);
	s = l.get_at(&l, 2L, &out);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ll_get_at\n");
}

void
test_slist_ll_get_first(void* mp)
{
	stamp();
	printf("test_slist_ll_get_first\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 777L;
	l.add(&l, &a);
	void* out = NULL;
	ec_status_t s = l.get_first(&l, &out);
	TEST_ASSERT(s == EC_OK && *(long long*)out == 777L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_get_first\n");
}

void
test_slist_ll_get_last(void* mp)
{
	stamp();
	printf("test_slist_ll_get_last\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 10L, b = 20L;
	l.add(&l, &a);
	l.add(&l, &b);
	void* out;
	ec_status_t s = l.get_last(&l, &out);
	TEST_ASSERT(s == EC_OK && *(long long*)out == 20L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_get_last\n");
}

void
test_slist_ll_splice(void* mp)
{
	stamp();
	printf("test_slist_ll_splice\n");
	ec_slist_ll_t l1, l2;
	ec_slist_ll_init(&l1, mp);
	ec_slist_ll_init(&l2, mp);
	long long a = 10L, b = 20L, c = 30L, d = 40L;
	l1.add(&l1, &a);
	l1.add(&l1, &b);
	l2.add(&l2, &c);
	l2.add(&l2, &d);
	ec_status_t s = l1.splice(&l1, &l2);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 4L && l2.size_fn(&l2) == 0L);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ll_splice\n");
}

void
test_slist_ll_splice_at(void* mp)
{
	stamp();
	printf("test_slist_ll_splice_at\n");
	ec_slist_ll_t l1, l2;
	ec_slist_ll_init(&l1, mp);
	ec_slist_ll_init(&l2, mp);
	long long a = 1L, b = 2L, c = 3L, d = 4L, e = 5L;
	l1.add(&l1, &a);
	l1.add(&l1, &b);
	l1.add(&l1, &c);
	l2.add(&l2, &d);
	l2.add(&l2, &e);
	ec_status_t s = l1.splice_at(&l1, &l2, 1L);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 5L && l2.size_fn(&l2) == 0L);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ll_splice_at\n");
}

void
test_slist_ll_add_all(void* mp)
{
	stamp();
	printf("test_slist_ll_add_all\n");
	ec_slist_ll_t l1, l2;
	ec_slist_ll_init(&l1, mp);
	ec_slist_ll_init(&l2, mp);
	long long a = 10L, b = 20L;
	l2.add(&l2, &a);
	l2.add(&l2, &b);
	ec_status_t s = l1.add_all(&l1, &l2);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 2L && l2.size_fn(&l2) == 2L);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ll_add_all\n");
}

void
test_slist_ll_add_all_at(void* mp)
{
	stamp();
	printf("test_slist_ll_add_all_at\n");
	ec_slist_ll_t l1, l2;
	ec_slist_ll_init(&l1, mp);
	ec_slist_ll_init(&l2, mp);
	long long a = 10L, b = 30L, c = 20L, d = 25L;
	l1.add(&l1, &a);
	l1.add(&l1, &b);
	l2.add(&l2, &c);
	l2.add(&l2, &d);
	ec_status_t s = l1.add_all_at(&l1, &l2, 1L);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 4L);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ll_add_all_at\n");
}

void
test_slist_ll_sublist(void* mp)
{
	stamp();
	printf("test_slist_ll_sublist\n");

	ec_slist_ll_t l;
	ec_status_t init_status = ec_slist_ll_init(&l, mp);
	if (init_status != EC_OK)
		{
			printf("ERROR: Failed to initialize the list. Status: %d\n", init_status);
			TEST_ASSERT(0L); // Force failure
		}

	// Adding elements: 10L, 20L, 30L, 40L, 50L
	for (long long i = 0L; i < 5L; i++)
		{
			long long v = (i + 1L) * 10L;
			ec_status_t add_status = l.add(&l, &v);
			if (add_status != EC_OK)
				{
					printf("ERROR: Failed to add value %d to list. Status: %d\n", v, add_status);
					TEST_ASSERT(0L); // Force failure
				}
		}

	// Creating sublist from index 1L to 4L (should contain 20L, 30L, 40L)
	ec_slist_ll_t* sub = NULL;
	ec_status_t sublist_status = l.sublist(&l, 1L, 4L, &sub);
	if (sublist_status != EC_OK)
		{
			printf("ERROR: Failed to create sublist from index 1L to 4. Status: %d\n", sublist_status);
			TEST_ASSERT(0L); // Force failure
		}

	// Verify sublist size
	size_t sublist_size = sub->size_fn(sub);
	if (sublist_size != 3L)
		{
			printf("ERROR: Sublist size incorrect. Expected 3L, got %zu\n", sublist_size);
			TEST_ASSERT(0L);
		}

	// Verify sublist content
	long long expected_values[] = { 20L, 30L, 40L };
	for (size_t i = 0L; i < 3L; i++)
		{
			void* out = NULL;
			ec_status_t get_status = sub->get_at(sub, i, &out);
			if (get_status != EC_OK)
				{
					printf("ERROR: Failed to get value at sublist index %zu. Status: %d\n", i, get_status);
					TEST_ASSERT(0L);
				}
			if (*(long long*)out != expected_values[i])
				{
					printf("ERROR: Sublist value mismatch at index %zu. Expected %d, got %d\n", i, expected_values[i], *(long long*)out);
					TEST_ASSERT(0L);
				}
		}

	// Cleanup
	ec_status_t free_sub_status = sub->free_fn(sub);
	if (free_sub_status != EC_OK)
		{
			printf("ERROR: Failed to free sublist. Status: %d\n", free_sub_status);
			TEST_ASSERT(0L);
		}

	free(sub);

	ec_status_t free_list_status = l.free_fn(&l);
	if (free_list_status != EC_OK)
		{
			printf("ERROR: Failed to free original list. Status: %d\n", free_list_status);
			TEST_ASSERT(0L);
		}

	printf("pass: test_slist_ll_sublist\n");
}

void
test_slist_ll_copy_shallow(void* mp)
{
	stamp();
	printf("test_slist_ll_copy_shallow\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	for (long long i = 1L; i <= 3L; i++)
		{
			long long v = i;
			l.add(&l, &v);
		}
	ec_slist_ll_t* cp = NULL;
	ec_status_t s = l.copy_shallow(&l, &cp);
	TEST_ASSERT(s == EC_OK && cp->size == 3L);
	cp->free_fn(cp);
	free(cp);
	l.free_fn(&l);
	printf("pass: test_slist_ll_copy_shallow\n");
}

static void*
dupe_ll(void* src)
{
	int* ss = (long long*)src;
	int* xx = (long long*)malloc(sizeof(int));
	*xx = *ss;
	return xx;
}

void
test_slist_ll_copy_deep(void* mp)
{
	stamp();
	printf("test_slist_ll_copy_deep\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	for (long long i = 1L; i <= 3L; i++)
		{
			long long v = i * 10L;
			l.add(&l, &v);
		}
	ec_slist_ll_t* cp = NULL;
	ec_status_t s = l.copy_deep(&l, dupe_ll, &cp);
	TEST_ASSERT(s == EC_OK && cp->size_fn(cp) == 3L);
	cp->free_fn(cp);
	free(cp);
	l.free_fn(&l);
	printf("pass: test_slist_ll_copy_deep\n");
}

void
test_slist_ll_replace_at(void* mp)
{
	stamp();
	printf("test_slist_ll_replace_at\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 5L, b = 10L, c = 99L;
	void* out = NULL;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_status_t s = l.replace_at(&l, &c, 1L);
	TEST_ASSERT(s == EC_OK);
	l.get_at(&l, 1L, &out);
	TEST_ASSERT(*(long long*)out == 99L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_replace_at\n");
}

void
test_slist_ll_contains(void* mp)
{
	stamp();
	printf("test_slist_ll_contains\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 10L, b = 20L, c = 30L;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	long long x = 20L;
	size_t r = l.contains(&l, &x);
	TEST_ASSERT(r == 1L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_contains\n");
}

void
test_slist_ll_contains_value(void* mp)
{
	stamp();
	printf("test_slist_ll_contains_value\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 10L, b = 20L, c = 30L;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	long long x = 30L;
	size_t r = l.contains_value(&l, &x, cmp_ll);
	TEST_ASSERT(r == 1L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_contains_value\n");
}

void
test_slist_ll_index_of(void* mp)
{
	stamp();
	printf("test_slist_ll_index_of\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	for (long long i = 1L; i <= 5L; i++)
		{
			long long v = i * 10L;
			l.add(&l, &v);
		}
	long long x = 40L;
	size_t idx;
	ec_status_t s = l.index_of(&l, &x, cmp_ll, &idx);
	TEST_ASSERT(s == EC_OK && idx == 3L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_index_of\n");
}

void
test_slist_ll_to_array(void* mp)
{
	stamp();
	printf("test_slist_ll_to_array\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 1L, b = 2L, c = 3L;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	void** arr = NULL;
	ec_status_t s = l.to_array(&l, &arr);
	TEST_ASSERT(s == EC_OK);
	TEST_ASSERT(*(long long*)arr[0L] == 1L && *(long long*)arr[1L] == 2L && *(long long*)arr[2L] == 3L);
	free(arr);
	l.free_fn(&l);
	printf("pass: test_slist_ll_to_array\n");
}

void
test_slist_ll_reverse(void* mp)
{
	stamp();
	printf("test_slist_ll_reverse\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 10L, b = 20L, c = 30L;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	l.reverse(&l);
	void* out;
	l.get_at(&l, 0L, &out);
	TEST_ASSERT(*(long long*)out == 30L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_reverse\n");
}

void
test_slist_ll_sort(void* mp)
{
	stamp();
	printf("test_slist_ll_sort\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 30L, b = 10L, c = 20L;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	ec_status_t s = l.sort(&l, cmp_ll);
	TEST_ASSERT(s == EC_OK);
	void* out;
	l.get_at(&l, 0L, &out);
	TEST_ASSERT(*(long long*)out == 10L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_sort\n");
}

void
test_slist_ll_foreach(void* mp)
{
	stamp();
	printf("test_slist_ll_foreach\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 5L, b = 6L;
	l.add(&l, &a);
	l.add(&l, &b);
	l.foreach (&l, inc_ll);
	void* out;
	l.get_at(&l, 0L, &out);
	TEST_ASSERT(*(long long*)out == 6L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_foreach\n");
}

void
test_slist_ll_reduce(void* mp)
{
	stamp();
	printf("test_slist_ll_reduce\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	for (long long i = 1L; i <= 4L; i++)
		{
			long long v = i;
			l.add(&l, &v);
		}
	long long sum = 0L;
	ec_status_t s = l.reduce(&l, add_lls, &sum);
	TEST_ASSERT(s == EC_OK && sum == 10L);
	long long prod = 0L;
	s = l.reduce(&l, mul_lls, &prod);
	TEST_ASSERT(s == EC_OK && prod == 24L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_reduce\n");
}

void
test_slist_ll_filter(void* mp)
{
	stamp();
	printf("test_slist_ll_filter\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long arr[6L] = { 10L, 15L, 20L, 25L, 30L, 35L };
	for (long long i = 0L; i < 6L; i++)
		l.add(&l, &arr[i]);
	ec_slist_ll_t* fl = NULL;
	ec_status_t s = l.filter(&l, is_even, &fl);
	TEST_ASSERT(s == EC_OK && fl->size_fn(fl) == 3L);
	fl->free_fn(fl);
	free(fl);
	l.free_fn(&l);
	printf("pass: test_slist_ll_filter\n");
}

void
test_slist_ll_filter_mut(void* mp)
{
	stamp();
	printf("test_slist_ll_filter_mut\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long arr[6L] = { 10L, 15L, 20L, 25L, 30L, 35L };
	for (long long i = 0L; i < 6L; i++)
		l.add(&l, &arr[i]);
	ec_status_t s = l.filter_mut(&l, is_even);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 3L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_filter_mut\n");
}

void
test_slist_ll_iterators(void* mp)
{
	stamp();
	printf("test_slist_ll_iterators\n");

	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);

	// Add initial values to the list: 100L, 101L, 102L, 103L, 104L
	for (long long i = 0L; i < 5L; i++)
		{
			long long v = i + 100L;
			l.add(&l, &v);
		}

	ec_slist_ll_iter_t it;
	l.iter_init(&it, &l); // Initialize the iterator

	void* out;
	size_t count = 0L;

	// Iterate through the list to verify all elements are >= 100L
	while (l.iter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(*(long long*)out >= 100L);
			count++;
		}
	TEST_ASSERT(count == 5L); // Ensure all 5L elements were traversed

	/**
	 * Correct Usage: `iter_remove` can only be called after `iter_next`
	 * has successfully moved the iterator to a valid node (`current != NULL`).
	 * So, we need to reinitialize the iterator and advance it before removing.
	 */

	l.iter_init(&it, &l);                         // Reinitialize iterator
	TEST_ASSERT(l.iter_next(&it, &out) == EC_OK); // Move to the first element

	// Remove the current node (which is now pointing to the first element, 100L)
	ec_status_t s = l.iter_remove(&it);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 4L); // List size should be reduced to 4L

	// Add a new element (999L) at the current iterator position
	long long x = 999L;
	s = l.iter_add(&it, &x);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 5L); // List size should now be back to 5L

	// Replace the current node's value with 777L
	long long y = 777L;
	s = l.iter_replace(&it, &y, NULL);
	TEST_ASSERT(s == EC_OK); // Replacement should succeed

	// Free the list after test completion
	l.free_fn(&l);

	printf("pass: test_slist_ll_iterators\n");
}

void
test_slist_ll_zip_iterators(void* mp)
{
	stamp();
	printf("test_slist_ll_zip_iterators\n");
	ec_slist_ll_t l1, l2;
	ec_slist_ll_init(&l1, mp);
	ec_slist_ll_init(&l2, mp);
	for (long long i = 0L; i < 5L; i++)
		{
			long long aa = i + 1L, bb = (i + 1L) * 10L;
			l1.add(&l1, &aa);
			l2.add(&l2, &bb);
		}
	ec_slist_ll_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	size_t c = 0L;
	while (l1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			long long *r1 = (long long*)out1, *r2 = (long long*)out2;
			TEST_ASSERT((*r2) == ((*r1) * 10L));
			c++;
		}
	TEST_ASSERT(c == 5L);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ll_zip_iterators\n");
}

// Negative test cases
void
test_slist_ll_add_at_neg(void* mp)
{
	stamp();
	printf("test_slist_ll_add_at_neg\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long x = 1L, y = 2L;
	l.add(&l, &x);
	ec_status_t s = l.add_at(&l, &y, 10L);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ll_add_at_neg\n");
}

void
test_slist_ll_remove_neg(void* mp)
{
	stamp();
	printf("test_slist_ll_remove_neg\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 10L, b = 20L;
	l.add(&l, &a);
	l.add(&l, &b);
	long long v = 99L;
	ec_status_t s = l.remove_fn(&l, &v);
	TEST_ASSERT(s == EC_ERR_NOT_FOUND);
	l.free_fn(&l);
	printf("pass: test_slist_ll_remove_neg\n");
}

void
test_slist_ll_remove_first_neg(void* mp)
{
	stamp();
	printf("test_slist_ll_remove_first_neg\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	ec_status_t s = l.remove_first(&l);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ll_remove_first_neg\n");
}

void
test_slist_ll_remove_last_neg(void* mp)
{
	stamp();
	printf("test_slist_ll_remove_last_neg\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	ec_status_t s = l.remove_last(&l);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ll_remove_last_neg\n");
}

void
test_slist_ll_remove_at_neg(void* mp)
{
	stamp();
	printf("test_slist_ll_remove_at_neg\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 10L;
	l.add(&l, &a);
	ec_status_t s = l.remove_at(&l, 5L);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ll_remove_at_neg\n");
}

void
test_slist_ll_get_first_neg(void* mp)
{
	stamp();
	printf("test_slist_ll_get_first_neg\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	void* out = NULL;
	ec_status_t s = l.get_first(&l, &out);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ll_get_first_neg\n");
}

void
test_slist_ll_get_last_neg(void* mp)
{
	stamp();
	printf("test_slist_ll_get_last_neg\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	void* out = NULL;
	ec_status_t s = l.get_last(&l, &out);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ll_get_last_neg\n");
}

void
test_slist_ll_splice_neg(void* mp)
{
	stamp();
	printf("test_slist_ll_splice_neg\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	ec_status_t s = l.splice(&l, NULL);
	TEST_ASSERT(s == EC_ERR_INVALID_ARG);
	l.free_fn(&l);
	printf("pass: test_slist_ll_splice_neg\n");
}

void
test_slist_ll_splice_at_neg(void* mp)
{
	stamp();
	printf("test_slist_ll_splice_at_neg\n");
	ec_slist_ll_t l1, l2;
	ec_slist_ll_init(&l1, mp);
	ec_slist_ll_init(&l2, mp);
	long long a = 10L;
	l1.add(&l1, &a);
	ec_status_t s = l1.splice_at(&l1, &l2, 10L);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ll_splice_at_neg\n");
}

void
test_slist_ll_sublist_neg(void* mp)
{
	stamp();
	printf("test_slist_ll_sublist_neg\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 10L, b = 20L;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_slist_ll_t* sub = NULL;
	ec_status_t s = l.sublist(&l, 1L, 10L, &sub);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ll_sublist_neg\n");
}

void
test_slist_ll_replace_at_neg(void* mp)
{
	stamp();
	printf("test_slist_ll_replace_at_neg\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 10L, b = 99L;
	l.add(&l, &a);
	ec_status_t s = l.replace_at(&l, &b, 10L);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ll_replace_at_neg\n");
}

void
test_slist_ll_contains_neg(void* mp)
{
	stamp();
	printf("test_slist_ll_contains_neg\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 10L, b = 20L;
	l.add(&l, &a);
	l.add(&l, &b);
	long long x = 99L;
	size_t r = l.contains(&l, &x);
	TEST_ASSERT(r == 0L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_contains_neg\n");
}

void
test_slist_ll_index_of_neg(void* mp)
{
	stamp();
	printf("test_slist_ll_index_of_neg\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	for (long long i = 1L; i <= 5L; i++)
		{
			long long v = i * 10L;
			l.add(&l, &v);
		}
	long long x = 99L;
	size_t idx;
	ec_status_t s = l.index_of(&l, &x, cmp_ll, &idx);
	TEST_ASSERT(s == EC_ERR_NOT_FOUND);
	l.free_fn(&l);
	printf("pass: test_slist_ll_index_of_neg\n");
}

// Additional methods
void
test_slist_ll_add_first(void* mp)
{
	stamp();
	printf("test_slist_ll_add_first\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 10L, b = 20L;
	l.add(&l, &b);
	l.add_first(&l, &a);
	void* out;
	l.get_at(&l, 0L, &out);
	TEST_ASSERT(*(long long*)out == 10L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_add_first\n");
}

void
test_slist_ll_add_last(void* mp)
{
	stamp();
	printf("test_slist_ll_add_last\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 10L, b = 20L;
	l.add(&l, &a);
	l.add_last(&l, &b);
	void* out;
	l.get_last(&l, &out);
	TEST_ASSERT(*(long long*)out == 20L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_add_last\n");
}

void
test_slist_ll_remove_all(void* mp)
{
	stamp();
	printf("test_slist_ll_remove_all\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	for (long long i = 0L; i < 5L; i++)
		{
			long long v = i + 10L;
			l.add(&l, &v);
		}
	ec_status_t s = l.remove_all(&l);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 0L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_remove_all\n");
}

static void
cb_ll(void* x)
{
	(void)x;
}

void
test_slist_ll_remove_all_cb(void* mp)
{
	stamp();
	printf("test_slist_ll_remove_all_cb\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	for (long long i = 0L; i < 5L; i++)
		{
			long long v = i + 10L;
			l.add(&l, &v);
		}
	ec_status_t s = l.remove_all_cb(&l, cb_ll);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 0L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_remove_all_cb\n");
}

void
test_slist_ll_sort_in_place(void* mp)
{
	stamp();
	printf("test_slist_ll_sort_in_place\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	long long a = 30L, b = 10L, c = 20L;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	l.sort_in_place(&l, cmp_ll);
	void* out;
	l.get_at(&l, 0L, &out);
	TEST_ASSERT(*(long long*)out == 10L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_sort_in_place\n");
}

void
test_slist_ll_iter_index(void* mp)
{
	stamp();
	printf("test_slist_ll_iter_index\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	for (long long i = 0L; i < 5L; i++)
		{
			long long v = i + 10L;
			l.add(&l, &v);
		}
	ec_slist_ll_iter_t it;
	l.iter_init(&it, &l);
	void* out;
	size_t idx = 1;  // iter_index is 1-based after first iter_next
	while (l.iter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(l.iter_index(&it) == idx);
			idx++;
		}
	l.free_fn(&l);
	printf("pass: test_slist_ll_iter_index\n");
}

void
test_slist_ll_diter(void* mp)
{
	stamp();
	printf("test_slist_ll_diter\n");
	ec_slist_ll_t l;
	ec_slist_ll_init(&l, mp);
	for (long long i = 0L; i < 5L; i++)
		{
			long long v = i + 10L;
			l.add(&l, &v);
		}
	ec_slist_ll_iter_t it;
	l.diter_init(&it, &l);
	void* out;
	size_t count = 0L;
	while (l.diter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(*(long long*)out >= 10L);
			count++;
		}
	TEST_ASSERT(count == 5L);
	TEST_ASSERT(l.diter_index(&it) == 5L);
	l.free_fn(&l);
	printf("pass: test_slist_ll_diter\n");
}

void
test_slist_ll_zip_iter_index(void* mp)
{
	stamp();
	printf("test_slist_ll_zip_iter_index\n");
	ec_slist_ll_t l1, l2;
	ec_slist_ll_init(&l1, mp);
	ec_slist_ll_init(&l2, mp);
	for (long long i = 0L; i < 5L; i++)
		{
			long long aa = i + 1L, bb = (i + 1L) * 10L;
			l1.add(&l1, &aa);
			l2.add(&l2, &bb);
		}
	ec_slist_ll_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	size_t idx = 0L;
	while (l1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			TEST_ASSERT(l1.zip_iter_index(&zit) == idx + 1L);
			idx++;
		}
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ll_zip_iter_index\n");
}

void
test_slist_ll_zip_iter_not_supported(void* mp)
{
	stamp();
	printf("test_slist_ll_zip_iter_not_supported\n");
	ec_slist_ll_t l1, l2;
	ec_slist_ll_init(&l1, mp);
	ec_slist_ll_init(&l2, mp);
	long long a = 10L, b = 20L;
	l1.add(&l1, &a);
	l2.add(&l2, &b);
	ec_slist_ll_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	l1.zip_iter_next(&zit, &out1, &out2);
	long long x = 99L, y = 88L;
	void *o1, *o2;
	ec_status_t s = l1.zip_iter_add(&zit, &x, &y);
	TEST_ASSERT(s == EC_ERR_NOT_SUPPORTED);
	s = l1.zip_iter_remove(&zit, &o1, &o2);
	TEST_ASSERT(s == EC_ERR_NOT_SUPPORTED);
	s = l1.zip_iter_replace(&zit, &x, &y, &o1, &o2);
	TEST_ASSERT(s == EC_ERR_NOT_SUPPORTED);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ll_zip_iter_not_supported\n");
}

int
main(void)
{
	printf("Starting ec_slist_ll tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64L, 100L, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8L, 0L, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_slist_ll_add(mp);
	test_slist_ll_add_at(mp);
	test_slist_ll_remove(mp);
	test_slist_ll_remove_first(mp);
	test_slist_ll_remove_last(mp);
	test_slist_ll_remove_at(mp);
	test_slist_ll_get_at(mp);
	test_slist_ll_get_first(mp);
	test_slist_ll_get_last(mp);
	test_slist_ll_splice(mp);
	test_slist_ll_splice_at(mp);
	test_slist_ll_add_all(mp);
	test_slist_ll_add_all_at(mp);
	test_slist_ll_sublist(mp);
	test_slist_ll_copy_shallow(mp);
	test_slist_ll_copy_deep(mp);
	test_slist_ll_replace_at(mp);
	test_slist_ll_contains(mp);
	test_slist_ll_contains_value(mp);
	test_slist_ll_index_of(mp);
	test_slist_ll_to_array(mp);
	test_slist_ll_reverse(mp);
	test_slist_ll_sort(mp);
	test_slist_ll_foreach(mp);
	test_slist_ll_reduce(mp);
	test_slist_ll_filter(mp);
	test_slist_ll_filter_mut(mp);
	test_slist_ll_iterators(mp);
	test_slist_ll_zip_iterators(mp);
	// Negative tests
	test_slist_ll_add_at_neg(mp);
	test_slist_ll_remove_neg(mp);
	test_slist_ll_remove_first_neg(mp);
	test_slist_ll_remove_last_neg(mp);
	test_slist_ll_remove_at_neg(mp);
	test_slist_ll_get_first_neg(mp);
	test_slist_ll_get_last_neg(mp);
	test_slist_ll_splice_neg(mp);
	test_slist_ll_splice_at_neg(mp);
	test_slist_ll_sublist_neg(mp);
	test_slist_ll_replace_at_neg(mp);
	test_slist_ll_contains_neg(mp);
	test_slist_ll_index_of_neg(mp);
	// Additional methods
	test_slist_ll_add_first(mp);
	test_slist_ll_add_last(mp);
	test_slist_ll_remove_all(mp);
	test_slist_ll_remove_all_cb(mp);
	test_slist_ll_sort_in_place(mp);
	test_slist_ll_iter_index(mp);
	test_slist_ll_diter(mp);
	test_slist_ll_zip_iter_index(mp);
	test_slist_ll_zip_iter_not_supported(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif
	printf("All ec_slist_ll tests passed.\n");
	return 0L;
}
