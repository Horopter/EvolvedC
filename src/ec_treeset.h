#ifndef EC_TREESET_H
#define EC_TREESET_H
#include "ec_common.h"
#include "ec_status.h"
#ifdef EC_USE_MEMPOOL
#define EC_TREESET_ALLOC(T, MPOOL) ((MPOOL) ? (T *)ec_mempool_alloc((ec_mempool_t *)(MPOOL), sizeof(T)) : (T *)EC_MALLOC(sizeof(T)))
#else
#define EC_TREESET_ALLOC(T, MPOOL) (T *)EC_MALLOC(sizeof(T))
#endif

#define EC_TREESET_DEF(TYPE, NAME)                                                                                                                                              \
    typedef struct ec_treeset_##NAME##_entry_s                                                                                                                                  \
    {                                                                                                                                                                           \
        TYPE key;                                                                                                                                                               \
        struct ec_treeset_##NAME##_entry_s *left, *right, *parent;                                                                                                              \
        int h;                                                                                                                                                                  \
    } ec_treeset_##NAME##_entry_t;                                                                                                                                              \
    typedef struct ec_treeset_##NAME##_iter_s                                                                                                                                   \
    {                                                                                                                                                                           \
        struct ec_treeset_##NAME##_s *m;                                                                                                                                        \
        ec_treeset_##NAME##_entry_t *curr;                                                                                                                                      \
    } ec_treeset_##NAME##_iter_t;                                                                                                                                               \
    typedef struct ec_treeset_##NAME##_s                                                                                                                                        \
    {                                                                                                                                                                           \
        ec_treeset_##NAME##_entry_t *root;                                                                                                                                      \
        size_t size;                                                                                                                                                            \
        EC_MUTEX_TYPE lock;                                                                                                                                                     \
        void *mempool;                                                                                                                                                          \
        int (*compare)(const TYPE *, const TYPE *);                                                                                                                             \
        ec_status_t (*add)(struct ec_treeset_##NAME##_s *, const TYPE *);                                                                                                       \
        ec_status_t (*remove)(struct ec_treeset_##NAME##_s *, const TYPE *);                                                                                                    \
        ec_status_t (*remove_all)(struct ec_treeset_##NAME##_s *);                                                                                                              \
        ec_status_t (*remove_first)(struct ec_treeset_##NAME##_s *);                                                                                                            \
        ec_status_t (*remove_last)(struct ec_treeset_##NAME##_s *);                                                                                                             \
        ec_status_t (*get_first)(struct ec_treeset_##NAME##_s *, TYPE *);                                                                                                       \
        ec_status_t (*get_last)(struct ec_treeset_##NAME##_s *, TYPE *);                                                                                                        \
        ec_status_t (*get_greater_than)(struct ec_treeset_##NAME##_s *, const TYPE *, TYPE *);                                                                                  \
        ec_status_t (*get_lesser_than)(struct ec_treeset_##NAME##_s *, const TYPE *, TYPE *);                                                                                   \
        size_t (*size_fn)(struct ec_treeset_##NAME##_s *);                                                                                                                      \
        ec_status_t (*contains)(struct ec_treeset_##NAME##_s *, const TYPE *);                                                                                                  \
        ec_status_t (*foreach)(struct ec_treeset_##NAME##_s *, void (*)(TYPE));                                                                                                 \
        ec_status_t (*iter_init)(struct ec_treeset_##NAME##_s *, ec_treeset_##NAME##_iter_t *);                                                                                 \
        ec_status_t (*iter_next)(ec_treeset_##NAME##_iter_t *, TYPE *);                                                                                                         \
        ec_status_t (*iter_remove)(ec_treeset_##NAME##_iter_t *);                                                                                                               \
    } ec_treeset_##NAME##_t;                                                                                                                                                    \
    static inline int ec_treeset_##NAME##_dfcmp(const TYPE *a, const TYPE *b) { return (*a < *b) ? -1 : ((*a > *b) ? 1 : 0); }                                                  \
    static inline int ec_treeset_##NAME##_h(ec_treeset_##NAME##_entry_t *n) { return n ? n->h : 0; }                                                                            \
    static inline void ec_treeset_##NAME##_uh(ec_treeset_##NAME##_entry_t *n)                                                                                                   \
    {                                                                                                                                                                           \
        int l = ec_treeset_##NAME##_h(n->left), r = ec_treeset_##NAME##_h(n->right);                                                                                            \
        n->h = ((l > r) ? l : r) + 1;                                                                                                                                           \
    }                                                                                                                                                                           \
    static inline ec_treeset_##NAME##_entry_t *ec_treeset_##NAME##_rr(ec_treeset_##NAME##_entry_t *y)                                                                           \
    {                                                                                                                                                                           \
        ec_treeset_##NAME##_entry_t *x = y->left;                                                                                                                               \
        y->left = x->right;                                                                                                                                                     \
        if (x->right)                                                                                                                                                           \
            x->right->parent = y;                                                                                                                                               \
        x->right = y;                                                                                                                                                           \
        x->parent = y->parent;                                                                                                                                                  \
        y->parent = x;                                                                                                                                                          \
        ec_treeset_##NAME##_uh(y);                                                                                                                                              \
        ec_treeset_##NAME##_uh(x);                                                                                                                                              \
        return x;                                                                                                                                                               \
    }                                                                                                                                                                           \
    static inline ec_treeset_##NAME##_entry_t *ec_treeset_##NAME##_rl(ec_treeset_##NAME##_entry_t *x)                                                                           \
    {                                                                                                                                                                           \
        ec_treeset_##NAME##_entry_t *y = x->right;                                                                                                                              \
        x->right = y->left;                                                                                                                                                     \
        if (y->left)                                                                                                                                                            \
            y->left->parent = x;                                                                                                                                                \
        y->left = x;                                                                                                                                                            \
        y->parent = x->parent;                                                                                                                                                  \
        x->parent = y;                                                                                                                                                          \
        ec_treeset_##NAME##_uh(x);                                                                                                                                              \
        ec_treeset_##NAME##_uh(y);                                                                                                                                              \
        return y;                                                                                                                                                               \
    }                                                                                                                                                                           \
    static inline ec_treeset_##NAME##_entry_t *ec_treeset_##NAME##_reb(ec_treeset_##NAME##_entry_t *n)                                                                          \
    {                                                                                                                                                                           \
        ec_treeset_##NAME##_uh(n);                                                                                                                                              \
        int b = ec_treeset_##NAME##_h(n->left) - ec_treeset_##NAME##_h(n->right);                                                                                               \
        if (b > 1)                                                                                                                                                              \
        {                                                                                                                                                                       \
            if (ec_treeset_##NAME##_h(n->left->left) < ec_treeset_##NAME##_h(n->left->right))                                                                                   \
                n->left = ec_treeset_##NAME##_rl(n->left);                                                                                                                      \
            return ec_treeset_##NAME##_rr(n);                                                                                                                                   \
        }                                                                                                                                                                       \
        else if (b < -1)                                                                                                                                                        \
        {                                                                                                                                                                       \
            if (ec_treeset_##NAME##_h(n->right->right) < ec_treeset_##NAME##_h(n->right->left))                                                                                 \
                n->right = ec_treeset_##NAME##_rr(n->right);                                                                                                                    \
            return ec_treeset_##NAME##_rl(n);                                                                                                                                   \
        }                                                                                                                                                                       \
        return n;                                                                                                                                                               \
    }                                                                                                                                                                           \
    static inline ec_treeset_##NAME##_entry_t *ec_treeset_##NAME##_mn(ec_treeset_##NAME##_entry_t *n)                                                                           \
    {                                                                                                                                                                           \
        while (n && n->left)                                                                                                                                                    \
            n = n->left;                                                                                                                                                        \
        return n;                                                                                                                                                               \
    }                                                                                                                                                                           \
    static inline ec_treeset_##NAME##_entry_t *ec_treeset_##NAME##_mx(ec_treeset_##NAME##_entry_t *n)                                                                           \
    {                                                                                                                                                                           \
        while (n && n->right)                                                                                                                                                   \
            n = n->right;                                                                                                                                                       \
        return n;                                                                                                                                                               \
    }                                                                                                                                                                           \
    static inline ec_treeset_##NAME##_entry_t *ec_treeset_##NAME##_ins(ec_treeset_##NAME##_t *m, ec_treeset_##NAME##_entry_t *r, ec_treeset_##NAME##_entry_t *p, const TYPE *k) \
    {                                                                                                                                                                           \
        if (!r)                                                                                                                                                                 \
        {                                                                                                                                                                       \
            r = EC_TREESET_ALLOC(ec_treeset_##NAME##_entry_t, m->mempool);                                                                                                      \
            if (!r)                                                                                                                                                             \
                return NULL;                                                                                                                                                    \
            r->key = (TYPE)(*k);                                                                                                                                                \
            r->left = r->right = NULL;                                                                                                                                          \
            r->parent = p;                                                                                                                                                      \
            r->h = 1;                                                                                                                                                           \
            m->size++;                                                                                                                                                          \
            return r;                                                                                                                                                           \
        }                                                                                                                                                                       \
        int c = m->compare(k, &r->key);                                                                                                                                         \
        if (c < 0)                                                                                                                                                              \
            r->left = ec_treeset_##NAME##_ins(m, r->left, r, k);                                                                                                                \
        else if (c > 0)                                                                                                                                                         \
            r->right = ec_treeset_##NAME##_ins(m, r->right, r, k);                                                                                                              \
        return ec_treeset_##NAME##_reb(r);                                                                                                                                      \
    }                                                                                                                                                                           \
    static inline ec_treeset_##NAME##_entry_t *ec_treeset_##NAME##_rm(ec_treeset_##NAME##_t *m, ec_treeset_##NAME##_entry_t *r, const TYPE *k)                                  \
    {                                                                                                                                                                           \
        if (!r)                                                                                                                                                                 \
            return NULL;                                                                                                                                                        \
        int c = m->compare(k, &r->key);                                                                                                                                         \
        if (c < 0)                                                                                                                                                              \
            r->left = ec_treeset_##NAME##_rm(m, r->left, k);                                                                                                                    \
        else if (c > 0)                                                                                                                                                         \
            r->right = ec_treeset_##NAME##_rm(m, r->right, k);                                                                                                                  \
        else                                                                                                                                                                    \
        {                                                                                                                                                                       \
            if (!r->left)                                                                                                                                                       \
            {                                                                                                                                                                   \
                ec_treeset_##NAME##_entry_t *nr = r->right;                                                                                                                     \
                EC_FREE(r);                                                                                                                                                     \
                m->size--;                                                                                                                                                      \
                return nr;                                                                                                                                                      \
            }                                                                                                                                                                   \
            else if (!r->right)                                                                                                                                                 \
            {                                                                                                                                                                   \
                ec_treeset_##NAME##_entry_t *nl = r->left;                                                                                                                      \
                EC_FREE(r);                                                                                                                                                     \
                m->size--;                                                                                                                                                      \
                return nl;                                                                                                                                                      \
            }                                                                                                                                                                   \
            else                                                                                                                                                                \
            {                                                                                                                                                                   \
                ec_treeset_##NAME##_entry_t *mn = ec_treeset_##NAME##_mn(r->right);                                                                                             \
                r->key = mn->key;                                                                                                                                               \
                r->right = ec_treeset_##NAME##_rm(m, r->right, &mn->key);                                                                                                       \
            }                                                                                                                                                                   \
        }                                                                                                                                                                       \
        return ec_treeset_##NAME##_reb(r);                                                                                                                                      \
    }                                                                                                                                                                           \
    static inline ec_treeset_##NAME##_entry_t *ec_treeset_##NAME##_rmn(ec_treeset_##NAME##_t *m, ec_treeset_##NAME##_entry_t *r)                                                \
    {                                                                                                                                                                           \
        if (!r)                                                                                                                                                                 \
            return NULL;                                                                                                                                                        \
        if (!r->left)                                                                                                                                                           \
        {                                                                                                                                                                       \
            ec_treeset_##NAME##_entry_t *x = r->right;                                                                                                                          \
            EC_FREE(r);                                                                                                                                                         \
            m->size--;                                                                                                                                                          \
            return x;                                                                                                                                                           \
        }                                                                                                                                                                       \
        r->left = ec_treeset_##NAME##_rmn(m, r->left);                                                                                                                          \
        return ec_treeset_##NAME##_reb(r);                                                                                                                                      \
    }                                                                                                                                                                           \
    static inline ec_treeset_##NAME##_entry_t *ec_treeset_##NAME##_rmx(ec_treeset_##NAME##_t *m, ec_treeset_##NAME##_entry_t *r)                                                \
    {                                                                                                                                                                           \
        if (!r)                                                                                                                                                                 \
            return NULL;                                                                                                                                                        \
        if (!r->right)                                                                                                                                                          \
        {                                                                                                                                                                       \
            ec_treeset_##NAME##_entry_t *x = r->left;                                                                                                                           \
            EC_FREE(r);                                                                                                                                                         \
            m->size--;                                                                                                                                                          \
            return x;                                                                                                                                                           \
        }                                                                                                                                                                       \
        r->right = ec_treeset_##NAME##_rmx(m, r->right);                                                                                                                        \
        return ec_treeset_##NAME##_reb(r);                                                                                                                                      \
    }                                                                                                                                                                           \
    static inline ec_treeset_##NAME##_entry_t *ec_treeset_##NAME##_suc(ec_treeset_##NAME##_entry_t *n)                                                                          \
    {                                                                                                                                                                           \
        if (n->right)                                                                                                                                                           \
        {                                                                                                                                                                       \
            n = n->right;                                                                                                                                                       \
            while (n->left)                                                                                                                                                     \
                n = n->left;                                                                                                                                                    \
            return n;                                                                                                                                                           \
        }                                                                                                                                                                       \
        ec_treeset_##NAME##_entry_t *p = n->parent;                                                                                                                             \
        while (p && n == p->right)                                                                                                                                              \
        {                                                                                                                                                                       \
            n = p;                                                                                                                                                              \
            p = p->parent;                                                                                                                                                      \
        }                                                                                                                                                                       \
        return p;                                                                                                                                                               \
    }                                                                                                                                                                           \
    static inline ec_status_t ec_treeset_##NAME##_add_f(struct ec_treeset_##NAME##_s *m, const TYPE *k)                                                                         \
    {                                                                                                                                                                           \
        if (!m || !k)                                                                                                                                                           \
            return EC_ERR_INVALID_ARG;                                                                                                                                          \
        EC_MUTEX_LOCK(m->lock);                                                                                                                                                 \
        m->root = ec_treeset_##NAME##_ins(m, m->root, NULL, k);                                                                                                                 \
        EC_MUTEX_UNLOCK(m->lock);                                                                                                                                               \
        return m->root ? EC_OK : EC_ERR_ALLOC;                                                                                                                                  \
    }                                                                                                                                                                           \
    static inline ec_status_t ec_treeset_##NAME##_remove_f(struct ec_treeset_##NAME##_s *m, const TYPE *k)                                                                      \
    {                                                                                                                                                                           \
        if (!m || !k)                                                                                                                                                           \
            return EC_ERR_INVALID_ARG;                                                                                                                                          \
        EC_MUTEX_LOCK(m->lock);                                                                                                                                                 \
        size_t os = m->size;                                                                                                                                                    \
        m->root = ec_treeset_##NAME##_rm(m, m->root, k);                                                                                                                        \
        EC_MUTEX_UNLOCK(m->lock);                                                                                                                                               \
        return (m->size < os) ? EC_OK : EC_ERR_NOT_FOUND;                                                                                                                       \
    }                                                                                                                                                                           \
    static inline ec_status_t ec_treeset_##NAME##_remove_all_f(struct ec_treeset_##NAME##_s *m)                                                                                 \
    {                                                                                                                                                                           \
        if (!m)                                                                                                                                                                 \
            return EC_ERR_INVALID_ARG;                                                                                                                                          \
        EC_MUTEX_LOCK(m->lock);                                                                                                                                                 \
        while (m->root)                                                                                                                                                         \
            m->root = ec_treeset_##NAME##_rmn(m, m->root);                                                                                                                      \
        EC_MUTEX_UNLOCK(m->lock);                                                                                                                                               \
        return EC_OK;                                                                                                                                                           \
    }                                                                                                                                                                           \
    static inline ec_status_t ec_treeset_##NAME##_remove_first_f(struct ec_treeset_##NAME##_s *m)                                                                               \
    {                                                                                                                                                                           \
        if (!m)                                                                                                                                                                 \
            return EC_ERR_INVALID_ARG;                                                                                                                                          \
        EC_MUTEX_LOCK(m->lock);                                                                                                                                                 \
        if (!m->root)                                                                                                                                                           \
        {                                                                                                                                                                       \
            EC_MUTEX_UNLOCK(m->lock);                                                                                                                                           \
            return EC_ERR_NOT_FOUND;                                                                                                                                            \
        }                                                                                                                                                                       \
        m->root = ec_treeset_##NAME##_rmn(m, m->root);                                                                                                                          \
        EC_MUTEX_UNLOCK(m->lock);                                                                                                                                               \
        return EC_OK;                                                                                                                                                           \
    }                                                                                                                                                                           \
    static inline ec_status_t ec_treeset_##NAME##_remove_last_f(struct ec_treeset_##NAME##_s *m)                                                                                \
    {                                                                                                                                                                           \
        if (!m)                                                                                                                                                                 \
            return EC_ERR_INVALID_ARG;                                                                                                                                          \
        EC_MUTEX_LOCK(m->lock);                                                                                                                                                 \
        if (!m->root)                                                                                                                                                           \
        {                                                                                                                                                                       \
            EC_MUTEX_UNLOCK(m->lock);                                                                                                                                           \
            return EC_ERR_NOT_FOUND;                                                                                                                                            \
        }                                                                                                                                                                       \
        m->root = ec_treeset_##NAME##_rmx(m, m->root);                                                                                                                          \
        EC_MUTEX_UNLOCK(m->lock);                                                                                                                                               \
        return EC_OK;                                                                                                                                                           \
    }                                                                                                                                                                           \
    static inline ec_status_t ec_treeset_##NAME##_get_first_f(struct ec_treeset_##NAME##_s *m, TYPE *k)                                                                         \
    {                                                                                                                                                                           \
        if (!m || !k)                                                                                                                                                           \
            return EC_ERR_INVALID_ARG;                                                                                                                                          \
        EC_MUTEX_LOCK(m->lock);                                                                                                                                                 \
        ec_treeset_##NAME##_entry_t *n = ec_treeset_##NAME##_mn(m->root);                                                                                                       \
        if (!n)                                                                                                                                                                 \
        {                                                                                                                                                                       \
            EC_MUTEX_UNLOCK(m->lock);                                                                                                                                           \
            return EC_ERR_NOT_FOUND;                                                                                                                                            \
        }                                                                                                                                                                       \
        *k = n->key;                                                                                                                                                            \
        EC_MUTEX_UNLOCK(m->lock);                                                                                                                                               \
        return EC_OK;                                                                                                                                                           \
    }                                                                                                                                                                           \
    static inline ec_status_t ec_treeset_##NAME##_get_last_f(struct ec_treeset_##NAME##_s *m, TYPE *k)                                                                          \
    {                                                                                                                                                                           \
        if (!m || !k)                                                                                                                                                           \
            return EC_ERR_INVALID_ARG;                                                                                                                                          \
        EC_MUTEX_LOCK(m->lock);                                                                                                                                                 \
        ec_treeset_##NAME##_entry_t *n = ec_treeset_##NAME##_mx(m->root);                                                                                                       \
        if (!n)                                                                                                                                                                 \
        {                                                                                                                                                                       \
            EC_MUTEX_UNLOCK(m->lock);                                                                                                                                           \
            return EC_ERR_NOT_FOUND;                                                                                                                                            \
        }                                                                                                                                                                       \
        *k = n->key;                                                                                                                                                            \
        EC_MUTEX_UNLOCK(m->lock);                                                                                                                                               \
        return EC_OK;                                                                                                                                                           \
    }                                                                                                                                                                           \
    static inline ec_status_t ec_treeset_##NAME##_get_greater_than_f(struct ec_treeset_##NAME##_s *m, const TYPE *k, TYPE *ok)                                                  \
    {                                                                                                                                                                           \
        if (!m || !k || !ok)                                                                                                                                                    \
            return EC_ERR_INVALID_ARG;                                                                                                                                          \
        EC_MUTEX_LOCK(m->lock);                                                                                                                                                 \
        ec_treeset_##NAME##_entry_t *c = m->root, *can = NULL;                                                                                                                  \
        while (c)                                                                                                                                                               \
        {                                                                                                                                                                       \
            int cmp = m->compare(k, &c->key);                                                                                                                                   \
            if (cmp < 0)                                                                                                                                                        \
            {                                                                                                                                                                   \
                can = c;                                                                                                                                                        \
                c = c->left;                                                                                                                                                    \
            }                                                                                                                                                                   \
            else                                                                                                                                                                \
                c = c->right;                                                                                                                                                   \
        }                                                                                                                                                                       \
        if (!can)                                                                                                                                                               \
        {                                                                                                                                                                       \
            EC_MUTEX_UNLOCK(m->lock);                                                                                                                                           \
            return EC_ERR_NOT_FOUND;                                                                                                                                            \
        }                                                                                                                                                                       \
        *ok = can->key;                                                                                                                                                         \
        EC_MUTEX_UNLOCK(m->lock);                                                                                                                                               \
        return EC_OK;                                                                                                                                                           \
    }                                                                                                                                                                           \
    static inline ec_status_t ec_treeset_##NAME##_get_lesser_than_f(struct ec_treeset_##NAME##_s *m, const TYPE *k, TYPE *ok)                                                   \
    {                                                                                                                                                                           \
        if (!m || !k || !ok)                                                                                                                                                    \
            return EC_ERR_INVALID_ARG;                                                                                                                                          \
        EC_MUTEX_LOCK(m->lock);                                                                                                                                                 \
        ec_treeset_##NAME##_entry_t *c = m->root, *can = NULL;                                                                                                                  \
        while (c)                                                                                                                                                               \
        {                                                                                                                                                                       \
            int cmp = m->compare(k, &c->key);                                                                                                                                   \
            if (cmp > 0)                                                                                                                                                        \
            {                                                                                                                                                                   \
                can = c;                                                                                                                                                        \
                c = c->right;                                                                                                                                                   \
            }                                                                                                                                                                   \
            else                                                                                                                                                                \
                c = c->left;                                                                                                                                                    \
        }                                                                                                                                                                       \
        if (!can)                                                                                                                                                               \
        {                                                                                                                                                                       \
            EC_MUTEX_UNLOCK(m->lock);                                                                                                                                           \
            return EC_ERR_NOT_FOUND;                                                                                                                                            \
        }                                                                                                                                                                       \
        *ok = can->key;                                                                                                                                                         \
        EC_MUTEX_UNLOCK(m->lock);                                                                                                                                               \
        return EC_OK;                                                                                                                                                           \
    }                                                                                                                                                                           \
    static inline size_t ec_treeset_##NAME##_size_f(struct ec_treeset_##NAME##_s *m)                                                                                            \
    {                                                                                                                                                                           \
        if (!m)                                                                                                                                                                 \
            return 0;                                                                                                                                                           \
        EC_MUTEX_LOCK(m->lock);                                                                                                                                                 \
        size_t s = m->size;                                                                                                                                                     \
        EC_MUTEX_UNLOCK(m->lock);                                                                                                                                               \
        return s;                                                                                                                                                               \
    }                                                                                                                                                                           \
    static inline ec_status_t ec_treeset_##NAME##_contains_f(struct ec_treeset_##NAME##_s *m, const TYPE *k)                                                                    \
    {                                                                                                                                                                           \
        if (!m || !k)                                                                                                                                                           \
            return EC_ERR_INVALID_ARG;                                                                                                                                          \
        EC_MUTEX_LOCK(m->lock);                                                                                                                                                 \
        ec_treeset_##NAME##_entry_t *n = m->root;                                                                                                                               \
        while (n)                                                                                                                                                               \
        {                                                                                                                                                                       \
            int c = m->compare(k, &n->key);                                                                                                                                     \
            if (!c)                                                                                                                                                             \
            {                                                                                                                                                                   \
                EC_MUTEX_UNLOCK(m->lock);                                                                                                                                       \
                return EC_OK;                                                                                                                                                   \
            }                                                                                                                                                                   \
            n = (c < 0) ? n->left : n->right;                                                                                                                                   \
        }                                                                                                                                                                       \
        EC_MUTEX_UNLOCK(m->lock);                                                                                                                                               \
        return EC_ERR_NOT_FOUND;                                                                                                                                                \
    }                                                                                                                                                                           \
    static inline ec_status_t ec_treeset_##NAME##_foreach_f(struct ec_treeset_##NAME##_s *m, void (*fn)(TYPE))                                                                  \
    {                                                                                                                                                                           \
        if (!m || !fn)                                                                                                                                                          \
            return EC_ERR_INVALID_ARG;                                                                                                                                          \
        EC_MUTEX_LOCK(m->lock);                                                                                                                                                 \
        ec_treeset_##NAME##_entry_t *cur = m->root;                                                                                                                             \
        while (cur)                                                                                                                                                             \
        {                                                                                                                                                                       \
            if (!cur->left)                                                                                                                                                     \
            {                                                                                                                                                                   \
                fn(cur->key);                                                                                                                                                   \
                cur = cur->right;                                                                                                                                               \
            }                                                                                                                                                                   \
            else                                                                                                                                                                \
            {                                                                                                                                                                   \
                ec_treeset_##NAME##_entry_t *pre = cur->left;                                                                                                                   \
                while (pre->right && pre->right != cur)                                                                                                                         \
                    pre = pre->right;                                                                                                                                           \
                if (!pre->right)                                                                                                                                                \
                {                                                                                                                                                               \
                    pre->right = cur;                                                                                                                                           \
                    cur = cur->left;                                                                                                                                            \
                }                                                                                                                                                               \
                else                                                                                                                                                            \
                {                                                                                                                                                               \
                    pre->right = NULL;                                                                                                                                          \
                    fn(cur->key);                                                                                                                                               \
                    cur = cur->right;                                                                                                                                           \
                }                                                                                                                                                               \
            }                                                                                                                                                                   \
        }                                                                                                                                                                       \
        EC_MUTEX_UNLOCK(m->lock);                                                                                                                                               \
        return EC_OK;                                                                                                                                                           \
    }                                                                                                                                                                           \
    static inline ec_status_t ec_treeset_##NAME##_iter_init_f(struct ec_treeset_##NAME##_s *m, ec_treeset_##NAME##_iter_t *it)                                                  \
    {                                                                                                                                                                           \
        if (!m || !it)                                                                                                                                                          \
            return EC_ERR_INVALID_ARG;                                                                                                                                          \
        it->m = m;                                                                                                                                                              \
        EC_MUTEX_LOCK(m->lock);                                                                                                                                                 \
        it->curr = ec_treeset_##NAME##_mn(m->root);                                                                                                                             \
        EC_MUTEX_UNLOCK(m->lock);                                                                                                                                               \
        return EC_OK;                                                                                                                                                           \
    }                                                                                                                                                                           \
    static inline ec_status_t ec_treeset_##NAME##_iter_next_f(ec_treeset_##NAME##_iter_t *it, TYPE *k)                                                                          \
    {                                                                                                                                                                           \
        if (!it || !k)                                                                                                                                                          \
            return EC_ERR_INVALID_ARG;                                                                                                                                          \
        EC_MUTEX_LOCK(it->m->lock);                                                                                                                                             \
        if (!it->curr)                                                                                                                                                          \
        {                                                                                                                                                                       \
            EC_MUTEX_UNLOCK(it->m->lock);                                                                                                                                       \
            return EC_ERR_OUT_OF_BOUNDS;                                                                                                                                        \
        }                                                                                                                                                                       \
        *k = it->curr->key;                                                                                                                                                     \
        it->curr = ec_treeset_##NAME##_suc(it->curr);                                                                                                                           \
        EC_MUTEX_UNLOCK(it->m->lock);                                                                                                                                           \
        return EC_OK;                                                                                                                                                           \
    }                                                                                                                                                                           \
    static inline ec_status_t ec_treeset_##NAME##_iter_remove_f(ec_treeset_##NAME##_iter_t *it)                                                                                 \
    {                                                                                                                                                                           \
        if (!it || !it->curr)                                                                                                                                                   \
            return EC_ERR_INVALID_ARG;                                                                                                                                          \
        EC_MUTEX_LOCK(it->m->lock);                                                                                                                                             \
        ec_treeset_##NAME##_entry_t *r = it->curr;                                                                                                                              \
        it->curr = ec_treeset_##NAME##_suc(it->curr);                                                                                                                           \
        it->m->root = ec_treeset_##NAME##_rm(it->m, it->m->root, &r->key);                                                                                                      \
        EC_MUTEX_UNLOCK(it->m->lock);                                                                                                                                           \
        return EC_OK;                                                                                                                                                           \
    }                                                                                                                                                                           \
    static inline ec_status_t ec_treeset_##NAME##_init(ec_treeset_##NAME##_t *m, void *mp, int (*cmp)(const TYPE *, const TYPE *))                                              \
    {                                                                                                                                                                           \
        if (!m)                                                                                                                                                                 \
            return EC_ERR_INVALID_ARG;                                                                                                                                          \
        EC_MUTEX_INIT(m->lock);                                                                                                                                                 \
        m->root = NULL;                                                                                                                                                         \
        m->size = 0;                                                                                                                                                            \
        m->mempool = mp;                                                                                                                                                        \
        m->compare = cmp ? cmp : ec_treeset_##NAME##_dfcmp;                                                                                                                     \
        m->add = ec_treeset_##NAME##_add_f;                                                                                                                                     \
        m->remove = ec_treeset_##NAME##_remove_f;                                                                                                                               \
        m->remove_all = ec_treeset_##NAME##_remove_all_f;                                                                                                                       \
        m->remove_first = ec_treeset_##NAME##_remove_first_f;                                                                                                                   \
        m->remove_last = ec_treeset_##NAME##_remove_last_f;                                                                                                                     \
        m->get_first = ec_treeset_##NAME##_get_first_f;                                                                                                                         \
        m->get_last = ec_treeset_##NAME##_get_last_f;                                                                                                                           \
        m->get_greater_than = ec_treeset_##NAME##_get_greater_than_f;                                                                                                           \
        m->get_lesser_than = ec_treeset_##NAME##_get_lesser_than_f;                                                                                                             \
        m->size_fn = ec_treeset_##NAME##_size_f;                                                                                                                                \
        m->contains = ec_treeset_##NAME##_contains_f;                                                                                                                           \
        m->foreach = ec_treeset_##NAME##_foreach_f;                                                                                                                             \
        m->iter_init = ec_treeset_##NAME##_iter_init_f;                                                                                                                         \
        m->iter_next = ec_treeset_##NAME##_iter_next_f;                                                                                                                         \
        m->iter_remove = ec_treeset_##NAME##_iter_remove_f;                                                                                                                     \
        return EC_OK;                                                                                                                                                           \
    }
EC_TREESET_DEF(int, int)
EC_TREESET_DEF(unsigned int, uint)
EC_TREESET_DEF(long, long)
EC_TREESET_DEF(long long, ll)
EC_TREESET_DEF(unsigned long, ul)
EC_TREESET_DEF(unsigned long long, ull)
EC_TREESET_DEF(uint32_t, u32)
EC_TREESET_DEF(uint64_t, u64)
EC_TREESET_DEF(double, dbl)
EC_TREESET_DEF(const char *, str)
EC_TREESET_DEF(void *, ptr)

#endif
