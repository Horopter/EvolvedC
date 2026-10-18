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
	DELETE_FILE("test_log_uint.log");
	DELETE_FILE("test_log_uint_neg.log");
}
static void
test_log_init_pos(void)
{
	{
		printf("[RUN] test_log_init_pos\n");
		ec_log_pool_init();
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, true, "test_log_uint.log", NULL, NULL) == EC_OK);
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
		TEST_ASSERT(ec_log_init(NULL, EC_LOG_INFO, true, "test_log_uint_neg.log", NULL, NULL) == EC_ERR_INVALID_ARG);
		printf("[PASS] test_log_init_neg\n");
	}
}
static void
test_log_uint_values_pos(void)
{
	{
		printf("[RUN] test_log_uint_values_pos\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_DEBUG, true, "test_log_uint.log", NULL, NULL) == EC_OK);
		unsigned int val1 = 42u, val2 = 100u, val3 = 0u, val4 = 4294967295u;
		EC_LOG_INFO(logger, "Value: %u", val1);
		EC_LOG_INFO(logger, "Value: %u", val2);
		EC_LOG_INFO(logger, "Zero: %u", val3);
		EC_LOG_INFO(logger, "Max: %u", val4);
		EC_LOG_INFO(logger, "Multiple: %u, %u, %u", val1, val2, val3);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_uint_values_pos\n");
	}
}
static void
test_log_uint_values_neg(void)
{
	{
		printf("[RUN] test_log_uint_values_neg\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_WARN, true, "test_log_uint.log", NULL, NULL) == EC_OK);
		unsigned int val = 42u;
		EC_LOG_DEBUG(logger, "No: %u", val);
		EC_LOG_INFO(logger, "No: %u", val);
		EC_LOG_WARN(logger, "Yes: %u", val);
		EC_LOG_ERROR(logger, "Yes: %u", val);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_uint_values_neg\n");
	}
}
static void
test_log_set_level_pos(void)
{
	{
		printf("[RUN] test_log_set_level_pos\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_ERROR, true, "test_log_uint.log", NULL, NULL) == EC_OK);
		unsigned int val = 123u;
		EC_LOG_ERROR(logger, "Error: %u", val);
		ec_log_set_level(logger, EC_LOG_DEBUG);
		EC_LOG_DEBUG(logger, "Debug: %u", val);
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
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, true, "test_log_uint.log", NULL, NULL) == EC_OK);
		unsigned int val = 999u;
		EC_LOG_INFO(logger, "Before: %u", val);
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
test_log_all_levels_uint(void)
{
	{
		printf("[RUN] test_log_all_levels_uint\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_DEBUG, true, "test_log_uint.log", NULL, NULL) == EC_OK);
		unsigned int val = 42u;
		EC_LOG_DEBUG(logger, "DEBUG: %u", val);
		EC_LOG_INFO(logger, "INFO: %u", val);
		EC_LOG_WARN(logger, "WARN: %u", val);
		EC_LOG_ERROR(logger, "ERROR: %u", val);
		EC_LOG_FATAL(logger, "FATAL: %u", val);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_all_levels_uint\n");
	}
}
static int callback_called_uint = 0;
static int
log_callback_uint(void* arg, ec_log_level_t level, const char* fmt, va_list args)
{
	{
		(void)arg;
		(void)level;
		(void)fmt;
		(void)args;
		callback_called_uint++;
		return 0;
	}
}
static void
test_log_callback_uint(void)
{
	{
		printf("[RUN] test_log_callback_uint\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, false, NULL, NULL, log_callback_uint) == EC_OK);
		unsigned int val = 42u;
		EC_LOG_INFO(logger, "Callback: %u", val);
		TEST_ASSERT(callback_called_uint > 0);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_callback_uint\n");
	}
}
static void
test_log_file_uint(void)
{
	{
		printf("[RUN] test_log_file_uint\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, false, "test_log_uint.log", NULL, NULL) == EC_OK);
		unsigned int val = 12345u;
		EC_LOG_INFO(logger, "File: %u", val);
		FILE* file = fopen("test_log_uint.log", "r");
		TEST_ASSERT(file != NULL);
		fclose(file);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_file_uint\n");
	}
}
static void
test_log_duplicate_file_uint(void)
{
	{
		printf("[RUN] test_log_duplicate_file_uint\n");
		ec_log_t *logger1 = NULL, *logger2 = NULL;
		TEST_ASSERT(ec_log_init(&logger1, EC_LOG_INFO, true, "test_log_uint.log", NULL, NULL) == EC_OK);
		TEST_ASSERT(ec_log_init(&logger2, EC_LOG_INFO, true, "test_log_uint.log", NULL, NULL) == EC_ERR_INVALID_OP);
		ec_log_shutdown(logger1);
		printf("[PASS] test_log_duplicate_file_uint\n");
	}
}
int
main(void)
{
	{
		printf("\nStarting `ec_log_uint` Tests...\n");
		cleanup_logs();
		test_log_init_pos();
		test_log_init_neg();
		test_log_uint_values_pos();
		test_log_uint_values_neg();
		test_log_set_level_pos();
		test_log_set_level_neg();
		test_log_shutdown_pos();
		test_log_shutdown_neg();
		test_log_all_levels_uint();
		test_log_callback_uint();
		test_log_file_uint();
		test_log_duplicate_file_uint();
		cleanup_logs();
		printf("\nAll `ec_log_uint` tests passed successfully!\n");
		return 0;
	}
}
