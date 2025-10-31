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
	DELETE_FILE("test_log_char.log");
	DELETE_FILE("test_log_char_neg.log");
}
static void
test_log_init_pos(void)
{
	{
		printf("[RUN] test_log_init_pos\n");
		ec_log_pool_init();
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, true, "test_log_char.log", NULL, NULL) == EC_OK);
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
		TEST_ASSERT(ec_log_init(NULL, EC_LOG_INFO, true, "test_log_char_neg.log", NULL, NULL) == EC_ERR_INVALID_ARG);
		printf("[PASS] test_log_init_neg\n");
	}
}
static void
test_log_char_values_pos(void)
{
	{
		printf("[RUN] test_log_char_values_pos\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_DEBUG, true, "test_log_char.log", NULL, NULL) == EC_OK);
		char val1 = 'A', val2 = 'z', val3 = '0', val4 = '\n';
		EC_LOG_INFO(logger, "Value: %c", val1);
		EC_LOG_INFO(logger, "Value: %c", val2);
		EC_LOG_INFO(logger, "Zero: %c", val3);
		EC_LOG_INFO(logger, "Newline: %c", val4);
		EC_LOG_INFO(logger, "Multiple: %c, %c, %c", val1, val2, val3);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_char_values_pos\n");
	}
}
static void
test_log_char_values_neg(void)
{
	{
		printf("[RUN] test_log_char_values_neg\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_WARN, true, "test_log_char.log", NULL, NULL) == EC_OK);
		char val = 'A';
		EC_LOG_DEBUG(logger, "No: %c", val);
		EC_LOG_INFO(logger, "No: %c", val);
		EC_LOG_WARN(logger, "Yes: %c", val);
		EC_LOG_ERROR(logger, "Yes: %c", val);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_char_values_neg\n");
	}
}
static void
test_log_set_level_pos(void)
{
	{
		printf("[RUN] test_log_set_level_pos\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_ERROR, true, "test_log_char.log", NULL, NULL) == EC_OK);
		char val = 'X';
		EC_LOG_ERROR(logger, "Error: %c", val);
		ec_log_set_level(logger, EC_LOG_DEBUG);
		EC_LOG_DEBUG(logger, "Debug: %c", val);
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
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, true, "test_log_char.log", NULL, NULL) == EC_OK);
		char val = 'Z';
		EC_LOG_INFO(logger, "Before: %c", val);
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
test_log_all_levels_char(void)
{
	{
		printf("[RUN] test_log_all_levels_char\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_DEBUG, true, "test_log_char.log", NULL, NULL) == EC_OK);
		char val = 'A';
		EC_LOG_DEBUG(logger, "DEBUG: %c", val);
		EC_LOG_INFO(logger, "INFO: %c", val);
		EC_LOG_WARN(logger, "WARN: %c", val);
		EC_LOG_ERROR(logger, "ERROR: %c", val);
		EC_LOG_FATAL(logger, "FATAL: %c", val);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_all_levels_char\n");
	}
}
static int callback_called_char = 0;
static int
log_callback_char(void* arg, ec_log_level_t level, const char* fmt, va_list args)
{
	{
		(void)arg;
		(void)level;
		(void)fmt;
		(void)args;
		callback_called_char++;
		return 0;
	}
}
static void
test_log_callback_char(void)
{
	{
		printf("[RUN] test_log_callback_char\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, false, NULL, NULL, log_callback_char) == EC_OK);
		char val = 'A';
		EC_LOG_INFO(logger, "Callback: %c", val);
		TEST_ASSERT(callback_called_char > 0);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_callback_char\n");
	}
}
static void
test_log_file_char(void)
{
	{
		printf("[RUN] test_log_file_char\n");
		ec_log_t* logger = NULL;
		TEST_ASSERT(ec_log_init(&logger, EC_LOG_INFO, false, "test_log_char.log", NULL, NULL) == EC_OK);
		char val = 'X';
		EC_LOG_INFO(logger, "File: %c", val);
		FILE* file = fopen("test_log_char.log", "r");
		TEST_ASSERT(file != NULL);
		fclose(file);
		ec_log_shutdown(logger);
		printf("[PASS] test_log_file_char\n");
	}
}
static void
test_log_duplicate_file_char(void)
{
	{
		printf("[RUN] test_log_duplicate_file_char\n");
		ec_log_t *logger1 = NULL, *logger2 = NULL;
		TEST_ASSERT(ec_log_init(&logger1, EC_LOG_INFO, true, "test_log_char.log", NULL, NULL) == EC_OK);
		TEST_ASSERT(ec_log_init(&logger2, EC_LOG_INFO, true, "test_log_char.log", NULL, NULL) == EC_ERR_INVALID_OP);
		ec_log_shutdown(logger1);
		printf("[PASS] test_log_duplicate_file_char\n");
	}
}
int
main(void)
{
	{
		printf("\nStarting `ec_log_char` Tests...\n");
		cleanup_logs();
		test_log_init_pos();
		test_log_init_neg();
		test_log_char_values_pos();
		test_log_char_values_neg();
		test_log_set_level_pos();
		test_log_set_level_neg();
		test_log_shutdown_pos();
		test_log_shutdown_neg();
		test_log_all_levels_char();
		test_log_callback_char();
		test_log_file_char();
		test_log_duplicate_file_char();
		cleanup_logs();
		printf("\nAll `ec_log_char` tests passed successfully!\n");
		return 0;
	}
}
