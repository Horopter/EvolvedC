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
	void* array[32ULL];
	size_t size = backtrace(array, 32ULL);
	char** symbols = backtrace_symbols(array, size);
	if (symbols)
		{
			fprintf(stderr, "--- Stack Trace (most recent call last) ---\n");
			for (size_t i = 0ULL; i < size; i++)
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
  A custom assertion macro that prints stack trace on failure, then exits(3ULL).
 *-----------------------------------------------------------------------------*/
#define TEST_ASSERT(cond)                                                                    \
	do                                                                                       \
		{                                                                                    \
			if (!(cond))                                                                     \
				{                                                                            \
					fprintf(stderr, "[ASSERT FAIL] %s:%d: %s\n", __FILE__, __LINE__, #cond); \
					print_stacktrace();                                                      \
					exit(3ULL);                                                              \
				}                                                                            \
		}                                                                                    \
	while (0ULL)

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
			char b[64ULL];
			strftime(b, sizeof(b), "[%Y-%m-%d %H:%M:%S]", m);
			printf("%s ", b);
		}
	else
		{
			printf("[timeerr] ");
		}
}

static int
cmp_ull(const void* a, const void* b)
{
	return (*(unsigned long long*)a - *(unsigned long long*)b);
}
static bool
is_even(const void* x)
{
	return ((*(unsigned long long*)x) % 2ULL) == 0ULL;
}
static void
inc_ull(void* x)
{
	(*(unsigned long long*)x)++;
}
static void
add_ulls(void* a, void* b, void* out)
{
	*(unsigned long long*)out = (*(unsigned long long*)a) + (*(unsigned long long*)b);
}
static void
mul_ulls(void* a, void* b, void* out)
{
	*(unsigned long long*)out = (*(unsigned long long*)a) * (*(unsigned long long*)b);
}

/*-----------------------------------------------------------------------------*
  Tests
 *-----------------------------------------------------------------------------*/

void
test_slist_ull_add(void* mp)
{
	stamp();
	printf("test_slist_ull_add\n");
	ec_slist_ull_t l;
	ec_status_t s = ec_slist_ull_init(&l, mp);
	TEST_ASSERT(s == EC_OK);
	unsigned long long a = 10ULL, b = 20ULL;
	l.add(&l, &a);
	l.add(&l, &b);
	TEST_ASSERT(l.size_fn(&l) == 2ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_add\n");
}

void
test_slist_ull_add_at(void* mp)
{
	stamp();
	printf("test_slist_ull_add_at\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long x = 1ULL, y = 2ULL, z = 3ULL;
	l.add(&l, &x);
	l.add(&l, &z);
	ec_status_t st = l.add_at(&l, &y, 1ULL);
	TEST_ASSERT(st == EC_OK);
	void* out;
	l.get_at(&l, 1ULL, &out);
	TEST_ASSERT(*(unsigned long long*)out == 2ULL);
	TEST_ASSERT(l.size_fn(&l) == 3ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_add_at\n");
}

void
test_slist_ull_remove(void* mp)
{
	stamp();
	printf("test_slist_ull_remove\n");

	ec_slist_ull_t l;
	ec_status_t init_status = ec_slist_ull_init(&l, mp);
	TEST_ASSERT(init_status == EC_OK);

	unsigned long long a = 10ULL, b = 20ULL, c = 30ULL;
	ec_status_t add_status_a = l.add(&l, &a);
	ec_status_t add_status_b = l.add(&l, &b);
	ec_status_t add_status_c = l.add(&l, &c);

	TEST_ASSERT(add_status_a == EC_OK);
	TEST_ASSERT(add_status_b == EC_OK);
	TEST_ASSERT(add_status_c == EC_OK);

	unsigned long long v = 20ULL;

	ec_status_t s = l.remove_fn(&l, &v);

	// Verify status code and pointer before dereferencing
	TEST_ASSERT(s == EC_OK);
	TEST_ASSERT(l.size_fn(&l) == 2ULL); // List should now have 2ULL elements

	l.free_fn(&l);
	printf("pass: test_slist_ull_remove\n");
}

void
test_slist_ull_remove_first(void* mp)
{
	stamp();
	printf("test_slist_ull_remove_first\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 11ULL, b = 22ULL;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_status_t s = l.remove_first(&l);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 1ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_remove_first\n");
}

void
test_slist_ull_remove_last(void* mp)
{
	stamp();
	printf("test_slist_ull_remove_last\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 33ULL, b = 44ULL;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_status_t s = l.remove_last(&l);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 1ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_remove_last\n");
}

void
test_slist_ull_remove_at(void* mp)
{
	stamp();
	printf("test_slist_ull_remove_at\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 10ULL, b = 20ULL, c = 30ULL;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	ec_status_t s = l.remove_at(&l, 1ULL);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 2ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_remove_at\n");
}

void
test_slist_ull_get_at(void* mp)
{
	stamp();
	printf("test_slist_ull_get_at\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 100ULL, b = 200ULL;
	l.add(&l, &a);
	l.add(&l, &b);
	void* out = NULL;
	ec_status_t s = l.get_at(&l, 1ULL, &out);
	TEST_ASSERT(s == EC_OK && *(unsigned long long*)out == 200ULL);
	s = l.get_at(&l, 2ULL, &out);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ull_get_at\n");
}

void
test_slist_ull_get_first(void* mp)
{
	stamp();
	printf("test_slist_ull_get_first\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 777ULL;
	l.add(&l, &a);
	void* out = NULL;
	ec_status_t s = l.get_first(&l, &out);
	TEST_ASSERT(s == EC_OK && *(unsigned long long*)out == 777ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_get_first\n");
}

void
test_slist_ull_get_last(void* mp)
{
	stamp();
	printf("test_slist_ull_get_last\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 10ULL, b = 20ULL;
	l.add(&l, &a);
	l.add(&l, &b);
	void* out;
	ec_status_t s = l.get_last(&l, &out);
	TEST_ASSERT(s == EC_OK && *(unsigned long long*)out == 20ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_get_last\n");
}

void
test_slist_ull_splice(void* mp)
{
	stamp();
	printf("test_slist_ull_splice\n");
	ec_slist_ull_t l1, l2;
	ec_slist_ull_init(&l1, mp);
	ec_slist_ull_init(&l2, mp);
	unsigned long long a = 10ULL, b = 20ULL, c = 30ULL, d = 40ULL;
	l1.add(&l1, &a);
	l1.add(&l1, &b);
	l2.add(&l2, &c);
	l2.add(&l2, &d);
	ec_status_t s = l1.splice(&l1, &l2);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 4ULL && l2.size_fn(&l2) == 0ULL);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ull_splice\n");
}

void
test_slist_ull_splice_at(void* mp)
{
	stamp();
	printf("test_slist_ull_splice_at\n");
	ec_slist_ull_t l1, l2;
	ec_slist_ull_init(&l1, mp);
	ec_slist_ull_init(&l2, mp);
	unsigned long long a = 1ULL, b = 2ULL, c = 3ULL, d = 4ULL, e = 5ULL;
	l1.add(&l1, &a);
	l1.add(&l1, &b);
	l1.add(&l1, &c);
	l2.add(&l2, &d);
	l2.add(&l2, &e);
	ec_status_t s = l1.splice_at(&l1, &l2, 1ULL);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 5ULL && l2.size_fn(&l2) == 0ULL);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ull_splice_at\n");
}

void
test_slist_ull_add_all(void* mp)
{
	stamp();
	printf("test_slist_ull_add_all\n");
	ec_slist_ull_t l1, l2;
	ec_slist_ull_init(&l1, mp);
	ec_slist_ull_init(&l2, mp);
	unsigned long long a = 10ULL, b = 20ULL;
	l2.add(&l2, &a);
	l2.add(&l2, &b);
	ec_status_t s = l1.add_all(&l1, &l2);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 2ULL && l2.size_fn(&l2) == 2ULL);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ull_add_all\n");
}

void
test_slist_ull_add_all_at(void* mp)
{
	stamp();
	printf("test_slist_ull_add_all_at\n");
	ec_slist_ull_t l1, l2;
	ec_slist_ull_init(&l1, mp);
	ec_slist_ull_init(&l2, mp);
	unsigned long long a = 10ULL, b = 30ULL, c = 20ULL, d = 25ULL;
	l1.add(&l1, &a);
	l1.add(&l1, &b);
	l2.add(&l2, &c);
	l2.add(&l2, &d);
	ec_status_t s = l1.add_all_at(&l1, &l2, 1ULL);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 4ULL);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ull_add_all_at\n");
}

void
test_slist_ull_sublist(void* mp)
{
	stamp();
	printf("test_slist_ull_sublist\n");

	ec_slist_ull_t l;
	ec_status_t init_status = ec_slist_ull_init(&l, mp);
	if (init_status != EC_OK)
		{
			printf("ERROR: Failed to initialize the list. Status: %d\n", init_status);
			TEST_ASSERT(0ULL); // Force failure
		}

	// Adding elements: 10ULL, 20ULL, 30ULL, 40ULL, 50ULL
	for (unsigned long long i = 0ULL; i < 5ULL; i++)
		{
			unsigned long long v = (i + 1ULL) * 10ULL;
			ec_status_t add_status = l.add(&l, &v);
			if (add_status != EC_OK)
				{
					printf("ERROR: Failed to add value %d to list. Status: %d\n", v, add_status);
					TEST_ASSERT(0ULL); // Force failure
				}
		}

	// Creating sublist from index 1ULL to 4ULL (should contain 20ULL, 30ULL, 40ULL)
	ec_slist_ull_t* sub = NULL;
	ec_status_t sublist_status = l.sublist(&l, 1ULL, 4ULL, &sub);
	if (sublist_status != EC_OK)
		{
			printf("ERROR: Failed to create sublist from index 1ULL to 4. Status: %d\n", sublist_status);
			TEST_ASSERT(0ULL); // Force failure
		}

	// Verify sublist size
	size_t sublist_size = sub->size_fn(sub);
	if (sublist_size != 3ULL)
		{
			printf("ERROR: Sublist size incorrect. Expected 3ULL, got %zu\n", sublist_size);
			TEST_ASSERT(0ULL);
		}

	// Verify sublist content
	unsigned long long expected_values[] = { 20ULL, 30ULL, 40ULL };
	for (size_t i = 0ULL; i < 3ULL; i++)
		{
			void* out = NULL;
			ec_status_t get_status = sub->get_at(sub, i, &out);
			if (get_status != EC_OK)
				{
					printf("ERROR: Failed to get value at sublist index %zu. Status: %d\n", i, get_status);
					TEST_ASSERT(0ULL);
				}
			if (*(unsigned long long*)out != expected_values[i])
				{
					printf("ERROR: Sublist value mismatch at index %zu. Expected %d, got %d\n", i, expected_values[i], *(unsigned long long*)out);
					TEST_ASSERT(0ULL);
				}
		}

	// Cleanup
	ec_status_t free_sub_status = sub->free_fn(sub);
	if (free_sub_status != EC_OK)
		{
			printf("ERROR: Failed to free sublist. Status: %d\n", free_sub_status);
			TEST_ASSERT(0ULL);
		}

	free(sub);

	ec_status_t free_list_status = l.free_fn(&l);
	if (free_list_status != EC_OK)
		{
			printf("ERROR: Failed to free original list. Status: %d\n", free_list_status);
			TEST_ASSERT(0ULL);
		}

	printf("pass: test_slist_ull_sublist\n");
}

void
test_slist_ull_copy_shallow(void* mp)
{
	stamp();
	printf("test_slist_ull_copy_shallow\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	for (unsigned long long i = 1ULL; i <= 3ULL; i++)
		{
			unsigned long long v = i;
			l.add(&l, &v);
		}
	ec_slist_ull_t* cp = NULL;
	ec_status_t s = l.copy_shallow(&l, &cp);
	TEST_ASSERT(s == EC_OK && cp->size == 3ULL);
	cp->free_fn(cp);
	free(cp);
	l.free_fn(&l);
	printf("pass: test_slist_ull_copy_shallow\n");
}

static void*
dupe_ull(void* src)
{
	int* ss = (unsigned long long*)src;
	int* xx = (unsigned long long*)malloc(sizeof(int));
	*xx = *ss;
	return xx;
}

void
test_slist_ull_copy_deep(void* mp)
{
	stamp();
	printf("test_slist_ull_copy_deep\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	for (unsigned long long i = 1ULL; i <= 3ULL; i++)
		{
			unsigned long long v = i * 10ULL;
			l.add(&l, &v);
		}
	ec_slist_ull_t* cp = NULL;
	ec_status_t s = l.copy_deep(&l, dupe_ull, &cp);
	TEST_ASSERT(s == EC_OK && cp->size_fn(cp) == 3ULL);
	cp->free_fn(cp);
	free(cp);
	l.free_fn(&l);
	printf("pass: test_slist_ull_copy_deep\n");
}

void
test_slist_ull_replace_at(void* mp)
{
	stamp();
	printf("test_slist_ull_replace_at\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 5ULL, b = 10ULL, c = 99ULL;
	void* out = NULL;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_status_t s = l.replace_at(&l, &c, 1ULL);
	TEST_ASSERT(s == EC_OK);
	l.get_at(&l, 1ULL, &out);
	TEST_ASSERT(*(unsigned long long*)out == 99ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_replace_at\n");
}

void
test_slist_ull_contains(void* mp)
{
	stamp();
	printf("test_slist_ull_contains\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 10ULL, b = 20ULL, c = 30ULL;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	unsigned long long x = 20ULL;
	size_t r = l.contains(&l, &x);
	TEST_ASSERT(r == 1ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_contains\n");
}

void
test_slist_ull_contains_value(void* mp)
{
	stamp();
	printf("test_slist_ull_contains_value\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 10ULL, b = 20ULL, c = 30ULL;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	unsigned long long x = 30ULL;
	size_t r = l.contains_value(&l, &x, cmp_ull);
	TEST_ASSERT(r == 1ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_contains_value\n");
}

void
test_slist_ull_index_of(void* mp)
{
	stamp();
	printf("test_slist_ull_index_of\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	for (unsigned long long i = 1ULL; i <= 5ULL; i++)
		{
			unsigned long long v = i * 10ULL;
			l.add(&l, &v);
		}
	unsigned long long x = 40ULL;
	size_t idx;
	ec_status_t s = l.index_of(&l, &x, cmp_ull, &idx);
	TEST_ASSERT(s == EC_OK && idx == 3ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_index_of\n");
}

void
test_slist_ull_to_array(void* mp)
{
	stamp();
	printf("test_slist_ull_to_array\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 1ULL, b = 2ULL, c = 3ULL;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	void** arr = NULL;
	ec_status_t s = l.to_array(&l, &arr);
	TEST_ASSERT(s == EC_OK);
	TEST_ASSERT(*(unsigned long long*)arr[0ULL] == 1ULL && *(unsigned long long*)arr[1ULL] == 2ULL && *(unsigned long long*)arr[2ULL] == 3ULL);
	free(arr);
	l.free_fn(&l);
	printf("pass: test_slist_ull_to_array\n");
}

void
test_slist_ull_reverse(void* mp)
{
	stamp();
	printf("test_slist_ull_reverse\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 10ULL, b = 20ULL, c = 30ULL;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	l.reverse(&l);
	void* out;
	l.get_at(&l, 0ULL, &out);
	TEST_ASSERT(*(unsigned long long*)out == 30ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_reverse\n");
}

void
test_slist_ull_sort(void* mp)
{
	stamp();
	printf("test_slist_ull_sort\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 30ULL, b = 10ULL, c = 20ULL;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	ec_status_t s = l.sort(&l, cmp_ull);
	TEST_ASSERT(s == EC_OK);
	void* out;
	l.get_at(&l, 0ULL, &out);
	TEST_ASSERT(*(unsigned long long*)out == 10ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_sort\n");
}

void
test_slist_ull_foreach(void* mp)
{
	stamp();
	printf("test_slist_ull_foreach\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 5ULL, b = 6ULL;
	l.add(&l, &a);
	l.add(&l, &b);
	l.foreach (&l, inc_ull);
	void* out;
	l.get_at(&l, 0ULL, &out);
	TEST_ASSERT(*(unsigned long long*)out == 6ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_foreach\n");
}

void
test_slist_ull_reduce(void* mp)
{
	stamp();
	printf("test_slist_ull_reduce\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	for (unsigned long long i = 1ULL; i <= 4ULL; i++)
		{
			unsigned long long v = i;
			l.add(&l, &v);
		}
	unsigned long long sum = 0ULL;
	ec_status_t s = l.reduce(&l, add_ulls, &sum);
	TEST_ASSERT(s == EC_OK && sum == 10ULL);
	unsigned long long prod = 0ULL;
	s = l.reduce(&l, mul_ulls, &prod);
	TEST_ASSERT(s == EC_OK && prod == 24ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_reduce\n");
}

void
test_slist_ull_filter(void* mp)
{
	stamp();
	printf("test_slist_ull_filter\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long arr[6ULL] = { 10ULL, 15ULL, 20ULL, 25ULL, 30ULL, 35ULL };
	for (unsigned long long i = 0ULL; i < 6ULL; i++)
		l.add(&l, &arr[i]);
	ec_slist_ull_t* fl = NULL;
	ec_status_t s = l.filter(&l, is_even, &fl);
	TEST_ASSERT(s == EC_OK && fl->size_fn(fl) == 3ULL);
	fl->free_fn(fl);
	free(fl);
	l.free_fn(&l);
	printf("pass: test_slist_ull_filter\n");
}

void
test_slist_ull_filter_mut(void* mp)
{
	stamp();
	printf("test_slist_ull_filter_mut\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long arr[6ULL] = { 10ULL, 15ULL, 20ULL, 25ULL, 30ULL, 35ULL };
	for (unsigned long long i = 0ULL; i < 6ULL; i++)
		l.add(&l, &arr[i]);
	ec_status_t s = l.filter_mut(&l, is_even);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 3ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_filter_mut\n");
}

void
test_slist_ull_iterators(void* mp)
{
	stamp();
	printf("test_slist_ull_iterators\n");

	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);

	// Add initial values to the list: 100ULL, 101ULL, 102ULL, 103ULL, 104ULL
	for (unsigned long long i = 0ULL; i < 5ULL; i++)
		{
			unsigned long long v = i + 100ULL;
			l.add(&l, &v);
		}

	ec_slist_ull_iter_t it;
	l.iter_init(&it, &l); // Initialize the iterator

	void* out;
	size_t count = 0ULL;

	// Iterate through the list to verify all elements are >= 100ULL
	while (l.iter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(*(unsigned long long*)out >= 100ULL);
			count++;
		}
	TEST_ASSERT(count == 5ULL); // Ensure all 5ULL elements were traversed

	/**
	 * Correct Usage: `iter_remove` can only be called after `iter_next`
	 * has successfully moved the iterator to a valid node (`current != NULL`).
	 * So, we need to reinitialize the iterator and advance it before removing.
	 */

	l.iter_init(&it, &l);                         // Reinitialize iterator
	TEST_ASSERT(l.iter_next(&it, &out) == EC_OK); // Move to the first element

	// Remove the current node (which is now pointing to the first element, 100ULL)
	ec_status_t s = l.iter_remove(&it);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 4ULL); // List size should be reduced to 4ULL

	// Add a new element (999ULL) at the current iterator position
	unsigned long long x = 999ULL;
	s = l.iter_add(&it, &x);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 5ULL); // List size should now be back to 5ULL

	// Replace the current node's value with 777ULL
	unsigned long long y = 777ULL;
	s = l.iter_replace(&it, &y, NULL);
	TEST_ASSERT(s == EC_OK); // Replacement should succeed

	// Free the list after test completion
	l.free_fn(&l);

	printf("pass: test_slist_ull_iterators\n");
}

void
test_slist_ull_zip_iterators(void* mp)
{
	stamp();
	printf("test_slist_ull_zip_iterators\n");
	ec_slist_ull_t l1, l2;
	ec_slist_ull_init(&l1, mp);
	ec_slist_ull_init(&l2, mp);
	for (unsigned long long i = 0ULL; i < 5ULL; i++)
		{
			unsigned long long aa = i + 1ULL, bb = (i + 1ULL) * 10ULL;
			l1.add(&l1, &aa);
			l2.add(&l2, &bb);
		}
	ec_slist_ull_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	size_t c = 0ULL;
	while (l1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			unsigned long long *r1 = (unsigned long long*)out1, *r2 = (unsigned long long*)out2;
			TEST_ASSERT((*r2) == ((*r1) * 10ULL));
			c++;
		}
	TEST_ASSERT(c == 5ULL);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ull_zip_iterators\n");
}

// Negative test cases
void
test_slist_ull_add_at_neg(void* mp)
{
	stamp();
	printf("test_slist_ull_add_at_neg\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long x = 1ULL, y = 2ULL;
	l.add(&l, &x);
	ec_status_t s = l.add_at(&l, &y, 10ULL);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ull_add_at_neg\n");
}

void
test_slist_ull_remove_neg(void* mp)
{
	stamp();
	printf("test_slist_ull_remove_neg\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 10ULL, b = 20ULL;
	l.add(&l, &a);
	l.add(&l, &b);
	unsigned long long v = 99ULL;
	ec_status_t s = l.remove_fn(&l, &v);
	TEST_ASSERT(s == EC_ERR_NOT_FOUND);
	l.free_fn(&l);
	printf("pass: test_slist_ull_remove_neg\n");
}

void
test_slist_ull_remove_first_neg(void* mp)
{
	stamp();
	printf("test_slist_ull_remove_first_neg\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	ec_status_t s = l.remove_first(&l);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ull_remove_first_neg\n");
}

void
test_slist_ull_remove_last_neg(void* mp)
{
	stamp();
	printf("test_slist_ull_remove_last_neg\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	ec_status_t s = l.remove_last(&l);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ull_remove_last_neg\n");
}

void
test_slist_ull_remove_at_neg(void* mp)
{
	stamp();
	printf("test_slist_ull_remove_at_neg\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 10ULL;
	l.add(&l, &a);
	ec_status_t s = l.remove_at(&l, 5ULL);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ull_remove_at_neg\n");
}

void
test_slist_ull_get_first_neg(void* mp)
{
	stamp();
	printf("test_slist_ull_get_first_neg\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	void* out = NULL;
	ec_status_t s = l.get_first(&l, &out);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ull_get_first_neg\n");
}

void
test_slist_ull_get_last_neg(void* mp)
{
	stamp();
	printf("test_slist_ull_get_last_neg\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	void* out = NULL;
	ec_status_t s = l.get_last(&l, &out);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ull_get_last_neg\n");
}

void
test_slist_ull_splice_neg(void* mp)
{
	stamp();
	printf("test_slist_ull_splice_neg\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	ec_status_t s = l.splice(&l, NULL);
	TEST_ASSERT(s == EC_ERR_INVALID_ARG);
	l.free_fn(&l);
	printf("pass: test_slist_ull_splice_neg\n");
}

void
test_slist_ull_splice_at_neg(void* mp)
{
	stamp();
	printf("test_slist_ull_splice_at_neg\n");
	ec_slist_ull_t l1, l2;
	ec_slist_ull_init(&l1, mp);
	ec_slist_ull_init(&l2, mp);
	unsigned long long a = 10ULL;
	l1.add(&l1, &a);
	ec_status_t s = l1.splice_at(&l1, &l2, 10ULL);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ull_splice_at_neg\n");
}

void
test_slist_ull_sublist_neg(void* mp)
{
	stamp();
	printf("test_slist_ull_sublist_neg\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 10ULL, b = 20ULL;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_slist_ull_t* sub = NULL;
	ec_status_t s = l.sublist(&l, 1ULL, 10ULL, &sub);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ull_sublist_neg\n");
}

void
test_slist_ull_replace_at_neg(void* mp)
{
	stamp();
	printf("test_slist_ull_replace_at_neg\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 10ULL, b = 99ULL;
	l.add(&l, &a);
	ec_status_t s = l.replace_at(&l, &b, 10ULL);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_slist_ull_replace_at_neg\n");
}

void
test_slist_ull_contains_neg(void* mp)
{
	stamp();
	printf("test_slist_ull_contains_neg\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 10ULL, b = 20ULL;
	l.add(&l, &a);
	l.add(&l, &b);
	unsigned long long x = 99ULL;
	size_t r = l.contains(&l, &x);
	TEST_ASSERT(r == 0ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_contains_neg\n");
}

void
test_slist_ull_index_of_neg(void* mp)
{
	stamp();
	printf("test_slist_ull_index_of_neg\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	for (unsigned long long i = 1ULL; i <= 5ULL; i++)
		{
			unsigned long long v = i * 10ULL;
			l.add(&l, &v);
		}
	unsigned long long x = 99ULL;
	size_t idx;
	ec_status_t s = l.index_of(&l, &x, cmp_ull, &idx);
	TEST_ASSERT(s == EC_ERR_NOT_FOUND);
	l.free_fn(&l);
	printf("pass: test_slist_ull_index_of_neg\n");
}

// Additional methods
void
test_slist_ull_add_first(void* mp)
{
	stamp();
	printf("test_slist_ull_add_first\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 10ULL, b = 20ULL;
	l.add(&l, &b);
	l.add_first(&l, &a);
	void* out;
	l.get_at(&l, 0ULL, &out);
	TEST_ASSERT(*(unsigned long long*)out == 10ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_add_first\n");
}

void
test_slist_ull_add_last(void* mp)
{
	stamp();
	printf("test_slist_ull_add_last\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 10ULL, b = 20ULL;
	l.add(&l, &a);
	l.add_last(&l, &b);
	void* out;
	l.get_last(&l, &out);
	TEST_ASSERT(*(unsigned long long*)out == 20ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_add_last\n");
}

void
test_slist_ull_remove_all(void* mp)
{
	stamp();
	printf("test_slist_ull_remove_all\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	for (unsigned long long i = 0ULL; i < 5ULL; i++)
		{
			unsigned long long v = i + 10ULL;
			l.add(&l, &v);
		}
	ec_status_t s = l.remove_all(&l);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 0ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_remove_all\n");
}

static void
cb_ull(void* x)
{
	(void)x;
}

void
test_slist_ull_remove_all_cb(void* mp)
{
	stamp();
	printf("test_slist_ull_remove_all_cb\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	for (unsigned long long i = 0ULL; i < 5ULL; i++)
		{
			unsigned long long v = i + 10ULL;
			l.add(&l, &v);
		}
	ec_status_t s = l.remove_all_cb(&l, cb_ull);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 0ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_remove_all_cb\n");
}

void
test_slist_ull_sort_in_place(void* mp)
{
	stamp();
	printf("test_slist_ull_sort_in_place\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	unsigned long long a = 30ULL, b = 10ULL, c = 20ULL;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	l.sort_in_place(&l, cmp_ull);
	void* out;
	l.get_at(&l, 0ULL, &out);
	TEST_ASSERT(*(unsigned long long*)out == 10ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_sort_in_place\n");
}

void
test_slist_ull_iter_index(void* mp)
{
	stamp();
	printf("test_slist_ull_iter_index\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	for (unsigned long long i = 0ULL; i < 5ULL; i++)
		{
			unsigned long long v = i + 10ULL;
			l.add(&l, &v);
		}
	ec_slist_ull_iter_t it;
	l.iter_init(&it, &l);
	void* out;
	size_t idx = 0ULL;
	while (l.iter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(l.iter_index(&it) == idx);
			idx++;
		}
	l.free_fn(&l);
	printf("pass: test_slist_ull_iter_index\n");
}

void
test_slist_ull_diter(void* mp)
{
	stamp();
	printf("test_slist_ull_diter\n");
	ec_slist_ull_t l;
	ec_slist_ull_init(&l, mp);
	for (unsigned long long i = 0ULL; i < 5ULL; i++)
		{
			unsigned long long v = i + 10ULL;
			l.add(&l, &v);
		}
	ec_slist_ull_iter_t it;
	l.diter_init(&it, &l);
	void* out;
	size_t count = 0ULL;
	while (l.diter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(*(unsigned long long*)out >= 10ULL);
			count++;
		}
	TEST_ASSERT(count == 5ULL);
	TEST_ASSERT(l.diter_index(&it) == 5ULL);
	l.free_fn(&l);
	printf("pass: test_slist_ull_diter\n");
}

void
test_slist_ull_zip_iter_index(void* mp)
{
	stamp();
	printf("test_slist_ull_zip_iter_index\n");
	ec_slist_ull_t l1, l2;
	ec_slist_ull_init(&l1, mp);
	ec_slist_ull_init(&l2, mp);
	for (unsigned long long i = 0ULL; i < 5ULL; i++)
		{
			unsigned long long aa = i + 1ULL, bb = (i + 1ULL) * 10ULL;
			l1.add(&l1, &aa);
			l2.add(&l2, &bb);
		}
	ec_slist_ull_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	size_t idx = 0ULL;
	while (l1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			TEST_ASSERT(l1.zip_iter_index(&zit) == idx + 1ULL);
			idx++;
		}
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ull_zip_iter_index\n");
}

void
test_slist_ull_zip_iter_not_supported(void* mp)
{
	stamp();
	printf("test_slist_ull_zip_iter_not_supported\n");
	ec_slist_ull_t l1, l2;
	ec_slist_ull_init(&l1, mp);
	ec_slist_ull_init(&l2, mp);
	unsigned long long a = 10ULL, b = 20ULL;
	l1.add(&l1, &a);
	l2.add(&l2, &b);
	ec_slist_ull_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	l1.zip_iter_next(&zit, &out1, &out2);
	unsigned long long x = 99ULL, y = 88ULL;
	void *o1, *o2;
	ec_status_t s = l1.zip_iter_add(&zit, &x, &y);
	TEST_ASSERT(s == EC_ERR_NOT_SUPPORTED);
	s = l1.zip_iter_remove(&zit, &o1, &o2);
	TEST_ASSERT(s == EC_ERR_NOT_SUPPORTED);
	s = l1.zip_iter_replace(&zit, &x, &y, &o1, &o2);
	TEST_ASSERT(s == EC_ERR_NOT_SUPPORTED);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_slist_ull_zip_iter_not_supported\n");
}

int
main(void)
{
	printf("Starting ec_slist_ull tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64ULL, 100ULL, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8ULL, 0ULL, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_slist_ull_add(mp);
	test_slist_ull_add_at(mp);
	test_slist_ull_remove(mp);
	test_slist_ull_remove_first(mp);
	test_slist_ull_remove_last(mp);
	test_slist_ull_remove_at(mp);
	test_slist_ull_get_at(mp);
	test_slist_ull_get_first(mp);
	test_slist_ull_get_last(mp);
	test_slist_ull_splice(mp);
	test_slist_ull_splice_at(mp);
	test_slist_ull_add_all(mp);
	test_slist_ull_add_all_at(mp);
	test_slist_ull_sublist(mp);
	test_slist_ull_copy_shallow(mp);
	test_slist_ull_copy_deep(mp);
	test_slist_ull_replace_at(mp);
	test_slist_ull_contains(mp);
	test_slist_ull_contains_value(mp);
	test_slist_ull_index_of(mp);
	test_slist_ull_to_array(mp);
	test_slist_ull_reverse(mp);
	test_slist_ull_sort(mp);
	test_slist_ull_foreach(mp);
	test_slist_ull_reduce(mp);
	test_slist_ull_filter(mp);
	test_slist_ull_filter_mut(mp);
	test_slist_ull_iterators(mp);
	test_slist_ull_zip_iterators(mp);
	// Negative tests
	test_slist_ull_add_at_neg(mp);
	test_slist_ull_remove_neg(mp);
	test_slist_ull_remove_first_neg(mp);
	test_slist_ull_remove_last_neg(mp);
	test_slist_ull_remove_at_neg(mp);
	test_slist_ull_get_first_neg(mp);
	test_slist_ull_get_last_neg(mp);
	test_slist_ull_splice_neg(mp);
	test_slist_ull_splice_at_neg(mp);
	test_slist_ull_sublist_neg(mp);
	test_slist_ull_replace_at_neg(mp);
	test_slist_ull_contains_neg(mp);
	test_slist_ull_index_of_neg(mp);
	// Additional methods
	test_slist_ull_add_first(mp);
	test_slist_ull_add_last(mp);
	test_slist_ull_remove_all(mp);
	test_slist_ull_remove_all_cb(mp);
	test_slist_ull_sort_in_place(mp);
	test_slist_ull_iter_index(mp);
	test_slist_ull_diter(mp);
	test_slist_ull_zip_iter_index(mp);
	test_slist_ull_zip_iter_not_supported(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif
	printf("All ec_slist_ull tests passed.\n");
	return 0ULL;
}
