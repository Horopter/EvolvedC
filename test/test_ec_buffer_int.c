#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../src/ec_buffer.h"
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

/* Print stack trace on failure (Linux only) */
static void print_stacktrace(void) {
#ifdef __linux__
    void *arr[32];
    size_t n = backtrace(arr, 32);
    char **sym = backtrace_symbols(arr, n);
    if (sym) {
        for (size_t i = 0; i < n; i++)
            fprintf(stderr, "%s\n", sym[i]);
        free(sym);
    }
#else
    fprintf(stderr, "(NoStackTrace)\n");
#endif
}

/* Assertion macro */
#define TEST_ASSERT(cond)                                                     \
    do {                                                                      \
        if (!(cond)) {                                                        \
            fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            print_stacktrace();                                               \
            exit(3);                                                          \
        }                                                                     \
    } while (0)

/* Timestamp logging */
static void stamp(void) {
    time_t t = time(NULL);
    struct tm *m = localtime(&t);
    if (m) {
        char b[64];
        strftime(b, sizeof(b), "[ %Y-%m-%d %H:%M:%S ]", m);
        printf("%s ", b);
    } else {
        printf("[timeerr] ");
    }
}

/* Test buffer initialization */
static void test_buf_init(void *mp) {
    stamp();
    printf("test_buf_init\n");

    ec_buf_int_t buf;
    TEST_ASSERT(ec_buf_int_init(&buf, 10, mp));
    TEST_ASSERT(buf.cap == 10);
    TEST_ASSERT(buf.wpos == 0);
    TEST_ASSERT(buf.rpos == 0);
    TEST_ASSERT(buf.err == false);

    ec_buf_int_free(&buf);
    printf("pass:test_buf_init\n");
}

/* Test writing and reading single values */
static void test_buf_put_get(void *mp) {
    stamp();
    printf("test_buf_put_get\n");

    ec_buf_int_t buf;
    ec_buf_int_init(&buf, 5, mp);

    for (int i = 1; i <= 5; i++) {
        TEST_ASSERT(ec_buf_int_put(&buf, i));
    }

    TEST_ASSERT(ec_buf_int_size(&buf) == 5);
    
    for (int i = 1; i <= 5; i++) {
        int val;
        TEST_ASSERT(ec_buf_int_get(&buf, &val));
        TEST_ASSERT(val == i);
    }

    TEST_ASSERT(ec_buf_int_size(&buf) == 0);
    ec_buf_int_free(&buf);
    printf("pass:test_buf_put_get\n");
}

/* Test buffer resizing */
static void test_buf_resize(void *mp) {
    stamp();
    printf("test_buf_resize\n");

    ec_buf_int_t buf;
    ec_buf_int_init(&buf, 2, mp);

    TEST_ASSERT(ec_buf_int_put(&buf, 10));
    TEST_ASSERT(ec_buf_int_put(&buf, 20));

    /* Force buffer to resize */
    TEST_ASSERT(ec_buf_int_put(&buf, 30));
    TEST_ASSERT(buf.cap >= 3);  // Should have resized

    int val;
    TEST_ASSERT(ec_buf_int_get(&buf, &val) && val == 10);
    TEST_ASSERT(ec_buf_int_get(&buf, &val) && val == 20);
    TEST_ASSERT(ec_buf_int_get(&buf, &val) && val == 30);
    
    ec_buf_int_free(&buf);
    printf("pass:test_buf_resize\n");
}

/* Test peeking values */
static void test_buf_peek(void *mp) {
    stamp();
    printf("test_buf_peek\n");

    ec_buf_int_t buf;
    ec_buf_int_init(&buf, 3, mp);

    TEST_ASSERT(ec_buf_int_put(&buf, 42));
    int val;
    TEST_ASSERT(ec_buf_int_peek(&buf, &val));
    TEST_ASSERT(val == 42);  // Peek should NOT move the read position

    TEST_ASSERT(ec_buf_int_get(&buf, &val));
    TEST_ASSERT(val == 42);  // Now actually reading

    ec_buf_int_free(&buf);
    printf("pass:test_buf_peek\n");
}

/* Test compacting buffer */
static void test_buf_compact(void *mp) {
    stamp();
    printf("test_buf_compact\n");

    ec_buf_int_t buf;
    ec_buf_int_init(&buf, 5, mp);

    for (int i = 1; i <= 3; i++) {
        TEST_ASSERT(ec_buf_int_put(&buf, i));
    }

    int val;
    TEST_ASSERT(ec_buf_int_get(&buf, &val) && val == 1);
    TEST_ASSERT(ec_buf_int_get(&buf, &val) && val == 2);

    ec_buf_int_compact(&buf);

    TEST_ASSERT(buf.rpos == 0);
    TEST_ASSERT(buf.wpos == 1);
    TEST_ASSERT(buf.mem[0] == 3);

    ec_buf_int_free(&buf);
    printf("pass:test_buf_compact\n");
}

/* Test buffer wrap (static memory usage) */
static void test_buf_wrap(void *mp) {
    stamp();
    printf("test_buf_wrap\n");

    int arr[4] = {10, 20, 30, 40};
    ec_buf_int_t buf;
    ec_buf_int_wrap(&buf, arr, 4);

    TEST_ASSERT(ec_buf_int_size(&buf) == 4);

    int val;
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT(ec_buf_int_get(&buf, &val));
        TEST_ASSERT(val == arr[i]);
    }

    printf("pass:test_buf_wrap\n");
}

/* Run all buffer tests */
int main(void) {
    printf("Starting ec_buf tests...\n");

#ifdef EC_USE_MEMPOOL
    ec_mempool_conf_t conf = {64, 100, EC_MEMPOOL_DYNAMIC, EC_THREAD_UNSAFE, 8, 0, 2.0f, NULL, NULL, NULL};
    ec_mempool_t pool;
    ec_status_t st = ec_mempool_init(&pool, &conf);
    TEST_ASSERT(st == EC_OK);
    void *mp = &pool;
#else
    void *mp = NULL;
#endif

    test_buf_init(mp);
    test_buf_put_get(mp);
    test_buf_resize(mp);
    test_buf_peek(mp);
    test_buf_compact(mp);
    test_buf_wrap(mp);

#ifdef EC_USE_MEMPOOL
    ec_mempool_destroy((ec_mempool_t *)mp);
#endif

    printf("All ec_buf tests passed.\n");
    return 0;
}
