#ifndef EC_SKIPLIST_H
#define EC_SKIPLIST_H
#include "ec_common.h"
#include "ec_status.h"
#ifdef EC_USE_MEMPOOL
#define EC_SKIPLIST_ALLOC(T, MPOOL) ((MPOOL) ? (T *)ec_mempool_alloc((ec_mempool_t *)(MPOOL), sizeof(T)) : (T *)EC_MALLOC(sizeof(T)))
#define EC_SKIPLIST_CALLOC(N, T, MPOOL) ((MPOOL) ? (T *)ec_mempool_alloc((ec_mempool_t *)(MPOOL), (N) * sizeof(T)) : (T *)EC_CALLOC(N, sizeof(T)))
#define EC_SKIPLIST_ALLOC_SIZE(S, MPOOL) ((MPOOL) ? ec_mempool_alloc((ec_mempool_t *)(MPOOL), (S)) : EC_MALLOC(S))
#else
#define EC_SKIPLIST_ALLOC(T, MPOOL) (T *)EC_MALLOC(sizeof(T))
#define EC_SKIPLIST_CALLOC(N, T, MPOOL) (T *)EC_CALLOC(N, sizeof(T))
#define EC_SKIPLIST_ALLOC_SIZE(S, MPOOL) EC_MALLOC(S)
#endif
#ifndef EC_MAX_SKIPLIST_LEVEL
#define EC_MAX_SKIPLIST_LEVEL 32
#endif
#ifndef EC_SKIPLIST_P
#define EC_SKIPLIST_P 0.25
#endif
#define EC_SKIPLIST_RANDOM_LEVEL() ({int lvl=1; while(((double)rand()/RAND_MAX)<EC_SKIPLIST_P && lvl<EC_MAX_SKIPLIST_LEVEL) lvl++; lvl; })
#define EC_SKIPLIST_DEF(TYPE, NAME)                                                                                                  \
    typedef struct ec_skiplist_##NAME##_node_s                                                                                       \
    {                                                                                                                                \
        TYPE key;                                                                                                                    \
        int level;                                                                                                                   \
        struct ec_skiplist_##NAME##_node_s *forward[];                                                                               \
    } ec_skiplist_##NAME##_node_t;                                                                                                   \
    typedef struct ec_skiplist_##NAME##_iter_s                                                                                       \
    {                                                                                                                                \
        struct ec_skiplist_##NAME##_s *s;                                                                                            \
        ec_skiplist_##NAME##_node_t *curr;                                                                                           \
    } ec_skiplist_##NAME##_iter_t;                                                                                                   \
    typedef struct ec_skiplist_##NAME##_s                                                                                            \
    {                                                                                                                                \
        int level;                                                                                                                   \
        size_t size;                                                                                                                 \
        ec_skiplist_##NAME##_node_t *header;                                                                                         \
        int (*compare)(const TYPE *, const TYPE *);                                                                                  \
        EC_MUTEX_TYPE lock;                                                                                                          \
        void *mempool;                                                                                                               \
        ec_status_t (*add)(struct ec_skiplist_##NAME##_s *, const TYPE *);                                                           \
        ec_status_t (*remove_f)(struct ec_skiplist_##NAME##_s *, const TYPE *);                                                      \
        ec_status_t (*remove_all)(struct ec_skiplist_##NAME##_s *);                                                                  \
        ec_status_t (*contains)(struct ec_skiplist_##NAME##_s *, const TYPE *);                                                      \
        size_t (*size_fn)(struct ec_skiplist_##NAME##_s *);                                                                          \
        ec_status_t (*get_keys)(struct ec_skiplist_##NAME##_s *, TYPE **, size_t *);                                                 \
        ec_status_t (*foreach_key)(struct ec_skiplist_##NAME##_s *, void (*)(TYPE));                                                 \
        ec_status_t (*iter_init)(struct ec_skiplist_##NAME##_s *, ec_skiplist_##NAME##_iter_t *);                                    \
        ec_status_t (*iter_next)(ec_skiplist_##NAME##_iter_t *, TYPE *);                                                             \
        ec_status_t (*iter_remove)(ec_skiplist_##NAME##_iter_t *);                                                                   \
    } ec_skiplist_##NAME##_t;                                                                                                        \
    static inline int ec_skiplist_##NAME##_dfcmp(const TYPE *a, const TYPE *b) { return (*a < *b) ? -1 : ((*a > *b) ? 1 : 0); }      \
    static inline ec_skiplist_##NAME##_node_t *ec_skiplist_##NAME##_create_node(int lvl, const TYPE *key, void *mp)                  \
    {                                                                                                                                \
        size_t sz = sizeof(ec_skiplist_##NAME##_node_t) + lvl * sizeof(ec_skiplist_##NAME##_node_t *);                               \
        ec_skiplist_##NAME##_node_t *n = (ec_skiplist_##NAME##_node_t *)EC_SKIPLIST_ALLOC_SIZE(sz, mp);                              \
        if (n)                                                                                                                       \
        {                                                                                                                            \
            n->level = lvl;                                                                                                          \
            if (key)                                                                                                                 \
                n->key = *key;                                                                                                       \
            else                                                                                                                     \
                memset(&n->key, 0, sizeof(TYPE));                                                                                    \
            for (int i = 0; i < lvl; i++)                                                                                            \
                n->forward[i] = NULL;                                                                                                \
        }                                                                                                                            \
        return n;                                                                                                                    \
    }                                                                                                                                \
    static inline ec_status_t ec_skiplist_##NAME##_add_f(struct ec_skiplist_##NAME##_s *s, const TYPE *key)                          \
    {                                                                                                                                \
        if (!s || !key)                                                                                                              \
            return EC_ERR_INVALID_ARG;                                                                                               \
        EC_MUTEX_LOCK(s->lock);                                                                                                      \
        ec_skiplist_##NAME##_node_t *update[EC_MAX_SKIPLIST_LEVEL], *x = s->header;                                                  \
        for (int i = s->level - 1; i >= 0; i--)                                                                                      \
        {                                                                                                                            \
            while (x->forward[i] && s->compare(&x->forward[i]->key, key) < 0)                                                        \
                x = x->forward[i];                                                                                                   \
            update[i] = x;                                                                                                           \
        }                                                                                                                            \
        x = x->forward[0];                                                                                                           \
        if (x && s->compare(&x->key, key) == 0)                                                                                      \
        {                                                                                                                            \
            EC_MUTEX_UNLOCK(s->lock);                                                                                                \
            return EC_OK;                                                                                                            \
        }                                                                                                                            \
        int lvl = EC_SKIPLIST_RANDOM_LEVEL();                                                                                        \
        if (lvl > s->level)                                                                                                          \
        {                                                                                                                            \
            for (int i = s->level; i < lvl; i++)                                                                                     \
                update[i] = s->header;                                                                                               \
            s->level = lvl;                                                                                                          \
        }                                                                                                                            \
        x = ec_skiplist_##NAME##_create_node(lvl, key, s->mempool);                                                                  \
        if (!x)                                                                                                                      \
        {                                                                                                                            \
            EC_MUTEX_UNLOCK(s->lock);                                                                                                \
            return EC_ERR_ALLOC;                                                                                                     \
        }                                                                                                                            \
        for (int i = 0; i < lvl; i++)                                                                                                \
        {                                                                                                                            \
            x->forward[i] = update[i]->forward[i];                                                                                   \
            update[i]->forward[i] = x;                                                                                               \
        }                                                                                                                            \
        s->size++;                                                                                                                   \
        EC_MUTEX_UNLOCK(s->lock);                                                                                                    \
        return EC_OK;                                                                                                                \
    }                                                                                                                                \
    static inline ec_status_t ec_skiplist_##NAME##_remove_f(struct ec_skiplist_##NAME##_s *s, const TYPE *key)                       \
    {                                                                                                                                \
        if (!s || !key)                                                                                                              \
            return EC_ERR_INVALID_ARG;                                                                                               \
        EC_MUTEX_LOCK(s->lock);                                                                                                      \
        ec_skiplist_##NAME##_node_t *update[EC_MAX_SKIPLIST_LEVEL], *x = s->header;                                                  \
        for (int i = s->level - 1; i >= 0; i--)                                                                                      \
        {                                                                                                                            \
            while (x->forward[i] && s->compare(&x->forward[i]->key, key) < 0)                                                        \
                x = x->forward[i];                                                                                                   \
            update[i] = x;                                                                                                           \
        }                                                                                                                            \
        x = x->forward[0];                                                                                                           \
        if (!x || s->compare(&x->key, key) != 0)                                                                                     \
        {                                                                                                                            \
            EC_MUTEX_UNLOCK(s->lock);                                                                                                \
            return EC_ERR_NOT_FOUND;                                                                                                 \
        }                                                                                                                            \
        for (int i = 0; i < s->level; i++)                                                                                           \
        {                                                                                                                            \
            if (update[i]->forward[i] != x)                                                                                          \
                break;                                                                                                               \
            else                                                                                                                     \
                update[i]->forward[i] = x->forward[i];                                                                               \
        }                                                                                                                            \
        EC_FREE(x);                                                                                                                  \
        while (s->level > 1 && s->header->forward[s->level - 1] == NULL)                                                             \
            s->level--;                                                                                                              \
        s->size--;                                                                                                                   \
        EC_MUTEX_UNLOCK(s->lock);                                                                                                    \
        return EC_OK;                                                                                                                \
    }                                                                                                                                \
    static inline ec_status_t ec_skiplist_##NAME##_remove_all_f(struct ec_skiplist_##NAME##_s *s)                                    \
    {                                                                                                                                \
        if (!s)                                                                                                                      \
            return EC_ERR_INVALID_ARG;                                                                                               \
        EC_MUTEX_LOCK(s->lock);                                                                                                      \
        ec_skiplist_##NAME##_node_t *x = s->header->forward[0], *tmp;                                                                \
        while (x)                                                                                                                    \
        {                                                                                                                            \
            tmp = x->forward[0];                                                                                                     \
            EC_FREE(x);                                                                                                              \
            x = tmp;                                                                                                                 \
        }                                                                                                                            \
        for (int i = 0; i < EC_MAX_SKIPLIST_LEVEL; i++)                                                                              \
            s->header->forward[i] = NULL;                                                                                            \
        s->level = 1;                                                                                                                \
        s->size = 0;                                                                                                                 \
        EC_MUTEX_UNLOCK(s->lock);                                                                                                    \
        return EC_OK;                                                                                                                \
    }                                                                                                                                \
    static inline ec_status_t ec_skiplist_##NAME##_contains_f(struct ec_skiplist_##NAME##_s *s, const TYPE *key)                     \
    {                                                                                                                                \
        if (!s || !key)                                                                                                              \
            return EC_ERR_INVALID_ARG;                                                                                               \
        EC_MUTEX_LOCK(s->lock);                                                                                                      \
        ec_skiplist_##NAME##_node_t *x = s->header;                                                                                  \
        for (int i = s->level - 1; i >= 0; i--)                                                                                      \
        {                                                                                                                            \
            while (x->forward[i] && s->compare(&x->forward[i]->key, key) < 0)                                                        \
                x = x->forward[i];                                                                                                   \
        }                                                                                                                            \
        x = x->forward[0];                                                                                                           \
        ec_status_t st = (x && s->compare(&x->key, key) == 0) ? EC_OK : EC_ERR_NOT_FOUND;                                            \
        EC_MUTEX_UNLOCK(s->lock);                                                                                                    \
        return st;                                                                                                                   \
    }                                                                                                                                \
    static inline size_t ec_skiplist_##NAME##_size_f(struct ec_skiplist_##NAME##_s *s)                                               \
    {                                                                                                                                \
        if (!s)                                                                                                                      \
            return 0;                                                                                                                \
        EC_MUTEX_LOCK(s->lock);                                                                                                      \
        size_t sz = s->size;                                                                                                         \
        EC_MUTEX_UNLOCK(s->lock);                                                                                                    \
        return sz;                                                                                                                   \
    }                                                                                                                                \
    static inline ec_status_t ec_skiplist_##NAME##_get_keys_f(struct ec_skiplist_##NAME##_s *s, TYPE **arr, size_t *count)           \
    {                                                                                                                                \
        if (!s || !arr || !count)                                                                                                    \
            return EC_ERR_INVALID_ARG;                                                                                               \
        EC_MUTEX_LOCK(s->lock);                                                                                                      \
        *count = s->size;                                                                                                            \
        if (s->size == 0)                                                                                                            \
        {                                                                                                                            \
            *arr = NULL;                                                                                                             \
            EC_MUTEX_UNLOCK(s->lock);                                                                                                \
            return EC_OK;                                                                                                            \
        }                                                                                                                            \
        *arr = (TYPE *)EC_MALLOC(sizeof(TYPE) * s->size);                                                                            \
        if (!*arr)                                                                                                                   \
        {                                                                                                                            \
            EC_MUTEX_UNLOCK(s->lock);                                                                                                \
            return EC_ERR_ALLOC;                                                                                                     \
        }                                                                                                                            \
        size_t idx = 0;                                                                                                              \
        ec_skiplist_##NAME##_node_t *x = s->header->forward[0];                                                                      \
        while (x)                                                                                                                    \
        {                                                                                                                            \
            (*arr)[idx++] = x->key;                                                                                                  \
            x = x->forward[0];                                                                                                       \
        }                                                                                                                            \
        EC_MUTEX_UNLOCK(s->lock);                                                                                                    \
        return EC_OK;                                                                                                                \
    }                                                                                                                                \
    static inline ec_status_t ec_skiplist_##NAME##_foreach_key_f(struct ec_skiplist_##NAME##_s *s, void (*fn)(TYPE))                 \
    {                                                                                                                                \
        if (!s || !fn)                                                                                                               \
            return EC_ERR_INVALID_ARG;                                                                                               \
        EC_MUTEX_LOCK(s->lock);                                                                                                      \
        ec_skiplist_##NAME##_node_t *x = s->header->forward[0];                                                                      \
        while (x)                                                                                                                    \
        {                                                                                                                            \
            fn(x->key);                                                                                                              \
            x = x->forward[0];                                                                                                       \
        }                                                                                                                            \
        EC_MUTEX_UNLOCK(s->lock);                                                                                                    \
        return EC_OK;                                                                                                                \
    }                                                                                                                                \
    static inline ec_status_t ec_skiplist_##NAME##_iter_init_f(struct ec_skiplist_##NAME##_s *s, ec_skiplist_##NAME##_iter_t *it)    \
    {                                                                                                                                \
        if (!s || !it)                                                                                                               \
            return EC_ERR_INVALID_ARG;                                                                                               \
        it->s = s;                                                                                                                   \
        it->curr = s->header->forward[0];                                                                                            \
        return EC_OK;                                                                                                                \
    }                                                                                                                                \
    static inline ec_status_t ec_skiplist_##NAME##_iter_next_f(ec_skiplist_##NAME##_iter_t *it, TYPE *key)                           \
    {                                                                                                                                \
        if (!it || !key)                                                                                                             \
            return EC_ERR_INVALID_ARG;                                                                                               \
        EC_MUTEX_LOCK(it->s->lock);                                                                                                  \
        if (it->curr)                                                                                                                \
        {                                                                                                                            \
            *key = it->curr->key;                                                                                                    \
            it->curr = it->curr->forward[0];                                                                                         \
            EC_MUTEX_UNLOCK(it->s->lock);                                                                                            \
            return EC_OK;                                                                                                            \
        }                                                                                                                            \
        EC_MUTEX_UNLOCK(it->s->lock);                                                                                                \
        return EC_ERR_OUT_OF_BOUNDS;                                                                                                 \
    }                                                                                                                                \
    static inline ec_status_t ec_skiplist_##NAME##_iter_remove_f(ec_skiplist_##NAME##_iter_t *it)                                    \
    {                                                                                                                                \
        if (!it || !it->s || !it->curr)                                                                                              \
            return EC_ERR_INVALID_ARG;                                                                                               \
        TYPE k = it->curr->key;                                                                                                      \
        ec_status_t st = ec_skiplist_##NAME##_remove_f(it->s, &k);                                                                   \
        it->curr = it->s->header->forward[0];                                                                                        \
        return st;                                                                                                                   \
    }                                                                                                                                \
    static inline ec_status_t ec_skiplist_##NAME##_init(ec_skiplist_##NAME##_t *s, void *mp, int (*cmp)(const TYPE *, const TYPE *)) \
    {                                                                                                                                \
        if (!s)                                                                                                                      \
            return EC_ERR_INVALID_ARG;                                                                                               \
        EC_MUTEX_INIT(s->lock);                                                                                                      \
        s->level = 1;                                                                                                                \
        s->size = 0;                                                                                                                 \
        s->mempool = mp;                                                                                                             \
        s->header = ec_skiplist_##NAME##_create_node(EC_MAX_SKIPLIST_LEVEL, NULL, mp);                                               \
        if (!s->header)                                                                                                              \
            return EC_ERR_ALLOC;                                                                                                     \
        s->compare = cmp ? cmp : ec_skiplist_##NAME##_dfcmp;                                                                         \
        s->add = ec_skiplist_##NAME##_add_f;                                                                                         \
        s->remove_f = ec_skiplist_##NAME##_remove_f;                                                                                 \
        s->remove_all = ec_skiplist_##NAME##_remove_all_f;                                                                           \
        s->contains = ec_skiplist_##NAME##_contains_f;                                                                               \
        s->size_fn = ec_skiplist_##NAME##_size_f;                                                                                    \
        s->get_keys = ec_skiplist_##NAME##_get_keys_f;                                                                               \
        s->foreach_key = ec_skiplist_##NAME##_foreach_key_f;                                                                         \
        s->iter_init = ec_skiplist_##NAME##_iter_init_f;                                                                             \
        s->iter_next = ec_skiplist_##NAME##_iter_next_f;                                                                             \
        s->iter_remove = ec_skiplist_##NAME##_iter_remove_f;                                                                         \
        return EC_OK;                                                                                                                \
    }                                                                                                                                
    EC_SKIPLIST_DEF(int, int)                                                                                                        
    EC_SKIPLIST_DEF(unsigned int, uint)                                                                                              
    EC_SKIPLIST_DEF(long, long)                                                                                                      
    EC_SKIPLIST_DEF(long long, ll)                                                                                                   
    EC_SKIPLIST_DEF(unsigned long, ul)                                                                                               
    EC_SKIPLIST_DEF(unsigned long long, ull)                                                                                         
    EC_SKIPLIST_DEF(uint32_t, u32)                                                                                                   
    EC_SKIPLIST_DEF(uint64_t, u64)                                                                                                   
    EC_SKIPLIST_DEF(double, dbl)                                                                                                     
    EC_SKIPLIST_DEF(const char *, str)                                                                                               
    EC_SKIPLIST_DEF(void *, ptr)
#endif
