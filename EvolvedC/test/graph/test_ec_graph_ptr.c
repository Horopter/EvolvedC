#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../../src/ec_graph.h"
#include <stdbool.h>
#include <stdint.h>
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif
#include <stdint.h>

/* --- Helper functions for debugging --- */

static void
stktrc(void)
{
#ifdef __linux__
	void* arr[32];
	size_t n = backtrace(arr, 32);
	char** syms = backtrace_symbols(arr, n);
	if (syms)
		{
			fprintf(stderr, "---StackTrace---\n");
			for (size_t i = 0; i < n; i++)
				{
					fprintf(stderr, "%s\n", syms[i]);
				}
			free(syms);
		}
#else
	fprintf(stderr, "(NoStackTrace)\n");
#endif
}

#define TASSERT(COND)                                                                                    \
	do                                                                                                   \
		{                                                                                                \
			if (!(COND))                                                                                 \
				{                                                                                        \
					fprintf(stderr, "[FAIL] %s:%d: Assertion '%s' failed\n", __FILE__, __LINE__, #COND); \
					stktrc();                                                                            \
					exit(1);                                                                             \
				}                                                                                        \
		}                                                                                                \
	while (0)

static void
stamp(void)
{
	time_t t = time(NULL);
	struct tm* m = localtime(&t);
	if (m)
		{
			char b[64];
			strftime(b, sizeof(b), "[%Y-%m-%d %H:%M:%S]", m);
			printf("%s ", b);
		}
	else
		{
			printf("[timeerr] ");
		}
}

/* --- Local helpers to reduce duplication --- */
static inline ec_status_t
add_dir(ec_graph_ptr_t* g, void* u, void* v, int w)
{
	return g->insert_edge(g, &u, &v, w, 1);
}

static inline ec_status_t
add_undir(ec_graph_ptr_t* g, void* u, void* v, int w)
{
	return g->insert_edge(g, &u, &v, w, 0);
}

static bool
has_edge(ec_graph_ptr_t* g, void* u, void* v)
{
	ec_graph_ptr_node_t* nu = ec_graph_ptr_find(g, &u);
	ec_graph_ptr_node_t* nv = ec_graph_ptr_find(g, &v);
	if (!nu || !nv)
		return false;
	for (size_t i = 0; i < nu->edge_count; i++)
		{
			if (nu->edges[i].dst == nv)
				return true;
		}
	return false;
}

/* --- Global variables for traversal tests --- */

static void* bfs_result[100];
static int x1 = 1, x2 = 2, x3 = 3, x4 = 4, x5 = 5, x6 = 6, x10 = 10, x20 = 20, x99 = 99;
static int bfs_index = 0;

static void
bfs_visit(void* key)
{
	bfs_result[bfs_index++] = key;
}

static void* dfs_result[100];
static int dfs_index = 0;

static void
dfs_visit(void* key)
{
	dfs_result[dfs_index++] = key;
}

/* --- Test Functions --- */

/* Test that a newly initialized graph is empty. */
static void
test_graph_init_pos(void* mp)
{
	stamp();
	printf("test_graph_init_pos\n");
	ec_graph_ptr_t g;
	ec_status_t st = ec_graph_ptr_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	TASSERT(g.node_count == 0);
	g.clear(&g);
	printf("pass:test_graph_init_pos\n");
}

/* Test init with NULL graph pointer */
static void
test_graph_init_neg_null(void* mp)
{
	stamp();
	printf("test_graph_init_neg_null\n");
	ec_status_t st = ec_graph_ptr_init(NULL, mp, NULL);
	TASSERT(st == EC_ERR_INVALID_ARG);
	printf("pass:test_graph_init_neg_null\n");
}

/* Test node insertion (and duplicate avoidance). */
static void
test_graph_insert_node_pos(void* mp)
{
	stamp();
	printf("test_graph_insert_node_pos\n");
	ec_graph_ptr_t g;
	ec_status_t st = ec_graph_ptr_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	void* n1 = &x1;
	st = g.insert_node(&g, &n1);
	TASSERT(st == EC_OK);
	TASSERT(g.node_count == 1);
	TASSERT(ec_graph_ptr_find(&g, &n1) != NULL);
	/* Inserting the same node again should not add a duplicate */
	st = g.insert_node(&g, &n1);
	TASSERT(st == EC_OK);
	TASSERT(g.node_count == 1);
	void* n2 = &x2;
	st = g.insert_node(&g, &n2);
	TASSERT(st == EC_OK);
	TASSERT(g.node_count == 2);
	g.clear(&g);
	printf("pass:test_graph_insert_node_pos\n");
}

/* Test insert_node with NULL arguments */
static void
test_graph_insert_node_neg(void* mp)
{
	stamp();
	printf("test_graph_insert_node_neg\n");
	ec_graph_ptr_t g;
	ec_status_t st = ec_graph_ptr_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	void* n1 = &x1;
	TASSERT(g.insert_node(NULL, &n1) == EC_ERR_INVALID_ARG);
	TASSERT(g.insert_node(&g, NULL) == EC_ERR_INVALID_ARG);
	g.clear(&g);
	printf("pass:test_graph_insert_node_neg\n");
}

/* Test edge insertion in both directed and undirected mode. */
static void
test_graph_insert_edge_pos(void* mp)
{
	stamp();
	printf("test_graph_insert_edge_pos\n");
	ec_graph_ptr_t g;
	ec_status_t st = ec_graph_ptr_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	/* directed {VAL1}->{VAL2} */
	void *u1 = &x10, *v1 = &x20;
	TASSERT(add_dir(&g, u1, v1, 5) == EC_OK);
	TASSERT(g.node_count == 2);
	TASSERT(has_edge(&g, u1, v1));
	/* undirected {VAL2}<->{VAL3} */
	void *u2 = &x20, *v2 = &x3;
	TASSERT(add_undir(&g, u2, v2, 2) == EC_OK);
	TASSERT(has_edge(&g, u2, v2) && has_edge(&g, v2, u2));
	g.clear(&g);
	printf("pass:test_graph_insert_edge_pos\n");
}

/* Test insert_edge with NULL arguments */
static void
test_graph_insert_edge_neg(void* mp)
{
	stamp();
	printf("test_graph_insert_edge_neg\n");
	ec_graph_ptr_t g;
	ec_status_t st = ec_graph_ptr_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	void *a = &x1, *b = &x2;
	TASSERT(g.insert_edge(NULL, &a, &b, 1, 0) == EC_ERR_INVALID_ARG);
	TASSERT(g.insert_edge(&g, NULL, &b, 1, 0) == EC_ERR_INVALID_ARG);
	TASSERT(g.insert_edge(&g, &a, NULL, 1, 0) == EC_ERR_INVALID_ARG);
	g.clear(&g);
	printf("pass:test_graph_insert_edge_neg\n");
}

/* BFS on a simple tree. */
static void
test_graph_bfs_pos(void* mp)
{
	stamp();
	printf("test_graph_bfs_pos\n");
	ec_graph_ptr_t g;
	ec_status_t st = ec_graph_ptr_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	void *n1 = &x1, *n2 = &x2, *n3 = &x3, *n4 = &x4, *n5 = &x5, *n6 = &x6;
	TASSERT(add_dir(&g, n1, n2, 1) == EC_OK);
	TASSERT(add_dir(&g, n1, n3, 1) == EC_OK);
	TASSERT(add_dir(&g, n2, n4, 1) == EC_OK);
	TASSERT(add_dir(&g, n2, n5, 1) == EC_OK);
	TASSERT(add_dir(&g, n3, n6, 1) == EC_OK);
	bfs_index = 0;
	TASSERT(g.bfs(&g, &n1, bfs_visit) == EC_OK);
	TASSERT(bfs_index == 6);
	g.clear(&g);
	printf("pass:test_graph_bfs_pos\n");
}

/* Test BFS with NULL arguments and non-existent node */
static void
test_graph_bfs_neg(void* mp)
{
	stamp();
	printf("test_graph_bfs_neg\n");
	ec_graph_ptr_t g;
	ec_status_t st = ec_graph_ptr_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	void *n1 = &x1, *n2 = &x2, *n99 = &x99;
	TASSERT(add_dir(&g, n1, n2, 1) == EC_OK);
	/* Test NULL arguments */
	TASSERT(g.bfs(NULL, &n1, bfs_visit) == EC_ERR_INVALID_ARG);
	TASSERT(g.bfs(&g, NULL, bfs_visit) == EC_ERR_INVALID_ARG);
	TASSERT(g.bfs(&g, &n1, NULL) == EC_ERR_INVALID_ARG);
	/* Test non-existent node */
	TASSERT(g.bfs(&g, &n99, bfs_visit) == EC_ERR_NOT_FOUND);
	g.clear(&g);
	printf("pass:test_graph_bfs_neg\n");
}

/* DFS on the same tree. */
static void
test_graph_dfs_pos(void* mp)
{
	stamp();
	printf("test_graph_dfs_pos\n");
	ec_graph_ptr_t g;
	ec_status_t st = ec_graph_ptr_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	void *n1 = &x1, *n2 = &x2, *n3 = &x3, *n4 = &x4, *n5 = &x5, *n6 = &x6;
	TASSERT(add_dir(&g, n1, n2, 1) == EC_OK);
	TASSERT(add_dir(&g, n1, n3, 1) == EC_OK);
	TASSERT(add_dir(&g, n2, n4, 1) == EC_OK);
	TASSERT(add_dir(&g, n2, n5, 1) == EC_OK);
	TASSERT(add_dir(&g, n3, n6, 1) == EC_OK);
	dfs_index = 0;
	TASSERT(g.dfs(&g, &n1, dfs_visit) == EC_OK);
	TASSERT(dfs_index == 6);
	g.clear(&g);
	printf("pass:test_graph_dfs_pos\n");
}

/* Test DFS with NULL arguments and non-existent node */
static void
test_graph_dfs_neg(void* mp)
{
	stamp();
	printf("test_graph_dfs_neg\n");
	ec_graph_ptr_t g;
	ec_status_t st = ec_graph_ptr_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	void *n1 = &x1, *n2 = &x2, *n99 = &x99;
	TASSERT(add_dir(&g, n1, n2, 1) == EC_OK);
	/* Test NULL arguments */
	TASSERT(g.dfs(NULL, &n1, dfs_visit) == EC_ERR_INVALID_ARG);
	TASSERT(g.dfs(&g, NULL, dfs_visit) == EC_ERR_INVALID_ARG);
	TASSERT(g.dfs(&g, &n1, NULL) == EC_ERR_INVALID_ARG);
	/* Test non-existent node */
	TASSERT(g.dfs(&g, &n99, dfs_visit) == EC_ERR_NOT_FOUND);
	g.clear(&g);
	printf("pass:test_graph_dfs_neg\n");
}

/* Dijkstra on a small weighted digraph. */
static void
test_graph_dijkstra_pos(void* mp)
{
	stamp();
	printf("test_graph_dijkstra_pos\n");
	ec_graph_ptr_t g;
	ec_status_t st = ec_graph_ptr_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	void *n1 = &x1, *n2 = &x2, *n3 = &x3, *n4 = &x4;
	TASSERT(add_dir(&g, n1, n2, 1) == EC_OK);
	TASSERT(add_dir(&g, n2, n3, 2) == EC_OK);
	TASSERT(add_dir(&g, n1, n3, 5) == EC_OK);
	TASSERT(add_dir(&g, n3, n4, 1) == EC_OK);
	int* distances = NULL;
	TASSERT(g.dijkstra(&g, &n1, &distances) == EC_OK);
	TASSERT(distances != NULL);
	TASSERT(distances[0] == 0);
	free(distances);
	g.clear(&g);
	printf("pass:test_graph_dijkstra_pos\n");
}

/* Test Dijkstra with NULL arguments and non-existent node */
static void
test_graph_dijkstra_neg(void* mp)
{
	stamp();
	printf("test_graph_dijkstra_neg\n");
	ec_graph_ptr_t g;
	ec_status_t st = ec_graph_ptr_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	void *n1 = &x1, *n2 = &x2, *n99 = &x99;
	TASSERT(add_dir(&g, n1, n2, 1) == EC_OK);
	int* distances = NULL;
	/* Test NULL arguments */
	TASSERT(g.dijkstra(NULL, &n1, &distances) == EC_ERR_INVALID_ARG);
	TASSERT(g.dijkstra(&g, NULL, &distances) == EC_ERR_INVALID_ARG);
	TASSERT(g.dijkstra(&g, &n1, NULL) == EC_ERR_INVALID_ARG);
	/* Test non-existent node */
	TASSERT(g.dijkstra(&g, &n99, &distances) == EC_ERR_NOT_FOUND);
	g.clear(&g);
	printf("pass:test_graph_dijkstra_neg\n");
}

/* Removal of a node and validation of incident edge cleanup. */
static void
test_graph_remove_node_pos(void* mp)
{
	stamp();
	printf("test_graph_remove_node_pos\n");
	ec_graph_ptr_t g;
	ec_status_t st = ec_graph_ptr_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	void *n1 = &x1, *n2 = &x2, *n3 = &x3;
	TASSERT(add_undir(&g, n1, n2, 1) == EC_OK);
	TASSERT(add_undir(&g, n2, n3, 1) == EC_OK);
	TASSERT(g.remove_node(&g, &n2) == EC_OK);
	TASSERT(g.node_count == 2);
	TASSERT(!has_edge(&g, n1, n2));
	TASSERT(!has_edge(&g, n3, n2));
	g.clear(&g);
	printf("pass:test_graph_remove_node_pos\n");
}

/* Test remove_node with NULL arguments and non-existent node */
static void
test_graph_remove_node_neg(void* mp)
{
	stamp();
	printf("test_graph_remove_node_neg\n");
	ec_graph_ptr_t g;
	ec_status_t st = ec_graph_ptr_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	void *n1 = &x1, *n2 = &x2, *n99 = &x99;
	TASSERT(add_dir(&g, n1, n2, 1) == EC_OK);
	/* Test NULL arguments */
	TASSERT(g.remove_node(NULL, &n1) == EC_ERR_INVALID_ARG);
	TASSERT(g.remove_node(&g, NULL) == EC_ERR_INVALID_ARG);
	/* Test non-existent node */
	TASSERT(g.remove_node(&g, &n99) == EC_ERR_NOT_FOUND);
	g.clear(&g);
	printf("pass:test_graph_remove_node_neg\n");
}

/* Removal of edges (directed and undirected). */
static void
test_graph_remove_edge_pos(void* mp)
{
	stamp();
	printf("test_graph_remove_edge_pos\n");
	ec_graph_ptr_t g;
	ec_status_t st = ec_graph_ptr_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	void *n1 = &x1, *n2 = &x2, *n3 = &x3;
	TASSERT(add_undir(&g, n1, n2, 1) == EC_OK);
	TASSERT(add_dir(&g, n1, n3, 1) == EC_OK);
	TASSERT(g.remove_edge(&g, &n1, &n2, 0) == EC_OK);
	TASSERT(!has_edge(&g, n1, n2) && !has_edge(&g, n2, n1));
	TASSERT(g.remove_edge(&g, &n1, &n3, 1) == EC_OK);
	TASSERT(!has_edge(&g, n1, n3));
	g.clear(&g);
	printf("pass:test_graph_remove_edge_pos\n");
}

/* Test remove_edge with NULL arguments and non-existent nodes/edges */
static void
test_graph_remove_edge_neg(void* mp)
{
	stamp();
	printf("test_graph_remove_edge_neg\n");
	ec_graph_ptr_t g;
	ec_status_t st = ec_graph_ptr_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	void *n1 = &x1, *n2 = &x2, *n99 = &x99;
	TASSERT(add_dir(&g, n1, n2, 1) == EC_OK);
	/* Test NULL arguments */
	TASSERT(g.remove_edge(NULL, &n1, &n2, 1) == EC_ERR_INVALID_ARG);
	TASSERT(g.remove_edge(&g, NULL, &n2, 1) == EC_ERR_INVALID_ARG);
	TASSERT(g.remove_edge(&g, &n1, NULL, 1) == EC_ERR_INVALID_ARG);
	/* Test non-existent nodes */
	TASSERT(g.remove_edge(&g, &n99, &n2, 1) == EC_ERR_NOT_FOUND);
	TASSERT(g.remove_edge(&g, &n1, &n99, 1) == EC_ERR_NOT_FOUND);
	g.clear(&g);
	printf("pass:test_graph_remove_edge_neg\n");
}

/* Clear. */
static void
test_graph_clear_pos(void* mp)
{
	stamp();
	printf("test_graph_clear_pos\n");
	ec_graph_ptr_t g;
	ec_status_t st = ec_graph_ptr_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	void *n1 = &x1, *n2 = &x2;
	TASSERT(add_undir(&g, n1, n2, 1) == EC_OK);
	TASSERT(g.node_count > 0);
	TASSERT(g.clear(&g) == EC_OK);
	TASSERT(g.node_count == 0);
	g.clear(&g);
	printf("pass:test_graph_clear_pos\n");
}

/* Test clear with NULL argument */
static void
test_graph_clear_neg(void* mp)
{
	stamp();
	printf("test_graph_clear_neg\n");
	ec_graph_ptr_t g;
	ec_status_t st = ec_graph_ptr_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	TASSERT(g.clear(NULL) == EC_ERR_INVALID_ARG);
	g.clear(&g);
	printf("pass:test_graph_clear_neg\n");
}

/* --- Main --- */
int
main(void)
{
	printf("Starting ec_graph_ptr tests...\n");
#ifdef EC_USE_MEMPOOL
	ec_mempool_conf_t conf = {
		.block_size = 64,
		.max_blocks = 100,
		.mode = EC_MEMPOOL_DYNAMIC,
		.thread_mode = EC_THREAD_UNSAFE,
		.alignment = 8,
		.flags = 0,
		.grow_factor = 2.0f,
		.alloc_fn = NULL,
		.free_fn = NULL,
		.realloc_fn = NULL
	};
	ec_mempool_t pool;
	ec_status_t st = ec_mempool_init(&pool, &conf);
	TASSERT(st == EC_OK);
	void* mp = &pool;
#else
	void* mp = NULL;
#endif

	test_graph_init_pos(mp);
	test_graph_init_neg_null(mp);
	test_graph_insert_node_pos(mp);
	test_graph_insert_node_neg(mp);
	test_graph_insert_edge_pos(mp);
	test_graph_insert_edge_neg(mp);
	test_graph_bfs_pos(mp);
	test_graph_bfs_neg(mp);
	test_graph_dfs_pos(mp);
	test_graph_dfs_neg(mp);
	test_graph_dijkstra_pos(mp);
	test_graph_dijkstra_neg(mp);
	test_graph_remove_node_pos(mp);
	test_graph_remove_node_neg(mp);
	test_graph_remove_edge_pos(mp);
	test_graph_remove_edge_neg(mp);
	test_graph_clear_pos(mp);
	test_graph_clear_neg(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif

	printf("All ec_graph_ptr tests passed.\n");
	return 0;
}
