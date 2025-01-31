#ifndef EC_VECTOR_H
#define EC_VECTOR_H

#include "ec_common.h" /* For EC_MUTEX_TYPE, EC_MUTEX_LOCK, etc. */
#include "ec_status.h" /* For ec_status_t definitions like EC_OK, EC_ERR_ALLOC, etc. */

#ifndef EC_ARRAY_GROWTH_FACTOR
#define EC_ARRAY_GROWTH_FACTOR 2
#endif

#ifdef EC_USE_MEMPOOL
/* Use mempool allocation if enabled */
#define EC_VECTOR_ALLOCATE(TYPE, CAP, MEMPOOL)                                             \
    ((MEMPOOL) ? (TYPE *)ec_mempool_alloc((ec_mempool_t *)(MEMPOOL), (CAP) * sizeof(TYPE)) \
               : (TYPE *)EC_MALLOC((CAP) * sizeof(TYPE)))
#else
#define EC_VECTOR_ALLOCATE(TYPE, CAP, MEMPOOL) (TYPE *)EC_MALLOC((CAP) * sizeof(TYPE))
#endif

/* When mempool support is disabled, ensure that no non-NULL mempool pointer is passed. */
#ifndef EC_USE_MEMPOOL
#define EC_CHECK_MEMPOOL(MEMPOOL)      \
    do                                 \
    {                                  \
        if ((MEMPOOL) != NULL)         \
        {                              \
            return EC_ERR_INVALID_ARG; \
        }                              \
    } while (0)
#else
#define EC_ARRAY_CHECK_MEMPOOL(MEMPOOL) \
    do                                  \
    {                                   \
    } while (0)
#endif

/*
 * EC_VECTOR_DEF(TYPE, NAME)
 *
 * Defines a dynamic vector type for the given TYPE.
 * The vector is automatically expanded using EC_ARRAY_GROWTH_FACTOR when needed.
 * It provides a wide variety of operations (push, insert, remove, get, replace, swap,
 * search, sort, reverse, map, reduce, subarray, copy, filter, etc.) as well as iterator support.
 */
#define EC_VECTOR_DEF(TYPE, NAME)                                                                                                                             \
                                                                                                                                                              \
    /* Iterator types for this vector */                                                                                                                      \
    typedef struct ec_vector_##NAME##_iter_s                                                                                                                  \
    {                                                                                                                                                         \
        struct ec_vector_##NAME##_s *vec;                                                                                                                     \
        size_t index;                                                                                                                                         \
        bool last_removed;                                                                                                                                    \
    } ec_vector_##NAME##_iter_t;                                                                                                                              \
                                                                                                                                                              \
    typedef struct ec_vector_##NAME##_zip_iter_s                                                                                                              \
    {                                                                                                                                                         \
        struct ec_vector_##NAME##_s *vec1;                                                                                                                    \
        struct ec_vector_##NAME##_s *vec2;                                                                                                                    \
        size_t index;                                                                                                                                         \
        bool last_removed;                                                                                                                                    \
    } ec_vector_##NAME##_zip_iter_t;                                                                                                                          \
                                                                                                                                                              \
    /* Forward-declare the vector struct */                                                                                                                   \
    typedef struct ec_vector_##NAME##_s ec_vector_##NAME##_t;                                                                                                 \
    static inline void ec_vector_##NAME##_init_functions(ec_vector_##NAME##_t *self);                                                                         \
                                                                                                                                                              \
    typedef struct ec_vector_##NAME##_s                                                                                                                       \
    {                                                                                                                                                         \
        TYPE *data;         /* Pointer to the vector's data */                                                                                                \
        size_t size;        /* Number of elements currently stored */                                                                                         \
        size_t capacity;    /* Current capacity of the vector */                                                                                              \
        EC_MUTEX_TYPE lock; /* Mutex for thread safety */                                                                                                     \
        void *mempool;      /* Optional mempool for memory management */                                                                                      \
                                                                                                                                                              \
        /* OOP–like function pointers */                                                                                                                      \
        ec_status_t (*push)(ec_vector_##NAME##_t * self, TYPE val);                                                                                           \
        ec_status_t (*insert)(ec_vector_##NAME##_t * self, size_t index, TYPE val);                                                                           \
        ec_status_t (*remove_at)(ec_vector_##NAME##_t * self, size_t index);                                                                                  \
        ec_status_t (*get)(ec_vector_##NAME##_t * self, size_t index, TYPE *out);                                                                             \
        ec_status_t (*replace_at)(ec_vector_##NAME##_t * self, size_t index, TYPE val, TYPE *old);                                                            \
        ec_status_t (*swap)(ec_vector_##NAME##_t * self, size_t idx1, size_t idx2);                                                                           \
                                                                                                                                                              \
        size_t (*contains)(ec_vector_##NAME##_t * self, TYPE val, int (*cmp)(const void *, const void *));                                                    \
        ec_status_t (*index_of)(ec_vector_##NAME##_t * self, TYPE val, size_t *out_index, int (*cmp)(const void *, const void *));                            \
        ec_status_t (*binary_search)(ec_vector_##NAME##_t * self, TYPE val, int (*cmp)(const void *, const void *), size_t *out_index);                       \
                                                                                                                                                              \
        void (*sort)(ec_vector_##NAME##_t * self, int (*cmp)(const void *, const void *));                                                                    \
        void (*reverse)(ec_vector_##NAME##_t * self);                                                                                                         \
        void (*map)(ec_vector_##NAME##_t * self, void (*fn)(TYPE *));                                                                                         \
        void (*reduce)(ec_vector_##NAME##_t * self, void (*fn)(TYPE *, TYPE *, TYPE *), TYPE *result);                                                        \
                                                                                                                                                              \
        ec_status_t (*subarray)(ec_vector_##NAME##_t * self, size_t from, size_t to, ec_vector_##NAME##_t *out);                                              \
        ec_status_t (*copy_shallow)(ec_vector_##NAME##_t * self, ec_vector_##NAME##_t *out);                                                                  \
        ec_status_t (*copy_deep)(ec_vector_##NAME##_t * self, TYPE (*cp)(TYPE *), ec_vector_##NAME##_t *out);                                                 \
        ec_status_t (*filter)(ec_vector_##NAME##_t * self, bool (*predicate)(const TYPE), ec_vector_##NAME##_t *out);                                         \
        ec_status_t (*filter_mut)(ec_vector_##NAME##_t * self, bool (*predicate)(const TYPE));                                                                \
                                                                                                                                                              \
        size_t (*size_fn)(ec_vector_##NAME##_t * self);                                                                                                       \
        size_t (*capacity_fn)(ec_vector_##NAME##_t * self);                                                                                                   \
        ec_status_t (*trim_capacity)(ec_vector_##NAME##_t * self);                                                                                            \
        ec_status_t (*free_fn)(ec_vector_##NAME##_t * self);                                                                                                  \
    };                                                                                                                                                        \
                                                                                                                                                              \
    /* Helper: Expand the vector capacity */                                                                                                                  \
    static inline ec_status_t ec_vector_##NAME##_expand(ec_vector_##NAME##_t *self)                                                                           \
    {                                                                                                                                                         \
        size_t new_cap = self->capacity * EC_ARRAY_GROWTH_FACTOR;                                                                                             \
        TYPE *new_data = (TYPE *)EC_REALLOC(self->data, new_cap * sizeof(TYPE));                                                                              \
        if (new_data == NULL)                                                                                                                                 \
        {                                                                                                                                                     \
            return EC_ERR_ALLOC;                                                                                                                              \
        }                                                                                                                                                     \
        self->data = new_data;                                                                                                                                \
        self->capacity = new_cap;                                                                                                                             \
        return EC_OK;                                                                                                                                         \
    }                                                                                                                                                         \
                                                                                                                                                              \
    /* Initialization and function pointer setter */                                                                                                          \
    static inline ec_status_t ec_vector_##NAME##_init(ec_vector_##NAME##_t *self, size_t capacity, void *mempool)                                             \
    {                                                                                                                                                         \
        if (!self || capacity == 0)                                                                                                                           \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        EC_CHECK_MEMPOOL(mempool);                                                                                                                            \
        self->mempool = mempool;                                                                                                                              \
                                                                                                                                                              \
        self->size = 0;                                                                                                                                       \
        self->capacity = capacity;                                                                                                                            \
        EC_MUTEX_INIT(self->lock);                                                                                                                            \
        self->data = EC_VECTOR_ALLOCATE(TYPE, capacity, self->mempool);                                                                                       \
        if (!self->data)                                                                                                                                      \
        {                                                                                                                                                     \
            EC_MUTEX_DESTROY(self->lock);                                                                                                                     \
            return EC_ERR_ALLOC;                                                                                                                              \
        }                                                                                                                                                     \
        ec_vector_##NAME##_init_functions(self);                                                                                                              \
        return EC_OK;                                                                                                                                         \
    }                                                                                                                                                         \
                                                                                                                                                              \
    /* Locking helpers */                                                                                                                                     \
    static inline void ec_vector_##NAME##_lock(ec_vector_##NAME##_t *self)                                                                                    \
    {                                                                                                                                                         \
        EC_MUTEX_LOCK(self->lock);                                                                                                                            \
    }                                                                                                                                                         \
    static inline void ec_vector_##NAME##_unlock(ec_vector_##NAME##_t *self)                                                                                  \
    {                                                                                                                                                         \
        EC_MUTEX_UNLOCK(self->lock);                                                                                                                          \
    }                                                                                                                                                         \
                                                                                                                                                              \
    /* Operations */                                                                                                                                          \
    static inline ec_status_t ec_vector_##NAME##_push(ec_vector_##NAME##_t *self, TYPE value)                                                                 \
    {                                                                                                                                                         \
        if (!self)                                                                                                                                            \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        if (self->size >= self->capacity)                                                                                                                     \
        {                                                                                                                                                     \
            ec_status_t st = ec_vector_##NAME##_expand(self);                                                                                                 \
            if (st != EC_OK)                                                                                                                                  \
            {                                                                                                                                                 \
                ec_vector_##NAME##_unlock(self);                                                                                                              \
                return st;                                                                                                                                    \
            }                                                                                                                                                 \
        }                                                                                                                                                     \
        self->data[self->size++] = value;                                                                                                                     \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
        return EC_OK;                                                                                                                                         \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline ec_status_t ec_vector_##NAME##_insert(ec_vector_##NAME##_t *self, size_t index, TYPE value)                                                 \
    {                                                                                                                                                         \
        if (!self)                                                                                                                                            \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        if (index > self->size)                                                                                                                               \
        {                                                                                                                                                     \
            ec_vector_##NAME##_unlock(self);                                                                                                                  \
            return EC_ERR_OUT_OF_BOUNDS;                                                                                                                      \
        }                                                                                                                                                     \
        if (self->size >= self->capacity)                                                                                                                     \
        {                                                                                                                                                     \
            ec_status_t st = ec_vector_##NAME##_expand(self);                                                                                                 \
            if (st != EC_OK)                                                                                                                                  \
            {                                                                                                                                                 \
                ec_vector_##NAME##_unlock(self);                                                                                                              \
                return st;                                                                                                                                    \
            }                                                                                                                                                 \
        }                                                                                                                                                     \
        memmove(&self->data[index + 1], &self->data[index], (self->size - index) * sizeof(TYPE));                                                             \
        self->data[index] = value;                                                                                                                            \
        self->size++;                                                                                                                                         \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
        return EC_OK;                                                                                                                                         \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline ec_status_t ec_vector_##NAME##_remove_at(ec_vector_##NAME##_t *self, size_t index)                                                          \
    {                                                                                                                                                         \
        if (!self)                                                                                                                                            \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        if (index >= self->size)                                                                                                                              \
        {                                                                                                                                                     \
            ec_vector_##NAME##_unlock(self);                                                                                                                  \
            return EC_ERR_OUT_OF_BOUNDS;                                                                                                                      \
        }                                                                                                                                                     \
        memmove(&self->data[index], &self->data[index + 1], ((self->size - index - 1)) * sizeof(TYPE));                                                       \
        self->size--;                                                                                                                                         \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
        return EC_OK;                                                                                                                                         \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline ec_status_t ec_vector_##NAME##_get(ec_vector_##NAME##_t *self, size_t index, TYPE *out)                                                     \
    {                                                                                                                                                         \
        if (!self || !out)                                                                                                                                    \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        if (index >= self->size)                                                                                                                              \
        {                                                                                                                                                     \
            ec_vector_##NAME##_unlock(self);                                                                                                                  \
            return EC_ERR_OUT_OF_BOUNDS;                                                                                                                      \
        }                                                                                                                                                     \
        *out = self->data[index];                                                                                                                             \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
        return EC_OK;                                                                                                                                         \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline ec_status_t ec_vector_##NAME##_replace_at(ec_vector_##NAME##_t *self, size_t index, TYPE val, TYPE *old)                                    \
    {                                                                                                                                                         \
        if (!self || !old)                                                                                                                                    \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        if (index >= self->size)                                                                                                                              \
        {                                                                                                                                                     \
            ec_vector_##NAME##_unlock(self);                                                                                                                  \
            return EC_ERR_OUT_OF_BOUNDS;                                                                                                                      \
        }                                                                                                                                                     \
        *old = self->data[index];                                                                                                                             \
        self->data[index] = val;                                                                                                                              \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
        return EC_OK;                                                                                                                                         \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline ec_status_t ec_vector_##NAME##_swap(ec_vector_##NAME##_t *self, size_t idx1, size_t idx2)                                                   \
    {                                                                                                                                                         \
        if (!self)                                                                                                                                            \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        if (idx1 >= self->size || idx2 >= self->size)                                                                                                         \
        {                                                                                                                                                     \
            ec_vector_##NAME##_unlock(self);                                                                                                                  \
            return EC_ERR_OUT_OF_BOUNDS;                                                                                                                      \
        }                                                                                                                                                     \
        TYPE temp = self->data[idx1];                                                                                                                         \
        self->data[idx1] = self->data[idx2];                                                                                                                  \
        self->data[idx2] = temp;                                                                                                                              \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
        return EC_OK;                                                                                                                                         \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline size_t ec_vector_##NAME##_contains(ec_vector_##NAME##_t *self, TYPE val, int (*cmp)(const void *, const void *))                            \
    {                                                                                                                                                         \
        if (!self || !cmp)                                                                                                                                    \
            return 0;                                                                                                                                         \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        for (size_t i = 0; i < self->size; i++)                                                                                                               \
        {                                                                                                                                                     \
            if (cmp(&self->data[i], &val) == 0)                                                                                                               \
            {                                                                                                                                                 \
                ec_vector_##NAME##_unlock(self);                                                                                                              \
                return 1;                                                                                                                                     \
            }                                                                                                                                                 \
        }                                                                                                                                                     \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
        return 0;                                                                                                                                             \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline ec_status_t ec_vector_##NAME##_index_of(ec_vector_##NAME##_t *self, TYPE val, size_t *out_idx, int (*cmp)(const void *, const void *))      \
    {                                                                                                                                                         \
        if (!self || !out_idx || !cmp)                                                                                                                        \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        for (size_t i = 0; i < self->size; i++)                                                                                                               \
        {                                                                                                                                                     \
            if (cmp(&self->data[i], &val) == 0)                                                                                                               \
            {                                                                                                                                                 \
                *out_idx = i;                                                                                                                                 \
                ec_vector_##NAME##_unlock(self);                                                                                                              \
                return EC_OK;                                                                                                                                 \
            }                                                                                                                                                 \
        }                                                                                                                                                     \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
        return EC_ERR_NOT_FOUND;                                                                                                                              \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline ec_status_t ec_vector_##NAME##_binary_search(ec_vector_##NAME##_t *self, TYPE val, int (*cmp)(const void *, const void *), size_t *out_idx) \
    {                                                                                                                                                         \
        if (!self || !cmp || !out_idx)                                                                                                                        \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        TYPE *res = (TYPE *)bsearch(&val, self->data, self->size, sizeof(TYPE), cmp);                                                                         \
        if (!res)                                                                                                                                             \
        {                                                                                                                                                     \
            ec_vector_##NAME##_unlock(self);                                                                                                                  \
            return EC_ERR_NOT_FOUND;                                                                                                                          \
        }                                                                                                                                                     \
        *out_idx = (size_t)(res - self->data);                                                                                                                \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
        return EC_OK;                                                                                                                                         \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline void ec_vector_##NAME##_sort(ec_vector_##NAME##_t *self, int (*cmp)(const void *, const void *))                                            \
    {                                                                                                                                                         \
        if (!self || !cmp)                                                                                                                                    \
            return;                                                                                                                                           \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        qsort(self->data, self->size, sizeof(TYPE), cmp);                                                                                                     \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline void ec_vector_##NAME##_reverse(ec_vector_##NAME##_t *self)                                                                                 \
    {                                                                                                                                                         \
        if (!self)                                                                                                                                            \
            return;                                                                                                                                           \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        size_t left = 0, right = (self->size > 0 ? self->size - 1 : 0);                                                                                       \
        while (left < right)                                                                                                                                  \
        {                                                                                                                                                     \
            TYPE tmp = self->data[left];                                                                                                                      \
            self->data[left] = self->data[right];                                                                                                             \
            self->data[right] = tmp;                                                                                                                          \
            left++;                                                                                                                                           \
            right--;                                                                                                                                          \
        }                                                                                                                                                     \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline void ec_vector_##NAME##_map(ec_vector_##NAME##_t *self, void (*fn)(TYPE *))                                                                 \
    {                                                                                                                                                         \
        if (!self || !fn)                                                                                                                                     \
            return;                                                                                                                                           \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        for (size_t i = 0; i < self->size; i++)                                                                                                               \
        {                                                                                                                                                     \
            fn(&self->data[i]);                                                                                                                               \
        }                                                                                                                                                     \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline void ec_vector_##NAME##_reduce(ec_vector_##NAME##_t *self, void (*fn)(TYPE *, TYPE *, TYPE *), TYPE *result)                                \
    {                                                                                                                                                         \
        if (!self || !fn || !result)                                                                                                                          \
            return;                                                                                                                                           \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        if (self->size == 0)                                                                                                                                  \
        {                                                                                                                                                     \
            ec_vector_##NAME##_unlock(self);                                                                                                                  \
            return;                                                                                                                                           \
        }                                                                                                                                                     \
        *result = self->data[0];                                                                                                                              \
        for (size_t i = 1; i < self->size; i++)                                                                                                               \
        {                                                                                                                                                     \
            fn(result, &self->data[i], result);                                                                                                               \
        }                                                                                                                                                     \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline ec_status_t ec_vector_##NAME##_subarray(ec_vector_##NAME##_t *self, size_t from, size_t to, ec_vector_##NAME##_t *out)                      \
    {                                                                                                                                                         \
        if (!self || !out || from > to)                                                                                                                       \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        if (to > self->size)                                                                                                                                  \
        {                                                                                                                                                     \
            ec_vector_##NAME##_unlock(self);                                                                                                                  \
            return EC_ERR_OUT_OF_BOUNDS;                                                                                                                      \
        }                                                                                                                                                     \
        ec_status_t st = ec_vector_##NAME##_init(out, to - from, self->mempool);                                                                              \
        if (st != EC_OK)                                                                                                                                      \
        {                                                                                                                                                     \
            ec_vector_##NAME##_unlock(self);                                                                                                                  \
            return st;                                                                                                                                        \
        }                                                                                                                                                     \
        memcpy(out->data, &self->data[from], (to - from) * sizeof(TYPE));                                                                                     \
        out->size = to - from;                                                                                                                                \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
        return EC_OK;                                                                                                                                         \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline ec_status_t ec_vector_##NAME##_copy_shallow(ec_vector_##NAME##_t *self, ec_vector_##NAME##_t *out)                                          \
    {                                                                                                                                                         \
        if (!self || !out)                                                                                                                                    \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        ec_status_t st = ec_vector_##NAME##_init(out, self->capacity, self->mempool);                                                                         \
        if (st == EC_OK)                                                                                                                                      \
        {                                                                                                                                                     \
            memcpy(out->data, self->data, self->size * sizeof(TYPE));                                                                                         \
            out->size = self->size;                                                                                                                           \
        }                                                                                                                                                     \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
        return st;                                                                                                                                            \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline ec_status_t ec_vector_##NAME##_copy_deep(ec_vector_##NAME##_t *self, TYPE (*cp)(TYPE *), ec_vector_##NAME##_t *out)                         \
    {                                                                                                                                                         \
        if (!self || !cp || !out)                                                                                                                             \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        ec_status_t st = ec_vector_##NAME##_init(out, self->capacity, self->mempool);                                                                         \
        if (st == EC_OK)                                                                                                                                      \
        {                                                                                                                                                     \
            for (size_t i = 0; i < self->size; i++)                                                                                                           \
            {                                                                                                                                                 \
                out->data[i] = cp(&self->data[i]);                                                                                                            \
            }                                                                                                                                                 \
            out->size = self->size;                                                                                                                           \
        }                                                                                                                                                     \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
        return st;                                                                                                                                            \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline ec_status_t ec_vector_##NAME##_filter(ec_vector_##NAME##_t *self, bool (*predicate)(const TYPE), ec_vector_##NAME##_t *out)                 \
    {                                                                                                                                                         \
        if (!self || !predicate || !out)                                                                                                                      \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        ec_status_t st = ec_vector_##NAME##_init(out, self->capacity, self->mempool);                                                                         \
        if (st == EC_OK)                                                                                                                                      \
        {                                                                                                                                                     \
            for (size_t i = 0; i < self->size; i++)                                                                                                           \
            {                                                                                                                                                 \
                if (predicate(self->data[i]))                                                                                                                 \
                {                                                                                                                                             \
                    out->data[out->size++] = self->data[i];                                                                                                   \
                }                                                                                                                                             \
            }                                                                                                                                                 \
        }                                                                                                                                                     \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
        return st;                                                                                                                                            \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline ec_status_t ec_vector_##NAME##_filter_mut(ec_vector_##NAME##_t *self, bool (*predicate)(const TYPE))                                        \
    {                                                                                                                                                         \
        if (!self || !predicate)                                                                                                                              \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        size_t write_idx = 0;                                                                                                                                 \
        for (size_t i = 0; i < self->size; i++)                                                                                                               \
        {                                                                                                                                                     \
            if (predicate(self->data[i]))                                                                                                                     \
            {                                                                                                                                                 \
                self->data[write_idx++] = self->data[i];                                                                                                      \
            }                                                                                                                                                 \
        }                                                                                                                                                     \
        self->size = write_idx;                                                                                                                               \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
        return EC_OK;                                                                                                                                         \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline size_t ec_vector_##NAME##_size(ec_vector_##NAME##_t *self)                                                                                  \
    {                                                                                                                                                         \
        if (!self)                                                                                                                                            \
            return 0;                                                                                                                                         \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        size_t sz = self->size;                                                                                                                               \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
        return sz;                                                                                                                                            \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline size_t ec_vector_##NAME##_capacity(ec_vector_##NAME##_t *self)                                                                              \
    {                                                                                                                                                         \
        if (!self)                                                                                                                                            \
            return 0;                                                                                                                                         \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        size_t cap = self->capacity;                                                                                                                          \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
        return cap;                                                                                                                                           \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline ec_status_t ec_vector_##NAME##_trim_capacity(ec_vector_##NAME##_t *self)                                                                    \
    {                                                                                                                                                         \
        if (!self)                                                                                                                                            \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        if (self->size == self->capacity)                                                                                                                     \
        {                                                                                                                                                     \
            ec_vector_##NAME##_unlock(self);                                                                                                                  \
            return EC_OK;                                                                                                                                     \
        }                                                                                                                                                     \
        TYPE *new_data = (TYPE *)EC_REALLOC(self->data, self->size * sizeof(TYPE));                                                                           \
        if (new_data == NULL)                                                                                                                                 \
        {                                                                                                                                                     \
            ec_vector_##NAME##_unlock(self);                                                                                                                  \
            return EC_ERR_ALLOC;                                                                                                                              \
        }                                                                                                                                                     \
        self->data = new_data;                                                                                                                                \
        self->capacity = self->size;                                                                                                                          \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
        return EC_OK;                                                                                                                                         \
    }                                                                                                                                                         \
                                                                                                                                                              \
    static inline ec_status_t ec_vector_##NAME##_free(ec_vector_##NAME##_t *self)                                                                             \
    {                                                                                                                                                         \
        if (!self)                                                                                                                                            \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(self);                                                                                                                        \
        if (self->data)                                                                                                                                       \
        {                                                                                                                                                     \
            EC_FREE(self->data);                                                                                                                              \
            self->data = NULL;                                                                                                                                \
        }                                                                                                                                                     \
        self->size = 0;                                                                                                                                       \
        self->capacity = 0;                                                                                                                                   \
        ec_vector_##NAME##_unlock(self);                                                                                                                      \
        EC_MUTEX_DESTROY(self->lock);                                                                                                                         \
        return EC_OK;                                                                                                                                         \
    }                                                                                                                                                         \
                                                                                                                                                              \
    /* Iterator functions */                                                                                                                                  \
    static inline ec_status_t ec_vector_##NAME##_iter_init(ec_vector_##NAME##_iter_t *iter, ec_vector_##NAME##_t *self)                                       \
    {                                                                                                                                                         \
        if (!iter || !self)                                                                                                                                   \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        iter->vec = self;                                                                                                                                     \
        iter->index = 0;                                                                                                                                      \
        iter->last_removed = false;                                                                                                                           \
        return EC_OK;                                                                                                                                         \
    }                                                                                                                                                         \
    static inline ec_status_t ec_vector_##NAME##_iter_next(ec_vector_##NAME##_iter_t *iter, TYPE *out)                                                        \
    {                                                                                                                                                         \
        if (!iter || !out)                                                                                                                                    \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(iter->vec);                                                                                                                   \
        if (iter->index >= iter->vec->size)                                                                                                                   \
        {                                                                                                                                                     \
            ec_vector_##NAME##_unlock(iter->vec);                                                                                                             \
            return EC_ITER_END;                                                                                                                               \
        }                                                                                                                                                     \
        *out = iter->vec->data[iter->index++];                                                                                                                \
        iter->last_removed = false;                                                                                                                           \
        ec_vector_##NAME##_unlock(iter->vec);                                                                                                                 \
        return EC_OK;                                                                                                                                         \
    }                                                                                                                                                         \
    static inline ec_status_t ec_vector_##NAME##_iter_remove(ec_vector_##NAME##_iter_t *iter)                                                                 \
    {                                                                                                                                                         \
        if (!iter)                                                                                                                                            \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(iter->vec);                                                                                                                   \
        if (iter->index == 0 || iter->last_removed)                                                                                                           \
        {                                                                                                                                                     \
            ec_vector_##NAME##_unlock(iter->vec);                                                                                                             \
            return EC_ERR_INVALID_OP;                                                                                                                         \
        }                                                                                                                                                     \
        ec_vector_##NAME##_remove_at(iter->vec, iter->index - 1);                                                                                             \
        iter->index--;                                                                                                                                        \
        iter->last_removed = true;                                                                                                                            \
        ec_vector_##NAME##_unlock(iter->vec);                                                                                                                 \
        return EC_OK;                                                                                                                                         \
    }                                                                                                                                                         \
    static inline ec_status_t ec_vector_##NAME##_iter_add(ec_vector_##NAME##_iter_t *iter, TYPE value)                                                        \
    {                                                                                                                                                         \
        if (!iter)                                                                                                                                            \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(iter->vec);                                                                                                                   \
        ec_status_t st = ec_vector_##NAME##_insert(iter->vec, iter->index, value);                                                                            \
        ec_vector_##NAME##_unlock(iter->vec);                                                                                                                 \
        return st;                                                                                                                                            \
    }                                                                                                                                                         \
    static inline ec_status_t ec_vector_##NAME##_iter_replace(ec_vector_##NAME##_iter_t *iter, TYPE value, TYPE *out)                                         \
    {                                                                                                                                                         \
        if (!iter)                                                                                                                                            \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(iter->vec);                                                                                                                   \
        if (iter->index == 0)                                                                                                                                 \
        {                                                                                                                                                     \
            ec_vector_##NAME##_unlock(iter->vec);                                                                                                             \
            return EC_ERR_INVALID_OP;                                                                                                                         \
        }                                                                                                                                                     \
        ec_status_t st = ec_vector_##NAME##_replace_at(iter->vec, iter->index - 1, value, out);                                                               \
        ec_vector_##NAME##_unlock(iter->vec);                                                                                                                 \
        return st;                                                                                                                                            \
    }                                                                                                                                                         \
    static inline size_t ec_vector_##NAME##_iter_index(ec_vector_##NAME##_iter_t *iter)                                                                       \
    {                                                                                                                                                         \
        return iter ? iter->index : 0;                                                                                                                        \
    }                                                                                                                                                         \
                                                                                                                                                              \
    /* Zip Iterator functions */                                                                                                                              \
    static inline ec_status_t ec_vector_##NAME##_zip_iter_init(ec_vector_##NAME##_zip_iter_t *zip, ec_vector_##NAME##_t *vec1, ec_vector_##NAME##_t *vec2)    \
    {                                                                                                                                                         \
        if (!zip || !vec1 || !vec2)                                                                                                                           \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        zip->vec1 = vec1;                                                                                                                                     \
        zip->vec2 = vec2;                                                                                                                                     \
        zip->index = 0;                                                                                                                                       \
        zip->last_removed = false;                                                                                                                            \
        return EC_OK;                                                                                                                                         \
    }                                                                                                                                                         \
    static inline ec_status_t ec_vector_##NAME##_zip_iter_next(ec_vector_##NAME##_zip_iter_t *zip, TYPE *out1, TYPE *out2)                                    \
    {                                                                                                                                                         \
        if (!zip || !out1 || !out2)                                                                                                                           \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(zip->vec1);                                                                                                                   \
        ec_vector_##NAME##_lock(zip->vec2);                                                                                                                   \
        if (zip->index >= zip->vec1->size || zip->index >= zip->vec2->size)                                                                                   \
        {                                                                                                                                                     \
            ec_vector_##NAME##_unlock(zip->vec2);                                                                                                             \
            ec_vector_##NAME##_unlock(zip->vec1);                                                                                                             \
            return EC_ITER_END;                                                                                                                               \
        }                                                                                                                                                     \
        *out1 = zip->vec1->data[zip->index];                                                                                                                  \
        *out2 = zip->vec2->data[zip->index];                                                                                                                  \
        zip->index++;                                                                                                                                         \
        zip->last_removed = false;                                                                                                                            \
        ec_vector_##NAME##_unlock(zip->vec2);                                                                                                                 \
        ec_vector_##NAME##_unlock(zip->vec1);                                                                                                                 \
        return EC_OK;                                                                                                                                         \
    }                                                                                                                                                         \
    static inline ec_status_t ec_vector_##NAME##_zip_iter_remove(ec_vector_##NAME##_zip_iter_t *zip)                                                          \
    {                                                                                                                                                         \
        if (!zip)                                                                                                                                             \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(zip->vec1);                                                                                                                   \
        ec_vector_##NAME##_lock(zip->vec2);                                                                                                                   \
        if (zip->index == 0 || zip->last_removed)                                                                                                             \
        {                                                                                                                                                     \
            ec_vector_##NAME##_unlock(zip->vec2);                                                                                                             \
            ec_vector_##NAME##_unlock(zip->vec1);                                                                                                             \
            return EC_ERR_INVALID_OP;                                                                                                                         \
        }                                                                                                                                                     \
        ec_vector_##NAME##_remove_at(zip->vec1, zip->index - 1);                                                                                              \
        ec_vector_##NAME##_remove_at(zip->vec2, zip->index - 1);                                                                                              \
        zip->index--;                                                                                                                                         \
        zip->last_removed = true;                                                                                                                             \
        ec_vector_##NAME##_unlock(zip->vec2);                                                                                                                 \
        ec_vector_##NAME##_unlock(zip->vec1);                                                                                                                 \
        return EC_OK;                                                                                                                                         \
    }                                                                                                                                                         \
    static inline ec_status_t ec_vector_##NAME##_zip_iter_add(ec_vector_##NAME##_zip_iter_t *zip, TYPE val1, TYPE val2)                                       \
    {                                                                                                                                                         \
        if (!zip)                                                                                                                                             \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(zip->vec1);                                                                                                                   \
        ec_vector_##NAME##_lock(zip->vec2);                                                                                                                   \
        ec_status_t s1 = ec_vector_##NAME##_insert(zip->vec1, zip->index, val1);                                                                              \
        ec_status_t s2 = ec_vector_##NAME##_insert(zip->vec2, zip->index, val2);                                                                              \
        ec_vector_##NAME##_unlock(zip->vec2);                                                                                                                 \
        ec_vector_##NAME##_unlock(zip->vec1);                                                                                                                 \
        return (s1 == EC_OK && s2 == EC_OK) ? EC_OK : EC_ERR_ALLOC;                                                                                           \
    }                                                                                                                                                         \
    static inline ec_status_t ec_vector_##NAME##_zip_iter_replace(ec_vector_##NAME##_zip_iter_t *zip, TYPE val1, TYPE val2, TYPE *out1, TYPE *out2)           \
    {                                                                                                                                                         \
        if (!zip)                                                                                                                                             \
            return EC_ERR_INVALID_ARG;                                                                                                                        \
        ec_vector_##NAME##_lock(zip->vec1);                                                                                                                   \
        ec_vector_##NAME##_lock(zip->vec2);                                                                                                                   \
        if (zip->index == 0)                                                                                                                                  \
        {                                                                                                                                                     \
            ec_vector_##NAME##_unlock(zip->vec2);                                                                                                             \
            ec_vector_##NAME##_unlock(zip->vec1);                                                                                                             \
            return EC_ERR_INVALID_OP;                                                                                                                         \
        }                                                                                                                                                     \
        ec_status_t s1 = ec_vector_##NAME##_replace_at(zip->vec1, zip->index - 1, val1, out1);                                                                \
        ec_status_t s2 = ec_vector_##NAME##_replace_at(zip->vec2, zip->index - 1, val2, out2);                                                                \
        ec_vector_##NAME##_unlock(zip->vec2);                                                                                                                 \
        ec_vector_##NAME##_unlock(zip->vec1);                                                                                                                 \
        return (s1 == EC_OK && s2 == EC_OK) ? EC_OK : EC_ERR_INVALID_OP;                                                                                      \
    }                                                                                                                                                         \
    static inline size_t ec_vector_##NAME##_zip_iter_index(ec_vector_##NAME##_zip_iter_t *zip)                                                                \
    {                                                                                                                                                         \
        return zip ? zip->index : 0;                                                                                                                          \
    }                                                                                                                                                         \
    static inline void ec_vector_##NAME##_init_functions(ec_vector_##NAME##_t *self)                                                                          \
    {                                                                                                                                                         \
        do                                                                                                                                                    \
        {                                                                                                                                                     \
            self->push = ec_vector_##NAME##_push;                                                                                                             \
            self->insert = ec_vector_##NAME##_insert;                                                                                                         \
            self->remove_at = ec_vector_##NAME##_remove_at;                                                                                                   \
            self->get = ec_vector_##NAME##_get;                                                                                                               \
            self->replace_at = ec_vector_##NAME##_replace_at;                                                                                                 \
            self->swap = ec_vector_##NAME##_swap;                                                                                                             \
            self->contains = ec_vector_##NAME##_contains;                                                                                                     \
            self->index_of = ec_vector_##NAME##_index_of;                                                                                                     \
            self->binary_search = ec_vector_##NAME##_binary_search;                                                                                           \
            self->sort = ec_vector_##NAME##_sort;                                                                                                             \
            self->reverse = ec_vector_##NAME##_reverse;                                                                                                       \
            self->map = ec_vector_##NAME##_map;                                                                                                               \
            self->reduce = ec_vector_##NAME##_reduce;                                                                                                         \
            self->subarray = ec_vector_##NAME##_subarray;                                                                                                     \
            self->copy_shallow = ec_vector_##NAME##_copy_shallow;                                                                                             \
            self->copy_deep = ec_vector_##NAME##_copy_deep;                                                                                                   \
            self->filter = ec_vector_##NAME##_filter;                                                                                                         \
            self->filter_mut = ec_vector_##NAME##_filter_mut;                                                                                                 \
            self->size_fn = ec_vector_##NAME##_size;                                                                                                          \
            self->capacity_fn = ec_vector_##NAME##_capacity;                                                                                                  \
            self->trim_capacity = ec_vector_##NAME##_trim_capacity;                                                                                           \
            self->free_fn = ec_vector_##NAME##_free;                                                                                                          \
        } while (0);                                                                                                                                          \
    }

/* End of EC_VECTOR_DEF macro */

/* Instantiate typical vector types. Notice for double we use "dbl" as the NAME. */
EC_VECTOR_DEF(int, int)
EC_VECTOR_DEF(unsigned int, uint)
EC_VECTOR_DEF(long, long)
EC_VECTOR_DEF(long long, ll)
EC_VECTOR_DEF(unsigned long, ul)
EC_VECTOR_DEF(unsigned long long, ull)
EC_VECTOR_DEF(uint32_t, u32)
EC_VECTOR_DEF(uint64_t, u64)
EC_VECTOR_DEF(double, dbl)
EC_VECTOR_DEF(const char *, str)
EC_VECTOR_DEF(void *, ptr)

#endif /* EC_VECTOR_H */
