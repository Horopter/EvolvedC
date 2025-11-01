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
	void* array[32UL];
	size_t size = backtrace(array, 32UL);
	char** symbols = backtrace_symbols(array, size);
	if (symbols)
		{
			fprintf(stderr, "--- Stack Trace (most recent call last) ---\n");
			for (size_t i = 0UL; i < size; i++)
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
  A custom assertion macro that prints stack trace on failure, then exits(3UL).
 *-----------------------------------------------------------------------------*/
#define TEST_ASSERT(cond)                                                                    \
	do                                                                                       \
		{                                                                                    \
			if (!(cond))                                                                     \
				{                                                                            \
					fprintf(stderr, "[ASSERT FAIL] %s:%d: %s\n", __FILE__, __LINE__, #cond); \
					print_stacktrace();                                                      \
					exit(3UL);                                                               \
				}                                                                            \
		}                                                                                    \
	while (0UL)

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
			char b[64UL];
			strftime(b, sizeof(b), "[%Y-%m-%d %H:%M:%S]", m);
			printf("%s ", b);
		}
	else
		{
			printf("[timeerr] ");
		}
}

static int
cmp_ul(const void* a, const void* b)
{
	return (*(unsigned long*)a - *(unsigned long*)b);
}
static bool
is_even(const void* x)
{
	return ((*(unsigned long*)x) % 2UL) == 0UL;
}
static void
inc_ul(void* x)
{
	(*(unsigned long*)x)++;
}
static void
add_uls(void* a, void* b, void* out)
{
	*(unsigned long*)out = (*(unsigned long*)a) + (*(unsigned long*)b);
}
static void
mul_uls(void* a, void* b, void* out)
{
	*(unsigned long*)out = (*(unsigned long*)a) * (*(unsigned long*)b);
}

/*-----------------------------------------------------------------------------*
  Tests
 *-----------------------------------------------------------------------------*/

void
test_slist_ul_add(void* mp)
{
	stamp();
	printf("test_slist_ul_add\n");
	ec_slist_ul_t l;
	ec_status_t s = ec_slist_ul_init(&l, mp);
	TEST_ASSERT(s == EC_OK);
	unsigned long a = 10UL, b = 20UL;
	l.add(&l, &a);
	l.add(&l, &b);
	TEST_ASSERT(l.size_fn(&l) == 2UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_add\n");
}

void
test_slist_ul_add_at(void* mp)
{
	stamp();
	printf("test_slist_ul_add_at\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long x = 1UL, y = 2UL, z = 3UL;
	l.add(&l, &x);
	l.add(&l, &z);
	ec_status_t st = l.add_at(&l, &y, 1UL);
	TEST_ASSERT(st == EC_OK);
	void* out;
	l.get_at(&l, 1UL, &out);
	TEST_ASSERT(*(unsigned long*)out == 2UL);
	TEST_ASSERT(l.size_fn(&l) == 3UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_add_at\n");
}

void
test_slist_ul_remove(void* mp)
{
	stamp();
	printf("test_slist_ul_remove\n");

	ec_slist_ul_t l;
	ec_status_t init_status = ec_slist_ul_init(&l, mp);
	TEST_ASSERT(init_status == EC_OK);

	unsigned long a = 10UL, b = 20UL, c = 30UL;
	ec_status_t add_status_a = l.add(&l, &a);
	ec_status_t add_status_b = l.add(&l, &b);
	ec_status_t add_status_c = l.add(&l, &c);

	TEST_ASSERT(add_status_a == EC_OK);
	TEST_ASSERT(add_status_b == EC_OK);
	TEST_ASSERT(add_status_c == EC_OK);

	unsigned long v = 20UL;

	ec_status_t s = l.remove_fn(&l, &v);

	// Verify status code and pointer before dereferencing
	TEST_ASSERT(s == EC_OK);
	TEST_ASSERT(l.size_fn(&l) == 2UL); // List should now have 2UL elements

	l.free_fn(&l);
	printf("pass: test_slist_ul_remove\n");
}

void
test_slist_ul_remove_first(void* mp)
{
	stamp();
	printf("test_slist_ul_remove_first\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 11UL, b = 22UL;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_status_t s = l.remove_first(&l);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 1UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_remove_first\n");
}

void
test_slist_ul_remove_last(void* mp)
{
	stamp();
	printf("test_slist_ul_remove_last\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 33UL, b = 44UL;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_status_t s = l.remove_last(&l);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 1UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_remove_last\n");
}

void
test_slist_ul_remove_at(void* mp)
{
	stamp();
	printf("test_slist_ul_remove_at\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 10UL, b = 20UL, c = 30UL;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	ec_status_t s = l.remove_at(&l, 1UL);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 2UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_remove_at\n");
}

void
test_slist_ul_get_at(void* mp)
{
	stamp();
	printf("test_slist_ul_get_at\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 100UL, b = 200UL;
	l.add(&l, &a);
	l.add(&l, &b);
	void* out = NULL;
	ec_status_t s = l.get_at(&l, 1UL, &out);
	TEST_ASSERT(s == EC_OK && *(unsigned long*)out == 200UL);
	s = l.get_at(&l, 2UL, &out);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ul_get_at\n");
}

void
test_slist_ul_get_first(void* mp)
{
	stamp();
	printf("test_slist_ul_get_first\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 777UL;
	l.add(&l, &a);
	void* out = NULL;
	ec_status_t s = l.get_first(&l, &out);
	TEST_ASSERT(s == EC_OK && *(unsigned long*)out == 777UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_get_first\n");
}

void
test_slist_ul_get_last(void* mp)
{
	stamp();
	printf("test_slist_ul_get_last\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 10UL, b = 20UL;
	l.add(&l, &a);
	l.add(&l, &b);
	void* out;
	ec_status_t s = l.get_last(&l, &out);
	TEST_ASSERT(s == EC_OK && *(unsigned long*)out == 20UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_get_last\n");
}

void
test_slist_ul_splice(void* mp)
{
	stamp();
	printf("test_slist_ul_splice\n");
	ec_slist_ul_t l1, l2;
	ec_slist_ul_init(&l1, mp);
	ec_slist_ul_init(&l2, mp);
	unsigned long a = 10UL, b = 20UL, c = 30UL, d = 40UL;
	l1.add(&l1, &a);
	l1.add(&l1, &b);
	l2.add(&l2, &c);
	l2.add(&l2, &d);
	ec_status_t s = l1.splice(&l1, &l2);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 4UL && l2.size_fn(&l2) == 0UL);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ul_splice\n");
}

void
test_slist_ul_splice_at(void* mp)
{
	stamp();
	printf("test_slist_ul_splice_at\n");
	ec_slist_ul_t l1, l2;
	ec_slist_ul_init(&l1, mp);
	ec_slist_ul_init(&l2, mp);
	unsigned long a = 1UL, b = 2UL, c = 3UL, d = 4UL, e = 5UL;
	l1.add(&l1, &a);
	l1.add(&l1, &b);
	l1.add(&l1, &c);
	l2.add(&l2, &d);
	l2.add(&l2, &e);
	ec_status_t s = l1.splice_at(&l1, &l2, 1UL);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 5UL && l2.size_fn(&l2) == 0UL);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ul_splice_at\n");
}

void
test_slist_ul_add_all(void* mp)
{
	stamp();
	printf("test_slist_ul_add_all\n");
	ec_slist_ul_t l1, l2;
	ec_slist_ul_init(&l1, mp);
	ec_slist_ul_init(&l2, mp);
	unsigned long a = 10UL, b = 20UL;
	l2.add(&l2, &a);
	l2.add(&l2, &b);
	ec_status_t s = l1.add_all(&l1, &l2);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 2UL && l2.size_fn(&l2) == 2UL);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ul_add_all\n");
}

void
test_slist_ul_add_all_at(void* mp)
{
	stamp();
	printf("test_slist_ul_add_all_at\n");
	ec_slist_ul_t l1, l2;
	ec_slist_ul_init(&l1, mp);
	ec_slist_ul_init(&l2, mp);
	unsigned long a = 10UL, b = 30UL, c = 20UL, d = 25UL;
	l1.add(&l1, &a);
	l1.add(&l1, &b);
	l2.add(&l2, &c);
	l2.add(&l2, &d);
	ec_status_t s = l1.add_all_at(&l1, &l2, 1UL);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 4UL);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ul_add_all_at\n");
}

void
test_slist_ul_sublist(void* mp)
{
	stamp();
	printf("test_slist_ul_sublist\n");

	ec_slist_ul_t l;
	ec_status_t init_status = ec_slist_ul_init(&l, mp);
	if (init_status != EC_OK)
		{
			printf("ERROR: Failed to initialize the list. Status: %d\n", init_status);
			TEST_ASSERT(0UL); // Force failure
		}

	// Adding elements: 10UL, 20UL, 30UL, 40UL, 50UL
	for (unsigned long i = 0UL; i < 5UL; i++)
		{
			unsigned long v = (i + 1UL) * 10UL;
			ec_status_t add_status = l.add(&l, &v);
			if (add_status != EC_OK)
				{
					printf("ERROR: Failed to add value %d to list. Status: %d\n", v, add_status);
					TEST_ASSERT(0UL); // Force failure
				}
		}

	// Creating sublist from index 1UL to 4UL (should contain 20UL, 30UL, 40UL)
	ec_slist_ul_t* sub = NULL;
	ec_status_t sublist_status = l.sublist(&l, 1UL, 4UL, &sub);
	if (sublist_status != EC_OK)
		{
			printf("ERROR: Failed to create sublist from index 1UL to 4. Status: %d\n", sublist_status);
			TEST_ASSERT(0UL); // Force failure
		}

	// Verify sublist size
	size_t sublist_size = sub->size_fn(sub);
	if (sublist_size != 3UL)
		{
			printf("ERROR: Sublist size incorrect. Expected 3UL, got %zu\n", sublist_size);
			TEST_ASSERT(0UL);
		}

	// Verify sublist content
	unsigned long expected_values[] = { 20UL, 30UL, 40UL };
	for (size_t i = 0UL; i < 3UL; i++)
		{
			void* out = NULL;
			ec_status_t get_status = sub->get_at(sub, i, &out);
			if (get_status != EC_OK)
				{
					printf("ERROR: Failed to get value at sublist index %zu. Status: %d\n", i, get_status);
					TEST_ASSERT(0UL);
				}
			if (*(unsigned long*)out != expected_values[i])
				{
					printf("ERROR: Sublist value mismatch at index %zu. Expected %d, got %d\n", i, expected_values[i], *(unsigned long*)out);
					TEST_ASSERT(0UL);
				}
		}

	// Cleanup
	ec_status_t free_sub_status = sub->free_fn(sub);
	if (free_sub_status != EC_OK)
		{
			printf("ERROR: Failed to free sublist. Status: %d\n", free_sub_status);
			TEST_ASSERT(0UL);
		}

	free(sub);

	ec_status_t free_list_status = l.free_fn(&l);
	if (free_list_status != EC_OK)
		{
			printf("ERROR: Failed to free original list. Status: %d\n", free_list_status);
			TEST_ASSERT(0UL);
		}

	printf("pass: test_slist_ul_sublist\n");
}

void
test_slist_ul_copy_shallow(void* mp)
{
	stamp();
	printf("test_slist_ul_copy_shallow\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	for (unsigned long i = 1UL; i <= 3UL; i++)
		{
			unsigned long v = i;
			l.add(&l, &v);
		}
	ec_slist_ul_t* cp = NULL;
	ec_status_t s = l.copy_shallow(&l, &cp);
	TEST_ASSERT(s == EC_OK && cp->size == 3UL);
	cp->free_fn(cp);
	free(cp);
	l.free_fn(&l);
	printf("pass: test_slist_ul_copy_shallow\n");
}

static void*
dupe_ul(void* src)
{
	int* ss = (unsigned long*)src;
	int* xx = (unsigned long*)malloc(sizeof(int));
	*xx = *ss;
	return xx;
}

void
test_slist_ul_copy_deep(void* mp)
{
	stamp();
	printf("test_slist_ul_copy_deep\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	for (unsigned long i = 1UL; i <= 3UL; i++)
		{
			unsigned long v = i * 10UL;
			l.add(&l, &v);
		}
	ec_slist_ul_t* cp = NULL;
	ec_status_t s = l.copy_deep(&l, dupe_ul, &cp);
	TEST_ASSERT(s == EC_OK && cp->size_fn(cp) == 3UL);
	cp->free_fn(cp);
	free(cp);
	l.free_fn(&l);
	printf("pass: test_slist_ul_copy_deep\n");
}

void
test_slist_ul_replace_at(void* mp)
{
	stamp();
	printf("test_slist_ul_replace_at\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 5UL, b = 10UL, c = 99UL;
	void* out = NULL;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_status_t s = l.replace_at(&l, &c, 1UL);
	TEST_ASSERT(s == EC_OK);
	l.get_at(&l, 1UL, &out);
	TEST_ASSERT(*(unsigned long*)out == 99UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_replace_at\n");
}

void
test_slist_ul_contains(void* mp)
{
	stamp();
	printf("test_slist_ul_contains\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 10UL, b = 20UL, c = 30UL;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	unsigned long x = 20UL;
	size_t r = l.contains(&l, &x);
	TEST_ASSERT(r == 1UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_contains\n");
}

void
test_slist_ul_contains_value(void* mp)
{
	stamp();
	printf("test_slist_ul_contains_value\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 10UL, b = 20UL, c = 30UL;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	unsigned long x = 30UL;
	size_t r = l.contains_value(&l, &x, cmp_ul);
	TEST_ASSERT(r == 1UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_contains_value\n");
}

void
test_slist_ul_index_of(void* mp)
{
	stamp();
	printf("test_slist_ul_index_of\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	for (unsigned long i = 1UL; i <= 5UL; i++)
		{
			unsigned long v = i * 10UL;
			l.add(&l, &v);
		}
	unsigned long x = 40UL;
	size_t idx;
	ec_status_t s = l.index_of(&l, &x, cmp_ul, &idx);
	TEST_ASSERT(s == EC_OK && idx == 3UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_index_of\n");
}

void
test_slist_ul_to_array(void* mp)
{
	stamp();
	printf("test_slist_ul_to_array\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 1UL, b = 2UL, c = 3UL;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	void** arr = NULL;
	ec_status_t s = l.to_array(&l, &arr);
	TEST_ASSERT(s == EC_OK);
	TEST_ASSERT(*(unsigned long*)arr[0UL] == 1UL && *(unsigned long*)arr[1UL] == 2UL && *(unsigned long*)arr[2UL] == 3UL);
	free(arr);
	l.free_fn(&l);
	printf("pass: test_slist_ul_to_array\n");
}

void
test_slist_ul_reverse(void* mp)
{
	stamp();
	printf("test_slist_ul_reverse\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 10UL, b = 20UL, c = 30UL;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	l.reverse(&l);
	void* out;
	l.get_at(&l, 0UL, &out);
	TEST_ASSERT(*(unsigned long*)out == 30UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_reverse\n");
}

void
test_slist_ul_sort(void* mp)
{
	stamp();
	printf("test_slist_ul_sort\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 30UL, b = 10UL, c = 20UL;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	ec_status_t s = l.sort(&l, cmp_ul);
	TEST_ASSERT(s == EC_OK);
	void* out;
	l.get_at(&l, 0UL, &out);
	TEST_ASSERT(*(unsigned long*)out == 10UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_sort\n");
}

void
test_slist_ul_foreach(void* mp)
{
	stamp();
	printf("test_slist_ul_foreach\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 5UL, b = 6UL;
	l.add(&l, &a);
	l.add(&l, &b);
	l.foreach (&l, inc_ul);
	void* out;
	l.get_at(&l, 0UL, &out);
	TEST_ASSERT(*(unsigned long*)out == 6UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_foreach\n");
}

void
test_slist_ul_reduce(void* mp)
{
	stamp();
	printf("test_slist_ul_reduce\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	for (unsigned long i = 1UL; i <= 4UL; i++)
		{
			unsigned long v = i;
			l.add(&l, &v);
		}
	unsigned long sum = 0UL;
	ec_status_t s = l.reduce(&l, add_uls, &sum);
	TEST_ASSERT(s == EC_OK && sum == 10UL);
	unsigned long prod = 0UL;
	s = l.reduce(&l, mul_uls, &prod);
	TEST_ASSERT(s == EC_OK && prod == 24UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_reduce\n");
}

void
test_slist_ul_filter(void* mp)
{
	stamp();
	printf("test_slist_ul_filter\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long arr[6UL] = { 10UL, 15UL, 20UL, 25UL, 30UL, 35UL };
	for (unsigned long i = 0UL; i < 6UL; i++)
		l.add(&l, &arr[i]);
	ec_slist_ul_t* fl = NULL;
	ec_status_t s = l.filter(&l, is_even, &fl);
	TEST_ASSERT(s == EC_OK && fl->size_fn(fl) == 3UL);
	fl->free_fn(fl);
	free(fl);
	l.free_fn(&l);
	printf("pass: test_slist_ul_filter\n");
}

void
test_slist_ul_filter_mut(void* mp)
{
	stamp();
	printf("test_slist_ul_filter_mut\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long arr[6UL] = { 10UL, 15UL, 20UL, 25UL, 30UL, 35UL };
	for (unsigned long i = 0UL; i < 6UL; i++)
		l.add(&l, &arr[i]);
	ec_status_t s = l.filter_mut(&l, is_even);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 3UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_filter_mut\n");
}

void
test_slist_ul_iterators(void* mp)
{
	stamp();
	printf("test_slist_ul_iterators\n");

	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);

	// Add initial values to the list: 100UL, 101UL, 102UL, 103UL, 104UL
	for (unsigned long i = 0UL; i < 5UL; i++)
		{
			unsigned long v = i + 100UL;
			l.add(&l, &v);
		}

	ec_slist_ul_iter_t it;
	l.iter_init(&it, &l); // Initialize the iterator

	void* out;
	size_t count = 0UL;

	// Iterate through the list to verify all elements are >= 100UL
	while (l.iter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(*(unsigned long*)out >= 100UL);
			count++;
		}
	TEST_ASSERT(count == 5UL); // Ensure all 5UL elements were traversed

	/**
	 * Correct Usage: `iter_remove` can only be called after `iter_next`
	 * has successfully moved the iterator to a valid node (`current != NULL`).
	 * So, we need to reinitialize the iterator and advance it before removing.
	 */

	l.iter_init(&it, &l);                         // Reinitialize iterator
	TEST_ASSERT(l.iter_next(&it, &out) == EC_OK); // Move to the first element

	// Remove the current node (which is now pointing to the first element, 100UL)
	ec_status_t s = l.iter_remove(&it);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 4UL); // List size should be reduced to 4UL

	// Add a new element (999UL) at the current iterator position
	unsigned long x = 999UL;
	s = l.iter_add(&it, &x);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 5UL); // List size should now be back to 5UL

	// Replace the current node's value with 777UL
	unsigned long y = 777UL;
	s = l.iter_replace(&it, &y, NULL);
	TEST_ASSERT(s == EC_OK); // Replacement should succeed

	// Free the list after test completion
	l.free_fn(&l);

	printf("pass: test_slist_ul_iterators\n");
}

void
test_slist_ul_zip_iterators(void* mp)
{
	stamp();
	printf("test_slist_ul_zip_iterators\n");
	ec_slist_ul_t l1, l2;
	ec_slist_ul_init(&l1, mp);
	ec_slist_ul_init(&l2, mp);
	for (unsigned long i = 0UL; i < 5UL; i++)
		{
			unsigned long aa = i + 1UL, bb = (i + 1UL) * 10UL;
			l1.add(&l1, &aa);
			l2.add(&l2, &bb);
		}
	ec_slist_ul_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	size_t c = 0UL;
	while (l1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			unsigned long *r1 = (unsigned long*)out1, *r2 = (unsigned long*)out2;
			TEST_ASSERT((*r2) == ((*r1) * 10UL));
			c++;
		}
	TEST_ASSERT(c == 5UL);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ul_zip_iterators\n");
}

// Negative test cases
void
test_slist_ul_add_at_neg(void* mp)
{
	stamp();
	printf("test_slist_ul_add_at_neg\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long x = 1UL, y = 2UL;
	l.add(&l, &x);
	ec_status_t s = l.add_at(&l, &y, 10UL);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ul_add_at_neg\n");
}

void
test_slist_ul_remove_neg(void* mp)
{
	stamp();
	printf("test_slist_ul_remove_neg\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 10UL, b = 20UL;
	l.add(&l, &a);
	l.add(&l, &b);
	unsigned long v = 99UL;
	ec_status_t s = l.remove_fn(&l, &v);
	TEST_ASSERT(s == EC_ERR_NOT_FOUND);
	l.free_fn(&l);
	printf("pass: test_slist_ul_remove_neg\n");
}

void
test_slist_ul_remove_first_neg(void* mp)
{
	stamp();
	printf("test_slist_ul_remove_first_neg\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	ec_status_t s = l.remove_first(&l);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ul_remove_first_neg\n");
}

void
test_slist_ul_remove_last_neg(void* mp)
{
	stamp();
	printf("test_slist_ul_remove_last_neg\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	ec_status_t s = l.remove_last(&l);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ul_remove_last_neg\n");
}

void
test_slist_ul_remove_at_neg(void* mp)
{
	stamp();
	printf("test_slist_ul_remove_at_neg\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 10UL;
	l.add(&l, &a);
	ec_status_t s = l.remove_at(&l, 5UL);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ul_remove_at_neg\n");
}

void
test_slist_ul_get_first_neg(void* mp)
{
	stamp();
	printf("test_slist_ul_get_first_neg\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	void* out = NULL;
	ec_status_t s = l.get_first(&l, &out);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ul_get_first_neg\n");
}

void
test_slist_ul_get_last_neg(void* mp)
{
	stamp();
	printf("test_slist_ul_get_last_neg\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	void* out = NULL;
	ec_status_t s = l.get_last(&l, &out);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ul_get_last_neg\n");
}

void
test_slist_ul_splice_neg(void* mp)
{
	stamp();
	printf("test_slist_ul_splice_neg\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	ec_status_t s = l.splice(&l, NULL);
	TEST_ASSERT(s == EC_ERR_INVALID_ARG);
	l.free_fn(&l);
	printf("pass: test_slist_ul_splice_neg\n");
}

void
test_slist_ul_splice_at_neg(void* mp)
{
	stamp();
	printf("test_slist_ul_splice_at_neg\n");
	ec_slist_ul_t l1, l2;
	ec_slist_ul_init(&l1, mp);
	ec_slist_ul_init(&l2, mp);
	unsigned long a = 10UL;
	l1.add(&l1, &a);
	ec_status_t s = l1.splice_at(&l1, &l2, 10UL);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ul_splice_at_neg\n");
}

void
test_slist_ul_sublist_neg(void* mp)
{
	stamp();
	printf("test_slist_ul_sublist_neg\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 10UL, b = 20UL;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_slist_ul_t* sub = NULL;
	ec_status_t s = l.sublist(&l, 1UL, 10UL, &sub);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ul_sublist_neg\n");
}

void
test_slist_ul_replace_at_neg(void* mp)
{
	stamp();
	printf("test_slist_ul_replace_at_neg\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 10UL, b = 99UL;
	l.add(&l, &a);
	ec_status_t s = l.replace_at(&l, &b, 10UL);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ul_replace_at_neg\n");
}

void
test_slist_ul_contains_neg(void* mp)
{
	stamp();
	printf("test_slist_ul_contains_neg\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 10UL, b = 20UL;
	l.add(&l, &a);
	l.add(&l, &b);
	unsigned long x = 99UL;
	size_t r = l.contains(&l, &x);
	TEST_ASSERT(r == 0UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_contains_neg\n");
}

void
test_slist_ul_index_of_neg(void* mp)
{
	stamp();
	printf("test_slist_ul_index_of_neg\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	for (unsigned long i = 1UL; i <= 5UL; i++)
		{
			unsigned long v = i * 10UL;
			l.add(&l, &v);
		}
	unsigned long x = 99UL;
	size_t idx;
	ec_status_t s = l.index_of(&l, &x, cmp_ul, &idx);
	TEST_ASSERT(s == EC_ERR_NOT_FOUND);
	l.free_fn(&l);
	printf("pass: test_slist_ul_index_of_neg\n");
}

// Additional methods
void
test_slist_ul_add_first(void* mp)
{
	stamp();
	printf("test_slist_ul_add_first\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 10UL, b = 20UL;
	l.add(&l, &b);
	l.add_first(&l, &a);
	void* out;
	l.get_at(&l, 0UL, &out);
	TEST_ASSERT(*(unsigned long*)out == 10UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_add_first\n");
}

void
test_slist_ul_add_last(void* mp)
{
	stamp();
	printf("test_slist_ul_add_last\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 10UL, b = 20UL;
	l.add(&l, &a);
	l.add_last(&l, &b);
	void* out;
	l.get_last(&l, &out);
	TEST_ASSERT(*(unsigned long*)out == 20UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_add_last\n");
}

void
test_slist_ul_remove_all(void* mp)
{
	stamp();
	printf("test_slist_ul_remove_all\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	for (unsigned long i = 0UL; i < 5UL; i++)
		{
			unsigned long v = i + 10UL;
			l.add(&l, &v);
		}
	ec_status_t s = l.remove_all(&l);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 0UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_remove_all\n");
}

static void
cb_ul(void* x)
{
	(void)x;
}

void
test_slist_ul_remove_all_cb(void* mp)
{
	stamp();
	printf("test_slist_ul_remove_all_cb\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	for (unsigned long i = 0UL; i < 5UL; i++)
		{
			unsigned long v = i + 10UL;
			l.add(&l, &v);
		}
	ec_status_t s = l.remove_all_cb(&l, cb_ul);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 0UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_remove_all_cb\n");
}

void
test_slist_ul_sort_in_place(void* mp)
{
	stamp();
	printf("test_slist_ul_sort_in_place\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	unsigned long a = 30UL, b = 10UL, c = 20UL;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	l.sort_in_place(&l, cmp_ul);
	void* out;
	l.get_at(&l, 0UL, &out);
	TEST_ASSERT(*(unsigned long*)out == 10UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_sort_in_place\n");
}

void
test_slist_ul_iter_index(void* mp)
{
	stamp();
	printf("test_slist_ul_iter_index\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	for (unsigned long i = 0UL; i < 5UL; i++)
		{
			unsigned long v = i + 10UL;
			l.add(&l, &v);
		}
	ec_slist_ul_iter_t it;
	l.iter_init(&it, &l);
	void* out;
	size_t idx = 0UL;
	while (l.iter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(l.iter_index(&it) == idx);
			idx++;
		}
	l.free_fn(&l);
	printf("pass: test_slist_ul_iter_index\n");
}

void
test_slist_ul_diter(void* mp)
{
	stamp();
	printf("test_slist_ul_diter\n");
	ec_slist_ul_t l;
	ec_slist_ul_init(&l, mp);
	for (unsigned long i = 0UL; i < 5UL; i++)
		{
			unsigned long v = i + 10UL;
			l.add(&l, &v);
		}
	ec_slist_ul_iter_t it;
	l.diter_init(&it, &l);
	void* out;
	size_t count = 0UL;
	while (l.diter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(*(unsigned long*)out >= 10UL);
			count++;
		}
	TEST_ASSERT(count == 5UL);
	TEST_ASSERT(l.diter_index(&it) == 5UL);
	l.free_fn(&l);
	printf("pass: test_slist_ul_diter\n");
}

void
test_slist_ul_zip_iter_index(void* mp)
{
	stamp();
	printf("test_slist_ul_zip_iter_index\n");
	ec_slist_ul_t l1, l2;
	ec_slist_ul_init(&l1, mp);
	ec_slist_ul_init(&l2, mp);
	for (unsigned long i = 0UL; i < 5UL; i++)
		{
			unsigned long aa = i + 1UL, bb = (i + 1UL) * 10UL;
			l1.add(&l1, &aa);
			l2.add(&l2, &bb);
		}
	ec_slist_ul_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	size_t idx = 0UL;
	while (l1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			TEST_ASSERT(l1.zip_iter_index(&zit) == idx + 1UL);
			idx++;
		}
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ul_zip_iter_index\n");
}

void
test_slist_ul_zip_iter_not_supported(void* mp)
{
	stamp();
	printf("test_slist_ul_zip_iter_not_supported\n");
	ec_slist_ul_t l1, l2;
	ec_slist_ul_init(&l1, mp);
	ec_slist_ul_init(&l2, mp);
	unsigned long a = 10UL, b = 20UL;
	l1.add(&l1, &a);
	l2.add(&l2, &b);
	ec_slist_ul_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	l1.zip_iter_next(&zit, &out1, &out2);
	unsigned long x = 99UL, y = 88UL;
	void *o1, *o2;
	ec_status_t s = l1.zip_iter_add(&zit, &x, &y);
	TEST_ASSERT(s == EC_ERR_NOT_SUPPORTED);
	s = l1.zip_iter_remove(&zit, &o1, &o2);
	TEST_ASSERT(s == EC_ERR_NOT_SUPPORTED);
	s = l1.zip_iter_replace(&zit, &x, &y, &o1, &o2);
	TEST_ASSERT(s == EC_ERR_NOT_SUPPORTED);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ul_zip_iter_not_supported\n");
}

int
main(void)
{
	printf("Starting ec_slist_ul tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64UL, 100UL, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8UL, 0UL, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_slist_ul_add(mp);
	test_slist_ul_add_at(mp);
	test_slist_ul_remove(mp);
	test_slist_ul_remove_first(mp);
	test_slist_ul_remove_last(mp);
	test_slist_ul_remove_at(mp);
	test_slist_ul_get_at(mp);
	test_slist_ul_get_first(mp);
	test_slist_ul_get_last(mp);
	test_slist_ul_splice(mp);
	test_slist_ul_splice_at(mp);
	test_slist_ul_add_all(mp);
	test_slist_ul_add_all_at(mp);
	test_slist_ul_sublist(mp);
	test_slist_ul_copy_shallow(mp);
	test_slist_ul_copy_deep(mp);
	test_slist_ul_replace_at(mp);
	test_slist_ul_contains(mp);
	test_slist_ul_contains_value(mp);
	test_slist_ul_index_of(mp);
	test_slist_ul_to_array(mp);
	test_slist_ul_reverse(mp);
	test_slist_ul_sort(mp);
	test_slist_ul_foreach(mp);
	test_slist_ul_reduce(mp);
	test_slist_ul_filter(mp);
	test_slist_ul_filter_mut(mp);
	test_slist_ul_iterators(mp);
	test_slist_ul_zip_iterators(mp);
	// Negative tests
	test_slist_ul_add_at_neg(mp);
	test_slist_ul_remove_neg(mp);
	test_slist_ul_remove_first_neg(mp);
	test_slist_ul_remove_last_neg(mp);
	test_slist_ul_remove_at_neg(mp);
	test_slist_ul_get_first_neg(mp);
	test_slist_ul_get_last_neg(mp);
	test_slist_ul_splice_neg(mp);
	test_slist_ul_splice_at_neg(mp);
	test_slist_ul_sublist_neg(mp);
	test_slist_ul_replace_at_neg(mp);
	test_slist_ul_contains_neg(mp);
	test_slist_ul_index_of_neg(mp);
	// Additional methods
	test_slist_ul_add_first(mp);
	test_slist_ul_add_last(mp);
	test_slist_ul_remove_all(mp);
	test_slist_ul_remove_all_cb(mp);
	test_slist_ul_sort_in_place(mp);
	test_slist_ul_iter_index(mp);
	test_slist_ul_diter(mp);
	test_slist_ul_zip_iter_index(mp);
	test_slist_ul_zip_iter_not_supported(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif
	printf("All ec_slist_ul tests passed.\n");
	return 0UL;
}
