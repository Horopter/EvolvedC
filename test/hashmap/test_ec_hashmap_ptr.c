#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include <stdint.h>
#include "../../src/ec_hashmap.h"
#include <stdbool.h>
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

EC_HASHMAP_DEF(void*, ptr)

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
test_hashmap_ptr_init(void* mp)
{
	stamp();
	printf("test_hashmap_ptr_init\n");
	ec_hashmap_ptr_t hm;
	ec_status_t st = ec_hashmap_ptr_init(&hm, mp);
	TEST_ASSERT(st == EC_OK);
	TEST_ASSERT(hm.capacity == 16);
	TEST_ASSERT(hm.size == 0);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ptr_init\n");
}
static void
test_hashmap_ptr_add_get(void* mp)
{
	stamp();
	printf("test_hashmap_ptr_add_get\n");
	ec_hashmap_ptr_t hm;
	ec_hashmap_ptr_init(&hm, mp);
	int a = 1, b = 2, c = 3;
	int va = 11, vb = 22, vc = 33;
	void *ka = &a, *kb = &b, *kc = &c;
	void *va_p = &va, *vb_p = &vb, *vc_p = &vc;
	hm.add(&hm, &ka, &va_p);
	hm.add(&hm, &kb, &vb_p);
	hm.add(&hm, &kc, &vc_p);
	void** out;
	ec_status_t s1 = hm.get(&hm, &ka, &out);
	TEST_ASSERT(s1 == EC_OK);
	TEST_ASSERT(*out == va_p);
	free(out);
	ec_status_t s2 = hm.get(&hm, &kb, &out);
	TEST_ASSERT(s2 == EC_OK);
	TEST_ASSERT(*out == vb_p);
	free(out);
	int bad = 999;
	void* bad_p = &bad;
	ec_status_t s3 = hm.get(&hm, &bad_p, &out);
	TEST_ASSERT(s3 == EC_ERR_NOT_FOUND);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ptr_add_get\n");
}
static void
test_hashmap_ptr_duplicate_keys(void* mp)
{
	stamp();
	printf("test_hashmap_ptr_duplicate_keys\n");
	ec_hashmap_ptr_t hm;
	ec_hashmap_ptr_init(&hm, mp);
	int x = 1, y = 10, z = 99;
	void* k = &x;
	void *v1 = &y, *v2 = &z;
	hm.add(&hm, &k, &v1);
	void** out;
	ec_status_t s = hm.get(&hm, &k, &out);
	TEST_ASSERT(s == EC_OK && *out == v1);
	free(out);
	hm.add(&hm, &k, &v2);
	s = hm.get(&hm, &k, &out);
	TEST_ASSERT(s == EC_OK && *out == v2);
	free(out);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ptr_duplicate_keys\n");
}
static void
test_hashmap_ptr_remove(void* mp)
{
	stamp();
	printf("test_hashmap_ptr_remove\n");
	ec_hashmap_ptr_t hm;
	ec_hashmap_ptr_init(&hm, mp);
	int a = 1, b = 2, c = 3;
	int va = 11, vb = 22, vc = 33;
	void *ka = &a, *kb = &b, *kc = &c;
	void *va_p = &va, *vb_p = &vb, *vc_p = &vc;
	hm.add(&hm, &ka, &va_p);
	hm.add(&hm, &kb, &vb_p);
	hm.add(&hm, &kc, &vc_p);
	ec_status_t st = hm.remove(&hm, &kb);
	TEST_ASSERT(st == EC_OK);
	TEST_ASSERT(hm.size_fn(&hm) == 2);
	st = hm.remove(&hm, &kb);
	TEST_ASSERT(st == EC_ERR_NOT_FOUND);
	st = hm.remove(NULL, &kc);
	TEST_ASSERT(st == EC_ERR_INVALID_ARG);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ptr_remove\n");
}
static void
test_hashmap_ptr_remove_all(void* mp)
{
	stamp();
	printf("test_hashmap_ptr_remove_all\n");
	ec_hashmap_ptr_t hm;
	ec_hashmap_ptr_init(&hm, mp);
	int arr[10];
	void* keys[10];
	void* values[10];
	for (int i = 0; i < 10; i++)
		{
			arr[i] = i;
			keys[i] = &arr[i];
			values[i] = &arr[i];
			hm.add(&hm, &keys[i], &values[i]);
		}
	TEST_ASSERT(hm.size_fn(&hm) == 10);
	ec_status_t ec_status_t st = hm.remove_all(&hm);
	TEST_ASSERT(st == EC_OK && hm.size_fn(&hm) == 0);
	st = hm.remove_all(&hm);
	TEST_ASSERT(st == EC_OK && hm.size_fn(&hm) == 0);
	printf("pass:test_hashmap_ptr_remove_all\n");
}
static void
test_hashmap_ptr_contains_key(void* mp)
{
	stamp();
	printf("test_hashmap_ptr_contains_key\n");
	ec_hashmap_ptr_t hm;
	ec_hashmap_ptr_init(&hm, mp);
	int x = 42, y = 999;
	void* k = &x;
	void* v = &y;
	hm.add(&hm, &k, &v);
	ec_status_t st = hm.contains_key(&hm, &k);
	TEST_ASSERT(st == EC_OK);
	int bad_val = 9999;
	void* bad_key = &bad_val;
	st = hm.contains_key(&hm, &bad_key);
	TEST_ASSERT(st == EC_ERR_NOT_FOUND);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ptr_contains_key\n");
}
static void
test_hashmap_ptr_get_keys_values(void* mp)
{
	stamp();
	printf("test_hashmap_ptr_get_keys_values\n");
	ec_hashmap_ptr_t hm;
	ec_hashmap_ptr_init(&hm, mp);
	int arr[5];
	void* keys[5];
	void* values[5];
	for (int i = 0; i < 5; i++)
		{
			arr[i] = i;
			keys[i] = &arr[i];
			values[i] = &arr[i];
			hm.add(&hm, &keys[i], &values[i]);
		}
	void** ks = NULL;
	size_t kc = 0;
	ec_status_t stK = hm.get_keys(&hm, &ks, &kc);
	TEST_ASSERT(stK == EC_OK && kc == 5);
	free(ks);
	void** vs = NULL;
	size_t vc = 0;
	ec_status_t stV = hm.get_values(&hm, &vs, &vc);
	TEST_ASSERT(stV == EC_OK && vc == 5);
	free(vs);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ptr_get_keys_values\n");
}
static void
foreach_key_ptr(void* k)
{
	(void)k;
}
static void
foreach_val_ptr(void* v)
{
	(void)v;
}
static void
test_hashmap_ptr_foreach(void* mp)
{
	stamp();
	printf("test_hashmap_ptr_foreach\n");
	ec_hashmap_ptr_t hm;
	ec_hashmap_ptr_init(&hm, mp);
	int arr[5];
	void* keys[5];
	void* values[5];
	for (int i = 0; i < 5; i++)
		{
			arr[i] = i;
			keys[i] = &arr[i];
			values[i] = &arr[i];
			hm.add(&hm, &keys[i], &values[i]);
		}
	TEST_ASSERT(hm.foreach_key(&hm, foreach_key_ptr) == EC_OK);
	TEST_ASSERT(hm.foreach_value(&hm, foreach_val_ptr) == EC_OK);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ptr_foreach\n");
}
static void
test_hashmap_ptr_iterators_empty(void* mp)
{
	stamp();
	printf("test_hashmap_ptr_iterators_empty\n");
	ec_hashmap_ptr_t hm;
	ec_hashmap_ptr_init(&hm, mp);
	ec_hashmap_ptr_iter_t it;
	ec_status_t ist = hm.iter_init(&hm, &it);
	TEST_ASSERT(ist == EC_OK);
	void *k, *v;
	TEST_ASSERT(hm.iter_next(&it, &k, &v) == EC_ERR_OUT_OF_BOUNDS);
	TEST_ASSERT(hm.iter_remove(&it) == EC_ERR_INVALID_ARG);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ptr_iterators_empty\n");
}
static void
test_hashmap_ptr_iterators(void* mp)
{
	stamp();
	printf("test_hashmap_ptr_iterators\n");
	ec_hashmap_ptr_t hm;
	ec_hashmap_ptr_init(&hm, mp);
	int arr[5];
	void* keys[5];
	void* values[5];
	for (int i = 0; i < 5; i++)
		{
			arr[i] = i + 100;
			keys[i] = &arr[i];
			values[i] = &arr[i];
			hm.add(&hm, &keys[i], &values[i]);
		}
	ec_hashmap_ptr_iter_t it;
	ec_status_t st = hm.iter_init(&hm, &it);
	TEST_ASSERT(st == EC_OK);
	void *key, *val;
	size_t c = 0;
	while (hm.iter_next(&it, &key, &val) == EC_OK)
		{
			c++;
		}
	TEST_ASSERT(c == 5);
	hm.iter_init(&hm, &it);
	ec_status_t nx = hm.iter_next(&it, &key, &val);
	TEST_ASSERT(nx == EC_OK);
	ec_status_t rm = hm.iter_remove(&it);
	TEST_ASSERT(rm == EC_OK && hm.size_fn(&hm) == 4);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ptr_iterators\n");
}
static void
test_hashmap_ptr_rehash(void* mp)
{
	stamp();
	printf("test_hashmap_ptr_rehash\n");
	ec_hashmap_ptr_t hm;
	ec_hashmap_ptr_init(&hm, mp);
	size_t ic = hm.capacity_fn(&hm);
	size_t th = (size_t)(ic * 0.75);
	int* arr = malloc(sizeof(int) * (th + 2));
	void** keys = malloc(sizeof(void*) * (th + 2));
	void** values = malloc(sizeof(void*) * (th + 2));
	for (int i = 0; i < (int)(th + 2); i++)
		{
			arr[i] = i;
			keys[i] = &arr[i];
			values[i] = &arr[i];
			hm.add(&hm, &keys[i], &values[i]);
		}
	size_t nc = hm.capacity_fn(&hm);
	TEST_ASSERT(nc == ic * 2);
	for (int i = 0; i < (int)(th + 2); i++)
		{
			void** kval = NULL;
			ec_status_t s = hm.get(&hm, &keys[i], &kval);
			TEST_ASSERT(s == EC_OK && *kval == values[i]);
			if (kval)
				free(kval);
		}
	free(arr);
	free(keys);
	free(values);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ptr_rehash\n");
}
static void
test_hashmap_ptr_invalid_args(void* mp)
{
	stamp();
	printf("test_hashmap_ptr_invalid_args\n");
	ec_hashmap_ptr_t hm;
	ec_hashmap_ptr_init(&hm, mp);
	int x = 1;
	void* k = &x;
	void* v = &x;
	TEST_ASSERT(ec_hashmap_ptr_init(NULL, mp) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.add(NULL, &k, &v) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.add(&hm, NULL, &v) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.add(&hm, &k, NULL) == EC_ERR_INVALID_ARG);
	void** out;
	TEST_ASSERT(hm.get(NULL, &k, &out) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.get(&hm, NULL, &out) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.get(&hm, &k, NULL) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.remove(NULL, &k) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.remove(&hm, NULL) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.contains_key(NULL, &k) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.contains_key(&hm, NULL) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.remove_all(NULL) == EC_ERR_INVALID_ARG);
	void** keys = NULL;
	size_t count = 0;
	TEST_ASSERT(hm.get_keys(NULL, &keys, &count) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.get_keys(&hm, NULL, &count) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.get_keys(&hm, &keys, NULL) == EC_ERR_INVALID_ARG);
	void** values = NULL;
	TEST_ASSERT(hm.get_values(NULL, &values, &count) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.get_values(&hm, NULL, &count) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.get_values(&hm, &values, NULL) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.foreach_key(NULL, foreach_key_ptr) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.foreach_key(&hm, NULL) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.foreach_value(NULL, foreach_val_ptr) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.foreach_value(&hm, NULL) == EC_ERR_INVALID_ARG);
	ec_hashmap_ptr_iter_t it;
	TEST_ASSERT(hm.iter_init(NULL, &it) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.iter_init(&hm, NULL) == EC_ERR_INVALID_ARG);
	void *key, *val;
	TEST_ASSERT(hm.iter_next(NULL, &key, &val) == EC_ERR_INVALID_ARG);
	hm.iter_init(&hm, &it);
	TEST_ASSERT(hm.iter_next(&it, NULL, &val) == EC_ERR_INVALID_ARG);
	hm.iter_init(&hm, &it);
	TEST_ASSERT(hm.iter_next(&it, &key, NULL) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(hm.iter_remove(NULL) == EC_ERR_INVALID_ARG);
	hm.remove_all(&hm);
	printf("pass:test_hashmap_ptr_invalid_args\n");
}
int
main(void)
{
	printf("Starting ec_hashmap_ptr tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif
	test_hashmap_ptr_init(mp);
	test_hashmap_ptr_add_get(mp);
	test_hashmap_ptr_duplicate_keys(mp);
	test_hashmap_ptr_remove(mp);
	test_hashmap_ptr_remove_all(mp);
	test_hashmap_ptr_contains_key(mp);
	test_hashmap_ptr_get_keys_values(mp);
	test_hashmap_ptr_foreach(mp);
	test_hashmap_ptr_iterators_empty(mp);
	test_hashmap_ptr_iterators(mp);
	test_hashmap_ptr_rehash(mp);
	test_hashmap_ptr_invalid_args(mp);
#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif
	printf("All ec_hashmap_ptr tests passed.\n");
	return 0;
}
