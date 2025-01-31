#ifndef EC_HEAP_H
#define EC_HEAP_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "ec_status.h"
#include "ec_common.h"
#include "ec_mempool.h"

#define EC_HEAP_GROWTH_FACTOR 2
#define EC_HEAP_LOCK(self)    EC_MUTEX_LOCK((self)->lock)
#define EC_HEAP_UNLOCK(self)  EC_MUTEX_UNLOCK((self)->lock)

#define EC_HEAP_DEF(TYPE, NAME)                                                  \
                                                                                 \
typedef struct ec_heap_##NAME##_s ec_heap_##NAME##_t;                            \
                                                                                 \
struct ec_heap_##NAME##_s {                                                      \
    TYPE *data;                                                                  \
    size_t size;                                                                 \
    size_t capacity;                                                             \
    int (*cmp)(const TYPE*, const TYPE*);                                        \
    EC_MUTEX_TYPE lock;                                                          \
    void *mempool;                                                               \
                                                                                 \
    ec_status_t (*insert)     (ec_heap_##NAME##_t*, TYPE);                       \
    ec_status_t (*extract_top)(ec_heap_##NAME##_t*, TYPE*);                      \
    ec_status_t (*peek_top)   (ec_heap_##NAME##_t*, TYPE*);                      \
    size_t      (*size_fn)    (ec_heap_##NAME##_t*);                             \
    ec_status_t (*free_fn)    (ec_heap_##NAME##_t*);                             \
};                                                                               \
                                                                                 \
static inline ec_status_t ec_heap_##NAME##_init(ec_heap_##NAME##_t*, size_t, int (*)(const TYPE*, const TYPE*), void*); \
static inline void        ec_heap_##NAME##_set_functions(ec_heap_##NAME##_t*);   \
static inline ec_status_t ec_heap_##NAME##_insert(ec_heap_##NAME##_t*, TYPE);    \
static inline ec_status_t ec_heap_##NAME##_extract_top(ec_heap_##NAME##_t*, TYPE*); \
static inline ec_status_t ec_heap_##NAME##_peek_top(ec_heap_##NAME##_t*, TYPE*); \
static inline size_t      ec_heap_##NAME##_size(ec_heap_##NAME##_t*);            \
static inline ec_status_t ec_heap_##NAME##_free(ec_heap_##NAME##_t*);            \
                                                                                 \
static inline void ec_heap_##NAME##_heapify_up(ec_heap_##NAME##_t*, size_t);     \
static inline void ec_heap_##NAME##_heapify_down(ec_heap_##NAME##_t*, size_t);   \
                                                                                 \
static inline ec_status_t ec_heap_##NAME##_init(ec_heap_##NAME##_t *self, size_t capacity, \
    int (*cmp)(const TYPE*, const TYPE*), void *mempool) {                       \
    if (!self || !cmp || capacity == 0) return EC_ERR_INVALID_ARG;               \
    self->size = 0;                                                              \
    self->capacity = capacity;                                                   \
    self->cmp = cmp;                                                             \
    self->mempool = mempool;                                                     \
    EC_MUTEX_INIT(self->lock);                                                   \
    self->data = (mempool) ?                                                     \
        (TYPE*)ec_mempool_alloc((ec_mempool_t*)mempool, capacity * sizeof(TYPE)) : \
        (TYPE*)EC_MALLOC(capacity * sizeof(TYPE));                               \
    if (!self->data) {                                                           \
        EC_MUTEX_DESTROY(self->lock);                                            \
        return EC_ERR_ALLOC;                                                     \
    }                                                                            \
    ec_heap_##NAME##_set_functions(self);                                        \
    return EC_OK;                                                                \
}                                                                                \
                                                                                 \
static inline void ec_heap_##NAME##_set_functions(ec_heap_##NAME##_t *self) {    \
    self->insert = ec_heap_##NAME##_insert;                                      \
    self->extract_top = ec_heap_##NAME##_extract_top;                            \
    self->peek_top = ec_heap_##NAME##_peek_top;                                  \
    self->size_fn = ec_heap_##NAME##_size;                                       \
    self->free_fn = ec_heap_##NAME##_free;                                       \
}                                                                                \
                                                                                 \
static inline void ec_heap_##NAME##_heapify_up(ec_heap_##NAME##_t *self, size_t index) { \
    while (index > 0) {                                                          \
        size_t parent = (index - 1) / 2;                                         \
        if (self->cmp(&self->data[parent], &self->data[index]) <= 0) break;      \
        TYPE temp = self->data[parent];                                          \
        self->data[parent] = self->data[index];                                  \
        self->data[index] = temp;                                                \
        index = parent;                                                          \
    }                                                                            \
}                                                                                \
                                                                                 \
static inline void ec_heap_##NAME##_heapify_down(ec_heap_##NAME##_t *self, size_t index) { \
    size_t left, right, smallest;                                                \
    while (true) {                                                               \
        left = 2 * index + 1;                                                    \
        right = 2 * index + 2;                                                   \
        smallest = index;                                                        \
        if (left < self->size && self->cmp(&self->data[left], &self->data[smallest]) < 0) { \
            smallest = left;                                                     \
        }                                                                        \
        if (right < self->size && self->cmp(&self->data[right], &self->data[smallest]) < 0) { \
            smallest = right;                                                    \
        }                                                                        \
        if (smallest == index) break;                                            \
        TYPE temp = self->data[index];                                           \
        self->data[index] = self->data[smallest];                                \
        self->data[smallest] = temp;                                             \
        index = smallest;                                                        \
    }                                                                            \
}                                                                                \
                                                                                 \
static inline ec_status_t ec_heap_##NAME##_insert(ec_heap_##NAME##_t *self, TYPE val) { \
    if (!self) return EC_ERR_INVALID_ARG;                                        \
    EC_HEAP_LOCK(self);                                                          \
    if (self->size >= self->capacity) {                                          \
        size_t new_capacity = self->capacity * EC_HEAP_GROWTH_FACTOR;            \
        TYPE *new_data = (TYPE*)EC_REALLOC(self->data, new_capacity * sizeof(TYPE)); \
        if (!new_data) {                                                         \
            EC_HEAP_UNLOCK(self);                                                \
            return EC_ERR_ALLOC;                                                 \
        }                                                                        \
        self->data = new_data;                                                   \
        self->capacity = new_capacity;                                           \
    }                                                                            \
    self->data[self->size] = val;                                                \
    ec_heap_##NAME##_heapify_up(self, self->size);                               \
    self->size++;                                                                \
    EC_HEAP_UNLOCK(self);                                                        \
    return EC_OK;                                                                \
}                                                                                \
                                                                                 \
static inline ec_status_t ec_heap_##NAME##_extract_top(ec_heap_##NAME##_t *self, TYPE *out) { \
    if (!self || !out) return EC_ERR_INVALID_ARG;                                \
    EC_HEAP_LOCK(self);                                                          \
    if (self->size == 0) {                                                       \
        EC_HEAP_UNLOCK(self);                                                    \
        return EC_ERR_OUT_OF_BOUNDS;                                             \
    }                                                                            \
    *out = self->data[0];                                                        \
    self->data[0] = self->data[--self->size];                                    \
    ec_heap_##NAME##_heapify_down(self, 0);                                      \
    EC_HEAP_UNLOCK(self);                                                        \
    return EC_OK;                                                                \
}                                                                                \
                                                                                 \
static inline size_t ec_heap_##NAME##_size(ec_heap_##NAME##_t *self) {           \
    return self ? self->size : 0;                                                \
}                                                                                \
                                                                                 \
static inline ec_status_t ec_heap_##NAME##_free(ec_heap_##NAME##_t *self) {      \
    if (!self) return EC_ERR_INVALID_ARG;                                        \
    EC_HEAP_LOCK(self);                                                          \
    EC_FREE(self->data);                                                         \
    self->size = 0;                                                              \
    self->capacity = 0;                                                          \
    EC_HEAP_UNLOCK(self);                                                        \
    EC_MUTEX_DESTROY(self->lock);                                                \
    return EC_OK;                                                                \
}                                                                                \

EC_HEAP_DEF(int, int)
EC_HEAP_DEF(unsigned int, uint)
EC_HEAP_DEF(long, long)
EC_HEAP_DEF(long long, ll)
EC_HEAP_DEF(unsigned long, ulong)
EC_HEAP_DEF(unsigned long long, ull)
EC_HEAP_DEF(uint32_t, u32)
EC_HEAP_DEF(uint64_t, u64)
EC_HEAP_DEF(double, double)
EC_HEAP_DEF(const char *, str)
EC_HEAP_DEF(void *, ptr)

#endif /* EC_HEAP_H */
