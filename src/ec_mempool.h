#ifndef EC_MEMPOOL_H
#define EC_MEMPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "ec_status.h"

/*=========================================================
  1) Cross-Platform Threading
=========================================================*/

#if defined(EC_USE_POSIX)
  #include <pthread.h>

  typedef enum {
      EC_THREAD_UNSAFE = 0,
      EC_THREAD_MUTEX,
      EC_THREAD_SPINLOCK
  } ec_thread_mode_t;

  typedef pthread_mutex_t   ec_mutex_t;
  #define EC_MUTEX_INIT(m)    pthread_mutex_init(&(m), NULL)
  #define EC_MUTEX_LOCK(m)    pthread_mutex_lock(&(m))
  #define EC_MUTEX_UNLOCK(m)  pthread_mutex_unlock(&(m))
  #define EC_MUTEX_DESTROY(m) pthread_mutex_destroy(&(m))

  typedef pthread_spinlock_t ec_spinlock_t;
  #define EC_SPIN_INIT(s)     pthread_spin_init(&(s), PTHREAD_PROCESS_PRIVATE)
  #define EC_SPIN_LOCK(s)     pthread_spin_lock(&(s))
  #define EC_SPIN_UNLOCK(s)   pthread_spin_unlock(&(s))
  #define EC_SPIN_DESTROY(s)  pthread_spin_destroy(&(s))

#else
  /* Fallback => no threading or user handles concurrency */
  typedef enum {
      EC_THREAD_UNSAFE = 0,
      EC_THREAD_MUTEX,
      EC_THREAD_SPINLOCK
  } ec_thread_mode_t;

  typedef char ec_mutex_t;
  #define EC_MUTEX_INIT(m)    ((void)0)
  #define EC_MUTEX_LOCK(m)    ((void)0)
  #define EC_MUTEX_UNLOCK(m)  ((void)0)
  #define EC_MUTEX_DESTROY(m) ((void)0)

  typedef char ec_spinlock_t;
  #define EC_SPIN_INIT(s)     ((void)0)
  #define EC_SPIN_LOCK(s)     ((void)0)
  #define EC_SPIN_UNLOCK(s)   ((void)0)
  #define EC_SPIN_DESTROY(s)  ((void)0)
#endif

/*=========================================================
  2) Pool Type: STATIC vs DYNAMIC
=========================================================*/
typedef enum {
    EC_MEMPOOL_STATIC = 0,
    EC_MEMPOOL_DYNAMIC
} ec_mempool_type_t;

/*=========================================================
  3) Stats & Configuration
=========================================================*/
typedef struct {
    size_t allocations;     
    size_t frees;          
    size_t expansions;     
    size_t high_watermark; 
} ec_mempool_stats_t;

/**
 * ec_mempool_conf_t - user configuration for the pool
 *
 *  - block_size: size in bytes for each block
 *  - num_blocks: initial # of blocks
 *  - pool_type: STATIC or DYNAMIC
 *  - thread_mode: concurrency approach (UNSAFE, MUTEX, SPINLOCK)
 *  - alignment: if >1, do aligned allocations
 *  - max_chunks: limit expansions (0 => no limit)
 *  - expansion_factor: e.g., 2.0 => double expansions
 *  - mem_alloc/mem_realloc/mem_free: custom memory functions
 */
typedef struct {
    size_t block_size;
    size_t num_blocks;

    ec_mempool_type_t pool_type;
    ec_thread_mode_t  thread_mode;

    size_t alignment;           
    size_t max_chunks;          
    float  expansion_factor;    

    void *(*mem_alloc)(size_t);
    void *(*mem_realloc)(void*, size_t);
    void  (*mem_free)(void*);
} ec_mempool_conf_t;

/* Internal chunk structure */
typedef struct ec_mempool_chunk_s {
    void      *memory;
    uintptr_t  start_addr;
    uintptr_t  end_addr;
    size_t     free_count;
    bool       is_main;
} ec_mempool_chunk_t;

/* The Pool */
typedef struct {
    ec_mempool_conf_t  conf;
    ec_mempool_stats_t stats;

    ec_mutex_t    mutex;
    ec_spinlock_t spin;

    /* free_list: pointers to blocks (void*) */
    void   **free_list;
    size_t   free_index;
    size_t   free_capacity;

    /* chunk array for expansions */
    ec_mempool_chunk_t *chunks;
    size_t              num_chunks;
    size_t              chunk_capacity;
} ec_mempool_t;

/*-----------------------------------------------------
   Helper for memory calls
-----------------------------------------------------*/
static inline void *ec_pool_alloc(ec_mempool_t *pool, size_t size) {
    if (pool->conf.mem_alloc) {
        return pool->conf.mem_alloc(size);
    }
    return malloc(size);
}

static inline void *ec_pool_realloc(ec_mempool_t *pool, void *ptr, size_t size) {
    if (pool->conf.mem_realloc) {
        return pool->conf.mem_realloc(ptr, size);
    }
    return realloc(ptr, size);
}

static inline void ec_pool_free(ec_mempool_t *pool, void *ptr) {
    if (pool->conf.mem_free) {
        pool->conf.mem_free(ptr);
    } else {
        free(ptr);
    }
}

/* alignment if needed */
#if defined(EC_USE_POSIX)
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
static inline void *ec_pool_aligned_alloc(ec_mempool_t *pool, size_t alignment, size_t size) {
    if (pool->conf.mem_alloc || alignment<=1) {
        return ec_pool_alloc(pool, size);
    }
    void *p = NULL;
    int ret = posix_memalign(&p, alignment, size);
    if (ret != 0) return NULL;
    return p;
}
#else
static inline void *ec_pool_aligned_alloc(ec_mempool_t *pool, size_t alignment, size_t size) {
    return ec_pool_alloc(pool, size);
}
#endif

/* Lock Helpers */
static inline void ec_pool_lock(ec_mempool_t *pool) {
    if (pool->conf.thread_mode == EC_THREAD_MUTEX) {
        EC_MUTEX_LOCK(pool->mutex);
    } else if (pool->conf.thread_mode == EC_THREAD_SPINLOCK) {
        EC_SPIN_LOCK(pool->spin);
    }
}

static inline void ec_pool_unlock(ec_mempool_t *pool) {
    if (pool->conf.thread_mode == EC_THREAD_MUTEX) {
        EC_MUTEX_UNLOCK(pool->mutex);
    } else if (pool->conf.thread_mode == EC_THREAD_SPINLOCK) {
        EC_SPIN_UNLOCK(pool->spin);
    }
}

/*-----------------------------------------------------
   ec_mempool_init
-----------------------------------------------------*/
static inline ec_status_t ec_mempool_init(ec_mempool_t *pool, const ec_mempool_conf_t *conf) {
    if (!pool || !conf) return EC_ERR_INVALID_ARG;
    memset(pool, 0, sizeof(*pool));
    pool->conf = *conf;
    if (pool->conf.expansion_factor < 1.0f) {
        pool->conf.expansion_factor = 1.0f;
    }

    /* init locks */
    if (conf->thread_mode == EC_THREAD_MUTEX) {
        EC_MUTEX_INIT(pool->mutex);
    } else if (conf->thread_mode == EC_THREAD_SPINLOCK) {
        EC_SPIN_INIT(pool->spin);
    }

    /* allocate free_list */
    pool->free_capacity = conf->num_blocks;
    pool->free_index    = conf->num_blocks;
    pool->free_list = (void**) ec_pool_alloc(pool, sizeof(void*) * pool->free_capacity);
    if (!pool->free_list) {
        return EC_ERR_ALLOC;
    }

    /* allocate chunk array */
    pool->chunk_capacity= 1;
    pool->chunks = (ec_mempool_chunk_t*) ec_pool_alloc(pool, sizeof(ec_mempool_chunk_t)* pool->chunk_capacity);
    if (!pool->chunks) {
        ec_pool_free(pool, pool->free_list);
        pool->free_list= NULL;
        return EC_ERR_ALLOC;
    }
    pool->num_chunks= 1;

    /* allocate first chunk memory */
    size_t block_size= conf->block_size;
    size_t num_blocks= conf->num_blocks;
    size_t chunk_bytes= block_size* num_blocks;

    void *mem = (conf->alignment>1)
      ? ec_pool_aligned_alloc(pool, conf->alignment, chunk_bytes)
      : ec_pool_alloc(pool, chunk_bytes);
    if (!mem) {
        ec_pool_free(pool, pool->chunks);
        ec_pool_free(pool, pool->free_list);
        return EC_ERR_ALLOC;
    }

    ec_mempool_chunk_t *cdesc= &pool->chunks[0];
    cdesc->memory     = mem;
    cdesc->start_addr = (uintptr_t) mem;
    cdesc->end_addr   = cdesc->start_addr+ chunk_bytes;
    cdesc->free_count = num_blocks;
    cdesc->is_main    = true;

    /* Fill free_list with pointers to each block */
    for (size_t i=0; i<num_blocks; i++) {
        pool->free_list[i]= (char*)mem + i* block_size;
    }

    return EC_OK;
}

/*-----------------------------------------------------
   ec_mempool_alloc_block
-----------------------------------------------------*/
static inline void* ec_mempool_alloc_block(ec_mempool_t *pool) {
    if (!pool) return NULL;
    ec_pool_lock(pool);

    if (pool->free_index>0) {
        pool->free_index--;
        void *blk= pool->free_list[pool->free_index];
        pool->stats.allocations++;
        size_t usage = pool->stats.allocations - pool->stats.frees;
        if (usage> pool->stats.high_watermark) {
            pool->stats.high_watermark= usage;
        }

        /* pointer-range check to decrement chunk->free_count */
        uintptr_t addr= (uintptr_t)blk;
        for (size_t i=0; i< pool->num_chunks; i++) {
            ec_mempool_chunk_t *ch= &pool->chunks[i];
            if (addr>= ch->start_addr && addr< ch->end_addr) {
                ch->free_count--;
                break;
            }
        }
        ec_pool_unlock(pool);
        return blk;
    }

    /* if static or out of expansions => no blocks left */
    if (pool->conf.pool_type== EC_MEMPOOL_STATIC) {
        ec_pool_unlock(pool);
        return NULL;
    }

    /* expand if possible */
    size_t curr_chunks= pool->num_chunks;
    if (pool->conf.max_chunks && curr_chunks>= pool->conf.max_chunks) {
        ec_pool_unlock(pool);
        return NULL;
    }

    size_t new_count= curr_chunks+1;
    if (pool->conf.expansion_factor>1.0f) {
        float wantf= (float) curr_chunks* pool->conf.expansion_factor;
        size_t w= (size_t)(wantf+0.999f);
        if (w> new_count) {
            new_count= w;
        }
    }
    if (pool->conf.max_chunks && new_count> pool->conf.max_chunks) {
        new_count= pool->conf.max_chunks;
    }

    size_t to_add= (new_count- curr_chunks);
    size_t block_size= pool->conf.block_size;
    size_t chunk_bytes= block_size* pool->conf.num_blocks;

    /* reallocate chunk array if needed */
    if (new_count> pool->chunk_capacity) {
        size_t oldcap= pool->chunk_capacity;
        size_t newcap= (oldcap*2> new_count)? oldcap*2 : new_count;
        ec_mempool_chunk_t *nc= (ec_mempool_chunk_t*) ec_pool_realloc(pool, pool->chunks,
                                        newcap* sizeof(ec_mempool_chunk_t));
        if (!nc) {
            ec_pool_unlock(pool);
            return NULL;
        }
        pool->chunks= nc;
        pool->chunk_capacity= newcap;
    }

    for (size_t i=0; i< to_add; i++) {
        void *mem= (pool->conf.alignment>1)
          ? ec_pool_aligned_alloc(pool, pool->conf.alignment, chunk_bytes)
          : ec_pool_alloc(pool, chunk_bytes);
        if (!mem) break;

        ec_mempool_chunk_t *ch= &pool->chunks[pool->num_chunks];
        ch->memory     = mem;
        ch->start_addr = (uintptr_t)mem;
        ch->end_addr   = ch->start_addr+ chunk_bytes;
        ch->free_count = pool->conf.num_blocks;
        ch->is_main    = false;

        /* expand free_list if needed */
        size_t needed= pool->free_index+ pool->conf.num_blocks;
        if (needed> pool->free_capacity) {
            size_t new_cap= (pool->free_capacity*2> needed)? pool->free_capacity*2 : needed;
            void **nf= (void**) ec_pool_realloc(pool, pool->free_list, new_cap* sizeof(void*));
            if (!nf) {
                ec_pool_free(pool, mem);
                break;
            }
            pool->free_list= nf;
            pool->free_capacity= new_cap;
        }

        /* fill new blocks into free_list */
        for (size_t b=0; b< pool->conf.num_blocks; b++) {
            pool->free_list[ pool->free_index++ ]= (char*)mem + b* block_size;
        }
        pool->num_chunks++;
        pool->stats.expansions++;
    }

    if (pool->free_index>0) {
        pool->free_index--;
        void *blk= pool->free_list[pool->free_index];
        pool->stats.allocations++;
        size_t usage= pool->stats.allocations- pool->stats.frees;
        if (usage> pool->stats.high_watermark) {
            pool->stats.high_watermark= usage;
        }
        /* pointer-range check => decrement free_count */
        uintptr_t addr= (uintptr_t) blk;
        for (size_t i=0; i< pool->num_chunks; i++) {
            ec_mempool_chunk_t *c= &pool->chunks[i];
            if (addr>= c->start_addr && addr< c->end_addr) {
                c->free_count--;
                break;
            }
        }
        ec_pool_unlock(pool);
        return blk;
    }

    ec_pool_unlock(pool);
    return NULL;
}

/*-----------------------------------------------------
   ec_mempool_free_block
-----------------------------------------------------*/
static inline ec_status_t ec_mempool_free_block(ec_mempool_t *pool, void* block) {
    if (!pool || !block) return EC_ERR_INVALID_ARG;

    ec_pool_lock(pool);

    uintptr_t addr= (uintptr_t)block;
    bool found= false;
    for (size_t i=0; i< pool->num_chunks; i++) {
        ec_mempool_chunk_t *ch= &pool->chunks[i];
        if (addr>= ch->start_addr && addr< ch->end_addr) {
            ch->free_count++;
            found= true;
            break;
        }
    }
    if (!found) {
        ec_pool_unlock(pool);
        return EC_ERR_INVALID_ARG; 
    }

    if (pool->free_index>= pool->free_capacity) {
        size_t new_cap= pool->free_capacity*2;
        void **nf= (void**) ec_pool_realloc(pool, pool->free_list, new_cap* sizeof(void*));
        if (!nf) {
            ec_pool_unlock(pool);
            return EC_ERR_ALLOC;
        }
        pool->free_list= nf;
        pool->free_capacity= new_cap;
    }
    pool->free_list[ pool->free_index++ ]= block;
    pool->stats.frees++;

    ec_pool_unlock(pool);
    return EC_OK;
}

/*-----------------------------------------------------
   ec_mempool_gc
-----------------------------------------------------*/
static inline void ec_mempool_gc(ec_mempool_t *pool) {
    if (!pool || pool->conf.pool_type== EC_MEMPOOL_STATIC) return;

    ec_pool_lock(pool);
    size_t blocks_per_chunk= pool->conf.num_blocks;
    size_t i=1;
    while (i< pool->num_chunks) {
        ec_mempool_chunk_t *ch= &pool->chunks[i];
        if (ch->free_count== blocks_per_chunk && !ch->is_main) {
            /* remove all blocks from free_list in [start_addr..end_addr) */
            uintptr_t start= ch->start_addr;
            uintptr_t end  = ch->end_addr;
            size_t write_idx= 0;
            for (size_t r=0; r< pool->free_index; r++) {
                uintptr_t p= (uintptr_t) pool->free_list[r];
                if (p>= start && p< end) {
                    /* skip */
                } else {
                    pool->free_list[write_idx++]= pool->free_list[r];
                }
            }
            pool->free_index= write_idx;

            ec_pool_free(pool, ch->memory);

            size_t last_idx= pool->num_chunks-1;
            if (i< last_idx) {
                memmove(&pool->chunks[i], &pool->chunks[i+1], (last_idx- i)* sizeof(ec_mempool_chunk_t));
            }
            pool->num_chunks--;
        } else {
            i++;
        }
    }
    ec_pool_unlock(pool);
}

/*-----------------------------------------------------
   ec_mempool_destroy
-----------------------------------------------------*/
static inline ec_status_t ec_mempool_destroy(ec_mempool_t *pool) {
    if (!pool) return EC_ERR_INVALID_ARG;

    for (size_t i=0; i< pool->num_chunks; i++) {
        ec_mempool_chunk_t *ch= &pool->chunks[i];
        if (ch->memory) {
            ec_pool_free(pool, ch->memory);
        }
    }
    if (pool->chunks) {
        ec_pool_free(pool, pool->chunks);
        pool->chunks= NULL;
    }
    if (pool->free_list) {
        ec_pool_free(pool, pool->free_list);
        pool->free_list= NULL;
    }

    if (pool->conf.thread_mode== EC_THREAD_MUTEX) {
        EC_MUTEX_DESTROY(pool->mutex);
    } else if (pool->conf.thread_mode== EC_THREAD_SPINLOCK) {
        EC_SPIN_DESTROY(pool->spin);
    }

    return EC_OK;
}

/*-----------------------------------------------------
   ec_mempool_get_stats
-----------------------------------------------------*/
static inline ec_mempool_stats_t ec_mempool_get_stats(ec_mempool_t *pool) {
    if (!pool) {
        ec_mempool_stats_t empty= {0,0,0,0};
        return empty;
    }
    return pool->stats;
}

#ifdef __cplusplus
}
#endif

#endif /* EC_MEMPOOL_H */
