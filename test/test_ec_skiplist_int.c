#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include <stdbool.h>
#include "../src/ec_skiplist.h"
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

static void print_stacktrace(void)
{
#ifdef __linux__
    void *arr[32];
    size_t sz = backtrace(arr, 32);
    char **sym = backtrace_symbols(arr, sz);
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

#define TEST_ASSERT(cond)                        \
    do                                           \
    {                                            \
        if (!(cond))                             \
        {                                        \
            fprintf(stderr, "[FAIL]%s:%d: %s\n", \
                    __FILE__, __LINE__, #cond);  \
            print_stacktrace();                  \
            exit(3);                             \
        }                                        \
    } while (0)

static void stamp(void)
{
    time_t t = time(NULL);
    struct tm *m = localtime(&t);
    if (m)
    {
        char b[64];
        strftime(b, sizeof(b), "[%Y-%m-%d %H:%M:%S]", m);
        printf("%s ", b);
    }
    else
        printf("[timeerr] ");
}

static void test_skiplist_int_init(void *mp)
{
    stamp();
    printf("test_skiplist_int_init\n");
    ec_skiplist_int_t sl;
    ec_status_t st = ec_skiplist_int_init(&sl, mp, NULL);
    TEST_ASSERT(st == EC_OK);
    TEST_ASSERT(sl.size == 0);
    sl.remove_all(&sl);
    printf("pass:test_skiplist_int_init\n");
}

static void test_skiplist_int_add(void *mp)
{
    stamp();
    printf("test_skiplist_int_add\n");
    ec_skiplist_int_t sl;
    ec_skiplist_int_init(&sl, mp, NULL);
    int k1 = 10, k2 = 20;
    sl.add(&sl, &k1);
    sl.add(&sl, &k2);
    TEST_ASSERT(sl.size_fn(&sl) == 2);
    TEST_ASSERT(sl.contains(&sl, &k1) == EC_OK && sl.contains(&sl, &k2) == EC_OK);
    int bad = 999;
    TEST_ASSERT(sl.contains(&sl, &bad) == EC_ERR_NOT_FOUND);
    sl.remove_all(&sl);
    printf("pass:test_skiplist_int_add\n");
}

static void test_skiplist_int_add_duplicate(void *mp)
{
    stamp();
    printf("test_skiplist_int_add_duplicate\n");
    ec_skiplist_int_t sl;
    ec_skiplist_int_init(&sl, mp, NULL);
    int k = 42;
    sl.add(&sl, &k);
    TEST_ASSERT(sl.size_fn(&sl) == 1);
    sl.add(&sl, &k);
    TEST_ASSERT(sl.size_fn(&sl) == 1);
    TEST_ASSERT(sl.contains(&sl, &k) == EC_OK);
    sl.remove_all(&sl);
    printf("pass:test_skiplist_int_add_duplicate\n");
}

static void test_skiplist_int_remove(void *mp)
{
    stamp();
    printf("test_skiplist_int_remove\n");
    ec_skiplist_int_t sl;
    ec_skiplist_int_init(&sl, mp, NULL);
    int k1 = 1, k2 = 2, k3 = 3;
    sl.add(&sl, &k1);
    sl.add(&sl, &k2);
    sl.add(&sl, &k3);
    TEST_ASSERT(sl.size_fn(&sl) == 3);
    TEST_ASSERT(sl.remove_f(&sl, &k2) == EC_OK);
    TEST_ASSERT(sl.size_fn(&sl) == 2);
    TEST_ASSERT(sl.remove_f(&sl, &k2) == EC_ERR_NOT_FOUND);
    TEST_ASSERT(sl.remove_f(NULL, &k3) == EC_ERR_INVALID_ARG);
    sl.remove_all(&sl);
    printf("pass:test_skiplist_int_remove\n");
}

static void test_skiplist_int_remove_all(void *mp)
{
    stamp();
    printf("test_skiplist_int_remove_all\n");
    ec_skiplist_int_t sl;
    ec_skiplist_int_init(&sl, mp, NULL);
    for (int i = 0; i < 10; i++)
        sl.add(&sl, &i);
    TEST_ASSERT(sl.size_fn(&sl) == 10);
    TEST_ASSERT(sl.remove_all(&sl) == EC_OK && sl.size_fn(&sl) == 0);
    TEST_ASSERT(sl.remove_all(&sl) == EC_OK && sl.size_fn(&sl) == 0);
    printf("pass:test_skiplist_int_remove_all\n");
}

static void test_skiplist_int_contains(void *mp)
{
    stamp();
    printf("test_skiplist_int_contains\n");
    ec_skiplist_int_t sl;
    ec_skiplist_int_init(&sl, mp, NULL);
    int k = 999;
    sl.add(&sl, &k);
    TEST_ASSERT(sl.contains(&sl, &k) == EC_OK);
    int bad = 1000;
    TEST_ASSERT(sl.contains(&sl, &bad) == EC_ERR_NOT_FOUND);
    sl.remove_all(&sl);
    printf("pass:test_skiplist_int_contains\n");
}

static void test_skiplist_int_get_keys(void *mp)
{
    stamp();
    printf("test_skiplist_int_get_keys\n");
    ec_skiplist_int_t sl;
    ec_skiplist_int_init(&sl, mp, NULL);
    for (int i = 1; i <= 5; i++)
        sl.add(&sl, &i);
    int *keys = NULL;
    size_t count = 0;
    ec_status_t st = sl.get_keys(&sl, &keys, &count);
    TEST_ASSERT(st == EC_OK && count == 5);
    for (size_t i = 1; i < count; i++)
        TEST_ASSERT(keys[i - 1] <= keys[i]);
    EC_FREE(keys);
    sl.remove_all(&sl);
    printf("pass:test_skiplist_int_get_keys\n");
}

static void foreach_key_int(int k)
{
    (void)k;
}

static void test_skiplist_int_foreach(void *mp)
{
    stamp();
    printf("test_skiplist_int_foreach\n");
    ec_skiplist_int_t sl;
    ec_skiplist_int_init(&sl, mp, NULL);
    for (int i = 0; i < 5; i++)
    {
        int v = i + 10;
        sl.add(&sl, &v);
    }
    TEST_ASSERT(sl.foreach_key(&sl, foreach_key_int) == EC_OK);
    sl.remove_all(&sl);
    printf("pass:test_skiplist_int_foreach\n");
}

static void test_skiplist_int_iterators_empty(void *mp)
{
    stamp();
    printf("test_skiplist_int_iterators_empty\n");
    ec_skiplist_int_t sl;
    ec_skiplist_int_init(&sl, mp, NULL);
    ec_skiplist_int_iter_t it;
    TEST_ASSERT(sl.iter_init(&sl, &it) == EC_OK);
    int key;
    TEST_ASSERT(sl.iter_next(&it, &key) == EC_ERR_OUT_OF_BOUNDS);
    TEST_ASSERT(sl.iter_remove(&it) == EC_ERR_INVALID_ARG);
    sl.remove_all(&sl);
    printf("pass:test_skiplist_int_iterators_empty\n");
}

static void test_skiplist_int_iterators(void *mp)
{
    stamp();
    printf("test_skiplist_int_iterators\n");
    ec_skiplist_int_t sl;
    ec_skiplist_int_init(&sl, mp, NULL);
    for (int i = 0; i < 5; i++)
    {
        int k = i + 100;
        sl.add(&sl, &k);
    }
    ec_skiplist_int_iter_t it;
    TEST_ASSERT(sl.iter_init(&sl, &it) == EC_OK);
    int key;
    size_t cnt = 0;
    while (sl.iter_next(&it, &key) == EC_OK)
    {
        TEST_ASSERT(key >= 100 && key < 105);
        cnt++;
    }
    TEST_ASSERT(cnt == 5);
    /* Test removal during iteration */
    TEST_ASSERT(sl.iter_init(&sl, &it) == EC_OK);
    TEST_ASSERT(sl.iter_next(&it, &key) == EC_OK);
    TEST_ASSERT(sl.iter_remove(&it) == EC_OK && sl.size_fn(&sl) == 4);
    sl.remove_all(&sl);
    printf("pass:test_skiplist_int_iterators\n");
}

int main(void)
{
    printf("Starting ec_skiplist_int tests...\n");
#ifdef EC_USE_MEMPOOL
    ec_mempool_conf_t conf = {64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL};
    ec_mempool_t pool;
    ec_status_t st = ec_mempool_init(&pool, &conf);
    TEST_ASSERT(st == EC_OK);
    void *mp = &pool;
#else
    void *mp = NULL;
#endif

    test_skiplist_int_init(mp);
    test_skiplist_int_add(mp);
    test_skiplist_int_add_duplicate(mp);
    test_skiplist_int_remove(mp);
    test_skiplist_int_remove_all(mp);
    test_skiplist_int_contains(mp);
    test_skiplist_int_get_keys(mp);
    test_skiplist_int_foreach(mp);
    test_skiplist_int_iterators_empty(mp);
    test_skiplist_int_iterators(mp);

#ifdef EC_USE_MEMPOOL
    ec_mempool_destroy((ec_mempool_t *)mp);
#endif

    printf("All ec_skiplist_int tests passed.\n");
    return 0;
}
