#ifndef EC_DEQUE_H
#define EC_DEQUE_H

#include "ec_common.h"
#include "ec_status.h"

#ifdef EC_USE_MEMPOOL
#define EC_DEQUE_ALLOC(TYPE, MPOOL) ((MPOOL) ? (TYPE*)ec_mempool_alloc((ec_mempool_t*)(MPOOL), sizeof(TYPE)) : (TYPE*)EC_MALLOC(sizeof(TYPE)))
#else
#define EC_DEQUE_ALLOC(TYPE, MPOOL) (TYPE*)EC_MALLOC(sizeof(TYPE))
#endif

#define EC_DEQUE_DEF(TYPE, NAME)                                                               \
	typedef struct ec_deque_##NAME##_node_s                                                    \
	{                                                                                          \
		TYPE data;                                                                             \
		struct ec_deque_##NAME##_node_s* next;                                                 \
		struct ec_deque_##NAME##_node_s* prev;                                                 \
	} ec_deque_##NAME##_node_t;                                                                \
                                                                                               \
	typedef struct ec_deque_##NAME##_s                                                         \
	{                                                                                          \
		ec_deque_##NAME##_node_t *head, *tail;                                                 \
		size_t size;                                                                           \
		EC_MUTEX_TYPE lock;                                                                    \
		void* mempool;                                                                         \
		ec_status_t (*push_front)(struct ec_deque_##NAME##_s*, void*);                         \
		ec_status_t (*push_back)(struct ec_deque_##NAME##_s*, void*);                          \
		ec_status_t (*pop_front)(struct ec_deque_##NAME##_s*, void**);                         \
		ec_status_t (*pop_back)(struct ec_deque_##NAME##_s*, void**);                          \
		ec_status_t (*peek_front)(struct ec_deque_##NAME##_s*, void**);                        \
		ec_status_t (*peek_back)(struct ec_deque_##NAME##_s*, void**);                         \
		size_t (*size_fn)(struct ec_deque_##NAME##_s*);                                        \
		ec_status_t (*free_fn)(struct ec_deque_##NAME##_s*);                                   \
	} ec_deque_##NAME##_t;                                                                     \
                                                                                               \
	static inline ec_status_t ec_deque_##NAME##_push_front(ec_deque_##NAME##_t* d, void* elem) \
	{                                                                                          \
		ec_deque_##NAME##_node_t* n = EC_DEQUE_ALLOC(ec_deque_##NAME##_node_t, d->mempool);    \
		if (!n)                                                                                \
			return EC_ERR_ALLOC;                                                               \
		n->data = *(TYPE*)elem;                                                                \
		n->prev = NULL;                                                                        \
		EC_MUTEX_LOCK(d->lock);                                                                \
		n->next = d->head;                                                                     \
		if (d->head)                                                                           \
			d->head->prev = n;                                                                 \
		d->head = n;                                                                           \
		if (!d->tail)                                                                          \
			d->tail = n;                                                                       \
		d->size++;                                                                             \
		EC_MUTEX_UNLOCK(d->lock);                                                              \
		return EC_OK;                                                                          \
	}                                                                                          \
                                                                                               \
	static inline ec_status_t ec_deque_##NAME##_push_back(ec_deque_##NAME##_t* d, void* elem)  \
	{                                                                                          \
		ec_deque_##NAME##_node_t* n = EC_DEQUE_ALLOC(ec_deque_##NAME##_node_t, d->mempool);    \
		if (!n)                                                                                \
			return EC_ERR_ALLOC;                                                               \
		n->data = *(TYPE*)elem;                                                                \
		n->next = NULL;                                                                        \
		EC_MUTEX_LOCK(d->lock);                                                                \
		n->prev = d->tail;                                                                     \
		if (d->tail)                                                                           \
			d->tail->next = n;                                                                 \
		d->tail = n;                                                                           \
		if (!d->head)                                                                          \
			d->head = n;                                                                       \
		d->size++;                                                                             \
		EC_MUTEX_UNLOCK(d->lock);                                                              \
		return EC_OK;                                                                          \
	}                                                                                          \
	static inline ec_status_t ec_deque_##NAME##_pop_front(ec_deque_##NAME##_t* d, void** out)  \
	{                                                                                          \
		if (!d || !out)                                                                        \
			return EC_ERR_INVALID_ARG;                                                         \
		EC_MUTEX_LOCK(d->lock);                                                                \
		if (!d->head)                                                                          \
			{                                                                                  \
				EC_MUTEX_UNLOCK(d->lock);                                                      \
				return EC_ERR_OUT_OF_BOUNDS;                                                   \
			}                                                                                  \
		ec_deque_##NAME##_node_t* temp = d->head;                                              \
		d->head = temp->next;                                                                  \
		if (d->head)                                                                           \
			d->head->prev = NULL;                                                              \
		else                                                                                   \
			d->tail = NULL;                                                                    \
		d->size--;                                                                             \
		*out = EC_MALLOC(sizeof(temp->data));                                                  \
		if (*out)                                                                              \
			{                                                                                  \
				EC_MEMCPY_SAFE(*out, &temp->data, sizeof(temp->data));                         \
			}                                                                                  \
		EC_FREE(temp);                                                                         \
		EC_MUTEX_UNLOCK(d->lock);                                                              \
		return EC_OK;                                                                          \
	}                                                                                          \
                                                                                               \
	static inline ec_status_t ec_deque_##NAME##_pop_back(ec_deque_##NAME##_t* d, void** out)   \
	{                                                                                          \
		if (!d || !out)                                                                        \
			return EC_ERR_INVALID_ARG;                                                         \
		EC_MUTEX_LOCK(d->lock);                                                                \
		if (!d->tail)                                                                          \
			{                                                                                  \
				EC_MUTEX_UNLOCK(d->lock);                                                      \
				return EC_ERR_OUT_OF_BOUNDS;                                                   \
			}                                                                                  \
		ec_deque_##NAME##_node_t* temp = d->tail;                                              \
		d->tail = temp->prev;                                                                  \
		if (d->tail)                                                                           \
			d->tail->next = NULL;                                                              \
		else                                                                                   \
			d->head = NULL;                                                                    \
		d->size--;                                                                             \
		*out = EC_MALLOC(sizeof(temp->data));                                                  \
		if (*out)                                                                              \
			{                                                                                  \
				EC_MEMCPY_SAFE(*out, &temp->data, sizeof(temp->data));                         \
			}                                                                                  \
		EC_FREE(temp);                                                                         \
		EC_MUTEX_UNLOCK(d->lock);                                                              \
		return EC_OK;                                                                          \
	}                                                                                          \
	static inline ec_status_t ec_deque_##NAME##_peek_front(ec_deque_##NAME##_t* d, void** out) \
	{                                                                                          \
		if (!d || !out)                                                                        \
			return EC_ERR_INVALID_ARG;                                                         \
		EC_MUTEX_LOCK(d->lock);                                                                \
		if (!d->head)                                                                          \
			{                                                                                  \
				EC_MUTEX_UNLOCK(d->lock);                                                      \
				return EC_ERR_OUT_OF_BOUNDS;                                                   \
			}                                                                                  \
		*out = EC_MALLOC(sizeof(d->head->data));                                               \
		if (*out)                                                                              \
			{                                                                                  \
				EC_MEMCPY_SAFE(*out, &d->head->data, sizeof(d->head->data));                   \
			}                                                                                  \
		EC_MUTEX_UNLOCK(d->lock);                                                              \
		return EC_OK;                                                                          \
	}                                                                                          \
                                                                                               \
	static inline ec_status_t ec_deque_##NAME##_peek_back(ec_deque_##NAME##_t* d, void** out)  \
	{                                                                                          \
		if (!d || !out)                                                                        \
			return EC_ERR_INVALID_ARG;                                                         \
		EC_MUTEX_LOCK(d->lock);                                                                \
		if (!d->tail)                                                                          \
			{                                                                                  \
				EC_MUTEX_UNLOCK(d->lock);                                                      \
				return EC_ERR_OUT_OF_BOUNDS;                                                   \
			}                                                                                  \
		*out = EC_MALLOC(sizeof(d->tail->data));                                               \
		if (*out)                                                                              \
			{                                                                                  \
				EC_MEMCPY_SAFE(*out, &d->tail->data, sizeof(d->tail->data));                   \
			}                                                                                  \
		EC_MUTEX_UNLOCK(d->lock);                                                              \
		return EC_OK;                                                                          \
	}                                                                                          \
	static inline size_t ec_deque_##NAME##_size(ec_deque_##NAME##_t* d)                        \
	{                                                                                          \
		EC_MUTEX_LOCK(d->lock);                                                                \
		size_t s = d->size;                                                                    \
		EC_MUTEX_UNLOCK(d->lock);                                                              \
		return s;                                                                              \
	}                                                                                          \
                                                                                               \
	static inline ec_status_t ec_deque_##NAME##_free(ec_deque_##NAME##_t* d)                   \
	{                                                                                          \
		EC_MUTEX_LOCK(d->lock);                                                                \
		ec_deque_##NAME##_node_t* cur = d->head;                                               \
		while (cur)                                                                            \
			{                                                                                  \
				ec_deque_##NAME##_node_t* next = cur->next;                                    \
				EC_FREE(cur);                                                                  \
				cur = next;                                                                    \
			}                                                                                  \
		d->head = d->tail = NULL;                                                              \
		d->size = 0;                                                                           \
		EC_MUTEX_UNLOCK(d->lock);                                                              \
		return EC_OK;                                                                          \
	}                                                                                          \
	static inline ec_status_t ec_deque_##NAME##_init(ec_deque_##NAME##_t* d, void* mempool)    \
	{                                                                                          \
		if (!d)                                                                                \
			return EC_ERR_INVALID_ARG;                                                         \
		EC_MUTEX_INIT(d->lock);                                                                \
		d->head = d->tail = NULL;                                                              \
		d->size = 0;                                                                           \
		d->mempool = mempool;                                                                  \
		d->push_front = ec_deque_##NAME##_push_front;                                          \
		d->push_back = ec_deque_##NAME##_push_back;                                            \
		d->pop_front = ec_deque_##NAME##_pop_front;                                            \
		d->pop_back = ec_deque_##NAME##_pop_back;                                              \
		d->peek_front = ec_deque_##NAME##_peek_front;                                          \
		d->peek_back = ec_deque_##NAME##_peek_back;                                            \
		d->size_fn = ec_deque_##NAME##_size;                                                   \
		d->free_fn = ec_deque_##NAME##_free;                                                   \
		return EC_OK;                                                                          \
	}
EC_DEQUE_DEF(int, int)
EC_DEQUE_DEF(unsigned int, uint)
EC_DEQUE_DEF(long, long)
EC_DEQUE_DEF(long long, ll)
EC_DEQUE_DEF(unsigned long, ul)
EC_DEQUE_DEF(unsigned long long, ull)
EC_DEQUE_DEF(uint32_t, u32)
EC_DEQUE_DEF(uint64_t, u64)
EC_DEQUE_DEF(double, dbl)
EC_DEQUE_DEF(const char*, str)
EC_DEQUE_DEF(void*, ptr)
#endif /* EC_DEQUE_H */
