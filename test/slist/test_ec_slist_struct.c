#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../../src/ec_slist.h"
#include <assert.h>
#include <stdbool.h>

#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

typedef struct
{
	int id;
} rec_t;
EC_SLIST_DEF(rec_t*, recp)

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
cmp_recp(const void* a, const void* b)
{
	return ((*(rec_t**)a)->id - (*(rec_t**)b)->id);
}

static bool
is_even_id_id(const void* x)
{
	return ((*(rec_t**)x)->id % 2) == 0;
}

static void
inc_recp(void* x)
{
	(*(rec_t**)x)->id++;
}

static void
add_recps(void* a, void* b, void* out)
{
	(*(rec_t**)out)->id = (*(rec_t**)a)->id + (*(rec_t**)b)->id;
}

static void*
dupe_recp(void* src)
{
	rec_t** ss = (rec_t**)src;
	rec_t* xx = (rec_t*)malloc(sizeof(rec_t));
	xx->id = (*ss)->id;
	rec_t** ret = (rec_t**)malloc(sizeof(rec_t*));
	*ret = xx;
	return ret;
}

static void
cb_recp(void* x)
{
	(void)x;
}

// Tests

void
test_slist_recp_add(void* mp)
{
	stamp();
	printf("test_slist_recp_add\n");
	ec_slist_recp_t l;
	ec_status_t s = ec_slist_recp_init(&l, mp);
	TEST_ASSERT(s == EC_OK);
	int a = 10, b = 20;
	l.add(&l, &a);
	l.add(&l, &b);
	TEST_ASSERT(l.size_fn(&l) == 2);
	l.free_fn(&l);
	printf("pass: test_slist_recp_add\n");
}

int
main(void)
{
	printf("Starting ec_slist_recp tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_slist_recp_add(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif
	printf("All ec_slist_recp tests passed.\n");
	return 0;
}
