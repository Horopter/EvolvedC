#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux__
#include <unistd.h>
#define DELETE_FILE(filename) unlink(filename)
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define DELETE_FILE(filename) DeleteFile(filename)
#else
#define DELETE_FILE(filename) remove(filename)
#endif
#include "../../src/ec_log.h"

#define TEST_ASSERT(cond)                                                             \
	do                                                                                \
		{                                                                             \
			if (!(cond))                                                              \
				{                                                                     \
					fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, #cond); \
					exit(EXIT_FAILURE);                                               \
				}                                                                     \
		}                                                                             \
	while (0)

static void
cleanup_logs(void)
{
	DELETE_FILE("test_log_int.log");
	DELETE_FILE("test_log_int_neg.log");
}

/* Test init - positive */
static void
test_log_init_pos(void)
{
	printf("[RUN] test_log_init_pos\n");
	ec_log_pool_init();
	ec_log_t* logger = NULL;
	TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, true, "test_log_int.log", NULL, NULL) == EC_OK);
	TEST_ASSERT(logger != NULL);
	TEST_ASSERT(logger->to_stdout == true);
	TEST_ASSERT(logger->level == EC_LOG_INFO);
	TEST_ASSERT(logger->file != NULL);
	ec_log_shutdown(logger);
	printf("[PASS] test_log_init_pos\n");
}

/* Test init - negative (NULL logger pointer) */
static void
test_log_init_neg(void)
{
	printf("[RUN] test_log_init_neg\n");
	ec_log_pool_init();
	TEST_ASSERT(ec_log_init(NULL, EC_LOG_INFO, true, "test_log_int_neg.log", NULL, NULL) == EC_ERR_INVALID_ARG);
	printf("[PASS] test_log_init_neg\n");
}

/* Test logging int values - positive */
static void
test_log_int_values_pos(void)
{
	printf("[RUN] test_log_int_values_pos\n");
	ec_log_t* logger = NULL;
	TEST_ASSERT(ec_log_init(&logger, EC_LOG_DEBUG, true, "test_log_int.log", NULL, NULL) == EC_OK);

	int val1 = 42;
	int val2 = -100;
	int val3 = 0;
	int val4 = 2147483647;
	int val5 = -2147483648;

	EC_LOG_INFO(logger, "Integer value: %d", val1);
	EC_LOG_INFO(logger, "Negative integer: %d", val2);
	EC_LOG_INFO(logger, "Zero: %d", val3);
	EC_LOG_INFO(logger, "Max int: %d", val4);
	EC_LOG_INFO(logger, "Min int: %d", val5);
	EC_LOG_INFO(logger, "Multiple ints: %d, %d, %d", val1, val2, val3);

	ec_log_shutdown(logger);
	printf("[PASS] test_log_int_values_pos\n");
}

/* Test logging int values - negative (level filtering) */
static void
test_log_int_values_neg(void)
{
	printf("[RUN] test_log_int_values_neg\n");
	ec_log_t* logger = NULL;
	TEST_ASSERT(ec_log_init(&logger, EC_LOG_WARN, true, "test_log_int.log", NULL, NULL) == EC_OK);

	int val = 42;
	/* DEBUG and INFO should not appear due to level filtering */
	EC_LOG_DEBUG(logger, "Should not appear: %d", val);
	EC_LOG_INFO(logger, "Should not appear: %d", val);
	/* WARN and above should appear */
	EC_LOG_WARN(logger, "Should appear: %d", val);
	EC_LOG_ERROR(logger, "Should appear: %d", val);

	ec_log_shutdown(logger);
	printf("[PASS] test_log_int_values_neg\n");
}

/* Test set_level - positive */
static void
test_log_set_level_pos(void)
{
	printf("[RUN] test_log_set_level_pos\n");
	ec_log_t* logger = NULL;
	TEST_ASSERT(ec_log_init(&logger, EC_LOG_ERROR, true, "test_log_int.log", NULL, NULL) == EC_OK);

	int val = 123;
	EC_LOG_ERROR(logger, "Error level: %d", val);
	ec_log_set_level(logger, EC_LOG_DEBUG);
	EC_LOG_DEBUG(logger, "Debug after set_level: %d", val);

	ec_log_shutdown(logger);
	printf("[PASS] test_log_set_level_pos\n");
}

/* Test set_level - negative (NULL logger) */
static void
test_log_set_level_neg(void)
{
	printf("[RUN] test_log_set_level_neg\n");
	/* Should not crash with NULL */
	ec_log_set_level(NULL, EC_LOG_DEBUG);
	printf("[PASS] test_log_set_level_neg\n");
}

/* Test shutdown - positive */
static void
test_log_shutdown_pos(void)
{
	printf("[RUN] test_log_shutdown_pos\n");
	ec_log_t* logger = NULL;
	TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, true, "test_log_int.log", NULL, NULL) == EC_OK);
	int val = 999;
	EC_LOG_INFO(logger, "Before shutdown: %d", val);
	ec_log_shutdown(logger);
	/* Logger should be properly cleaned up */
	printf("[PASS] test_log_shutdown_pos\n");
}

/* Test shutdown - negative (NULL logger, should not crash) */
static void
test_log_shutdown_neg(void)
{
	printf("[RUN] test_log_shutdown_neg\n");
	ec_log_shutdown(NULL);
	printf("[PASS] test_log_shutdown_neg\n");
}

/* Test all log levels with int */
static void
test_log_all_levels_int(void)
{
	printf("[RUN] test_log_all_levels_int\n");
	ec_log_t* logger = NULL;
	TEST_ASSERT(ec_log_init(&logger, EC_LOG_DEBUG, true, "test_log_int.log", NULL, NULL) == EC_OK);

	int val = 42;
	EC_LOG_DEBUG(logger, "DEBUG: %d", val);
	EC_LOG_INFO(logger, "INFO: %d", val);
	EC_LOG_WARN(logger, "WARN: %d", val);
	EC_LOG_ERROR(logger, "ERROR: %d", val);
	EC_LOG_FATAL(logger, "FATAL: %d", val);

	ec_log_shutdown(logger);
	printf("[PASS] test_log_all_levels_int\n");
}

/* Test callback with int */
static int callback_called_int = 0;
static int
log_callback_int(void* arg, ec_log_level_t level, const char* fmt, va_list args)
{
	(void)arg;
	(void)level;
	(void)fmt;
	(void)args;
	callback_called_int++;
	return 0;
}

static void
test_log_callback_int(void)
{
	printf("[RUN] test_log_callback_int\n");
	ec_log_t* logger = NULL;
	TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, false, NULL, NULL, log_callback_int) == EC_OK);
	int val = 42;
	EC_LOG_INFO(logger, "Callback test: %d", val);
	TEST_ASSERT(callback_called_int > 0);
	ec_log_shutdown(logger);
	printf("[PASS] test_log_callback_int\n");
}

/* Test file logging with int */
static void
test_log_file_int(void)
{
	printf("[RUN] test_log_file_int\n");
	ec_log_t* logger = NULL;
	TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, false, "test_log_int.log", NULL, NULL) == EC_OK);
	int val = 12345;
	EC_LOG_INFO(logger, "File log test: %d", val);
	/* Verify file exists */
	FILE* file = fopen("test_log_int.log", "r");
	TEST_ASSERT(file != NULL);
	fclose(file);
	ec_log_shutdown(logger);
	printf("[PASS] test_log_file_int\n");
}

/* Test prevent duplicate files */
static void
test_log_duplicate_file_int(void)
{
	printf("[RUN] test_log_duplicate_file_int\n");
	ec_log_t *logger1 = NULL, *logger2 = NULL;
	TEST_ASSERT(ec_log_init(&logger1, EC_LOG_INFO, true, "test_log_int.log", NULL, NULL) == EC_OK);
	TEST_ASSERT(ec_log_init(&logger2, EC_LOG_INFO, true, "test_log_int.log", NULL, NULL) == EC_ERR_INVALID_OP);
	ec_log_shutdown(logger1);
	printf("[PASS] test_log_duplicate_file_int\n");
}

int
main(void)
{
	printf("\nStarting `ec_log_int` Tests...\n");
	cleanup_logs();
	test_log_init_pos();
	test_log_init_neg();
	test_log_int_values_pos();
	test_log_int_values_neg();
	test_log_set_level_pos();
	test_log_set_level_neg();
	test_log_shutdown_pos();
	test_log_shutdown_neg();
	test_log_all_levels_int();
	test_log_callback_int();
	test_log_file_int();
	test_log_duplicate_file_int();
	cleanup_logs();
	printf("\nAll `ec_log_int` tests passed successfully!\n");
	return 0;
}
