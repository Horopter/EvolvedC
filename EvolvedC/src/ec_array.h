#ifndef EC_ARRAY_H
#define EC_ARRAY_H

#include "ec_common.h"
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

/*---------------------------------------------------------------------------
  Helper Macros for Mempool Usage
---------------------------------------------------------------------------*/
#ifdef EC_USE_MEMPOOL
/* Allocate memory using the mempool if available; otherwise, fall back to standard allocation. */
#define EC_ARRAY_ALLOCATE(TYPE, CAPACITY, MEMPOOL)                                            \
	((MEMPOOL) ? (TYPE*)ec_mempool_alloc((ec_mempool_t*)(MEMPOOL), (CAPACITY) * sizeof(TYPE)) \
	           : (TYPE*)EC_MALLOC((CAPACITY) * sizeof(TYPE)))
/* Free memory using the mempool free function. */
#define EC_ARRAY_FREE(PTR, MEMPOOL) ec_mempool_free((ec_mempool_t*)(MEMPOOL), (PTR))
#else
#define EC_ARRAY_ALLOCATE(TYPE, CAPACITY, MEMPOOL) (TYPE*)EC_MALLOC((CAPACITY) * sizeof(TYPE))
#define EC_ARRAY_FREE(PTR, MEMPOOL) EC_FREE(PTR)
#endif

/*---------------------------------------------------------------------------
  EC_ARRAY_DEF(TYPE, NAME)

  Defines a static-capacity array type for the given TYPE.
  The array is created with a fixed capacity. Elements can be appended
  via push (up to capacity) and later accessed or updated.
---------------------------------------------------------------------------*/
#define EC_ARRAY_DEF(TYPE, NAME)                                                                                \
	typedef struct ec_array_##NAME##_s                                                                          \
	{                                                                                                           \
		TYPE* data;         /* Pointer to the array's data */                                                   \
		size_t size;        /* Number of elements currently stored */                                           \
		size_t capacity;    /* Fixed capacity (max elements) */                                                 \
		EC_MUTEX_TYPE lock; /* Mutex for thread-safety */                                                       \
		void* mempool;      /* Optional mempool for memory management */                                        \
                                                                                                                \
		/* Function pointers for OOP-like syntax */                                                             \
		ec_status_t (*push)(struct ec_array_##NAME##_s*, TYPE);                                                 \
		ec_status_t (*set)(struct ec_array_##NAME##_s*, size_t, TYPE);                                          \
		ec_status_t (*get)(struct ec_array_##NAME##_s*, size_t, TYPE*);                                         \
		void (*free)(struct ec_array_##NAME##_s*);                                                              \
	} ec_array_##NAME##_t;                                                                                      \
                                                                                                                \
	static inline ec_status_t ec_array_##NAME##_push(ec_array_##NAME##_t* self, TYPE value)                     \
	{                                                                                                           \
		if (!self)                                                                                              \
			{                                                                                                   \
				return EC_ERR_INVALID_ARG;                                                                      \
			}                                                                                                   \
		EC_MUTEX_LOCK(self->lock);                                                                              \
		if (self->size >= self->capacity)                                                                       \
			{                                                                                                   \
				EC_MUTEX_UNLOCK(self->lock);                                                                    \
				return EC_ERR_MAX_CAPACITY;                                                                     \
			}                                                                                                   \
		self->data[self->size++] = value;                                                                       \
		EC_MUTEX_UNLOCK(self->lock);                                                                            \
		return EC_OK;                                                                                           \
	}                                                                                                           \
                                                                                                                \
	static inline ec_status_t ec_array_##NAME##_set(ec_array_##NAME##_t* self, size_t index, TYPE value)        \
	{                                                                                                           \
		if (!self || index >= self->size)                                                                       \
			{                                                                                                   \
				return EC_ERR_OUT_OF_BOUNDS;                                                                    \
			}                                                                                                   \
		EC_MUTEX_LOCK(self->lock);                                                                              \
		self->data[index] = value;                                                                              \
		EC_MUTEX_UNLOCK(self->lock);                                                                            \
		return EC_OK;                                                                                           \
	}                                                                                                           \
                                                                                                                \
	static inline ec_status_t ec_array_##NAME##_get(ec_array_##NAME##_t* self, size_t index, TYPE* out)         \
	{                                                                                                           \
		if (!self || !out || index >= self->size)                                                               \
			{                                                                                                   \
				return EC_ERR_OUT_OF_BOUNDS;                                                                    \
			}                                                                                                   \
		EC_MUTEX_LOCK(self->lock);                                                                              \
		*out = self->data[index];                                                                               \
		EC_MUTEX_UNLOCK(self->lock);                                                                            \
		return EC_OK;                                                                                           \
	}                                                                                                           \
                                                                                                                \
	static inline void ec_array_##NAME##_free(ec_array_##NAME##_t* self)                                        \
	{                                                                                                           \
		if (!self)                                                                                              \
			{                                                                                                   \
				return;                                                                                         \
			}                                                                                                   \
		EC_MUTEX_LOCK(self->lock);                                                                              \
		if (self->data)                                                                                         \
			{                                                                                                   \
				EC_ARRAY_FREE(self->data, self->mempool);                                                       \
				self->data = NULL;                                                                              \
			}                                                                                                   \
		self->size = 0;                                                                                         \
		self->capacity = 0;                                                                                     \
		EC_MUTEX_UNLOCK(self->lock);                                                                            \
		EC_MUTEX_DESTROY(self->lock);                                                                           \
	}                                                                                                           \
                                                                                                                \
	static inline void ec_array_##NAME##_set_functions(ec_array_##NAME##_t* self)                               \
	{                                                                                                           \
		do                                                                                                      \
			{                                                                                                   \
				self->push = ec_array_##NAME##_push;                                                            \
				self->set = ec_array_##NAME##_set;                                                              \
				self->get = ec_array_##NAME##_get;                                                              \
				self->free = ec_array_##NAME##_free;                                                            \
			}                                                                                                   \
		while (0);                                                                                              \
	}                                                                                                           \
                                                                                                                \
	static inline ec_status_t ec_array_##NAME##_init(ec_array_##NAME##_t* self, size_t capacity, void* mempool) \
	{                                                                                                           \
		if (!self || capacity == 0)                                                                             \
			{                                                                                                   \
				return EC_ERR_INVALID_ARG;                                                                      \
			}                                                                                                   \
		EC_CHECK_MEMPOOL(mempool);                                                                              \
		self->capacity = capacity;                                                                              \
		self->size = 0;                                                                                         \
		self->mempool = mempool;                                                                                \
		EC_MUTEX_INIT(self->lock);                                                                              \
		self->data = EC_ARRAY_ALLOCATE(TYPE, capacity, self->mempool);                                          \
		if (!self->data)                                                                                        \
			{                                                                                                   \
				EC_MUTEX_DESTROY(self->lock);                                                                   \
				return EC_ERR_ALLOC;                                                                            \
			}                                                                                                   \
		ec_array_##NAME##_set_functions(self);                                                                  \
		return EC_OK;                                                                                           \
	}

/*---------------------------------------------------------------------------
  Provide Typical Type Definitions using the Static Array Interface.
---------------------------------------------------------------------------*/
EC_ARRAY_DEF(int, int)
EC_ARRAY_DEF(unsigned int, uint)
EC_ARRAY_DEF(long, long)
EC_ARRAY_DEF(long long, ll)
EC_ARRAY_DEF(unsigned long, ulong)
EC_ARRAY_DEF(unsigned long long, ull)
EC_ARRAY_DEF(uint32_t, u32)
EC_ARRAY_DEF(uint64_t, u64)
EC_ARRAY_DEF(double, double)
EC_ARRAY_DEF(const char*, str)
EC_ARRAY_DEF(void*, ptr)

#endif /* EC_ARRAY_H */
