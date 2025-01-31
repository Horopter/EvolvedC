#ifndef EC_COMMON_H
#define EC_COMMON_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#include "ec_status.h"

/* Custom Memory Functions */
#ifndef EC_MALLOC
#define EC_MALLOC  malloc
#define EC_CALLOC  calloc
#define EC_REALLOC realloc
#define EC_FREE    free
#endif

/* Thread-Safety */
#ifdef EC_THREADSAFE
#ifdef _WIN32
#define EC_MUTEX_TYPE CRITICAL_SECTION
#define EC_MUTEX_INIT(m)    InitializeCriticalSection(&(m))
#define EC_MUTEX_LOCK(m)    EnterCriticalSection(&(m))
#define EC_MUTEX_UNLOCK(m)  LeaveCriticalSection(&(m))
#define EC_MUTEX_DESTROY(m) DeleteCriticalSection(&(m))
#else
#define EC_MUTEX_TYPE       pthread_mutex_t
#define EC_MUTEX_INIT(m)    pthread_mutex_init(&(m), NULL)
#define EC_MUTEX_LOCK(m)    pthread_mutex_lock(&(m))
#define EC_MUTEX_UNLOCK(m)  pthread_mutex_unlock(&(m))
#define EC_MUTEX_DESTROY(m) pthread_mutex_destroy(&(m))
#endif
#else
#define EC_MUTEX_TYPE       char
#define EC_MUTEX_INIT(m)    ((void)0)
#define EC_MUTEX_LOCK(m)    ((void)0)
#define EC_MUTEX_UNLOCK(m)  ((void)0)
#define EC_MUTEX_DESTROY(m) ((void)0)
#endif

#ifdef EC_USE_MEMPOOL
/* Use mempool allocation if enabled */
#define EC_GET_MEMPOOL(BODY) BODY
#else
#define EC_GET_MEMPOOL(BODY) NULL
#endif


#endif /* EC_COMMON_H */
