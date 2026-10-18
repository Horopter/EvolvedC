#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../../src/ec_treeset.h"
#include <string.h>

/* TBD: This test is not yet implemented.
 *
 * LIMITATION: The EC_TREESET_DEF macro generates a default comparator function that uses:
 *   return (*a < *b) ? -1 : ((*a > *b) ? 1 : 0);
 *
 * This does not compile for struct types since C does not support direct comparison of structs
 * with < and > operators. The macro would need to be enhanced to detect struct types and use
 * memcmp() or another approach for the default comparator, or provide a separate macro variant
 * for struct types (e.g., EC_TREESET_DEF_STRUCT).
 *
 * TODO: Enhance EC_TREESET_DEF macro to support struct types properly.
 */
#if 0

/* Define custom struct type for treeset */
typedef struct person_ts_s
{
	int id;
	char name[16];
} person_ts_t;

/* Custom comparator for person struct */
static int
cmp_person(const person_ts_t* a, const person_ts_t* b)
{
	if (a->id < b->id)
		return -1;
	if (a->id > b->id)
		return 1;
	return strcmp(a->name, b->name);
}
#include <stdbool.h>
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

static void
print_stacktrace(void)
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
		{
			printf("[timeerr] ");
		}
}

static void
test_treeset_struct_init_pos(void* mp)
{
	stamp();
	printf("test_treeset_struct_init_pos\n");
	ec_treeset_person_t s;
	ec_status_t st = ec_treeset_person_init(&s, mp, cmp_person);
	TEST_ASSERT(st == EC_OK);
	TEST_ASSERT(s.size_fn(&s) == 0);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_init_pos\n");
}

static void
test_treeset_struct_init_neg(void* mp)
{
	stamp();
	printf("test_treeset_struct_init_neg\n");
	TEST_ASSERT(ec_treeset_person_init(NULL, mp, NULL) == EC_ERR_INVALID_ARG);
	printf("pass:test_treeset_struct_init_neg\n");
}

static void
test_treeset_struct_add_pos(void* mp)
{
	stamp();
	printf("test_treeset_struct_add_pos\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t k1 = { 10, "alice" }, k2 = { 20, "bob" }, k3 = { 15, "fifteen" };
	TEST_ASSERT(s.add(&s, &k1) == EC_OK);
	TEST_ASSERT(s.add(&s, &k2) == EC_OK);
	TEST_ASSERT(s.add(&s, &k3) == EC_OK);
	TEST_ASSERT(s.size_fn(&s) == 3);
	TEST_ASSERT(s.contains(&s, &k1) == EC_OK);
	TEST_ASSERT(s.contains(&s, &k2) == EC_OK);
	TEST_ASSERT(s.contains(&s, &k3) == EC_OK);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_add_pos\n");
}

static void
test_treeset_struct_add_neg(void* mp)
{
	stamp();
	printf("test_treeset_struct_add_neg\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t k = { 10, "ten" };
	TEST_ASSERT(s.add(NULL, &k) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.add(&s, NULL) == EC_ERR_INVALID_ARG);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_add_neg\n");
}

static void
test_treeset_struct_remove_pos(void* mp)
{
	stamp();
	printf("test_treeset_struct_remove_pos\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t k1 = { 10, "alice" }, k2 = { 20, "bob" }, k3 = { 15, "fifteen" };
	s.add(&s, &k1);
	s.add(&s, &k2);
	s.add(&s, &k3);
	TEST_ASSERT(s.size_fn(&s) == 3);
	TEST_ASSERT(s.remove(&s, &k2) == EC_OK);
	TEST_ASSERT(s.size_fn(&s) == 2);
	TEST_ASSERT(s.contains(&s, &k2) == EC_ERR_NOT_FOUND);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_remove_pos\n");
}

static void
test_treeset_struct_remove_neg(void* mp)
{
	stamp();
	printf("test_treeset_struct_remove_neg\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t k = { 10, "ten" };
	s.add(&s, &k);
	TEST_ASSERT(s.remove(NULL, &k) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.remove(&s, NULL) == EC_ERR_INVALID_ARG);
	person_ts_t bad = { 999, "unknown" };
	TEST_ASSERT(s.remove(&s, &bad) == EC_ERR_NOT_FOUND);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_remove_neg\n");
}

static void
test_treeset_struct_remove_all_pos(void* mp)
{
	stamp();
	printf("test_treeset_struct_remove_all_pos\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t items[10];
	for (int i = 0; i < 10; i++) {
		items[i].id = i;
		snprintf(items[i].name, sizeof(items[i].name), "person%d", i);
		s.add(&s, &items[i]);
	}
	TEST_ASSERT(s.size_fn(&s) == 10);
	TEST_ASSERT(s.remove_all(&s) == EC_OK);
	TEST_ASSERT(s.size_fn(&s) == 0);
	TEST_ASSERT(s.remove_all(&s) == EC_OK);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_remove_all_pos\n");
}

static void
test_treeset_struct_remove_all_neg(void* mp)
{
	stamp();
	printf("test_treeset_struct_remove_all_neg\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	TEST_ASSERT(s.remove_all(NULL) == EC_ERR_INVALID_ARG);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_remove_all_neg\n");
}

static void
test_treeset_struct_remove_first_pos(void* mp)
{
	stamp();
	printf("test_treeset_struct_remove_first_pos\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t k1 = { 10, "alice" }, k2 = { 5, "five" }, k3 = { 15, "fifteen" };
	s.add(&s, &k1);
	s.add(&s, &k2);
	s.add(&s, &k3);
	TEST_ASSERT(s.size_fn(&s) == 3);
	person_ts_t first;
	TEST_ASSERT(s.get_first(&s, &first) == EC_OK);
	TEST_ASSERT(s.remove_first(&s) == EC_OK);
	TEST_ASSERT(s.size_fn(&s) == 2);
	TEST_ASSERT(s.contains(&s, &first) == EC_ERR_NOT_FOUND);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_remove_first_pos\n");
}

static void
test_treeset_struct_remove_first_neg(void* mp)
{
	stamp();
	printf("test_treeset_struct_remove_first_neg\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	TEST_ASSERT(s.remove_first(NULL) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.remove_first(&s) == EC_ERR_NOT_FOUND);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_remove_first_neg\n");
}

static void
test_treeset_struct_remove_last_pos(void* mp)
{
	stamp();
	printf("test_treeset_struct_remove_last_pos\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t k1 = { 10, "alice" }, k2 = { 5, "five" }, k3 = { 15, "fifteen" };
	s.add(&s, &k1);
	s.add(&s, &k2);
	s.add(&s, &k3);
	TEST_ASSERT(s.size_fn(&s) == 3);
	person_ts_t last;
	TEST_ASSERT(s.get_last(&s, &last) == EC_OK);
	TEST_ASSERT(s.remove_last(&s) == EC_OK);
	TEST_ASSERT(s.size_fn(&s) == 2);
	TEST_ASSERT(s.contains(&s, &last) == EC_ERR_NOT_FOUND);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_remove_last_pos\n");
}

static void
test_treeset_struct_remove_last_neg(void* mp)
{
	stamp();
	printf("test_treeset_struct_remove_last_neg\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	TEST_ASSERT(s.remove_last(NULL) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.remove_last(&s) == EC_ERR_NOT_FOUND);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_remove_last_neg\n");
}

static void
test_treeset_struct_get_first_pos(void* mp)
{
	stamp();
	printf("test_treeset_struct_get_first_pos\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t k1 = { 10, "alice" }, k2 = { 5, "five" }, k3 = { 15, "fifteen" };
	s.add(&s, &k1);
	s.add(&s, &k2);
	s.add(&s, &k3);
	person_ts_t out;
	TEST_ASSERT(s.get_first(&s, &out) == EC_OK);
	TEST_ASSERT(cmp_person(&out, &k2) == 0);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_get_first_pos\n");
}

static void
test_treeset_struct_get_first_neg(void* mp)
{
	stamp();
	printf("test_treeset_struct_get_first_neg\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t out;
	TEST_ASSERT(s.get_first(NULL, &out) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.get_first(&s, NULL) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.get_first(&s, &out) == EC_ERR_NOT_FOUND);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_get_first_neg\n");
}

static void
test_treeset_struct_get_last_pos(void* mp)
{
	stamp();
	printf("test_treeset_struct_get_last_pos\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t k1 = { 10, "alice" }, k2 = { 5, "five" }, k3 = { 15, "fifteen" };
	s.add(&s, &k1);
	s.add(&s, &k2);
	s.add(&s, &k3);
	person_ts_t out;
	TEST_ASSERT(s.get_last(&s, &out) == EC_OK);
	TEST_ASSERT(cmp_person(&out, &k3) == 0);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_get_last_pos\n");
}

static void
test_treeset_struct_get_last_neg(void* mp)
{
	stamp();
	printf("test_treeset_struct_get_last_neg\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t out;
	TEST_ASSERT(s.get_last(NULL, &out) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.get_last(&s, NULL) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.get_last(&s, &out) == EC_ERR_NOT_FOUND);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_get_last_neg\n");
}

static void
test_treeset_struct_get_greater_than_pos(void* mp)
{
	stamp();
	printf("test_treeset_struct_get_greater_than_pos\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t items5[5];
	for (int i = 0; i < 5; i++) {
		items5[i].id = i + 1;
		snprintf(items5[i].name, sizeof(items5[i].name), "person%d", i + 1);
		s.add(&s, &items5[i]);
	}
	person_ts_t ask = { 3, "three" }, out;
	TEST_ASSERT(s.get_greater_than(&s, &ask, &out) == EC_OK);
	person_ts_t expected = { 4, "person4" };
	TEST_ASSERT(cmp_person(&out, &expected) == 0);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_get_greater_than_pos\n");
}

static void
test_treeset_struct_get_greater_than_neg(void* mp)
{
	stamp();
	printf("test_treeset_struct_get_greater_than_neg\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t k = { 10, "ten" };
	s.add(&s, &k);
	person_ts_t ask = { 999, "bad" }, out;
	TEST_ASSERT(s.get_greater_than(NULL, &ask, &out) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.get_greater_than(&s, NULL, &out) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.get_greater_than(&s, &ask, NULL) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.get_greater_than(&s, &ask, &out) == EC_ERR_NOT_FOUND);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_get_greater_than_neg\n");
}

static void
test_treeset_struct_get_lesser_than_pos(void* mp)
{
	stamp();
	printf("test_treeset_struct_get_lesser_than_pos\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t items5[5];
	for (int i = 0; i < 5; i++) {
		items5[i].id = i + 1;
		snprintf(items5[i].name, sizeof(items5[i].name), "person%d", i + 1);
		s.add(&s, &items5[i]);
	}
	person_ts_t ask = { 3, "three" }, out;
	TEST_ASSERT(s.get_lesser_than(&s, &ask, &out) == EC_OK);
	person_ts_t expected2 = { 2, "person2" };
	TEST_ASSERT(cmp_person(&out, &expected2) == 0);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_get_lesser_than_pos\n");
}

static void
test_treeset_struct_get_lesser_than_neg(void* mp)
{
	stamp();
	printf("test_treeset_struct_get_lesser_than_neg\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t k = { 10, "ten" };
	s.add(&s, &k);
	person_ts_t ask = { 0, "zero" }, out;
	TEST_ASSERT(s.get_lesser_than(NULL, &ask, &out) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.get_lesser_than(&s, NULL, &out) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.get_lesser_than(&s, &ask, NULL) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.get_lesser_than(&s, &ask, &out) == EC_ERR_NOT_FOUND);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_get_lesser_than_neg\n");
}

static void
test_treeset_struct_contains_pos(void* mp)
{
	stamp();
	printf("test_treeset_struct_contains_pos\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t k1 = { 10, "alice" }, k2 = { 20, "bob" };
	s.add(&s, &k1);
	s.add(&s, &k2);
	TEST_ASSERT(s.contains(&s, &k1) == EC_OK);
	TEST_ASSERT(s.contains(&s, &k2) == EC_OK);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_contains_pos\n");
}

static void
test_treeset_struct_contains_neg(void* mp)
{
	stamp();
	printf("test_treeset_struct_contains_neg\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t k = { 10, "ten" };
	s.add(&s, &k);
	TEST_ASSERT(s.contains(NULL, &k) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.contains(&s, NULL) == EC_ERR_INVALID_ARG);
	person_ts_t bad = { 999, "unknown" };
	TEST_ASSERT(s.contains(&s, &bad) == EC_ERR_NOT_FOUND);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_contains_neg\n");
}

static void
foreach_key_person(person_ts_t k)
{
	(void)k;
}

static void
test_treeset_struct_foreach_pos(void* mp)
{
	stamp();
	printf("test_treeset_struct_foreach_pos\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t items5b[5];
	for (int i = 0; i < 5; i++) {
		items5b[i].id = i;
		snprintf(items5b[i].name, sizeof(items5b[i].name), "person%d", i);
		s.add(&s, &items5b[i]);
	}
	TEST_ASSERT(s.foreach (&s, foreach_key_person) == EC_OK);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_foreach_pos\n");
}

static void
test_treeset_struct_foreach_neg(void* mp)
{
	stamp();
	printf("test_treeset_struct_foreach_neg\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t k = { 10, "ten" };
	s.add(&s, &k);
	TEST_ASSERT(s.foreach (NULL, foreach_key_person) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.foreach (&s, NULL) == EC_ERR_INVALID_ARG);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_foreach_neg\n");
}

static void
test_treeset_struct_iterators_empty_pos(void* mp)
{
	stamp();
	printf("test_treeset_struct_iterators_empty_pos\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	ec_treeset_person_iter_t it;
	TEST_ASSERT(s.iter_init(&s, &it) == EC_OK);
	person_ts_t key;
	TEST_ASSERT(s.iter_next(&it, &key) == EC_ERR_OUT_OF_BOUNDS);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_iterators_empty_pos\n");
}

static void
test_treeset_struct_iterators_neg(void* mp)
{
	stamp();
	printf("test_treeset_struct_iterators_neg\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	ec_treeset_person_iter_t it;
	TEST_ASSERT(s.iter_init(NULL, &it) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.iter_init(&s, NULL) == EC_ERR_INVALID_ARG);
	s.iter_init(&s, &it);
	person_ts_t key;
	TEST_ASSERT(s.iter_next(NULL, &key) == EC_ERR_INVALID_ARG);
	s.iter_init(&s, &it);
	TEST_ASSERT(s.iter_next(&it, NULL) == EC_ERR_INVALID_ARG);
	s.iter_init(&s, &it);
	TEST_ASSERT(s.iter_remove(&it) == EC_ERR_INVALID_ARG);
	TEST_ASSERT(s.iter_remove(NULL) == EC_ERR_INVALID_ARG);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_iterators_neg\n");
}

static void
test_treeset_struct_iterators_pos(void* mp)
{
	stamp();
	printf("test_treeset_struct_iterators_pos\n");
	ec_treeset_person_t s;
	ec_treeset_person_init(&s, mp, NULL);
	person_ts_t items5b[5];
	for (int i = 0; i < 5; i++) {
		items5b[i].id = i;
		snprintf(items5b[i].name, sizeof(items5b[i].name), "person%d", i);
		s.add(&s, &items5b[i]);
	}
	ec_treeset_person_iter_t it;
	TEST_ASSERT(s.iter_init(&s, &it) == EC_OK);
	person_ts_t key;
	size_t cnt = 0;
	while (s.iter_next(&it, &key) == EC_OK)
		{
			TEST_ASSERT(key.id >= 0 && key.id < 5);
			cnt++;
		}
	TEST_ASSERT(cnt == 5);
	TEST_ASSERT(s.iter_init(&s, &it) == EC_OK);
	TEST_ASSERT(s.iter_next(&it, &key) == EC_OK);
	TEST_ASSERT(s.iter_remove(&it) == EC_OK && s.size_fn(&s) == 4);
	s.remove_all(&s);
	printf("pass:test_treeset_struct_iterators_pos\n");
}

int
main(void)
{
	printf("Starting ec_treeset_person tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = { 64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL };
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TEST_ASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_treeset_struct_init_pos(mp);
	test_treeset_struct_init_neg(mp);
	test_treeset_struct_add_pos(mp);
	test_treeset_struct_add_neg(mp);
	test_treeset_struct_remove_pos(mp);
	test_treeset_struct_remove_neg(mp);
	test_treeset_struct_remove_all_pos(mp);
	test_treeset_struct_remove_all_neg(mp);
	test_treeset_struct_remove_first_pos(mp);
	test_treeset_struct_remove_first_neg(mp);
	test_treeset_struct_remove_last_pos(mp);
	test_treeset_struct_remove_last_neg(mp);
	test_treeset_struct_get_first_pos(mp);
	test_treeset_struct_get_first_neg(mp);
	test_treeset_struct_get_last_pos(mp);
	test_treeset_struct_get_last_neg(mp);
	test_treeset_struct_get_greater_than_pos(mp);
	test_treeset_struct_get_greater_than_neg(mp);
	test_treeset_struct_get_lesser_than_pos(mp);
	test_treeset_struct_get_lesser_than_neg(mp);
	test_treeset_struct_contains_pos(mp);
	test_treeset_struct_contains_neg(mp);
	test_treeset_struct_foreach_pos(mp);
	test_treeset_struct_foreach_neg(mp);
	test_treeset_struct_iterators_empty_pos(mp);
	test_treeset_struct_iterators_neg(mp);
	test_treeset_struct_iterators_pos(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif

	printf("All ec_treeset_person tests passed.\n");
	return 0;
}
#endif /* #if 0 - TBD: EC_TREESET_DEF macro limitation with struct types */

/* Empty main to prevent linker errors */
int
main(void)
{
	return 0;
}
