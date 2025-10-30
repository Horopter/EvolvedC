#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../../src/ec_dlist.h"
#include <assert.h>
#include <stdbool.h>
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif
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
		printf("[timeerr] ");
}
static int
cmp_int(const void* a, const void* b)
{
	return (*(int*)a - *(int*)b);
}
static bool
is_even(const void* x)
{
	return ((*(int*)x) % 2) == 0;
}
static void
inc_int(void* x)
{
	(*(int*)x)++;
}
static void
add_ints(void* a, void* b, void* out)
{
	*(int*)out = (*(int*)a) + (*(int*)b);
}
static void
mul_ints(void* a, void* b, void* out)
{
	*(int*)out = (*(int*)a) * (*(int*)b);
}
void
test_dlist_int_add(void* mp)
{
	stamp();
	printf("test_dlist_int_add\n");
	ec_dlist_int_t l;
	ec_status_t s = ec_dlist_int_init(&l, mp);
	TEST_ASSERT(s == EC_OK);
	int a = 10, b = 20;
	l.add(&l, &a);
	l.add(&l, &b);
	TEST_ASSERT(l.size_fn(&l) == 2);
	l.free_fn(&l);
	printf("pass: test_dlist_int_add\n");
}
void
test_dlist_int_add_at(void* mp)
{
	stamp();
	printf("test_dlist_int_add_at\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	int x = 1, y = 2, z = 3;
	l.add(&l, &x);
	l.add(&l, &z);
	ec_status_t st = l.add_at(&l, &y, 1);
	TEST_ASSERT(st == EC_OK);
	void* out;
	l.get_at(&l, 1, &out);
	TEST_ASSERT(*(int*)out == 2);
	TEST_ASSERT(l.size_fn(&l) == 3);
	l.free_fn(&l);
	printf("pass: test_dlist_int_add_at\n");
}
void
test_dlist_int_remove(void* mp)
{
	stamp();
	printf("test_dlist_int_remove\n");
	ec_dlist_int_t l;
	ec_status_t init_status = ec_dlist_int_init(&l, mp);
	TEST_ASSERT(init_status == EC_OK);
	int a = 10, b = 20, c = 30;
	ec_status_t sa = l.add(&l, &a), sb = l.add(&l, &b), sc = l.add(&l, &c);
	TEST_ASSERT(sa == EC_OK && sb == EC_OK && sc == EC_OK);
	int v = 20;
	ec_status_t s = l.remove_fn(&l, &v);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 2);
	l.free_fn(&l);
	printf("pass: test_dlist_int_remove\n");
}
void
test_dlist_int_remove_first(void* mp)
{
	stamp();
	printf("test_dlist_int_remove_first\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	int a = 11, b = 22;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_status_t s = l.remove_first(&l);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 1);
	l.free_fn(&l);
	printf("pass: test_dlist_int_remove_first\n");
}
void
test_dlist_int_remove_last(void* mp)
{
	stamp();
	printf("test_dlist_int_remove_last\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	int a = 33, b = 44;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_status_t s = l.remove_last(&l);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 1);
	l.free_fn(&l);
	printf("pass: test_dlist_int_remove_last\n");
}
void
test_dlist_int_remove_at(void* mp)
{
	stamp();
	printf("test_dlist_int_remove_at\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	int a = 10, b = 20, c = 30;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	ec_status_t s = l.remove_at(&l, 1);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 2);
	l.free_fn(&l);
	printf("pass: test_dlist_int_remove_at\n");
}
void
test_dlist_int_get_at(void* mp)
{
	stamp();
	printf("test_dlist_int_get_at\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	int a = 100, b = 200;
	l.add(&l, &a);
	l.add(&l, &b);
	void* out = NULL;
	ec_status_t s = l.get_at(&l, 1, &out);
	TEST_ASSERT(s == EC_OK && *(int*)out == 200);
	s = l.get_at(&l, 2, &out);
	TEST_ASSERT(s == EC_ERR_OUT_OF_BOUNDS);
	l.free_fn(&l);
	printf("pass: test_dlist_int_get_at\n");
}
void
test_dlist_int_get_first(void* mp)
{
	stamp();
	printf("test_dlist_int_get_first\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	int a = 777;
	l.add(&l, &a);
	void* out = NULL;
	ec_status_t s = l.get_first(&l, &out);
	TEST_ASSERT(s == EC_OK && *(int*)out == 777);
	l.free_fn(&l);
	printf("pass: test_dlist_int_get_first\n");
}
void
test_dlist_int_get_last(void* mp)
{
	stamp();
	printf("test_dlist_int_get_last\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	int a = 10, b = 20;
	l.add(&l, &a);
	l.add(&l, &b);
	void* out;
	ec_status_t s = l.get_last(&l, &out);
	TEST_ASSERT(s == EC_OK && *(int*)out == 20);
	l.free_fn(&l);
	printf("pass: test_dlist_int_get_last\n");
}
void
test_dlist_int_splice(void* mp)
{
	stamp();
	printf("test_dlist_int_splice\n");
	ec_dlist_int_t l1, l2;
	ec_dlist_int_init(&l1, mp);
	ec_dlist_int_init(&l2, mp);
	int a = 10, b = 20, c = 30, d = 40;
	l1.add(&l1, &a);
	l1.add(&l1, &b);
	l2.add(&l2, &c);
	l2.add(&l2, &d);
	ec_status_t s = l1.splice(&l1, &l2);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 4 && l2.size_fn(&l2) == 0);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_dlist_int_splice\n");
}
void
test_dlist_int_splice_at(void* mp)
{
	stamp();
	printf("test_dlist_int_splice_at\n");
	ec_dlist_int_t l1, l2;
	ec_dlist_int_init(&l1, mp);
	ec_dlist_int_init(&l2, mp);
	int a = 1, b = 2, c = 3, d = 4, e = 5;
	l1.add(&l1, &a);
	l1.add(&l1, &b);
	l1.add(&l1, &c);
	l2.add(&l2, &d);
	l2.add(&l2, &e);
	ec_status_t s = l1.splice_at(&l1, &l2, 1);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 5 && l2.size_fn(&l2) == 0);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_dlist_int_splice_at\n");
}
void
test_dlist_int_add_all(void* mp)
{
	stamp();
	printf("test_dlist_int_add_all\n");
	ec_dlist_int_t l1, l2;
	ec_dlist_int_init(&l1, mp);
	ec_dlist_int_init(&l2, mp);
	int a = 10, b = 20;
	l2.add(&l2, &a);
	l2.add(&l2, &b);
	ec_status_t s = l1.add_all(&l1, &l2);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 2 && l2.size_fn(&l2) == 2);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_dlist_int_add_all\n");
}
void
test_dlist_int_add_all_at(void* mp)
{
	stamp();
	printf("test_dlist_int_add_all_at\n");
	ec_dlist_int_t l1, l2;
	ec_dlist_int_init(&l1, mp);
	ec_dlist_int_init(&l2, mp);
	int a = 10, b = 30, c = 20, d = 25;
	l1.add(&l1, &a);
	l1.add(&l1, &b);
	l2.add(&l2, &c);
	l2.add(&l2, &d);
	ec_status_t s = l1.add_all_at(&l1, &l2, 1);
	TEST_ASSERT(s == EC_OK && l1.size_fn(&l1) == 4);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_dlist_int_add_all_at\n");
}
void
test_dlist_int_sublist(void* mp)
{
	stamp();
	printf("test_dlist_int_sublist\n");
	ec_dlist_int_t l;
	ec_status_t init_status = ec_dlist_int_init(&l, mp);
	TEST_ASSERT(init_status == EC_OK);
	for (int i = 0; i < 5; i++)
		{
			int v = (i + 1) * 10;
			TEST_ASSERT(l.add(&l, &v) == EC_OK);
		}
	ec_dlist_int_t* sub = NULL;
	ec_status_t s = l.sublist(&l, 1, 4, &sub);
	TEST_ASSERT(s == EC_OK);
	size_t subsize = sub->size_fn(sub);
	TEST_ASSERT(subsize == 3);
	int exp[] = { 20, 30, 40 };
	for (size_t i = 0; i < 3; i++)
		{
			void* out = NULL;
			TEST_ASSERT(sub->get_at(sub, i, &out) == EC_OK && *(int*)out == exp[i]);
		}
	TEST_ASSERT(sub->free_fn(sub) == EC_OK);
	free(sub);
	TEST_ASSERT(l.free_fn(&l) == EC_OK);
	printf("pass: test_dlist_int_sublist\n");
}
void
test_dlist_int_copy_shallow(void* mp)
{
	stamp();
	printf("test_dlist_int_copy_shallow\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	for (int i = 1; i <= 3; i++)
		{
			int v = i;
			l.add(&l, &v);
		}
	ec_dlist_int_t* cp = NULL;
	ec_status_t s = l.copy_shallow(&l, &cp);
	TEST_ASSERT(s == EC_OK && cp->size == 3);
	cp->free_fn(cp);
	free(cp);
	l.free_fn(&l);
	printf("pass: test_dlist_int_copy_shallow\n");
}
static void*
dupe_int(void* src)
{
	int* ss = (int*)src;
	int* xx = (int*)malloc(sizeof(int));
	*xx = *ss;
	return xx;
}
void
test_dlist_int_copy_deep(void* mp)
{
	stamp();
	printf("test_dlist_int_copy_deep\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	for (int i = 1; i <= 3; i++)
		{
			int v = i * 10;
			l.add(&l, &v);
		}
	ec_dlist_int_t* cp = NULL;
	ec_status_t s = l.copy_deep(&l, dupe_int, &cp);
	TEST_ASSERT(s == EC_OK && cp->size_fn(cp) == 3);
	cp->free_fn(cp);
	free(cp);
	l.free_fn(&l);
	printf("pass: test_dlist_int_copy_deep\n");
}
void
test_dlist_int_replace_at(void* mp)
{
	stamp();
	printf("test_dlist_int_replace_at\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	int a = 5, b = 10, c = 99;
	void* out = NULL;
	l.add(&l, &a);
	l.add(&l, &b);
	ec_status_t s = l.replace_at(&l, &c, 1);
	TEST_ASSERT(s == EC_OK);
	l.get_at(&l, 1, &out);
	TEST_ASSERT(*(int*)out == 99);
	l.free_fn(&l);
	printf("pass: test_dlist_int_replace_at\n");
}
void
test_dlist_int_contains(void* mp)
{
	stamp();
	printf("test_dlist_int_contains\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	int a = 10, b = 20, c = 30;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	int x = 20;
	size_t r = l.contains(&l, &x);
	TEST_ASSERT(r == 1);
	l.free_fn(&l);
	printf("pass: test_dlist_int_contains\n");
}
void
test_dlist_int_contains_value(void* mp)
{
	stamp();
	printf("test_dlist_int_contains_value\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	int a = 10, b = 20, c = 30;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	int x = 30;
	size_t r = l.contains_value(&l, &x, cmp_int);
	TEST_ASSERT(r == 1);
	l.free_fn(&l);
	printf("pass: test_dlist_int_contains_value\n");
}
void
test_dlist_int_index_of(void* mp)
{
	stamp();
	printf("test_dlist_int_index_of\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	for (int i = 1; i <= 5; i++)
		{
			int v = i * 10;
			l.add(&l, &v);
		}
	int x = 40;
	size_t idx;
	ec_status_t s = l.index_of(&l, &x, cmp_int, &idx);
	TEST_ASSERT(s == EC_OK && idx == 3);
	l.free_fn(&l);
	printf("pass: test_dlist_int_index_of\n");
}
void
test_dlist_int_to_array(void* mp)
{
	stamp();
	printf("test_dlist_int_to_array\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	int a = 1, b = 2, c = 3;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	void** arr = NULL;
	ec_status_t s = l.to_array(&l, &arr);
	TEST_ASSERT(s == EC_OK);
	TEST_ASSERT(*(int*)arr[0] == 1 && *(int*)arr[1] == 2 && *(int*)arr[2] == 3);
	free(arr);
	l.free_fn(&l);
	printf("pass: test_dlist_int_to_array\n");
}
void
test_dlist_int_reverse(void* mp)
{
	stamp();
	printf("test_dlist_int_reverse\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	int a = 10, b = 20, c = 30;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	l.reverse(&l);
	void* out;
	l.get_at(&l, 0, &out);
	TEST_ASSERT(*(int*)out == 30);
	l.free_fn(&l);
	printf("pass: test_dlist_int_reverse\n");
}
void
test_dlist_int_sort(void* mp)
{
	stamp();
	printf("test_dlist_int_sort\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	int a = 30, b = 10, c = 20;
	l.add(&l, &a);
	l.add(&l, &b);
	l.add(&l, &c);
	ec_status_t s = l.sort(&l, cmp_int);
	TEST_ASSERT(s == EC_OK);
	void* out;
	l.get_at(&l, 0, &out);
	TEST_ASSERT(*(int*)out == 10);
	l.free_fn(&l);
	printf("pass: test_dlist_int_sort\n");
}
void
test_dlist_int_foreach(void* mp)
{
	stamp();
	printf("test_dlist_int_foreach\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	int a = 5, b = 6;
	l.add(&l, &a);
	l.add(&l, &b);
	l.foreach (&l, inc_int);
	void* out;
	l.get_at(&l, 0, &out);
	TEST_ASSERT(*(int*)out == 6);
	l.free_fn(&l);
	printf("pass: test_dlist_int_foreach\n");
}
void
test_dlist_int_reduce(void* mp)
{
	stamp();
	printf("test_dlist_int_reduce\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	for (int i = 1; i <= 4; i++)
		{
			int v = i;
			l.add(&l, &v);
		}
	int sum = 0;
	ec_status_t s = l.reduce(&l, add_ints, &sum);
	TEST_ASSERT(s == EC_OK && sum == 10);
	int prod = 0;
	s = l.reduce(&l, mul_ints, &prod);
	TEST_ASSERT(s == EC_OK && prod == 24);
	l.free_fn(&l);
	printf("pass: test_dlist_int_reduce\n");
}
void
test_dlist_int_filter(void* mp)
{
	stamp();
	printf("test_dlist_int_filter\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	int arr[6] = { 10, 15, 20, 25, 30, 35 };
	for (int i = 0; i < 6; i++)
		l.add(&l, &arr[i]);
	ec_dlist_int_t* fl = NULL;
	ec_status_t s = l.filter(&l, is_even, &fl);
	TEST_ASSERT(s == EC_OK && fl->size_fn(fl) == 3);
	fl->free_fn(fl);
	free(fl);
	l.free_fn(&l);
	printf("pass: test_dlist_int_filter\n");
}
void
test_dlist_int_filter_mut(void* mp)
{
	stamp();
	printf("test_dlist_int_filter_mut\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	int arr[6] = { 10, 15, 20, 25, 30, 35 };
	for (int i = 0; i < 6; i++)
		l.add(&l, &arr[i]);
	ec_status_t s = l.filter_mut(&l, is_even);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 3);
	l.free_fn(&l);
	printf("pass: test_dlist_int_filter_mut\n");
}
void
test_dlist_int_iterators(void* mp)
{
	stamp();
	printf("test_dlist_int_iterators\n");
	ec_dlist_int_t l;
	ec_dlist_int_init(&l, mp);
	for (int i = 0; i < 5; i++)
		{
			int v = i + 100;
			l.add(&l, &v);
		}
	ec_dlist_int_iter_t it;
	l.iter_init(&it, &l);
	void* out;
	size_t count = 0;
	while (l.iter_next(&it, &out) == EC_OK)
		{
			TEST_ASSERT(*(int*)out >= 100);
			count++;
		}
	TEST_ASSERT(count == 5);
	l.iter_init(&it, &l);
	TEST_ASSERT(l.iter_next(&it, &out) == EC_OK);
	ec_status_t s = l.iter_remove(&it);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 4);
	int x = 999;
	s = l.iter_add(&it, &x);
	TEST_ASSERT(s == EC_OK && l.size_fn(&l) == 5);
	int y = 777;
	s = l.iter_replace(&it, &y, NULL);
	TEST_ASSERT(s == EC_OK);
	l.free_fn(&l);
	printf("pass: test_dlist_int_iterators\n");
}
void
test_dlist_int_zip_iterators(void* mp)
{
	stamp();
	printf("test_dlist_int_zip_iterators\n");
	ec_dlist_int_t l1, l2;
	ec_dlist_int_init(&l1, mp);
	ec_dlist_int_init(&l2, mp);
	for (int i = 0; i < 5; i++)
		{
			int aa = i + 1, bb = (i + 1) * 10;
			l1.add(&l1, &aa);
			l2.add(&l2, &bb);
		}
	ec_dlist_int_zip_iter_t zit;
	l1.zip_iter_init(&zit, &l1, &l2);
	void *out1, *out2;
	size_t c = 0;
	while (l1.zip_iter_next(&zit, &out1, &out2) == EC_OK)
		{
			int *r1 = (int*)out1, *r2 = (int*)out2;
			TEST_ASSERT((*r2) == ((*r1) * 10));
			c++;
		}
	TEST_ASSERT(c == 5);
	l1.free_fn(&l1);
	l2.free_fn(&l2);
	printf("pass: test_dlist_int_zip_iterators\n");
}
int
main(void)
{
	printf("Starting ec_dlist_int tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif
	test_dlist_int_add(mp);
	test_dlist_int_add_at(mp);
	test_dlist_int_remove(mp);
	test_dlist_int_remove_first(mp);
	test_dlist_int_remove_last(mp);
	test_dlist_int_remove_at(mp);
	test_dlist_int_get_at(mp);
	test_dlist_int_get_first(mp);
	test_dlist_int_get_last(mp);
	test_dlist_int_splice(mp);
	test_dlist_int_splice_at(mp);
	test_dlist_int_add_all(mp);
	test_dlist_int_add_all_at(mp);
	test_dlist_int_sublist(mp);
	test_dlist_int_copy_shallow(mp);
	test_dlist_int_copy_deep(mp);
	test_dlist_int_replace_at(mp);
	test_dlist_int_contains(mp);
	test_dlist_int_contains_value(mp);
	test_dlist_int_index_of(mp);
	test_dlist_int_to_array(mp);
	test_dlist_int_reverse(mp);
	test_dlist_int_sort(mp);
	test_dlist_int_foreach(mp);
	test_dlist_int_reduce(mp);
	test_dlist_int_filter(mp);
	test_dlist_int_filter_mut(mp);
	test_dlist_int_iterators(mp);
	test_dlist_int_zip_iterators(mp);
#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif
	printf("All ec_dlist_int tests passed.\n");
	return 0;
}
