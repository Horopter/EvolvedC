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

typedef struct person_log_s
{
	int id;
	char name[16];
} person_log_t;

static void
cleanup_logs(void)
{
	DELETE_FILE("test_log_struct.log");
	DELETE_FILE("test_log_struct_neg.log");
}

/* Test init - positive */
static void
test_log_init_pos(void)
{
	printf("[RUN] test_log_init_pos\n");
	ec_log_pool_init();
	ec_log_t* logger = NULL;
	TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, true, "test_log_struct.log", NULL, NULL) == EC_OK);
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
	TEST_ASSERT(ec_log_init(NULL, EC_LOG_INFO, true, "test_log_struct_neg.log", NULL, NULL) == EC_ERR_INVALID_ARG);
	printf("[PASS] test_log_init_neg\n");
}

/* Test logging struct values - positive */
static void
test_log_struct_values_pos(void)
{
	printf("[RUN] test_log_struct_values_pos\n");
	ec_log_t* logger = NULL;
	TEST_ASSERT(ec_log_init(&logger, EC_LOG_DEBUG, true, "test_log_struct.log", NULL, NULL) == EC_OK);

	person_log_t val1 = { 1, "Alice" };
	person_log_t val2 = { 2, "Bob" };
	person_log_t val3 = { 3, "Charlie" };
	person_log_t val4 = { 4, "Diana" };

	EC_LOG_INFO(logger, "Person: id=%d, name=%s", val1.id, val1.name);
	EC_LOG_INFO(logger, "Person: id=%d, name=%s", val2.id, val2.name);
	EC_LOG_INFO(logger, "Person: id=%d, name=%s", val3.id, val3.name);
	EC_LOG_INFO(logger, "Person: id=%d, name=%s", val4.id, val4.name);
	EC_LOG_INFO(logger, "Multiple: %d/%s, %d/%s, %d/%s", val1.id, val1.name, val2.id, val2.name, val3.id, val3.name);

	ec_log_shutdown(logger);
	printf("[PASS] test_log_struct_values_pos\n");
}

/* Test logging struct values - negative (level filtering) */
static void
test_log_struct_values_neg(void)
{
	printf("[RUN] test_log_struct_values_neg\n");
	ec_log_t* logger = NULL;
	TEST_ASSERT(ec_log_init(&logger, EC_LOG_WARN, true, "test_log_struct.log", NULL, NULL) == EC_OK);

	person_log_t val = { 42, "Test" };
	/* DEBUG and INFO should not appear due to level filtering */
	EC_LOG_DEBUG(logger, "No: id=%d, name=%s", val.id, val.name);
	EC_LOG_INFO(logger, "No: id=%d, name=%s", val.id, val.name);
	/* WARN and above should appear */
	EC_LOG_WARN(logger, "Yes: id=%d, name=%s", val.id, val.name);
	EC_LOG_ERROR(logger, "Yes: id=%d, name=%s", val.id, val.name);

	ec_log_shutdown(logger);
	printf("[PASS] test_log_struct_values_neg\n");
}

/* Test set_level - positive */
static void
test_log_set_level_pos(void)
{
	printf("[RUN] test_log_set_level_pos\n");
	ec_log_t* logger = NULL;
	TEST_ASSERT(ec_log_init(&logger, EC_LOG_ERROR, true, "test_log_struct.log", NULL, NULL) == EC_OK);

	person_log_t val = { 123, "ErrorTest" };
	EC_LOG_ERROR(logger, "Error: id=%d, name=%s", val.id, val.name);
	ec_log_set_level(logger, EC_LOG_DEBUG);
	EC_LOG_DEBUG(logger, "Debug: id=%d, name=%s", val.id, val.name);

	ec_log_shutdown(logger);
	printf("[PASS] test_log_set_level_pos\n");
}

/* Test set_level - negative (NULL logger) */
static void
test_log_set_level_neg(void)
{
	printf("[RUN] test_log_set_level_neg\n");
	ec_log_set_level(NULL, EC_LOG_DEBUG);
	printf("[PASS] test_log_set_level_neg\n");
}

/* Test shutdown - positive */
static void
test_log_shutdown_pos(void)
{
	printf("[RUN] test_log_shutdown_pos\n");
	ec_log_t* logger = NULL;
	TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, true, "test_log_struct.log", NULL, NULL) == EC_OK);
	person_log_t val = { 999, "Shutdown" };
	EC_LOG_INFO(logger, "Before: id=%d, name=%s", val.id, val.name);
	ec_log_shutdown(logger);
	printf("[PASS] test_log_shutdown_pos\n");
}

/* Test shutdown - negative (NULL logger) */
static void
test_log_shutdown_neg(void)
{
	printf("[RUN] test_log_shutdown_neg\n");
	ec_log_shutdown(NULL);
	printf("[PASS] test_log_shutdown_neg\n");
}

/* Test all log levels with struct */
static void
test_log_all_levels_struct(void)
{
	printf("[RUN] test_log_all_levels_struct\n");
	ec_log_t* logger = NULL;
	TEST_ASSERT(ec_log_init(&logger, EC_LOG_DEBUG, true, "test_log_struct.log", NULL, NULL) == EC_OK);

	person_log_t val = { 42, "Test" };
	EC_LOG_DEBUG(logger, "DEBUG: id=%d, name=%s", val.id, val.name);
	EC_LOG_INFO(logger, "INFO: id=%d, name=%s", val.id, val.name);
	EC_LOG_WARN(logger, "WARN: id=%d, name=%s", val.id, val.name);
	EC_LOG_ERROR(logger, "ERROR: id=%d, name=%s", val.id, val.name);
	EC_LOG_FATAL(logger, "FATAL: id=%d, name=%s", val.id, val.name);

	ec_log_shutdown(logger);
	printf("[PASS] test_log_all_levels_struct\n");
}

/* Test callback with struct */
static int callback_called_struct = 0;
static int
log_callback_struct(void* arg, ec_log_level_t level, const char* fmt, va_list args)
{
	(void)arg;
	(void)level;
	(void)fmt;
	(void)args;
	callback_called_struct++;
	return 0;
}

static void
test_log_callback_struct(void)
{
	printf("[RUN] test_log_callback_struct\n");
	ec_log_t* logger = NULL;
	TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, false, NULL, NULL, log_callback_struct) == EC_OK);
	person_log_t val = { 42, "Callback" };
	EC_LOG_INFO(logger, "Callback: id=%d, name=%s", val.id, val.name);
	TEST_ASSERT(callback_called_struct > 0);
	ec_log_shutdown(logger);
	printf("[PASS] test_log_callback_struct\n");
}

/* Test file logging with struct */
static void
test_log_file_struct(void)
{
	printf("[RUN] test_log_file_struct\n");
	ec_log_t* logger = NULL;
	TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, false, "test_log_struct.log", NULL, NULL) == EC_OK);
	person_log_t val = { 12345, "FileTest" };
	EC_LOG_INFO(logger, "File: id=%d, name=%s", val.id, val.name);
	FILE* file = fopen("test_log_struct.log", "r");
	TEST_ASSERT(file != NULL);
	fclose(file);
	ec_log_shutdown(logger);
	printf("[PASS] test_log_file_struct\n");
}

/* Test prevent duplicate files */
static void
test_log_duplicate_file_struct(void)
{
	printf("[RUN] test_log_duplicate_file_struct\n");
	ec_log_t *logger1 = NULL, *logger2 = NULL;
	TEST_ASSERT(ec_log_init(&logger1, EC_LOG_INFO, true, "test_log_struct.log", NULL, NULL) == EC_OK);
	TEST_ASSERT(ec_log_init(&logger2, EC_LOG_INFO, true, "test_log_struct.log", NULL, NULL) == EC_ERR_INVALID_OP);
	ec_log_shutdown(logger1);
	printf("[PASS] test_log_duplicate_file_struct\n");
}

int
main(void)
{
	printf("\nStarting `ec_log_struct` Tests...\n");
	cleanup_logs();
	test_log_init_pos();
	test_log_init_neg();
	test_log_struct_values_pos();
	test_log_struct_values_neg();
	test_log_set_level_pos();
	test_log_set_level_neg();
	test_log_shutdown_pos();
	test_log_shutdown_neg();
	test_log_all_levels_struct();
	test_log_callback_struct();
	test_log_file_struct();
	test_log_duplicate_file_struct();
	cleanup_logs();
	printf("\nAll `ec_log_struct` tests passed successfully!\n");
	return 0;
}
