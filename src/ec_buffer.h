#ifndef EC_BUF_H
#define EC_BUF_H

#include "ec_common.h"
#include "ec_status.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifdef EC_USE_MEMPOOL
#define EC_BUF_ALLOC(SZ, MPOOL) ((MPOOL) ? ec_mempool_alloc((ec_mempool_t *)(MPOOL), (SZ)) : EC_MALLOC(SZ))
#define EC_BUF_FREE(PTR, MPOOL) ((MPOOL) ? ec_mempool_free((ec_mempool_t *)(MPOOL), (PTR)) : EC_FREE(PTR))
#else
#define EC_BUF_ALLOC(SZ, MPOOL) EC_MALLOC(SZ)
#define EC_BUF_FREE(PTR, MPOOL) EC_FREE(PTR)
#endif

/* Define a Generic Buffer for Any Type */
#define EC_BUF_DEF(TYPE, NAME)                                                                                               \
    typedef struct                                                                                                           \
    {                                                                                                                        \
        TYPE *mem;                                                                                                           \
        size_t cap;                                                                                                          \
        size_t limit;                                                                                                        \
        size_t rpos;                                                                                                         \
        size_t wpos;                                                                                                         \
        bool ref;                                                                                                            \
        bool err;                                                                                                            \
        void *mempool;                                                                                                       \
    } ec_buf_##NAME##_t;                                                                                                     \
                                                                                                                             \
    static inline bool ec_buf_##NAME##_init(ec_buf_##NAME##_t *buf, size_t cap, void *mempool)                               \
    {                                                                                                                        \
        if (!buf)                                                                                                            \
            return false;                                                                                                    \
        buf->mem = (TYPE *)EC_BUF_ALLOC(cap * sizeof(TYPE), mempool);                                                        \
        if (!buf->mem)                                                                                                       \
            return false;                                                                                                    \
        buf->cap = cap;                                                                                                      \
        buf->limit = SIZE_MAX;                                                                                               \
        buf->rpos = buf->wpos = 0;                                                                                           \
        buf->ref = false;                                                                                                    \
        buf->err = false;                                                                                                    \
        buf->mempool = mempool;                                                                                              \
        return true;                                                                                                         \
    }                                                                                                                        \
                                                                                                                             \
    static inline void ec_buf_##NAME##_wrap(ec_buf_##NAME##_t *buf, TYPE *data, size_t len)                                  \
    {                                                                                                                        \
        buf->mem = data;                                                                                                     \
        buf->cap = len;                                                                                                      \
        buf->rpos = 0;                                                                                                       \
        buf->wpos = len;                                                                                                     \
        buf->ref = true;                                                                                                     \
        buf->err = false;                                                                                                    \
    }                                                                                                                        \
                                                                                                                             \
    static inline void ec_buf_##NAME##_free(ec_buf_##NAME##_t *buf)                                                          \
    {                                                                                                                        \
        if (buf && buf->mem && !buf->ref)                                                                                    \
        {                                                                                                                    \
            EC_BUF_FREE(buf->mem, buf->mempool);                                                                             \
        }                                                                                                                    \
        buf->mem = NULL;                                                                                                     \
        buf->cap = buf->rpos = buf->wpos = 0;                                                                                \
    }                                                                                                                        \
                                                                                                                             \
    static inline void ec_buf_##NAME##_clear(ec_buf_##NAME##_t *buf)                                                         \
    {                                                                                                                        \
        if (!buf)                                                                                                            \
            return;                                                                                                          \
        buf->rpos = buf->wpos = 0;                                                                                           \
        buf->err = false;                                                                                                    \
    }                                                                                                                        \
                                                                                                                             \
    static inline size_t ec_buf_##NAME##_quota(ec_buf_##NAME##_t *buf) { return buf ? buf->cap - buf->wpos : 0; }            \
    static inline size_t ec_buf_##NAME##_size(ec_buf_##NAME##_t *buf) { return buf ? buf->wpos - buf->rpos : 0; }            \
                                                                                                                             \
    static inline bool ec_buf_##NAME##_reserve(ec_buf_##NAME##_t *buf, size_t len)                                           \
    {                                                                                                                        \
        if (!buf || buf->cap >= len)                                                                                         \
            return true;                                                                                                     \
        if (buf->limit < len)                                                                                                \
        {                                                                                                                    \
            buf->err = true;                                                                                                 \
            return false;                                                                                                    \
        }                                                                                                                    \
        TYPE *new_mem = (TYPE *)EC_BUF_ALLOC(len * sizeof(TYPE), buf->mempool);                                              \
        if (!new_mem)                                                                                                        \
        {                                                                                                                    \
            buf->err = true;                                                                                                 \
            return false;                                                                                                    \
        }                                                                                                                    \
        memcpy(new_mem, buf->mem, buf->wpos * sizeof(TYPE));                                                                 \
        EC_BUF_FREE(buf->mem, buf->mempool);                                                                                 \
        buf->mem = new_mem;                                                                                                  \
        buf->cap = len;                                                                                                      \
        return true;                                                                                                         \
    }                                                                                                                        \
                                                                                                                             \
    static inline bool ec_buf_##NAME##_get(ec_buf_##NAME##_t *buf, TYPE *val)                                                \
    {                                                                                                                        \
        if (!buf || buf->rpos >= buf->wpos)                                                                                  \
        {                                                                                                                    \
            buf->err = true;                                                                                                 \
            return false;                                                                                                    \
        }                                                                                                                    \
        EC_MEMCPY_SAFE(val, &buf->mem[buf->rpos++], sizeof(TYPE));                                                           \
        return true;                                                                                                         \
    }                                                                                                                        \
    static inline bool ec_buf_##NAME##_peek(ec_buf_##NAME##_t *buf, TYPE *val)                                               \
    {                                                                                                                        \
        if (!buf || buf->rpos >= buf->wpos)                                                                                  \
        {                                                                                                                    \
            buf->err = true;                                                                                                 \
            return false;                                                                                                    \
        }                                                                                                                    \
        EC_MEMCPY_SAFE(val, &buf->mem[buf->rpos], sizeof(TYPE));                                                             \
        return true;                                                                                                         \
    }                                                                                                                        \
                                                                                                                             \
    static inline bool ec_buf_##NAME##_put(ec_buf_##NAME##_t *buf, TYPE val)                                                 \
    {                                                                                                                        \
        if (!buf || buf->wpos >= buf->cap)                                                                                   \
        {                                                                                                                    \
            if (!ec_buf_##NAME##_reserve(buf, buf->cap * 2))                                                                 \
                return false;                                                                                                \
        }                                                                                                                    \
        buf->mem[buf->wpos++] = val;                                                                                         \
        return true;                                                                                                         \
    }                                                                                                                        \
                                                                                                                             \
    static inline void *ec_buf_##NAME##_rbuf(ec_buf_##NAME##_t *buf) { return buf ? (void *)(buf->mem + buf->rpos) : NULL; } \
    static inline void *ec_buf_##NAME##_wbuf(ec_buf_##NAME##_t *buf) { return buf ? (void *)(buf->mem + buf->wpos) : NULL; } \
                                                                                                                             \
    static inline void ec_buf_##NAME##_compact(ec_buf_##NAME##_t *buf)                                                       \
    {                                                                                                                        \
        if (!buf || buf->rpos == 0)                                                                                          \
            return;                                                                                                          \
        size_t size = buf->wpos - buf->rpos;                                                                                 \
        memmove(buf->mem, buf->mem + buf->rpos, size * sizeof(TYPE));                                                        \
        buf->wpos = size;                                                                                                    \
        buf->rpos = 0;                                                                                                       \
    }                                                                                                                        \
                                                                                                                             \
    static inline bool ec_buf_##NAME##_valid(ec_buf_##NAME##_t *buf) { return buf ? !buf->err : false; }

/* Instantiate Buffers for Common Types */
EC_BUF_DEF(int, int)
EC_BUF_DEF(unsigned int, uint)
EC_BUF_DEF(long, long)
EC_BUF_DEF(long long, ll)
EC_BUF_DEF(unsigned long, ul)
EC_BUF_DEF(unsigned long long, ull)
EC_BUF_DEF(uint32_t, u32)
EC_BUF_DEF(uint64_t, u64)
EC_BUF_DEF(float, flt)
EC_BUF_DEF(double, dbl)
EC_BUF_DEF(const char *, str)
EC_BUF_DEF(void *, ptr)

#endif // EC_BUF_H
