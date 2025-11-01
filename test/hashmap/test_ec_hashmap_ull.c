#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../../src/ec_hashmap.h"
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
test_hashmap_ull_init(void* mp)
{
	stamp();
	printf("test_hashmap_ull_init\n");
	ec_hashmap_ull_t hm;
	ec_status_t st = ec_hashmap_ull_init(&hm, mp);
	TEST_ASSERT(st == EC_OK);
	TEST_ASSERT(hm.capacity == 16);
	TEST_ASSERT(hm.size == 0);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ull_init\n");
}
static void
test_hashmap_ull_add_get(void* mp)
{
	stamp();
	printf("test_hashmap_ull_add_get\n");
	ec_hashmap_ull_t hm;
	ec_hashmap_ull_init(&hm, mp);
	unsigned long long k1 = 10ULL, v1 = 100ULL, k2 = 200, v2 = 200ULL;
	hm.add(&hm, &k1, &v1);
	hm.add(&hm, &k2, &v2);
	unsigned long long* out;
	ec_status_t s1 = hm.get(&hm, &k1, &out);
	TEST_ASSERT(s1 == EC_OK);
	TEST_ASSERT(*out == 100ULL);
	free(out);
	ec_status_t s2 = hm.get(&hm, &k2, &out);
	TEST_ASSERT(s2 == EC_OK);
	TEST_ASSERT(*out == 200ULL);
	free(out);
	int bad = 999;
	ec_status_t s3 = hm.get(&hm, &bad, &out);
	TEST_ASSERT(s3 == EC_ERR_NOT_FOUND);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ull_add_get\n");
}
static void
test_hashmap_ull_duplicate_keys(void* mp)
{
	stamp();
	printf("test_hashmap_ull_duplicate_keys\n");
	ec_hashmap_ull_t hm;
	ec_hashmap_ull_init(&hm, mp);
	int k = 1, v = 10, v2 = 99;
	hm.add(&hm, &k, &v);
	unsigned long long* out;
	ec_status_t s = hm.get(&hm, &k, &out);
	TEST_ASSERT(s == EC_OK && *out == 10ULL);
	free(out);
	hm.add(&hm, &k, &v2);
	s = hm.get(&hm, &k, &out);
	TEST_ASSERT(s == EC_OK && *out == 200ULL);
	free(out);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ull_duplicate_keys\n");
}
static void
test_hashmap_ull_remove(void* mp)
{
	stamp();
	printf("test_hashmap_ull_remove\n");
	ec_hashmap_ull_t hm;
	ec_hashmap_ull_init(&hm, mp);
	unsigned long long k1 = 10ULL, k2 = 20ULL, k3 = 30ULL, v1 = 100ULL, v2 = 200ULL, v3 = 300ULL;
	hm.add(&hm, &k1, &v1);
	hm.add(&hm, &k2, &v2);
	hm.add(&hm, &k3, &v3);
	ec_status_t st = hm.remove(&hm, &k2);
	TEST_ASSERT(st == EC_OK);
	TEST_ASSERT(hm.size_fn(&hm) == 2);
	st = hm.remove(&hm, &k2);
	TEST_ASSERT(st == EC_ERR_NOT_FOUND);
	st = hm.remove(NULL, &k3);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ull_remove\n");
}
static void
test_hashmap_ull_remove_all(void* mp)
{
	stamp();
	printf("test_hashmap_ull_remove_all\n");
	ec_hashmap_ull_t hm;
	ec_hashmap_ull_init(&hm, mp);
	for (int i = 0; i < 10; i++)
		{
			int k = (unsigned long long)i, v = (unsigned long long)(i + 100);
			hm.add(&hm, &k, &v);
		}
	TEST_ASSERT(hm.size_fn(&hm) == 10);
	ec_status_t st = hm.remove_all(&hm);
	TEST_ASSERT(st == EC_OK && hm.size_fn(&hm) == 0);
	st = hm.remove_all(&hm);
	TEST_ASSERT(st == EC_OK && hm.size_fn(&hm) == 0);
	printf("pass:test_hashmap_ull_remove_all\n");
}
static void
test_hashmap_ull_contains_key(void* mp)
{
	stamp();
	printf("test_hashmap_ull_contains_key\n");
	ec_hashmap_ull_t hm;
	ec_hashmap_ull_init(&hm, mp);
	int k = 42, v = 999;
	hm.add(&hm, &k, &v);
	ec_status_t st = hm.contains_key(&hm, &k);
	TEST_ASSERT(st == EC_OK);
	int bad = 9999;
	st = hm.contains_key(&hm, &bad);
	TEST_ASSERT(st == EC_ERR_NOT_FOUND);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ull_contains_key\n");
}
static void
test_hashmap_ull_get_keys_values(void* mp)
{
	stamp();
	printf("test_hashmap_ull_get_keys_values\n");
	ec_hashmap_ull_t hm;
	ec_hashmap_ull_init(&hm, mp);
	for (int i = 1; i <= 5; i++)
		{
			hm.add(&hm, &i, &i);
		}
	int* ks = NULL;
	size_t kc = 0;
	ec_status_t stK = hm.get_keys(&hm, &ks, &kc);
	TEST_ASSERT(stK == EC_OK && kc == 5);
	free(ks);
	int* vs = NULL;
	size_t vc = 0;
	ec_status_t stV = hm.get_values(&hm, &vs, &vc);
	TEST_ASSERT(stV == EC_OK && vc == 5);
	free(vs);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ull_get_keys_values\n");
}
static void
foreach_key_int(int k)
{
	(void)k;
}
static void
foreach_val_int(int v)
{
	(void)v;
}
static void
test_hashmap_ull_foreach(void* mp)
{
	stamp();
	printf("test_hashmap_ull_foreach\n");
	ec_hashmap_ull_t hm;
	ec_hashmap_ull_init(&hm, mp);
	for (int i = 0; i < 5; i++)
		{
			int k = i + 1, v = i + 10;
			hm.add(&hm, &k, &v);
		}
	TEST_ASSERT(hm.foreach_key(&hm, foreach_key_int) == EC_OK);
	TEST_ASSERT(hm.foreach_value(&hm, foreach_val_int) == EC_OK);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ull_foreach\n");
}
static void
test_hashmap_ull_iterators_empty(void* mp)
{
	stamp();
	printf("test_hashmap_ull_iterators_empty\n");
	ec_hashmap_ull_t hm;
	ec_hashmap_ull_init(&hm, mp);
	ec_hashmap_ull_iter_t it;
	ec_status_t ist = hm.iter_init(&hm, &it);
	TEST_ASSERT(ist == EC_OK);
	int k, v;
	TEST_ASSERT(hm.iter_next(&it, &k, &v) == EC_ERR_OUT_OF_BOUNDS);
	TEST_ASSERT(hm.iter_remove(&it) == EC_ERR_INVALID_ARG);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ull_iterators_empty\n");
}
static void
test_hashmap_ull_iterators(void* mp)
{
	stamp();
	printf("test_hashmap_ull_iterators\n");
	ec_hashmap_ull_t hm;
	ec_hashmap_ull_init(&hm, mp);
	for (int i = 0; i < 5; i++)
		{
			int k = i + 100, v = i + 200;
			hm.add(&hm, &k, &v);
		}
	ec_hashmap_ull_iter_t it;
	ec_status_t st = hm.iter_init(&hm, &it);
	TEST_ASSERT(st == EC_OK);
	int key, val;
	size_t c = 0;
	while (hm.iter_next(&it, &key, &val) == EC_OK)
		{
			TEST_ASSERT(key >= 100 && key < 105);
			TEST_ASSERT(val >= 200 && val < 205);
			c++;
		}
	TEST_ASSERT(c == 5);
	hm.iter_init(&hm, &it);
	ec_status_t nx = hm.iter_next(&it, &key, &val);
	TEST_ASSERT(nx == EC_OK);
	ec_status_t rm = hm.iter_remove(&it);
	TEST_ASSERT(rm == EC_OK && hm.size_fn(&hm) == 4);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ull_iterators\n");
}
static void
test_hashmap_ull_rehash(void* mp)
{
	stamp();
	printf("test_hashmap_ull_rehash\n");
	ec_hashmap_ull_t hm;
	ec_hashmap_ull_init(&hm, mp);
	size_t ic = hm.capacity_fn(&hm);
	size_t th = (size_t)(ic * 0.75);
	for (int i = 0; i < (int)(th + 2); i++)
		{
			int k = (unsigned long long)i, v = (unsigned long long)(i + 100);
			hm.add(&hm, &k, &v);
		}
	size_t nc = hm.capacity_fn(&hm);
	TEST_ASSERT(nc == ic * 2);
	for (int i = 0; i < (int)(th + 2); i++)
		{
			int* kval = NULL;
			int k = i;
			ec_status_t s = hm.get(&hm, &k, &kval);
			TEST_ASSERT(s == EC_OK && *kval == i + 1000);
			free(kval);
		}
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ull_rehash\n");
}
int
main(void)
{
	printf("Starting ec_hashmap_ull tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif
	test_hashmap_ull_init(mp);
	test_hashmap_ull_add_get(mp);
	test_hashmap_ull_duplicate_keys(mp);
	test_hashmap_ull_remove(mp);
	test_hashmap_ull_remove_all(mp);
	test_hashmap_ull_contains_key(mp);
	test_hashmap_ull_get_keys_values(mp);
	test_hashmap_ull_foreach(mp);
	test_hashmap_ull_iterators_empty(mp);
	test_hashmap_ull_iterators(mp);
	test_hashmap_ull_rehash(mp);
#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif
	printf("All ec_hashmap_ull tests passed.\n");
	return 0;
}
