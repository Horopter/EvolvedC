#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define DELETE_FILE(filename) DeleteFile(filename)
typedef HANDLE thread_t;
#define THREAD_FUNC DWORD WINAPI
#else
#include <pthread.h>
#include <unistd.h>
#define DELETE_FILE(filename) unlink(filename)
typedef pthread_t thread_t;
#define THREAD_FUNC void *
#endif
#include "../src/ec_log.h"
#define TEST_ASSERT(cond)                                                     \
    do                                                                        \
    {                                                                         \
        if (!(cond))                                                          \
        {                                                                     \
            fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            exit(EXIT_FAILURE);                                               \
        }                                                                     \
    } while (0)
static int callback_called = 0;
static int log_callback(void *arg, ec_log_level_t level, const char *fmt, va_list args)
{
    (void)arg;
    (void)level;
    (void)fmt;
    (void)args;
    callback_called++;
    return 0;
}
static void cleanup_logs(void)
{
    DELETE_FILE("test_log.log");
    DELETE_FILE("same_file.log");
    DELETE_FILE("test_write.log");
    DELETE_FILE("log_level_test.log");
    DELETE_FILE("thread_safety.log");
    DELETE_FILE("shutdown_test.log");
    DELETE_FILE("log_rotation_test.log");
    DELETE_FILE("multiple_logger_1.log");
    DELETE_FILE("multiple_logger_2.log");
    DELETE_FILE("file_integrity_test.log");
}
static void test_log_init(void)
{
    printf("[RUN] test_log_init\n");
    ec_log_pool_init();
    ec_log_t *logger = NULL;
    TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, true, "test_log.log", NULL, NULL) == EC_OK);
    TEST_ASSERT(logger && logger->to_stdout && logger->level == EC_LOG_INFO && logger->file);
    ec_log_shutdown(logger);
    printf("[PASS] test_log_init\n");
}
static void test_log_auto_allocate(void)
{
    printf("[RUN] test_log_auto_allocate\n");
    ec_log_t *logger = NULL;
    TEST_ASSERT(ec_log_init(&logger, EC_LOG_WARN, false, NULL, NULL, NULL) == EC_OK);
    TEST_ASSERT(logger && logger->file == NULL && logger->level == EC_LOG_WARN);
    ec_log_shutdown(logger);
    printf("[PASS] test_log_auto_allocate\n");
}
static void test_log_prevent_duplicate_files(void)
{
    printf("[RUN] test_log_prevent_duplicate_files\n");
    ec_log_t *logger1 = NULL, *logger2 = NULL;
    TEST_ASSERT(ec_log_init(&logger1, EC_LOG_INFO, true, "same_file.log", NULL, NULL) == EC_OK);
    TEST_ASSERT(ec_log_init(&logger2, EC_LOG_INFO, true, "same_file.log", NULL, NULL) == EC_ERR_INVALID_OP);
    ec_log_shutdown(logger1);
    printf("[PASS] test_log_prevent_duplicate_files\n");
}
static void test_log_write(void)
{
    printf("[RUN] test_log_write\n");
    ec_log_t *logger = NULL;
    ec_log_init(&logger, EC_LOG_DEBUG, true, "test_write.log", NULL, NULL);
    EC_LOG_INFO(logger, "Test message.");
    ec_log_shutdown(logger);
    printf("[PASS] test_log_write\n");
}
static void test_log_rotation(void)
{
    printf("[RUN] test_log_rotation\n");
    ec_log_t *logger = NULL;
    ec_log_init(&logger, EC_LOG_INFO, true, "log_rotation_test.log", NULL, NULL);
    for (int i = 0; i < 1000; i++)
        EC_LOG_INFO(logger, "Log %d", i);
    ec_log_shutdown(logger);
    printf("[PASS] test_log_rotation\n");
}
static void test_multiple_loggers(void)
{
    printf("[RUN] test_multiple_loggers\n");
    ec_log_t *logger1 = NULL, *logger2 = NULL;
    ec_log_init(&logger1, EC_LOG_INFO, true, "multiple_logger_1.log", NULL, NULL);
    ec_log_init(&logger2, EC_LOG_WARN, false, "multiple_logger_2.log", NULL, NULL);
    EC_LOG_INFO(logger1, "Logger 1.");
    EC_LOG_WARN(logger2, "Logger 2.");
    ec_log_shutdown(logger1);
    ec_log_shutdown(logger2);
    printf("[PASS] test_multiple_loggers\n");
}
static void test_log_file_integrity(void)
{
    printf("[RUN] test_log_file_integrity\n");
    ec_log_t *logger = NULL;
    ec_log_init(&logger, EC_LOG_INFO, true, "file_integrity_test.log", NULL, NULL);
    EC_LOG_INFO(logger, "Checking integrity.");
    FILE *file = fopen("file_integrity_test.log", "r");
    TEST_ASSERT(file);
    fclose(file);
    ec_log_shutdown(logger);
    printf("[PASS] test_log_file_integrity\n");
}
#define THREAD_COUNT 5
#define MESSAGES_PER_THREAD 100
#define TOTAL_EXPECTED_MESSAGES (THREAD_COUNT * MESSAGES_PER_THREAD)
THREAD_FUNC thread_log_worker(void *arg)
{
    ec_log_t *logger = (ec_log_t *)arg;
    for (int i = 0; i < MESSAGES_PER_THREAD; i++)
        EC_LOG_INFO(logger, "Thread %d", i);
    return 0;
}
static void test_log_thread_safety(void)
{
    printf("[RUN] test_log_thread_safety\n");
    ec_log_t *logger = NULL;
    ec_log_init(&logger, EC_LOG_INFO, true, "thread_safety.log", NULL, NULL);
    thread_t threads[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++)
    {
#if defined(_WIN32) || defined(_WIN64)
        threads[i] = CreateThread(NULL, 0, thread_log_worker, logger, 0, NULL);
        TEST_ASSERT(threads[i] != NULL);
#else
        TEST_ASSERT(pthread_create(&threads[i], NULL, thread_log_worker, logger) == 0);
#endif
    }
    for (int i = 0; i < THREAD_COUNT; i++)
    {
#if defined(_WIN32) || defined(_WIN64)
        WaitForSingleObject(threads[i], INFINITE);
        CloseHandle(threads[i]);
#else
        pthread_join(threads[i], NULL);
#endif
    }
    ec_log_shutdown(logger);
    printf("[PASS] test_log_thread_safety\n");
}
static void test_log_set_level(void)
{
    printf("[RUN] test_log_set_level\n");
    ec_log_t *logger = NULL;
    ec_log_init(&logger, EC_LOG_INFO, true, "log_level_test.log", NULL, NULL);
    EC_LOG_DEBUG(logger, "Should NOT appear.");
    EC_LOG_INFO(logger, "Should appear.");
    ec_log_set_level(logger, EC_LOG_DEBUG);
    EC_LOG_DEBUG(logger, "Now debug appears.");
    ec_log_shutdown(logger);
    printf("[PASS] test_log_set_level\n");
}
static void test_log_callback(void)
{
    printf("[RUN] test_log_callback\n");
    ec_log_t *logger = NULL;
    ec_log_init(&logger, EC_LOG_INFO, false, NULL, NULL, log_callback);
    EC_LOG_INFO(logger, "Testing callback.");
    TEST_ASSERT(callback_called > 0);
    ec_log_shutdown(logger);
    printf("[PASS] test_log_callback\n");
}
static void test_log_shutdown(void)
{
    printf("[RUN] test_log_shutdown\n");
    ec_log_t *logger = NULL;
    ec_log_init(&logger, EC_LOG_INFO, true, "shutdown_test.log", NULL, NULL);
    ec_log_shutdown(logger);
    printf("[PASS] test_log_shutdown\n");
}
int main(void)
{
    printf("\nStarting `ec_log` Tests...\n");
    test_log_init();
    test_log_auto_allocate();
    test_log_prevent_duplicate_files();
    test_log_write();
    test_log_rotation();
    test_multiple_loggers();
    test_log_file_integrity();
    test_log_thread_safety();
    test_log_set_level();
    test_log_callback();
    test_log_shutdown();
    cleanup_logs();
    printf("\nAll `ec_log` tests passed successfully!\n");
    return 0;
}
