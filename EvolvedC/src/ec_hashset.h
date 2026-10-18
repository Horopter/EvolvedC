#ifndef EC_HASHSET_H
#define EC_HASHSET_H
#include "ec_common.h"
#include "ec_status.h"
#ifdef EC_USE_MEMPOOL
#define EC_HASHSET_ALLOC(T, MPOOL) ((MPOOL) ? (T*)ec_mempool_alloc((ec_mempool_t*)(MPOOL), sizeof(T)) : (T*)EC_MALLOC(sizeof(T)))
#define EC_HASHSET_CALLOC(N, T, MPOOL) ((MPOOL) ? (T*)ec_mempool_alloc((ec_mempool_t*)(MPOOL), (N) * sizeof(T)) : (T*)EC_CALLOC(N, sizeof(T)))
#else
#define EC_HASHSET_ALLOC(T, MPOOL) (T*)EC_MALLOC(sizeof(T))
#define EC_HASHSET_CALLOC(N, T, MPOOL) (T*)EC_CALLOC(N, sizeof(T))
#endif
#define EC_HASHSET_DEF(TYPE, NAME)                                                                                            \
	typedef struct ec_hashset_##NAME##_entry_s                                                                                \
	{                                                                                                                         \
		TYPE key;                                                                                                             \
		struct ec_hashset_##NAME##_entry_s* next;                                                                             \
	} ec_hashset_##NAME##_entry_t;                                                                                            \
	typedef struct ec_hashset_##NAME##_iter_s                                                                                 \
	{                                                                                                                         \
		struct ec_hashset_##NAME##_s* m;                                                                                      \
		size_t bucket;                                                                                                        \
		ec_hashset_##NAME##_entry_t* prev;                                                                                    \
		ec_hashset_##NAME##_entry_t* curr;                                                                                    \
	} ec_hashset_##NAME##_iter_t;                                                                                             \
	typedef struct ec_hashset_##NAME##_s                                                                                      \
	{                                                                                                                         \
		ec_hashset_##NAME##_entry_t** table;                                                                                  \
		size_t capacity;                                                                                                      \
		size_t size;                                                                                                          \
		size_t threshold;                                                                                                     \
		double load_factor;                                                                                                   \
		EC_MUTEX_TYPE lock;                                                                                                   \
		void* mempool;                                                                                                        \
		ec_status_t (*add)(struct ec_hashset_##NAME##_s*, const TYPE*);                                                       \
		ec_status_t (*remove)(struct ec_hashset_##NAME##_s*, const TYPE*);                                                    \
		ec_status_t (*remove_all)(struct ec_hashset_##NAME##_s*);                                                             \
		ec_status_t (*contains)(struct ec_hashset_##NAME##_s*, const TYPE*);                                                  \
		size_t (*size_fn)(struct ec_hashset_##NAME##_s*);                                                                     \
		size_t (*capacity_fn)(struct ec_hashset_##NAME##_s*);                                                                 \
		ec_status_t (*get_keys)(struct ec_hashset_##NAME##_s*, TYPE**, size_t*);                                              \
		size_t (*hash_string)(const char*);                                                                                   \
		size_t (*hash_ptr)(const void*);                                                                                      \
		size_t (*hash_key)(TYPE);                                                                                             \
		ec_status_t (*foreach_key)(struct ec_hashset_##NAME##_s*, void (*)(TYPE));                                            \
		ec_status_t (*iter_init)(struct ec_hashset_##NAME##_s*, ec_hashset_##NAME##_iter_t*);                                 \
		ec_status_t (*iter_next)(ec_hashset_##NAME##_iter_t*, TYPE*);                                                         \
		ec_status_t (*iter_remove)(ec_hashset_##NAME##_iter_t*);                                                              \
	} ec_hashset_##NAME##_t;                                                                                                  \
	static inline size_t ec_hashset_##NAME##_hash_string(const char* s)                                                       \
	{                                                                                                                         \
		size_t h = 0;                                                                                                         \
		while (*s)                                                                                                            \
			h = 31 * h + (unsigned char)(*s++);                                                                               \
		return h;                                                                                                             \
	}                                                                                                                         \
	static inline size_t ec_hashset_##NAME##_hash_ptr(const void* p)                                                          \
	{                                                                                                                         \
		return (size_t)((uintptr_t)p * 2654435761U);                                                                          \
	}                                                                                                                         \
	static inline size_t ec_hashset_##NAME##_hash(TYPE k)                                                                     \
	{                                                                                                                         \
		union                                                                                                                 \
		{                                                                                                                     \
			TYPE t;                                                                                                           \
			size_t u;                                                                                                         \
		} c;                                                                                                                  \
		c.t = (TYPE)k;                                                                                                        \
		return c.u * 2654435761U;                                                                                             \
	}                                                                                                                         \
	static inline void ec_hashset_##NAME##_rehash(ec_hashset_##NAME##_t* m)                                                   \
	{                                                                                                                         \
		size_t oc = m->capacity;                                                                                              \
		ec_hashset_##NAME##_entry_t** ot = m->table;                                                                          \
		size_t nc = oc << 1;                                                                                                  \
		if (!nc)                                                                                                              \
			return;                                                                                                           \
		ec_hashset_##NAME##_entry_t** nt = EC_HASHSET_CALLOC(nc, ec_hashset_##NAME##_entry_t*, m->mempool);                   \
		if (!nt)                                                                                                              \
			return;                                                                                                           \
		for (size_t i = 0; i < oc; i++)                                                                                       \
			{                                                                                                                 \
				ec_hashset_##NAME##_entry_t* e = ot[i];                                                                       \
				while (e)                                                                                                     \
					{                                                                                                         \
						ec_hashset_##NAME##_entry_t* n = e->next;                                                             \
						size_t idx = (m->hash_key((TYPE)e->key)) & (nc - 1);                                                  \
						e->next = nt[idx];                                                                                    \
						nt[idx] = e;                                                                                          \
						e = n;                                                                                                \
					}                                                                                                         \
			}                                                                                                                 \
		EC_FREE(ot);                                                                                                          \
		m->table = nt;                                                                                                        \
		m->capacity = nc;                                                                                                     \
		m->threshold = (size_t)(nc * m->load_factor);                                                                         \
	}                                                                                                                         \
	static inline ec_status_t ec_hashset_##NAME##_add_f(struct ec_hashset_##NAME##_s* m, const TYPE* key)                     \
	{                                                                                                                         \
		if (!m || !key)                                                                                                       \
			return EC_ERR_INVALID_ARG;                                                                                        \
		EC_MUTEX_LOCK(m->lock);                                                                                               \
		if (m->size >= m->threshold)                                                                                          \
			ec_hashset_##NAME##_rehash(m);                                                                                    \
		size_t idx = (m->hash_key((TYPE) * key)) & (m->capacity - 1);                                                         \
		ec_hashset_##NAME##_entry_t* e = m->table[idx];                                                                       \
		while (e)                                                                                                             \
			{                                                                                                                 \
				if ((TYPE)e->key == (TYPE) * key)                                                                             \
					{                                                                                                         \
						EC_MUTEX_UNLOCK(m->lock);                                                                             \
						return EC_OK;                                                                                         \
					}                                                                                                         \
				e = e->next;                                                                                                  \
			}                                                                                                                 \
		ec_hashset_##NAME##_entry_t* ne = EC_HASHSET_ALLOC(ec_hashset_##NAME##_entry_t, m->mempool);                          \
		if (!ne)                                                                                                              \
			{                                                                                                                 \
				EC_MUTEX_UNLOCK(m->lock);                                                                                     \
				return EC_ERR_ALLOC;                                                                                          \
			}                                                                                                                 \
		ne->key = (TYPE) * key;                                                                                               \
		ne->next = m->table[idx];                                                                                             \
		m->table[idx] = ne;                                                                                                   \
		m->size++;                                                                                                            \
		EC_MUTEX_UNLOCK(m->lock);                                                                                             \
		return EC_OK;                                                                                                         \
	}                                                                                                                         \
	static inline ec_status_t ec_hashset_##NAME##_remove_f(struct ec_hashset_##NAME##_s* m, const TYPE* key)                  \
	{                                                                                                                         \
		if (!m || !key)                                                                                                       \
			return EC_ERR_INVALID_ARG;                                                                                        \
		EC_MUTEX_LOCK(m->lock);                                                                                               \
		size_t idx = (m->hash_key((TYPE) * key)) & (m->capacity - 1);                                                         \
		ec_hashset_##NAME##_entry_t* e = m->table[idx];                                                                       \
		ec_hashset_##NAME##_entry_t* prev = NULL;                                                                             \
		while (e)                                                                                                             \
			{                                                                                                                 \
				if ((TYPE)e->key == (TYPE) * key)                                                                             \
					{                                                                                                         \
						if (!prev)                                                                                            \
							m->table[idx] = e->next;                                                                          \
						else                                                                                                  \
							prev->next = e->next;                                                                             \
						EC_FREE(e);                                                                                           \
						m->size--;                                                                                            \
						EC_MUTEX_UNLOCK(m->lock);                                                                             \
						return EC_OK;                                                                                         \
					}                                                                                                         \
				prev = e;                                                                                                     \
				e = e->next;                                                                                                  \
			}                                                                                                                 \
		EC_MUTEX_UNLOCK(m->lock);                                                                                             \
		return EC_ERR_NOT_FOUND;                                                                                              \
	}                                                                                                                         \
	static inline ec_status_t ec_hashset_##NAME##_remove_all_f(struct ec_hashset_##NAME##_s* m)                               \
	{                                                                                                                         \
		if (!m)                                                                                                               \
			return EC_ERR_INVALID_ARG;                                                                                        \
		EC_MUTEX_LOCK(m->lock);                                                                                               \
		for (size_t i = 0; i < m->capacity; i++)                                                                              \
			{                                                                                                                 \
				ec_hashset_##NAME##_entry_t* e = m->table[i];                                                                 \
				while (e)                                                                                                     \
					{                                                                                                         \
						ec_hashset_##NAME##_entry_t* n = e->next;                                                             \
						EC_FREE(e);                                                                                           \
						e = n;                                                                                                \
					}                                                                                                         \
				m->table[i] = NULL;                                                                                           \
			}                                                                                                                 \
		m->size = 0;                                                                                                          \
		EC_MUTEX_UNLOCK(m->lock);                                                                                             \
		return EC_OK;                                                                                                         \
	}                                                                                                                         \
	static inline ec_status_t ec_hashset_##NAME##_contains_f(struct ec_hashset_##NAME##_s* m, const TYPE* key)                \
	{                                                                                                                         \
		if (!m || !key)                                                                                                       \
			return EC_ERR_INVALID_ARG;                                                                                        \
		EC_MUTEX_LOCK(m->lock);                                                                                               \
		size_t idx = (m->hash_key((TYPE) * key)) & (m->capacity - 1);                                                         \
		ec_hashset_##NAME##_entry_t* e = m->table[idx];                                                                       \
		while (e)                                                                                                             \
			{                                                                                                                 \
				if ((TYPE)e->key == (TYPE) * key)                                                                             \
					{                                                                                                         \
						EC_MUTEX_UNLOCK(m->lock);                                                                             \
						return EC_OK;                                                                                         \
					}                                                                                                         \
				e = e->next;                                                                                                  \
			}                                                                                                                 \
		EC_MUTEX_UNLOCK(m->lock);                                                                                             \
		return EC_ERR_NOT_FOUND;                                                                                              \
	}                                                                                                                         \
	static inline size_t ec_hashset_##NAME##_size_f(struct ec_hashset_##NAME##_s* m)                                          \
	{                                                                                                                         \
		if (!m)                                                                                                               \
			return 0;                                                                                                         \
		EC_MUTEX_LOCK(m->lock);                                                                                               \
		size_t s = m->size;                                                                                                   \
		EC_MUTEX_UNLOCK(m->lock);                                                                                             \
		return s;                                                                                                             \
	}                                                                                                                         \
	static inline size_t ec_hashset_##NAME##_capacity_f(struct ec_hashset_##NAME##_s* m)                                      \
	{                                                                                                                         \
		if (!m)                                                                                                               \
			return 0;                                                                                                         \
		EC_MUTEX_LOCK(m->lock);                                                                                               \
		size_t c = m->capacity;                                                                                               \
		EC_MUTEX_UNLOCK(m->lock);                                                                                             \
		return c;                                                                                                             \
	}                                                                                                                         \
	static inline ec_status_t ec_hashset_##NAME##_get_keys_f(struct ec_hashset_##NAME##_s* m, TYPE** arr, size_t* count)      \
	{                                                                                                                         \
		if (!m || !arr || !count)                                                                                             \
			return EC_ERR_INVALID_ARG;                                                                                        \
		EC_MUTEX_LOCK(m->lock);                                                                                               \
		*count = m->size;                                                                                                     \
		if (!m->size)                                                                                                         \
			{                                                                                                                 \
				*arr = NULL;                                                                                                  \
				EC_MUTEX_UNLOCK(m->lock);                                                                                     \
				return EC_OK;                                                                                                 \
			}                                                                                                                 \
		*arr = (TYPE*)EC_MALLOC(sizeof(TYPE) * m->size);                                                                      \
		if (!*arr)                                                                                                            \
			{                                                                                                                 \
				EC_MUTEX_UNLOCK(m->lock);                                                                                     \
				return EC_ERR_ALLOC;                                                                                          \
			}                                                                                                                 \
		size_t idx = 0;                                                                                                       \
		for (size_t i = 0; i < m->capacity; i++)                                                                              \
			{                                                                                                                 \
				ec_hashset_##NAME##_entry_t* e = m->table[i];                                                                 \
				while (e)                                                                                                     \
					{                                                                                                         \
						(*arr)[idx++] = e->key;                                                                               \
						e = e->next;                                                                                          \
					}                                                                                                         \
			}                                                                                                                 \
		EC_MUTEX_UNLOCK(m->lock);                                                                                             \
		return EC_OK;                                                                                                         \
	}                                                                                                                         \
	static inline ec_status_t ec_hashset_##NAME##_foreach_key_f(struct ec_hashset_##NAME##_s* m, void (*fn)(TYPE))            \
	{                                                                                                                         \
		if (!m || !fn)                                                                                                        \
			return EC_ERR_INVALID_ARG;                                                                                        \
		EC_MUTEX_LOCK(m->lock);                                                                                               \
		for (size_t i = 0; i < m->capacity; i++)                                                                              \
			{                                                                                                                 \
				ec_hashset_##NAME##_entry_t* e = m->table[i];                                                                 \
				while (e)                                                                                                     \
					{                                                                                                         \
						fn(e->key);                                                                                           \
						e = e->next;                                                                                          \
					}                                                                                                         \
			}                                                                                                                 \
		EC_MUTEX_UNLOCK(m->lock);                                                                                             \
		return EC_OK;                                                                                                         \
	}                                                                                                                         \
	static inline ec_status_t ec_hashset_##NAME##_iter_init_f(struct ec_hashset_##NAME##_s* m, ec_hashset_##NAME##_iter_t* i) \
	{                                                                                                                         \
		if (!m || !i)                                                                                                         \
			return EC_ERR_INVALID_ARG;                                                                                        \
		i->m = m;                                                                                                             \
		i->bucket = 0;                                                                                                        \
		i->prev = NULL;                                                                                                       \
		i->curr = NULL;                                                                                                       \
		return EC_OK;                                                                                                         \
	}                                                                                                                         \
	static inline ec_status_t ec_hashset_##NAME##_iter_next_f(ec_hashset_##NAME##_iter_t* i, TYPE* key)                       \
	{                                                                                                                         \
		if (!i || !key)                                                                                                       \
			return EC_ERR_INVALID_ARG;                                                                                        \
		EC_MUTEX_LOCK(i->m->lock);                                                                                            \
		if (i->curr && i->curr->next)                                                                                         \
			{                                                                                                                 \
				i->prev = i->curr;                                                                                            \
				i->curr = i->curr->next;                                                                                      \
				*key = i->curr->key;                                                                                          \
				EC_MUTEX_UNLOCK(i->m->lock);                                                                                  \
				return EC_OK;                                                                                                 \
			}                                                                                                                 \
		while (i->bucket < i->m->capacity)                                                                                    \
			{                                                                                                                 \
				if (i->bucket == 0 && !i->curr)                                                                               \
					{                                                                                                         \
						i->curr = NULL;                                                                                       \
					}                                                                                                         \
				ec_hashset_##NAME##_entry_t* e = i->m->table[i->bucket];                                                      \
				if (e)                                                                                                        \
					{                                                                                                         \
						i->prev = NULL;                                                                                       \
						i->curr = e;                                                                                          \
						*key = e->key;                                                                                        \
						i->bucket++;                                                                                          \
						EC_MUTEX_UNLOCK(i->m->lock);                                                                          \
						return EC_OK;                                                                                         \
					}                                                                                                         \
				i->bucket++;                                                                                                  \
			}                                                                                                                 \
		EC_MUTEX_UNLOCK(i->m->lock);                                                                                          \
		return EC_ERR_OUT_OF_BOUNDS;                                                                                          \
	}                                                                                                                         \
	static inline ec_status_t ec_hashset_##NAME##_iter_remove_f(ec_hashset_##NAME##_iter_t* i)                                \
	{                                                                                                                         \
		if (!i || !i->curr)                                                                                                   \
			return EC_ERR_INVALID_ARG;                                                                                        \
		EC_MUTEX_LOCK(i->m->lock);                                                                                            \
		size_t idx = (i->m->hash_key((TYPE)i->curr->key)) & (i->m->capacity - 1);                                             \
		if (!i->prev)                                                                                                         \
			{                                                                                                                 \
				i->m->table[idx] = i->curr->next;                                                                             \
			}                                                                                                                 \
		else                                                                                                                  \
			{                                                                                                                 \
				i->prev->next = i->curr->next;                                                                                \
			}                                                                                                                 \
		EC_FREE(i->curr);                                                                                                     \
		i->m->size--;                                                                                                         \
		i->curr = i->prev;                                                                                                    \
		EC_MUTEX_UNLOCK(i->m->lock);                                                                                          \
		return EC_OK;                                                                                                         \
	}                                                                                                                         \
	static inline ec_status_t ec_hashset_##NAME##_init(ec_hashset_##NAME##_t* m, void* mp)                                    \
	{                                                                                                                         \
		if (!m)                                                                                                               \
			return EC_ERR_INVALID_ARG;                                                                                        \
		EC_MUTEX_INIT(m->lock);                                                                                               \
		m->capacity = 16;                                                                                                     \
		m->size = 0;                                                                                                          \
		m->load_factor = 0.75;                                                                                                \
		m->threshold = (size_t)(m->capacity * m->load_factor);                                                                \
		m->table = EC_HASHSET_CALLOC(m->capacity, ec_hashset_##NAME##_entry_t*, mp);                                          \
		if (!m->table)                                                                                                        \
			return EC_ERR_ALLOC;                                                                                              \
		m->mempool = mp;                                                                                                      \
		m->add = ec_hashset_##NAME##_add_f;                                                                                   \
		m->remove = ec_hashset_##NAME##_remove_f;                                                                             \
		m->remove_all = ec_hashset_##NAME##_remove_all_f;                                                                     \
		m->contains = ec_hashset_##NAME##_contains_f;                                                                         \
		m->size_fn = ec_hashset_##NAME##_size_f;                                                                              \
		m->capacity_fn = ec_hashset_##NAME##_capacity_f;                                                                      \
		m->get_keys = ec_hashset_##NAME##_get_keys_f;                                                                         \
		m->hash_string = ec_hashset_##NAME##_hash_string;                                                                     \
		m->hash_ptr = ec_hashset_##NAME##_hash_ptr;                                                                           \
		m->hash_key = ec_hashset_##NAME##_hash;                                                                               \
		m->foreach_key = ec_hashset_##NAME##_foreach_key_f;                                                                   \
		m->iter_init = ec_hashset_##NAME##_iter_init_f;                                                                       \
		m->iter_next = ec_hashset_##NAME##_iter_next_f;                                                                       \
		m->iter_remove = ec_hashset_##NAME##_iter_remove_f;                                                                   \
		return EC_OK;                                                                                                         \
	}
EC_HASHSET_DEF(int, int)
EC_HASHSET_DEF(unsigned int, uint)
EC_HASHSET_DEF(long, long)
EC_HASHSET_DEF(long long, ll)
EC_HASHSET_DEF(unsigned long, ul)
EC_HASHSET_DEF(unsigned long long, ull)
EC_HASHSET_DEF(uint32_t, u32)
EC_HASHSET_DEF(uint64_t, u64)
EC_HASHSET_DEF(double, dbl)
EC_HASHSET_DEF(const char*, str)
EC_HASHSET_DEF(void*, ptr)
#endif
