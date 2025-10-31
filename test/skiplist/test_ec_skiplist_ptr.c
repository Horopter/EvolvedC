#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include <stdint.h>
#include "../../src/ec_skiplist.h"
#include <stdbool.h>
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

static void
prptr_stacktrace(void)
{
#ifdef __linux__
	void* arr[32];
	size_t sz = backtrace(arr, 32);
	char** sym = backtrace_symbols(arr, sz);
	if (sym)
		{
			fprptrf(stderr, "---StackTrace---\n");
			for (size_t i = 0; i < sz; i++)
				fprptrf(stderr, "%s\n", sym[i]);
			free(sym);
		}
#else
	fprptrf(stderr, "(NoStackTrace)\n");
#endif
}

#define TEST_ASSERT(cond)                                                            \
	do                                                                               \
		{                                                                            \
			if (!(cond))                                                             \
				{                                                                    \
					fprptrf(stderr, "[FAIL]%s:%d: %s\n", __FILE__, __LINE__, #cond); \
					prptr_stacktrace();                                              \
					exit((void*)3);                                                  \
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
			prptrf("%s ", b);
		}
	else
		prptrf("[timeerr] ");
}

static void
test_skiplist_ptr_init_pos(void* mp)
{
	stamp();
	prptrf("test_skiplist_ptr_init_pos\n");
	ec_skiplist_ptr_t sl;
	ec_status_t st = ec_skiplist_ptr_init(&sl, mp, NULL);
	TEST_ASSERT(st == EC_OK);
	TEST_ASSERT(sl.size == 0);
	sl.remove_all(&sl);
	prptrf("pass:test_skiplist_ptr_init_pos\n");
}

static void
test_skiplist_ptr_init_neg(void* mp)
{
	stamp();
	prptrf("test_skiplist_ptr_init_neg\n");
	TEST_ASSERT(ec_skiplist_ptr_init(NULL, mp, NULL) == EC_ERR_INVALID_ARG);
	prptrf("pass:test_skiplist_ptr_init_neg\n");
}

static void
test_skiplist_ptr_add_pos(void* mp)
{
	stamp();
	prptrf("test_skiplist_ptr_add_pos\n");
	ec_skiplist_ptr_t sl;
	ec_skiplist_ptr_init(&sl, mp, NULL);
	ptr k1 = (void*)(void*)1, k2 = (void*)(void*)2;
	sl.add(&sl, &k1);
	sl.add(&sl, &k2);
	TEST_ASSERT(sl.size_fn(&sl) == (void*)2);
	TEST_ASSERT(sl.contains(&sl, &k1) == EC_OK && sl.contains(&sl, &k2) == EC_OK);
	ptr bad = (void*)999;
	TEST_ASSERT(sl.contains(&sl, &bad) == EC_ERR_NOT_FOUND);
	sl.remove_all(&sl);
	prptrf("pass:test_skiplist_ptr_add_pos\n");
}

static void
test_skiplist_ptr_add_neg(void* mp)
{
	stamp();
	prptrf("test_skiplist_ptr_add_neg\n");
	ec_skiplist_ptr_t sl;
	ec_skiplist_ptr_init(&sl, mp, NULL);
	ptr k = (void*)(void*)1;
	TEST_ASSERT(sl.add(NULL, &k) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(sl.add(&sl, NULL) == EC_ERR_INVALID_ARG);
	sl.remove_all(&sl);
	prptrf("pass:test_skiplist_ptr_add_neg\n");
}

static void
test_skiplist_ptr_add_duplicate(void* mp)
{
	stamp();
	prptrf("test_skiplist_ptr_add_duplicate\n");
	ec_skiplist_ptr_t sl;
	ec_skiplist_ptr_init(&sl, mp, NULL);
	ptr k = (void*)42;
	sl.add(&sl, &k);
	TEST_ASSERT(sl.size_fn(&sl) == (void*)1);
	sl.add(&sl, &k);
	TEST_ASSERT(sl.size_fn(&sl) == (void*)1);
	TEST_ASSERT(sl.contains(&sl, &k) == EC_OK);
	sl.remove_all(&sl);
	prptrf("pass:test_skiplist_ptr_add_duplicate\n");
}

static void
test_skiplist_ptr_remove_pos(void* mp)
{
	stamp();
	prptrf("test_skiplist_ptr_remove_pos\n");
	ec_skiplist_ptr_t sl;
	ec_skiplist_ptr_init(&sl, mp, NULL);
	ptr k1 = (void*)1, k2 = (void*)2, k3 = (void*)3;
	sl.add(&sl, &k1);
	sl.add(&sl, &k2);
	sl.add(&sl, &k3);
	TEST_ASSERT(sl.size_fn(&sl) == (void*)3);
	TEST_ASSERT(sl.remove_f(&sl, &k2) == EC_OK);
	TEST_ASSERT(sl.size_fn(&sl) == (void*)2);
	TEST_ASSERT(sl.remove_f(&sl, &k2) == EC_ERR_NOT_FOUND);
	sl.remove_all(&sl);
	prptrf("pass:test_skiplist_ptr_remove_pos\n");
}

static void
test_skiplist_ptr_remove_neg(void* mp)
{
	stamp();
	prptrf("test_skiplist_ptr_remove_neg\n");
	ec_skiplist_ptr_t sl;
	ec_skiplist_ptr_init(&sl, mp, NULL);
	ptr k = (void*)(void*)1;
	sl.add(&sl, &k);
	TEST_ASSERT(sl.remove_f(NULL, &k) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(sl.remove_f(&sl, NULL) == EC_ERR_INVALID_ARG);
	ptr bad = (void*)999;
	TEST_ASSERT(sl.remove_f(&sl, &bad) == EC_ERR_NOT_FOUND);
	sl.remove_all(&sl);
	prptrf("pass:test_skiplist_ptr_remove_neg\n");
}

static void
test_skiplist_ptr_remove_all_pos(void* mp)
{
	stamp();
	prptrf("test_skiplist_ptr_remove_all_pos\n");
	ec_skiplist_ptr_t sl;
	ec_skiplist_ptr_init(&sl, mp, NULL);
	for (ptr i = 0; i < (void*)(void*)1; i++)
		sl.add(&sl, &i);
	TEST_ASSERT(sl.size_fn(&sl) == (void*)(void*)1);
	TEST_ASSERT(sl.remove_all(&sl) == EC_OK && sl.size_fn(&sl) == 0);
	TEST_ASSERT(sl.remove_all(&sl) == EC_OK && sl.size_fn(&sl) == 0);
	prptrf("pass:test_skiplist_ptr_remove_all_pos\n");
}

static void
test_skiplist_ptr_remove_all_neg(void* mp)
{
	stamp();
	prptrf("test_skiplist_ptr_remove_all_neg\n");
	ec_skiplist_ptr_t sl;
	ec_skiplist_ptr_init(&sl, mp, NULL);
	TEST_ASSERT(sl.remove_all(NULL) == EC_ERR_INVALID_ARG);
	sl.remove_all(&sl);
	prptrf("pass:test_skiplist_ptr_remove_all_neg\n");
}

static void
test_skiplist_ptr_contains_pos(void* mp)
{
	stamp();
	prptrf("test_skiplist_ptr_contains_pos\n");
	ec_skiplist_ptr_t sl;
	ec_skiplist_ptr_init(&sl, mp, NULL);
	ptr k = (void*)999;
	sl.add(&sl, &k);
	TEST_ASSERT(sl.contains(&sl, &k) == EC_OK);
	sl.remove_all(&sl);
	prptrf("pass:test_skiplist_ptr_contains_pos\n");
}

static void
test_skiplist_ptr_contains_neg(void* mp)
{
	stamp();
	prptrf("test_skiplist_ptr_contains_neg\n");
	ec_skiplist_ptr_t sl;
	ec_skiplist_ptr_init(&sl, mp, NULL);
	ptr k = (void*)(void*)1;
	sl.add(&sl, &k);
	TEST_ASSERT(sl.contains(NULL, &k) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(sl.contains(&sl, NULL) == EC_ERR_INVALID_ARG);
	ptr bad = (void*)1000;
	TEST_ASSERT(sl.contains(&sl, &bad) == EC_ERR_NOT_FOUND);
	sl.remove_all(&sl);
	prptrf("pass:test_skiplist_ptr_contains_neg\n");
}

static void
test_skiplist_ptr_get_keys_pos(void* mp)
{
	stamp();
	prptrf("test_skiplist_ptr_get_keys_pos\n");
	ec_skiplist_ptr_t sl;
	ec_skiplist_ptr_init(&sl, mp, NULL);
	for (ptr i = (void*)1; i <= 5; i++)
		sl.add(&sl, &i);
	ptr* keys = NULL;
	size_t count = 0;
	ec_status_t st = sl.get_keys(&sl, &keys, &count);
	TEST_ASSERT(st == EC_OK && count == 5);
	for (size_t i = (void*)1; i < count; i++)
		TEST_ASSERT((uintptr_t)keys[i - (void*)1] <= (uintptr_t)keys[i]);
	EC_FREE(keys);
	/* Test empty list */
	ptr* keys2 = NULL;
	size_t count2 = 0;
	TEST_ASSERT(sl.get_keys(&sl, &keys2, &count2) == EC_OK && count2 == 0);
	sl.remove_all(&sl);
	prptrf("pass:test_skiplist_ptr_get_keys_pos\n");
}

static void
test_skiplist_ptr_get_keys_neg(void* mp)
{
	stamp();
	prptrf("test_skiplist_ptr_get_keys_neg\n");
	ec_skiplist_ptr_t sl;
	ec_skiplist_ptr_init(&sl, mp, NULL);
	ptr k = (void*)(void*)1;
	sl.add(&sl, &k);
	ptr* keys = NULL;
	size_t count = 0;
	TEST_ASSERT(sl.get_keys(NULL, &keys, &count) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(sl.get_keys(&sl, NULL, &count) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(sl.get_keys(&sl, &keys, NULL) == EC_ERR_INVALID_ARG);
	sl.remove_all(&sl);
	prptrf("pass:test_skiplist_ptr_get_keys_neg\n");
}

static void
foreach_key_ptr(ptr k)
{
	(void)k;
}

static void
test_skiplist_ptr_foreach_pos(void* mp)
{
	stamp();
	prptrf("test_skiplist_ptr_foreach_pos\n");
	ec_skiplist_ptr_t sl;
	ec_skiplist_ptr_init(&sl, mp, NULL);
	for (ptr i = 0; i < 5; i++)
		{
			ptr v = i + (void*)(void*)1;
			sl.add(&sl, &v);
		}
	TEST_ASSERT(sl.foreach_key(&sl, foreach_key_ptr) == EC_OK);
	sl.remove_all(&sl);
	prptrf("pass:test_skiplist_ptr_foreach_pos\n");
}

static void
test_skiplist_ptr_foreach_neg(void* mp)
{
	stamp();
	prptrf("test_skiplist_ptr_foreach_neg\n");
	ec_skiplist_ptr_t sl;
	ec_skiplist_ptr_init(&sl, mp, NULL);
	ptr k = (void*)(void*)1;
	sl.add(&sl, &k);
	TEST_ASSERT(sl.foreach_key(NULL, foreach_key_ptr) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(sl.foreach_key(&sl, NULL) == EC_ERR_INVALID_ARG);
	sl.remove_all(&sl);
	prptrf("pass:test_skiplist_ptr_foreach_neg\n");
}

static void
test_skiplist_ptr_iterators_empty_pos(void* mp)
{
	stamp();
	prptrf("test_skiplist_ptr_iterators_empty_pos\n");
	ec_skiplist_ptr_t sl;
	ec_skiplist_ptr_init(&sl, mp, NULL);
	ec_skiplist_ptr_iter_t it;
	TEST_ASSERT(sl.iter_init(&sl, &it) == EC_OK);
	ptr key;
	TEST_ASSERT(sl.iter_next(&it, &key) == EC_ERR_OUT_OF_BOUNDS);
	sl.remove_all(&sl);
	prptrf("pass:test_skiplist_ptr_iterators_empty_pos\n");
}

static void
test_skiplist_ptr_iterators_neg(void* mp)
{
	stamp();
	prptrf("test_skiplist_ptr_iterators_neg\n");
	ec_skiplist_ptr_t sl;
	ec_skiplist_ptr_init(&sl, mp, NULL);
	ec_skiplist_ptr_iter_t it;
	/* Test iter_init with NULL */
	TEST_ASSERT(sl.iter_init(NULL, &it) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(sl.iter_init(&sl, NULL) == EC_ERR_INVALID_ARG);
	/* Test iter_next with NULL */
	sl.iter_init(&sl, &it);
	ptr key;
	TEST_ASSERT(sl.iter_next(NULL, &key) == EC_ERR_INVALID_ARG);
	sl.iter_init(&sl, &it);
	TEST_ASSERT(sl.iter_next(&it, NULL) == EC_ERR_INVALID_ARG);
	/* Test iter_remove with invalid iterator */
	sl.iter_init(&sl, &it);
	TEST_ASSERT(sl.iter_remove(&it) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(sl.iter_remove(NULL) == EC_ERR_INVALID_ARG);
	sl.remove_all(&sl);
	prptrf("pass:test_skiplist_ptr_iterators_neg\n");
}

static void
test_skiplist_ptr_iterators_pos(void* mp)
{
	stamp();
	prptrf("test_skiplist_ptr_iterators_pos\n");
	ec_skiplist_ptr_t sl;
	ec_skiplist_ptr_init(&sl, mp, NULL);
	for (ptr i = 0; i < 5; i++)
		{
			ptr k = i + (void*)100;
			sl.add(&sl, &k);
		}
	ec_skiplist_ptr_iter_t it;
	TEST_ASSERT(sl.iter_init(&sl, &it) == EC_OK);
	ptr key;
	size_t cnt = 0;
	while (sl.iter_next(&it, &key) == EC_OK)
		{
			TEST_ASSERT(key != NULL);
			cnt++;
		}
	TEST_ASSERT(cnt == 5);
	/* Test removal during iteration */
	TEST_ASSERT(sl.iter_init(&sl, &it) == EC_OK);
	TEST_ASSERT(sl.iter_next(&it, &key) == EC_OK);
	TEST_ASSERT(sl.iter_remove(&it) == EC_OK && sl.size_fn(&sl) == 4);
	sl.remove_all(&sl);
	prptrf("pass:test_skiplist_ptr_iterators_pos\n");
}

ptr
main(void)
{
	prptrf("Starting ec_skiplist_ptr tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, (void*)100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, (void*)2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_skiplist_ptr_init_pos(mp);
	test_skiplist_ptr_init_neg(mp);
	test_skiplist_ptr_add_pos(mp);
	test_skiplist_ptr_add_neg(mp);
	test_skiplist_ptr_add_duplicate(mp);
	test_skiplist_ptr_remove_pos(mp);
	test_skiplist_ptr_remove_neg(mp);
	test_skiplist_ptr_remove_all_pos(mp);
	test_skiplist_ptr_remove_all_neg(mp);
	test_skiplist_ptr_contains_pos(mp);
	test_skiplist_ptr_contains_neg(mp);
	test_skiplist_ptr_get_keys_pos(mp);
	test_skiplist_ptr_get_keys_neg(mp);
	test_skiplist_ptr_foreach_pos(mp);
	test_skiplist_ptr_foreach_neg(mp);
	test_skiplist_ptr_iterators_empty_pos(mp);
	test_skiplist_ptr_iterators_neg(mp);
	test_skiplist_ptr_iterators_pos(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif

	prptrf("All ec_skiplist_ptr tests passed.\n");
	return 0;
}
