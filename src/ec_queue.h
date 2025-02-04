#ifndef EC_QUEUE_H
#define EC_QUEUE_H

#include "ec_common.h"
#include "ec_status.h"

#ifdef EC_USE_MEMPOOL
#define EC_QUEUE_ALLOC(TYPE, MPOOL) ((MPOOL) ? (TYPE *)ec_mempool_alloc((ec_mempool_t *)(MPOOL), sizeof(TYPE)) : (TYPE *)EC_MALLOC(sizeof(TYPE)))
#else
#define EC_QUEUE_ALLOC(TYPE, MPOOL) (TYPE *)EC_MALLOC(sizeof(TYPE))
#endif

#define EC_MEMCPY_SAFE(DEST, SRC, SIZE) \
    do                                  \
    {                                   \
        if ((DEST) == NULL)             \
        {                               \
            (DEST) = malloc((SIZE));    \
            if ((DEST) == NULL)         \
            {                           \
                return EC_ERR_ALLOC;    \
            }                           \
        }                               \
        memcpy((DEST), (SRC), (SIZE));  \
    } while (0)

#define EC_QUEUE_DEF(TYPE, NAME)                                                                                                                  \
    typedef struct ec_queue_##NAME##_node_s ec_queue_##NAME##_node_t;                                                                             \
    typedef struct ec_queue_##NAME##_s ec_queue_##NAME##_t;                                                                                       \
    typedef struct ec_queue_##NAME##_iter_s ec_queue_##NAME##_iter_t;                                                                             \
    typedef struct ec_queue_##NAME##_zip_iter_s ec_queue_##NAME##_zip_iter_t;                                                                     \
                                                                                                                                                  \
    struct ec_queue_##NAME##_node_s                                                                                                               \
    {                                                                                                                                             \
        TYPE data;                                                                                                                                \
        ec_queue_##NAME##_node_t *next;                                                                                                           \
    };                                                                                                                                            \
                                                                                                                                                  \
    struct ec_queue_##NAME##_s                                                                                                                    \
    {                                                                                                                                             \
        ec_queue_##NAME##_node_t *front;                                                                                                          \
        ec_queue_##NAME##_node_t *rear;                                                                                                           \
        size_t size;                                                                                                                              \
        EC_MUTEX_TYPE lock;                                                                                                                       \
        void *mempool;                                                                                                                            \
        size_t (*struct_size)(ec_queue_##NAME##_t *);                                                                                             \
        ec_status_t (*enqueue)(ec_queue_##NAME##_t *, void *);                                                                                    \
        ec_status_t (*dequeue)(ec_queue_##NAME##_t *, void **);                                                                                   \
        ec_status_t (*peek)(ec_queue_##NAME##_t *, void **);                                                                                      \
        size_t (*size_fn)(ec_queue_##NAME##_t *);                                                                                                 \
        ec_status_t (*free_fn)(ec_queue_##NAME##_t *);                                                                                            \
        void (*iter_init)(ec_queue_##NAME##_iter_t *, ec_queue_##NAME##_t *);                                                                     \
        ec_status_t (*iter_next)(ec_queue_##NAME##_iter_t *, void **);                                                                            \
        ec_status_t (*iter_replace)(ec_queue_##NAME##_iter_t *, void *, void **);                                                                 \
        void (*zip_iter_init)(ec_queue_##NAME##_zip_iter_t *, ec_queue_##NAME##_t *, ec_queue_##NAME##_t *);                                      \
        ec_status_t (*zip_iter_next)(ec_queue_##NAME##_zip_iter_t *, void **, void **);                                                           \
        ec_status_t (*zip_iter_replace)(ec_queue_##NAME##_zip_iter_t *, void *, void *, void **, void **);                                        \
    };                                                                                                                                            \
                                                                                                                                                  \
    struct ec_queue_##NAME##_iter_s                                                                                                               \
    {                                                                                                                                             \
        ec_queue_##NAME##_t *queue;                                                                                                               \
        ec_queue_##NAME##_node_t *current;                                                                                                        \
        ec_queue_##NAME##_node_t *prev;                                                                                                           \
        size_t idx;                                                                                                                               \
        bool init;                                                                                                                                \
    };                                                                                                                                            \
                                                                                                                                                  \
    struct ec_queue_##NAME##_zip_iter_s                                                                                                           \
    {                                                                                                                                             \
        ec_queue_##NAME##_t *q1;                                                                                                                  \
        ec_queue_##NAME##_t *q2;                                                                                                                  \
        ec_queue_##NAME##_node_t *c1;                                                                                                             \
        ec_queue_##NAME##_node_t *c2;                                                                                                             \
        size_t idx;                                                                                                                               \
    };                                                                                                                                            \
                                                                                                                                                  \
    static inline size_t ec_queue_##NAME##_struct_size(ec_queue_##NAME##_t *q)                                                                    \
    {                                                                                                                                             \
        (void)q;                                                                                                                                  \
        return sizeof(ec_queue_##NAME##_t);                                                                                                       \
    }                                                                                                                                             \
                                                                                                                                                  \
    static inline ec_queue_##NAME##_node_t *ec_queue_##NAME##_make_node(ec_queue_##NAME##_t *q, TYPE d)                                           \
    {                                                                                                                                             \
        ec_queue_##NAME##_node_t *n = EC_QUEUE_ALLOC(ec_queue_##NAME##_node_t, q->mempool);                                                       \
        if (n)                                                                                                                                    \
        {                                                                                                                                         \
            n->data = d;                                                                                                                          \
            n->next = NULL;                                                                                                                       \
        }                                                                                                                                         \
        return n;                                                                                                                                 \
    }                                                                                                                                             \
                                                                                                                                                  \
    static inline ec_status_t ec_queue_##NAME##_enqueue(ec_queue_##NAME##_t *q, void *elem)                                                       \
    {                                                                                                                                             \
        if (!q)                                                                                                                                   \
            return EC_ERR_INVALID_ARG;                                                                                                            \
        EC_MUTEX_LOCK(q->lock);                                                                                                                   \
        ec_queue_##NAME##_node_t *n = ec_queue_##NAME##_make_node(q, *(TYPE *)elem);                                                              \
        if (!n)                                                                                                                                   \
        {                                                                                                                                         \
            EC_MUTEX_UNLOCK(q->lock);                                                                                                             \
            return EC_ERR_ALLOC;                                                                                                                  \
        }                                                                                                                                         \
        if (!q->rear)                                                                                                                             \
        {                                                                                                                                         \
            q->front = q->rear = n;                                                                                                               \
        }                                                                                                                                         \
        else                                                                                                                                      \
        {                                                                                                                                         \
            q->rear->next = n;                                                                                                                    \
            q->rear = n;                                                                                                                          \
        }                                                                                                                                         \
        q->size++;                                                                                                                                \
        EC_MUTEX_UNLOCK(q->lock);                                                                                                                 \
        return EC_OK;                                                                                                                             \
    }                                                                                                                                             \
    static inline ec_status_t ec_queue_##NAME##_dequeue(ec_queue_##NAME##_t *q, void **out)                                                       \
    {                                                                                                                                             \
        if (!q || !out)                                                                                                                           \
            return EC_ERR_INVALID_ARG;                                                                                                            \
        EC_MUTEX_LOCK(q->lock);                                                                                                                   \
        if (!q->front)                                                                                                                            \
        {                                                                                                                                         \
            EC_MUTEX_UNLOCK(q->lock);                                                                                                             \
            return EC_ERR_OUT_OF_BOUNDS;                                                                                                          \
        }                                                                                                                                         \
        ec_queue_##NAME##_node_t *temp = q->front;                                                                                                \
        if (out)                                                                                                                                  \
        {                                                                                                                                         \
            EC_MEMCPY_SAFE(*out, &temp->data, sizeof(temp->data));                                                                                \
        }                                                                                                                                         \
        q->front = q->front->next;                                                                                                                \
        if (!q->front)                                                                                                                            \
            q->rear = NULL;                                                                                                                       \
        q->size--;                                                                                                                                \
        EC_FREE(temp);                                                                                                                            \
        EC_MUTEX_UNLOCK(q->lock);                                                                                                                 \
        return EC_OK;                                                                                                                             \
    }                                                                                                                                             \
                                                                                                                                                  \
    static inline ec_status_t ec_queue_##NAME##_peek(ec_queue_##NAME##_t *q, void **out)                                                          \
    {                                                                                                                                             \
        if (!q || !out)                                                                                                                           \
            return EC_ERR_INVALID_ARG;                                                                                                            \
        EC_MUTEX_LOCK(q->lock);                                                                                                                   \
        if (!q->front)                                                                                                                            \
        {                                                                                                                                         \
            EC_MUTEX_UNLOCK(q->lock);                                                                                                             \
            return EC_ERR_OUT_OF_BOUNDS;                                                                                                          \
        }                                                                                                                                         \
        if (out)                                                                                                                                  \
        {                                                                                                                                         \
            EC_MEMCPY_SAFE(*out, &q->front->data, sizeof(q->front->data));                                                                        \
        }                                                                                                                                         \
        EC_MUTEX_UNLOCK(q->lock);                                                                                                                 \
        return EC_OK;                                                                                                                             \
    }                                                                                                                                             \
                                                                                                                                                  \
    static inline size_t ec_queue_##NAME##_size_fn(ec_queue_##NAME##_t *q)                                                                        \
    {                                                                                                                                             \
        if (!q)                                                                                                                                   \
            return 0;                                                                                                                             \
        EC_MUTEX_LOCK(q->lock);                                                                                                                   \
        size_t size = q->size;                                                                                                                    \
        EC_MUTEX_UNLOCK(q->lock);                                                                                                                 \
        return size;                                                                                                                              \
    }                                                                                                                                             \
    static inline ec_status_t ec_queue_##NAME##_free_fn(ec_queue_##NAME##_t *q)                                                                   \
    {                                                                                                                                             \
        if (!q)                                                                                                                                   \
            return EC_ERR_INVALID_ARG;                                                                                                            \
        EC_MUTEX_LOCK(q->lock);                                                                                                                   \
        ec_queue_##NAME##_node_t *current = q->front;                                                                                             \
        while (current)                                                                                                                           \
        {                                                                                                                                         \
            ec_queue_##NAME##_node_t *temp = current;                                                                                             \
            current = current->next;                                                                                                              \
            EC_FREE(temp);                                                                                                                        \
        }                                                                                                                                         \
        q->front = q->rear = NULL;                                                                                                                \
        q->size = 0;                                                                                                                              \
        EC_MUTEX_UNLOCK(q->lock);                                                                                                                 \
        EC_MUTEX_DESTROY(q->lock);                                                                                                                \
        return EC_OK;                                                                                                                             \
    }                                                                                                                                             \
                                                                                                                                                  \
    static inline void ec_queue_##NAME##_iter_init(ec_queue_##NAME##_iter_t *it, ec_queue_##NAME##_t *q)                                          \
    {                                                                                                                                             \
        it->queue = q;                                                                                                                            \
        it->current = q->front;                                                                                                                   \
        it->prev = NULL;                                                                                                                          \
        it->idx = 0;                                                                                                                              \
        it->init = true;                                                                                                                          \
    }                                                                                                                                             \
                                                                                                                                                  \
    static inline ec_status_t ec_queue_##NAME##_iter_next(ec_queue_##NAME##_iter_t *it, void **out)                                               \
    {                                                                                                                                             \
        if (!it || !out)                                                                                                                          \
            return EC_ERR_INVALID_ARG;                                                                                                            \
        EC_MUTEX_LOCK(it->queue->lock);                                                                                                           \
        if (!it->current)                                                                                                                         \
        {                                                                                                                                         \
            EC_MUTEX_UNLOCK(it->queue->lock);                                                                                                     \
            return EC_ITER_END;                                                                                                                   \
        }                                                                                                                                         \
        if (out)                                                                                                                                  \
        {                                                                                                                                         \
            EC_MEMCPY_SAFE(*out, &it->current->data, sizeof(it->current->data));                                                                  \
        }                                                                                                                                         \
        it->prev = it->current;                                                                                                                   \
        it->current = it->current->next;                                                                                                          \
        it->idx++;                                                                                                                                \
        EC_MUTEX_UNLOCK(it->queue->lock);                                                                                                         \
        return EC_OK;                                                                                                                             \
    }                                                                                                                                             \
                                                                                                                                                  \
    static inline ec_status_t ec_queue_##NAME##_iter_replace(ec_queue_##NAME##_iter_t *it, void *elem, void **out)                                \
    {                                                                                                                                             \
        if (!it || !it->prev)                                                                                                                     \
            return EC_ERR_INVALID_ARG;                                                                                                            \
        EC_MUTEX_LOCK(it->queue->lock);                                                                                                           \
        if (out)                                                                                                                                  \
        {                                                                                                                                         \
            EC_MEMCPY_SAFE(*out, &it->prev->data, sizeof(it->prev->data));                                                                        \
        }                                                                                                                                         \
        it->prev->data = *(TYPE *)elem;                                                                                                           \
        EC_MUTEX_UNLOCK(it->queue->lock);                                                                                                         \
        return EC_OK;                                                                                                                             \
    }                                                                                                                                             \
                                                                                                                                                  \
    static inline void ec_queue_##NAME##_zip_iter_init(ec_queue_##NAME##_zip_iter_t *zit, ec_queue_##NAME##_t *q1, ec_queue_##NAME##_t *q2)       \
    {                                                                                                                                             \
        zit->q1 = q1;                                                                                                                             \
        zit->q2 = q2;                                                                                                                             \
        zit->c1 = q1->front;                                                                                                                      \
        zit->c2 = q2->front;                                                                                                                      \
        zit->idx = 0;                                                                                                                             \
    }                                                                                                                                             \
                                                                                                                                                  \
    static inline ec_status_t ec_queue_##NAME##_zip_iter_next(ec_queue_##NAME##_zip_iter_t *zit, void **out1, void **out2)                        \
    {                                                                                                                                             \
        if (!zit || !out1 || !out2)                                                                                                               \
            return EC_ERR_INVALID_ARG;                                                                                                            \
        EC_MUTEX_LOCK(zit->q1->lock);                                                                                                             \
        EC_MUTEX_LOCK(zit->q2->lock);                                                                                                             \
        if (!zit->c1 || !zit->c2)                                                                                                                 \
        {                                                                                                                                         \
            EC_MUTEX_UNLOCK(zit->q2->lock);                                                                                                       \
            EC_MUTEX_UNLOCK(zit->q1->lock);                                                                                                       \
            return EC_ITER_END;                                                                                                                   \
        }                                                                                                                                         \
        if (out1 && out2)                                                                                                                         \
        {                                                                                                                                         \
            EC_MEMCPY_SAFE(*out1, &zit->c1->data, sizeof(zit->c1->data));                                                                         \
            EC_MEMCPY_SAFE(*out2, &zit->c2->data, sizeof(zit->c2->data));                                                                         \
        }                                                                                                                                         \
        zit->c1 = zit->c1->next;                                                                                                                  \
        zit->c2 = zit->c2->next;                                                                                                                  \
        zit->idx++;                                                                                                                               \
        EC_MUTEX_UNLOCK(zit->q2->lock);                                                                                                           \
        EC_MUTEX_UNLOCK(zit->q1->lock);                                                                                                           \
        return EC_OK;                                                                                                                             \
    }                                                                                                                                             \
                                                                                                                                                  \
    static inline ec_status_t ec_queue_##NAME##_zip_iter_replace(ec_queue_##NAME##_zip_iter_t *zit, void *e1, void *e2, void **out1, void **out2) \
    {                                                                                                                                             \
        return EC_ERR_NOT_SUPPORTED;                                                                                                              \
    }                                                                                                                                             \
    static inline ec_status_t ec_queue_##NAME##_init(ec_queue_##NAME##_t *q, void *mempool)                                                       \
    {                                                                                                                                             \
        if (!q)                                                                                                                                   \
            return EC_ERR_INVALID_ARG;                                                                                                            \
        EC_MUTEX_INIT(q->lock);                                                                                                                   \
        q->front = NULL;                                                                                                                          \
        q->rear = NULL;                                                                                                                           \
        q->size = 0;                                                                                                                              \
        q->mempool = mempool;                                                                                                                     \
        q->struct_size = ec_queue_##NAME##_struct_size;                                                                                           \
        q->enqueue = ec_queue_##NAME##_enqueue;                                                                                                   \
        q->dequeue = ec_queue_##NAME##_dequeue;                                                                                                   \
        q->peek = ec_queue_##NAME##_peek;                                                                                                         \
        q->size_fn = ec_queue_##NAME##_size_fn;                                                                                                   \
        q->iter_init = ec_queue_##NAME##_iter_init;                                                                                               \
        q->iter_next = ec_queue_##NAME##_iter_next;                                                                                               \
        q->iter_replace = ec_queue_##NAME##_iter_replace;                                                                                         \
        q->zip_iter_init = ec_queue_##NAME##_zip_iter_init;                                                                                       \
        q->zip_iter_next = ec_queue_##NAME##_zip_iter_next;                                                                                       \
        q->zip_iter_replace = ec_queue_##NAME##_zip_iter_replace;                                                                                 \
        q->free_fn = ec_queue_##NAME##_free_fn;                                                                                                   \
        return EC_OK;                                                                                                                             \
    }

EC_QUEUE_DEF(int, int)
EC_QUEUE_DEF(unsigned int, uint)
EC_QUEUE_DEF(long, long)
EC_QUEUE_DEF(long long, ll)
EC_QUEUE_DEF(unsigned long, ul)
EC_QUEUE_DEF(unsigned long long, ull)
EC_QUEUE_DEF(uint32_t, u32)
EC_QUEUE_DEF(uint64_t, u64)
EC_QUEUE_DEF(double, dbl)
EC_QUEUE_DEF(const char *, str)
EC_QUEUE_DEF(void *, ptr)

#endif
