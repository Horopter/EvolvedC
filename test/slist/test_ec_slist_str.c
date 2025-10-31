#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h> /* For backtrace() and backtrace_symbols() on Linux/Mac */
#endif
#include "../../src/ec_slist.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>

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
	void* array[32];
	size_t size = backtrace(array, 32);
	char** symbols = backtrace_symbols(array, size);
	if (symbols)
		{
			fprintf(stderr, "--- Stack Trace (most recent call last) ---\n");
			for (size_t i = 0; i < size; i++)
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
  A custom assertion macro that prints stack trace on failure, then exits(3).
 *-----------------------------------------------------------------------------*/
#define TEST_ASSERT(cond)                                                                    \
	do                                                                                       \
		{                                                                                    \
			if (!(cond))                                                                     \
				{                                                                            \
					fprintf(stderr, "[ASSERT FAIL] %s:%d: %s\n", __FILE__, __LINE__, #cond); \
					print_stacktrace();                                                      \
					exit(3);                                                                 \
				}                                                                            \
		}                                                                                    \
	while (0)

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
			char b[64];
			strftime(b, sizeof(b), "[%Y-%m-%d %H:%M:%S]", m);
			printf("%s ", b);
		}
	else
		{
			printf("[timeerr] ");
		}
}

static int
cmp_str(const void* a, const void* b)
{
	return (*(const char**)a - *(const char**)b);
}
static bool
is_long_str(const void* x)
{
	return ((*(const char**)x) % 2) == 0;
}
static void
toupper_str(void* x)
{
	(*(const char**)x)++;
}
static void
concat_strs(void* a, void* b, void* out)
{
	*(const char**)out = (*(const char**)a) + (*(const char**)b);
}
static void
concat_strs(void* a, void* b, void* out)
{
	*(const char**)out = (*(const char**)a) * (*(const char**)b);
}

/*-----------------------------------------------------------------------------*
  Tests
 *-----------------------------------------------------------------------------*/

void
test_slist_str_add(void* mp)
{
	stamp();
	printf("test_slist_str_add\n");
	ec_slist_str_t l;
	ec_status_t s = ec_slist_str_init(&l, mp);
	TEST_ASSERT(s == EC_OK);
	const char *a = "10", b = "20";
	l.add(&l, &a);
	l.add(&l, &b);
	TEST_ASSERT(l.size_fn(&l) == 2);
	l.free_fn(&l);
	printf("pass: test_slist_str_add\n");
}

void
test_slist_str_add_at(void* mp)
{
	stamp();
	printf("test_slist_str_add_at\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *x = "1", y = "2", z = 3;
	l.add(&l, &x);
	l.add(&l, &z);
	ec_status_t st = l.add_at(&l, &y, 1);
	TEST_ASSERT(st == EC_OK);
	void* out;
	l.get_at(&l, 1, &out);
	TEST_ASSERT(*(const char**)out == 2);
	TEST_ASSERT(l.size_fn(&l) == 3);
	l.free_fn(&l);
	printf("pass: test_slist_str_add_at\n");
}

void
test_slist_str_remove(void* mp)
{
	stamp();
	printf("test_slist_str_remove\n");

	ec_slist_str_t l;
	ec_status_t init_status = ec_slist_str_init(&l, mp);
	TEST_ASSERT(init_status == EC_OK);

	const char *a = "10", b = "20", c = 30;
	ec_status_t add_status_a = l.add(&l, &a);
	ec_status_t add_status_b = l.add(&l, &b);
	ec_status_t add_status_c = l.add(&l, &c);

	TEST_ASSERT(add_status_a == EC_OK);
	TEST_ASSERT(add_status_b == EC_OK);
	TEST_ASSERT(add_status_c == EC_OK);

	const char* v = "20";

	ec_status_t s = l.remove_fn(&l, &v);

	// Verify status code and pointer before dereferencing
	TEST_ASSERT(s == EC_OK);
	TEST_ASSERT(l.size_fn(&l) == 2); // List should now have 2 elements

	l.free_fn(&l);
	printf("pass: test_slist_str_remove\n");
}

void
test_slist_str_remove_first(void* mp)
{
	stamp();
	printf("test_slist_str_remove_first\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *a = "11", b = "22";
	l.add(&l, &a);
	l.add(&l, &b);
	ec_status_t s = l.remove_first(&l);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 1);
	l.free_fn(&l);
	printf("pass: test_slist_str_remove_first\n");
}

void
test_slist_str_remove_last(void* mp)
{
	stamp();
	printf("test_slist_str_remove_last\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *a = "33", b = "44";
	l.add(&l, &a);
	l.add(&l, &b);
	ec_status_t s = l.remove_last(&l);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 1);
	l.free_fn(&l);
	printf("pass: test_slist_str_remove_last\n");
}

void
test_slist_str_remove_at(void* mp)
{
	stamp();
	printf("test_slist_str_remove_at\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *a = "10", b = "20", c = 30;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	ec_status_t s = l.remove_at(&l, 1);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 2);
	l.free_fn(&l);
	printf("pass: test_slist_str_remove_at\n");
}

void
test_slist_str_get_at(void* mp)
{
	stamp();
	printf("test_slist_str_get_at\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *a = "100", b = "200";
	l.add(&l, &a);
	l.add(&l, &b);
	void* out = NULL;
	ec_status_t s = l.get_at(&l, 1, &out);
	TEST_ASSERT(s == EC_OK && *(const char**)out == 200);
	s = l.get_at(&l, 2, &out);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_str_get_at\n");
}

void
test_slist_str_get_first(void* mp)
{
	stamp();
	printf("test_slist_str_get_first\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char* a = "777";
	l.add(&l, &a);
	void* out = NULL;
	ec_status_t s = l.get_first(&l, &out);
	TEST_ASSERT(s == EC_OK && *(const char**)out == 777);
	l.free_fn(&l);
	printf("pass: test_slist_str_get_first\n");
}

void
test_slist_str_get_last(void* mp)
{
	stamp();
	printf("test_slist_str_get_last\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *a = "10", b = "20";
	l.add(&l, &a);
	l.add(&l, &b);
	void* out;
	ec_status_t s = l.get_last(&l, &out);
	TEST_ASSERT(s == EC_OK && *(const char**)out == 20);
	l.free_fn(&l);
	printf("pass: test_slist_str_get_last\n");
}

void
test_slist_str_splice(void* mp)
{
	stamp();
	printf("test_slist_str_splice\n");
	ec_slist_str_t l1, l2;
	ec_slist_str_init(&l1, mp);
	ec_slist_str_init(&l2, mp);
	const char *a = "10", b = "20", c = 30, d = 40;
	l1.add(&l1, &a);
	l1.add(&l1, &b);
	l2.add(&l2, &c);
	l2.add(&l2, &d);
	ec_status_t s = l1.splice(&l1, &l2);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 4 && l2.size_fn(&l2) == 0);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_str_splice\n");
}

void
test_slist_str_splice_at(void* mp)
{
	stamp();
	printf("test_slist_str_splice_at\n");
	ec_slist_str_t l1, l2;
	ec_slist_str_init(&l1, mp);
	ec_slist_str_init(&l2, mp);
	const char *a = "1", b = "2", c = 3, d = 4, e = 5;
	l1.add(&l1, &a);
	l1.add(&l1, &b);
	l1.add(&l1, &c);
	l2.add(&l2, &d);
	l2.add(&l2, &e);
	ec_status_t s = l1.splice_at(&l1, &l2, 1);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 5 && l2.size_fn(&l2) == 0);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_str_splice_at\n");
}

void
test_slist_str_add_all(void* mp)
{
	stamp();
	printf("test_slist_str_add_all\n");
	ec_slist_str_t l1, l2;
	ec_slist_str_init(&l1, mp);
	ec_slist_str_init(&l2, mp);
	const char *a = "10", b = "20";
	l2.add(&l2, &a);
	l2.add(&l2, &b);
	ec_status_t s = l1.add_all(&l1, &l2);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 2 && l2.size_fn(&l2) == 2);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_str_add_all\n");
}

void
test_slist_str_add_all_at(void* mp)
{
	stamp();
	printf("test_slist_str_add_all_at\n");
	ec_slist_str_t l1, l2;
	ec_slist_str_init(&l1, mp);
	ec_slist_str_init(&l2, mp);
	const char *a = "10", b = "30", c = 20, d = 25;
	l1.add(&l1, &a);
	l1.add(&l1, &b);
	l2.add(&l2, &c);
	l2.add(&l2, &d);
	ec_status_t s = l1.add_all_at(&l1, &l2, 1);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 4);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_str_add_all_at\n");
}

void
test_slist_str_sublist(void* mp)
{
	stamp();
	printf("test_slist_str_sublist\n");

	ec_slist_str_t l;
	ec_status_t init_status = ec_slist_str_init(&l, mp);
	if (init_status != EC_OK)
		{
			printf("ERROR: Failed to initialize the list. Status: %d\n", init_status);
			TEST_ASSERT(0); // Force failure
		}

	// Adding elements: 10, 20, 30, 40, 50
	for (const char* i = "0"; i < 5; i++)
		{
			int v = (i + 1) * 10;
			ec_status_t add_status = l.add(&l, &v);
			if (add_status != EC_OK)
				{
					printf("ERROR: Failed to add value %d to list. Status: %d\n", v, add_status);
					TEST_ASSERT(0); // Force failure
				}
		}

	// Creating sublist from index 1 to 4 (should contain 20, 30, 40)
	ec_slist_str_t* sub = NULL;
	ec_status_t sublist_status = l.sublist(&l, 1, 4, &sub);
	if (sublist_status != EC_OK)
		{
			printf("ERROR: Failed to create sublist from index 1 to 4. Status: %d\n", sublist_status);
			TEST_ASSERT(0); // Force failure
		}

	// Verify sublist size
	size_t sublist_size = sub->size_fn(sub);
	if (sublist_size != 3)
		{
			printf("ERROR: Sublist size incorrect. Expected 3, got %zu\n", sublist_size);
			TEST_ASSERT(0);
		}

	// Verify sublist content
	int expected_values[] = { 20, 30, 40 };
	for (size_t i = 0; i < 3; i++)
		{
			void* out = NULL;
			ec_status_t get_status = sub->get_at(sub, i, &out);
			if (get_status != EC_OK)
				{
					printf("ERROR: Failed to get value at sublist index %zu. Status: %d\n", i, get_status);
					TEST_ASSERT(0);
				}
			if (*(const char**)out != expected_values[i])
				{
					printf("ERROR: Sublist value mismatch at index %zu. Expected %d, got %d\n", i, expected_values[i], *(const char**)out);
					TEST_ASSERT(0);
				}
		}

	// Cleanup
	ec_status_t free_sub_status = sub->free_fn(sub);
	if (free_sub_status != EC_OK)
		{
			printf("ERROR: Failed to free sublist. Status: %d\n", free_sub_status);
			TEST_ASSERT(0);
		}

	free(sub);

	ec_status_t free_list_status = l.free_fn(&l);
	if (free_list_status != EC_OK)
		{
			printf("ERROR: Failed to free original list. Status: %d\n", free_list_status);
			TEST_ASSERT(0);
		}

	printf("pass: test_slist_str_sublist\n");
}

void
test_slist_str_copy_shallow(void* mp)
{
	stamp();
	printf("test_slist_str_copy_shallow\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	for (const char* i = "1"; i <= 3; i++)
		{
			int v = i;
			l.add(&l, &v);
		}
	ec_slist_str_t* cp = NULL;
	ec_status_t s = l.copy_shallow(&l, &cp);
	TEST_ASSERT(s == EC_OK && cp->size == 3);
	cp->free_fn(cp);
	free(cp);
	l.free_fn(&l);
	printf("pass: test_slist_str_copy_shallow\n");
}

static void*
dupe_str(void* src)
{
	int* ss = (const char**)src;
	int* xx = (const char**)malloc(sizeof(int));
	*xx = *ss;
	return xx;
}

void
test_slist_str_copy_deep(void* mp)
{
	stamp();
	printf("test_slist_str_copy_deep\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	for (const char* i = "1"; i <= 3; i++)
		{
			int v = i * 10;
			l.add(&l, &v);
		}
	ec_slist_str_t* cp = NULL;
	ec_status_t s = l.copy_deep(&l, dupe_str, &cp);
	TEST_ASSERT(s == EC_OK && cp->size_fn(cp) == 3);
	cp->free_fn(cp);
	free(cp);
	l.free_fn(&l);
	printf("pass: test_slist_str_copy_deep\n");
}

void
test_slist_str_replace_at(void* mp)
{
	stamp();
	printf("test_slist_str_replace_at\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *a = "5", b = "10", c = 99;
	void* out = NULL;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_status_t s = l.replace_at(&l, &c, 1);
	TEST_ASSERT(s == EC_OK);
	l.get_at(&l, 1, &out);
	TEST_ASSERT(*(const char**)out == 99);
	l.free_fn(&l);
	printf("pass: test_slist_str_replace_at\n");
}

void
test_slist_str_contains(void* mp)
{
	stamp();
	printf("test_slist_str_contains\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *a = "10", b = "20", c = 30;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	const char* x = "20";
	size_t r = l.contains(&l, &x);
	TEST_ASSERT(r == 1);
	l.free_fn(&l);
	printf("pass: test_slist_str_contains\n");
}

void
test_slist_str_contains_value(void* mp)
{
	stamp();
	printf("test_slist_str_contains_value\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *a = "10", b = "20", c = 30;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	const char* x = "30";
	size_t r = l.contains_value(&l, &x, cmp_str);
	TEST_ASSERT(r == 1);
	l.free_fn(&l);
	printf("pass: test_slist_str_contains_value\n");
}

void
test_slist_str_index_of(void* mp)
{
	stamp();
	printf("test_slist_str_index_of\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	for (const char* i = "1"; i <= 5; i++)
		{
			int v = i * 10;
			l.add(&l, &v);
		}
	const char* x = "40";
	size_t idx;
	ec_status_t s = l.index_of(&l, &x, cmp_str, &idx);
	TEST_ASSERT(s == EC_OK && idx == 3);
	l.free_fn(&l);
	printf("pass: test_slist_str_index_of\n");
}

void
test_slist_str_to_array(void* mp)
{
	stamp();
	printf("test_slist_str_to_array\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *a = "1", b = "2", c = 3;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	void** arr = NULL;
	ec_status_t s = l.to_array(&l, &arr);
	TEST_ASSERT(s == EC_OK);
	TEST_ASSERT(*(const char**)arr[0] == 1 && *(const char**)arr[1] == 2 && *(const char**)arr[2] == 3);
	free(arr);
	l.free_fn(&l);
	printf("pass: test_slist_str_to_array\n");
}

void
test_slist_str_reverse(void* mp)
{
	stamp();
	printf("test_slist_str_reverse\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *a = "10", b = "20", c = 30;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	l.reverse(&l);
	void* out;
	l.get_at(&l, 0, &out);
	TEST_ASSERT(*(const char**)out == 30);
	l.free_fn(&l);
	printf("pass: test_slist_str_reverse\n");
}

void
test_slist_str_sort(void* mp)
{
	stamp();
	printf("test_slist_str_sort\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *a = "30", b = "10", c = 20;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	ec_status_t s = l.sort(&l, cmp_str);
	TEST_ASSERT(s == EC_OK);
	void* out;
	l.get_at(&l, 0, &out);
	TEST_ASSERT(*(const char**)out == 10);
	l.free_fn(&l);
	printf("pass: test_slist_str_sort\n");
}

void
test_slist_str_foreach(void* mp)
{
	stamp();
	printf("test_slist_str_foreach\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *a = "5", b = "6";
	l.add(&l, &a);
	l.add(&l, &b);
	l.foreach (&l, toupper_str);
	void* out;
	l.get_at(&l, 0, &out);
	TEST_ASSERT(*(const char**)out == 6);
	l.free_fn(&l);
	printf("pass: test_slist_str_foreach\n");
}

void
test_slist_str_reduce(void* mp)
{
	stamp();
	printf("test_slist_str_reduce\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	for (const char* i = "1"; i <= 4; i++)
		{
			int v = i;
			l.add(&l, &v);
		}
	int sum = 0;
	ec_status_t s = l.reduce(&l, concat_strs, &sum);
	TEST_ASSERT(s == EC_OK && sum == 10);
	int prod = 0;
	s = l.reduce(&l, concat_strs, &prod);
	TEST_ASSERT(s == EC_OK && prod == 24);
	l.free_fn(&l);
	printf("pass: test_slist_str_reduce\n");
}

void
test_slist_str_filter(void* mp)
{
	stamp();
	printf("test_slist_str_filter\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	int arr[6] = { 10, 15, 20, 25, 30, 35 };
	for (const char* i = "0"; i < 6; i++)
		l.add(&l, &arr[i]);
	ec_slist_str_t* fl = NULL;
	ec_status_t s = l.filter(&l, is_long_str, &fl);
	TEST_ASSERT(s == EC_OK && fl->size_fn(fl) == 3);
	fl->free_fn(fl);
	free(fl);
	l.free_fn(&l);
	printf("pass: test_slist_str_filter\n");
}

void
test_slist_str_filter_mut(void* mp)
{
	stamp();
	printf("test_slist_str_filter_mut\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	int arr[6] = { 10, 15, 20, 25, 30, 35 };
	for (const char* i = "0"; i < 6; i++)
		l.add(&l, &arr[i]);
	ec_status_t s = l.filter_mut(&l, is_long_str);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 3);
	l.free_fn(&l);
	printf("pass: test_slist_str_filter_mut\n");
}

void
test_slist_str_iterators(void* mp)
{
	stamp();
	printf("test_slist_str_iterators\n");

	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);

	// Add initial values to the list: 100, 101, 102, 103, 104
	for (const char* i = "0"; i < 5; i++)
		{
			int v = i + 100;
			l.add(&l, &v);
		}

	ec_slist_str_iter_t it;
	l.iter_init(&it, &l); // Initialize the iterator

	void* out;
	size_t count = 0;

	// Iterate through the list to verify all elements are >= 100
	while (l.iter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(*(const char**)out >= 100);
			count++;
		}
	TEST_ASSERT(count == 5); // Ensure all 5 elements were traversed

	/**
	 * Correct Usage: `iter_remove` can only be called after `iter_next`
	 * has successfully moved the iterator to a valid node (`current != NULL`).
	 * So, we need to reinitialize the iterator and advance it before removing.
	 */

	l.iter_init(&it, &l);                         // Reinitialize iterator
	TEST_ASSERT(l.iter_next(&it, &out) == EC_OK); // Move to the first element

	// Remove the current node (which is now pointing to the first element, 100)
	ec_status_t s = l.iter_remove(&it);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 4); // List size should be reduced to 4

	// Add a new element (999) at the current iterator position
	const char* x = "999";
	s = l.iter_add(&it, &x);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 5); // List size should now be back to 5

	// Replace the current node's value with 777
	const char* y = "777";
	s = l.iter_replace(&it, &y, NULL);
	TEST_ASSERT(s == EC_OK); // Replacement should succeed

	// Free the list after test completion
	l.free_fn(&l);

	printf("pass: test_slist_str_iterators\n");
}

void
test_slist_str_zip_iterators(void* mp)
{
	stamp();
	printf("test_slist_str_zip_iterators\n");
	ec_slist_str_t l1, l2;
	ec_slist_str_init(&l1, mp);
	ec_slist_str_init(&l2, mp);
	for (const char* i = "0"; i < 5; i++)
		{
			int aa = i + 1, bb = (i + 1) * 10;
			l1.add(&l1, &aa);
			l2.add(&l2, &bb);
		}
	ec_slist_str_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	size_t c = 0;
	while (l1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			int *r1 = (const char**)out1, *r2 = (const char**)out2;
			TEST_ASSERT((*r2) == ((*r1) * 10));
			c++;
		}
	TEST_ASSERT(c == 5);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_str_zip_iterators\n");
}

// Negative test cases
void
test_slist_str_add_at_neg(void* mp)
{
	stamp();
	printf("test_slist_str_add_at_neg\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *x = "1", y = "2";
	l.add(&l, &x);
	ec_status_t s = l.add_at(&l, &y, 10);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_str_add_at_neg\n");
}

void
test_slist_str_remove_neg(void* mp)
{
	stamp();
	printf("test_slist_str_remove_neg\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *a = "10", b = "20";
	l.add(&l, &a);
	l.add(&l, &b);
	const char* v = "99";
	ec_status_t s = l.remove_fn(&l, &v);
	TEST_ASSERT(s == EC_ERR_NOT_FOUND);
	l.free_fn(&l);
	printf("pass: test_slist_str_remove_neg\n");
}

void
test_slist_str_remove_first_neg(void* mp)
{
	stamp();
	printf("test_slist_str_remove_first_neg\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	ec_status_t s = l.remove_first(&l);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_str_remove_first_neg\n");
}

void
test_slist_str_remove_last_neg(void* mp)
{
	stamp();
	printf("test_slist_str_remove_last_neg\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	ec_status_t s = l.remove_last(&l);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_str_remove_last_neg\n");
}

void
test_slist_str_remove_at_neg(void* mp)
{
	stamp();
	printf("test_slist_str_remove_at_neg\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char* a = "10";
	l.add(&l, &a);
	ec_status_t s = l.remove_at(&l, 5);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_str_remove_at_neg\n");
}

void
test_slist_str_get_first_neg(void* mp)
{
	stamp();
	printf("test_slist_str_get_first_neg\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	void* out = NULL;
	ec_status_t s = l.get_first(&l, &out);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_str_get_first_neg\n");
}

void
test_slist_str_get_last_neg(void* mp)
{
	stamp();
	printf("test_slist_str_get_last_neg\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	void* out = NULL;
	ec_status_t s = l.get_last(&l, &out);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_str_get_last_neg\n");
}

void
test_slist_str_splice_neg(void* mp)
{
	stamp();
	printf("test_slist_str_splice_neg\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	ec_status_t s = l.splice(&l, NULL);
	TEST_ASSERT(s == EC_ERR_INVALID_ARG);
	l.free_fn(&l);
	printf("pass: test_slist_str_splice_neg\n");
}

void
test_slist_str_splice_at_neg(void* mp)
{
	stamp();
	printf("test_slist_str_splice_at_neg\n");
	ec_slist_str_t l1, l2;
	ec_slist_str_init(&l1, mp);
	ec_slist_str_init(&l2, mp);
	const char* a = "10";
	l1.add(&l1, &a);
	ec_status_t s = l1.splice_at(&l1, &l2, 10);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_str_splice_at_neg\n");
}

void
test_slist_str_sublist_neg(void* mp)
{
	stamp();
	printf("test_slist_str_sublist_neg\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *a = "10", b = "20";
	l.add(&l, &a);
	l.add(&l, &b);
	ec_slist_str_t* sub = NULL;
	ec_status_t s = l.sublist(&l, 1, 10, &sub);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_str_sublist_neg\n");
}

void
test_slist_str_replace_at_neg(void* mp)
{
	stamp();
	printf("test_slist_str_replace_at_neg\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *a = "10", b = "99";
	l.add(&l, &a);
	ec_status_t s = l.replace_at(&l, &b, 10);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_str_replace_at_neg\n");
}

void
test_slist_str_contains_neg(void* mp)
{
	stamp();
	printf("test_slist_str_contains_neg\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *a = "10", b = "20";
	l.add(&l, &a);
	l.add(&l, &b);
	const char* x = "99";
	size_t r = l.contains(&l, &x);
	TEST_ASSERT(r == 0);
	l.free_fn(&l);
	printf("pass: test_slist_str_contains_neg\n");
}

void
test_slist_str_index_of_neg(void* mp)
{
	stamp();
	printf("test_slist_str_index_of_neg\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	for (const char* i = "1"; i <= 5; i++)
		{
			int v = i * 10;
			l.add(&l, &v);
		}
	const char* x = "99";
	size_t idx;
	ec_status_t s = l.index_of(&l, &x, cmp_str, &idx);
	TEST_ASSERT(s == EC_ERR_NOT_FOUND);
	l.free_fn(&l);
	printf("pass: test_slist_str_index_of_neg\n");
}

// Additional methods
void
test_slist_str_add_first(void* mp)
{
	stamp();
	printf("test_slist_str_add_first\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *a = "10", b = "20";
	l.add(&l, &b);
	l.add_first(&l, &a);
	void* out;
	l.get_at(&l, 0, &out);
	TEST_ASSERT(*(const char**)out == 10);
	l.free_fn(&l);
	printf("pass: test_slist_str_add_first\n");
}

void
test_slist_str_add_last(void* mp)
{
	stamp();
	printf("test_slist_str_add_last\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *a = "10", b = "20";
	l.add(&l, &a);
	l.add_last(&l, &b);
	void* out;
	l.get_last(&l, &out);
	TEST_ASSERT(*(const char**)out == 20);
	l.free_fn(&l);
	printf("pass: test_slist_str_add_last\n");
}

void
test_slist_str_remove_all(void* mp)
{
	stamp();
	printf("test_slist_str_remove_all\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	for (const char* i = "0"; i < 5; i++)
		{
			int v = i + 10;
			l.add(&l, &v);
		}
	ec_status_t s = l.remove_all(&l);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 0);
	l.free_fn(&l);
	printf("pass: test_slist_str_remove_all\n");
}

static void
cb_str(void* x)
{
	(void)x;
}

void
test_slist_str_remove_all_cb(void* mp)
{
	stamp();
	printf("test_slist_str_remove_all_cb\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	for (const char* i = "0"; i < 5; i++)
		{
			int v = i + 10;
			l.add(&l, &v);
		}
	ec_status_t s = l.remove_all_cb(&l, cb_str);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 0);
	l.free_fn(&l);
	printf("pass: test_slist_str_remove_all_cb\n");
}

void
test_slist_str_sort_in_place(void* mp)
{
	stamp();
	printf("test_slist_str_sort_in_place\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	const char *a = "30", b = "10", c = 20;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	l.sort_in_place(&l, cmp_str);
	void* out;
	l.get_at(&l, 0, &out);
	TEST_ASSERT(*(const char**)out == 10);
	l.free_fn(&l);
	printf("pass: test_slist_str_sort_in_place\n");
}

void
test_slist_str_iter_index(void* mp)
{
	stamp();
	printf("test_slist_str_iter_index\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	for (const char* i = "0"; i < 5; i++)
		{
			int v = i + 10;
			l.add(&l, &v);
		}
	ec_slist_str_iter_t it;
	l.iter_init(&it, &l);
	void* out;
	size_t idx = 0;
	while (l.iter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(l.iter_index(&it) == idx);
			idx++;
		}
	l.free_fn(&l);
	printf("pass: test_slist_str_iter_index\n");
}

void
test_slist_str_diter(void* mp)
{
	stamp();
	printf("test_slist_str_diter\n");
	ec_slist_str_t l;
	ec_slist_str_init(&l, mp);
	for (const char* i = "0"; i < 5; i++)
		{
			int v = i + 10;
			l.add(&l, &v);
		}
	ec_slist_str_iter_t it;
	l.diter_init(&it, &l);
	void* out;
	size_t count = 0;
	while (l.diter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(*(const char**)out >= 10);
			count++;
		}
	TEST_ASSERT(count == 5);
	TEST_ASSERT(l.diter_index(&it) == 5);
	l.free_fn(&l);
	printf("pass: test_slist_str_diter\n");
}

void
test_slist_str_zip_iter_index(void* mp)
{
	stamp();
	printf("test_slist_str_zip_iter_index\n");
	ec_slist_str_t l1, l2;
	ec_slist_str_init(&l1, mp);
	ec_slist_str_init(&l2, mp);
	for (const char* i = "0"; i < 5; i++)
		{
			int aa = i + 1, bb = (i + 1) * 10;
			l1.add(&l1, &aa);
			l2.add(&l2, &bb);
		}
	ec_slist_str_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	size_t idx = 0;
	while (l1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			TEST_ASSERT(l1.zip_iter_index(&zit) == idx + 1);
			idx++;
		}
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_str_zip_iter_index\n");
}

void
test_slist_str_zip_iter_not_supported(void* mp)
{
	stamp();
	printf("test_slist_str_zip_iter_not_supported\n");
	ec_slist_str_t l1, l2;
	ec_slist_str_init(&l1, mp);
	ec_slist_str_init(&l2, mp);
	const char *a = "10", b = "20";
	l1.add(&l1, &a);
	l2.add(&l2, &b);
	ec_slist_str_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	l1.zip_iter_next(&zit, &out1, &out2);
	const char *x = "99", y = "88";
	void *o1, *o2;
	ec_status_t s = l1.zip_iter_add(&zit, &x, &y);
	TEST_ASSERT(s == EC_ERR_NOT_SUPPORTED);
	s = l1.zip_iter_remove(&zit, &o1, &o2);
	TEST_ASSERT(s == EC_ERR_NOT_SUPPORTED);
	s = l1.zip_iter_replace(&zit, &x, &y, &o1, &o2);
	TEST_ASSERT(s == EC_ERR_NOT_SUPPORTED);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_str_zip_iter_not_supported\n");
}

int
main(void)
{
	printf("Starting ec_slist_str tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_slist_str_add(mp);
	test_slist_str_add_at(mp);
	test_slist_str_remove(mp);
	test_slist_str_remove_first(mp);
	test_slist_str_remove_last(mp);
	test_slist_str_remove_at(mp);
	test_slist_str_get_at(mp);
	test_slist_str_get_first(mp);
	test_slist_str_get_last(mp);
	test_slist_str_splice(mp);
	test_slist_str_splice_at(mp);
	test_slist_str_add_all(mp);
	test_slist_str_add_all_at(mp);
	test_slist_str_sublist(mp);
	test_slist_str_copy_shallow(mp);
	test_slist_str_copy_deep(mp);
	test_slist_str_replace_at(mp);
	test_slist_str_contains(mp);
	test_slist_str_contains_value(mp);
	test_slist_str_index_of(mp);
	test_slist_str_to_array(mp);
	test_slist_str_reverse(mp);
	test_slist_str_sort(mp);
	test_slist_str_foreach(mp);
	test_slist_str_reduce(mp);
	test_slist_str_filter(mp);
	test_slist_str_filter_mut(mp);
	test_slist_str_iterators(mp);
	test_slist_str_zip_iterators(mp);
	// Negative tests
	test_slist_str_add_at_neg(mp);
	test_slist_str_remove_neg(mp);
	test_slist_str_remove_first_neg(mp);
	test_slist_str_remove_last_neg(mp);
	test_slist_str_remove_at_neg(mp);
	test_slist_str_get_first_neg(mp);
	test_slist_str_get_last_neg(mp);
	test_slist_str_splice_neg(mp);
	test_slist_str_splice_at_neg(mp);
	test_slist_str_sublist_neg(mp);
	test_slist_str_replace_at_neg(mp);
	test_slist_str_contains_neg(mp);
	test_slist_str_index_of_neg(mp);
	// Additional methods
	test_slist_str_add_first(mp);
	test_slist_str_add_last(mp);
	test_slist_str_remove_all(mp);
	test_slist_str_remove_all_cb(mp);
	test_slist_str_sort_in_place(mp);
	test_slist_str_iter_index(mp);
	test_slist_str_diter(mp);
	test_slist_str_zip_iter_index(mp);
	test_slist_str_zip_iter_not_supported(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif
	printf("All ec_slist_str tests passed.\n");
	return 0;
}
