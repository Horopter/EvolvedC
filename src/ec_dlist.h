#ifndef EC_DLIST_H
#define EC_DLIST_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "ec_status.h"
#include "ec_common.h"

#define EC_DLIST_LOCK(self)   EC_MUTEX_LOCK((self)->lock)
#define EC_DLIST_UNLOCK(self) EC_MUTEX_UNLOCK((self)->lock)

/*
 * EC_DLIST_DEF(TYPE, NAME)
 * Doubly-linked list for 'TYPE' named 'NAME'.
 * OOP-like usage: 
 *   ec_dlist_int_t list;
 *   ec_dlist_int_init(&list, NULL);
 *   list.push_front(&list, 10);
 *   list.push_back(&list, 20);
 *   list.sort(&list, cmp);
 *   list.free_fn(&list);
 */

#define EC_DLIST_DEF(TYPE, NAME)                                                      \
                                                                                     \
/* Forward declarations */                                                           \
typedef struct ec_dlist_##NAME##_node_s ec_dlist_##NAME##_node_t;                    \
typedef struct ec_dlist_##NAME##_s       ec_dlist_##NAME##_t;                        \
                                                                                     \
struct ec_dlist_##NAME##_node_s {                                                     \
    TYPE                          data;                                               \
    ec_dlist_##NAME##_node_t     *prev;                                               \
    ec_dlist_##NAME##_node_t     *next;                                               \
};                                                                                    \
                                                                                     \
typedef ec_status_t (*ec_dlist_##NAME##_push_front_fn)(ec_dlist_##NAME##_t*, TYPE);   \
typedef ec_status_t (*ec_dlist_##NAME##_push_back_fn) (ec_dlist_##NAME##_t*, TYPE);   \
typedef ec_status_t (*ec_dlist_##NAME##_pop_front_fn) (ec_dlist_##NAME##_t*, TYPE*);  \
typedef ec_status_t (*ec_dlist_##NAME##_pop_back_fn)  (ec_dlist_##NAME##_t*, TYPE*);  \
typedef ec_status_t (*ec_dlist_##NAME##_insert_at_fn) (ec_dlist_##NAME##_t*, size_t, TYPE); \
typedef ec_status_t (*ec_dlist_##NAME##_remove_fn)    (ec_dlist_##NAME##_t*, TYPE, int(*cmp)(const void*, const void*)); \
typedef ec_status_t (*ec_dlist_##NAME##_find_fn)      (ec_dlist_##NAME##_t*, TYPE, size_t*, int(*cmp)(const void*, const void*)); \
typedef ec_status_t (*ec_dlist_##NAME##_sort_fn)      (ec_dlist_##NAME##_t*, int(*cmp)(const void*, const void*)); \
typedef ec_status_t (*ec_dlist_##NAME##_free_fn)      (ec_dlist_##NAME##_t*);          \
typedef size_t     (*ec_dlist_##NAME##_size_fn)       (ec_dlist_##NAME##_t*);          \
                                                                                     \
struct ec_dlist_##NAME##_s {                                                         \
    ec_dlist_##NAME##_node_t *head;                                                  \
    ec_dlist_##NAME##_node_t *tail;                                                  \
    size_t                     size;                                                  \
    EC_MUTEX_TYPE              lock;                                                  \
    void                      *mempool;                                               \
                                                                                     \
    ec_dlist_##NAME##_push_front_fn push_front;                                       \
    ec_dlist_##NAME##_push_back_fn  push_back;                                        \
    ec_dlist_##NAME##_pop_front_fn  pop_front;                                        \
    ec_dlist_##NAME##_pop_back_fn   pop_back;                                         \
    ec_dlist_##NAME##_insert_at_fn  insert_at;                                        \
    ec_dlist_##NAME##_remove_fn     remove;                                           \
    ec_dlist_##NAME##_find_fn       find;                                             \
    ec_dlist_##NAME##_sort_fn       sort;                                             \
    ec_dlist_##NAME##_free_fn       free_fn;                                          \
    ec_dlist_##NAME##_size_fn       size_fn;                                          \
};                                                                                    \
                                                                                     \
static inline ec_status_t ec_dlist_##NAME##_init(ec_dlist_##NAME##_t *self, void *mempool);             \
static inline void        ec_dlist_##NAME##_set_functions(ec_dlist_##NAME##_t *self);                   \
static inline ec_status_t ec_dlist_##NAME##_push_front(ec_dlist_##NAME##_t *self, TYPE val);            \
static inline ec_status_t ec_dlist_##NAME##_push_back(ec_dlist_##NAME##_t *self, TYPE val);             \
static inline ec_status_t ec_dlist_##NAME##_pop_front(ec_dlist_##NAME##_t *self, TYPE *out);            \
static inline ec_status_t ec_dlist_##NAME##_pop_back(ec_dlist_##NAME##_t *self, TYPE *out);             \
static inline ec_status_t ec_dlist_##NAME##_insert_at(ec_dlist_##NAME##_t *self, size_t index, TYPE val); \
static inline ec_status_t ec_dlist_##NAME##_remove(ec_dlist_##NAME##_t *self, TYPE val, int(*cmp)(const void*, const void*)); \
static inline ec_status_t ec_dlist_##NAME##_find(ec_dlist_##NAME##_t *self, TYPE val, size_t *idx, int(*cmp)(const void*, const void*)); \
static inline ec_status_t ec_dlist_##NAME##_sort(ec_dlist_##NAME##_t *self, int(*cmp)(const void*, const void*)); \
static inline ec_status_t ec_dlist_##NAME##_free(ec_dlist_##NAME##_t *self);                            \
static inline size_t      ec_dlist_##NAME##_size(ec_dlist_##NAME##_t *self);                            \
                                                                                     \
static inline ec_dlist_##NAME##_node_t* ec_dlist_##NAME##_alloc_node(ec_dlist_##NAME##_t *self);         \
static inline ec_dlist_##NAME##_node_t* ec_dlist_##NAME##_merge_sort(ec_dlist_##NAME##_node_t *head, int(*cmp)(const void*, const void*)); \
static inline ec_dlist_##NAME##_node_t* ec_dlist_##NAME##_split(ec_dlist_##NAME##_node_t *head);         \
static inline ec_dlist_##NAME##_node_t* ec_dlist_##NAME##_merge(ec_dlist_##NAME##_node_t *a, ec_dlist_##NAME##_node_t *b, int(*cmp)(const void*, const void*)); \
static inline void ec_dlist_##NAME##_update_tail(ec_dlist_##NAME##_t *self);       \
                                                                                     \
static inline ec_status_t ec_dlist_##NAME##_init(ec_dlist_##NAME##_t *self, void *mempool) { \
    if (!self) return EC_ERR_INVALID_ARG;                                            \
    self->head    = NULL;                                                           \
    self->tail    = NULL;                                                           \
    self->size    = 0;                                                              \
    self->mempool = mempool;                                                        \
    EC_MUTEX_INIT(self->lock);                                                      \
    ec_dlist_##NAME##_set_functions(self);                                          \
    return EC_OK;                                                                   \
}                                                                                    \
                                                                                     \
static inline ec_dlist_##NAME##_node_t* ec_dlist_##NAME##_alloc_node(ec_dlist_##NAME##_t *self) { \
    if (self->mempool) {                                                            \
        return (ec_dlist_##NAME##_node_t*) ec_mempool_alloc((ec_mempool_t*)self->mempool, sizeof(ec_dlist_##NAME##_node_t)); \
    } else {                                                                        \
        return (ec_dlist_##NAME##_node_t*) malloc(sizeof(ec_dlist_##NAME##_node_t));\
    }                                                                               \
}                                                                                    \
                                                                                     \
static inline ec_status_t ec_dlist_##NAME##_push_front(ec_dlist_##NAME##_t *self, TYPE val) { \
    if (!self) return EC_ERR_INVALID_ARG;                                            \
    EC_DLIST_LOCK(self);                                                            \
    ec_dlist_##NAME##_node_t *node = ec_dlist_##NAME##_alloc_node(self);            \
    if (!node) {                                                                    \
        EC_DLIST_UNLOCK(self);                                                      \
        return EC_ERR_ALLOC;                                                        \
    }                                                                               \
    node->data = val;                                                               \
    node->prev = NULL;                                                              \
    node->next = self->head;                                                        \
    if (self->head) {                                                               \
        self->head->prev = node;                                                    \
    } else {                                                                        \
        self->tail = node;                                                          \
    }                                                                               \
    self->head = node;                                                              \
    self->size++;                                                                   \
    EC_DLIST_UNLOCK(self);                                                          \
    return EC_OK;                                                                   \
}                                                                                    \
                                                                                     \
static inline ec_status_t ec_dlist_##NAME##_push_back(ec_dlist_##NAME##_t *self, TYPE val) { \
    if (!self) return EC_ERR_INVALID_ARG;                                            \
    EC_DLIST_LOCK(self);                                                            \
    ec_dlist_##NAME##_node_t *node = ec_dlist_##NAME##_alloc_node(self);            \
    if (!node) {                                                                    \
        EC_DLIST_UNLOCK(self);                                                      \
        return EC_ERR_ALLOC;                                                        \
    }                                                                               \
    node->data = val;                                                               \
    node->next = NULL;                                                              \
    node->prev = self->tail;                                                        \
    if (self->tail) {                                                               \
        self->tail->next = node;                                                    \
    } else {                                                                        \
        self->head = node;                                                          \
    }                                                                               \
    self->tail = node;                                                              \
    self->size++;                                                                   \
    EC_DLIST_UNLOCK(self);                                                          \
    return EC_OK;                                                                   \
}                                                                                    \
                                                                                     \
static inline ec_status_t ec_dlist_##NAME##_pop_front(ec_dlist_##NAME##_t *self, TYPE *out) { \
    if (!self || !out) return EC_ERR_INVALID_ARG;                                    \
    EC_DLIST_LOCK(self);                                                            \
    if (self->size == 0) {                                                          \
        EC_DLIST_UNLOCK(self);                                                      \
        return EC_ERR_OUT_OF_BOUNDS;                                                \
    }                                                                               \
    ec_dlist_##NAME##_node_t *node = self->head;                                    \
    *out = node->data;                                                              \
    self->head = node->next;                                                        \
    if (self->head) {                                                               \
        self->head->prev = NULL;                                                    \
    } else {                                                                        \
        self->tail = NULL;                                                          \
    }                                                                               \
    self->size--;                                                                   \
    if (!self->mempool) free(node);                                                 \
    EC_DLIST_UNLOCK(self);                                                          \
    return EC_OK;                                                                   \
}                                                                                    \
                                                                                     \
static inline ec_status_t ec_dlist_##NAME##_pop_back(ec_dlist_##NAME##_t *self, TYPE *out) { \
    if (!self || !out) return EC_ERR_INVALID_ARG;                                    \
    EC_DLIST_LOCK(self);                                                            \
    if (self->size == 0) {                                                          \
        EC_DLIST_UNLOCK(self);                                                      \
        return EC_ERR_OUT_OF_BOUNDS;                                                \
    }                                                                               \
    ec_dlist_##NAME##_node_t *node = self->tail;                                    \
    *out = node->data;                                                              \
    self->tail = node->prev;                                                        \
    if (self->tail) {                                                               \
        self->tail->next = NULL;                                                    \
    } else {                                                                        \
        self->head = NULL;                                                          \
    }                                                                               \
    self->size--;                                                                   \
    if (!self->mempool) free(node);                                                 \
    EC_DLIST_UNLOCK(self);                                                          \
    return EC_OK;                                                                   \
}                                                                                    \
                                                                                     \
static inline ec_status_t ec_dlist_##NAME##_insert_at(ec_dlist_##NAME##_t *self, size_t index, TYPE val) { \
    if (!self) return EC_ERR_INVALID_ARG;                                            \
    EC_DLIST_LOCK(self);                                                            \
    if (index > self->size) {                                                       \
        EC_DLIST_UNLOCK(self);                                                      \
        return EC_ERR_OUT_OF_BOUNDS;                                                \
    }                                                                               \
    if (index == 0) {                                                               \
        ec_dlist_##NAME##_push_front(self, val);                                    \
        EC_DLIST_UNLOCK(self);                                                      \
        return EC_OK;                                                               \
    } else if (index == self->size) {                                               \
        ec_dlist_##NAME##_push_back(self, val);                                     \
        EC_DLIST_UNLOCK(self);                                                      \
        return EC_OK;                                                               \
    }                                                                               \
    ec_dlist_##NAME##_node_t *node = ec_dlist_##NAME##_alloc_node(self);            \
    if (!node) {                                                                    \
        EC_DLIST_UNLOCK(self);                                                      \
        return EC_ERR_ALLOC;                                                        \
    }                                                                               \
    node->data = val;                                                               \
    ec_dlist_##NAME##_node_t *cur = self->head;                                     \
    for (size_t i = 0; i < index; i++) {                                            \
        cur = cur->next;                                                            \
    }                                                                               \
    node->next = cur;                                                               \
    node->prev = cur->prev;                                                         \
    if (cur->prev) cur->prev->next = node;                                          \
    cur->prev = node;                                                               \
    self->size++;                                                                   \
    EC_DLIST_UNLOCK(self);                                                          \
    return EC_OK;                                                                   \
}                                                                                    \
                                                                                     \
static inline ec_status_t ec_dlist_##NAME##_remove(ec_dlist_##NAME##_t *self, TYPE val, int(*cmp)(const void*, const void*)) { \
    if (!self || !cmp) return EC_ERR_INVALID_ARG;                                    \
    EC_DLIST_LOCK(self);                                                            \
    ec_dlist_##NAME##_node_t *cur = self->head;                                     \
    while (cur) {                                                                   \
        if (cmp(&cur->data, &val) == 0) {                                           \
            if (cur->prev) cur->prev->next = cur->next;                             \
            else self->head = cur->next;                                            \
            if (cur->next) cur->next->prev = cur->prev;                             \
            else self->tail = cur->prev;                                            \
            self->size--;                                                           \
            if (!self->mempool) free(cur);                                          \
            EC_DLIST_UNLOCK(self);                                                  \
            return EC_OK;                                                           \
        }                                                                           \
        cur = cur->next;                                                            \
    }                                                                               \
    EC_DLIST_UNLOCK(self);                                                          \
    return EC_ERR_NOT_FOUND;                                                        \
}                                                                                    \
                                                                                     \
static inline ec_status_t ec_dlist_##NAME##_find(ec_dlist_##NAME##_t *self, TYPE val, size_t *idx, int(*cmp)(const void*, const void*)) { \
    if (!self || !idx || !cmp) return EC_ERR_INVALID_ARG;                            \
    EC_DLIST_LOCK(self);                                                            \
    size_t i = 0;                                                                   \
    for (ec_dlist_##NAME##_node_t *cur = self->head; cur; cur = cur->next) {        \
        if (cmp(&cur->data, &val) == 0) {                                           \
            *idx = i;                                                               \
            EC_DLIST_UNLOCK(self);                                                  \
            return EC_OK;                                                           \
        }                                                                           \
        i++;                                                                        \
    }                                                                               \
    EC_DLIST_UNLOCK(self);                                                          \
    return EC_ERR_NOT_FOUND;                                                        \
}                                                                                    \
                                                                                     \
static inline size_t ec_dlist_##NAME##_size(ec_dlist_##NAME##_t *self) {            \
    if (!self) return 0;                                                            \
    EC_DLIST_LOCK(self);                                                            \
    size_t sz = self->size;                                                         \
    EC_DLIST_UNLOCK(self);                                                          \
    return sz;                                                                      \
}                                                                                    \
                                                                                     \
/* Merge sort helpers for doubly-linked list */                                      \
static inline ec_dlist_##NAME##_node_t* ec_dlist_##NAME##_split(ec_dlist_##NAME##_node_t *head) { \
    ec_dlist_##NAME##_node_t *fast = head, *slow = head;                            \
    while (fast && fast->next && fast->next->next) {                                \
        fast = fast->next->next;                                                   \
        slow = slow->next;                                                         \
    }                                                                               \
    ec_dlist_##NAME##_node_t *mid = slow->next;                                     \
    slow->next = NULL;                                                             \
    if (mid) mid->prev = NULL;                                                     \
    return mid;                                                                     \
}                                                                                    \
                                                                                     \
static inline ec_dlist_##NAME##_node_t* ec_dlist_##NAME##_merge(ec_dlist_##NAME##_node_t *a, \
    ec_dlist_##NAME##_node_t *b, int(*cmp)(const void*, const void*)) {            \
    if (!a) return b;                                                               \
    if (!b) return a;                                                               \
    if (cmp(&a->data, &b->data) <= 0) {                                             \
        a->next = ec_dlist_##NAME##_merge(a->next, b, cmp);                         \
        a->next->prev = a;                                                         \
        a->prev = NULL;                                                            \
        return a;                                                                   \
    } else {                                                                        \
        b->next = ec_dlist_##NAME##_merge(a, b->next, cmp);                         \
        b->next->prev = b;                                                         \
        b->prev = NULL;                                                            \
        return b;                                                                   \
    }                                                                               \
}                                                                                    \
                                                                                     \
static inline ec_dlist_##NAME##_node_t* ec_dlist_##NAME##_merge_sort(ec_dlist_##NAME##_node_t *head, \
    int(*cmp)(const void*, const void*)) {                                          \
    if (!head || !head->next) return head;                                          \
    ec_dlist_##NAME##_node_t *mid = ec_dlist_##NAME##_split(head);                  \
    ec_dlist_##NAME##_node_t *left  = ec_dlist_##NAME##_merge_sort(head, cmp);      \
    ec_dlist_##NAME##_node_t *right = ec_dlist_##NAME##_merge_sort(mid,  cmp);      \
    return ec_dlist_##NAME##_merge(left, right, cmp);                               \
}                                                                                    \
                                                                                     \
static inline void ec_dlist_##NAME##_update_tail(ec_dlist_##NAME##_t *self) {       \
    ec_dlist_##NAME##_node_t *cur = self->head;                                     \
    self->tail = NULL;                                                              \
    while (cur) {                                                                   \
        if (!cur->next) self->tail = cur;                                           \
        cur = cur->next;                                                            \
    }                                                                               \
}                                                                                    \
                                                                                     \
static inline ec_status_t ec_dlist_##NAME##_sort(ec_dlist_##NAME##_t *self, int(*cmp)(const void*, const void*)) { \
    if (!self || !cmp) return EC_ERR_INVALID_ARG;                                   \
    EC_DLIST_LOCK(self);                                                           \
    if (self->size < 2) {                                                          \
        EC_DLIST_UNLOCK(self);                                                     \
        return EC_OK;                                                              \
    }                                                                               \
    self->head = ec_dlist_##NAME##_merge_sort(self->head, cmp);                    \
    ec_dlist_##NAME##_update_tail(self);                                           \
    EC_DLIST_UNLOCK(self);                                                         \
    return EC_OK;                                                                  \
}                                                                                    \
                                                                                     \
static inline ec_status_t ec_dlist_##NAME##_free(ec_dlist_##NAME##_t *self) {        \
    if (!self) return EC_ERR_INVALID_ARG;                                           \
    EC_DLIST_LOCK(self);                                                           \
    ec_dlist_##NAME##_node_t *cur = self->head;                                    \
    while (cur) {                                                                  \
        ec_dlist_##NAME##_node_t *next = cur->next;                                \
        if (!self->mempool) free(cur);                                             \
        cur = next;                                                                \
    }                                                                              \
    self->head = NULL;                                                             \
    self->tail = NULL;                                                             \
    self->size = 0;                                                                \
    EC_DLIST_UNLOCK(self);                                                         \
    EC_MUTEX_DESTROY(self->lock);                                                  \
    return EC_OK;                                                                  \
}                                                                                    \
                                                                                     \
static inline void ec_dlist_##NAME##_set_functions(ec_dlist_##NAME##_t *self) {      \
    self->push_front = ec_dlist_##NAME##_push_front;                                \
    self->push_back  = ec_dlist_##NAME##_push_back;                                 \
    self->pop_front  = ec_dlist_##NAME##_pop_front;                                 \
    self->pop_back   = ec_dlist_##NAME##_pop_back;                                  \
    self->insert_at  = ec_dlist_##NAME##_insert_at;                                 \
    self->remove     = ec_dlist_##NAME##_remove;                                    \
    self->find       = ec_dlist_##NAME##_find;                                      \
    self->sort       = ec_dlist_##NAME##_sort;                                      \
    self->free_fn    = ec_dlist_##NAME##_free;                                      \
    self->size_fn    = ec_dlist_##NAME##_size;                                      \
}                                                                                    \
                                                                                     \
static inline size_t ec_dlist_##NAME##_size(ec_dlist_##NAME##_t *self) {             \
    if (!self) return 0;                                                            \
    EC_DLIST_LOCK(self);                                                            \
    size_t s = self->size;                                                          \
    EC_DLIST_UNLOCK(self);                                                          \
    return s;                                                                       \
}

/* Provide expansions for typical types */
EC_DLIST_DEF(int, int)
EC_DLIST_DEF(unsigned int, uint)
EC_DLIST_DEF(long, long)
EC_DLIST_DEF(long long, ll)
EC_DLIST_DEF(unsigned long, ulong)
EC_DLIST_DEF(unsigned long long, ull)
EC_DLIST_DEF(uint32_t, u32)
EC_DLIST_DEF(uint64_t, u64)
EC_DLIST_DEF(double, double)
EC_DLIST_DEF(const char*, str)
EC_DLIST_DEF(void*, ptr)

#endif /* EC_DLIST_H */
