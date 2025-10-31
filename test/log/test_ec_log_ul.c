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
	DELETE_FILE("test_log_ul.log");
	DELETE_FILE("test_log_ul_neg.log");
}
static void
test_log_init_pos(void)
{
	{
		printf("[RUN] test_log_init_pos\n");
		ec_log_pool_init();
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, true, "test_log_ul.log", NULL, NULL) == EC_OK);
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
		TEST_ASSERT(ec_log_init(NULL, EC_LOG_INFO, true, "test_log_ul_neg.log", NULL, NULL) == EC_ERR_INVALID_ARG);
		printf("[PASS] test_log_init_neg\n");
	}
}
static void
test_log_ul_values_pos(void)
{
	{
		printf("[RUN] test_log_ul_values_pos\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_DEBUG, true, "test_log_ul.log", NULL, NULL) == EC_OK);
		unsigned long val1 = 42UL, val2 = 100UL, val3 = 0UL, val4 = 18446744073709551615UL;
		EC_LOG_INFO(logger, "Value: %lu", val1);
		EC_LOG_INFO(logger, "Value: %lu", val2);
		EC_LOG_INFO(logger, "Zero: %lu", val3);
		EC_LOG_INFO(logger, "Max: %lu", val4);
		EC_LOG_INFO(logger, "Multiple: %lu, %lu, %lu", val1, val2, val3);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_ul_values_pos\n");
	}
}
static void
test_log_ul_values_neg(void)
{
	{
		printf("[RUN] test_log_ul_values_neg\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_WARN, true, "test_log_ul.log", NULL, NULL) == EC_OK);
		unsigned long val = 42UL;
		EC_LOG_DEBUG(logger, "No: %lu", val);
		EC_LOG_INFO(logger, "No: %lu", val);
		EC_LOG_WARN(logger, "Yes: %lu", val);
		EC_LOG_ERROR(logger, "Yes: %lu", val);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_ul_values_neg\n");
	}
}
static void
test_log_set_level_pos(void)
{
	{
		printf("[RUN] test_log_set_level_pos\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_ERROR, true, "test_log_ul.log", NULL, NULL) == EC_OK);
		unsigned long val = 123UL;
		EC_LOG_ERROR(logger, "Error: %lu", val);
		ec_log_set_level(logger, EC_LOG_DEBUG);
		EC_LOG_DEBUG(logger, "Debug: %lu", val);
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
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, true, "test_log_ul.log", NULL, NULL) == EC_OK);
		unsigned long val = 999UL;
		EC_LOG_INFO(logger, "Before: %lu", val);
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
test_log_all_levels_ul(void)
{
	{
		printf("[RUN] test_log_all_levels_ul\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_DEBUG, true, "test_log_ul.log", NULL, NULL) == EC_OK);
		unsigned long val = 42UL;
		EC_LOG_DEBUG(logger, "DEBUG: %lu", val);
		EC_LOG_INFO(logger, "INFO: %lu", val);
		EC_LOG_WARN(logger, "WARN: %lu", val);
		EC_LOG_ERROR(logger, "ERROR: %lu", val);
		EC_LOG_FATAL(logger, "FATAL: %lu", val);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_all_levels_ul\n");
	}
}
static int callback_called_ul = 0;
static int
log_callback_ul(void* arg, ec_log_level_t level, const char* fmt, va_list args)
{
	{
		(void)arg;
		(void)level;
		(void)fmt;
		(void)args;
		callback_called_ul++;
		return 0;
	}
}
static void
test_log_callback_ul(void)
{
	{
		printf("[RUN] test_log_callback_ul\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, false, NULL, NULL, log_callback_ul) == EC_OK);
		unsigned long val = 42UL;
		EC_LOG_INFO(logger, "Callback: %lu", val);
		TEST_ASSERT(callback_called_ul > 0);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_callback_ul\n");
	}
}
static void
test_log_file_ul(void)
{
	{
		printf("[RUN] test_log_file_ul\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, false, "test_log_ul.log", NULL, NULL) == EC_OK);
		unsigned long val = 12345UL;
		EC_LOG_INFO(logger, "File: %lu", val);
		FILE* file = fopen("test_log_ul.log", "r");
		TEST_ASSERT(file != NULL);
		fclose(file);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_file_ul\n");
	}
}
static void
test_log_duplicate_file_ul(void)
{
	{
		printf("[RUN] test_log_duplicate_file_ul\n");
		ec_log_t *logger1 = NULL, *logger2 = NULL;
		TEST_ASSERT(ec_log_init(&logger1, EC_LOG_INFO, true, "test_log_ul.log", NULL, NULL) == EC_OK);
		TEST_ASSERT(ec_log_init(&logger2, EC_LOG_INFO, true, "test_log_ul.log", NULL, NULL) == EC_ERR_INVALID_OP);
		ec_log_shutdown(logger1);
		printf("[PASS] test_log_duplicate_file_ul\n");
	}
}
int
main(void)
{
	{
		printf("\nStarting `ec_log_ul` Tests...\n");
		cleanup_logs();
		test_log_init_pos();
		test_log_init_neg();
		test_log_ul_values_pos();
		test_log_ul_values_neg();
		test_log_set_level_pos();
		test_log_set_level_neg();
		test_log_shutdown_pos();
		test_log_shutdown_neg();
		test_log_all_levels_ul();
		test_log_callback_ul();
		test_log_file_ul();
		test_log_duplicate_file_ul();
		cleanup_logs();
		printf("\nAll `ec_log_ul` tests passed successfully!\n");
		return 0;
	}
}
