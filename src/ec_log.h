#ifndef EC_LOG_H
#define EC_LOG_H

#include "ec_common.h"
#include "ec_status.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

/* Maximum loggers allowed */
#ifndef EC_LOG_MAX_LOGGERS
#define EC_LOG_MAX_LOGGERS 16
#endif

/* Handle Windows (MSVC) and MinGW separately */
#include <time.h>

#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__)
    // Windows & MinGW: Use localtime() and gmtime()
    #define LOCALTIME_SAFE(timep, result) do { \
        time_t _t = *(timep); \
        struct tm *_tmp = localtime(&_t); \
        if (_tmp) *(result) = *_tmp; \
    } while (0)

    #define GMTIME_SAFE(timep, result) do { \
        time_t _t = *(timep); \
        struct tm *_tmp = gmtime(&_t); \
        if (_tmp) *(result) = *_tmp; \
    } while (0)

#elif defined(__linux__) || defined(__APPLE__)
    // Linux/macOS: Use standard thread-safe localtime_r() and gmtime_r()
    #define LOCALTIME_SAFE(timep, result) localtime_r(timep, result)
    #define GMTIME_SAFE(timep, result) gmtime_r(timep, result)
#else
    #error "Unsupported platform"
#endif

/* Log Levels */
typedef enum {
    EC_LOG_DEBUG,
    EC_LOG_INFO,
    EC_LOG_WARN,
    EC_LOG_ERROR,
    EC_LOG_FATAL
} ec_log_level_t;

/* Logger Structure */
typedef struct {
    FILE *file;
    char filename[256];  
    bool to_stdout;
    ec_log_level_t level;
    void *arg;
    int (*callback)(void *, ec_log_level_t, const char *, va_list);
    EC_MUTEX_TYPE lock;
} ec_log_t;

/* Logger Pool */
static struct {
    ec_log_t *loggers[EC_LOG_MAX_LOGGERS];
    int count;
    EC_MUTEX_TYPE lock;
    bool initialized;
} ec_logger_pool = { .count = 0, .initialized = false };

/* Initialize Logger Pool */
static inline void ec_log_pool_init(void) {
    if (!ec_logger_pool.initialized) {
        EC_MUTEX_INIT(ec_logger_pool.lock);
        ec_logger_pool.initialized = true;
    }
}

/* Check if file is already used */
static inline bool ec_log_is_file_used(const char *filename) {
    EC_MUTEX_LOCK(ec_logger_pool.lock);
    for (int i = 0; i < ec_logger_pool.count; i++) {
        if (strcmp(ec_logger_pool.loggers[i]->filename, filename) == 0) {
            EC_MUTEX_UNLOCK(ec_logger_pool.lock);
            return true;
        }
    }
    EC_MUTEX_UNLOCK(ec_logger_pool.lock);
    return false;
}

/* Register a logger */
static inline void ec_log_register_logger(ec_log_t *logger) {
    EC_MUTEX_LOCK(ec_logger_pool.lock);
    if (ec_logger_pool.count < EC_LOG_MAX_LOGGERS) {
        ec_logger_pool.loggers[ec_logger_pool.count++] = logger;
    }
    EC_MUTEX_UNLOCK(ec_logger_pool.lock);
}

/* Unregister a logger */
static inline void ec_log_unregister_logger(ec_log_t *logger) {
    EC_MUTEX_LOCK(ec_logger_pool.lock);
    for (int i = 0; i < ec_logger_pool.count; i++) {
        if (ec_logger_pool.loggers[i] == logger) {
            ec_logger_pool.loggers[i] = ec_logger_pool.loggers[--ec_logger_pool.count];
            break;
        }
    }
    EC_MUTEX_UNLOCK(ec_logger_pool.lock);
}

/* Initialize Logger */
static inline ec_status_t ec_log_init(
    ec_log_t **logger, 
    ec_log_level_t log_level, 
    bool to_stdout, 
    const char *filename, 
    void *arg, 
    int (*callback)(void *, ec_log_level_t, const char *, va_list)
) {
    if (!logger) return EC_ERR_INVALID_ARG;

    if (*logger == NULL) {
        *logger = (ec_log_t *)EC_MALLOC(sizeof(ec_log_t));
        if (!(*logger)) return EC_ERR_ALLOC;
    }

    (*logger)->to_stdout = to_stdout;
    (*logger)->level = log_level;
    (*logger)->arg = arg;
    (*logger)->callback = callback;
    (*logger)->file = NULL;
    EC_MUTEX_INIT((*logger)->lock);

    if (filename) {
        if (ec_log_is_file_used(filename)) {
            return EC_ERR_INVALID_OP;  // File already in use
        }

        (*logger)->file = fopen(filename, "a");
        if (!(*logger)->file) {
            EC_FREE(*logger);
            return EC_ERR_ALLOC;
        }

        strncpy((*logger)->filename, filename, sizeof((*logger)->filename) - 1);
        (*logger)->filename[sizeof((*logger)->filename) - 1] = '\0';  // Ensure null termination
        ec_log_register_logger(*logger);
    } else {
        (*logger)->filename[0] = '\0';
    }

    return EC_OK;
}

/* Shutdown Logger */
static inline void ec_log_shutdown(ec_log_t *logger) {
    if (!logger) return;

    EC_MUTEX_LOCK(logger->lock);
    
    if (logger->file) {
        fclose(logger->file);
        ec_log_unregister_logger(logger);
        logger->file = NULL;
    }

    EC_MUTEX_UNLOCK(logger->lock);
    EC_MUTEX_DESTROY(logger->lock);
    EC_FREE(logger);
}

/* Set Log Level */
static inline void ec_log_set_level(ec_log_t *logger, ec_log_level_t level) {
    if (!logger) return;
    EC_MUTEX_LOCK(logger->lock);
    logger->level = level;
    EC_MUTEX_UNLOCK(logger->lock);
}

/* Get Timestamp with UTC Offset */
static inline void ec_log_timestamp(char *buffer, size_t size) {
    time_t t = time(NULL);
    struct tm tm_local, tm_utc;

    LOCALTIME_SAFE(&t, &tm_local);
    GMTIME_SAFE(&t, &tm_utc);

    int offset_sec = (int)difftime(mktime(&tm_local), mktime(&tm_utc));
    int offset_hour = offset_sec / 3600;
    int offset_min = (offset_sec % 3600) / 60;

    snprintf(buffer, size, "%04d-%02d-%02d %02d:%02d:%02d %+03d:%02d",
             tm_local.tm_year + 1900, tm_local.tm_mon + 1, tm_local.tm_mday,
             tm_local.tm_hour, tm_local.tm_min, tm_local.tm_sec,
             offset_hour, offset_min);
}

/* Internal Log Writer */
static inline void ec_log_write(ec_log_t *logger, ec_log_level_t level, const char *fmt, ...) {
    if (!logger || level < logger->level) return;

    EC_MUTEX_LOCK(logger->lock);
    
    char timestamp[64];
    ec_log_timestamp(timestamp, sizeof(timestamp));

    va_list args;

    /* Log to stdout */
    if (logger->to_stdout) {
        FILE *stream = (level >= EC_LOG_ERROR) ? stderr : stdout;
        fprintf(stream, "[%s] [%d] ", timestamp, level);
        va_start(args, fmt);
        vfprintf(stream, fmt, args);
        va_end(args);
        fprintf(stream, "\n");
    }

    /* Log to file */
    if (logger->file) {
        fprintf(logger->file, "[%s] [%d] ", timestamp, level);
        va_start(args, fmt);
        vfprintf(logger->file, fmt, args);
        va_end(args);
        fprintf(logger->file, "\n");
        fflush(logger->file);
    }

    /* Log via callback */
    if (logger->callback) {
        va_start(args, fmt);
        logger->callback(logger->arg, level, fmt, args);
        va_end(args);
    }

    EC_MUTEX_UNLOCK(logger->lock);
}

/* Convenience Macros */
#define EC_LOG_DEBUG(logger, fmt, ...) ec_log_write(logger, EC_LOG_DEBUG, fmt, ##__VA_ARGS__)
#define EC_LOG_INFO(logger, fmt, ...)  ec_log_write(logger, EC_LOG_INFO, fmt, ##__VA_ARGS__)
#define EC_LOG_WARN(logger, fmt, ...)  ec_log_write(logger, EC_LOG_WARN, fmt, ##__VA_ARGS__)
#define EC_LOG_ERROR(logger, fmt, ...) ec_log_write(logger, EC_LOG_ERROR, fmt, ##__VA_ARGS__)
#define EC_LOG_FATAL(logger, fmt, ...) ec_log_write(logger, EC_LOG_FATAL, fmt, ##__VA_ARGS__)

#endif // EC_LOG_H
