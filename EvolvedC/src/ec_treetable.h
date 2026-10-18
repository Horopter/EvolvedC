#ifndef EC_TREETABLE_H
#define EC_TREETABLE_H
#include "ec_common.h"
#include "ec_status.h"
#ifdef EC_USE_MEMPOOL
#define EC_TREETABLE_ALLOC(T, MPOOL) ((MPOOL) ? (T*)ec_mempool_alloc((ec_mempool_t*)(MPOOL), sizeof(T)) : (T*)EC_MALLOC(sizeof(T)))
#else
#define EC_TREETABLE_ALLOC(T, MPOOL) (T*)EC_MALLOC(sizeof(T))
#endif

#define EC_TREETABLE_DEF(KT, VT, NAME)                                                                                                                                                               \
	typedef struct ec_treetable_##NAME##_entry_s                                                                                                                                                     \
	{                                                                                                                                                                                                \
		KT key;                                                                                                                                                                                      \
		VT value;                                                                                                                                                                                    \
		struct ec_treetable_##NAME##_entry_s *left, *right, *parent;                                                                                                                                 \
		int h;                                                                                                                                                                                       \
	} ec_treetable_##NAME##_entry_t;                                                                                                                                                                 \
	typedef struct ec_treetable_##NAME##_iter_s                                                                                                                                                      \
	{                                                                                                                                                                                                \
		struct ec_treetable_##NAME##_s* m;                                                                                                                                                           \
		ec_treetable_##NAME##_entry_t* curr;                                                                                                                                                         \
	} ec_treetable_##NAME##_iter_t;                                                                                                                                                                  \
	typedef struct ec_treetable_##NAME##_s                                                                                                                                                           \
	{                                                                                                                                                                                                \
		ec_treetable_##NAME##_entry_t* root;                                                                                                                                                         \
		size_t size;                                                                                                                                                                                 \
		EC_MUTEX_TYPE lock;                                                                                                                                                                          \
		void* mempool;                                                                                                                                                                               \
		int (*compare)(const KT*, const KT*);                                                                                                                                                        \
		ec_status_t (*add)(struct ec_treetable_##NAME##_s*, const KT*, const VT*);                                                                                                                   \
		ec_status_t (*remove)(struct ec_treetable_##NAME##_s*, const KT*);                                                                                                                           \
		ec_status_t (*remove_all)(struct ec_treetable_##NAME##_s*);                                                                                                                                  \
		ec_status_t (*remove_first)(struct ec_treetable_##NAME##_s*);                                                                                                                                \
		ec_status_t (*remove_last)(struct ec_treetable_##NAME##_s*);                                                                                                                                 \
		ec_status_t (*get)(struct ec_treetable_##NAME##_s*, const KT*, VT*);                                                                                                                         \
		ec_status_t (*get_first_value)(struct ec_treetable_##NAME##_s*, VT*);                                                                                                                        \
		ec_status_t (*get_first_key)(struct ec_treetable_##NAME##_s*, KT*);                                                                                                                          \
		ec_status_t (*get_last_value)(struct ec_treetable_##NAME##_s*, VT*);                                                                                                                         \
		ec_status_t (*get_last_key)(struct ec_treetable_##NAME##_s*, KT*);                                                                                                                           \
		ec_status_t (*get_greater_than)(struct ec_treetable_##NAME##_s*, const KT*, KT*, VT*);                                                                                                       \
		ec_status_t (*get_lesser_than)(struct ec_treetable_##NAME##_s*, const KT*, KT*, VT*);                                                                                                        \
		size_t (*size_fn)(struct ec_treetable_##NAME##_s*);                                                                                                                                          \
		ec_status_t (*contains_key)(struct ec_treetable_##NAME##_s*, const KT*);                                                                                                                     \
		ec_status_t (*contains_value)(struct ec_treetable_##NAME##_s*, const VT*);                                                                                                                   \
		ec_status_t (*foreach_key)(struct ec_treetable_##NAME##_s*, void (*)(KT));                                                                                                                   \
		ec_status_t (*foreach_value)(struct ec_treetable_##NAME##_s*, void (*)(VT));                                                                                                                 \
		ec_status_t (*iter_init)(struct ec_treetable_##NAME##_s*, ec_treetable_##NAME##_iter_t*);                                                                                                    \
		ec_status_t (*iter_next)(ec_treetable_##NAME##_iter_t*, KT*, VT*);                                                                                                                           \
		ec_status_t (*iter_remove)(ec_treetable_##NAME##_iter_t*);                                                                                                                                   \
	} ec_treetable_##NAME##_t;                                                                                                                                                                       \
	static inline int ec_treetable_##NAME##_dfcmp(const KT* a, const KT* b)                                                                                                                          \
	{                                                                                                                                                                                                \
		return (*a < *b) ? -1 : ((*a > *b) ? 1 : 0);                                                                                                                                                 \
	}                                                                                                                                                                                                \
	static inline int ec_treetable_##NAME##_h(ec_treetable_##NAME##_entry_t* n)                                                                                                                      \
	{                                                                                                                                                                                                \
		return n ? n->h : 0;                                                                                                                                                                         \
	}                                                                                                                                                                                                \
	static inline void ec_treetable_##NAME##_uh(ec_treetable_##NAME##_entry_t* n)                                                                                                                    \
	{                                                                                                                                                                                                \
		int l = ec_treetable_##NAME##_h(n->left), r = ec_treetable_##NAME##_h(n->right);                                                                                                             \
		n->h = ((l > r) ? l : r) + 1;                                                                                                                                                                \
	}                                                                                                                                                                                                \
	static inline ec_treetable_##NAME##_entry_t* ec_treetable_##NAME##_rr(ec_treetable_##NAME##_entry_t* y)                                                                                          \
	{                                                                                                                                                                                                \
		ec_treetable_##NAME##_entry_t* x = y->left;                                                                                                                                                  \
		y->left = x->right;                                                                                                                                                                          \
		if (x->right)                                                                                                                                                                                \
			x->right->parent = y;                                                                                                                                                                    \
		x->right = y;                                                                                                                                                                                \
		x->parent = y->parent;                                                                                                                                                                       \
		y->parent = x;                                                                                                                                                                               \
		ec_treetable_##NAME##_uh(y);                                                                                                                                                                 \
		ec_treetable_##NAME##_uh(x);                                                                                                                                                                 \
		return x;                                                                                                                                                                                    \
	}                                                                                                                                                                                                \
	static inline ec_treetable_##NAME##_entry_t* ec_treetable_##NAME##_rl(ec_treetable_##NAME##_entry_t* x)                                                                                          \
	{                                                                                                                                                                                                \
		ec_treetable_##NAME##_entry_t* y = x->right;                                                                                                                                                 \
		x->right = y->left;                                                                                                                                                                          \
		if (y->left)                                                                                                                                                                                 \
			y->left->parent = x;                                                                                                                                                                     \
		y->left = x;                                                                                                                                                                                 \
		y->parent = x->parent;                                                                                                                                                                       \
		x->parent = y;                                                                                                                                                                               \
		ec_treetable_##NAME##_uh(x);                                                                                                                                                                 \
		ec_treetable_##NAME##_uh(y);                                                                                                                                                                 \
		return y;                                                                                                                                                                                    \
	}                                                                                                                                                                                                \
	static inline ec_treetable_##NAME##_entry_t* ec_treetable_##NAME##_reb(ec_treetable_##NAME##_entry_t* n)                                                                                         \
	{                                                                                                                                                                                                \
		ec_treetable_##NAME##_uh(n);                                                                                                                                                                 \
		int b = ec_treetable_##NAME##_h(n->left) - ec_treetable_##NAME##_h(n->right);                                                                                                                \
		if (b > 1)                                                                                                                                                                                   \
			{                                                                                                                                                                                        \
				if (ec_treetable_##NAME##_h(n->left->left) < ec_treetable_##NAME##_h(n->left->right))                                                                                                \
					n->left = ec_treetable_##NAME##_rl(n->left);                                                                                                                                     \
				return ec_treetable_##NAME##_rr(n);                                                                                                                                                  \
			}                                                                                                                                                                                        \
		else if (b < -1)                                                                                                                                                                             \
			{                                                                                                                                                                                        \
				if (ec_treetable_##NAME##_h(n->right->right) < ec_treetable_##NAME##_h(n->right->left))                                                                                              \
					n->right = ec_treetable_##NAME##_rr(n->right);                                                                                                                                   \
				return ec_treetable_##NAME##_rl(n);                                                                                                                                                  \
			}                                                                                                                                                                                        \
		return n;                                                                                                                                                                                    \
	}                                                                                                                                                                                                \
	static inline ec_treetable_##NAME##_entry_t* ec_treetable_##NAME##_ins(ec_treetable_##NAME##_t* m, ec_treetable_##NAME##_entry_t* r, ec_treetable_##NAME##_entry_t* p, const KT* k, const VT* v) \
	{                                                                                                                                                                                                \
		if (!r)                                                                                                                                                                                      \
			{                                                                                                                                                                                        \
				r = EC_TREETABLE_ALLOC(ec_treetable_##NAME##_entry_t, m->mempool);                                                                                                                   \
				if (!r)                                                                                                                                                                              \
					return NULL;                                                                                                                                                                     \
				r->key = (KT)(*k);                                                                                                                                                                   \
				r->value = (VT)(*v);                                                                                                                                                                 \
				r->left = r->right = NULL;                                                                                                                                                           \
				r->parent = p;                                                                                                                                                                       \
				r->h = 1;                                                                                                                                                                            \
				m->size++;                                                                                                                                                                           \
				return r;                                                                                                                                                                            \
			}                                                                                                                                                                                        \
		int c = m->compare(k, &r->key);                                                                                                                                                              \
		if (c < 0)                                                                                                                                                                                   \
			r->left = ec_treetable_##NAME##_ins(m, r->left, r, k, v);                                                                                                                                \
		else if (c > 0)                                                                                                                                                                              \
			r->right = ec_treetable_##NAME##_ins(m, r->right, r, k, v);                                                                                                                              \
		else                                                                                                                                                                                         \
			r->value = (VT)(*v);                                                                                                                                                                     \
		return ec_treetable_##NAME##_reb(r);                                                                                                                                                         \
	}                                                                                                                                                                                                \
	static inline ec_treetable_##NAME##_entry_t* ec_treetable_##NAME##_mn(ec_treetable_##NAME##_entry_t* n)                                                                                          \
	{                                                                                                                                                                                                \
		while (n && n->left)                                                                                                                                                                         \
			n = n->left;                                                                                                                                                                             \
		return n;                                                                                                                                                                                    \
	}                                                                                                                                                                                                \
	static inline ec_treetable_##NAME##_entry_t* ec_treetable_##NAME##_mx(ec_treetable_##NAME##_entry_t* n)                                                                                          \
	{                                                                                                                                                                                                \
		while (n && n->right)                                                                                                                                                                        \
			n = n->right;                                                                                                                                                                            \
		return n;                                                                                                                                                                                    \
	}                                                                                                                                                                                                \
	static inline ec_treetable_##NAME##_entry_t* ec_treetable_##NAME##_rmn(ec_treetable_##NAME##_t* m, ec_treetable_##NAME##_entry_t* r)                                                             \
	{                                                                                                                                                                                                \
		if (!r)                                                                                                                                                                                      \
			return NULL;                                                                                                                                                                             \
		if (!r->left)                                                                                                                                                                                \
			{                                                                                                                                                                                        \
				ec_treetable_##NAME##_entry_t* x = r->right;                                                                                                                                         \
				EC_FREE(r);                                                                                                                                                                          \
				m->size--;                                                                                                                                                                           \
				return x;                                                                                                                                                                            \
			}                                                                                                                                                                                        \
		r->left = ec_treetable_##NAME##_rmn(m, r->left);                                                                                                                                             \
		return ec_treetable_##NAME##_reb(r);                                                                                                                                                         \
	}                                                                                                                                                                                                \
	static inline ec_treetable_##NAME##_entry_t* ec_treetable_##NAME##_rmx(ec_treetable_##NAME##_t* m, ec_treetable_##NAME##_entry_t* r)                                                             \
	{                                                                                                                                                                                                \
		if (!r)                                                                                                                                                                                      \
			return NULL;                                                                                                                                                                             \
		if (!r->right)                                                                                                                                                                               \
			{                                                                                                                                                                                        \
				ec_treetable_##NAME##_entry_t* x = r->left;                                                                                                                                          \
				EC_FREE(r);                                                                                                                                                                          \
				m->size--;                                                                                                                                                                           \
				return x;                                                                                                                                                                            \
			}                                                                                                                                                                                        \
		r->right = ec_treetable_##NAME##_rmx(m, r->right);                                                                                                                                           \
		return ec_treetable_##NAME##_reb(r);                                                                                                                                                         \
	}                                                                                                                                                                                                \
	static inline ec_treetable_##NAME##_entry_t* ec_treetable_##NAME##_rm(ec_treetable_##NAME##_t* m, ec_treetable_##NAME##_entry_t* r, const KT* k)                                                 \
	{                                                                                                                                                                                                \
		if (!r)                                                                                                                                                                                      \
			return NULL;                                                                                                                                                                             \
		int c = m->compare(k, &r->key);                                                                                                                                                              \
		if (c < 0)                                                                                                                                                                                   \
			r->left = ec_treetable_##NAME##_rm(m, r->left, k);                                                                                                                                       \
		else if (c > 0)                                                                                                                                                                              \
			r->right = ec_treetable_##NAME##_rm(m, r->right, k);                                                                                                                                     \
		else                                                                                                                                                                                         \
			{                                                                                                                                                                                        \
				/* Found the node to remove */                                                                                                                                                       \
				if (!r->left)                                                                                                                                                                        \
					{                                                                                                                                                                                \
						/* No left child: replace with right */                                                                                                                                      \
						ec_treetable_##NAME##_entry_t* temp = r->right;                                                                                                                              \
						EC_FREE(r);                                                                                                                                                                  \
						m->size--;                                                                                                                                                                   \
						return temp;                                                                                                                                                                 \
					}                                                                                                                                                                                \
				else if (!r->right)                                                                                                                                                                  \
					{                                                                                                                                                                                \
						/* No right child: replace with left */                                                                                                                                      \
						ec_treetable_##NAME##_entry_t* temp = r->left;                                                                                                                               \
						EC_FREE(r);                                                                                                                                                                  \
						m->size--;                                                                                                                                                                   \
						return temp;                                                                                                                                                                 \
					}                                                                                                                                                                                \
				else                                                                                                                                                                                 \
					{                                                                                                                                                                                \
						/* Both children exist: copy successor data into r, remove successor */                                                                                                      \
						ec_treetable_##NAME##_entry_t* mn = ec_treetable_##NAME##_mn(r->right);                                                                                                      \
						r->key = mn->key;                                                                                                                                                            \
						r->value = mn->value;                                                                                                                                                        \
						r->right = ec_treetable_##NAME##_rm(m, r->right, &mn->key);                                                                                                                  \
					}                                                                                                                                                                                \
			}                                                                                                                                                                                        \
		return ec_treetable_##NAME##_reb(r);                                                                                                                                                         \
	}                                                                                                                                                                                                \
	static inline ec_treetable_##NAME##_entry_t* ec_treetable_##NAME##_suc(ec_treetable_##NAME##_entry_t* n)                                                                                         \
	{                                                                                                                                                                                                \
		if (n->right)                                                                                                                                                                                \
			{                                                                                                                                                                                        \
				n = n->right;                                                                                                                                                                        \
				while (n->left)                                                                                                                                                                      \
					n = n->left;                                                                                                                                                                     \
				return n;                                                                                                                                                                            \
			}                                                                                                                                                                                        \
		ec_treetable_##NAME##_entry_t* p = n->parent;                                                                                                                                                \
		while (p && n == p->right)                                                                                                                                                                   \
			{                                                                                                                                                                                        \
				n = p;                                                                                                                                                                               \
				p = p->parent;                                                                                                                                                                       \
			}                                                                                                                                                                                        \
		return p;                                                                                                                                                                                    \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_add_f(struct ec_treetable_##NAME##_s* m, const KT* k, const VT* v)                                                                               \
	{                                                                                                                                                                                                \
		if (!m || !k || !v)                                                                                                                                                                          \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                                                      \
		m->root = ec_treetable_##NAME##_ins(m, m->root, NULL, k, v);                                                                                                                                 \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                                    \
		return m->root ? EC_OK : EC_ERR_ALLOC;                                                                                                                                                       \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_remove_f(struct ec_treetable_##NAME##_s* m, const KT* k)                                                                                         \
	{                                                                                                                                                                                                \
		if (!m || !k)                                                                                                                                                                                \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                                                      \
		size_t os = m->size;                                                                                                                                                                         \
		m->root = ec_treetable_##NAME##_rm(m, m->root, k);                                                                                                                                           \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                                    \
		return (m->size < os) ? EC_OK : EC_ERR_NOT_FOUND;                                                                                                                                            \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_remove_all_f(struct ec_treetable_##NAME##_s* m)                                                                                                  \
	{                                                                                                                                                                                                \
		if (!m)                                                                                                                                                                                      \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                                                      \
		while (m->root)                                                                                                                                                                              \
			m->root = ec_treetable_##NAME##_rmn(m, m->root);                                                                                                                                         \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                                    \
		return EC_OK;                                                                                                                                                                                \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_remove_first_f(struct ec_treetable_##NAME##_s* m)                                                                                                \
	{                                                                                                                                                                                                \
		if (!m)                                                                                                                                                                                      \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                                                      \
		if (!m->root)                                                                                                                                                                                \
			{                                                                                                                                                                                        \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                            \
				return EC_ERR_NOT_FOUND;                                                                                                                                                             \
			}                                                                                                                                                                                        \
		m->root = ec_treetable_##NAME##_rmn(m, m->root);                                                                                                                                             \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                                    \
		return EC_OK;                                                                                                                                                                                \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_remove_last_f(struct ec_treetable_##NAME##_s* m)                                                                                                 \
	{                                                                                                                                                                                                \
		if (!m)                                                                                                                                                                                      \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                                                      \
		if (!m->root)                                                                                                                                                                                \
			{                                                                                                                                                                                        \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                            \
				return EC_ERR_NOT_FOUND;                                                                                                                                                             \
			}                                                                                                                                                                                        \
		m->root = ec_treetable_##NAME##_rmx(m, m->root);                                                                                                                                             \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                                    \
		return EC_OK;                                                                                                                                                                                \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_get_f(struct ec_treetable_##NAME##_s* m, const KT* k, VT* v)                                                                                     \
	{                                                                                                                                                                                                \
		if (!m || !k || !v)                                                                                                                                                                          \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                                                      \
		ec_treetable_##NAME##_entry_t* n = m->root;                                                                                                                                                  \
		while (n)                                                                                                                                                                                    \
			{                                                                                                                                                                                        \
				int c = m->compare(k, &n->key);                                                                                                                                                      \
				if (!c)                                                                                                                                                                              \
					{                                                                                                                                                                                \
						*v = n->value;                                                                                                                                                               \
						EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                    \
						return EC_OK;                                                                                                                                                                \
					}                                                                                                                                                                                \
				n = (c < 0) ? n->left : n->right;                                                                                                                                                    \
			}                                                                                                                                                                                        \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                                    \
		return EC_ERR_NOT_FOUND;                                                                                                                                                                     \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_get_first_value_f(struct ec_treetable_##NAME##_s* m, VT* v)                                                                                      \
	{                                                                                                                                                                                                \
		if (!m || !v)                                                                                                                                                                                \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                                                      \
		ec_treetable_##NAME##_entry_t* n = ec_treetable_##NAME##_mn(m->root);                                                                                                                        \
		if (!n)                                                                                                                                                                                      \
			{                                                                                                                                                                                        \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                            \
				return EC_ERR_NOT_FOUND;                                                                                                                                                             \
			}                                                                                                                                                                                        \
		*v = n->value;                                                                                                                                                                               \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                                    \
		return EC_OK;                                                                                                                                                                                \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_get_first_key_f(struct ec_treetable_##NAME##_s* m, KT* k)                                                                                        \
	{                                                                                                                                                                                                \
		if (!m || !k)                                                                                                                                                                                \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                                                      \
		ec_treetable_##NAME##_entry_t* n = ec_treetable_##NAME##_mn(m->root);                                                                                                                        \
		if (!n)                                                                                                                                                                                      \
			{                                                                                                                                                                                        \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                            \
				return EC_ERR_NOT_FOUND;                                                                                                                                                             \
			}                                                                                                                                                                                        \
		*k = n->key;                                                                                                                                                                                 \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                                    \
		return EC_OK;                                                                                                                                                                                \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_get_last_value_f(struct ec_treetable_##NAME##_s* m, VT* v)                                                                                       \
	{                                                                                                                                                                                                \
		if (!m || !v)                                                                                                                                                                                \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                                                      \
		ec_treetable_##NAME##_entry_t* n = ec_treetable_##NAME##_mx(m->root);                                                                                                                        \
		if (!n)                                                                                                                                                                                      \
			{                                                                                                                                                                                        \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                            \
				return EC_ERR_NOT_FOUND;                                                                                                                                                             \
			}                                                                                                                                                                                        \
		*v = n->value;                                                                                                                                                                               \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                                    \
		return EC_OK;                                                                                                                                                                                \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_get_last_key_f(struct ec_treetable_##NAME##_s* m, KT* k)                                                                                         \
	{                                                                                                                                                                                                \
		if (!m || !k)                                                                                                                                                                                \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                                                      \
		ec_treetable_##NAME##_entry_t* n = ec_treetable_##NAME##_mx(m->root);                                                                                                                        \
		if (!n)                                                                                                                                                                                      \
			{                                                                                                                                                                                        \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                            \
				return EC_ERR_NOT_FOUND;                                                                                                                                                             \
			}                                                                                                                                                                                        \
		*k = n->key;                                                                                                                                                                                 \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                                    \
		return EC_OK;                                                                                                                                                                                \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_get_greater_than_f(struct ec_treetable_##NAME##_s* m, const KT* k, KT* ok, VT* ov)                                                               \
	{                                                                                                                                                                                                \
		if (!m || !k || !ok || !ov)                                                                                                                                                                  \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                                                      \
		ec_treetable_##NAME##_entry_t *c = m->root, *can = NULL;                                                                                                                                     \
		while (c)                                                                                                                                                                                    \
			{                                                                                                                                                                                        \
				int cmp = m->compare(k, &c->key);                                                                                                                                                    \
				if (cmp < 0)                                                                                                                                                                         \
					{                                                                                                                                                                                \
						can = c;                                                                                                                                                                     \
						c = c->left;                                                                                                                                                                 \
					}                                                                                                                                                                                \
				else                                                                                                                                                                                 \
					c = c->right;                                                                                                                                                                    \
			}                                                                                                                                                                                        \
		if (!can)                                                                                                                                                                                    \
			{                                                                                                                                                                                        \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                            \
				return EC_ERR_NOT_FOUND;                                                                                                                                                             \
			}                                                                                                                                                                                        \
		*ok = can->key;                                                                                                                                                                              \
		*ov = can->value;                                                                                                                                                                            \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                                    \
		return EC_OK;                                                                                                                                                                                \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_get_lesser_than_f(struct ec_treetable_##NAME##_s* m, const KT* k, KT* ok, VT* ov)                                                                \
	{                                                                                                                                                                                                \
		if (!m || !k || !ok || !ov)                                                                                                                                                                  \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                                                      \
		ec_treetable_##NAME##_entry_t *c = m->root, *can = NULL;                                                                                                                                     \
		while (c)                                                                                                                                                                                    \
			{                                                                                                                                                                                        \
				int cmp = m->compare(k, &c->key);                                                                                                                                                    \
				if (cmp > 0)                                                                                                                                                                         \
					{                                                                                                                                                                                \
						can = c;                                                                                                                                                                     \
						c = c->right;                                                                                                                                                                \
					}                                                                                                                                                                                \
				else                                                                                                                                                                                 \
					c = c->left;                                                                                                                                                                     \
			}                                                                                                                                                                                        \
		if (!can)                                                                                                                                                                                    \
			{                                                                                                                                                                                        \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                            \
				return EC_ERR_NOT_FOUND;                                                                                                                                                             \
			}                                                                                                                                                                                        \
		*ok = can->key;                                                                                                                                                                              \
		*ov = can->value;                                                                                                                                                                            \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                                    \
		return EC_OK;                                                                                                                                                                                \
	}                                                                                                                                                                                                \
	static inline size_t ec_treetable_##NAME##_size_f(struct ec_treetable_##NAME##_s* m)                                                                                                             \
	{                                                                                                                                                                                                \
		if (!m)                                                                                                                                                                                      \
			return 0;                                                                                                                                                                                \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                                                      \
		size_t s = m->size;                                                                                                                                                                          \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                                    \
		return s;                                                                                                                                                                                    \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_contains_key_f(struct ec_treetable_##NAME##_s* m, const KT* k)                                                                                   \
	{                                                                                                                                                                                                \
		if (!m || !k)                                                                                                                                                                                \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                                                      \
		ec_treetable_##NAME##_entry_t* n = m->root;                                                                                                                                                  \
		while (n)                                                                                                                                                                                    \
			{                                                                                                                                                                                        \
				int c = m->compare(k, &n->key);                                                                                                                                                      \
				if (!c)                                                                                                                                                                              \
					{                                                                                                                                                                                \
						EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                    \
						return EC_OK;                                                                                                                                                                \
					}                                                                                                                                                                                \
				n = (c < 0) ? n->left : n->right;                                                                                                                                                    \
			}                                                                                                                                                                                        \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                                    \
		return EC_ERR_NOT_FOUND;                                                                                                                                                                     \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_contains_value_f(struct ec_treetable_##NAME##_s* m, const VT* v)                                                                                 \
	{                                                                                                                                                                                                \
		if (!m || !v)                                                                                                                                                                                \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                                                      \
		ec_treetable_##NAME##_entry_t* cur = m->root;                                                                                                                                                \
		ec_status_t ret = EC_ERR_NOT_FOUND;                                                                                                                                                          \
		while (cur)                                                                                                                                                                                  \
			{                                                                                                                                                                                        \
				if (cur->left)                                                                                                                                                                       \
					{                                                                                                                                                                                \
						ec_treetable_##NAME##_entry_t* pre = cur->left;                                                                                                                              \
						while (pre->right && pre->right != cur)                                                                                                                                      \
							pre = pre->right;                                                                                                                                                        \
						if (!pre->right)                                                                                                                                                             \
							{                                                                                                                                                                        \
								pre->right = cur;                                                                                                                                                    \
								cur = cur->left;                                                                                                                                                     \
							}                                                                                                                                                                        \
						else                                                                                                                                                                         \
							{                                                                                                                                                                        \
								pre->right = NULL;                                                                                                                                                   \
								if (cur->value == (VT)(*v))                                                                                                                                          \
									{                                                                                                                                                                \
										ret = EC_OK;                                                                                                                                                 \
										break;                                                                                                                                                       \
									}                                                                                                                                                                \
								cur = cur->right;                                                                                                                                                    \
							}                                                                                                                                                                        \
					}                                                                                                                                                                                \
				else                                                                                                                                                                                 \
					{                                                                                                                                                                                \
						if (cur->value == (VT)(*v))                                                                                                                                                  \
							{                                                                                                                                                                        \
								ret = EC_OK;                                                                                                                                                         \
								break;                                                                                                                                                               \
							}                                                                                                                                                                        \
						cur = cur->right;                                                                                                                                                            \
					}                                                                                                                                                                                \
			}                                                                                                                                                                                        \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                                    \
		return ret;                                                                                                                                                                                  \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_foreach_key_f(struct ec_treetable_##NAME##_s* m, void (*fn)(KT))                                                                                 \
	{                                                                                                                                                                                                \
		if (!m || !fn)                                                                                                                                                                               \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                                                      \
		ec_treetable_##NAME##_entry_t* cur = m->root;                                                                                                                                                \
		while (cur)                                                                                                                                                                                  \
			{                                                                                                                                                                                        \
				if (!cur->left)                                                                                                                                                                      \
					{                                                                                                                                                                                \
						fn(cur->key);                                                                                                                                                                \
						cur = cur->right;                                                                                                                                                            \
					}                                                                                                                                                                                \
				else                                                                                                                                                                                 \
					{                                                                                                                                                                                \
						ec_treetable_##NAME##_entry_t* pre = cur->left;                                                                                                                              \
						while (pre->right && pre->right != cur)                                                                                                                                      \
							pre = pre->right;                                                                                                                                                        \
						if (!pre->right)                                                                                                                                                             \
							{                                                                                                                                                                        \
								pre->right = cur;                                                                                                                                                    \
								cur = cur->left;                                                                                                                                                     \
							}                                                                                                                                                                        \
						else                                                                                                                                                                         \
							{                                                                                                                                                                        \
								pre->right = NULL;                                                                                                                                                   \
								fn(cur->key);                                                                                                                                                        \
								cur = cur->right;                                                                                                                                                    \
							}                                                                                                                                                                        \
					}                                                                                                                                                                                \
			}                                                                                                                                                                                        \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                                    \
		return EC_OK;                                                                                                                                                                                \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_foreach_value_f(struct ec_treetable_##NAME##_s* m, void (*fn)(VT))                                                                               \
	{                                                                                                                                                                                                \
		if (!m || !fn)                                                                                                                                                                               \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                                                      \
		ec_treetable_##NAME##_entry_t* cur = m->root;                                                                                                                                                \
		while (cur)                                                                                                                                                                                  \
			{                                                                                                                                                                                        \
				if (!cur->left)                                                                                                                                                                      \
					{                                                                                                                                                                                \
						fn(cur->value);                                                                                                                                                              \
						cur = cur->right;                                                                                                                                                            \
					}                                                                                                                                                                                \
				else                                                                                                                                                                                 \
					{                                                                                                                                                                                \
						ec_treetable_##NAME##_entry_t* pre = cur->left;                                                                                                                              \
						while (pre->right && pre->right != cur)                                                                                                                                      \
							pre = pre->right;                                                                                                                                                        \
						if (!pre->right)                                                                                                                                                             \
							{                                                                                                                                                                        \
								pre->right = cur;                                                                                                                                                    \
								cur = cur->left;                                                                                                                                                     \
							}                                                                                                                                                                        \
						else                                                                                                                                                                         \
							{                                                                                                                                                                        \
								pre->right = NULL;                                                                                                                                                   \
								fn(cur->value);                                                                                                                                                      \
								cur = cur->right;                                                                                                                                                    \
							}                                                                                                                                                                        \
					}                                                                                                                                                                                \
			}                                                                                                                                                                                        \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                                    \
		return EC_OK;                                                                                                                                                                                \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_iter_init_f(struct ec_treetable_##NAME##_s* m, ec_treetable_##NAME##_iter_t* it)                                                                 \
	{                                                                                                                                                                                                \
		if (!m || !it)                                                                                                                                                                               \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		it->m = m;                                                                                                                                                                                   \
		EC_MUTEX_LOCK(m->lock);                                                                                                                                                                      \
		it->curr = ec_treetable_##NAME##_mn(m->root);                                                                                                                                                \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                                                                                    \
		return EC_OK;                                                                                                                                                                                \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_iter_next_f(ec_treetable_##NAME##_iter_t* it, KT* ok, VT* ov)                                                                                    \
	{                                                                                                                                                                                                \
		if (!it || !ok || !ov)                                                                                                                                                                       \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		EC_MUTEX_LOCK(it->m->lock);                                                                                                                                                                  \
		if (!it->curr)                                                                                                                                                                               \
			{                                                                                                                                                                                        \
				EC_MUTEX_UNLOCK(it->m->lock);                                                                                                                                                        \
				return EC_ERR_OUT_OF_BOUNDS;                                                                                                                                                         \
			}                                                                                                                                                                                        \
		*ok = it->curr->key;                                                                                                                                                                         \
		*ov = it->curr->value;                                                                                                                                                                       \
		it->curr = ec_treetable_##NAME##_suc(it->curr);                                                                                                                                              \
		EC_MUTEX_UNLOCK(it->m->lock);                                                                                                                                                                \
		return EC_OK;                                                                                                                                                                                \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_iter_remove_f(ec_treetable_##NAME##_iter_t* it)                                                                                                  \
	{                                                                                                                                                                                                \
		if (!it || !it->curr)                                                                                                                                                                        \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		EC_MUTEX_LOCK(it->m->lock);                                                                                                                                                                  \
		ec_treetable_##NAME##_entry_t* r = it->curr;                                                                                                                                                 \
		it->curr = ec_treetable_##NAME##_suc(it->curr);                                                                                                                                              \
		it->m->root = ec_treetable_##NAME##_rm(it->m, it->m->root, &r->key);                                                                                                                         \
		EC_MUTEX_UNLOCK(it->m->lock);                                                                                                                                                                \
		return EC_OK;                                                                                                                                                                                \
	}                                                                                                                                                                                                \
	static inline ec_status_t ec_treetable_##NAME##_init(ec_treetable_##NAME##_t* m, void* mp, int (*cmp)(const KT*, const KT*))                                                                     \
	{                                                                                                                                                                                                \
		if (!m)                                                                                                                                                                                      \
			return EC_ERR_INVALID_ARG;                                                                                                                                                               \
		EC_MUTEX_INIT(m->lock);                                                                                                                                                                      \
		m->root = NULL;                                                                                                                                                                              \
		m->size = 0;                                                                                                                                                                                 \
		m->mempool = mp;                                                                                                                                                                             \
		m->compare = cmp ? cmp : ec_treetable_##NAME##_dfcmp;                                                                                                                                        \
		m->add = ec_treetable_##NAME##_add_f;                                                                                                                                                        \
		m->remove = ec_treetable_##NAME##_remove_f;                                                                                                                                                  \
		m->remove_all = ec_treetable_##NAME##_remove_all_f;                                                                                                                                          \
		m->remove_first = ec_treetable_##NAME##_remove_first_f;                                                                                                                                      \
		m->remove_last = ec_treetable_##NAME##_remove_last_f;                                                                                                                                        \
		m->get = ec_treetable_##NAME##_get_f;                                                                                                                                                        \
		m->get_first_value = ec_treetable_##NAME##_get_first_value_f;                                                                                                                                \
		m->get_first_key = ec_treetable_##NAME##_get_first_key_f;                                                                                                                                    \
		m->get_last_value = ec_treetable_##NAME##_get_last_value_f;                                                                                                                                  \
		m->get_last_key = ec_treetable_##NAME##_get_last_key_f;                                                                                                                                      \
		m->get_greater_than = ec_treetable_##NAME##_get_greater_than_f;                                                                                                                              \
		m->get_lesser_than = ec_treetable_##NAME##_get_lesser_than_f;                                                                                                                                \
		m->size_fn = ec_treetable_##NAME##_size_f;                                                                                                                                                   \
		m->contains_key = ec_treetable_##NAME##_contains_key_f;                                                                                                                                      \
		m->contains_value = ec_treetable_##NAME##_contains_value_f;                                                                                                                                  \
		m->foreach_key = ec_treetable_##NAME##_foreach_key_f;                                                                                                                                        \
		m->foreach_value = ec_treetable_##NAME##_foreach_value_f;                                                                                                                                    \
		m->iter_init = ec_treetable_##NAME##_iter_init_f;                                                                                                                                            \
		m->iter_next = ec_treetable_##NAME##_iter_next_f;                                                                                                                                            \
		m->iter_remove = ec_treetable_##NAME##_iter_remove_f;                                                                                                                                        \
		return EC_OK;                                                                                                                                                                                \
	}
/* Predefined expansions. Adjust as needed. */
EC_TREETABLE_DEF(int, int, int)
EC_TREETABLE_DEF(unsigned int, unsigned int, uint)
EC_TREETABLE_DEF(long, long, long)
EC_TREETABLE_DEF(long long, long long, ll)
EC_TREETABLE_DEF(unsigned long, unsigned long, ul)
EC_TREETABLE_DEF(unsigned long long, unsigned long long, ull)
EC_TREETABLE_DEF(uint32_t, uint32_t, u32)
EC_TREETABLE_DEF(uint64_t, uint64_t, u64)
EC_TREETABLE_DEF(double, double, dbl)
EC_TREETABLE_DEF(const char*, const char*, str)
EC_TREETABLE_DEF(void*, void*, ptr)

#endif
