#ifndef EC_LIST_H
#define EC_LIST_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "ec_status.h"
#include "ec_common.h"

#define EC_LIST_LOCK(self)   EC_MUTEX_LOCK((self)->lock)
#define EC_LIST_UNLOCK(self) EC_MUTEX_UNLOCK((self)->lock)

/*
 * EC_LIST_DEF(TYPE, NAME)
 * Singly-linked list for 'TYPE' named 'NAME'.
 * Supports push_front, push_back, pop_front, pop_back, insert_at(index),
 * remove(value), find(value), sort (merge sort), etc.
 */

#define EC_LIST_DEF(TYPE, NAME)                                                         \
                                                                                       \
/* Forward declarations */                                                             \
typedef struct ec_list_##NAME##_node_s ec_list_##NAME##_node_t;                        \
typedef struct ec_list_##NAME##_s       ec_list_##NAME##_t;                            \
                                                                                       \
struct ec_list_##NAME##_node_s {                                                       \
    TYPE                      data;                                                    \
    ec_list_##NAME##_node_t  *next;                                                    \
};                                                                                     \
                                                                                       \
/* OOP-like function pointers */                                                       \
typedef ec_status_t (*ec_list_##NAME##_push_front_fn)(ec_list_##NAME##_t*, TYPE);      \
typedef ec_status_t (*ec_list_##NAME##_push_back_fn) (ec_list_##NAME##_t*, TYPE);       \
typedef ec_status_t (*ec_list_##NAME##_pop_front_fn) (ec_list_##NAME##_t*, TYPE*);      \
typedef ec_status_t (*ec_list_##NAME##_pop_back_fn)  (ec_list_##NAME##_t*, TYPE*);      \
typedef ec_status_t (*ec_list_##NAME##_insert_at_fn) (ec_list_##NAME##_t*, size_t, TYPE);\
typedef ec_status_t (*ec_list_##NAME##_remove_fn)    (ec_list_##NAME##_t*, TYPE, int(*cmp)(const void*, const void*)); \
typedef ec_status_t (*ec_list_##NAME##_find_fn)      (ec_list_##NAME##_t*, TYPE, size_t*, int(*cmp)(const void*, const void*)); \
typedef ec_status_t (*ec_list_##NAME##_sort_fn)      (ec_list_##NAME##_t*, int(*cmp)(const void*, const void*)); \
typedef ec_status_t (*ec_list_##NAME##_free_fn)      (ec_list_##NAME##_t*);             \
typedef size_t     (*ec_list_##NAME##_size_fn)       (ec_list_##NAME##_t*);             \
                                                                                       \
struct ec_list_##NAME##_s {                                                            \
    ec_list_##NAME##_node_t *head;                                                     \
    ec_list_##NAME##_node_t *tail;                                                     \
    size_t                    size;                                                     \
    EC_MUTEX_TYPE             lock;                                                     \
    void                     *mempool;                                                  \
                                                                                       \
    ec_list_##NAME##_push_front_fn push_front;                                         \
    ec_list_##NAME##_push_back_fn  push_back;                                          \
    ec_list_##NAME##_pop_front_fn  pop_front;                                          \
    ec_list_##NAME##_pop_back_fn   pop_back;                                           \
    ec_list_##NAME##_insert_at_fn  insert_at;                                          \
    ec_list_##NAME##_remove_fn     remove;                                             \
    ec_list_##NAME##_find_fn       find;                                               \
    ec_list_##NAME##_sort_fn       sort;                                               \
    ec_list_##NAME##_free_fn       free_fn;                                            \
    ec_list_##NAME##_size_fn       size_fn;                                            \
};                                                                                     \
                                                                                       \
static inline ec_status_t ec_list_##NAME##_init(ec_list_##NAME##_t *self, void *mempool);                \
static inline void        ec_list_##NAME##_set_functions(ec_list_##NAME##_t *self);                      \
static inline ec_status_t ec_list_##NAME##_push_front(ec_list_##NAME##_t *self, TYPE val);               \
static inline ec_status_t ec_list_##NAME##_push_back(ec_list_##NAME##_t *self, TYPE val);                \
static inline ec_status_t ec_list_##NAME##_pop_front(ec_list_##NAME##_t *self, TYPE *out);               \
static inline ec_status_t ec_list_##NAME##_pop_back(ec_list_##NAME##_t *self, TYPE *out);                \
static inline ec_status_t ec_list_##NAME##_insert_at(ec_list_##NAME##_t *self, size_t index, TYPE val);   \
static inline ec_status_t ec_list_##NAME##_remove(ec_list_##NAME##_t *self, TYPE val, int(*cmp)(const void*, const void*)); \
static inline ec_status_t ec_list_##NAME##_find(ec_list_##NAME##_t *self, TYPE val, size_t *idx, int(*cmp)(const void*, const void*)); \
static inline ec_status_t ec_list_##NAME##_sort(ec_list_##NAME##_t *self, int(*cmp)(const void*, const void*)); \
static inline ec_status_t ec_list_##NAME##_free(ec_list_##NAME##_t *self);                                  \
static inline size_t      ec_list_##NAME##_size(ec_list_##NAME##_t *self);                                  \
                                                                                       \
/* A merge-sort helper for singly linked lists */                                      \
static inline ec_list_##NAME##_node_t* ec_list_##NAME##_merge_sort_nodes(ec_list_##NAME##_node_t *head, int(*cmp)(const void*, const void*)); \
static inline ec_list_##NAME##_node_t* ec_list_##NAME##_merge_nodes(ec_list_##NAME##_node_t *a, ec_list_##NAME##_node_t *b, int(*cmp)(const void*, const void*)); \
static inline void ec_list_##NAME##_update_tail(ec_list_##NAME##_t *self);            \
                                                                                       \
static inline ec_status_t ec_list_##NAME##_init(ec_list_##NAME##_t *self, void *mempool) { \
    if (!self) return EC_ERR_INVALID_ARG;                                              \
    self->head    = NULL;                                                             \
    self->tail    = NULL;                                                             \
    self->size    = 0;                                                                \
    self->mempool = mempool;                                                          \
    EC_MUTEX_INIT(self->lock);                                                        \
    ec_list_##NAME##_set_functions(self);                                             \
    return EC_OK;                                                                     \
}                                                                                      \
                                                                                       \
static inline ec_list_##NAME##_node_t* ec_list_##NAME##_alloc_node(ec_list_##NAME##_t *self) { \
    if (self->mempool) {                                                              \
        return (ec_list_##NAME##_node_t*) ec_mempool_alloc((ec_mempool_t*)self->mempool, sizeof(ec_list_##NAME##_node_t)); \
    } else {                                                                          \
        return (ec_list_##NAME##_node_t*) malloc(sizeof(ec_list_##NAME##_node_t));    \
    }                                                                                 \
}                                                                                      \
                                                                                       \
static inline ec_status_t ec_list_##NAME##_push_front(ec_list_##NAME##_t *self, TYPE val) { \
    if (!self) return EC_ERR_INVALID_ARG;                                              \
    EC_LIST_LOCK(self);                                                               \
    ec_list_##NAME##_node_t *node = ec_list_##NAME##_alloc_node(self);                \
    if (!node) {                                                                      \
        EC_LIST_UNLOCK(self);                                                         \
        return EC_ERR_ALLOC;                                                          \
    }                                                                                 \
    node->data = val;                                                                 \
    node->next = self->head;                                                          \
    self->head = node;                                                                \
    if (self->size == 0) self->tail = node;                                           \
    self->size++;                                                                     \
    EC_LIST_UNLOCK(self);                                                             \
    return EC_OK;                                                                     \
}                                                                                      \
                                                                                       \
static inline ec_status_t ec_list_##NAME##_push_back(ec_list_##NAME##_t *self, TYPE val) { \
    if (!self) return EC_ERR_INVALID_ARG;                                              \
    EC_LIST_LOCK(self);                                                               \
    ec_list_##NAME##_node_t *node = ec_list_##NAME##_alloc_node(self);                \
    if (!node) {                                                                      \
        EC_LIST_UNLOCK(self);                                                         \
        return EC_ERR_ALLOC;                                                          \
    }                                                                                 \
    node->data = val;                                                                 \
    node->next = NULL;                                                                \
    if (self->size == 0) {                                                            \
        self->head = node;                                                            \
        self->tail = node;                                                            \
    } else {                                                                          \
        self->tail->next = node;                                                      \
        self->tail = node;                                                            \
    }                                                                                 \
    self->size++;                                                                     \
    EC_LIST_UNLOCK(self);                                                             \
    return EC_OK;                                                                     \
}                                                                                      \
                                                                                       \
static inline ec_status_t ec_list_##NAME##_pop_front(ec_list_##NAME##_t *self, TYPE *out) { \
    if (!self || !out) return EC_ERR_INVALID_ARG;                                      \
    EC_LIST_LOCK(self);                                                               \
    if (self->size == 0) {                                                            \
        EC_LIST_UNLOCK(self);                                                         \
        return EC_ERR_OUT_OF_BOUNDS;                                                  \
    }                                                                                 \
    ec_list_##NAME##_node_t *node = self->head;                                       \
    *out = node->data;                                                                \
    self->head = node->next;                                                          \
    if (!self->head) self->tail = NULL;                                               \
    self->size--;                                                                     \
    if (!self->mempool) free(node);                                                   \
    EC_LIST_UNLOCK(self);                                                             \
    return EC_OK;                                                                     \
}                                                                                      \
                                                                                       \
static inline ec_status_t ec_list_##NAME##_pop_back(ec_list_##NAME##_t *self, TYPE *out) { \
    if (!self || !out) return EC_ERR_INVALID_ARG;                                      \
    EC_LIST_LOCK(self);                                                               \
    if (self->size == 0) {                                                            \
        EC_LIST_UNLOCK(self);                                                         \
        return EC_ERR_OUT_OF_BOUNDS;                                                  \
    }                                                                                 \
    /* If only one node */                                                            \
    if (self->size == 1) {                                                            \
        *out = self->head->data;                                                      \
        if (!self->mempool) free(self->head);                                         \
        self->head = NULL;                                                            \
        self->tail = NULL;                                                            \
        self->size = 0;                                                               \
        EC_LIST_UNLOCK(self);                                                         \
        return EC_OK;                                                                 \
    }                                                                                 \
    /* Otherwise find second-last node. O(n), but singly linked. */                   \
    ec_list_##NAME##_node_t *prev = NULL;                                             \
    ec_list_##NAME##_node_t *cur  = self->head;                                       \
    while (cur->next) {                                                               \
        prev = cur;                                                                   \
        cur = cur->next;                                                              \
    }                                                                                 \
    *out = cur->data;                                                                 \
    prev->next = NULL;                                                                \
    self->tail = prev;                                                                \
    self->size--;                                                                     \
    if (!self->mempool) free(cur);                                                    \
    EC_LIST_UNLOCK(self);                                                             \
    return EC_OK;                                                                     \
}                                                                                      \
                                                                                       \
static inline ec_status_t ec_list_##NAME##_insert_at(ec_list_##NAME##_t *self, size_t index, TYPE val) { \
    if (!self) return EC_ERR_INVALID_ARG;                                              \
    EC_LIST_LOCK(self);                                                               \
    if (index > self->size) {                                                         \
        EC_LIST_UNLOCK(self);                                                         \
        return EC_ERR_OUT_OF_BOUNDS;                                                  \
    }                                                                                 \
    if (index == 0) {                                                                 \
        ec_list_##NAME##_push_front(self, val);                                       \
        EC_LIST_UNLOCK(self);                                                         \
        return EC_OK;                                                                 \
    } else if (index == self->size) {                                                 \
        ec_list_##NAME##_push_back(self, val);                                        \
        EC_LIST_UNLOCK(self);                                                         \
        return EC_OK;                                                                 \
    }                                                                                 \
    ec_list_##NAME##_node_t *node = ec_list_##NAME##_alloc_node(self);                \
    if (!node) {                                                                      \
        EC_LIST_UNLOCK(self);                                                         \
        return EC_ERR_ALLOC;                                                          \
    }                                                                                 \
    node->data = val;                                                                 \
    ec_list_##NAME##_node_t *prev = NULL;                                             \
    ec_list_##NAME##_node_t *cur  = self->head;                                       \
    size_t i = 0;                                                                     \
    while (i < index) {                                                               \
        prev = cur;                                                                   \
        cur = cur->next;                                                              \
        i++;                                                                          \
    }                                                                                 \
    prev->next = node;                                                                \
    node->next = cur;                                                                 \
    if (cur == NULL) self->tail = node;                                               \
    self->size++;                                                                     \
    EC_LIST_UNLOCK(self);                                                             \
    return EC_OK;                                                                     \
}                                                                                      \
                                                                                       \
static inline ec_status_t ec_list_##NAME##_find(ec_list_##NAME##_t *self, TYPE val, size_t *idx, int(*cmp)(const void*, const void*)) { \
    if (!self || !idx || !cmp) return EC_ERR_INVALID_ARG;                              \
    EC_LIST_LOCK(self);                                                               \
    size_t i = 0;                                                                     \
    for (ec_list_##NAME##_node_t *cur = self->head; cur; cur = cur->next) {           \
        if (cmp(&cur->data, &val) == 0) {                                             \
            *idx = i;                                                                 \
            EC_LIST_UNLOCK(self);                                                     \
            return EC_OK;                                                             \
        }                                                                             \
        i++;                                                                          \
    }                                                                                 \
    EC_LIST_UNLOCK(self);                                                             \
    return EC_ERR_NOT_FOUND;                                                          \
}                                                                                      \
                                                                                       \
static inline ec_status_t ec_list_##NAME##_remove(ec_list_##NAME##_t *self, TYPE val, int(*cmp)(const void*, const void*)) { \
    if (!self || !cmp) return EC_ERR_INVALID_ARG;                                      \
    EC_LIST_LOCK(self);                                                               \
    ec_list_##NAME##_node_t *prev = NULL;                                             \
    ec_list_##NAME##_node_t *cur  = self->head;                                       \
    while (cur) {                                                                     \
        if (cmp(&cur->data, &val) == 0) {                                             \
            if (prev) {                                                               \
                prev->next = cur->next;                                               \
                if (cur == self->tail) self->tail = prev;                             \
            } else {                                                                  \
                self->head = cur->next;                                               \
                if (!self->head) self->tail = NULL;                                   \
            }                                                                         \
            self->size--;                                                             \
            if (!self->mempool) free(cur);                                            \
            EC_LIST_UNLOCK(self);                                                     \
            return EC_OK;                                                             \
        }                                                                             \
        prev = cur;                                                                   \
        cur  = cur->next;                                                             \
    }                                                                                 \
    EC_LIST_UNLOCK(self);                                                             \
    return EC_ERR_NOT_FOUND;                                                          \
}                                                                                      \
                                                                                       \
/* Merge-sort style sorting for singly linked list: O(n log n). */                   \
static inline ec_list_##NAME##_node_t* ec_list_##NAME##_merge_nodes(ec_list_##NAME##_node_t *a, ec_list_##NAME##_node_t *b, int(*cmp)(const void*, const void*)) { \
    ec_list_##NAME##_node_t dummy;                                                    \
    ec_list_##NAME##_node_t *tail = &dummy;                                           \
    dummy.next = NULL;                                                                \
    while (a && b) {                                                                  \
        if (cmp(&a->data, &b->data) <= 0) {                                           \
            tail->next = a;                                                           \
            a = a->next;                                                              \
        } else {                                                                      \
            tail->next = b;                                                           \
            b = b->next;                                                              \
        }                                                                             \
        tail = tail->next;                                                            \
    }                                                                                 \
    tail->next = (a ? a : b);                                                         \
    return dummy.next;                                                                \
}                                                                                      \
                                                                                       \
static inline ec_list_##NAME##_node_t* ec_list_##NAME##_merge_sort_nodes(ec_list_##NAME##_node_t *head, int(*cmp)(const void*, const void*)) { \
    if (!head || !head->next) return head;                                            \
    ec_list_##NAME##_node_t *slow = head;                                             \
    ec_list_##NAME##_node_t *fast = head->next;                                       \
    while (fast && fast->next) {                                                     \
        slow = slow->next;                                                           \
        fast = fast->next->next;                                                     \
    }                                                                                 \
    ec_list_##NAME##_node_t *mid = slow->next;                                        \
    slow->next = NULL;                                                               \
    ec_list_##NAME##_node_t *left  = ec_list_##NAME##_merge_sort_nodes(head, cmp);    \
    ec_list_##NAME##_node_t *right = ec_list_##NAME##_merge_sort_nodes(mid,  cmp);    \
    return ec_list_##NAME##_merge_nodes(left, right, cmp);                            \
}                                                                                      \
                                                                                       \
static inline void ec_list_##NAME##_update_tail(ec_list_##NAME##_t *self) {           \
    self->tail = NULL;                                                               \
    ec_list_##NAME##_node_t *cur = self->head;                                       \
    while (cur) {                                                                    \
        if (!cur->next) self->tail = cur;                                            \
        cur = cur->next;                                                             \
    }                                                                                \
}                                                                                      \
                                                                                       \
static inline ec_status_t ec_list_##NAME##_sort(ec_list_##NAME##_t *self, int(*cmp)(const void*, const void*)) { \
    if (!self || !cmp) return EC_ERR_INVALID_ARG;                                     \
    EC_LIST_LOCK(self);                                                              \
    if (self->size < 2) {                                                            \
        EC_LIST_UNLOCK(self);                                                        \
        return EC_OK; /* Already sorted or empty */                                   \
    }                                                                                \
    self->head = ec_list_##NAME##_merge_sort_nodes(self->head, cmp);                 \
    ec_list_##NAME##_update_tail(self);                                              \
    EC_LIST_UNLOCK(self);                                                            \
    return EC_OK;                                                                    \
}                                                                                      \
                                                                                       \
static inline ec_status_t ec_list_##NAME##_free(ec_list_##NAME##_t *self) {           \
    if (!self) return EC_ERR_INVALID_ARG;                                             \
    EC_LIST_LOCK(self);                                                              \
    ec_list_##NAME##_node_t *cur = self->head;                                       \
    while (cur) {                                                                    \
        ec_list_##NAME##_node_t *next = cur->next;                                   \
        if (!self->mempool) free(cur);                                               \
        cur = next;                                                                  \
    }                                                                                \
    self->head = NULL;                                                               \
    self->tail = NULL;                                                               \
    self->size = 0;                                                                  \
    EC_LIST_UNLOCK(self);                                                            \
    EC_MUTEX_DESTROY(self->lock);                                                    \
    return EC_OK;                                                                    \
}                                                                                      \
                                                                                       \
static inline size_t ec_list_##NAME##_size(ec_list_##NAME##_t *self) {                \
    if (!self) return 0;                                                             \
    EC_LIST_LOCK(self);                                                              \
    size_t sz = self->size;                                                          \
    EC_LIST_UNLOCK(self);                                                            \
    return sz;                                                                       \
}                                                                                      \
                                                                                       \
static inline void ec_list_##NAME##_set_functions(ec_list_##NAME##_t *self) {         \
    self->push_front = ec_list_##NAME##_push_front;                                  \
    self->push_back  = ec_list_##NAME##_push_back;                                   \
    self->pop_front  = ec_list_##NAME##_pop_front;                                   \
    self->pop_back   = ec_list_##NAME##_pop_back;                                    \
    self->insert_at  = ec_list_##NAME##_insert_at;                                   \
    self->remove     = ec_list_##NAME##_remove;                                      \
    self->find       = ec_list_##NAME##_find;                                        \
    self->sort       = ec_list_##NAME##_sort;                                        \
    self->free_fn    = ec_list_##NAME##_free;                                        \
    self->size_fn    = ec_list_##NAME##_size;                                        \
}

/* Provide expansions for typical types */
EC_LIST_DEF(int, int)
EC_LIST_DEF(unsigned int, uint)
EC_LIST_DEF(long, long)
EC_LIST_DEF(long long, ll)
EC_LIST_DEF(unsigned long, ulong)
EC_LIST_DEF(unsigned long long, ull)
EC_LIST_DEF(uint32_t, u32)
EC_LIST_DEF(uint64_t, u64)
EC_LIST_DEF(double, double)
EC_LIST_DEF(const char*, str)
EC_LIST_DEF(void*, ptr)

#endif /* EC_LIST_H */
