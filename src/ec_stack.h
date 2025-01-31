#ifndef EC_STACK_H
#define EC_STACK_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ec_status.h"
#include "ec_common.h"

#ifndef EC_STACK_GROWTH_FACTOR
#define EC_STACK_GROWTH_FACTOR 2
#endif

#define EC_STACK_DEF(TYPE, NAME)                                                   \
                                                                                   \
typedef struct ec_stack_##NAME##_s ec_stack_##NAME##_t;                            \
                                                                                   \
struct ec_stack_##NAME##_s {                                                      \
    TYPE          *data;                                                          \
    size_t         size;                                                          \
    size_t         capacity;                                                      \
    EC_MUTEX_TYPE  lock;                                                          \
    void          *mempool;                                                       \
                                                                                   \
    ec_status_t (*push)(ec_stack_##NAME##_t *self, TYPE val);                     \
    ec_status_t (*pop)(ec_stack_##NAME##_t *self, TYPE *out);                     \
    ec_status_t (*top)(ec_stack_##NAME##_t *self, TYPE *out);                     \
    size_t     (*size_fn)(ec_stack_##NAME##_t *self);                             \
    size_t     (*capacity_fn)(ec_stack_##NAME##_t *self);                         \
    bool       (*is_empty)(ec_stack_##NAME##_t *self);                            \
    ec_status_t(*free_fn)(ec_stack_##NAME##_t *self);                             \
};                                                                                 \
                                                                                   \
static inline ec_status_t ec_stack_##NAME##_init(ec_stack_##NAME##_t *self, size_t capacity, void *mempool); \
static inline void        ec_stack_##NAME##_set_functions(ec_stack_##NAME##_t *self); \
static inline ec_status_t ec_stack_##NAME##_push(ec_stack_##NAME##_t *self, TYPE val); \
static inline ec_status_t ec_stack_##NAME##_pop(ec_stack_##NAME##_t *self, TYPE *out); \
static inline ec_status_t ec_stack_##NAME##_top(ec_stack_##NAME##_t *self, TYPE *out); \
static inline size_t      ec_stack_##NAME##_size(ec_stack_##NAME##_t *self);            \
static inline size_t      ec_stack_##NAME##_capacity(ec_stack_##NAME##_t *self);        \
static inline bool        ec_stack_##NAME##_is_empty(ec_stack_##NAME##_t *self);        \
static inline ec_status_t ec_stack_##NAME##_free(ec_stack_##NAME##_t *self);            \
                                                                                   \
static inline void ec_stack_##NAME##_lock(ec_stack_##NAME##_t *self)   { EC_MUTEX_LOCK(self->lock);   } \
static inline void ec_stack_##NAME##_unlock(ec_stack_##NAME##_t *self) { EC_MUTEX_UNLOCK(self->lock); } \
                                                                                   \
static inline ec_status_t ec_stack_##NAME##_init(ec_stack_##NAME##_t *self, size_t capacity, void *mempool) { \
    if (!self || capacity == 0) return EC_ERR_INVALID_ARG;                          \
    self->size     = 0;                                                            \
    self->capacity = capacity;                                                     \
    self->mempool  = mempool;                                                      \
    EC_MUTEX_INIT(self->lock);                                                     \
    if (mempool) {                                                                 \
        self->data = (TYPE*) ec_mempool_alloc((ec_mempool_t*)mempool, capacity * sizeof(TYPE)); \
    } else {                                                                        \
        self->data = (TYPE*) malloc(capacity * sizeof(TYPE));                      \
    }                                                                               \
    if (!self->data) {                                                             \
        EC_MUTEX_DESTROY(self->lock);                                              \
        return EC_ERR_ALLOC;                                                       \
    }                                                                               \
    ec_stack_##NAME##_set_functions(self);                                          \
    return EC_OK;                                                                  \
}                                                                                  \
                                                                                   \
static inline ec_status_t ec_stack_##NAME##_push(ec_stack_##NAME##_t *self, TYPE val) { \
    if (!self) return EC_ERR_INVALID_ARG;                                          \
    ec_stack_##NAME##_lock(self);                                                 \
    if (self->size >= self->capacity) {                                           \
        size_t new_cap = self->capacity * EC_STACK_GROWTH_FACTOR;                 \
        TYPE *new_data = (TYPE*) realloc(self->data, new_cap * sizeof(TYPE));     \
        if (!new_data) {                                                          \
            ec_stack_##NAME##_unlock(self);                                       \
            return EC_ERR_ALLOC;                                                  \
        }                                                                         \
        self->data     = new_data;                                                \
        self->capacity = new_cap;                                                \
    }                                                                             \
    self->data[self->size++] = val;                                              \
    ec_stack_##NAME##_unlock(self);                                              \
    return EC_OK;                                                                \
}                                                                                  \
                                                                                   \
static inline ec_status_t ec_stack_##NAME##_pop(ec_stack_##NAME##_t *self, TYPE *out) { \
    if (!self || !out) return EC_ERR_INVALID_ARG;                                  \
    ec_stack_##NAME##_lock(self);                                                 \
    if (self->size == 0) {                                                        \
        ec_stack_##NAME##_unlock(self);                                           \
        return EC_ERR_OUT_OF_BOUNDS;                                              \
    }                                                                             \
    *out = self->data[--self->size];                                              \
    ec_stack_##NAME##_unlock(self);                                               \
    return EC_OK;                                                                 \
}                                                                                  \
                                                                                   \
static inline ec_status_t ec_stack_##NAME##_top(ec_stack_##NAME##_t *self, TYPE *out) { \
    if (!self || !out) return EC_ERR_INVALID_ARG;                                  \
    ec_stack_##NAME##_lock(self);                                                 \
    if (self->size == 0) {                                                        \
        ec_stack_##NAME##_unlock(self);                                           \
        return EC_ERR_OUT_OF_BOUNDS;                                              \
    }                                                                             \
    *out = self->data[self->size - 1];                                            \
    ec_stack_##NAME##_unlock(self);                                               \
    return EC_OK;                                                                 \
}                                                                                  \
                                                                                   \
static inline size_t ec_stack_##NAME##_size(ec_stack_##NAME##_t *self) {          \
    if (!self) return 0;                                                          \
    ec_stack_##NAME##_lock(self);                                                 \
    size_t sz = self->size;                                                       \
    ec_stack_##NAME##_unlock(self);                                               \
    return sz;                                                                    \
}                                                                                  \
                                                                                   \
static inline size_t ec_stack_##NAME##_capacity(ec_stack_##NAME##_t *self) {      \
    if (!self) return 0;                                                          \
    ec_stack_##NAME##_lock(self);                                                 \
    size_t cap = self->capacity;                                                  \
    ec_stack_##NAME##_unlock(self);                                               \
    return cap;                                                                   \
}                                                                                  \
                                                                                   \
static inline bool ec_stack_##NAME##_is_empty(ec_stack_##NAME##_t *self) {        \
    if (!self) return true;                                                       \
    ec_stack_##NAME##_lock(self);                                                 \
    bool empty = (self->size == 0);                                               \
    ec_stack_##NAME##_unlock(self);                                               \
    return empty;                                                                 \
}                                                                                  \
                                                                                   \
static inline ec_status_t ec_stack_##NAME##_free(ec_stack_##NAME##_t *self) {     \
    if (!self) return EC_ERR_INVALID_ARG;                                          \
    ec_stack_##NAME##_lock(self);                                                 \
    if (self->data) {                                                             \
        if (self->mempool) ec_mempool_free((ec_mempool_t*)self->mempool, self->data); \
        else free(self->data);                                                    \
        self->data = NULL;                                                        \
    }                                                                             \
    self->size = 0;                                                               \
    self->capacity = 0;                                                           \
    ec_stack_##NAME##_unlock(self);                                               \
    EC_MUTEX_DESTROY(self->lock);                                                 \
    return EC_OK;                                                                 \
}                                                                                  \
                                                                                   \
static inline void ec_stack_##NAME##_set_functions(ec_stack_##NAME##_t *self) {    \
    self->push        = ec_stack_##NAME##_push;                                    \
    self->pop         = ec_stack_##NAME##_pop;                                     \
    self->top         = ec_stack_##NAME##_top;                                     \
    self->size_fn     = ec_stack_##NAME##_size;                                    \
    self->capacity_fn = ec_stack_##NAME##_capacity;                                \
    self->is_empty    = ec_stack_##NAME##_is_empty;                                \
    self->free_fn     = ec_stack_##NAME##_free;                                    \
}

/* Provide expansions for typical types */
EC_STACK_DEF(int, int)
EC_STACK_DEF(unsigned int, uint)
EC_STACK_DEF(long, long)
EC_STACK_DEF(long long, ll)
EC_STACK_DEF(unsigned long, ulong)
EC_STACK_DEF(unsigned long long, ull)
EC_STACK_DEF(uint32_t, u32)
EC_STACK_DEF(uint64_t, u64)
EC_STACK_DEF(double, double)
EC_STACK_DEF(const char*, str)
EC_STACK_DEF(void*, ptr)

#endif /* EC_STACK_H */
