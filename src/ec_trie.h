#ifndef EC_TRIE_H
#define EC_TRIE_H
#include "ec_common.h"
#include "ec_status.h"
#ifdef EC_USE_MEMPOOL
#define EC_TRIE_ALLOC(T, MP) ((MP) ? (T*)ec_mempool_alloc((ec_mempool_t*)(MP), sizeof(T)) : (T*)EC_MALLOC(sizeof(T)))
#define EC_TRIE_REALLOC(OLD, T, CNT, MP) ((MP) ? NULL : (T*)EC_REALLOC(OLD, (CNT) * sizeof(T)))
#else
#define EC_TRIE_ALLOC(T, MP) (T*)EC_MALLOC(sizeof(T))
#define EC_TRIE_REALLOC(OLD, T, CNT, MP) (T*)EC_REALLOC(OLD, (CNT) * sizeof(T))
#endif

/*
  Generic Trie for UTF-8 (or any char-based) strings.

  1) Each key is treated as a null-terminated 'const char*' string.
  2) Each node holds an array of (char -> childNode), grown dynamically.
  3) 'end' indicates if the node terminates a key. 'val' stores the exact string pointer.
  4) 'compare' can be a custom compare if you need additional checks (like case-insensitive).
*/

#define EC_TRIE_DEF(TYPE, NAME)                                                                                                                                             \
	typedef struct ec_trie_##NAME##_link_s                                                                                                                                  \
	{                                                                                                                                                                       \
		unsigned char c;                                                                                                                                                    \
		struct ec_trie_##NAME##_node_s* next;                                                                                                                               \
	} ec_trie_##NAME##_link_t;                                                                                                                                              \
	typedef struct ec_trie_##NAME##_node_s                                                                                                                                  \
	{                                                                                                                                                                       \
		ec_trie_##NAME##_link_t* links;                                                                                                                                     \
		size_t link_count;                                                                                                                                                  \
		unsigned char end;                                                                                                                                                  \
		TYPE val;                                                                                                                                                           \
	} ec_trie_##NAME##_node_t;                                                                                                                                              \
	typedef struct ec_trie_##NAME##_s                                                                                                                                       \
	{                                                                                                                                                                       \
		ec_trie_##NAME##_node_t* root;                                                                                                                                      \
		EC_MUTEX_TYPE lock;                                                                                                                                                 \
		void* mempool;                                                                                                                                                      \
		int (*compare)(const TYPE*, const TYPE*);                                                                                                                           \
		ec_status_t (*add)(struct ec_trie_##NAME##_s*, const TYPE*);                                                                                                        \
		ec_status_t (*remove)(struct ec_trie_##NAME##_s*, const TYPE*);                                                                                                     \
		ec_status_t (*contains)(struct ec_trie_##NAME##_s*, const TYPE*);                                                                                                   \
		ec_status_t (*clear)(struct ec_trie_##NAME##_s*);                                                                                                                   \
		ec_status_t (*foreach)(struct ec_trie_##NAME##_s*, void (*)(TYPE));                                                                                                 \
	} ec_trie_##NAME##_t;                                                                                                                                                   \
	static inline int ec_trie_##NAME##_dfcmp(const TYPE* a, const TYPE* b)                                                                                                  \
	{                                                                                                                                                                       \
		return (*a < *b) ? -1 : ((*a > *b) ? 1 : 0);                                                                                                                        \
	}                                                                                                                                                                       \
	static inline ec_trie_##NAME##_node_t* ec_trie_##NAME##_alloc_node(void* mp)                                                                                            \
	{                                                                                                                                                                       \
		ec_trie_##NAME##_node_t* n = EC_TRIE_ALLOC(ec_trie_##NAME##_node_t, mp);                                                                                            \
		if (!n)                                                                                                                                                             \
			return NULL;                                                                                                                                                    \
		n->links = NULL;                                                                                                                                                    \
		n->link_count = 0;                                                                                                                                                  \
		n->end = 0;                                                                                                                                                         \
		return n;                                                                                                                                                           \
	}                                                                                                                                                                       \
	static inline ec_trie_##NAME##_node_t* ec_trie_##NAME##_get_child(ec_trie_##NAME##_node_t* n, unsigned char c)                                                          \
	{                                                                                                                                                                       \
		if (!n)                                                                                                                                                             \
			return NULL;                                                                                                                                                    \
		for (size_t i = 0; i < n->link_count; i++)                                                                                                                          \
			{                                                                                                                                                               \
				if (n->links[i].c == c)                                                                                                                                     \
					return n->links[i].next;                                                                                                                                \
			}                                                                                                                                                               \
		return NULL;                                                                                                                                                        \
	}                                                                                                                                                                       \
	static inline ec_trie_##NAME##_node_t* ec_trie_##NAME##_ensure_child(ec_trie_##NAME##_node_t* n, void* mp, unsigned char c)                                             \
	{                                                                                                                                                                       \
		for (size_t i = 0; i < n->link_count; i++)                                                                                                                          \
			{                                                                                                                                                               \
				if (n->links[i].c == c)                                                                                                                                     \
					return n->links[i].next;                                                                                                                                \
			}                                                                                                                                                               \
		ec_trie_##NAME##_link_t* tmp = EC_TRIE_REALLOC(n->links, ec_trie_##NAME##_link_t, n->link_count + 1, mp);                                                           \
		if (!tmp)                                                                                                                                                           \
			return NULL;                                                                                                                                                    \
		n->links = tmp;                                                                                                                                                     \
		n->links[n->link_count].c = c;                                                                                                                                      \
		n->links[n->link_count].next = ec_trie_##NAME##_alloc_node(mp);                                                                                                     \
		if (!n->links[n->link_count].next)                                                                                                                                  \
			return NULL;                                                                                                                                                    \
		n->link_count++;                                                                                                                                                    \
		return n->links[n->link_count - 1].next;                                                                                                                            \
	}                                                                                                                                                                       \
	static inline ec_status_t ec_trie_##NAME##_add_f(struct ec_trie_##NAME##_s* m, const TYPE* k)                                                                           \
	{                                                                                                                                                                       \
		if (!m || !k)                                                                                                                                                       \
			return EC_ERR_INVALID_ARG;                                                                                                                                      \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                             \
		if (!m->root)                                                                                                                                                       \
			{                                                                                                                                                               \
				m->root = ec_trie_##NAME##_alloc_node(m->mempool);                                                                                                          \
				if (!m->root)                                                                                                                                               \
					{                                                                                                                                                       \
						EC_MUTEX_UNLOCK(m->lock);                                                                                                                           \
						return EC_ERR_ALLOC;                                                                                                                                \
					}                                                                                                                                                       \
			}                                                                                                                                                               \
		const unsigned char* str = (const unsigned char*)(*k); /* interpret *k as null-terminated string */                                                                 \
		ec_trie_##NAME##_node_t* cur = m->root;                                                                                                                             \
		while (*str)                                                                                                                                                        \
			{                                                                                                                                                               \
				ec_trie_##NAME##_node_t* nx = ec_trie_##NAME##_ensure_child(cur, m->mempool, *str);                                                                         \
				if (!nx)                                                                                                                                                    \
					{                                                                                                                                                       \
						EC_MUTEX_UNLOCK(m->lock);                                                                                                                           \
						return EC_ERR_ALLOC;                                                                                                                                \
					}                                                                                                                                                       \
				cur = nx;                                                                                                                                                   \
				str++;                                                                                                                                                      \
			}                                                                                                                                                               \
		cur->end = 1;                                                                                                                                                       \
		cur->val = (TYPE)(*k);                                                                                                                                              \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                           \
		return EC_OK;                                                                                                                                                       \
	}                                                                                                                                                                       \
	static inline ec_status_t ec_trie_##NAME##_contains_f(struct ec_trie_##NAME##_s* m, const TYPE* k)                                                                      \
	{                                                                                                                                                                       \
		if (!m || !k)                                                                                                                                                       \
			return EC_ERR_INVALID_ARG;                                                                                                                                      \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                             \
		ec_trie_##NAME##_node_t* cur = m->root;                                                                                                                             \
		if (!cur)                                                                                                                                                           \
			{                                                                                                                                                               \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                                                   \
				return EC_ERR_NOT_FOUND;                                                                                                                                    \
			}                                                                                                                                                               \
		const unsigned char* str = (const unsigned char*)(*k);                                                                                                              \
		while (*str)                                                                                                                                                        \
			{                                                                                                                                                               \
				cur = ec_trie_##NAME##_get_child(cur, *str);                                                                                                                \
				if (!cur)                                                                                                                                                   \
					{                                                                                                                                                       \
						EC_MUTEX_UNLOCK(m->lock);                                                                                                                           \
						return EC_ERR_NOT_FOUND;                                                                                                                            \
					}                                                                                                                                                       \
				str++;                                                                                                                                                      \
			}                                                                                                                                                               \
		if (!cur->end)                                                                                                                                                      \
			{                                                                                                                                                               \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                                                   \
				return EC_ERR_NOT_FOUND;                                                                                                                                    \
			}                                                                                                                                                               \
		if (m->compare)                                                                                                                                                     \
			{                                                                                                                                                               \
				if (m->compare(k, &cur->val))                                                                                                                               \
					{                                                                                                                                                       \
						EC_MUTEX_UNLOCK(m->lock);                                                                                                                           \
						return EC_ERR_NOT_FOUND;                                                                                                                            \
					}                                                                                                                                                       \
			}                                                                                                                                                               \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                           \
		return EC_OK;                                                                                                                                                       \
	}                                                                                                                                                                       \
	static inline int ec_trie_##NAME##_is_leaf(ec_trie_##NAME##_node_t* n)                                                                                                  \
	{                                                                                                                                                                       \
		return (!n->end && (!n->links || !n->link_count));                                                                                                                  \
	}                                                                                                                                                                       \
	static inline ec_trie_##NAME##_node_t* ec_trie_##NAME##_rm_node(ec_trie_##NAME##_t* m, ec_trie_##NAME##_node_t* n, const TYPE* k, const unsigned char* str, int* found) \
	{                                                                                                                                                                       \
		if (!n)                                                                                                                                                             \
			return NULL;                                                                                                                                                    \
		if (!*str)                                                                                                                                                          \
			{                                                                                                                                                               \
				if (n->end)                                                                                                                                                 \
					{                                                                                                                                                       \
						if (m->compare)                                                                                                                                     \
							{                                                                                                                                               \
								if (!m->compare(k, &n->val))                                                                                                                \
									n->end = 0;                                                                                                                             \
							}                                                                                                                                               \
						else                                                                                                                                                \
							n->end = 0;                                                                                                                                     \
						if (!n->end)                                                                                                                                        \
							*found = 1;                                                                                                                                     \
					}                                                                                                                                                       \
			}                                                                                                                                                               \
		else                                                                                                                                                                \
			{                                                                                                                                                               \
				for (size_t i = 0; i < n->link_count; i++)                                                                                                                  \
					{                                                                                                                                                       \
						if (n->links[i].c == *str)                                                                                                                          \
							{                                                                                                                                               \
								n->links[i].next = ec_trie_##NAME##_rm_node(m, n->links[i].next, k, str + 1, found);                                                        \
								if (!n->links[i].next || ec_trie_##NAME##_is_leaf(n->links[i].next))                                                                        \
									{                                                                                                                                       \
										if (!n->links[i].next || (!n->links[i].next->end && n->links[i].next->link_count == 0))                                             \
											{                                                                                                                               \
												EC_FREE(n->links[i].next);                                                                                                  \
												for (size_t j = i; j < n->link_count - 1; j++)                                                                              \
													{                                                                                                                       \
														n->links[j] = n->links[j + 1];                                                                                      \
													}                                                                                                                       \
												n->link_count--;                                                                                                            \
												ec_trie_##NAME##_link_t* tmp = EC_TRIE_REALLOC(n->links, ec_trie_##NAME##_link_t, n->link_count, m->mempool);               \
												if (tmp || n->link_count == 0)                                                                                              \
													n->links = tmp;                                                                                                         \
											}                                                                                                                               \
									}                                                                                                                                       \
								break;                                                                                                                                      \
							}                                                                                                                                               \
					}                                                                                                                                                       \
			}                                                                                                                                                               \
		if (ec_trie_##NAME##_is_leaf(n))                                                                                                                                    \
			{                                                                                                                                                               \
				EC_FREE(n->links);                                                                                                                                          \
				EC_FREE(n);                                                                                                                                                 \
				n = NULL;                                                                                                                                                   \
			}                                                                                                                                                               \
		return n;                                                                                                                                                           \
	}                                                                                                                                                                       \
	static inline ec_status_t ec_trie_##NAME##_remove_f(struct ec_trie_##NAME##_s* m, const TYPE* k)                                                                        \
	{                                                                                                                                                                       \
		if (!m || !k)                                                                                                                                                       \
			return EC_ERR_INVALID_ARG;                                                                                                                                      \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                             \
		int found = 0;                                                                                                                                                      \
		if (m->root)                                                                                                                                                        \
			m->root = ec_trie_##NAME##_rm_node(m, m->root, k, (const unsigned char*)(*k), &found);                                                                          \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                           \
		return found ? EC_OK : EC_ERR_NOT_FOUND;                                                                                                                            \
	}                                                                                                                                                                       \
	static inline void ec_trie_##NAME##_dfs_clear(ec_trie_##NAME##_node_t* n)                                                                                               \
	{                                                                                                                                                                       \
		if (!n)                                                                                                                                                             \
			return;                                                                                                                                                         \
		for (size_t i = 0; i < n->link_count; i++)                                                                                                                          \
			{                                                                                                                                                               \
				ec_trie_##NAME##_dfs_clear(n->links[i].next);                                                                                                               \
			}                                                                                                                                                               \
		EC_FREE(n->links);                                                                                                                                                  \
		EC_FREE(n);                                                                                                                                                         \
	}                                                                                                                                                                       \
	static inline ec_status_t ec_trie_##NAME##_clear_f(struct ec_trie_##NAME##_s* m)                                                                                        \
	{                                                                                                                                                                       \
		if (!m)                                                                                                                                                             \
			return EC_ERR_INVALID_ARG;                                                                                                                                      \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                             \
		ec_trie_##NAME##_node_t* r = m->root;                                                                                                                               \
		m->root = NULL;                                                                                                                                                     \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                           \
		if (r)                                                                                                                                                              \
			ec_trie_##NAME##_dfs_clear(r);                                                                                                                                  \
		return EC_OK;                                                                                                                                                       \
	}                                                                                                                                                                       \
	static inline void ec_trie_##NAME##_dfs_foreach(ec_trie_##NAME##_node_t* n, void (*fn)(TYPE))                                                                           \
	{                                                                                                                                                                       \
		if (!n)                                                                                                                                                             \
			return;                                                                                                                                                         \
		if (n->end)                                                                                                                                                         \
			fn(n->val);                                                                                                                                                     \
		for (size_t i = 0; i < n->link_count; i++)                                                                                                                          \
			{                                                                                                                                                               \
				ec_trie_##NAME##_dfs_foreach(n->links[i].next, fn);                                                                                                         \
			}                                                                                                                                                               \
	}                                                                                                                                                                       \
	static inline ec_status_t ec_trie_##NAME##_foreach_f(struct ec_trie_##NAME##_s* m, void (*fn)(TYPE))                                                                    \
	{                                                                                                                                                                       \
		if (!m || !fn)                                                                                                                                                      \
			return EC_ERR_INVALID_ARG;                                                                                                                                      \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                             \
		ec_trie_##NAME##_node_t* r = m->root;                                                                                                                               \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                           \
		if (r)                                                                                                                                                              \
			ec_trie_##NAME##_dfs_foreach(r, fn);                                                                                                                            \
		return EC_OK;                                                                                                                                                       \
	}                                                                                                                                                                       \
	static inline ec_status_t ec_trie_##NAME##_init(ec_trie_##NAME##_t* m, void* mp, int (*cmp)(const TYPE*, const TYPE*))                                                  \
	{                                                                                                                                                                       \
		if (!m)                                                                                                                                                             \
			return EC_ERR_INVALID_ARG;                                                                                                                                      \
		EC_MUTEX_INIT(m->lock);                                                                                                                                             \
		m->root = NULL;                                                                                                                                                     \
		m->mempool = mp;                                                                                                                                                    \
		m->compare = cmp ? cmp : ec_trie_##NAME##_dfcmp;                                                                                                                    \
		m->add = ec_trie_##NAME##_add_f;                                                                                                                                    \
		m->remove = ec_trie_##NAME##_remove_f;                                                                                                                              \
		m->contains = ec_trie_##NAME##_contains_f;                                                                                                                          \
		m->clear = ec_trie_##NAME##_clear_f;                                                                                                                                \
		m->foreach = ec_trie_##NAME##_foreach_f;                                                                                                                            \
		return EC_OK;                                                                                                                                                       \
	}
/* Replace numeric expansions with typical language-coded expansions for char strings. Example: English, Chinese, Korean, Telugu, etc. */
EC_TRIE_DEF(const char*, en)
EC_TRIE_DEF(const char*, zh)
EC_TRIE_DEF(const char*, ko)
EC_TRIE_DEF(const char*, te)
EC_TRIE_DEF(const char*, hi)
EC_TRIE_DEF(const char*, any)

#endif
