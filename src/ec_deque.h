#ifndef EC_DEQUE_H
#define EC_DEQUE_H

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "ec_status.h"
#include "ec_mempool.h"
#include "ec_common.h"

#define EC_DEQUE_LOCK(self)   EC_MUTEX_LOCK((self)->lock)
#define EC_DEQUE_UNLOCK(self) EC_MUTEX_UNLOCK((self)->lock)

/*
 * EC_DEQUE_DEF(TYPE, NAME)
 * Defines a **double-ended queue** (deque) using a **doubly linked list**.
 * OOP-like usage:
 *   ec_deque_int_t dq;
 *   ec_deque_int_init(&dq, NULL);
 *   dq.push_front(&dq, 10);
 *   dq.push_back(&dq, 20);
 *   dq.pop_front(&dq, &val);
 *   dq.free_fn(&dq);
 */

#define EC_DEQUE_DEF(TYPE, NAME)                                                 \
                                                                                 \
typedef struct ec_deque_##NAME##_node_s ec_deque_##NAME##_node_t;                \
typedef struct ec_deque_##NAME##_s ec_deque_##NAME##_t;                          \
                                                                                 \
struct ec_deque_##NAME##_node_s {                                                \
    TYPE value;                                                                  \
    ec_deque_##NAME##_node_t *prev, *next;                                       \
};                                                                               \
                                                                                 \
struct ec_deque_##NAME##_s {                                                     \
    ec_deque_##NAME##_node_t *head, *tail;                                       \
    size_t size;                                                                 \
    EC_MUTEX_TYPE lock;                                                          \
    void *mempool;                                                               \
                                                                                 \
    ec_status_t (*push_front) (ec_deque_##NAME##_t*, TYPE);                      \
    ec_status_t (*push_back)  (ec_deque_##NAME##_t*, TYPE);                      \
    ec_status_t (*pop_front)  (ec_deque_##NAME##_t*, TYPE*);                     \
    ec_status_t (*pop_back)   (ec_deque_##NAME##_t*, TYPE*);                     \
    ec_status_t (*peek_front) (ec_deque_##NAME##_t*, TYPE*);                     \
    ec_status_t (*peek_back)  (ec_deque_##NAME##_t*, TYPE*);                     \
    size_t      (*size_fn)    (ec_deque_##NAME##_t*);                            \
    ec_status_t (*free_fn)    (ec_deque_##NAME##_t*);                            \
};                                                                               \
                                                                                 \
static inline ec_status_t ec_deque_##NAME##_init(ec_deque_##NAME##_t*, void*);   \
static inline void        ec_deque_##NAME##_set_functions(ec_deque_##NAME##_t*); \
static inline ec_status_t ec_deque_##NAME##_push_front(ec_deque_##NAME##_t*, TYPE); \
static inline ec_status_t ec_deque_##NAME##_push_back(ec_deque_##NAME##_t*, TYPE); \
static inline ec_status_t ec_deque_##NAME##_pop_front(ec_deque_##NAME##_t*, TYPE*); \
static inline ec_status_t ec_deque_##NAME##_pop_back(ec_deque_##NAME##_t*, TYPE*); \
static inline ec_status_t ec_deque_##NAME##_peek_front(ec_deque_##NAME##_t*, TYPE*); \
static inline ec_status_t ec_deque_##NAME##_peek_back(ec_deque_##NAME##_t*, TYPE*); \
static inline size_t      ec_deque_##NAME##_size(ec_deque_##NAME##_t*);         \
static inline ec_status_t ec_deque_##NAME##_free(ec_deque_##NAME##_t*);         \
                                                                                 \
static inline ec_status_t ec_deque_##NAME##_init(ec_deque_##NAME##_t *self, void *mempool) { \
    if (!self) return EC_ERR_INVALID_ARG;                                        \
    self->head = self->tail = NULL;                                              \
    self->size = 0;                                                              \
    self->mempool = mempool;                                                     \
    EC_MUTEX_INIT(self->lock);                                                   \
    ec_deque_##NAME##_set_functions(self);                                       \
    return EC_OK;                                                                \
}                                                                                \
                                                                                 \
static inline void ec_deque_##NAME##_set_functions(ec_deque_##NAME##_t *self) {  \
    self->push_front = ec_deque_##NAME##_push_front;                             \
    self->push_back  = ec_deque_##NAME##_push_back;                              \
    self->pop_front  = ec_deque_##NAME##_pop_front;                              \
    self->pop_back   = ec_deque_##NAME##_pop_back;                               \
    self->peek_front = ec_deque_##NAME##_peek_front;                             \
    self->peek_back  = ec_deque_##NAME##_peek_back;                              \
    self->size_fn    = ec_deque_##NAME##_size;                                   \
    self->free_fn    = ec_deque_##NAME##_free;                                   \
}                                                                                \
                                                                                 \
static inline ec_status_t ec_deque_##NAME##_push_front(ec_deque_##NAME##_t *self, TYPE val) { \
    if (!self) return EC_ERR_INVALID_ARG;                                        \
    EC_DEQUE_LOCK(self);                                                         \
    ec_deque_##NAME##_node_t *node = (self->mempool) ?                           \
        (ec_deque_##NAME##_node_t*)ec_mempool_alloc((ec_mempool_t*)self->mempool, sizeof(ec_deque_##NAME##_node_t)) : \
        (ec_deque_##NAME##_node_t*)EC_MALLOC(sizeof(ec_deque_##NAME##_node_t));  \
    if (!node) {                                                                 \
        EC_DEQUE_UNLOCK(self);                                                   \
        return EC_ERR_ALLOC;                                                     \
    }                                                                            \
    node->value = val;                                                           \
    node->next = self->head;                                                     \
    node->prev = NULL;                                                           \
    if (self->head) self->head->prev = node;                                     \
    self->head = node;                                                           \
    if (!self->tail) self->tail = node;                                          \
    self->size++;                                                                \
    EC_DEQUE_UNLOCK(self);                                                       \
    return EC_OK;                                                                \
}                                                                                \
                                                                                 \
static inline ec_status_t ec_deque_##NAME##_push_back(ec_deque_##NAME##_t *self, TYPE val) { \
    if (!self) return EC_ERR_INVALID_ARG;                                        \
    EC_DEQUE_LOCK(self);                                                         \
    ec_deque_##NAME##_node_t *node = (self->mempool) ?                           \
        (ec_deque_##NAME##_node_t*)ec_mempool_alloc((ec_mempool_t*)self->mempool, sizeof(ec_deque_##NAME##_node_t)) : \
        (ec_deque_##NAME##_node_t*)EC_MALLOC(sizeof(ec_deque_##NAME##_node_t));  \
    if (!node) {                                                                 \
        EC_DEQUE_UNLOCK(self);                                                   \
        return EC_ERR_ALLOC;                                                     \
    }                                                                            \
    node->value = val;                                                           \
    node->next = NULL;                                                           \
    node->prev = self->tail;                                                     \
    if (self->tail) self->tail->next = node;                                     \
    self->tail = node;                                                           \
    if (!self->head) self->head = node;                                          \
    self->size++;                                                                \
    EC_DEQUE_UNLOCK(self);                                                       \
    return EC_OK;                                                                \
}                                                                                \
                                                                                 \
static inline ec_status_t ec_deque_##NAME##_pop_front(ec_deque_##NAME##_t *self, TYPE *out) { \
    if (!self || !out) return EC_ERR_INVALID_ARG;                                \
    EC_DEQUE_LOCK(self);                                                         \
    if (!self->head) {                                                           \
        EC_DEQUE_UNLOCK(self);                                                   \
        return EC_ERR_OUT_OF_BOUNDS;                                             \
    }                                                                            \
    *out = self->head->value;                                                    \
    ec_deque_##NAME##_node_t *temp = self->head;                                 \
    self->head = self->head->next;                                               \
    if (self->head) self->head->prev = NULL;                                     \
    else self->tail = NULL;                                                      \
    EC_FREE(temp);                                                               \
    self->size--;                                                                \
    EC_DEQUE_UNLOCK(self);                                                       \
    return EC_OK;                                                                \
}                                                                                \
                                                                                 \
static inline size_t ec_deque_##NAME##_size(ec_deque_##NAME##_t *self) {         \
    return self ? self->size : 0;                                                \
}                                                                                \
                                                                                 \
static inline ec_status_t ec_deque_##NAME##_free(ec_deque_##NAME##_t *self) {    \
    if (!self) return EC_ERR_INVALID_ARG;                                        \
    EC_DEQUE_LOCK(self);                                                         \
    while (self->head) {                                                         \
        ec_deque_##NAME##_node_t *temp = self->head;                             \
        self->head = self->head->next;                                           \
        EC_FREE(temp);                                                           \
    }                                                                            \
    self->size = 0;                                                              \
    self->tail = NULL;                                                           \
    EC_DEQUE_UNLOCK(self);                                                       \
    EC_MUTEX_DESTROY(self->lock);                                                \
    return EC_OK;                                                                \
}

/* Provide typical type definitions */
EC_DEQUE_DEF(int, int)
EC_DEQUE_DEF(unsigned int, uint)
EC_DEQUE_DEF(long, long)
EC_DEQUE_DEF(long long, ll)
EC_DEQUE_DEF(unsigned long, ulong)
EC_DEQUE_DEF(unsigned long long, ull)
EC_DEQUE_DEF(uint32_t, u32)
EC_DEQUE_DEF(uint64_t, u64)
EC_DEQUE_DEF(double, double)
EC_DEQUE_DEF(const char *, str)
EC_DEQUE_DEF(void *, ptr)

#endif /* EC_DEQUE_H */
