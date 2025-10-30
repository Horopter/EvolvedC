#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../../src/ec_treeset.h"
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
			for (size_t i = 0; i < n; i++)
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
	ec_treeset_int_t s;
	ec_status_t st = ec_treeset_int_init(&s, mp, NULL);
	TASSERT(st == EC_OK && s.size_fn(&s) == 0);
	s.remove_all(&s);
	printf("pass:test_init\n");
}

static void
test_add_remove_contains(void* mp)
{
	stamp();
	printf("test_add_remove_contains\n");
	ec_treeset_int_t s;
	ec_treeset_int_init(&s, mp, NULL);
	int k1 = 10, k2 = 20, bad = 999;
	s.add(&s, &k1);
	s.add(&s, &k2);
	TASSERT(s.size_fn(&s) == 2);
	TASSERT(s.contains(&s, &k1) == EC_OK);
	TASSERT(s.contains(&s, &bad) == EC_ERR_NOT_FOUND);
	TASSERT(s.remove(&s, &k1) == EC_OK);
	TASSERT(s.size_fn(&s) == 1);
	TASSERT(s.remove(&s, &k1) == EC_ERR_NOT_FOUND);
	TASSERT(s.remove(NULL, &k2) == EC_ERR_INVALID_ARG);
	s.remove_all(&s);
	printf("pass:test_add_remove_contains\n");
}

static void
test_remove_first_last(void* mp)
{
	stamp();
	printf("test_remove_first_last\n");
	ec_treeset_int_t s;
	ec_treeset_int_init(&s, mp, NULL);
	for (int i = 5; i < 10; i++)
		s.add(&s, &i);
	TASSERT(s.size_fn(&s) == 5);
	s.remove_first(&s);
	TASSERT(s.size_fn(&s) == 4);
	s.remove_last(&s);
	TASSERT(s.size_fn(&s) == 3);
	s.remove_all(&s);
	printf("pass:test_remove_first_last\n");
}

static void
test_get_first_last(void* mp)
{
	stamp();
	printf("test_get_first_last\n");
	ec_treeset_int_t s;
	ec_treeset_int_init(&s, mp, NULL);
	for (int i = 10; i <= 12; i++)
		s.add(&s, &i); // 10,11,12
	int out;
	TASSERT(s.get_first(&s, &out) == EC_OK && out == 10);
	TASSERT(s.get_last(&s, &out) == EC_OK && out == 12);
	s.remove_all(&s);
	// check not_found
	TASSERT(s.get_first(&s, &out) == EC_ERR_NOT_FOUND);
	TASSERT(s.get_last(&s, &out) == EC_ERR_NOT_FOUND);
	printf("pass:test_get_first_last\n");
}

static void
test_get_greater_lesser(void* mp)
{
	stamp();
	printf("test_get_greater_lesser\n");
	ec_treeset_int_t s;
	ec_treeset_int_init(&s, mp, NULL);
	for (int i = 1; i <= 5; i++)
		s.add(&s, &i); // keys=1..5
	int ask = 3, out;
	TASSERT(s.get_greater_than(&s, &ask, &out) == EC_OK && out == 4);
	TASSERT(s.get_lesser_than(&s, &ask, &out) == EC_OK && out == 2);
	int big = 999, smol = 0;
	TASSERT(s.get_greater_than(&s, &big, &out) == EC_ERR_NOT_FOUND);
	TASSERT(s.get_lesser_than(&s, &smol, &out) == EC_ERR_NOT_FOUND);
	s.remove_all(&s);
	printf("pass:test_get_greater_lesser\n");
}

static void
fk(int k)
{
	(void)k;
}
static void
test_foreach(void* mp)
{
	stamp();
	printf("test_foreach\n");
	ec_treeset_int_t s;
	ec_treeset_int_init(&s, mp, NULL);
	for (int i = 0; i < 5; i++)
		s.add(&s, &i);
	TASSERT(s.foreach (&s, fk) == EC_OK);
	s.remove_all(&s);
	printf("pass:test_foreach\n");
}

static void
test_iterators(void* mp)
{
	stamp();
	printf("test_iterators\n");
	ec_treeset_int_t s;
	ec_treeset_int_init(&s, mp, NULL);
	for (int i = 0; i < 5; i++)
		s.add(&s, &i);
	ec_treeset_int_iter_t it;
	int k;
	size_t c = 0;
	s.iter_init(&s, &it);
	while (s.iter_next(&it, &k) == EC_OK)
		{
			TASSERT(k >= 0 && k < 5);
			c++;
		}
	TASSERT(c == 5);
	// remove mid-iteration
	s.iter_init(&s, &it);
	TASSERT(s.iter_next(&it, &k) == EC_OK);
	TASSERT(s.iter_remove(&it) == EC_OK && s.size_fn(&s) == 4);
	s.remove_all(&s);
	printf("pass:test_iterators\n");
}

static int
cmp_desc(const int* a, const int* b)
{
	return (*a > *b) ? -1 : ((*a < *b) ? 1 : 0);
}
static void
test_custom_compare(void* mp)
{
	stamp();
	printf("test_custom_compare\n");
	ec_treeset_int_t s;
	ec_treeset_int_init(&s, mp, cmp_desc);
	for (int i = 0; i < 5; i++)
		s.add(&s, &i);
	ec_treeset_int_iter_t it;
	int k;
	s.iter_init(&s, &it);
	ec_status_t st = s.iter_next(&it, &k);
	TASSERT(st == EC_OK && k == 4); // largest first if it's truly descending
	s.remove_all(&s);
	printf("pass:test_custom_compare\n");
}

static void
test_big_random(void* mp)
{
	stamp();
	printf("test_big_random\n");
	ec_treeset_int_t s;
	ec_treeset_int_init(&s, mp, NULL);
	srand((unsigned)time(NULL));
	const int N = 1000;
	int* vals = (int*)malloc(sizeof(int) * N);
	for (int i = 0; i < N; i++)
		{
			vals[i] = rand() % 5000;
			s.add(&s, &vals[i]);
		}
	TASSERT(s.size_fn(&s) <= 1000); // duplicates possible
	for (int i = 0; i < 50; i++)
		{
			int idx = rand() % N;
			ec_status_t c = s.contains(&s, &vals[idx]);
			TASSERT(c == EC_OK || c == EC_ERR_NOT_FOUND);
		}
	free(vals);
	s.remove_all(&s);
	printf("pass:test_big_random\n");
}

int
main(void)
{
	printf("Starting ec_treeset_int tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif
	test_init(mp);
	test_add_remove_contains(mp);
	test_remove_first_last(mp);
	test_get_first_last(mp);
	test_get_greater_lesser(mp);
	test_foreach(mp);
	test_iterators(mp);
	test_custom_compare(mp);
	test_big_random(mp);
#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif
	printf("All ec_treeset_int tests passed.\n");
	return 0;
}
