#ifndef EC_TIMER_H
#define EC_TIMER_H

#include "ec_common.h"
#include "ec_status.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    typedef LARGE_INTEGER ec_hrtime_t;
#elif defined(__APPLE__) || defined(__linux__) || defined(__unix__) || defined(__posix__)
    #include <sys/time.h>
    #include <time.h>
    typedef struct timespec ec_hrtime_t;
#else
    #error "Unsupported platform!"
#endif

#define EC_TIMER_INVALID UINT64_MAX
#define EC_TIMER_MAX_EVENTS 1024

typedef enum {
    EC_TIMER_ONESHOT,
    EC_TIMER_PERIODIC
} ec_timer_type_t;

typedef struct {
    uint64_t timeout;
    uint64_t interval;
    ec_timer_type_t type;
    void *arg;
    void (*callback)(void *);
    bool active;
} ec_timer_event_t;

typedef struct {
    ec_timer_event_t events[EC_TIMER_MAX_EVENTS];
    uint64_t timestamp;
    EC_MUTEX_TYPE lock;
} ec_timer_t;

/* Initialize Timer */
static inline void ec_timer_init(ec_timer_t *t) {
    if (!t) return;
    memset(t, 0, sizeof(ec_timer_t));
    t->timestamp = 0;
    EC_MUTEX_INIT(t->lock);
}

/* Get High-Resolution Time */
static inline uint64_t ec_timer_now() {
#if defined(_WIN32) || defined(_WIN64)
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (counter.QuadPart * 1000000) / freq.QuadPart;
#elif defined(__APPLE__) || defined(__linux__) || defined(__unix__) || defined(__posix__)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000 + (ts.tv_nsec / 1000);
#else
    #error "Unsupported platform!"
#endif
}

/* Sleep (Milliseconds) */
static inline void EC_SLEEP_MS(uint32_t ms) {
#if defined(_WIN32) || defined(_WIN64)
    Sleep(ms);
#elif defined(__APPLE__) || defined(__linux__) || defined(__unix__) || defined(__posix__)
    struct timespec ts = {ms / 1000, (ms % 1000) * 1000000};
    nanosleep(&ts, NULL);
#endif
}

/* Sleep (Microseconds) */
static inline void EC_SLEEP_US(uint32_t us) {
#if defined(_WIN32) || defined(_WIN64)
    Sleep(us / 1000);
#elif defined(__APPLE__) || defined(__linux__) || defined(__unix__) || defined(__posix__)
    struct timespec ts = {us / 1000000, (us % 1000000) * 1000};
    nanosleep(&ts, NULL);
#endif
}

/* Start a Timer */
static inline uint64_t ec_timer_start(ec_timer_t *t, uint64_t delay_us, uint64_t interval_us,
                                      ec_timer_type_t type, void (*callback)(void *), void *arg) {
    if (!t || !callback) return EC_TIMER_INVALID;
    
    EC_MUTEX_LOCK(t->lock);
    for (int i = 0; i < EC_TIMER_MAX_EVENTS; i++) {
        if (!t->events[i].active) {
            t->events[i].timeout = ec_timer_now() + delay_us;
            t->events[i].interval = interval_us;
            t->events[i].type = type;
            t->events[i].callback = callback;
            t->events[i].arg = arg;
            t->events[i].active = true;
            EC_MUTEX_UNLOCK(t->lock);
            return i;
        }
    }
    EC_MUTEX_UNLOCK(t->lock);
    return EC_TIMER_INVALID;
}

/* Cancel a Timer */
static inline void ec_timer_cancel(ec_timer_t *t, uint64_t id) {
    if (!t || id >= EC_TIMER_MAX_EVENTS) return;
    EC_MUTEX_LOCK(t->lock);
    t->events[id].active = false;
    EC_MUTEX_UNLOCK(t->lock);
}

/* Process Timers */
static inline void ec_timer_process(ec_timer_t *t) {
    if (!t) return;
    
    uint64_t now = ec_timer_now();
    EC_MUTEX_LOCK(t->lock);
    
    for (int i = 0; i < EC_TIMER_MAX_EVENTS; i++) {
        if (t->events[i].active && t->events[i].timeout <= now) {
            t->events[i].callback(t->events[i].arg);
            if (t->events[i].type == EC_TIMER_PERIODIC) {
                t->events[i].timeout = now + t->events[i].interval;
            } else {
                t->events[i].active = false;
            }
        }
    }
    
    EC_MUTEX_UNLOCK(t->lock);
}

/* Destroy Timer */
static inline void ec_timer_destroy(ec_timer_t *t) {
    if (!t) return;
    EC_MUTEX_DESTROY(t->lock);
}

#endif /* EC_TIMER_H */
