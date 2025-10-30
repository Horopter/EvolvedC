#ifndef EC_DLIST_H
#define EC_DLIST_H
#include "ec_common.h"
#include "ec_status.h"
#ifdef EC_USE_MEMPOOL
#define EC_DLIST_ALLOC(TYPE, MPOOL) ((MPOOL) ? (TYPE*)ec_mempool_alloc((ec_mempool_t*)(MPOOL), sizeof(TYPE)) : (TYPE*)EC_MALLOC(sizeof(TYPE)))
#else
#define EC_DLIST_ALLOC(TYPE, MPOOL) (TYPE*)EC_MALLOC(sizeof(TYPE))
#endif
#define EC_DLIST_DEF(TYPE, NAME)                                                                                                                  \
	typedef struct ec_dlist_##NAME##_node_s                                                                                                       \
	{                                                                                                                                             \
		TYPE data;                                                                                                                                \
		struct ec_dlist_##NAME##_node_s* next;                                                                                                    \
		struct ec_dlist_##NAME##_node_s* prev;                                                                                                    \
	} ec_dlist_##NAME##_node_t;                                                                                                                   \
	typedef struct ec_dlist_##NAME##_s ec_dlist_##NAME##_t;                                                                                       \
	static inline ec_status_t ec_dlist_##NAME##_init(ec_dlist_##NAME##_t* l, void* mempool);                                                      \
	static inline void ec_dlist_##NAME##_init_functions(ec_dlist_##NAME##_t* l);                                                                  \
	typedef struct ec_dlist_##NAME##_iter_s                                                                                                       \
	{                                                                                                                                             \
		ec_dlist_##NAME##_t* list;                                                                                                                \
		ec_dlist_##NAME##_node_t* current;                                                                                                        \
		ec_dlist_##NAME##_node_t* prev;                                                                                                           \
		size_t idx;                                                                                                                               \
		bool dinit;                                                                                                                               \
	} ec_dlist_##NAME##_iter_t;                                                                                                                   \
	typedef struct ec_dlist_##NAME##_zip_iter_s                                                                                                   \
	{                                                                                                                                             \
		ec_dlist_##NAME##_t* l1;                                                                                                                  \
		ec_dlist_##NAME##_t* l2;                                                                                                                  \
		ec_dlist_##NAME##_node_t* c1;                                                                                                             \
		ec_dlist_##NAME##_node_t* c2;                                                                                                             \
		ec_dlist_##NAME##_node_t* p1;                                                                                                             \
		ec_dlist_##NAME##_node_t* p2;                                                                                                             \
		size_t idx;                                                                                                                               \
	} ec_dlist_##NAME##_zip_iter_t;                                                                                                               \
	struct ec_dlist_##NAME##_s                                                                                                                    \
	{                                                                                                                                             \
		ec_dlist_##NAME##_node_t *head, *tail;                                                                                                    \
		size_t size;                                                                                                                              \
		EC_MUTEX_TYPE lock;                                                                                                                       \
		void* mempool;                                                                                                                            \
		size_t (*struct_size)(ec_dlist_##NAME##_t*);                                                                                              \
		ec_status_t (*splice)(ec_dlist_##NAME##_t*, ec_dlist_##NAME##_t*);                                                                        \
		ec_status_t (*splice_at)(ec_dlist_##NAME##_t*, ec_dlist_##NAME##_t*, size_t);                                                             \
		ec_status_t (*add)(ec_dlist_##NAME##_t*, void*);                                                                                          \
		ec_status_t (*add_at)(ec_dlist_##NAME##_t*, void*, size_t);                                                                               \
		ec_status_t (*add_all)(ec_dlist_##NAME##_t*, ec_dlist_##NAME##_t*);                                                                       \
		ec_status_t (*add_all_at)(ec_dlist_##NAME##_t*, ec_dlist_##NAME##_t*, size_t);                                                            \
		ec_status_t (*add_first)(ec_dlist_##NAME##_t*, void*);                                                                                    \
		ec_status_t (*add_last)(ec_dlist_##NAME##_t*, void*);                                                                                     \
		ec_status_t (*remove_fn)(ec_dlist_##NAME##_t*, void*);                                                                                    \
		ec_status_t (*remove_first)(ec_dlist_##NAME##_t*);                                                                                        \
		ec_status_t (*remove_last)(ec_dlist_##NAME##_t*);                                                                                         \
		ec_status_t (*remove_at)(ec_dlist_##NAME##_t*, size_t);                                                                                   \
		ec_status_t (*remove_all)(ec_dlist_##NAME##_t*);                                                                                          \
		ec_status_t (*remove_all_cb)(ec_dlist_##NAME##_t*, void (*)(void*));                                                                      \
		ec_status_t (*get_at)(ec_dlist_##NAME##_t*, size_t, void**);                                                                              \
		ec_status_t (*get_first)(ec_dlist_##NAME##_t*, void**);                                                                                   \
		ec_status_t (*get_last)(ec_dlist_##NAME##_t*, void**);                                                                                    \
		ec_status_t (*sublist)(ec_dlist_##NAME##_t*, size_t, size_t, ec_dlist_##NAME##_t**);                                                      \
		ec_status_t (*copy_shallow)(ec_dlist_##NAME##_t*, ec_dlist_##NAME##_t**);                                                                 \
		ec_status_t (*copy_deep)(ec_dlist_##NAME##_t*, void* (*)(void*), ec_dlist_##NAME##_t**);                                                  \
		ec_status_t (*replace_at)(ec_dlist_##NAME##_t*, void*, size_t);                                                                           \
		size_t (*contains)(ec_dlist_##NAME##_t*, void*);                                                                                          \
		size_t (*contains_value)(ec_dlist_##NAME##_t*, void*, int (*)(const void*, const void*));                                                 \
		ec_status_t (*index_of)(ec_dlist_##NAME##_t*, void*, int (*)(const void*, const void*), size_t*);                                         \
		ec_status_t (*to_array)(ec_dlist_##NAME##_t*, void***);                                                                                   \
		void (*reverse)(ec_dlist_##NAME##_t*);                                                                                                    \
		ec_status_t (*sort)(ec_dlist_##NAME##_t*, int (*)(const void*, const void*));                                                             \
		void (*sort_in_place)(ec_dlist_##NAME##_t*, int (*)(const void*, const void*));                                                           \
		size_t (*size_fn)(ec_dlist_##NAME##_t*);                                                                                                  \
		void (*foreach)(ec_dlist_##NAME##_t*, void (*)(void*));                                                                                   \
		ec_status_t (*reduce)(ec_dlist_##NAME##_t*, void (*)(void*, void*, void*), void*);                                                        \
		ec_status_t (*filter_mut)(ec_dlist_##NAME##_t*, bool (*)(const void*));                                                                   \
		ec_status_t (*filter)(ec_dlist_##NAME##_t*, bool (*)(const void*), ec_dlist_##NAME##_t**);                                                \
		void (*iter_init)(ec_dlist_##NAME##_iter_t*, ec_dlist_##NAME##_t*);                                                                       \
		ec_status_t (*iter_remove)(ec_dlist_##NAME##_iter_t*);                                                                                    \
		ec_status_t (*iter_add)(ec_dlist_##NAME##_iter_t*, void*);                                                                                \
		ec_status_t (*iter_replace)(ec_dlist_##NAME##_iter_t*, void*, void**);                                                                    \
		size_t (*iter_index)(ec_dlist_##NAME##_iter_t*);                                                                                          \
		ec_status_t (*iter_next)(ec_dlist_##NAME##_iter_t*, void**);                                                                              \
		void (*zip_iter_init)(ec_dlist_##NAME##_zip_iter_t*, ec_dlist_##NAME##_t*, ec_dlist_##NAME##_t*);                                         \
		ec_status_t (*zip_iter_next)(ec_dlist_##NAME##_zip_iter_t*, void**, void**);                                                              \
		ec_status_t (*zip_iter_add)(ec_dlist_##NAME##_zip_iter_t*, void*, void*);                                                                 \
		ec_status_t (*zip_iter_remove)(ec_dlist_##NAME##_zip_iter_t*, void**, void**);                                                            \
		ec_status_t (*zip_iter_replace)(ec_dlist_##NAME##_zip_iter_t*, void*, void*, void**, void**);                                             \
		size_t (*zip_iter_index)(ec_dlist_##NAME##_zip_iter_t*);                                                                                  \
		ec_status_t (*free_fn)(ec_dlist_##NAME##_t*);                                                                                             \
	};                                                                                                                                            \
	static inline void ec_dlist_##NAME##_lock(ec_dlist_##NAME##_t* l)                                                                             \
	{                                                                                                                                             \
		EC_MUTEX_LOCK(l->lock);                                                                                                                   \
	}                                                                                                                                             \
	static inline void ec_dlist_##NAME##_unlock(ec_dlist_##NAME##_t* l)                                                                           \
	{                                                                                                                                             \
		EC_MUTEX_UNLOCK(l->lock);                                                                                                                 \
	}                                                                                                                                             \
	static inline size_t ec_dlist_##NAME##_struct_size(ec_dlist_##NAME##_t* l)                                                                    \
	{                                                                                                                                             \
		(void)l;                                                                                                                                  \
		return sizeof(ec_dlist_##NAME##_t);                                                                                                       \
	}                                                                                                                                             \
	static inline ec_dlist_##NAME##_node_t* ec_dlist_##NAME##_make_node(ec_dlist_##NAME##_t* l, TYPE d)                                           \
	{                                                                                                                                             \
		ec_dlist_##NAME##_node_t* n = EC_DLIST_ALLOC(ec_dlist_##NAME##_node_t, l->mempool);                                                       \
		if (n)                                                                                                                                    \
			{                                                                                                                                     \
				n->data = d;                                                                                                                      \
				n->next = NULL;                                                                                                                   \
				n->prev = NULL;                                                                                                                   \
			}                                                                                                                                     \
		return n;                                                                                                                                 \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_splice(ec_dlist_##NAME##_t* l1, ec_dlist_##NAME##_t* l2)                                          \
	{                                                                                                                                             \
		if (!l1 || !l2)                                                                                                                           \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l1);                                                                                                               \
		ec_dlist_##NAME##_lock(l2);                                                                                                               \
		if (!l2->head)                                                                                                                            \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l2);                                                                                                     \
				ec_dlist_##NAME##_unlock(l1);                                                                                                     \
				return EC_OK;                                                                                                                     \
			}                                                                                                                                     \
		if (!l1->head)                                                                                                                            \
			{                                                                                                                                     \
				l1->head = l2->head;                                                                                                              \
				l1->tail = l2->tail;                                                                                                              \
				l1->size = l2->size;                                                                                                              \
			}                                                                                                                                     \
		else                                                                                                                                      \
			{                                                                                                                                     \
				l1->tail->next = l2->head;                                                                                                        \
				l2->head->prev = l1->tail;                                                                                                        \
				l1->tail = l2->tail;                                                                                                              \
				l1->size += l2->size;                                                                                                             \
			}                                                                                                                                     \
		l2->head = l2->tail = NULL;                                                                                                               \
		l2->size = 0;                                                                                                                             \
		ec_dlist_##NAME##_unlock(l2);                                                                                                             \
		ec_dlist_##NAME##_unlock(l1);                                                                                                             \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_splice_at(ec_dlist_##NAME##_t* l, ec_dlist_##NAME##_t* l2, size_t idx)                            \
	{                                                                                                                                             \
		if (!l || !l2)                                                                                                                            \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		ec_dlist_##NAME##_lock(l2);                                                                                                               \
		if (idx > l->size)                                                                                                                        \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l2);                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_ERR_OUT_OF_BOUNDS;                                                                                                      \
			}                                                                                                                                     \
		if (!l2->head)                                                                                                                            \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l2);                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_OK;                                                                                                                     \
			}                                                                                                                                     \
		if (idx == 0)                                                                                                                             \
			{                                                                                                                                     \
				l2->tail->next = l->head;                                                                                                         \
				if (l->head)                                                                                                                      \
					l->head->prev = l2->tail;                                                                                                     \
				l->head = l2->head;                                                                                                               \
				if (l->size == 0)                                                                                                                 \
					l->tail = l2->tail;                                                                                                           \
				l->size += l2->size;                                                                                                              \
			}                                                                                                                                     \
		else if (idx == l->size)                                                                                                                  \
			{                                                                                                                                     \
				l->tail->next = l2->head;                                                                                                         \
				l2->head->prev = l->tail;                                                                                                         \
				l->tail = l2->tail;                                                                                                               \
				l->size += l2->size;                                                                                                              \
			}                                                                                                                                     \
		else                                                                                                                                      \
			{                                                                                                                                     \
				ec_dlist_##NAME##_node_t* p = l->head;                                                                                            \
				for (size_t i = 0; i < idx - 1; i++)                                                                                              \
					p = p->next;                                                                                                                  \
				ec_dlist_##NAME##_node_t* nxt = p->next;                                                                                          \
				p->next = l2->head;                                                                                                               \
				l2->head->prev = p;                                                                                                               \
				l2->tail->next = nxt;                                                                                                             \
				if (nxt)                                                                                                                          \
					nxt->prev = l2->tail;                                                                                                         \
				l->size += l2->size;                                                                                                              \
			}                                                                                                                                     \
		l2->head = l2->tail = NULL;                                                                                                               \
		l2->size = 0;                                                                                                                             \
		ec_dlist_##NAME##_unlock(l2);                                                                                                             \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_add(ec_dlist_##NAME##_t* l, void* e)                                                              \
	{                                                                                                                                             \
		if (!l)                                                                                                                                   \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		ec_dlist_##NAME##_node_t* n = ec_dlist_##NAME##_make_node(l, *(TYPE*)e);                                                                  \
		if (!n)                                                                                                                                   \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_ERR_ALLOC;                                                                                                              \
			}                                                                                                                                     \
		if (!l->head)                                                                                                                             \
			{                                                                                                                                     \
				l->head = l->tail = n;                                                                                                            \
			}                                                                                                                                     \
		else                                                                                                                                      \
			{                                                                                                                                     \
				l->tail->next = n;                                                                                                                \
				n->prev = l->tail;                                                                                                                \
				l->tail = n;                                                                                                                      \
			}                                                                                                                                     \
		l->size++;                                                                                                                                \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_add_at(ec_dlist_##NAME##_t* l, void* e, size_t idx)                                               \
	{                                                                                                                                             \
		if (!l)                                                                                                                                   \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		if (idx > l->size)                                                                                                                        \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_ERR_OUT_OF_BOUNDS;                                                                                                      \
			}                                                                                                                                     \
		ec_dlist_##NAME##_node_t* n = ec_dlist_##NAME##_make_node(l, *(TYPE*)e);                                                                  \
		if (!n)                                                                                                                                   \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_ERR_ALLOC;                                                                                                              \
			}                                                                                                                                     \
		if (idx == 0)                                                                                                                             \
			{                                                                                                                                     \
				n->next = l->head;                                                                                                                \
				if (l->head)                                                                                                                      \
					l->head->prev = n;                                                                                                            \
				l->head = n;                                                                                                                      \
				if (l->size == 0)                                                                                                                 \
					l->tail = n;                                                                                                                  \
			}                                                                                                                                     \
		else if (idx == l->size)                                                                                                                  \
			{                                                                                                                                     \
				l->tail->next = n;                                                                                                                \
				n->prev = l->tail;                                                                                                                \
				l->tail = n;                                                                                                                      \
			}                                                                                                                                     \
		else                                                                                                                                      \
			{                                                                                                                                     \
				ec_dlist_##NAME##_node_t* p = l->head;                                                                                            \
				for (size_t i = 0; i < idx - 1; i++)                                                                                              \
					p = p->next;                                                                                                                  \
				n->next = p->next;                                                                                                                \
				n->prev = p;                                                                                                                      \
				p->next->prev = n;                                                                                                                \
				p->next = n;                                                                                                                      \
			}                                                                                                                                     \
		l->size++;                                                                                                                                \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_add_all(ec_dlist_##NAME##_t* l1, ec_dlist_##NAME##_t* l2)                                         \
	{                                                                                                                                             \
		if (!l1 || !l2)                                                                                                                           \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l1);                                                                                                               \
		ec_dlist_##NAME##_lock(l2);                                                                                                               \
		ec_dlist_##NAME##_node_t* c = l2->head;                                                                                                   \
		while (c)                                                                                                                                 \
			{                                                                                                                                     \
				ec_dlist_##NAME##_node_t* n = ec_dlist_##NAME##_make_node(l1, c->data);                                                           \
				if (!n)                                                                                                                           \
					{                                                                                                                             \
						ec_dlist_##NAME##_unlock(l2);                                                                                             \
						ec_dlist_##NAME##_unlock(l1);                                                                                             \
						return EC_ERR_ALLOC;                                                                                                      \
					}                                                                                                                             \
				if (!l1->head)                                                                                                                    \
					{                                                                                                                             \
						l1->head = l1->tail = n;                                                                                                  \
					}                                                                                                                             \
				else                                                                                                                              \
					{                                                                                                                             \
						l1->tail->next = n;                                                                                                       \
						n->prev = l1->tail;                                                                                                       \
						l1->tail = n;                                                                                                             \
					}                                                                                                                             \
				l1->size++;                                                                                                                       \
				c = c->next;                                                                                                                      \
			}                                                                                                                                     \
		ec_dlist_##NAME##_unlock(l2);                                                                                                             \
		ec_dlist_##NAME##_unlock(l1);                                                                                                             \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_add_all_at(ec_dlist_##NAME##_t* l, ec_dlist_##NAME##_t* l2, size_t idx)                           \
	{                                                                                                                                             \
		if (!l || !l2)                                                                                                                            \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		ec_dlist_##NAME##_lock(l2);                                                                                                               \
		if (idx > l->size)                                                                                                                        \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l2);                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_ERR_OUT_OF_BOUNDS;                                                                                                      \
			}                                                                                                                                     \
		ec_dlist_##NAME##_node_t *dh = NULL, *dt = NULL, *c = l2->head;                                                                           \
		while (c)                                                                                                                                 \
			{                                                                                                                                     \
				ec_dlist_##NAME##_node_t* n = ec_dlist_##NAME##_make_node(l, c->data);                                                            \
				if (!n)                                                                                                                           \
					{                                                                                                                             \
						ec_dlist_##NAME##_node_t* x = dh;                                                                                         \
						while (x)                                                                                                                 \
							{                                                                                                                     \
								ec_dlist_##NAME##_node_t* y = x->next;                                                                            \
								EC_FREE(x);                                                                                                       \
								x = y;                                                                                                            \
							}                                                                                                                     \
						ec_dlist_##NAME##_unlock(l2);                                                                                             \
						ec_dlist_##NAME##_unlock(l);                                                                                              \
						return EC_ERR_ALLOC;                                                                                                      \
					}                                                                                                                             \
				if (!dh)                                                                                                                          \
					{                                                                                                                             \
						dh = n;                                                                                                                   \
						dt = n;                                                                                                                   \
					}                                                                                                                             \
				else                                                                                                                              \
					{                                                                                                                             \
						dt->next = n;                                                                                                             \
						n->prev = dt;                                                                                                             \
						dt = n;                                                                                                                   \
					}                                                                                                                             \
				c = c->next;                                                                                                                      \
			}                                                                                                                                     \
		if (idx == 0)                                                                                                                             \
			{                                                                                                                                     \
				if (l->head)                                                                                                                      \
					{                                                                                                                             \
						dt->next = l->head;                                                                                                       \
						l->head->prev = dt;                                                                                                       \
					}                                                                                                                             \
				l->head = dh;                                                                                                                     \
				if (l->size == 0)                                                                                                                 \
					l->tail = dt;                                                                                                                 \
			}                                                                                                                                     \
		else if (idx == l->size)                                                                                                                  \
			{                                                                                                                                     \
				l->tail->next = dh;                                                                                                               \
				dh->prev = l->tail;                                                                                                               \
				l->tail = dt;                                                                                                                     \
			}                                                                                                                                     \
		else                                                                                                                                      \
			{                                                                                                                                     \
				ec_dlist_##NAME##_node_t* p = l->head;                                                                                            \
				for (size_t i = 0; i < idx - 1; i++)                                                                                              \
					p = p->next;                                                                                                                  \
				ec_dlist_##NAME##_node_t* nxt = p->next;                                                                                          \
				p->next = dh;                                                                                                                     \
				dh->prev = p;                                                                                                                     \
				dt->next = nxt;                                                                                                                   \
				if (nxt)                                                                                                                          \
					nxt->prev = dt;                                                                                                               \
			}                                                                                                                                     \
		l->size += l2->size;                                                                                                                      \
		ec_dlist_##NAME##_unlock(l2);                                                                                                             \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_add_first(ec_dlist_##NAME##_t* l, void* e)                                                        \
	{                                                                                                                                             \
		return ec_dlist_##NAME##_add_at(l, e, 0);                                                                                                 \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_add_last(ec_dlist_##NAME##_t* l, void* e)                                                         \
	{                                                                                                                                             \
		return ec_dlist_##NAME##_add(l, e);                                                                                                       \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_remove_fn(ec_dlist_##NAME##_t* l, void* e)                                                        \
	{                                                                                                                                             \
		if (!l)                                                                                                                                   \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		ec_dlist_##NAME##_node_t* c = l->head;                                                                                                    \
		while (c)                                                                                                                                 \
			{                                                                                                                                     \
				if (c->data == *(TYPE*)e)                                                                                                         \
					{                                                                                                                             \
						if (c->prev)                                                                                                              \
							c->prev->next = c->next;                                                                                              \
						else                                                                                                                      \
							l->head = c->next;                                                                                                    \
						if (c->next)                                                                                                              \
							c->next->prev = c->prev;                                                                                              \
						else                                                                                                                      \
							l->tail = c->prev;                                                                                                    \
						l->size--;                                                                                                                \
						EC_FREE(c);                                                                                                               \
						ec_dlist_##NAME##_unlock(l);                                                                                              \
						return EC_OK;                                                                                                             \
					}                                                                                                                             \
				c = c->next;                                                                                                                      \
			}                                                                                                                                     \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_ERR_NOT_FOUND;                                                                                                                  \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_remove_first(ec_dlist_##NAME##_t* l)                                                              \
	{                                                                                                                                             \
		if (!l)                                                                                                                                   \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		if (!l->head)                                                                                                                             \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_ERR_OUT_OF_BOUNDS;                                                                                                      \
			}                                                                                                                                     \
		ec_dlist_##NAME##_node_t* h = l->head;                                                                                                    \
		l->head = h->next;                                                                                                                        \
		if (l->head)                                                                                                                              \
			l->head->prev = NULL;                                                                                                                 \
		else                                                                                                                                      \
			l->tail = NULL;                                                                                                                       \
		l->size--;                                                                                                                                \
		EC_FREE(h);                                                                                                                               \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_remove_last(ec_dlist_##NAME##_t* l)                                                               \
	{                                                                                                                                             \
		if (!l)                                                                                                                                   \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		if (!l->tail)                                                                                                                             \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_ERR_OUT_OF_BOUNDS;                                                                                                      \
			}                                                                                                                                     \
		ec_dlist_##NAME##_node_t* t = l->tail;                                                                                                    \
		l->tail = t->prev;                                                                                                                        \
		if (l->tail)                                                                                                                              \
			l->tail->next = NULL;                                                                                                                 \
		else                                                                                                                                      \
			l->head = NULL;                                                                                                                       \
		l->size--;                                                                                                                                \
		EC_FREE(t);                                                                                                                               \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_remove_at(ec_dlist_##NAME##_t* l, size_t idx)                                                     \
	{                                                                                                                                             \
		if (!l)                                                                                                                                   \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		if (idx >= l->size)                                                                                                                       \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_ERR_OUT_OF_BOUNDS;                                                                                                      \
			}                                                                                                                                     \
		ec_dlist_##NAME##_node_t* c = l->head;                                                                                                    \
		for (size_t i = 0; i < idx; i++)                                                                                                          \
			c = c->next;                                                                                                                          \
		if (c->prev)                                                                                                                              \
			c->prev->next = c->next;                                                                                                              \
		else                                                                                                                                      \
			l->head = c->next;                                                                                                                    \
		if (c->next)                                                                                                                              \
			c->next->prev = c->prev;                                                                                                              \
		else                                                                                                                                      \
			l->tail = c->prev;                                                                                                                    \
		l->size--;                                                                                                                                \
		EC_FREE(c);                                                                                                                               \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_remove_all(ec_dlist_##NAME##_t* l)                                                                \
	{                                                                                                                                             \
		if (!l)                                                                                                                                   \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		ec_dlist_##NAME##_node_t* c = l->head;                                                                                                    \
		while (c)                                                                                                                                 \
			{                                                                                                                                     \
				ec_dlist_##NAME##_node_t* n = c->next;                                                                                            \
				EC_FREE(c);                                                                                                                       \
				c = n;                                                                                                                            \
			}                                                                                                                                     \
		l->head = l->tail = NULL;                                                                                                                 \
		l->size = 0;                                                                                                                              \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_remove_all_cb(ec_dlist_##NAME##_t* l, void (*cb)(void*))                                          \
	{                                                                                                                                             \
		if (!l)                                                                                                                                   \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		ec_dlist_##NAME##_node_t* c = l->head;                                                                                                    \
		while (c)                                                                                                                                 \
			{                                                                                                                                     \
				ec_dlist_##NAME##_node_t* n = c->next;                                                                                            \
				if (cb)                                                                                                                           \
					cb(&c->data);                                                                                                                 \
				EC_FREE(c);                                                                                                                       \
				c = n;                                                                                                                            \
			}                                                                                                                                     \
		l->head = l->tail = NULL;                                                                                                                 \
		l->size = 0;                                                                                                                              \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_get_at(ec_dlist_##NAME##_t* l, size_t idx, void** out)                                            \
	{                                                                                                                                             \
		if (!l || !out)                                                                                                                           \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		if (idx >= l->size)                                                                                                                       \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_ERR_OUT_OF_BOUNDS;                                                                                                      \
			}                                                                                                                                     \
		ec_dlist_##NAME##_node_t* c = l->head;                                                                                                    \
		for (size_t i = 0; i < idx; i++)                                                                                                          \
			c = c->next;                                                                                                                          \
		*out = &c->data;                                                                                                                          \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_get_first(ec_dlist_##NAME##_t* l, void** out)                                                     \
	{                                                                                                                                             \
		if (!l || !out)                                                                                                                           \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		if (!l->head)                                                                                                                             \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_ERR_OUT_OF_BOUNDS;                                                                                                      \
			}                                                                                                                                     \
		*out = &l->head->data;                                                                                                                    \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_get_last(ec_dlist_##NAME##_t* l, void** out)                                                      \
	{                                                                                                                                             \
		if (!l || !out)                                                                                                                           \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		if (!l->tail)                                                                                                                             \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_ERR_OUT_OF_BOUNDS;                                                                                                      \
			}                                                                                                                                     \
		*out = &l->tail->data;                                                                                                                    \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_sublist(ec_dlist_##NAME##_t* l, size_t f, size_t t, ec_dlist_##NAME##_t** out)                    \
	{                                                                                                                                             \
		if (!l || !out)                                                                                                                           \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		if (f > t)                                                                                                                                \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		if (t > l->size)                                                                                                                          \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_ERR_OUT_OF_BOUNDS;                                                                                                      \
			}                                                                                                                                     \
		ec_dlist_##NAME##_t* sub = EC_DLIST_ALLOC(ec_dlist_##NAME##_t, l->mempool);                                                               \
		if (!sub)                                                                                                                                 \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_ERR_ALLOC;                                                                                                              \
			}                                                                                                                                     \
		ec_status_t s = ec_dlist_##NAME##_init(sub, l->mempool);                                                                                  \
		if (s != EC_OK)                                                                                                                           \
			{                                                                                                                                     \
				EC_FREE(sub);                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return s;                                                                                                                         \
			}                                                                                                                                     \
		ec_dlist_##NAME##_node_t* c = l->head;                                                                                                    \
		for (size_t i = 0; i < f; i++)                                                                                                            \
			c = c->next;                                                                                                                          \
		for (size_t i = f; i < t; i++)                                                                                                            \
			{                                                                                                                                     \
				ec_dlist_##NAME##_node_t* n = ec_dlist_##NAME##_make_node(sub, c->data);                                                          \
				if (!n)                                                                                                                           \
					{                                                                                                                             \
						ec_dlist_##NAME##_unlock(l);                                                                                              \
						ec_dlist_##NAME##_node_t* tmp = sub->head;                                                                                \
						while (tmp)                                                                                                               \
							{                                                                                                                     \
								ec_dlist_##NAME##_node_t* nx = tmp->next;                                                                         \
								EC_FREE(tmp);                                                                                                     \
								tmp = nx;                                                                                                         \
							}                                                                                                                     \
						sub->head = sub->tail = NULL;                                                                                             \
						sub->size = 0;                                                                                                            \
						sub->free_fn(sub);                                                                                                        \
						EC_FREE(sub);                                                                                                             \
						return EC_ERR_ALLOC;                                                                                                      \
					}                                                                                                                             \
				if (!sub->head)                                                                                                                   \
					{                                                                                                                             \
						sub->head = sub->tail = n;                                                                                                \
					}                                                                                                                             \
				else                                                                                                                              \
					{                                                                                                                             \
						sub->tail->next = n;                                                                                                      \
						n->prev = sub->tail;                                                                                                      \
						sub->tail = n;                                                                                                            \
					}                                                                                                                             \
				sub->size++;                                                                                                                      \
				c = c->next;                                                                                                                      \
			}                                                                                                                                     \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		*out = sub;                                                                                                                               \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_copy_shallow(ec_dlist_##NAME##_t* l, ec_dlist_##NAME##_t** out)                                   \
	{                                                                                                                                             \
		if (!l || !out)                                                                                                                           \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		ec_dlist_##NAME##_t* cp = EC_DLIST_ALLOC(ec_dlist_##NAME##_t, l->mempool);                                                                \
		if (!cp)                                                                                                                                  \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_ERR_ALLOC;                                                                                                              \
			}                                                                                                                                     \
		ec_status_t s = ec_dlist_##NAME##_init(cp, l->mempool);                                                                                   \
		if (s != EC_OK)                                                                                                                           \
			{                                                                                                                                     \
				EC_FREE(cp);                                                                                                                      \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return s;                                                                                                                         \
			}                                                                                                                                     \
		ec_dlist_##NAME##_node_t* c = l->head;                                                                                                    \
		while (c)                                                                                                                                 \
			{                                                                                                                                     \
				ec_dlist_##NAME##_node_t* n = ec_dlist_##NAME##_make_node(cp, c->data);                                                           \
				if (!n)                                                                                                                           \
					{                                                                                                                             \
						cp->free_fn(cp);                                                                                                          \
						EC_FREE(cp);                                                                                                              \
						ec_dlist_##NAME##_unlock(l);                                                                                              \
						return EC_ERR_ALLOC;                                                                                                      \
					}                                                                                                                             \
				if (!cp->head)                                                                                                                    \
					{                                                                                                                             \
						cp->head = cp->tail = n;                                                                                                  \
					}                                                                                                                             \
				else                                                                                                                              \
					{                                                                                                                             \
						cp->tail->next = n;                                                                                                       \
						n->prev = cp->tail;                                                                                                       \
						cp->tail = n;                                                                                                             \
					}                                                                                                                             \
				cp->size++;                                                                                                                       \
				c = c->next;                                                                                                                      \
			}                                                                                                                                     \
		*out = cp;                                                                                                                                \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_copy_deep(ec_dlist_##NAME##_t* l, void* (*cpfn)(void*), ec_dlist_##NAME##_t** out)                \
	{                                                                                                                                             \
		if (!l || !cpfn || !out)                                                                                                                  \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		ec_dlist_##NAME##_t* cp = EC_DLIST_ALLOC(ec_dlist_##NAME##_t, l->mempool);                                                                \
		if (!cp)                                                                                                                                  \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_ERR_ALLOC;                                                                                                              \
			}                                                                                                                                     \
		ec_status_t s = ec_dlist_##NAME##_init(cp, l->mempool);                                                                                   \
		if (s != EC_OK)                                                                                                                           \
			{                                                                                                                                     \
				EC_FREE(cp);                                                                                                                      \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return s;                                                                                                                         \
			}                                                                                                                                     \
		ec_dlist_##NAME##_node_t* c = l->head;                                                                                                    \
		while (c)                                                                                                                                 \
			{                                                                                                                                     \
				void* dup = cpfn(&c->data);                                                                                                       \
				if (!dup)                                                                                                                         \
					{                                                                                                                             \
						cp->free_fn(cp);                                                                                                          \
						EC_FREE(cp);                                                                                                              \
						ec_dlist_##NAME##_unlock(l);                                                                                              \
						return EC_ERR_ALLOC;                                                                                                      \
					}                                                                                                                             \
				ec_dlist_##NAME##_node_t* n = ec_dlist_##NAME##_make_node(cp, *(TYPE*)dup);                                                       \
				EC_FREE(dup);                                                                                                                     \
				if (!n)                                                                                                                           \
					{                                                                                                                             \
						cp->free_fn(cp);                                                                                                          \
						EC_FREE(cp);                                                                                                              \
						ec_dlist_##NAME##_unlock(l);                                                                                              \
						return EC_ERR_ALLOC;                                                                                                      \
					}                                                                                                                             \
				if (!cp->head)                                                                                                                    \
					{                                                                                                                             \
						cp->head = cp->tail = n;                                                                                                  \
					}                                                                                                                             \
				else                                                                                                                              \
					{                                                                                                                             \
						cp->tail->next = n;                                                                                                       \
						n->prev = cp->tail;                                                                                                       \
						cp->tail = n;                                                                                                             \
					}                                                                                                                             \
				cp->size++;                                                                                                                       \
				c = c->next;                                                                                                                      \
			}                                                                                                                                     \
		*out = cp;                                                                                                                                \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_replace_at(ec_dlist_##NAME##_t* l, void* e, size_t idx)                                           \
	{                                                                                                                                             \
		if (!l)                                                                                                                                   \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		if (idx >= l->size)                                                                                                                       \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_ERR_OUT_OF_BOUNDS;                                                                                                      \
			}                                                                                                                                     \
		ec_dlist_##NAME##_node_t* c = l->head;                                                                                                    \
		for (size_t i = 0; i < idx; i++)                                                                                                          \
			c = c->next;                                                                                                                          \
		c->data = *(TYPE*)e;                                                                                                                      \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline size_t ec_dlist_##NAME##_contains(ec_dlist_##NAME##_t* l, void* e)                                                              \
	{                                                                                                                                             \
		if (!l)                                                                                                                                   \
			return 0;                                                                                                                             \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		ec_dlist_##NAME##_node_t* c = l->head;                                                                                                    \
		while (c)                                                                                                                                 \
			{                                                                                                                                     \
				if (c->data == *(TYPE*)e)                                                                                                         \
					{                                                                                                                             \
						ec_dlist_##NAME##_unlock(l);                                                                                              \
						return 1;                                                                                                                 \
					}                                                                                                                             \
				c = c->next;                                                                                                                      \
			}                                                                                                                                     \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return 0;                                                                                                                                 \
	}                                                                                                                                             \
	static inline size_t ec_dlist_##NAME##_contains_value(ec_dlist_##NAME##_t* l, void* e, int (*cmp)(const void*, const void*))                  \
	{                                                                                                                                             \
		if (!l || !cmp)                                                                                                                           \
			return 0;                                                                                                                             \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		ec_dlist_##NAME##_node_t* c = l->head;                                                                                                    \
		while (c)                                                                                                                                 \
			{                                                                                                                                     \
				if (cmp(&c->data, e) == 0)                                                                                                        \
					{                                                                                                                             \
						ec_dlist_##NAME##_unlock(l);                                                                                              \
						return 1;                                                                                                                 \
					}                                                                                                                             \
				c = c->next;                                                                                                                      \
			}                                                                                                                                     \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return 0;                                                                                                                                 \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_index_of(ec_dlist_##NAME##_t* l, void* e, int (*cmp)(const void*, const void*), size_t* outidx)   \
	{                                                                                                                                             \
		if (!l || !outidx || !cmp)                                                                                                                \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		size_t i = 0;                                                                                                                             \
		ec_dlist_##NAME##_node_t* c = l->head;                                                                                                    \
		while (c)                                                                                                                                 \
			{                                                                                                                                     \
				if (cmp(&c->data, e) == 0)                                                                                                        \
					{                                                                                                                             \
						*outidx = i;                                                                                                              \
						ec_dlist_##NAME##_unlock(l);                                                                                              \
						return EC_OK;                                                                                                             \
					}                                                                                                                             \
				i++;                                                                                                                              \
				c = c->next;                                                                                                                      \
			}                                                                                                                                     \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_ERR_NOT_FOUND;                                                                                                                  \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_to_array(ec_dlist_##NAME##_t* l, void*** out)                                                     \
	{                                                                                                                                             \
		if (!l || !out)                                                                                                                           \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		*out = (void**)EC_MALLOC(sizeof(void*) * l->size);                                                                                        \
		if (!*out)                                                                                                                                \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_ERR_ALLOC;                                                                                                              \
			}                                                                                                                                     \
		ec_dlist_##NAME##_node_t* c = l->head;                                                                                                    \
		size_t i = 0;                                                                                                                             \
		while (c)                                                                                                                                 \
			{                                                                                                                                     \
				(*out)[i++] = &c->data;                                                                                                           \
				c = c->next;                                                                                                                      \
			}                                                                                                                                     \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline void ec_dlist_##NAME##_reverse(ec_dlist_##NAME##_t* l)                                                                          \
	{                                                                                                                                             \
		if (!l)                                                                                                                                   \
			return;                                                                                                                               \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		ec_dlist_##NAME##_node_t *cur = l->head, *temp = NULL;                                                                                    \
		l->tail = l->head;                                                                                                                        \
		while (cur)                                                                                                                               \
			{                                                                                                                                     \
				temp = cur->prev;                                                                                                                 \
				cur->prev = cur->next;                                                                                                            \
				cur->next = temp;                                                                                                                 \
				cur = cur->prev;                                                                                                                  \
			}                                                                                                                                     \
		if (temp)                                                                                                                                 \
			l->head = temp->prev;                                                                                                                 \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_sort(ec_dlist_##NAME##_t* l, int (*cmp)(const void*, const void*))                                \
	{                                                                                                                                             \
		if (!l || !cmp)                                                                                                                           \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		if (l->size < 2)                                                                                                                          \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_OK;                                                                                                                     \
			}                                                                                                                                     \
		TYPE* arr = (TYPE*)EC_MALLOC(l->size * sizeof(TYPE));                                                                                     \
		if (!arr)                                                                                                                                 \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_ERR_ALLOC;                                                                                                              \
			}                                                                                                                                     \
		ec_dlist_##NAME##_node_t* c = l->head;                                                                                                    \
		size_t i = 0;                                                                                                                             \
		while (c)                                                                                                                                 \
			{                                                                                                                                     \
				arr[i++] = c->data;                                                                                                               \
				c = c->next;                                                                                                                      \
			}                                                                                                                                     \
		qsort(arr, l->size, sizeof(TYPE), cmp);                                                                                                   \
		c = l->head;                                                                                                                              \
		i = 0;                                                                                                                                    \
		while (c)                                                                                                                                 \
			{                                                                                                                                     \
				c->data = arr[i++];                                                                                                               \
				c = c->next;                                                                                                                      \
			}                                                                                                                                     \
		EC_FREE(arr);                                                                                                                             \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline void ec_dlist_##NAME##_sort_in_place(ec_dlist_##NAME##_t* l, int (*cmp)(const void*, const void*))                              \
	{                                                                                                                                             \
		ec_dlist_##NAME##_sort(l, cmp);                                                                                                           \
	}                                                                                                                                             \
	static inline size_t ec_dlist_##NAME##_size_fn(ec_dlist_##NAME##_t* l)                                                                        \
	{                                                                                                                                             \
		if (!l)                                                                                                                                   \
			return 0;                                                                                                                             \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		size_t s = l->size;                                                                                                                       \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return s;                                                                                                                                 \
	}                                                                                                                                             \
	static inline void ec_dlist_##NAME##_foreach(ec_dlist_##NAME##_t* l, void (*op)(void*))                                                       \
	{                                                                                                                                             \
		if (!l || !op)                                                                                                                            \
			return;                                                                                                                               \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		ec_dlist_##NAME##_node_t* c = l->head;                                                                                                    \
		while (c)                                                                                                                                 \
			{                                                                                                                                     \
				op(&c->data);                                                                                                                     \
				c = c->next;                                                                                                                      \
			}                                                                                                                                     \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_reduce(ec_dlist_##NAME##_t* l, void (*fn)(void*, void*, void*), void* res)                        \
	{                                                                                                                                             \
		if (!l || !fn || !res)                                                                                                                    \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		if (!l->size)                                                                                                                             \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_OK;                                                                                                                     \
			}                                                                                                                                     \
		ec_dlist_##NAME##_node_t* c = l->head;                                                                                                    \
		*(TYPE*)res = c->data;                                                                                                                    \
		c = c->next;                                                                                                                              \
		while (c)                                                                                                                                 \
			{                                                                                                                                     \
				fn(res, &c->data, res);                                                                                                           \
				c = c->next;                                                                                                                      \
			}                                                                                                                                     \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_filter_mut(ec_dlist_##NAME##_t* l, bool (*p)(const void*))                                        \
	{                                                                                                                                             \
		if (!l || !p)                                                                                                                             \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		ec_dlist_##NAME##_node_t* c = l->head;                                                                                                    \
		while (c)                                                                                                                                 \
			{                                                                                                                                     \
				if (!p(&c->data))                                                                                                                 \
					{                                                                                                                             \
						ec_dlist_##NAME##_node_t* nxt = c->next;                                                                                  \
						if (c->prev)                                                                                                              \
							c->prev->next = c->next;                                                                                              \
						else                                                                                                                      \
							l->head = c->next;                                                                                                    \
						if (c->next)                                                                                                              \
							c->next->prev = c->prev;                                                                                              \
						else                                                                                                                      \
							l->tail = c->prev;                                                                                                    \
						l->size--;                                                                                                                \
						EC_FREE(c);                                                                                                               \
						c = nxt;                                                                                                                  \
					}                                                                                                                             \
				else                                                                                                                              \
					c = c->next;                                                                                                                  \
			}                                                                                                                                     \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_filter(ec_dlist_##NAME##_t* l, bool (*p)(const void*), ec_dlist_##NAME##_t** out)                 \
	{                                                                                                                                             \
		if (!l || !p || !out)                                                                                                                     \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		ec_dlist_##NAME##_t* fl = EC_DLIST_ALLOC(ec_dlist_##NAME##_t, l->mempool);                                                                \
		if (!fl)                                                                                                                                  \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return EC_ERR_ALLOC;                                                                                                              \
			}                                                                                                                                     \
		ec_status_t s = ec_dlist_##NAME##_init(fl, l->mempool);                                                                                   \
		if (s != EC_OK)                                                                                                                           \
			{                                                                                                                                     \
				EC_FREE(fl);                                                                                                                      \
				ec_dlist_##NAME##_unlock(l);                                                                                                      \
				return s;                                                                                                                         \
			}                                                                                                                                     \
		ec_dlist_##NAME##_node_t* c = l->head;                                                                                                    \
		while (c)                                                                                                                                 \
			{                                                                                                                                     \
				if (p(&c->data))                                                                                                                  \
					{                                                                                                                             \
						ec_dlist_##NAME##_node_t* n = ec_dlist_##NAME##_make_node(fl, c->data);                                                   \
						if (!n)                                                                                                                   \
							{                                                                                                                     \
								fl->free_fn(fl);                                                                                                  \
								EC_FREE(fl);                                                                                                      \
								ec_dlist_##NAME##_unlock(l);                                                                                      \
								return EC_ERR_ALLOC;                                                                                              \
							}                                                                                                                     \
						if (!fl->head)                                                                                                            \
							{                                                                                                                     \
								fl->head = fl->tail = n;                                                                                          \
							}                                                                                                                     \
						else                                                                                                                      \
							{                                                                                                                     \
								fl->tail->next = n;                                                                                               \
								n->prev = fl->tail;                                                                                               \
								fl->tail = n;                                                                                                     \
							}                                                                                                                     \
						fl->size++;                                                                                                               \
					}                                                                                                                             \
				c = c->next;                                                                                                                      \
			}                                                                                                                                     \
		*out = fl;                                                                                                                                \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline void ec_dlist_##NAME##_iter_init(ec_dlist_##NAME##_iter_t* it, ec_dlist_##NAME##_t* l)                                          \
	{                                                                                                                                             \
		it->list = l;                                                                                                                             \
		it->current = NULL;                                                                                                                       \
		it->prev = NULL;                                                                                                                          \
		it->idx = 0;                                                                                                                              \
		it->dinit = false;                                                                                                                        \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_iter_remove(ec_dlist_##NAME##_iter_t* it)                                                         \
	{                                                                                                                                             \
		if (!it || !it->current)                                                                                                                  \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(it->list);                                                                                                         \
		ec_dlist_##NAME##_node_t* rm = it->current;                                                                                               \
		if (rm->prev)                                                                                                                             \
			rm->prev->next = rm->next;                                                                                                            \
		else                                                                                                                                      \
			it->list->head = rm->next;                                                                                                            \
		if (rm->next)                                                                                                                             \
			rm->next->prev = rm->prev;                                                                                                            \
		else                                                                                                                                      \
			it->list->tail = rm->prev;                                                                                                            \
		it->list->size--;                                                                                                                         \
		it->current = rm->next;                                                                                                                   \
		EC_FREE(rm);                                                                                                                              \
		ec_dlist_##NAME##_unlock(it->list);                                                                                                       \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_iter_add(ec_dlist_##NAME##_iter_t* it, void* elem)                                                \
	{                                                                                                                                             \
		if (!it)                                                                                                                                  \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(it->list);                                                                                                         \
		ec_dlist_##NAME##_node_t* n = ec_dlist_##NAME##_make_node(it->list, *(TYPE*)elem);                                                        \
		if (!n)                                                                                                                                   \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(it->list);                                                                                               \
				return EC_ERR_ALLOC;                                                                                                              \
			}                                                                                                                                     \
		if (!it->prev)                                                                                                                            \
			{                                                                                                                                     \
				n->next = it->list->head;                                                                                                         \
				if (it->list->head)                                                                                                               \
					it->list->head->prev = n;                                                                                                     \
				it->list->head = n;                                                                                                               \
				if (it->list->size == 0)                                                                                                          \
					it->list->tail = n;                                                                                                           \
			}                                                                                                                                     \
		else                                                                                                                                      \
			{                                                                                                                                     \
				n->next = it->prev->next;                                                                                                         \
				n->prev = it->prev;                                                                                                               \
				it->prev->next = n;                                                                                                               \
				if (n->next)                                                                                                                      \
					n->next->prev = n;                                                                                                            \
				else                                                                                                                              \
					it->list->tail = n;                                                                                                           \
			}                                                                                                                                     \
		it->list->size++;                                                                                                                         \
		it->prev = n;                                                                                                                             \
		it->current = n->next;                                                                                                                    \
		ec_dlist_##NAME##_unlock(it->list);                                                                                                       \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_iter_replace(ec_dlist_##NAME##_iter_t* it, void* elem, void** out)                                \
	{                                                                                                                                             \
		if (!it || !it->current)                                                                                                                  \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(it->list);                                                                                                         \
		if (out)                                                                                                                                  \
			*out = &it->current->data;                                                                                                            \
		it->current->data = *(TYPE*)elem;                                                                                                         \
		ec_dlist_##NAME##_unlock(it->list);                                                                                                       \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline size_t ec_dlist_##NAME##_iter_index(ec_dlist_##NAME##_iter_t* it)                                                               \
	{                                                                                                                                             \
		return it ? it->idx : 0;                                                                                                                  \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_iter_next(ec_dlist_##NAME##_iter_t* it, void** out)                                               \
	{                                                                                                                                             \
		if (!it || !out)                                                                                                                          \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(it->list);                                                                                                         \
		if (!it->current && !it->dinit)                                                                                                           \
			{                                                                                                                                     \
				it->current = it->list->head;                                                                                                     \
				it->dinit = true;                                                                                                                 \
			}                                                                                                                                     \
		else if (it->current)                                                                                                                     \
			it->current = it->current->next;                                                                                                      \
		if (!it->current)                                                                                                                         \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(it->list);                                                                                               \
				return EC_ITER_END;                                                                                                               \
			}                                                                                                                                     \
		*out = &it->current->data;                                                                                                                \
		it->idx++;                                                                                                                                \
		ec_dlist_##NAME##_unlock(it->list);                                                                                                       \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline void ec_dlist_##NAME##_zip_iter_init(ec_dlist_##NAME##_zip_iter_t* zit, ec_dlist_##NAME##_t* l1, ec_dlist_##NAME##_t* l2)       \
	{                                                                                                                                             \
		zit->l1 = l1;                                                                                                                             \
		zit->l2 = l2;                                                                                                                             \
		zit->c1 = NULL;                                                                                                                           \
		zit->c2 = NULL;                                                                                                                           \
		zit->p1 = NULL;                                                                                                                           \
		zit->p2 = NULL;                                                                                                                           \
		zit->idx = 0;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_zip_iter_next(ec_dlist_##NAME##_zip_iter_t* zit, void** out1, void** out2)                        \
	{                                                                                                                                             \
		if (!zit || !out1 || !out2)                                                                                                               \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(zit->l1);                                                                                                          \
		ec_dlist_##NAME##_lock(zit->l2);                                                                                                          \
		if (!zit->c1 && !zit->c2 && zit->idx == 0)                                                                                                \
			{                                                                                                                                     \
				zit->c1 = zit->l1->head;                                                                                                          \
				zit->c2 = zit->l2->head;                                                                                                          \
			}                                                                                                                                     \
		else                                                                                                                                      \
			{                                                                                                                                     \
				if (zit->c1)                                                                                                                      \
					zit->c1 = zit->c1->next;                                                                                                      \
				if (zit->c2)                                                                                                                      \
					zit->c2 = zit->c2->next;                                                                                                      \
			}                                                                                                                                     \
		if (!zit->c1 || !zit->c2)                                                                                                                 \
			{                                                                                                                                     \
				ec_dlist_##NAME##_unlock(zit->l2);                                                                                                \
				ec_dlist_##NAME##_unlock(zit->l1);                                                                                                \
				return EC_ITER_END;                                                                                                               \
			}                                                                                                                                     \
		*out1 = &zit->c1->data;                                                                                                                   \
		*out2 = &zit->c2->data;                                                                                                                   \
		zit->idx++;                                                                                                                               \
		ec_dlist_##NAME##_unlock(zit->l2);                                                                                                        \
		ec_dlist_##NAME##_unlock(zit->l1);                                                                                                        \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_zip_iter_add(ec_dlist_##NAME##_zip_iter_t* zit, void* e1, void* e2)                               \
	{                                                                                                                                             \
		return EC_ERR_NOT_SUPPORTED;                                                                                                              \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_zip_iter_remove(ec_dlist_##NAME##_zip_iter_t* zit, void** o1, void** o2)                          \
	{                                                                                                                                             \
		return EC_ERR_NOT_SUPPORTED;                                                                                                              \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_zip_iter_replace(ec_dlist_##NAME##_zip_iter_t* zit, void* e1, void* e2, void** out1, void** out2) \
	{                                                                                                                                             \
		return EC_ERR_NOT_SUPPORTED;                                                                                                              \
	}                                                                                                                                             \
	static inline size_t ec_dlist_##NAME##_zip_iter_index(ec_dlist_##NAME##_zip_iter_t* zit)                                                      \
	{                                                                                                                                             \
		return zit->idx;                                                                                                                          \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_free_fn(ec_dlist_##NAME##_t* l)                                                                   \
	{                                                                                                                                             \
		if (!l)                                                                                                                                   \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		ec_dlist_##NAME##_lock(l);                                                                                                                \
		ec_dlist_##NAME##_node_t* c = l->head;                                                                                                    \
		while (c)                                                                                                                                 \
			{                                                                                                                                     \
				ec_dlist_##NAME##_node_t* n = c->next;                                                                                            \
				EC_FREE(c);                                                                                                                       \
				c = n;                                                                                                                            \
			}                                                                                                                                     \
		l->head = l->tail = NULL;                                                                                                                 \
		l->size = 0;                                                                                                                              \
		ec_dlist_##NAME##_unlock(l);                                                                                                              \
		EC_MUTEX_DESTROY(l->lock);                                                                                                                \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline ec_status_t ec_dlist_##NAME##_init(ec_dlist_##NAME##_t* l, void* mempool)                                                       \
	{                                                                                                                                             \
		if (!l)                                                                                                                                   \
			return EC_ERR_INVALID_ARG;                                                                                                            \
		EC_MUTEX_INIT(l->lock);                                                                                                                   \
		l->head = l->tail = NULL;                                                                                                                 \
		l->size = 0;                                                                                                                              \
		l->mempool = mempool;                                                                                                                     \
		ec_dlist_##NAME##_init_functions(l);                                                                                                      \
		return EC_OK;                                                                                                                             \
	}                                                                                                                                             \
	static inline void ec_dlist_##NAME##_init_functions(ec_dlist_##NAME##_t* l)                                                                   \
	{                                                                                                                                             \
		l->struct_size = ec_dlist_##NAME##_struct_size;                                                                                           \
		l->splice = ec_dlist_##NAME##_splice;                                                                                                     \
		l->splice_at = ec_dlist_##NAME##_splice_at;                                                                                               \
		l->add = ec_dlist_##NAME##_add;                                                                                                           \
		l->add_at = ec_dlist_##NAME##_add_at;                                                                                                     \
		l->add_all = ec_dlist_##NAME##_add_all;                                                                                                   \
		l->add_all_at = ec_dlist_##NAME##_add_all_at;                                                                                             \
		l->add_first = ec_dlist_##NAME##_add_first;                                                                                               \
		l->add_last = ec_dlist_##NAME##_add_last;                                                                                                 \
		l->remove_fn = ec_dlist_##NAME##_remove_fn;                                                                                               \
		l->remove_first = ec_dlist_##NAME##_remove_first;                                                                                         \
		l->remove_last = ec_dlist_##NAME##_remove_last;                                                                                           \
		l->remove_at = ec_dlist_##NAME##_remove_at;                                                                                               \
		l->remove_all = ec_dlist_##NAME##_remove_all;                                                                                             \
		l->remove_all_cb = ec_dlist_##NAME##_remove_all_cb;                                                                                       \
		l->get_at = ec_dlist_##NAME##_get_at;                                                                                                     \
		l->get_first = ec_dlist_##NAME##_get_first;                                                                                               \
		l->get_last = ec_dlist_##NAME##_get_last;                                                                                                 \
		l->sublist = ec_dlist_##NAME##_sublist;                                                                                                   \
		l->copy_shallow = ec_dlist_##NAME##_copy_shallow;                                                                                         \
		l->copy_deep = ec_dlist_##NAME##_copy_deep;                                                                                               \
		l->replace_at = ec_dlist_##NAME##_replace_at;                                                                                             \
		l->contains = ec_dlist_##NAME##_contains;                                                                                                 \
		l->contains_value = ec_dlist_##NAME##_contains_value;                                                                                     \
		l->index_of = ec_dlist_##NAME##_index_of;                                                                                                 \
		l->to_array = ec_dlist_##NAME##_to_array;                                                                                                 \
		l->reverse = ec_dlist_##NAME##_reverse;                                                                                                   \
		l->sort = ec_dlist_##NAME##_sort;                                                                                                         \
		l->sort_in_place = ec_dlist_##NAME##_sort_in_place;                                                                                       \
		l->size_fn = ec_dlist_##NAME##_size_fn;                                                                                                   \
		l->foreach = ec_dlist_##NAME##_foreach;                                                                                                   \
		l->reduce = ec_dlist_##NAME##_reduce;                                                                                                     \
		l->filter_mut = ec_dlist_##NAME##_filter_mut;                                                                                             \
		l->filter = ec_dlist_##NAME##_filter;                                                                                                     \
		l->iter_init = ec_dlist_##NAME##_iter_init;                                                                                               \
		l->iter_remove = ec_dlist_##NAME##_iter_remove;                                                                                           \
		l->iter_add = ec_dlist_##NAME##_iter_add;                                                                                                 \
		l->iter_replace = ec_dlist_##NAME##_iter_replace;                                                                                         \
		l->iter_index = ec_dlist_##NAME##_iter_index;                                                                                             \
		l->iter_next = ec_dlist_##NAME##_iter_next;                                                                                               \
		l->zip_iter_init = ec_dlist_##NAME##_zip_iter_init;                                                                                       \
		l->zip_iter_next = ec_dlist_##NAME##_zip_iter_next;                                                                                       \
		l->zip_iter_add = ec_dlist_##NAME##_zip_iter_add;                                                                                         \
		l->zip_iter_remove = ec_dlist_##NAME##_zip_iter_remove;                                                                                   \
		l->zip_iter_replace = ec_dlist_##NAME##_zip_iter_replace;                                                                                 \
		l->zip_iter_index = ec_dlist_##NAME##_zip_iter_index;                                                                                     \
		l->free_fn = ec_dlist_##NAME##_free_fn;                                                                                                   \
	}
EC_DLIST_DEF(int, int)
EC_DLIST_DEF(unsigned int, uint)
EC_DLIST_DEF(long, long)
EC_DLIST_DEF(long long, ll)
EC_DLIST_DEF(unsigned long, ul)
EC_DLIST_DEF(unsigned long long, ull)
EC_DLIST_DEF(uint32_t, u32)
EC_DLIST_DEF(uint64_t, u64)
EC_DLIST_DEF(double, dbl)
EC_DLIST_DEF(const char*, str)
EC_DLIST_DEF(void*, ptr)
#endif
