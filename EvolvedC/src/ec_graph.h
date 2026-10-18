#ifndef EC_GRAPH_H
#define EC_GRAPH_H
#include "ec_common.h"
#include "ec_status.h"

#ifdef EC_USE_MEMPOOL
#define EC_GRAPH_ALLOC(T, MP) ((MP) ? (T*)ec_mempool_alloc((ec_mempool_t*)(MP), sizeof(T)) : (T*)EC_MALLOC(sizeof(T)))
#define EC_GRAPH_REALLOC(OLD, T, CNT, MP) ((MP) ? NULL : (T*)EC_REALLOC(OLD, (CNT) * sizeof(T)))
#else
#define EC_GRAPH_ALLOC(T, MP) (T*)EC_MALLOC(sizeof(T))
#define EC_GRAPH_REALLOC(OLD, T, CNT, MP) (T*)EC_REALLOC(OLD, (CNT) * sizeof(T))
#endif

/*
  Adjacency-List Graph with function pointers:
  insert_node, insert_edge, bfs, dfs, dijkstra, clear, etc.
*/

#define EC_GRAPH_DEF(TYPE, NAME)                                                                                                           \
	typedef struct ec_graph_##NAME##_edge_s                                                                                                \
	{                                                                                                                                      \
		struct ec_graph_##NAME##_node_s* dst;                                                                                              \
		int w;                                                                                                                             \
	} ec_graph_##NAME##_edge_t;                                                                                                            \
	typedef struct ec_graph_##NAME##_node_s                                                                                                \
	{                                                                                                                                      \
		TYPE key;                                                                                                                          \
		ec_graph_##NAME##_edge_t* edges;                                                                                                   \
		size_t edge_count;                                                                                                                 \
	} ec_graph_##NAME##_node_t;                                                                                                            \
	typedef struct ec_graph_##NAME##_s ec_graph_##NAME##_t;                                                                                \
	typedef struct ec_graph_##NAME##_s                                                                                                     \
	{                                                                                                                                      \
		ec_graph_##NAME##_node_t* nodes;                                                                                                   \
		size_t node_count;                                                                                                                 \
		EC_MUTEX_TYPE lock;                                                                                                                \
		void* mempool;                                                                                                                     \
		int (*compare)(const TYPE*, const TYPE*);                                                                                          \
		/* Removed the init pointer to avoid accidental self–calls that cause an infinite loop */                                          \
		ec_status_t (*insert_node)(ec_graph_##NAME##_t*, const TYPE*);                                                                     \
		ec_status_t (*insert_edge)(ec_graph_##NAME##_t*, const TYPE*, const TYPE*, int, int);                                              \
		ec_status_t (*bfs)(ec_graph_##NAME##_t*, const TYPE*, void (*)(TYPE));                                                             \
		ec_status_t (*dfs)(ec_graph_##NAME##_t*, const TYPE*, void (*)(TYPE));                                                             \
		ec_status_t (*dijkstra)(ec_graph_##NAME##_t*, const TYPE*, int**);                                                                 \
		ec_status_t (*clear)(ec_graph_##NAME##_t*);                                                                                        \
		ec_status_t (*remove_node)(ec_graph_##NAME##_t*, const TYPE*);                                                                     \
		ec_status_t (*remove_edge)(ec_graph_##NAME##_t*, const TYPE*, const TYPE*, int);                                                   \
	} ec_graph_##NAME##_t;                                                                                                                 \
	static inline int ec_graph_##NAME##_dfcmp(const TYPE* a, const TYPE* b)                                                                \
	{                                                                                                                                      \
		return (*a < *b) ? -1 : ((*a > *b) ? 1 : 0);                                                                                       \
	}                                                                                                                                      \
	static inline ec_graph_##NAME##_node_t* ec_graph_##NAME##_find(ec_graph_##NAME##_t* m, const TYPE* key)                                \
	{                                                                                                                                      \
		for (size_t i = 0; i < m->node_count; i++)                                                                                         \
			{                                                                                                                              \
				if (!m->compare(key, &m->nodes[i].key))                                                                                    \
					return &m->nodes[i];                                                                                                   \
			}                                                                                                                              \
		return NULL;                                                                                                                       \
	}                                                                                                                                      \
	static inline ec_status_t ec_graph_##NAME##_insert_node(ec_graph_##NAME##_t* m, const TYPE* key)                                       \
	{                                                                                                                                      \
		if (!m || !key)                                                                                                                    \
			return EC_ERR_INVALID_ARG;                                                                                                     \
		ec_graph_##NAME##_node_t* f = ec_graph_##NAME##_find(m, key);                                                                      \
		if (f)                                                                                                                             \
			return EC_OK;                                                                                                                  \
		/* Save the old base pointer and current count */                                                                                  \
		ec_graph_##NAME##_node_t* old_nodes = m->nodes;                                                                                    \
		size_t old_count = m->node_count;                                                                                                  \
		ec_graph_##NAME##_node_t* tmp = EC_GRAPH_REALLOC(m->nodes, ec_graph_##NAME##_node_t, old_count + 1, m->mempool);                   \
		if (!tmp)                                                                                                                          \
			return EC_ERR_ALLOC;                                                                                                           \
		m->nodes = tmp;                                                                                                                    \
		/* If the nodes array moved, update every stored edge pointer */                                                                   \
		if (old_nodes != NULL && m->nodes != old_nodes)                                                                                    \
			{                                                                                                                              \
				for (size_t i = 0; i < old_count; i++)                                                                                     \
					{                                                                                                                      \
						ec_graph_##NAME##_node_t* node = &m->nodes[i];                                                                     \
						for (size_t j = 0; j < node->edge_count; j++)                                                                      \
							{                                                                                                              \
								ptrdiff_t offset = (char*)node->edges[j].dst - (char*)old_nodes;                                           \
								node->edges[j].dst = (ec_graph_##NAME##_node_t*)((char*)m->nodes + offset);                                \
							}                                                                                                              \
					}                                                                                                                      \
			}                                                                                                                              \
		m->nodes[old_count].key = *key; /* assign the new node’s key */                                                                    \
		m->nodes[old_count].edges = NULL;                                                                                                  \
		m->nodes[old_count].edge_count = 0;                                                                                                \
		m->node_count++;                                                                                                                   \
		return EC_OK;                                                                                                                      \
	}                                                                                                                                      \
	static inline ec_status_t ec_graph_##NAME##_insert_edge(ec_graph_##NAME##_t* m, const TYPE* src, const TYPE* dst, int w, int directed) \
	{                                                                                                                                      \
		if (!m || !src || !dst)                                                                                                            \
			return EC_ERR_INVALID_ARG;                                                                                                     \
		EC_MUTEX_LOCK(m->lock);                                                                                                            \
		ec_status_t st = m->insert_node(m, src);                                                                                           \
		if (st != EC_OK)                                                                                                                   \
			{                                                                                                                              \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                  \
				return st;                                                                                                                 \
			}                                                                                                                              \
		st = m->insert_node(m, dst);                                                                                                       \
		if (st != EC_OK)                                                                                                                   \
			{                                                                                                                              \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                  \
				return st;                                                                                                                 \
			}                                                                                                                              \
		ec_graph_##NAME##_node_t *s = ec_graph_##NAME##_find(m, src), *d = ec_graph_##NAME##_find(m, dst);                                 \
		ec_graph_##NAME##_edge_t* te = EC_GRAPH_REALLOC(s->edges, ec_graph_##NAME##_edge_t, s->edge_count + 1, m->mempool);                \
		if (!te)                                                                                                                           \
			{                                                                                                                              \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                  \
				return EC_ERR_ALLOC;                                                                                                       \
			}                                                                                                                              \
		s->edges = te;                                                                                                                     \
		s->edges[s->edge_count].dst = d;                                                                                                   \
		s->edges[s->edge_count].w = w;                                                                                                     \
		s->edge_count++;                                                                                                                   \
		if (!directed && s != d)                                                                                                           \
			{                                                                                                                              \
				ec_graph_##NAME##_edge_t* te2 = EC_GRAPH_REALLOC(d->edges, ec_graph_##NAME##_edge_t, d->edge_count + 1, m->mempool);       \
				if (!te2)                                                                                                                  \
					{                                                                                                                      \
						EC_MUTEX_UNLOCK(m->lock);                                                                                          \
						return EC_ERR_ALLOC;                                                                                               \
					}                                                                                                                      \
				d->edges = te2;                                                                                                            \
				d->edges[d->edge_count].dst = s;                                                                                           \
				d->edges[d->edge_count].w = w;                                                                                             \
				d->edge_count++;                                                                                                           \
			}                                                                                                                              \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                          \
		return EC_OK;                                                                                                                      \
	}                                                                                                                                      \
	static inline ec_status_t ec_graph_##NAME##_clear(ec_graph_##NAME##_t* m)                                                              \
	{                                                                                                                                      \
		if (!m)                                                                                                                            \
			return EC_ERR_INVALID_ARG;                                                                                                     \
		EC_MUTEX_LOCK(m->lock);                                                                                                            \
		if (m->nodes)                                                                                                                      \
			{                                                                                                                              \
				for (size_t i = 0; i < m->node_count; i++)                                                                                 \
					{                                                                                                                      \
						if (m->nodes[i].edges)                                                                                             \
							EC_FREE(m->nodes[i].edges);                                                                                    \
					}                                                                                                                      \
				EC_FREE(m->nodes);                                                                                                         \
			}                                                                                                                              \
		m->nodes = NULL;                                                                                                                   \
		m->node_count = 0;                                                                                                                 \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                          \
		return EC_OK;                                                                                                                      \
	}                                                                                                                                      \
	static inline ec_status_t ec_graph_##NAME##_bfs(ec_graph_##NAME##_t* m, const TYPE* start, void (*visit)(TYPE))                        \
	{                                                                                                                                      \
		if (!m || !start || !visit)                                                                                                        \
			return EC_ERR_INVALID_ARG;                                                                                                     \
		EC_MUTEX_LOCK(m->lock);                                                                                                            \
		ec_graph_##NAME##_node_t* s = ec_graph_##NAME##_find(m, start);                                                                    \
		if (!s)                                                                                                                            \
			{                                                                                                                              \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                  \
				return EC_ERR_NOT_FOUND;                                                                                                   \
			}                                                                                                                              \
		char* vis = (char*)EC_CALLOC(m->node_count, sizeof(char));                                                                         \
		ec_graph_##NAME##_node_t** q = (ec_graph_##NAME##_node_t**)EC_CALLOC(m->node_count, sizeof(ec_graph_##NAME##_node_t*));            \
		if (!vis || !q)                                                                                                                    \
			{                                                                                                                              \
				EC_FREE(vis);                                                                                                              \
				EC_FREE(q);                                                                                                                \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                  \
				return EC_ERR_ALLOC;                                                                                                       \
			}                                                                                                                              \
		int front = 0, rear = 0;                                                                                                           \
		q[rear++] = s;                                                                                                                     \
		while (front < rear)                                                                                                               \
			{                                                                                                                              \
				ec_graph_##NAME##_node_t* cur = q[front++];                                                                                \
				size_t idx = (size_t)(cur - m->nodes);                                                                                     \
				if (!vis[idx])                                                                                                             \
					{                                                                                                                      \
						vis[idx] = 1;                                                                                                      \
						visit(cur->key);                                                                                                   \
						for (size_t i = 0; i < cur->edge_count; i++)                                                                       \
							{                                                                                                              \
								ec_graph_##NAME##_node_t* nxt = cur->edges[i].dst;                                                         \
								size_t nx = (size_t)(nxt - m->nodes);                                                                      \
								if (!vis[nx])                                                                                              \
									q[rear++] = nxt;                                                                                       \
							}                                                                                                              \
					}                                                                                                                      \
			}                                                                                                                              \
		EC_FREE(vis);                                                                                                                      \
		EC_FREE(q);                                                                                                                        \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                          \
		return EC_OK;                                                                                                                      \
	}                                                                                                                                      \
	static inline void ec_graph_##NAME##_dfs_util(ec_graph_##NAME##_t* m, ec_graph_##NAME##_node_t* cur, char* vis, void (*visit)(TYPE))   \
	{                                                                                                                                      \
		size_t idx = (size_t)(cur - m->nodes);                                                                                             \
		if (vis[idx])                                                                                                                      \
			return;                                                                                                                        \
		vis[idx] = 1;                                                                                                                      \
		visit(cur->key);                                                                                                                   \
		for (size_t i = 0; i < cur->edge_count; i++)                                                                                       \
			{                                                                                                                              \
				ec_graph_##NAME##_dfs_util(m, cur->edges[i].dst, vis, visit);                                                              \
			}                                                                                                                              \
	}                                                                                                                                      \
	static inline ec_status_t ec_graph_##NAME##_dfs(ec_graph_##NAME##_t* m, const TYPE* start, void (*visit)(TYPE))                        \
	{                                                                                                                                      \
		if (!m || !start || !visit)                                                                                                        \
			return EC_ERR_INVALID_ARG;                                                                                                     \
		EC_MUTEX_LOCK(m->lock);                                                                                                            \
		ec_graph_##NAME##_node_t* s = ec_graph_##NAME##_find(m, start);                                                                    \
		if (!s)                                                                                                                            \
			{                                                                                                                              \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                  \
				return EC_ERR_NOT_FOUND;                                                                                                   \
			}                                                                                                                              \
		char* vis = (char*)EC_CALLOC(m->node_count, sizeof(char));                                                                         \
		if (!vis)                                                                                                                          \
			{                                                                                                                              \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                  \
				return EC_ERR_ALLOC;                                                                                                       \
			}                                                                                                                              \
		ec_graph_##NAME##_dfs_util(m, s, vis, visit);                                                                                      \
		EC_FREE(vis);                                                                                                                      \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                          \
		return EC_OK;                                                                                                                      \
	}                                                                                                                                      \
	static inline ec_status_t ec_graph_##NAME##_dijkstra(ec_graph_##NAME##_t* m, const TYPE* start, int** dist_out)                        \
	{                                                                                                                                      \
		if (!m || !start || !dist_out)                                                                                                     \
			return EC_ERR_INVALID_ARG;                                                                                                     \
		EC_MUTEX_LOCK(m->lock);                                                                                                            \
		ec_graph_##NAME##_node_t* s = ec_graph_##NAME##_find(m, start);                                                                    \
		if (!s)                                                                                                                            \
			{                                                                                                                              \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                  \
				return EC_ERR_NOT_FOUND;                                                                                                   \
			}                                                                                                                              \
		int* dist = (int*)EC_MALLOC(sizeof(int) * m->node_count);                                                                          \
		char* done = (char*)EC_CALLOC(m->node_count, sizeof(char));                                                                        \
		if (!dist || !done)                                                                                                                \
			{                                                                                                                              \
				EC_FREE(dist);                                                                                                             \
				EC_FREE(done);                                                                                                             \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                  \
				return EC_ERR_ALLOC;                                                                                                       \
			}                                                                                                                              \
		for (size_t i = 0; i < m->node_count; i++)                                                                                         \
			{                                                                                                                              \
				dist[i] = 0x3f3f3f3f;                                                                                                      \
			}                                                                                                                              \
		size_t sid = (size_t)(s - m->nodes);                                                                                               \
		dist[sid] = 0;                                                                                                                     \
		for (size_t rep = 0; rep < m->node_count; rep++)                                                                                   \
			{                                                                                                                              \
				int mn = 0x3f3f3f3f;                                                                                                       \
				size_t u = (size_t)-1;                                                                                                     \
				for (size_t i = 0; i < m->node_count; i++)                                                                                 \
					{                                                                                                                      \
						if (!done[i] && dist[i] < mn)                                                                                      \
							{                                                                                                              \
								mn = dist[i];                                                                                              \
								u = i;                                                                                                     \
							}                                                                                                              \
					}                                                                                                                      \
				if (u == (size_t)-1)                                                                                                       \
					break;                                                                                                                 \
				done[u] = 1;                                                                                                               \
				ec_graph_##NAME##_node_t* cu = &m->nodes[u];                                                                               \
				for (size_t e = 0; e < cu->edge_count; e++)                                                                                \
					{                                                                                                                      \
						size_t v = (size_t)(cu->edges[e].dst - m->nodes);                                                                  \
						int nd = dist[u] + cu->edges[e].w;                                                                                 \
						if (nd < dist[v])                                                                                                  \
							dist[v] = nd;                                                                                                  \
					}                                                                                                                      \
			}                                                                                                                              \
		*dist_out = dist;                                                                                                                  \
		EC_FREE(done);                                                                                                                     \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                          \
		return EC_OK;                                                                                                                      \
	}                                                                                                                                      \
	static inline ec_status_t ec_graph_##NAME##_remove_node(ec_graph_##NAME##_t* m, const TYPE* key)                                       \
	{                                                                                                                                      \
		if (!m || !key)                                                                                                                    \
			return EC_ERR_INVALID_ARG;                                                                                                     \
		EC_MUTEX_LOCK(m->lock);                                                                                                            \
		size_t idx = (size_t)-1;                                                                                                           \
		for (size_t i = 0; i < m->node_count; i++)                                                                                         \
			{                                                                                                                              \
				if (!m->compare(key, &m->nodes[i].key))                                                                                    \
					{                                                                                                                      \
						idx = i;                                                                                                           \
						break;                                                                                                             \
					}                                                                                                                      \
			}                                                                                                                              \
		if (idx == (size_t)-1)                                                                                                             \
			{                                                                                                                              \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                  \
				return EC_ERR_NOT_FOUND;                                                                                                   \
			}                                                                                                                              \
		/* Remove edges pointing to this node */                                                                                           \
		for (size_t i = 0; i < m->node_count; i++)                                                                                         \
			{                                                                                                                              \
				ec_graph_##NAME##_node_t* n = &m->nodes[i];                                                                                \
				size_t j = 0;                                                                                                              \
				while (j < n->edge_count)                                                                                                  \
					{                                                                                                                      \
						if (n->edges[j].dst == &m->nodes[idx])                                                                             \
							{                                                                                                              \
								/* Shift left to remove edge */                                                                            \
								for (size_t k = j; k < n->edge_count - 1; k++)                                                             \
									n->edges[k] = n->edges[k + 1];                                                                         \
								n->edge_count--;                                                                                           \
								n->edges = EC_GRAPH_REALLOC(n->edges, ec_graph_##NAME##_edge_t, n->edge_count, m->mempool);                \
							}                                                                                                              \
						else                                                                                                               \
							{                                                                                                              \
								j++;                                                                                                       \
							}                                                                                                              \
					}                                                                                                                      \
			}                                                                                                                              \
		/* Free edges of the node itself */                                                                                                \
		if (m->nodes[idx].edges)                                                                                                           \
			EC_FREE(m->nodes[idx].edges);                                                                                                  \
		/* Shift remaining nodes left */                                                                                                   \
		for (size_t i = idx; i < m->node_count - 1; i++)                                                                                   \
			m->nodes[i] = m->nodes[i + 1];                                                                                                 \
		m->node_count--;                                                                                                                   \
		m->nodes = EC_GRAPH_REALLOC(m->nodes, ec_graph_##NAME##_node_t, m->node_count, m->mempool);                                        \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                          \
		return EC_OK;                                                                                                                      \
	}                                                                                                                                      \
	static inline ec_status_t ec_graph_##NAME##_remove_edge(ec_graph_##NAME##_t* m, const TYPE* src, const TYPE* dst, int directed)        \
	{                                                                                                                                      \
		if (!m || !src || !dst)                                                                                                            \
			return EC_ERR_INVALID_ARG;                                                                                                     \
		EC_MUTEX_LOCK(m->lock);                                                                                                            \
		ec_graph_##NAME##_node_t* s = ec_graph_##NAME##_find(m, src);                                                                      \
		ec_graph_##NAME##_node_t* d = ec_graph_##NAME##_find(m, dst);                                                                      \
		if (!s || !d)                                                                                                                      \
			{                                                                                                                              \
				EC_MUTEX_UNLOCK(m->lock);                                                                                                  \
				return EC_ERR_NOT_FOUND;                                                                                                   \
			}                                                                                                                              \
		/* Remove edge from src to dst */                                                                                                  \
		size_t j = 0;                                                                                                                      \
		while (j < s->edge_count)                                                                                                          \
			{                                                                                                                              \
				if (s->edges[j].dst == d)                                                                                                  \
					{                                                                                                                      \
						for (size_t k = j; k < s->edge_count - 1; k++)                                                                     \
							s->edges[k] = s->edges[k + 1];                                                                                 \
						s->edge_count--;                                                                                                   \
						s->edges = EC_GRAPH_REALLOC(s->edges, ec_graph_##NAME##_edge_t, s->edge_count, m->mempool);                        \
					}                                                                                                                      \
				else                                                                                                                       \
					{                                                                                                                      \
						j++;                                                                                                               \
					}                                                                                                                      \
			}                                                                                                                              \
		/* Remove edge from dst to src if undirected */                                                                                    \
		if (!directed)                                                                                                                     \
			{                                                                                                                              \
				j = 0;                                                                                                                     \
				while (j < d->edge_count)                                                                                                  \
					{                                                                                                                      \
						if (d->edges[j].dst == s)                                                                                          \
							{                                                                                                              \
								for (size_t k = j; k < d->edge_count - 1; k++)                                                             \
									d->edges[k] = d->edges[k + 1];                                                                         \
								d->edge_count--;                                                                                           \
								d->edges = EC_GRAPH_REALLOC(d->edges, ec_graph_##NAME##_edge_t, d->edge_count, m->mempool);                \
							}                                                                                                              \
						else                                                                                                               \
							{                                                                                                              \
								j++;                                                                                                       \
							}                                                                                                              \
					}                                                                                                                      \
			}                                                                                                                              \
		EC_MUTEX_UNLOCK(m->lock);                                                                                                          \
		return EC_OK;                                                                                                                      \
	}                                                                                                                                      \
	static inline ec_status_t ec_graph_##NAME##_init(ec_graph_##NAME##_t* g, void* mp, int (*cmp)(const TYPE*, const TYPE*))               \
	{                                                                                                                                      \
		if (!g)                                                                                                                            \
			return EC_ERR_INVALID_ARG;                                                                                                     \
		/* Only initialize the data members—not the function pointers which act like a v–table */                                          \
		g->mempool = mp;                                                                                                                   \
		g->compare = cmp ? cmp : ec_graph_##NAME##_dfcmp;                                                                                  \
		g->node_count = 0;                                                                                                                 \
		g->nodes = NULL;                                                                                                                   \
		g->insert_node = ec_graph_##NAME##_insert_node;                                                                                    \
		g->insert_edge = ec_graph_##NAME##_insert_edge;                                                                                    \
		g->remove_node = ec_graph_##NAME##_remove_node;                                                                                    \
		g->remove_edge = ec_graph_##NAME##_remove_edge;                                                                                    \
		g->bfs = ec_graph_##NAME##_bfs;                                                                                                    \
		g->dfs = ec_graph_##NAME##_dfs;                                                                                                    \
		g->dijkstra = ec_graph_##NAME##_dijkstra;                                                                                          \
		g->clear = ec_graph_##NAME##_clear;                                                                                                \
		/* Do not assign g->init to avoid recursive calls */                                                                               \
		return EC_OK;                                                                                                                      \
	}

EC_GRAPH_DEF(int, int)
EC_GRAPH_DEF(unsigned int, uint)
EC_GRAPH_DEF(long, long)
EC_GRAPH_DEF(long long, ll)
EC_GRAPH_DEF(unsigned long, ul)
EC_GRAPH_DEF(unsigned long long, ull)
EC_GRAPH_DEF(uint32_t, u32)
EC_GRAPH_DEF(uint64_t, u64)
EC_GRAPH_DEF(double, dbl)
EC_GRAPH_DEF(const char*, str)
EC_GRAPH_DEF(void*, ptr)

#endif
