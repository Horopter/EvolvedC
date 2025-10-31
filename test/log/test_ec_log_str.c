#include <inttypes.h>
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
	DELETE_FILE("test_log_str.log");
	DELETE_FILE("test_log_str_neg.log");
}
static void
test_log_init_pos(void)
{
	{
		printf("[RUN] test_log_init_pos\n");
		ec_log_pool_init();
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, true, "test_log_str.log", NULL, NULL) == EC_OK);
		TEST_ASSERT(logger != NULL);
		TEST_ASSERT(logger->to_stdout == true);
		TEST_ASSERT(logger->level == EC_LOG_INFO);
		TEST_ASSERT(logger->file != NULL);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_init_pos\n");
	}
}
static void
test_log_init_neg(void)
{
	{
		printf("[RUN] test_log_init_neg\n");
		ec_log_pool_init();
		TEST_ASSERT(ec_log_init(NULL, EC_LOG_INFO, true, "test_log_str_neg.log", NULL, NULL) == EC_ERR_INVALID_ARG);
		printf("[PASS] test_log_init_neg\n");
	}
}
static void
test_log_str_values_pos(void)
{
	{
		printf("[RUN] test_log_str_values_pos\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_DEBUG, true, "test_log_str.log", NULL, NULL) == EC_OK);
		const char* val1 = "Hello";
		const char* val2 = "World";
		const char* val3 = "";
		const char* val4 = "Test";
		EC_LOG_INFO(logger, "Value: %s", val1);
		EC_LOG_INFO(logger, "Value: %s", val2);
		EC_LOG_INFO(logger, "Empty: %s", val3);
		EC_LOG_INFO(logger, "Value: %s", val4);
		EC_LOG_INFO(logger, "Multiple: %s, %s, %s", val1, val2, val3);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_str_values_pos\n");
	}
}
static void
test_log_str_values_neg(void)
{
	{
		printf("[RUN] test_log_str_values_neg\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_WARN, true, "test_log_str.log", NULL, NULL) == EC_OK);
		const char* val = "test";
		EC_LOG_DEBUG(logger, "No: %s", val);
		EC_LOG_INFO(logger, "No: %s", val);
		EC_LOG_WARN(logger, "Yes: %s", val);
		EC_LOG_ERROR(logger, "Yes: %s", val);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_str_values_neg\n");
	}
}
static void
test_log_set_level_pos(void)
{
	{
		printf("[RUN] test_log_set_level_pos\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_ERROR, true, "test_log_str.log", NULL, NULL) == EC_OK);
		const char* val = "error";
		EC_LOG_ERROR(logger, "Error: %s", val);
		ec_log_set_level(logger, EC_LOG_DEBUG);
		EC_LOG_DEBUG(logger, "Debug: %s", val);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_set_level_pos\n");
	}
}
static void
test_log_set_level_neg(void)
{
	{
		printf("[RUN] test_log_set_level_neg\n");
		ec_log_set_level(NULL, EC_LOG_DEBUG);
		printf("[PASS] test_log_set_level_neg\n");
	}
}
static void
test_log_shutdown_pos(void)
{
	{
		printf("[RUN] test_log_shutdown_pos\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, true, "test_log_str.log", NULL, NULL) == EC_OK);
		const char* val = "shutdown";
		EC_LOG_INFO(logger, "Before: %s", val);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_shutdown_pos\n");
	}
}
static void
test_log_shutdown_neg(void)
{
	{
		printf("[RUN] test_log_shutdown_neg\n");
		ec_log_shutdown(NULL);
		printf("[PASS] test_log_shutdown_neg\n");
	}
}
static void
test_log_all_levels_str(void)
{
	{
		printf("[RUN] test_log_all_levels_str\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_DEBUG, true, "test_log_str.log", NULL, NULL) == EC_OK);
		const char* val = "test";
		EC_LOG_DEBUG(logger, "DEBUG: %s", val);
		EC_LOG_INFO(logger, "INFO: %s", val);
		EC_LOG_WARN(logger, "WARN: %s", val);
		EC_LOG_ERROR(logger, "ERROR: %s", val);
		EC_LOG_FATAL(logger, "FATAL: %s", val);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_all_levels_str\n");
	}
}
static int callback_called_str = 0;
static int
log_callback_str(void* arg, ec_log_level_t level, const char* fmt, va_list args)
{
	{
		(void)arg;
		(void)level;
		(void)fmt;
		(void)args;
		callback_called_str++;
		return 0;
	}
}
static void
test_log_callback_str(void)
{
	{
		printf("[RUN] test_log_callback_str\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, false, NULL, NULL, log_callback_str) == EC_OK);
		const char* val = "callback";
		EC_LOG_INFO(logger, "Callback: %s", val);
		TEST_ASSERT(callback_called_str > 0);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_callback_str\n");
	}
}
static void
test_log_file_str(void)
{
	{
		printf("[RUN] test_log_file_str\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, false, "test_log_str.log", NULL, NULL) == EC_OK);
		const char* val = "filetest";
		EC_LOG_INFO(logger, "File: %s", val);
		FILE* file = fopen("test_log_str.log", "r");
		TEST_ASSERT(file != NULL);
		fclose(file);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_file_str\n");
	}
}
static void
test_log_duplicate_file_str(void)
{
	{
		printf("[RUN] test_log_duplicate_file_str\n");
		ec_log_t *logger1 = NULL, *logger2 = NULL;
		TEST_ASSERT(ec_log_init(&logger1, EC_LOG_INFO, true, "test_log_str.log", NULL, NULL) == EC_OK);
		TEST_ASSERT(ec_log_init(&logger2, EC_LOG_INFO, true, "test_log_str.log", NULL, NULL) == EC_ERR_INVALID_OP);
		ec_log_shutdown(logger1);
		printf("[PASS] test_log_duplicate_file_str\n");
	}
}
int
main(void)
{
	{
		printf("\nStarting `ec_log_str` Tests...\n");
		cleanup_logs();
		test_log_init_pos();
		test_log_init_neg();
		test_log_str_values_pos();
		test_log_str_values_neg();
		test_log_set_level_pos();
		test_log_set_level_neg();
		test_log_shutdown_pos();
		test_log_shutdown_neg();
		test_log_all_levels_str();
		test_log_callback_str();
		test_log_file_str();
		test_log_duplicate_file_str();
		cleanup_logs();
		printf("\nAll `ec_log_str` tests passed successfully!\n");
		return 0;
	}
}
