#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../../src/ec_trie.h"
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

/*--- Tests using ec_trie_ko_t (English-based expansions) ---*/

static void
test_init(void* mp)
{
	stamp();
	printf("test_init\n");
	ec_trie_ko_t trie;
	ec_status_t s = ec_trie_ko_init(&trie, mp, NULL);
	TASSERT(s == EC_OK && trie.root == NULL);
	trie.clear(&trie);
	printf("pass:test_init\n");
}

static void
test_add_contains(void* mp)
{
	stamp();
	printf("test_add_contains\n");
	ec_trie_ko_t trie;
	ec_trie_ko_init(&trie, mp, NULL);
	const char* k1 = "hello";
	const char* k2 = "world";
	const char* bad = "nope";
	trie.add(&trie, &k1);
	trie.add(&trie, &k2);
	TASSERT(trie.contains(&trie, &k1) == EC_OK);
	TASSERT(trie.contains(&trie, &bad) == EC_ERR_NOT_FOUND);
	trie.clear(&trie);
	printf("pass:test_add_contains\n");
}

static void
test_remove(void* mp)
{
	stamp();
	printf("test_remove\n");
	ec_trie_ko_t trie;
	ec_trie_ko_init(&trie, mp, NULL);
	const char* k1 = "abc";
	const char* k2 = "abcd";
	const char* k3 = "zzz";
	trie.add(&trie, &k1);
	trie.add(&trie, &k2);
	trie.add(&trie, &k3);
	TASSERT(trie.contains(&trie, &k2) == EC_OK);
	TASSERT(trie.remove(&trie, &k2) == EC_OK);
	TASSERT(trie.contains(&trie, &k2) == EC_ERR_NOT_FOUND);
	// removing again => not found
	TASSERT(trie.remove(&trie, &k2) == EC_ERR_NOT_FOUND);
	trie.clear(&trie);
	printf("pass:test_remove\n");
}

static void
test_empty_remove_contains(void* mp)
{
	stamp();
	printf("test_empty_remove_contains\n");
	ec_trie_ko_t trie;
	ec_trie_ko_init(&trie, mp, NULL);
	const char* foo = "foo";
	TASSERT(trie.remove(&trie, &foo) == EC_ERR_NOT_FOUND);
	TASSERT(trie.contains(&trie, &foo) == EC_ERR_NOT_FOUND);
	trie.clear(&trie);
	printf("pass:test_empty_remove_contains\n");
}

static void
fstr(const char* s)
{ /* for foreach test, just no-op or print */
}
static void
test_foreach(void* mp)
{
	stamp();
	printf("test_foreach\n");
	ec_trie_ko_t trie;
	ec_trie_ko_init(&trie, mp, NULL);
	const char* s1 = "cat";
	const char* s2 = "dog";
	const char* s3 = "mouse";
	trie.add(&trie, &s1);
	trie.add(&trie, &s2);
	trie.add(&trie, &s3);
	ec_status_t st = trie.foreach (&trie, fstr);
	TASSERT(st == EC_OK);
	trie.clear(&trie);
	printf("pass:test_foreach\n");
}

static void
test_clear(void* mp)
{
	stamp();
	printf("test_clear\n");
	ec_trie_ko_t trie;
	ec_trie_ko_init(&trie, mp, NULL);
	const char* x = "somekey";
	trie.add(&trie, &x);
	TASSERT(trie.contains(&trie, &x) == EC_OK);
	trie.clear(&trie);
	TASSERT(trie.contains(&trie, &x) == EC_ERR_NOT_FOUND);
	// clearing again is a no-op
	ec_status_t st = trie.clear(&trie);
	TASSERT(st == EC_OK);
	printf("pass:test_clear\n");
}

/* Simple compare that does strcmp(*A, *B).
       Only triggers AFTER the entire path ("abc") is matched. */
int
mycmp(const char** A, const char** B)
{
	return strcmp(*A, *B); /* 0 => equal, nonzero => different */
}
static void
test_custom_compare(void* mp)
{
	stamp();
	printf("test_custom_compare\n");

	ec_trie_ko_t trie;
	ec_trie_ko_init(&trie, mp, mycmp);

	/* Insert using one pointer. */
	const char* k1 = "abc";
	trie.add(&trie, &k1);

	/* Check membership using a different pointer but the same content. */
	char k2[4] = { 'a', 'b', 'c', '\0' };
	const char* p2 = k2; /* p2 is a pointer to the string in k2 */
	TASSERT(trie.contains(&trie, &p2) == EC_OK);

	/* Clean up. */
	trie.clear(&trie);
	printf("pass:test_custom_compare\n");
}

static int
str_cmp(const char** A, const char** B)
{
	/* Compare string contents, not pointer addresses. */
	return strcmp(*A, *B);
}

static void
test_big_random(void* mp)
{
	stamp();
	printf("test_big_random\n");
	ec_trie_ko_t trie;
	/* Pass our string-compare function instead of NULL. */
	ec_trie_ko_init(&trie, mp, str_cmp);
	srand((unsigned)time(NULL));

#define MAX_LEN 8
	const int N = 500;
	char** arr = (char**)malloc(sizeof(char*) * N);

	for (int i = 0; i < N; i++)
		{
			int len = 1 + (rand() % MAX_LEN);
			arr[i] = (char*)malloc(len + 1);
			for (int j = 0; j < len; j++)
				arr[i][j] = (char)('a' + rand() % 26);
			arr[i][len] = '\0';
			trie.add(&trie, &arr[i]);
			/* We store pointer arr[i], but now membership checks will compare
			   actual string content, not pointer addresses. */
		}

	/* Check membership on random subset. Should always succeed now. */
	for (int i = 0; i < 50; i++)
		{
			int idx = rand() % N;
			ec_status_t c = trie.contains(&trie, &arr[idx]);
			TASSERT(c == EC_OK);
		}

	/* Clean up */
	for (int i = 0; i < N; i++)
		free(arr[i]);
	free(arr);

	trie.clear(&trie);
	printf("pass:test_big_random\n");
}

static void
test_add_null_args(void* mp)
{
	stamp();
	printf("test_add_null_args\n");
	ec_trie_ko_t trie;
	ec_trie_ko_init(&trie, mp, NULL);
	const char* k = "test";
	TASSERT(trie.add(NULL, &k) == EC_ERR_INVALID_ARG);
	TASSERT(trie.add(&trie, NULL) == EC_ERR_INVALID_ARG);
	trie.clear(&trie);
	printf("pass:test_add_null_args\n");
}

static void
test_contains_null_args(void* mp)
{
	stamp();
	printf("test_contains_null_args\n");
	ec_trie_ko_t trie;
	ec_trie_ko_init(&trie, mp, NULL);
	const char* k = "test";
	TASSERT(trie.contains(NULL, &k) == EC_ERR_INVALID_ARG);
	TASSERT(trie.contains(&trie, NULL) == EC_ERR_INVALID_ARG);
	trie.clear(&trie);
	printf("pass:test_contains_null_args\n");
}

static void
test_remove_null_args(void* mp)
{
	stamp();
	printf("test_remove_null_args\n");
	ec_trie_ko_t trie;
	ec_trie_ko_init(&trie, mp, NULL);
	const char* k = "test";
	TASSERT(trie.remove(NULL, &k) == EC_ERR_INVALID_ARG);
	TASSERT(trie.remove(&trie, NULL) == EC_ERR_INVALID_ARG);
	trie.clear(&trie);
	printf("pass:test_remove_null_args\n");
}

static void
test_prefix_operations(void* mp)
{
	stamp();
	printf("test_prefix_operations\n");
	ec_trie_ko_t trie;
	ec_trie_ko_init(&trie, mp, NULL);
	const char* k1 = "abc";
	const char* k2 = "abcd";
	const char* k3 = "abcde";
	trie.add(&trie, &k1);
	trie.add(&trie, &k2);
	trie.add(&trie, &k3);
	TASSERT(trie.contains(&trie, &k1) == EC_OK);
	TASSERT(trie.contains(&trie, &k2) == EC_OK);
	TASSERT(trie.contains(&trie, &k3) == EC_OK);
	// Remove middle one
	TASSERT(trie.remove(&trie, &k2) == EC_OK);
	TASSERT(trie.contains(&trie, &k1) == EC_OK);
	TASSERT(trie.contains(&trie, &k2) == EC_ERR_NOT_FOUND);
	TASSERT(trie.contains(&trie, &k3) == EC_OK);
	trie.clear(&trie);
	printf("pass:test_prefix_operations\n");
}

static void
test_empty_string(void* mp)
{
	stamp();
	printf("test_empty_string\n");
	ec_trie_ko_t trie;
	ec_trie_ko_init(&trie, mp, NULL);
	const char* empty = "";
	// Empty string should be addable
	ec_status_t s = trie.add(&trie, &empty);
	TASSERT(s == EC_OK);
	TASSERT(trie.contains(&trie, &empty) == EC_OK);
	TASSERT(trie.remove(&trie, &empty) == EC_OK);
	TASSERT(trie.contains(&trie, &empty) == EC_ERR_NOT_FOUND);
	trie.clear(&trie);
	printf("pass:test_empty_string\n");
}

static void
test_foreach_null(void* mp)
{
	stamp();
	printf("test_foreach_null\n");
	ec_trie_ko_t trie;
	ec_trie_ko_init(&trie, mp, NULL);
	const char* k = "test";
	trie.add(&trie, &k);
	// NULL trie should return error
	TASSERT(trie.foreach (NULL, fstr) == EC_ERR_INVALID_ARG);
	// NULL callback might be invalid, but let's test it
	ec_status_t s = trie.foreach (&trie, NULL);
	// Depending on implementation, this might be OK or error
	trie.clear(&trie);
	printf("pass:test_foreach_null\n");
}

int
main(void)
{
	printf("Starting ec_trie_ko tests...\n");
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
	test_add_contains(mp);
	test_remove(mp);
	test_empty_remove_contains(mp);
	test_foreach(mp);
	test_clear(mp);
	test_custom_compare(mp);
	test_big_random(mp);
	// Additional negative tests
	test_add_null_args(mp);
	test_contains_null_args(mp);
	test_remove_null_args(mp);
	test_prefix_operations(mp);
	test_empty_string(mp);
	test_foreach_null(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif
	printf("All ec_trie_ko tests passed.\n");
	return 0;
}
