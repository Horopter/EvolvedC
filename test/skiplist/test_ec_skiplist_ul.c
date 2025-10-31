#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../../src/ec_skiplist.h"
#include <stdbool.h>
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

static void
print_stacktrace(void)
{
#ifdef __linux__
	void* arr[32];
	size_t sz = backtrace(arr, 32);
	char** sym = backtrace_symbols(arr, sz);
	if (sym)
		{
			fprintf(stderr, "---StackTrace---\n");
			for (size_t i = 0; i < sz; i++)
				fprintf(stderr, "%s\n", sym[i]);
			free(sym);
		}
#else
	fprintf(stderr, "(NoStackTrace)\n");
#endif
}

#define TEST_ASSERT(cond)                                                            \
	do                                                                               \
		{                                                                            \
			if (!(cond))                                                             \
				{                                                                    \
					fprintf(stderr, "[FAIL]%s:%d: %s\n", __FILE__, __LINE__, #cond); \
					print_stacktrace();                                              \
					exit(3);                                                         \
				}                                                                    \
		}                                                                            \
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

static void
test_skiplist_ul_init_pos(void* mp)
{
	stamp();
	printf("test_skiplist_ul_init_pos\n");
	ec_skiplist_ul_t sl;
	ec_status_t st = ec_skiplist_ul_init(&sl, mp, NULL);
	TEST_ASSERT(st == EC_OK);
	TEST_ASSERT(sl.size == 0);
	sl.remove_all(&sl);
	printf("pass:test_skiplist_ul_init_pos\n");
}

static void
test_skiplist_ul_init_neg(void* mp)
{
	stamp();
	printf("test_skiplist_ul_init_neg\n");
	TEST_ASSERT(ec_skiplist_ul_init(NULL, mp, NULL) == EC_ERR_INVALID_ARG);
	printf("pass:test_skiplist_ul_init_neg\n");
}

static void
test_skiplist_ul_add_pos(void* mp)
{
	stamp();
	printf("test_skiplist_ul_add_pos\n");
	ec_skiplist_ul_t sl;
	ec_skiplist_ul_init(&sl, mp, NULL);
	ul k1 = 10UL, k2 = 20UL;
	sl.add(&sl, &k1);
	sl.add(&sl, &k2);
	TEST_ASSERT(sl.size_fn(&sl) == 2UL);
	TEST_ASSERT(sl.contains(&sl, &k1) == EC_OK && sl.contains(&sl, &k2) == EC_OK);
	ul bad = 999UL;
	TEST_ASSERT(sl.contains(&sl, &bad) == EC_ERR_NOT_FOUND);
	sl.remove_all(&sl);
	printf("pass:test_skiplist_ul_add_pos\n");
}

static void
test_skiplist_ul_add_neg(void* mp)
{
	stamp();
	printf("test_skiplist_ul_add_neg\n");
	ec_skiplist_ul_t sl;
	ec_skiplist_ul_init(&sl, mp, NULL);
	ul k = 10UL;
	TEST_ASSERT(sl.add(NULL, &k) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(sl.add(&sl, NULL) == EC_ERR_INVALID_ARG);
	sl.remove_all(&sl);
	printf("pass:test_skiplist_ul_add_neg\n");
}

static void
test_skiplist_ul_add_duplicate(void* mp)
{
	stamp();
	printf("test_skiplist_ul_add_duplicate\n");
	ec_skiplist_ul_t sl;
	ec_skiplist_ul_init(&sl, mp, NULL);
	ul k = 42UL;
	sl.add(&sl, &k);
	TEST_ASSERT(sl.size_fn(&sl) == 1UL);
	sl.add(&sl, &k);
	TEST_ASSERT(sl.size_fn(&sl) == 1UL);
	TEST_ASSERT(sl.contains(&sl, &k) == EC_OK);
	sl.remove_all(&sl);
	printf("pass:test_skiplist_ul_add_duplicate\n");
}

static void
test_skiplist_ul_remove_pos(void* mp)
{
	stamp();
	printf("test_skiplist_ul_remove_pos\n");
	ec_skiplist_ul_t sl;
	ec_skiplist_ul_init(&sl, mp, NULL);
	ul k1 = 1UL, k2 = 2UL, k3 = 3UL;
	sl.add(&sl, &k1);
	sl.add(&sl, &k2);
	sl.add(&sl, &k3);
	TEST_ASSERT(sl.size_fn(&sl) == 3UL);
	TEST_ASSERT(sl.remove_f(&sl, &k2) == EC_OK);
	TEST_ASSERT(sl.size_fn(&sl) == 2UL);
	TEST_ASSERT(sl.remove_f(&sl, &k2) == EC_ERR_NOT_FOUND);
	sl.remove_all(&sl);
	printf("pass:test_skiplist_ul_remove_pos\n");
}

static void
test_skiplist_ul_remove_neg(void* mp)
{
	stamp();
	printf("test_skiplist_ul_remove_neg\n");
	ec_skiplist_ul_t sl;
	ec_skiplist_ul_init(&sl, mp, NULL);
	ul k = 10UL;
	sl.add(&sl, &k);
	TEST_ASSERT(sl.remove_f(NULL, &k) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(sl.remove_f(&sl, NULL) == EC_ERR_INVALID_ARG);
	ul bad = 999UL;
	TEST_ASSERT(sl.remove_f(&sl, &bad) == EC_ERR_NOT_FOUND);
	sl.remove_all(&sl);
	printf("pass:test_skiplist_ul_remove_neg\n");
}

static void
test_skiplist_ul_remove_all_pos(void* mp)
{
	stamp();
	printf("test_skiplist_ul_remove_all_pos\n");
	ec_skiplist_ul_t sl;
	ec_skiplist_ul_init(&sl, mp, NULL);
	for (ul i = 0; i < 10UL; i++)
		sl.add(&sl, &i);
	TEST_ASSERT(sl.size_fn(&sl) == 10UL);
	TEST_ASSERT(sl.remove_all(&sl) == EC_OK && sl.size_fn(&sl) == 0);
	TEST_ASSERT(sl.remove_all(&sl) == EC_OK && sl.size_fn(&sl) == 0);
	printf("pass:test_skiplist_ul_remove_all_pos\n");
}

static void
test_skiplist_ul_remove_all_neg(void* mp)
{
	stamp();
	printf("test_skiplist_ul_remove_all_neg\n");
	ec_skiplist_ul_t sl;
	ec_skiplist_ul_init(&sl, mp, NULL);
	TEST_ASSERT(sl.remove_all(NULL) == EC_ERR_INVALID_ARG);
	sl.remove_all(&sl);
	printf("pass:test_skiplist_ul_remove_all_neg\n");
}

static void
test_skiplist_ul_contains_pos(void* mp)
{
	stamp();
	printf("test_skiplist_ul_contains_pos\n");
	ec_skiplist_ul_t sl;
	ec_skiplist_ul_init(&sl, mp, NULL);
	ul k = 999UL;
	sl.add(&sl, &k);
	TEST_ASSERT(sl.contains(&sl, &k) == EC_OK);
	sl.remove_all(&sl);
	printf("pass:test_skiplist_ul_contains_pos\n");
}

static void
test_skiplist_ul_contains_neg(void* mp)
{
	stamp();
	printf("test_skiplist_ul_contains_neg\n");
	ec_skiplist_ul_t sl;
	ec_skiplist_ul_init(&sl, mp, NULL);
	ul k = 10UL;
	sl.add(&sl, &k);
	TEST_ASSERT(sl.contains(NULL, &k) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(sl.contains(&sl, NULL) == EC_ERR_INVALID_ARG);
	ul bad = 1000UL;
	TEST_ASSERT(sl.contains(&sl, &bad) == EC_ERR_NOT_FOUND);
	sl.remove_all(&sl);
	printf("pass:test_skiplist_ul_contains_neg\n");
}

static void
test_skiplist_ul_get_keys_pos(void* mp)
{
	stamp();
	printf("test_skiplist_ul_get_keys_pos\n");
	ec_skiplist_ul_t sl;
	ec_skiplist_ul_init(&sl, mp, NULL);
	for (ul i = 1UL; i <= 5; i++)
		sl.add(&sl, &i);
	ul* keys = NULL;
	size_t count = 0;
	ec_status_t st = sl.get_keys(&sl, &keys, &count);
	TEST_ASSERT(st == EC_OK && count == 5);
	for (size_t i = 1UL; i < count; i++)
		TEST_ASSERT(keys[i - 1UL] <= keys[i]);
	EC_FREE(keys);
	/* Test empty list */
	ul* keys2 = NULL;
	size_t count2 = 0;
	TEST_ASSERT(sl.get_keys(&sl, &keys2, &count2) == EC_OK && count2 == 0);
	sl.remove_all(&sl);
	printf("pass:test_skiplist_ul_get_keys_pos\n");
}

static void
test_skiplist_ul_get_keys_neg(void* mp)
{
	stamp();
	printf("test_skiplist_ul_get_keys_neg\n");
	ec_skiplist_ul_t sl;
	ec_skiplist_ul_init(&sl, mp, NULL);
	ul k = 10UL;
	sl.add(&sl, &k);
	ul* keys = NULL;
	size_t count = 0;
	TEST_ASSERT(sl.get_keys(NULL, &keys, &count) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(sl.get_keys(&sl, NULL, &count) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(sl.get_keys(&sl, &keys, NULL) == EC_ERR_INVALID_ARG);
	sl.remove_all(&sl);
	printf("pass:test_skiplist_ul_get_keys_neg\n");
}

static void
foreach_key_ul(ul k)
{
	(void)k;
}

static void
test_skiplist_ul_foreach_pos(void* mp)
{
	stamp();
	printf("test_skiplist_ul_foreach_pos\n");
	ec_skiplist_ul_t sl;
	ec_skiplist_ul_init(&sl, mp, NULL);
	for (ul i = 0; i < 5; i++)
		{
			ul v = i + 10UL;
			sl.add(&sl, &v);
		}
	TEST_ASSERT(sl.foreach_key(&sl, foreach_key_ul) == EC_OK);
	sl.remove_all(&sl);
	printf("pass:test_skiplist_ul_foreach_pos\n");
}

static void
test_skiplist_ul_foreach_neg(void* mp)
{
	stamp();
	printf("test_skiplist_ul_foreach_neg\n");
	ec_skiplist_ul_t sl;
	ec_skiplist_ul_init(&sl, mp, NULL);
	ul k = 10UL;
	sl.add(&sl, &k);
	TEST_ASSERT(sl.foreach_key(NULL, foreach_key_ul) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(sl.foreach_key(&sl, NULL) == EC_ERR_INVALID_ARG);
	sl.remove_all(&sl);
	printf("pass:test_skiplist_ul_foreach_neg\n");
}

static void
test_skiplist_ul_iterators_empty_pos(void* mp)
{
	stamp();
	printf("test_skiplist_ul_iterators_empty_pos\n");
	ec_skiplist_ul_t sl;
	ec_skiplist_ul_init(&sl, mp, NULL);
	ec_skiplist_ul_iter_t it;
	TEST_ASSERT(sl.iter_init(&sl, &it) == EC_OK);
	ul key;
	TEST_ASSERT(sl.iter_next(&it, &key) == EC_ERR_OUT_OF_BOUNDS);
	sl.remove_all(&sl);
	printf("pass:test_skiplist_ul_iterators_empty_pos\n");
}

static void
test_skiplist_ul_iterators_neg(void* mp)
{
	stamp();
	printf("test_skiplist_ul_iterators_neg\n");
	ec_skiplist_ul_t sl;
	ec_skiplist_ul_init(&sl, mp, NULL);
	ec_skiplist_ul_iter_t it;
	/* Test iter_init with NULL */
	TEST_ASSERT(sl.iter_init(NULL, &it) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(sl.iter_init(&sl, NULL) == EC_ERR_INVALID_ARG);
	/* Test iter_next with NULL */
	sl.iter_init(&sl, &it);
	ul key;
	TEST_ASSERT(sl.iter_next(NULL, &key) == EC_ERR_INVALID_ARG);
	sl.iter_init(&sl, &it);
	TEST_ASSERT(sl.iter_next(&it, NULL) == EC_ERR_INVALID_ARG);
	/* Test iter_remove with invalid iterator */
	sl.iter_init(&sl, &it);
	TEST_ASSERT(sl.iter_remove(&it) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(sl.iter_remove(NULL) == EC_ERR_INVALID_ARG);
	sl.remove_all(&sl);
	printf("pass:test_skiplist_ul_iterators_neg\n");
}

static void
test_skiplist_ul_iterators_pos(void* mp)
{
	stamp();
	printf("test_skiplist_ul_iterators_pos\n");
	ec_skiplist_ul_t sl;
	ec_skiplist_ul_init(&sl, mp, NULL);
	for (ul i = 0; i < 5; i++)
		{
			ul k = i + 100UL;
			sl.add(&sl, &k);
		}
	ec_skiplist_ul_iter_t it;
	TEST_ASSERT(sl.iter_init(&sl, &it) == EC_OK);
	ul key;
	size_t cnt = 0;
	while (sl.iter_next(&it, &key) == EC_OK)
		{
			TEST_ASSERT(key >= 100UL && key < 105);
			cnt++;
		}
	TEST_ASSERT(cnt == 5);
	/* Test removal during iteration */
	TEST_ASSERT(sl.iter_init(&sl, &it) == EC_OK);
	TEST_ASSERT(sl.iter_next(&it, &key) == EC_OK);
	TEST_ASSERT(sl.iter_remove(&it) == EC_OK && sl.size_fn(&sl) == 4);
	sl.remove_all(&sl);
	printf("pass:test_skiplist_ul_iterators_pos\n");
}

ul
main(void)
{
	printf("Starting ec_skiplist_ul tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100UL, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2UL.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_skiplist_ul_init_pos(mp);
	test_skiplist_ul_init_neg(mp);
	test_skiplist_ul_add_pos(mp);
	test_skiplist_ul_add_neg(mp);
	test_skiplist_ul_add_duplicate(mp);
	test_skiplist_ul_remove_pos(mp);
	test_skiplist_ul_remove_neg(mp);
	test_skiplist_ul_remove_all_pos(mp);
	test_skiplist_ul_remove_all_neg(mp);
	test_skiplist_ul_contains_pos(mp);
	test_skiplist_ul_contains_neg(mp);
	test_skiplist_ul_get_keys_pos(mp);
	test_skiplist_ul_get_keys_neg(mp);
	test_skiplist_ul_foreach_pos(mp);
	test_skiplist_ul_foreach_neg(mp);
	test_skiplist_ul_iterators_empty_pos(mp);
	test_skiplist_ul_iterators_neg(mp);
	test_skiplist_ul_iterators_pos(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif

	printf("All ec_skiplist_ul tests passed.\n");
	return 0;
}
