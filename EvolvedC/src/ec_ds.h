#ifndef EC_DS_H
#define EC_DS_H
#include "ec_common.h"
#include "ec_status.h"
#ifdef EC_USE_MEMPOOL
#define EC_DS_ALLOC(T, MPOOL) ((MPOOL) ? (T*)ec_mempool_alloc((ec_mempool_t*)(MPOOL), sizeof(T)) : (T*)EC_MALLOC(sizeof(T)))
#else
#define EC_DS_ALLOC(T, MPOOL) (T*)EC_MALLOC(sizeof(T))
#endif

/* Unified Disjoint‐Set with optional weighting.
   Every node always stores a weight (default 1.0). Plain DS operations ignore weight.
   For weighted unions, use the _weighted variants.
*/
#define EC_DS_DEF(TYPE, NAME)                                                                                                \
	typedef struct ec_ds_##NAME##_node_s                                                                                     \
	{                                                                                                                        \
		TYPE key;                                                                                                            \
		struct ec_ds_##NAME##_node_s* parent;                                                                                \
		int rank;                                                                                                            \
		double weight; /* For weighted DS; for plain DS remain 1.0 */                                                        \
		size_t size;                                                                                                         \
	} ec_ds_##NAME##_node_t;                                                                                                 \
	static inline ec_ds_##NAME##_node_t* ec_ds_##NAME##_make_set(const TYPE* key, void* mp)                                  \
	{                                                                                                                        \
		ec_ds_##NAME##_node_t* n = EC_DS_ALLOC(ec_ds_##NAME##_node_t, mp);                                                   \
		if (n)                                                                                                               \
			{                                                                                                                \
				n->key = (TYPE)(*key);                                                                                       \
				n->parent = n;                                                                                               \
				n->rank = 0;                                                                                                 \
				n->weight = 1.0;                                                                                             \
				n->size = 1;                                                                                                 \
			}                                                                                                                \
		return n;                                                                                                            \
	}                                                                                                                        \
	static inline ec_ds_##NAME##_node_t* ec_ds_##NAME##_find(ec_ds_##NAME##_node_t* n)                                       \
	{                                                                                                                        \
		if (n->parent != n)                                                                                                  \
			{                                                                                                                \
				ec_ds_##NAME##_node_t* temp = ec_ds_##NAME##_find(n->parent);                                                \
				n->weight *= n->parent->weight;                                                                              \
				n->parent = temp;                                                                                            \
			}                                                                                                                \
		return n->parent;                                                                                                    \
	}                                                                                                                        \
	static inline void ec_ds_##NAME##_union_by_rank(ec_ds_##NAME##_node_t* x, ec_ds_##NAME##_node_t* y)                      \
	{                                                                                                                        \
		ec_ds_##NAME##_node_t *rx = ec_ds_##NAME##_find(x), *ry = ec_ds_##NAME##_find(y);                                    \
		if (rx == ry)                                                                                                        \
			return;                                                                                                          \
		/* Plain union-by-rank: weight remains 1.0 */                                                                        \
		if (rx->rank < ry->rank)                                                                                             \
			rx->parent = ry;                                                                                                 \
		else if (rx->rank > ry->rank)                                                                                        \
			ry->parent = rx;                                                                                                 \
		else                                                                                                                 \
			{                                                                                                                \
				ry->parent = rx;                                                                                             \
				rx->rank++;                                                                                                  \
			}                                                                                                                \
	}                                                                                                                        \
	static inline void ec_ds_##NAME##_union_by_rank_weighted(ec_ds_##NAME##_node_t* x, ec_ds_##NAME##_node_t* y, double val) \
	{                                                                                                                        \
		ec_ds_##NAME##_node_t *rx = ec_ds_##NAME##_find(x), *ry = ec_ds_##NAME##_find(y);                                    \
		if (rx == ry)                                                                                                        \
			return;                                                                                                          \
		if (rx->rank < ry->rank)                                                                                             \
			{                                                                                                                \
				rx->parent = ry;                                                                                             \
				rx->weight = val * ry->weight / x->weight;                                                                   \
			}                                                                                                                \
		else if (rx->rank > ry->rank)                                                                                        \
			{                                                                                                                \
				ry->parent = rx;                                                                                             \
				ry->weight = x->weight / (val * y->weight);                                                                  \
			}                                                                                                                \
		else                                                                                                                 \
			{                                                                                                                \
				ry->parent = rx;                                                                                             \
				rx->rank++;                                                                                                  \
				ry->weight = x->weight / (val * y->weight);                                                                  \
			}                                                                                                                \
	}                                                                                                                        \
	static inline void ec_ds_##NAME##_union_by_size(ec_ds_##NAME##_node_t* x, ec_ds_##NAME##_node_t* y)                      \
	{                                                                                                                        \
		ec_ds_##NAME##_node_t *rx = ec_ds_##NAME##_find(x), *ry = ec_ds_##NAME##_find(y);                                    \
		if (rx == ry)                                                                                                        \
			return;                                                                                                          \
		if (rx->size < ry->size)                                                                                             \
			{                                                                                                                \
				rx->parent = ry;                                                                                             \
				ry->size += rx->size;                                                                                        \
			}                                                                                                                \
		else                                                                                                                 \
			{                                                                                                                \
				ry->parent = rx;                                                                                             \
				rx->size += ry->size;                                                                                        \
			}                                                                                                                \
	}                                                                                                                        \
	static inline void ec_ds_##NAME##_union_by_size_weighted(ec_ds_##NAME##_node_t* x, ec_ds_##NAME##_node_t* y, double val) \
	{                                                                                                                        \
		ec_ds_##NAME##_node_t *rx = ec_ds_##NAME##_find(x), *ry = ec_ds_##NAME##_find(y);                                    \
		if (rx == ry)                                                                                                        \
			return;                                                                                                          \
		if (rx->size < ry->size)                                                                                             \
			{                                                                                                                \
				rx->parent = ry;                                                                                             \
				rx->weight = val * ry->weight / x->weight;                                                                   \
				ry->size += rx->size;                                                                                        \
			}                                                                                                                \
		else                                                                                                                 \
			{                                                                                                                \
				ry->parent = rx;                                                                                             \
				ry->weight = x->weight / (val * y->weight);                                                                  \
				rx->size += ry->size;                                                                                        \
			}                                                                                                                \
	}                                                                                                                        \
	static inline double ec_ds_##NAME##_calc(ec_ds_##NAME##_node_t* x, ec_ds_##NAME##_node_t* y)                             \
	{                                                                                                                        \
		return (ec_ds_##NAME##_find(x) == ec_ds_##NAME##_find(y)) ? x->weight / y->weight : -1.0;                            \
	}

/* Instantiate unified DS for common types */
EC_DS_DEF(int, int)
EC_DS_DEF(unsigned int, uint)
EC_DS_DEF(long, long)
EC_DS_DEF(long long, ll)
EC_DS_DEF(unsigned long, ul)
EC_DS_DEF(unsigned long long, ull)
EC_DS_DEF(uint32_t, u32)
EC_DS_DEF(uint64_t, u64)
EC_DS_DEF(double, dbl)
EC_DS_DEF(const char*, str)
EC_DS_DEF(void*, ptr)
#endif
