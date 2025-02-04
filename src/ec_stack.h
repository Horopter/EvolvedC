#ifndef EC_STACK_H
#define EC_STACK_H
#include "ec_common.h"
#include "ec_status.h"
#ifdef EC_USE_MEMPOOL
#define EC_STACK_ALLOC(TYPE, MPOOL) ((MPOOL) ? (TYPE *)ec_mempool_alloc((ec_mempool_t *)(MPOOL), sizeof(TYPE)) : (TYPE *)EC_MALLOC(sizeof(TYPE)))
#else
#define EC_STACK_ALLOC(TYPE, MPOOL) (TYPE *)EC_MALLOC(sizeof(TYPE))
#endif
#define EC_STACK_DEF(TYPE, NAME)                                                                                                                  \
    typedef struct ec_stack_##NAME##_node_s                                                                                                       \
    {                                                                                                                                             \
        TYPE data;                                                                                                                                \
        struct ec_stack_##NAME##_node_s *next;                                                                                                    \
    } ec_stack_##NAME##_node_t;                                                                                                                   \
    typedef struct ec_stack_##NAME##_s ec_stack_##NAME##_t;                                                                                \
    typedef struct ec_stack_##NAME##_iter_s                                                                                                       \
    {                                                                                                                                             \
        ec_stack_##NAME##_t *stack;                                                                                                               \
        ec_stack_##NAME##_node_t *current;                                                                                                        \
        ec_stack_##NAME##_node_t *prev;                                                                                                           \
        size_t idx;                                                                                                                               \
        bool init;                                                                                                                                \
    } ec_stack_##NAME##_iter_t;                                                                                                                   \
    typedef struct ec_stack_##NAME##_zip_iter_s                                                                                                   \
    {                                                                                                                                             \
        ec_stack_##NAME##_t *s1;                                                                                                                  \
        ec_stack_##NAME##_t *s2;                                                                                                                  \
        ec_stack_##NAME##_node_t *c1;                                                                                                             \
        ec_stack_##NAME##_node_t *c2;                                                                                                             \
        size_t idx;                                                                                                                               \
    } ec_stack_##NAME##_zip_iter_t;                                                                                                               \
    typedef struct ec_stack_##NAME##_s                                                                                                            \
    {                                                                                                                                             \
        ec_stack_##NAME##_node_t *top;                                                                                                            \
        size_t size;                                                                                                                              \
        EC_MUTEX_TYPE lock;                                                                                                                       \
        void *mempool;                                                                                                                            \
        size_t (*struct_size)(struct ec_stack_##NAME##_s *);                                                                                      \
        ec_status_t (*push)(struct ec_stack_##NAME##_s *, void *);                                                                                \
        ec_status_t (*peek)(struct ec_stack_##NAME##_s *, void **);                                                                               \
        ec_status_t (*pop)(struct ec_stack_##NAME##_s *, void **);                                                                                \
        size_t (*size_fn)(struct ec_stack_##NAME##_s *);                                                                                          \
        void (*iter_init)(struct ec_stack_##NAME##_iter_s *, struct ec_stack_##NAME##_s *);                                                       \
        ec_status_t (*iter_next)(struct ec_stack_##NAME##_iter_s *, void **);                                                                     \
        ec_status_t (*iter_replace)(struct ec_stack_##NAME##_iter_s *, void *, void **);                                                          \
        void (*zip_iter_init)(struct ec_stack_##NAME##_zip_iter_s *, struct ec_stack_##NAME##_s *, struct ec_stack_##NAME##_s *);                 \
        ec_status_t (*zip_iter_next)(struct ec_stack_##NAME##_zip_iter_s *, void **, void **);                                                    \
        ec_status_t (*zip_iter_replace)(struct ec_stack_##NAME##_zip_iter_s *, void *, void *, void **, void **);                                 \
        ec_status_t (*free_fn)(struct ec_stack_##NAME##_s *);                                                                                     \
    } ec_stack_##NAME##_t;                                                                                                                        \
    static inline size_t ec_stack_##NAME##_struct_size(ec_stack_##NAME##_t *s)                                                                    \
    {                                                                                                                                             \
        (void)s;                                                                                                                                  \
        return sizeof(ec_stack_##NAME##_t);                                                                                                       \
    }                                                                                                                                             \
    static inline ec_stack_##NAME##_node_t *ec_stack_##NAME##_make_node(ec_stack_##NAME##_t *s, TYPE d)                                           \
    {                                                                                                                                             \
        ec_stack_##NAME##_node_t *n = EC_STACK_ALLOC(ec_stack_##NAME##_node_t, s->mempool);                                                       \
        if (n)                                                                                                                                    \
        {                                                                                                                                         \
            n->data = d;                                                                                                                          \
            n->next = NULL;                                                                                                                       \
        }                                                                                                                                         \
        return n;                                                                                                                                 \
    }                                                                                                                                             \
    static inline ec_status_t ec_stack_##NAME##_push(ec_stack_##NAME##_t *s, void *elem)                                                          \
    {                                                                                                                                             \
        if (!s)                                                                                                                                   \
            return EC_ERR_INVALID_ARG;                                                                                                            \
        EC_MUTEX_LOCK(s->lock);                                                                                                                   \
        ec_stack_##NAME##_node_t *n = ec_stack_##NAME##_make_node(s, *(TYPE *)elem);                                                              \
        if (!n)                                                                                                                                   \
        {                                                                                                                                         \
            EC_MUTEX_UNLOCK(s->lock);                                                                                                             \
            return EC_ERR_ALLOC;                                                                                                                  \
        }                                                                                                                                         \
        n->next = s->top;                                                                                                                         \
        s->top = n;                                                                                                                               \
        s->size++;                                                                                                                                \
        EC_MUTEX_UNLOCK(s->lock);                                                                                                                 \
        return EC_OK;                                                                                                                             \
    }                                                                                                                                             \
    static inline ec_status_t ec_stack_##NAME##_peek(ec_stack_##NAME##_t *s, void **out)                                                          \
    {                                                                                                                                             \
        if (!s || !out)                                                                                                                           \
            return EC_ERR_INVALID_ARG;                                                                                                            \
        EC_MUTEX_LOCK(s->lock);                                                                                                                   \
        if (!s->top)                                                                                                                              \
        {                                                                                                                                         \
            EC_MUTEX_UNLOCK(s->lock);                                                                                                             \
            return EC_ERR_OUT_OF_BOUNDS;                                                                                                          \
        }                                                                                                                                         \
        *out = &s->top->data;                                                                                                                     \
        EC_MUTEX_UNLOCK(s->lock);                                                                                                                 \
        return EC_OK;                                                                                                                             \
    }                                                                                                                                             \
    static inline ec_status_t ec_stack_##NAME##_pop(ec_stack_##NAME##_t *s, void **out)                                                           \
    {                                                                                                                                             \
        if (!s || !out)                                                                                                                           \
            return EC_ERR_INVALID_ARG;                                                                                                            \
        EC_MUTEX_LOCK(s->lock);                                                                                                                   \
        if (!s->top)                                                                                                                              \
        {                                                                                                                                         \
            EC_MUTEX_UNLOCK(s->lock);                                                                                                             \
            return EC_ERR_OUT_OF_BOUNDS;                                                                                                          \
        }                                                                                                                                         \
        ec_stack_##NAME##_node_t *n = s->top;                                                                                                     \
        *out = &n->data;                                                                                                                          \
        s->top = n->next;                                                                                                                         \
        s->size--;                                                                                                                                \
        EC_FREE(n);                                                                                                                               \
        EC_MUTEX_UNLOCK(s->lock);                                                                                                                 \
        return EC_OK;                                                                                                                             \
    }                                                                                                                                             \
    static inline size_t ec_stack_##NAME##_size_fn(ec_stack_##NAME##_t *s)                                                                        \
    {                                                                                                                                             \
        if (!s)                                                                                                                                   \
            return 0;                                                                                                                             \
        EC_MUTEX_LOCK(s->lock);                                                                                                                   \
        size_t sz = s->size;                                                                                                                      \
        EC_MUTEX_UNLOCK(s->lock);                                                                                                                 \
        return sz;                                                                                                                                \
    }                                                                                                                                             \
    static inline void ec_stack_##NAME##_iter_init(ec_stack_##NAME##_iter_t *it, ec_stack_##NAME##_t *s)                                          \
    {                                                                                                                                             \
        it->stack = s;                                                                                                                            \
        it->current = s->top;                                                                                                                     \
        it->prev = NULL;                                                                                                                          \
        it->idx = 0;                                                                                                                              \
        it->init = true;                                                                                                                          \
    }                                                                                                                                             \
    static inline ec_status_t ec_stack_##NAME##_iter_next(ec_stack_##NAME##_iter_t *it, void **out)                                               \
    {                                                                                                                                             \
        if (!it || !out)                                                                                                                          \
            return EC_ERR_INVALID_ARG;                                                                                                            \
        EC_MUTEX_LOCK(it->stack->lock);                                                                                                           \
        if (!it->current)                                                                                                                         \
        {                                                                                                                                         \
            EC_MUTEX_UNLOCK(it->stack->lock);                                                                                                     \
            return EC_ITER_END;                                                                                                                   \
        }                                                                                                                                         \
        *out = &it->current->data;                                                                                                                \
        it->prev = it->current;                                                                                                                   \
        it->current = it->current->next;                                                                                                          \
        it->idx++;                                                                                                                                \
        EC_MUTEX_UNLOCK(it->stack->lock);                                                                                                         \
        return EC_OK;                                                                                                                             \
    }                                                                                                                                             \
    static inline ec_status_t ec_stack_##NAME##_iter_replace(ec_stack_##NAME##_iter_t *it, void *elem, void **out)                                \
    {                                                                                                                                             \
        if (!it || !it->prev)                                                                                                                     \
            return EC_ERR_INVALID_ARG;                                                                                                            \
        EC_MUTEX_LOCK(it->stack->lock);                                                                                                           \
        if (out)                                                                                                                                  \
            *out = &it->prev->data;                                                                                                               \
        it->prev->data = *(TYPE *)elem;                                                                                                           \
        EC_MUTEX_UNLOCK(it->stack->lock);                                                                                                         \
        return EC_OK;                                                                                                                             \
    }                                                                                                                                             \
    static inline void ec_stack_##NAME##_zip_iter_init(ec_stack_##NAME##_zip_iter_t *zit, ec_stack_##NAME##_t *s1, ec_stack_##NAME##_t *s2)       \
    {                                                                                                                                             \
        zit->s1 = s1;                                                                                                                             \
        zit->s2 = s2;                                                                                                                             \
        zit->c1 = s1->top;                                                                                                                        \
        zit->c2 = s2->top;                                                                                                                        \
        zit->idx = 0;                                                                                                                             \
    }                                                                                                                                             \
    static inline ec_status_t ec_stack_##NAME##_zip_iter_next(ec_stack_##NAME##_zip_iter_t *zit, void **out1, void **out2)                        \
    {                                                                                                                                             \
        if (!zit || !out1 || !out2)                                                                                                               \
            return EC_ERR_INVALID_ARG;                                                                                                            \
        EC_MUTEX_LOCK(zit->s1->lock);                                                                                                             \
        EC_MUTEX_LOCK(zit->s2->lock);                                                                                                             \
        if (!zit->c1 || !zit->c2)                                                                                                                 \
        {                                                                                                                                         \
            EC_MUTEX_UNLOCK(zit->s2->lock);                                                                                                       \
            EC_MUTEX_UNLOCK(zit->s1->lock);                                                                                                       \
            return EC_ITER_END;                                                                                                                   \
        }                                                                                                                                         \
        *out1 = &zit->c1->data;                                                                                                                   \
        *out2 = &zit->c2->data;                                                                                                                   \
        zit->c1 = zit->c1->next;                                                                                                                  \
        zit->c2 = zit->c2->next;                                                                                                                  \
        zit->idx++;                                                                                                                               \
        EC_MUTEX_UNLOCK(zit->s2->lock);                                                                                                           \
        EC_MUTEX_UNLOCK(zit->s1->lock);                                                                                                           \
        return EC_OK;                                                                                                                             \
    }                                                                                                                                             \
    static inline ec_status_t ec_stack_##NAME##_zip_iter_replace(ec_stack_##NAME##_zip_iter_t *zit, void *e1, void *e2, void **out1, void **out2) \
    {                                                                                                                                             \
        return EC_ERR_NOT_SUPPORTED;                                                                                                              \
    }                                                                                                                                             \
    static inline ec_status_t ec_stack_##NAME##_free_fn(ec_stack_##NAME##_t *s)                                                                   \
    {                                                                                                                                             \
        if (!s)                                                                                                                                   \
            return EC_ERR_INVALID_ARG;                                                                                                            \
        EC_MUTEX_LOCK(s->lock);                                                                                                                   \
        ec_stack_##NAME##_node_t *cur = s->top;                                                                                                   \
        while (cur)                                                                                                                               \
        {                                                                                                                                         \
            ec_stack_##NAME##_node_t *tmp = cur->next;                                                                                            \
            EC_FREE(cur);                                                                                                                         \
            cur = tmp;                                                                                                                            \
        }                                                                                                                                         \
        s->top = NULL;                                                                                                                            \
        s->size = 0;                                                                                                                              \
        EC_MUTEX_UNLOCK(s->lock);                                                                                                                 \
        EC_MUTEX_DESTROY(s->lock);                                                                                                                \
        return EC_OK;                                                                                                                             \
    }                                                                                                                                             \
    static inline ec_status_t ec_stack_##NAME##_init(ec_stack_##NAME##_t *s, void *mempool)                                                       \
    {                                                                                                                                             \
        if (!s)                                                                                                                                   \
            return EC_ERR_INVALID_ARG;                                                                                                            \
        EC_MUTEX_INIT(s->lock);                                                                                                                   \
        s->top = NULL;                                                                                                                            \
        s->size = 0;                                                                                                                              \
        s->mempool = mempool;                                                                                                                     \
        s->struct_size = ec_stack_##NAME##_struct_size;                                                                                           \
        s->push = ec_stack_##NAME##_push;                                                                                                         \
        s->peek = ec_stack_##NAME##_peek;                                                                                                         \
        s->pop = ec_stack_##NAME##_pop;                                                                                                           \
        s->size_fn = ec_stack_##NAME##_size_fn;                                                                                                   \
        s->iter_init = ec_stack_##NAME##_iter_init;                                                                                               \
        s->iter_next = ec_stack_##NAME##_iter_next;                                                                                               \
        s->iter_replace = ec_stack_##NAME##_iter_replace;                                                                                         \
        s->zip_iter_init = ec_stack_##NAME##_zip_iter_init;                                                                                       \
        s->zip_iter_next = ec_stack_##NAME##_zip_iter_next;                                                                                       \
        s->zip_iter_replace = ec_stack_##NAME##_zip_iter_replace;                                                                                 \
        s->free_fn = ec_stack_##NAME##_free_fn;                                                                                                   \
        return EC_OK;                                                                                                                             \
    }
EC_STACK_DEF(int, int)
EC_STACK_DEF(unsigned int, uint)
EC_STACK_DEF(long, long)
EC_STACK_DEF(long long, ll)
EC_STACK_DEF(unsigned long, ul)
EC_STACK_DEF(unsigned long long, ull)
EC_STACK_DEF(uint32_t, u32)
EC_STACK_DEF(uint64_t, u64)
EC_STACK_DEF(double, dbl)
EC_STACK_DEF(const char *, str)
EC_STACK_DEF(void *, ptr)
#endif
