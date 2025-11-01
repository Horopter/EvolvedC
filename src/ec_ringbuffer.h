#ifndef EC_RINGBUF_H
#define EC_RINGBUF_H
#include "ec_common.h"
#include "ec_status.h"
#ifdef EC_USE_MEMPOOL
#define EC_RINGBUF_ALLOC(T, MP) ((MP) ? (T*)ec_mempool_alloc((ec_mempool_t*)(MP), sizeof(T)) : (T*)EC_MALLOC(sizeof(T)))
#define EC_RINGBUF_REALLOC(OLD, T, NEW, MP) ((MP) ? NULL : (T*)EC_REALLOC(OLD, (NEW) * sizeof(T))) /* If mempool in use, no real realloc */
#else
#define EC_RINGBUF_ALLOC(T, MP) (T*)EC_MALLOC(sizeof(T))
#define EC_RINGBUF_REALLOC(OLD, T, NEW, MP) (T*)EC_REALLOC(OLD, (NEW) * sizeof(T))
#endif

#define EC_RINGBUF_DEF(TYPE, NAME)                                                                                                               \
	typedef struct ec_ringbuf_##NAME##_s ec_ringbuf_##NAME##_t;                                                                                  \
	typedef struct ec_ringbuf_##NAME##_s                                                                                                         \
	{                                                                                                                                            \
		TYPE* buf;                                                                                                                               \
		size_t cap;                                                                                                                              \
		size_t start;                                                                                                                            \
		size_t end;                                                                                                                              \
		size_t count;                                                                                                                            \
		EC_MUTEX_TYPE lock;                                                                                                                      \
		void* mempool;                                                                                                                           \
		int (*compare)(const TYPE*, const TYPE*);                                                                                                \
		ec_status_t (*push)(ec_ringbuf_##NAME##_t*, const TYPE*);                                                                                \
		ec_status_t (*pop)(ec_ringbuf_##NAME##_t*, TYPE*);                                                                                       \
		ec_status_t (*front)(ec_ringbuf_##NAME##_t*, TYPE*);                                                                                     \
		ec_status_t (*back)(ec_ringbuf_##NAME##_t*, TYPE*);                                                                                      \
		ec_status_t (*clear)(ec_ringbuf_##NAME##_t*);                                                                                            \
		size_t (*size_fn)(ec_ringbuf_##NAME##_t*);                                                                                               \
		size_t (*capacity_fn)(ec_ringbuf_##NAME##_t*);                                                                                           \
		ec_status_t (*contains)(ec_ringbuf_##NAME##_t*, const TYPE*);                                                                            \
		ec_status_t (*foreach)(ec_ringbuf_##NAME##_t*, void (*)(TYPE));                                                                          \
	} ec_ringbuf_##NAME##_t;                                                                                                                     \
	static inline int ec_ringbuf_##NAME##_dfcmp(const TYPE* a, const TYPE* b)                                                                    \
	{                                                                                                                                            \
		return (*a < *b) ? -1 : ((*a > *b) ? 1 : 0);                                                                                             \
	}                                                                                                                                            \
	static inline size_t ec_ringbuf_##NAME##_size_f(ec_ringbuf_##NAME##_t* m)                                                                    \
	{                                                                                                                                            \
		if (!m)                                                                                                                                  \
			return 0;                                                                                                                            \
		EC_MUTEX_LOCK(m->lock);                                                                                                                  \
		size_t sz = m->count;                                                                                                                    \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                \
		return sz;                                                                                                                               \
	}                                                                                                                                            \
	static inline size_t ec_ringbuf_##NAME##_cap_f(ec_ringbuf_##NAME##_t* m)                                                                     \
	{                                                                                                                                            \
		if (!m)                                                                                                                                  \
			return 0;                                                                                                                            \
		EC_MUTEX_LOCK(m->lock);                                                                                                                  \
		size_t c = m->cap;                                                                                                                       \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                \
		return c;                                                                                                                                \
	}                                                                                                                                            \
	static inline ec_status_t ec_ringbuf_##NAME##_grow(ec_ringbuf_##NAME##_t* m)                                                                 \
	{                                                                                                                                            \
		size_t newcap = (m->cap < 1) ? 4 : (m->cap * 2);                                                                                         \
		TYPE* n = (TYPE*)EC_RINGBUF_REALLOC(m->buf, TYPE, newcap, m->mempool);                                                                   \
		if (!n)                                                                                                                                  \
			return EC_ERR_ALLOC;                                                                                                                 \
		if (m->end < m->start || (m->count == m->cap && m->end == m->start))                                                                     \
			{                                                                                                                                    \
				size_t tailCount = m->cap - m->start;                                                                                            \
				for (size_t i = 0; i < tailCount; i++)                                                                                           \
					{                                                                                                                            \
						n[newcap - tailCount + i] = n[m->start + i];                                                                             \
					}                                                                                                                            \
				m->end = newcap - (m->cap - m->end);                                                                                             \
			}                                                                                                                                    \
		m->buf = n;                                                                                                                              \
		m->cap = newcap;                                                                                                                         \
		return EC_OK;                                                                                                                            \
	}                                                                                                                                            \
	static inline ec_status_t ec_ringbuf_##NAME##_push_f(ec_ringbuf_##NAME##_t* m, const TYPE* val)                                              \
	{                                                                                                                                            \
		if (!m || !val)                                                                                                                          \
			return EC_ERR_INVALID_ARG;                                                                                                           \
		EC_MUTEX_LOCK(m->lock);                                                                                                                  \
		if (m->count == m->cap)                                                                                                                  \
			{                                                                                                                                    \
				ec_status_t st = ec_ringbuf_##NAME##_grow(m);                                                                                    \
				if (st != EC_OK)                                                                                                                 \
					{                                                                                                                            \
						EC_MUTEX_UNLOCK(m->lock);                                                                                                \
						return st;                                                                                                               \
					}                                                                                                                            \
			}                                                                                                                                    \
		m->buf[m->end] = (TYPE)(*val);                                                                                                           \
		m->end = (m->end + 1) % m->cap;                                                                                                          \
		m->count++;                                                                                                                              \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                \
		return EC_OK;                                                                                                                            \
	}                                                                                                                                            \
	static inline ec_status_t ec_ringbuf_##NAME##_pop_f(ec_ringbuf_##NAME##_t* m, TYPE* out)                                                     \
	{                                                                                                                                            \
		if (!m || !out)                                                                                                                          \
			return EC_ERR_INVALID_ARG;                                                                                                           \
		EC_MUTEX_LOCK(m->lock);                                                                                                                  \
		if (!m->count)                                                                                                                           \
			{                                                                                                                                    \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                        \
				return EC_ERR_NOT_FOUND;                                                                                                         \
			}                                                                                                                                    \
		*out = m->buf[m->start];                                                                                                                 \
		m->start = (m->start + 1) % m->cap;                                                                                                      \
		m->count--;                                                                                                                              \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                \
		return EC_OK;                                                                                                                            \
	}                                                                                                                                            \
	static inline ec_status_t ec_ringbuf_##NAME##_front_f(ec_ringbuf_##NAME##_t* m, TYPE* out)                                                   \
	{                                                                                                                                            \
		if (!m || !out)                                                                                                                          \
			return EC_ERR_INVALID_ARG;                                                                                                           \
		EC_MUTEX_LOCK(m->lock);                                                                                                                  \
		if (!m->count)                                                                                                                           \
			{                                                                                                                                    \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                        \
				return EC_ERR_NOT_FOUND;                                                                                                         \
			}                                                                                                                                    \
		*out = m->buf[m->start];                                                                                                                 \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                \
		return EC_OK;                                                                                                                            \
	}                                                                                                                                            \
	static inline ec_status_t ec_ringbuf_##NAME##_back_f(ec_ringbuf_##NAME##_t* m, TYPE* out)                                                    \
	{                                                                                                                                            \
		if (!m || !out)                                                                                                                          \
			return EC_ERR_INVALID_ARG;                                                                                                           \
		EC_MUTEX_LOCK(m->lock);                                                                                                                  \
		if (!m->count)                                                                                                                           \
			{                                                                                                                                    \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                        \
				return EC_ERR_NOT_FOUND;                                                                                                         \
			}                                                                                                                                    \
		size_t idx = (m->end == 0 ? m->cap - 1 : m->end - 1);                                                                                    \
		*out = m->buf[idx];                                                                                                                      \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                \
		return EC_OK;                                                                                                                            \
	}                                                                                                                                            \
	static inline ec_status_t ec_ringbuf_##NAME##_clear_f(ec_ringbuf_##NAME##_t* m)                                                              \
	{                                                                                                                                            \
		if (!m)                                                                                                                                  \
			return EC_ERR_INVALID_ARG;                                                                                                           \
		EC_MUTEX_LOCK(m->lock);                                                                                                                  \
		m->start = m->end = 0;                                                                                                                   \
		m->count = 0;                                                                                                                            \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                \
		return EC_OK;                                                                                                                            \
	}                                                                                                                                            \
	static inline ec_status_t ec_ringbuf_##NAME##_contains_f(ec_ringbuf_##NAME##_t* m, const TYPE* val)                                          \
	{                                                                                                                                            \
		if (!m || !val)                                                                                                                          \
			return EC_ERR_INVALID_ARG;                                                                                                           \
		EC_MUTEX_LOCK(m->lock);                                                                                                                  \
		size_t cnt = m->count, i = m->start;                                                                                                     \
		ec_status_t ret = EC_ERR_NOT_FOUND;                                                                                                      \
		while (cnt > 0)                                                                                                                          \
			{                                                                                                                                    \
				if (!m->compare(&m->buf[i], val))                                                                                                \
					{                                                                                                                            \
						ret = EC_OK;                                                                                                             \
						break;                                                                                                                   \
					}                                                                                                                            \
				i = (i + 1) % m->cap;                                                                                                            \
				cnt--;                                                                                                                           \
			}                                                                                                                                    \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                \
		return ret;                                                                                                                              \
	}                                                                                                                                            \
	static inline ec_status_t ec_ringbuf_##NAME##_foreach_f(ec_ringbuf_##NAME##_t* m, void (*fn)(TYPE))                                          \
	{                                                                                                                                            \
		if (!m || !fn)                                                                                                                           \
			return EC_ERR_INVALID_ARG;                                                                                                           \
		EC_MUTEX_LOCK(m->lock);                                                                                                                  \
		size_t cnt = m->count, i = m->start;                                                                                                     \
		while (cnt > 0)                                                                                                                          \
			{                                                                                                                                    \
				fn(m->buf[i]);                                                                                                                   \
				i = (i + 1) % m->cap;                                                                                                            \
				cnt--;                                                                                                                           \
			}                                                                                                                                    \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                \
		return EC_OK;                                                                                                                            \
	}                                                                                                                                            \
	static inline ec_status_t ec_ringbuf_##NAME##_init(ec_ringbuf_##NAME##_t* m, void* mp, size_t initcap, int (*cmp)(const TYPE*, const TYPE*)) \
	{                                                                                                                                            \
		if (!m)                                                                                                                                  \
			return EC_ERR_INVALID_ARG;                                                                                                           \
		EC_MUTEX_INIT(m->lock);                                                                                                                  \
		m->buf = NULL;                                                                                                                           \
		m->cap = 0;                                                                                                                              \
		m->start = 0;                                                                                                                            \
		m->end = 0;                                                                                                                              \
		m->count = 0;                                                                                                                            \
		m->mempool = mp;                                                                                                                         \
		m->compare = cmp ? cmp : ec_ringbuf_##NAME##_dfcmp;                                                                                      \
		if (initcap > 0)                                                                                                                         \
			{                                                                                                                                    \
				m->buf = (TYPE*)EC_RINGBUF_REALLOC(NULL, TYPE, initcap, mp);                                                                     \
				if (!m->buf)                                                                                                                     \
					return EC_ERR_ALLOC;                                                                                                         \
				m->cap = initcap;                                                                                                                \
			}                                                                                                                                    \
		m->push = ec_ringbuf_##NAME##_push_f;                                                                                                    \
		m->pop = ec_ringbuf_##NAME##_pop_f;                                                                                                      \
		m->front = ec_ringbuf_##NAME##_front_f;                                                                                                  \
		m->back = ec_ringbuf_##NAME##_back_f;                                                                                                    \
		m->clear = ec_ringbuf_##NAME##_clear_f;                                                                                                  \
		m->size_fn = ec_ringbuf_##NAME##_size_f;                                                                                                 \
		m->capacity_fn = ec_ringbuf_##NAME##_cap_f;                                                                                              \
		m->contains = ec_ringbuf_##NAME##_contains_f;                                                                                            \
		m->foreach = ec_ringbuf_##NAME##_foreach_f;                                                                                              \
		return EC_OK;                                                                                                                            \
	}
EC_RINGBUF_DEF(char, ch)
EC_RINGBUF_DEF(int, int)
EC_RINGBUF_DEF(unsigned int, uint)
EC_RINGBUF_DEF(long, long)
EC_RINGBUF_DEF(long long, ll)
EC_RINGBUF_DEF(unsigned long, ul)
EC_RINGBUF_DEF(unsigned long long, ull)
EC_RINGBUF_DEF(uint32_t, u32)
EC_RINGBUF_DEF(uint64_t, u64)
EC_RINGBUF_DEF(double, dbl)
EC_RINGBUF_DEF(const char*, str)
EC_RINGBUF_DEF(void*, ptr)

#endif
