#ifndef EC_QUEUE_H
#define EC_QUEUE_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ec_status.h"
#include "ec_common.h"

#ifndef EC_QUEUE_GROWTH_FACTOR
#define EC_QUEUE_GROWTH_FACTOR 2
#endif

/*
 * EC_QUEUE_DEF(TYPE, NAME):
 * Creates a dynamic ring-buffer queue.
 *
 * Usage:
 *   ec_queue_int_t q;
 *   ec_queue_int_init(&q, 16, NULL);
 *   q.enqueue(&q, 42);
 *   q.dequeue(&q, &value);
 *   q.free_fn(&q);
 */

#define EC_QUEUE_DEF(TYPE, NAME)                                                   \
                                                                                  \
typedef struct ec_queue_##NAME##_s ec_queue_##NAME##_t;                           \
                                                                                  \
struct ec_queue_##NAME##_s {                                                      \
    TYPE          *data;                                                          \
    size_t         size;    /* number of items */                                 \
    size_t         capacity;                                                      \
    size_t         head;    /* ring head */                                       \
    size_t         tail;    /* ring tail */                                       \
    EC_MUTEX_TYPE  lock;                                                          \
    void          *mempool;                                                      \
                                                                                  \
    ec_status_t (*enqueue)(ec_queue_##NAME##_t *self, TYPE val);                  \
    ec_status_t (*dequeue)(ec_queue_##NAME##_t *self, TYPE *out);                 \
    ec_status_t (*front)(ec_queue_##NAME##_t *self, TYPE *out);                   \
    size_t     (*size_fn)(ec_queue_##NAME##_t *self);                             \
    size_t     (*capacity_fn)(ec_queue_##NAME##_t *self);                         \
    bool       (*is_empty)(ec_queue_##NAME##_t *self);                            \
    ec_status_t(*free_fn)(ec_queue_##NAME##_t *self);                             \
};                                                                                \
                                                                                  \
static inline ec_status_t ec_queue_##NAME##_init(ec_queue_##NAME##_t *self, size_t capacity, void *mempool); \
static inline void        ec_queue_##NAME##_set_functions(ec_queue_##NAME##_t *self); \
static inline ec_status_t ec_queue_##NAME##_enqueue(ec_queue_##NAME##_t *self, TYPE val); \
static inline ec_status_t ec_queue_##NAME##_dequeue(ec_queue_##NAME##_t *self, TYPE *out); \
static inline ec_status_t ec_queue_##NAME##_front(ec_queue_##NAME##_t *self, TYPE *out);  \
static inline size_t      ec_queue_##NAME##_size(ec_queue_##NAME##_t *self);             \
static inline size_t      ec_queue_##NAME##_capacity(ec_queue_##NAME##_t *self);         \
static inline bool        ec_queue_##NAME##_is_empty(ec_queue_##NAME##_t *self);         \
static inline ec_status_t ec_queue_##NAME##_free(ec_queue_##NAME##_t *self);             \
                                                                                  \
static inline void ec_queue_##NAME##_lock(ec_queue_##NAME##_t *self)   { EC_MUTEX_LOCK(self->lock);   } \
static inline void ec_queue_##NAME##_unlock(ec_queue_##NAME##_t *self) { EC_MUTEX_UNLOCK(self->lock); } \
                                                                                  \
static inline ec_status_t ec_queue_##NAME##_init(ec_queue_##NAME##_t *self, size_t capacity, void *mempool) { \
    if (!self || capacity == 0) return EC_ERR_INVALID_ARG;                         \
    self->size = 0;                                                               \
    self->capacity = capacity;                                                    \
    self->head = 0;                                                               \
    self->tail = 0;                                                               \
    self->mempool = mempool;                                                      \
    EC_MUTEX_INIT(self->lock);                                                    \
    if (mempool) {                                                                \
        self->data = (TYPE*) ec_mempool_alloc((ec_mempool_t*)mempool, capacity * sizeof(TYPE)); \
    } else {                                                                      \
        self->data = (TYPE*) malloc(capacity * sizeof(TYPE));                     \
    }                                                                             \
    if (!self->data) {                                                            \
        EC_MUTEX_DESTROY(self->lock);                                             \
        return EC_ERR_ALLOC;                                                      \
    }                                                                             \
    ec_queue_##NAME##_set_functions(self);                                         \
    return EC_OK;                                                                 \
}                                                                                  \
                                                                                  \
static inline ec_status_t ec_queue_##NAME##_enqueue(ec_queue_##NAME##_t *self, TYPE val) { \
    if (!self) return EC_ERR_INVALID_ARG;                                          \
    ec_queue_##NAME##_lock(self);                                                 \
    if (self->size >= self->capacity) {                                           \
        size_t new_cap = self->capacity * EC_QUEUE_GROWTH_FACTOR;                 \
        TYPE *new_data = (TYPE*) realloc(self->data, new_cap * sizeof(TYPE));     \
        if (!new_data) {                                                          \
            ec_queue_##NAME##_unlock(self);                                       \
            return EC_ERR_ALLOC;                                                  \
        }                                                                         \
        /* Rebuild ring if head != 0 => reposition elements from head..end to index 0 */ \
        if (self->head > 0) {                                                     \
            size_t right_count = self->capacity - self->head;                     \
            memmove(new_data, &new_data[self->head], right_count * sizeof(TYPE)); \
            if (self->tail < self->head) {                                        \
                /* elements from 0..(tail-1) move after that block */             \
                memmove(&new_data[right_count], new_data, self->tail * sizeof(TYPE)); \
                self->tail = right_count + self->tail;                            \
            }                                                                     \
            self->head = 0;                                                       \
        }                                                                         \
        self->data = new_data;                                                    \
        self->capacity = new_cap;                                                \
    }                                                                             \
    size_t pos = (self->head + self->size) % self->capacity;                      \
    self->data[pos] = val;                                                       \
    self->size++;                                                                 \
    self->tail = (self->tail + 1) % self->capacity;                               \
    ec_queue_##NAME##_unlock(self);                                               \
    return EC_OK;                                                                 \
}                                                                                  \
                                                                                  \
static inline ec_status_t ec_queue_##NAME##_dequeue(ec_queue_##NAME##_t *self, TYPE *out) { \
    if (!self || !out) return EC_ERR_INVALID_ARG;                                  \
    ec_queue_##NAME##_lock(self);                                                 \
    if (self->size == 0) {                                                        \
        ec_queue_##NAME##_unlock(self);                                           \
        return EC_ERR_OUT_OF_BOUNDS;                                              \
    }                                                                             \
    *out = self->data[self->head];                                                \
    self->head = (self->head + 1) % self->capacity;                               \
    self->size--;                                                                 \
    ec_queue_##NAME##_unlock(self);                                               \
    return EC_OK;                                                                 \
}                                                                                  \
                                                                                  \
static inline ec_status_t ec_queue_##NAME##_front(ec_queue_##NAME##_t *self, TYPE *out) { \
    if (!self || !out) return EC_ERR_INVALID_ARG;                                  \
    ec_queue_##NAME##_lock(self);                                                 \
    if (self->size == 0) {                                                        \
        ec_queue_##NAME##_unlock(self);                                           \
        return EC_ERR_OUT_OF_BOUNDS;                                              \
    }                                                                             \
    *out = self->data[self->head];                                                \
    ec_queue_##NAME##_unlock(self);                                               \
    return EC_OK;                                                                 \
}                                                                                  \
                                                                                  \
static inline size_t ec_queue_##NAME##_size(ec_queue_##NAME##_t *self) {          \
    if (!self) return 0;                                                          \
    ec_queue_##NAME##_lock(self);                                                 \
    size_t sz = self->size;                                                       \
    ec_queue_##NAME##_unlock(self);                                               \
    return sz;                                                                    \
}                                                                                  \
                                                                                  \
static inline size_t ec_queue_##NAME##_capacity(ec_queue_##NAME##_t *self) {      \
    if (!self) return 0;                                                          \
    ec_queue_##NAME##_lock(self);                                                 \
    size_t cap = self->capacity;                                                  \
    ec_queue_##NAME##_unlock(self);                                               \
    return cap;                                                                   \
}                                                                                  \
                                                                                  \
static inline bool ec_queue_##NAME##_is_empty(ec_queue_##NAME##_t *self) {        \
    if (!self) return true;                                                       \
    ec_queue_##NAME##_lock(self);                                                 \
    bool empty = (self->size == 0);                                               \
    ec_queue_##NAME##_unlock(self);                                               \
    return empty;                                                                 \
}                                                                                  \
                                                                                  \
static inline ec_status_t ec_queue_##NAME##_free(ec_queue_##NAME##_t *self) {     \
    if (!self) return EC_ERR_INVALID_ARG;                                          \
    ec_queue_##NAME##_lock(self);                                                 \
    if (self->data) {                                                             \
        if (self->mempool) {                                                      \
            ec_mempool_free((ec_mempool_t*)self->mempool, self->data);            \
        } else {                                                                  \
            free(self->data);                                                     \
        }                                                                         \
        self->data = NULL;                                                        \
    }                                                                             \
    self->size = 0;                                                               \
    self->capacity = 0;                                                           \
    self->head = 0;                                                               \
    self->tail = 0;                                                               \
    ec_queue_##NAME##_unlock(self);                                               \
    EC_MUTEX_DESTROY(self->lock);                                                 \
    return EC_OK;                                                                 \
}                                                                                  \
                                                                                  \
static inline void ec_queue_##NAME##_set_functions(ec_queue_##NAME##_t *self) {    \
    self->enqueue     = ec_queue_##NAME##_enqueue;                                 \
    self->dequeue     = ec_queue_##NAME##_dequeue;                                 \
    self->front       = ec_queue_##NAME##_front;                                   \
    self->size_fn     = ec_queue_##NAME##_size;                                    \
    self->capacity_fn = ec_queue_##NAME##_capacity;                                \
    self->is_empty    = ec_queue_##NAME##_is_empty;                                \
    self->free_fn     = ec_queue_##NAME##_free;                                    \
}

/* Provide expansions for typical types */
EC_QUEUE_DEF(int, int)
EC_QUEUE_DEF(unsigned int, uint)
EC_QUEUE_DEF(long, long)
EC_QUEUE_DEF(long long, ll)
EC_QUEUE_DEF(unsigned long, ulong)
EC_QUEUE_DEF(unsigned long long, ull)
EC_QUEUE_DEF(uint32_t, u32)
EC_QUEUE_DEF(uint64_t, u64)
EC_QUEUE_DEF(double, double)
EC_QUEUE_DEF(const char*, str)
EC_QUEUE_DEF(void*, ptr)

#endif /* EC_QUEUE_H */
