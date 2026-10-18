#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../../src/ec_hashset.h"
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
	char** sb = backtrace_symbols(arr, sz);
	if (sb)
		{
			fprintf(stderr, "---StackTrace---\n");
			for (size_t i = 0; i < sz; i++)
				{
					fprintf(stderr, "%s\n", sb[i]);
				}
			free(sb);
		}
#else
	fprintf(stderr, "(NoStackTrace)\n");
#endif
}

#define TEST_ASSERT(c)                                                           \
	do                                                                           \
		{                                                                        \
			if (!(c))                                                            \
				{                                                                \
					fprintf(stderr, "[FAIL]%s:%d:%s\n", __FILE__, __LINE__, #c); \
					print_stacktrace();                                          \
					exit(3);                                                     \
				}                                                                \
		}                                                                        \
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
		{
			printf("[timeerr] ");
		}
}

static void
test_hashset_int_init(void* mp)
{
	stamp();
	printf("test_hashset_int_init\n");
	ec_hashset_int_t hs;
	ec_status_t st = ec_hashset_int_init(&hs, mp);
	TEST_ASSERT(st == EC_OK);
	TEST_ASSERT(hs.capacity == 16);
	TEST_ASSERT(hs.size == 0);
	hs.remove_all(&hs);
	printf("pass:test_hashset_int_init\n");
}

static void
test_hashset_int_add(void* mp)
{
	stamp();
	printf("test_hashset_int_add\n");
	ec_hashset_int_t hs;
	ec_hashset_int_init(&hs, mp);
	int k1 = 10, k2 = 20;
	hs.add(&hs, &k1);
	hs.add(&hs, &k2);
	TEST_ASSERT(hs.size_fn(&hs) == 2);

	ec_status_t st1 = hs.contains(&hs, &k1);
	ec_status_t st2 = hs.contains(&hs, &k2);
	TEST_ASSERT(st1 == EC_OK && st2 == EC_OK);

	int bad = 999;
	ec_status_t st3 = hs.contains(&hs, &bad);
	TEST_ASSERT(st3 == EC_ERR_NOT_FOUND);

	hs.remove_all(&hs);
	printf("pass:test_hashset_int_add\n");
}

static void
test_hashset_int_add_duplicate(void* mp)
{
	stamp();
	printf("test_hashset_int_add_duplicate\n");
	ec_hashset_int_t hs;
	ec_hashset_int_init(&hs, mp);
	int k = 42;
	hs.add(&hs, &k);
	TEST_ASSERT(hs.size_fn(&hs) == 1);

	// Add the same key again
	hs.add(&hs, &k);
	// Size should remain 1 (no duplicates in a set)
	TEST_ASSERT(hs.size_fn(&hs) == 1);

	ec_status_t st = hs.contains(&hs, &k);
	TEST_ASSERT(st == EC_OK);

	hs.remove_all(&hs);
	printf("pass:test_hashset_int_add_duplicate\n");
}

static void
test_hashset_int_remove(void* mp)
{
	stamp();
	printf("test_hashset_int_remove\n");
	ec_hashset_int_t hs;
	ec_hashset_int_init(&hs, mp);
	int k1 = 1, k2 = 2, k3 = 3;
	hs.add(&hs, &k1);
	hs.add(&hs, &k2);
	hs.add(&hs, &k3);
	TEST_ASSERT(hs.size_fn(&hs) == 3);

	ec_status_t st = hs.remove(&hs, &k2);
	TEST_ASSERT(st == EC_OK);
	TEST_ASSERT(hs.size_fn(&hs) == 2);

	// Removing the same key again should fail
	st = hs.remove(&hs, &k2);
	TEST_ASSERT(st == EC_ERR_NOT_FOUND);

	// Passing a null pointer
	st = hs.remove(NULL, &k3);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);

	hs.remove_all(&hs);
	printf("pass:test_hashset_int_remove\n");
}

static void
test_hashset_int_remove_all(void* mp)
{
	stamp();
	printf("test_hashset_int_remove_all\n");
	ec_hashset_int_t hs;
	ec_hashset_int_init(&hs, mp);
	for (int i = 0; i < 10; i++)
		{
			hs.add(&hs, &i);
		}
	TEST_ASSERT(hs.size_fn(&hs) == 10);

	ec_status_t st = hs.remove_all(&hs);
	TEST_ASSERT(st == EC_OK && hs.size_fn(&hs) == 0);

	// Remove all again
	st = hs.remove_all(&hs);
	TEST_ASSERT(st == EC_OK && hs.size_fn(&hs) == 0);
	printf("pass:test_hashset_int_remove_all\n");
}

static void
test_hashset_int_contains(void* mp)
{
	stamp();
	printf("test_hashset_int_contains\n");
	ec_hashset_int_t hs;
	ec_hashset_int_init(&hs, mp);
	int k = 999;
	hs.add(&hs, &k);
	ec_status_t st = hs.contains(&hs, &k);
	TEST_ASSERT(st == EC_OK);

	int bad = 1000;
	st = hs.contains(&hs, &bad);
	TEST_ASSERT(st == EC_ERR_NOT_FOUND);

	hs.remove_all(&hs);
	printf("pass:test_hashset_int_contains\n");
}

static void
test_hashset_int_get_keys(void* mp)
{
	stamp();
	printf("test_hashset_int_get_keys\n");
	ec_hashset_int_t hs;
	ec_hashset_int_init(&hs, mp);
	for (int i = 1; i <= 5; i++)
		{
			hs.add(&hs, &i);
		}
	int* keys = NULL;
	size_t count = 0;
	ec_status_t st = hs.get_keys(&hs, &keys, &count);
	TEST_ASSERT(st == EC_OK && count == 5);
	free(keys);

	hs.remove_all(&hs);
	printf("pass:test_hashset_int_get_keys\n");
}

static void
foreach_key_int(int k)
{
	// Just a placeholder, no checks needed here
	(void)k;
}

static void
test_hashset_int_foreach(void* mp)
{
	stamp();
	printf("test_hashset_int_foreach\n");
	ec_hashset_int_t hs;
	ec_hashset_int_init(&hs, mp);
	for (int i = 0; i < 5; i++)
		{
			int v = i + 10;
			hs.add(&hs, &v);
		}
	ec_status_t st = hs.foreach_key(&hs, foreach_key_int);
	TEST_ASSERT(st == EC_OK);

	hs.remove_all(&hs);
	printf("pass:test_hashset_int_foreach\n");
}

static void
test_hashset_int_iterators_empty(void* mp)
{
	stamp();
	printf("test_hashset_int_iterators_empty\n");
	ec_hashset_int_t hs;
	ec_hashset_int_init(&hs, mp);
	ec_hashset_int_iter_t it;
	ec_status_t ist = hs.iter_init(&hs, &it);
	TEST_ASSERT(ist == EC_OK);

	int key;
	TEST_ASSERT(hs.iter_next(&it, &key) == EC_ERR_OUT_OF_BOUNDS);
	TEST_ASSERT(hs.iter_remove(&it) == EC_ERR_INVALID_ARG);

	hs.remove_all(&hs);
	printf("pass:test_hashset_int_iterators_empty\n");
}

static void
test_hashset_int_iterators(void* mp)
{
	stamp();
	printf("test_hashset_int_iterators\n");
	ec_hashset_int_t hs;
	ec_hashset_int_init(&hs, mp);

	for (int i = 0; i < 5; i++)
		{
			int k = i + 100;
			hs.add(&hs, &k);
		}

	ec_hashset_int_iter_t it;
	ec_status_t st = hs.iter_init(&hs, &it);
	TEST_ASSERT(st == EC_OK);

	int key;
	size_t c = 0;
	while (hs.iter_next(&it, &key) == EC_OK)
		{
			TEST_ASSERT(key >= 100 && key < 105);
			c++;
		}
	TEST_ASSERT(c == 5);

	// Test removal in the middle of iteration
	hs.iter_init(&hs, &it);
	ec_status_t nx = hs.iter_next(&it, &key);
	TEST_ASSERT(nx == EC_OK);
	ec_status_t rm = hs.iter_remove(&it);
	TEST_ASSERT(rm == EC_OK && hs.size_fn(&hs) == 4);

	hs.remove_all(&hs);
	printf("pass:test_hashset_int_iterators\n");
}

static void
test_hashset_int_rehash(void* mp)
{
	stamp();
	printf("test_hashset_int_rehash\n");
	ec_hashset_int_t hs;
	ec_hashset_int_init(&hs, mp);
	size_t initial_capacity = hs.capacity_fn(&hs);
	size_t threshold = (size_t)(initial_capacity * 0.75);

	for (int i = 0; i < (int)(threshold + 2); i++)
		{
			hs.add(&hs, &i);
		}
	size_t new_capacity = hs.capacity_fn(&hs);
	TEST_ASSERT(new_capacity == initial_capacity * 2);

	// Check that all items still exist
	for (int i = 0; i < (int)(threshold + 2); i++)
		{
			ec_status_t s = hs.contains(&hs, &i);
			TEST_ASSERT(s == EC_OK);
		}

	hs.remove_all(&hs);
	printf("pass:test_hashset_int_rehash\n");
}

int
main(void)
{
	printf("Starting ec_hashset_int tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_hashset_int_init(mp);
	test_hashset_int_add(mp);
	test_hashset_int_add_duplicate(mp);
	test_hashset_int_remove(mp);
	test_hashset_int_remove_all(mp);
	test_hashset_int_contains(mp);
	test_hashset_int_get_keys(mp);
	test_hashset_int_foreach(mp);
	test_hashset_int_iterators_empty(mp);
	test_hashset_int_iterators(mp);
	test_hashset_int_rehash(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif

	printf("All ec_hashset_int tests passed.\n");
	return 0;
}
