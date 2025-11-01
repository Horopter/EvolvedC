#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../../src/ec_treetable.h"
#include <stdbool.h>
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

static void
stktrc(void)
{
#ifdef __linux__
	void* arr[32];
	size_t n = backtrace(arr, 32);
	char** syms = backtrace_symbols(arr, n);
	if (syms)
		{
			fprintf(stderr, "---StackTrace---\n");
			for (size_t i = 0ULL; i < n; i++)
				{
					fprintf(stderr, "%s\n", syms[i]);
				}
			free(syms);
		}
#else
	fprintf(stderr, "(NoStackTrace)\n");
#endif
}
#define TASSERT(COND)                                                               \
	do                                                                              \
		{                                                                           \
			if (!(COND))                                                            \
				{                                                                   \
					fprintf(stderr, "[FAIL]%s:%d:%s\n", __FILE__, __LINE__, #COND); \
					stktrc();                                                       \
					exit(1);                                                        \
				}                                                                   \
		}                                                                           \
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

/*--- Tests ---*/
static void
test_init(void* mp)
{
	stamp();
	printf("test_init\n");
	ec_treetable_ull_t t;
	ec_status_t s = ec_treetable_ull_init(&t, mp, NULL);
	TASSERT(s == EC_OK);
	TASSERT(t.size_fn(&t) == 0ULL);
	t.remove_all(&t);
	printf("pass:test_init\n");
}

static void
test_add_get_remove(void* mp)
{
	stamp();
	printf("test_add_get_remove\n");
	ec_treetable_ull_t t;
	ec_treetable_ull_init(&t, mp, NULL);
	unsigned long long k1 = 10ULL, v1 = 100ULL, k2 = 20ULL, v2 = 200ULL;
	t.add(&t, &k1, &v1);
	t.add(&t, &k2, &v2);
	TASSERT(t.size_fn(&t) == 2ULL);
	unsigned long long outv;
	ec_status_t s1 = t.get(&t, &k1, &outv);
	TASSERT(s1 == EC_OK && outv == 100ULL);
	unsigned long long bad = 999ULL;
	TASSERT(t.get(&t, &bad, &outv) == EC_ERR_NOT_FOUND);

	// Separate the remove & size checks
	ec_status_t rm = t.remove(&t, &k1);
	TASSERT(rm == EC_OK);
	TASSERT(t.size_fn(&t) == 1ULL);

	TASSERT(t.remove(&t, &k1) == EC_ERR_NOT_FOUND);
	t.remove_all(&t);
	printf("pass:test_add_get_remove\n");
}

static void
test_get_first_last(void* mp)
{
	stamp();
	printf("test_get_first_last\n");
	ec_treetable_ull_t t;
	ec_treetable_ull_init(&t, mp, NULL);
	unsigned long long k1 = 10ULL, v1 = 100ULL, k2 = 5ULL, v2 = 50ULL, k3 = 20ULL, v3 = 200ULL;
	t.add(&t, &k1, &v1);
	t.add(&t, &k2, &v2);
	t.add(&t, &k3, &v3);

	unsigned long long first_k, first_v, last_k, last_v;
	TASSERT(t.get_first_key(&t, &first_k) == EC_OK && first_k == 5ULL);
	TASSERT(t.get_first_value(&t, &first_v) == EC_OK && first_v == 50ULL);
	TASSERT(t.get_last_key(&t, &last_k) == EC_OK && last_k == 20ULL);
	TASSERT(t.get_last_value(&t, &last_v) == EC_OK && last_v == 200ULL);

	ec_treetable_ull_t empty;
	ec_treetable_ull_init(&empty, mp, NULL);
	TASSERT(empty.get_first_key(&empty, &first_k) == EC_ERR_NOT_FOUND);
	TASSERT(empty.get_first_value(&empty, &first_v) == EC_ERR_NOT_FOUND);
	TASSERT(empty.get_last_key(&empty, &last_k) == EC_ERR_NOT_FOUND);
	TASSERT(empty.get_last_value(&empty, &last_v) == EC_ERR_NOT_FOUND);
	empty.remove_all(&empty);
	t.remove_all(&t);
	printf("pass:test_get_first_last\n");
}

static void
test_remove_first_last(void* mp)
{
	stamp();
	printf("test_remove_first_last\n");
	ec_treetable_ull_t t;
	ec_treetable_ull_init(&t, mp, NULL);
	for (unsigned long long i = 5ULL; i < 10; i++)
		{
			unsigned long long val = i * 10;
			t.add(&t, &i, &val);
		}
	TASSERT(t.size_fn(&t) == 5ULL);
	t.remove_first(&t);
	TASSERT(t.size_fn(&t) == 4ULL);
	t.remove_last(&t);
	TASSERT(t.size_fn(&t) == 3ULL);

	ec_treetable_ull_t empty;
	ec_treetable_ull_init(&empty, mp, NULL);
	TASSERT(empty.remove_first(&empty) == EC_ERR_NOT_FOUND);
	TASSERT(empty.remove_last(&empty) == EC_ERR_NOT_FOUND);
	empty.remove_all(&empty);
	t.remove_all(&t);
	printf("pass:test_remove_first_last\n");
}

static void
test_greater_lesser(void* mp)
{
	stamp();
	printf("test_greater_lesser\n");
	ec_treetable_ull_t t;
	ec_treetable_ull_init(&t, mp, NULL);
	for (unsigned long long i = 1ULL; i <= 5ULL; i++)
		{
			unsigned long long val = i + 100;
			t.add(&t, &i, &val);
		}
	unsigned long long ask = 3ULL, kO, vO; // keys=1..5
	TASSERT(t.get_greater_than(&t, &ask, &kO, &vO) == EC_OK && kO == 4ULL && vO == 104ULL);
	TASSERT(t.get_lesser_than(&t, &ask, &kO, &vO) == EC_OK && kO == 2ULL && vO == 102ULL);
	unsigned long long big = 999ULL, smol = 0ULL;
	TASSERT(t.get_greater_than(&t, &big, &kO, &vO) == EC_ERR_NOT_FOUND);
	TASSERT(t.get_lesser_than(&t, &smol, &kO, &vO) == EC_ERR_NOT_FOUND);
	t.remove_all(&t);
	printf("pass:test_greater_lesser\n");
}

static void
test_contains(void* mp)
{
	stamp();
	printf("test_contains\n");
	ec_treetable_ull_t t;
	ec_treetable_ull_init(&t, mp, NULL);
	unsigned long long k1 = 10ULL, v1 = 100ULL;
	t.add(&t, &k1, &v1);
	TASSERT(t.contains_key(&t, &k1) == EC_OK);
	unsigned long long bad = 999ULL;
	TASSERT(t.contains_key(&t, &bad) == EC_ERR_NOT_FOUND);
	unsigned long long vCheck = 100ULL;
	TASSERT(t.contains_value(&t, &vCheck) == EC_OK);
	unsigned long long vBad = 555ULL;
	TASSERT(t.contains_value(&t, &vBad) == EC_ERR_NOT_FOUND);
	t.remove_all(&t);
	printf("pass:test_contains\n");
}

static int
cmp_desc(const unsigned long long* a, const unsigned long long* b)
{
	return (*a > *b) ? -1 : ((*a < *b) ? 1 : 0);
}

static void
test_custom_compare(void* mp)
{
	stamp();
	printf("test_custom_compare\n");
	ec_treetable_ull_t t;
	ec_treetable_ull_init(&t, mp, cmp_desc);
	for (unsigned long long i = 0ULL; i < 5; i++)
		{
			unsigned long long v = i + 100;
			t.add(&t, &i, &v);
		}
	ec_treetable_ull_iter_t it;
	unsigned long long k, v;
	t.iter_init(&t, &it);
	ec_status_t s = t.iter_next(&it, &k, &v);
	TASSERT(s == EC_OK && k == 4ULL && v == 104ULL);
	t.remove_all(&t);
	printf("pass:test_custom_compare\n");
}

static void
test_iterators(void* mp)
{
	stamp();
	printf("test_iterators\n");
	ec_treetable_ull_t t;
	ec_treetable_ull_init(&t, mp, NULL);
	for (unsigned long long i = 0ULL; i < 5; i++)
		{
			unsigned long long val = i + 100;
			t.add(&t, &i, &val);
		}
	ec_treetable_ull_iter_t it;
	t.iter_init(&t, &it);
	unsigned long long k, v;
	size_t c = 0;
	while (t.iter_next(&it, &k, &v) == EC_OK)
		{
			TASSERT(k >= 0ULL && k < 5);
			c++;
		}
	TASSERT(c == 5ULL);
	// remove in iteration
	t.iter_init(&t, &it);
	TASSERT(t.iter_next(&it, &k, &v) == EC_OK);
	TASSERT(t.iter_remove(&it) == EC_OK && t.size_fn(&t) == 4ULL);
	t.remove_all(&t);
	printf("pass:test_iterators\n");
}

static void
fkey(unsigned long long k)
{
	(void)k;
}
static void
fval(unsigned long long v)
{
	(void)v;
}

static void
test_foreach(void* mp)
{
	stamp();
	printf("test_foreach\n");
	ec_treetable_ull_t t;
	ec_treetable_ull_init(&t, mp, NULL);
	for (unsigned long long i = 10ULL; i < 15; i++)
		{
			unsigned long long vv = i * 10;
			t.add(&t, &i, &vv);
		}
	TASSERT(t.foreach_key(&t, fkey) == EC_OK);
	TASSERT(t.foreach_value(&t, fval) == EC_OK);
	t.remove_all(&t);
	printf("pass:test_foreach\n");
}

static void
test_big_random(void* mp)
{
	stamp();
	printf("test_big_random\n");
	ec_treetable_ull_t t;
	ec_treetable_ull_init(&t, mp, NULL);
	srand((unsigned)time(NULL));
	const unsigned long long N = 100ULL;
	unsigned long long* vals = (unsigned long long*)malloc(sizeof(uint32_t) * N);
	for (unsigned long long i = 0ULL; i < N; i++)
		{
			vals[i] = rand() % 5000;
			unsigned long long v = vals[i] * 10;
			t.add(&t, &vals[i], &v);
		}
	TASSERT(t.size_fn(&t) <= 100ULL);
	for (unsigned long long i = 0ULL; i < 50; i++)
		{
			unsigned long long idx = rand() % N;
			unsigned long long outv;
			ec_status_t s = t.get(&t, &vals[idx], &outv);
			TASSERT(s == EC_OK || s == EC_ERR_NOT_FOUND);
		}
	free(vals);
	t.remove_all(&t);
	printf("pass:test_big_random\n");
}

static void
test_invalid_args(void* mp)
{
	stamp();
	printf("test_invalid_args\n");
	ec_treetable_ull_t t;
	ec_treetable_ull_init(&t, mp, NULL);
	unsigned long long k = 10ULL, v = 100ULL, out_k, out_v;

	TASSERT(ec_treetable_ull_init(NULL, mp, NULL) == EC_ERR_INVALID_ARG);
	TASSERT(t.add(NULL, &k, &v) == EC_ERR_INVALID_ARG);
	TASSERT(t.add(&t, NULL, &v) == EC_ERR_INVALID_ARG);
	TASSERT(t.add(&t, &k, NULL) == EC_ERR_INVALID_ARG);
	TASSERT(t.remove(NULL, &k) == EC_ERR_INVALID_ARG);
	TASSERT(t.remove(&t, NULL) == EC_ERR_INVALID_ARG);
	TASSERT(t.remove_all(NULL) == EC_ERR_INVALID_ARG);
	TASSERT(t.remove_first(NULL) == EC_ERR_INVALID_ARG);
	TASSERT(t.remove_last(NULL) == EC_ERR_INVALID_ARG);
	TASSERT(t.get(NULL, &k, &out_v) == EC_ERR_INVALID_ARG);
	TASSERT(t.get(&t, NULL, &out_v) == EC_ERR_INVALID_ARG);
	TASSERT(t.get(&t, &k, NULL) == EC_ERR_INVALID_ARG);
	TASSERT(t.get_first_key(NULL, &out_k) == EC_ERR_INVALID_ARG);
	TASSERT(t.get_first_key(&t, NULL) == EC_ERR_INVALID_ARG);
	TASSERT(t.get_first_value(NULL, &out_v) == EC_ERR_INVALID_ARG);
	TASSERT(t.get_first_value(&t, NULL) == EC_ERR_INVALID_ARG);
	TASSERT(t.get_last_key(NULL, &out_k) == EC_ERR_INVALID_ARG);
	TASSERT(t.get_last_key(&t, NULL) == EC_ERR_INVALID_ARG);
	TASSERT(t.get_last_value(NULL, &out_v) == EC_ERR_INVALID_ARG);
	TASSERT(t.get_last_value(&t, NULL) == EC_ERR_INVALID_ARG);
	TASSERT(t.get_greater_than(NULL, &k, &out_k, &out_v) == EC_ERR_INVALID_ARG);
	TASSERT(t.get_greater_than(&t, NULL, &out_k, &out_v) == EC_ERR_INVALID_ARG);
	TASSERT(t.get_greater_than(&t, &k, NULL, &out_v) == EC_ERR_INVALID_ARG);
	TASSERT(t.get_greater_than(&t, &k, &out_k, NULL) == EC_ERR_INVALID_ARG);
	TASSERT(t.get_lesser_than(NULL, &k, &out_k, &out_v) == EC_ERR_INVALID_ARG);
	TASSERT(t.get_lesser_than(&t, NULL, &out_k, &out_v) == EC_ERR_INVALID_ARG);
	TASSERT(t.get_lesser_than(&t, &k, NULL, &out_v) == EC_ERR_INVALID_ARG);
	TASSERT(t.get_lesser_than(&t, &k, &out_k, NULL) == EC_ERR_INVALID_ARG);
	TASSERT(t.contains_key(NULL, &k) == EC_ERR_INVALID_ARG);
	TASSERT(t.contains_key(&t, NULL) == EC_ERR_INVALID_ARG);
	TASSERT(t.contains_value(NULL, &v) == EC_ERR_INVALID_ARG);
	TASSERT(t.contains_value(&t, NULL) == EC_ERR_INVALID_ARG);
	TASSERT(t.foreach_key(NULL, fkey) == EC_ERR_INVALID_ARG);
	TASSERT(t.foreach_key(&t, NULL) == EC_ERR_INVALID_ARG);
	TASSERT(t.foreach_value(NULL, fval) == EC_ERR_INVALID_ARG);
	TASSERT(t.foreach_value(&t, NULL) == EC_ERR_INVALID_ARG);

	ec_treetable_ull_iter_t it;
	TASSERT(t.iter_init(NULL, &it) == EC_ERR_INVALID_ARG);
	TASSERT(t.iter_init(&t, NULL) == EC_ERR_INVALID_ARG);
	t.iter_init(&t, &it);
	TASSERT(t.iter_next(NULL, &out_k, &out_v) == EC_ERR_INVALID_ARG);
	TASSERT(t.iter_next(&it, NULL, &out_v) == EC_ERR_INVALID_ARG);
	TASSERT(t.iter_next(&it, &out_k, NULL) == EC_ERR_INVALID_ARG);
	TASSERT(t.iter_remove(NULL) == EC_ERR_INVALID_ARG);

	TASSERT(t.size_fn(NULL) == 0ULL);

	t.remove_all(&t);
	printf("pass:test_invalid_args\n");
}

int
main(void)
{
	printf("Starting ec_treetable_unsigned long long tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t c = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t p;
	ec_status_t st = ec_mempool_init(&p, &c);
	TASSERT(st == EC_OK);
	void* mp = &p;
#else
	void* mp = NULL;
#endif
	test_init(mp);
	test_add_get_remove(mp);
	test_get_first_last(mp);
	test_remove_first_last(mp);
	test_greater_lesser(mp);
	test_contains(mp);
	test_custom_compare(mp);
	test_iterators(mp);
	test_foreach(mp);
	test_big_random(mp);
	test_invalid_args(mp);
#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif
	printf("All ec_treetable_unsigned long long tests passed.\n");
	return 0;
}
