#include "../../src/ec_graph.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Helper macros */
#define TASSERT(COND)                                                                                    \
	do                                                                                                   \
		{                                                                                                \
			if (!(COND))                                                                                 \
				{                                                                                        \
					fprintf(stderr, "[FAIL] %s:%d: Assertion '%s' failed\n", __FILE__, __LINE__, #COND); \
					exit(1);                                                                             \
				}                                                                                        \
		}                                                                                                \
	while (0)

/* Helper function to check if edge exists */
static bool
has_edge_str(ec_graph_str_t* g, const char* u, const char* v)
{
	ec_graph_str_node_t* nu = ec_graph_str_find(g, &u);
	ec_graph_str_node_t* nv = ec_graph_str_find(g, &v);
	if (!nu || !nv)
		return false;
	for (size_t i = 0; i < nu->edge_count; i++)
		{
			if (nu->edges[i].dst == nv)
				return true;
		}
	return false;
}

/* Helper for directed edges */
static inline ec_status_t
add_dir_str(ec_graph_str_t* g, const char* u, const char* v, int w)
{
	return g->insert_edge(g, &u, &v, w, 1);
}

/* Helper for undirected edges */
static inline ec_status_t
add_undir_str(ec_graph_str_t* g, const char* u, const char* v, int w)
{
	return g->insert_edge(g, &u, &v, w, 0);
}

/* Global variables for traversal tests */
static const char* bfs_result[100];
static int bfs_index = 0;

static void
bfs_visit_str(const char* key)
{
	bfs_result[bfs_index++] = key;
}

static const char* dfs_result[100];
static int dfs_index = 0;

static void
dfs_visit_str(const char* key)
{
	dfs_result[dfs_index++] = key;
}

/* Test Functions */

/* Test init - positive */
static void
test_graph_init_pos(void)
{
	printf("test_graph_init_pos\n");
	ec_graph_str_t g;
	ec_status_t st = ec_graph_str_init(&g, NULL, NULL);
	TASSERT(st == EC_OK);
	TASSERT(g.node_count == 0);
	g.clear(&g);
	printf("pass:test_graph_init_pos\n");
}

/* Test init - negative (NULL pointer) */
static void
test_graph_init_neg(void)
{
	printf("test_graph_init_neg\n");
	ec_status_t st = ec_graph_str_init(NULL, NULL, NULL);
	TASSERT(st == EC_ERR_INVALID_ARG);
	printf("pass:test_graph_init_neg\n");
}

/* Test insert_node - positive */
static void
test_graph_insert_node_pos(void)
{
	printf("test_graph_insert_node_pos\n");
	ec_graph_str_t g;
	ec_status_t st = ec_graph_str_init(&g, NULL, NULL);
	TASSERT(st == EC_OK);
	const char* n1 = "A";
	st = g.insert_node(&g, &n1);
	TASSERT(st == EC_OK);
	TASSERT(g.node_count == 1);
	TASSERT(ec_graph_str_find(&g, &n1) != NULL);
	/* Inserting the same node again should not add a duplicate */
	st = g.insert_node(&g, &n1);
	TASSERT(st == EC_OK);
	TASSERT(g.node_count == 1);
	const char* n2 = "B";
	st = g.insert_node(&g, &n2);
	TASSERT(st == EC_OK);
	TASSERT(g.node_count == 2);
	g.clear(&g);
	printf("pass:test_graph_insert_node_pos\n");
}

/* Test insert_node - negative (NULL arguments) */
static void
test_graph_insert_node_neg(void)
{
	printf("test_graph_insert_node_neg\n");
	ec_graph_str_t g;
	ec_status_t st = ec_graph_str_init(&g, NULL, NULL);
	TASSERT(st == EC_OK);
	const char* n1 = "A";
	TASSERT(g.insert_node(NULL, &n1) == EC_ERR_INVALID_ARG);
	TASSERT(g.insert_node(&g, NULL) == EC_ERR_INVALID_ARG);
	g.clear(&g);
	printf("pass:test_graph_insert_node_neg\n");
}

/* Test insert_edge - positive */
static void
test_graph_insert_edge_pos(void)
{
	printf("test_graph_insert_edge_pos\n");
	ec_graph_str_t g;
	ec_status_t st = ec_graph_str_init(&g, NULL, NULL);
	TASSERT(st == EC_OK);
	const char *a = "A", *b = "B", *c = "C";
	/* directed A->B */
	TASSERT(add_dir_str(&g, a, b, 5) == EC_OK);
	TASSERT(g.node_count == 2);
	TASSERT(has_edge_str(&g, a, b));
	/* undirected B<->C */
	TASSERT(add_undir_str(&g, b, c, 2) == EC_OK);
	TASSERT(has_edge_str(&g, b, c) && has_edge_str(&g, c, b));
	g.clear(&g);
	printf("pass:test_graph_insert_edge_pos\n");
}

/* Test insert_edge - negative (NULL arguments) */
static void
test_graph_insert_edge_neg(void)
{
	printf("test_graph_insert_edge_neg\n");
	ec_graph_str_t g;
	ec_status_t st = ec_graph_str_init(&g, NULL, NULL);
	TASSERT(st == EC_OK);
	const char *a = "A", *b = "B";
	TASSERT(g.insert_edge(NULL, &a, &b, 1, 0) == EC_ERR_INVALID_ARG);
	TASSERT(g.insert_edge(&g, NULL, &b, 1, 0) == EC_ERR_INVALID_ARG);
	TASSERT(g.insert_edge(&g, &a, NULL, 1, 0) == EC_ERR_INVALID_ARG);
	g.clear(&g);
	printf("pass:test_graph_insert_edge_neg\n");
}

/* Test BFS - positive */
static void
test_graph_bfs_pos(void)
{
	printf("test_graph_bfs_pos\n");
	ec_graph_str_t g;
	ec_status_t st = ec_graph_str_init(&g, NULL, NULL);
	TASSERT(st == EC_OK);
	const char *a = "A", *b = "B", *c = "C", *d = "D", *e = "E", *f = "F";
	TASSERT(add_dir_str(&g, a, b, 1) == EC_OK);
	TASSERT(add_dir_str(&g, a, c, 1) == EC_OK);
	TASSERT(add_dir_str(&g, b, d, 1) == EC_OK);
	TASSERT(add_dir_str(&g, b, e, 1) == EC_OK);
	TASSERT(add_dir_str(&g, c, f, 1) == EC_OK);
	bfs_index = 0;
	TASSERT(g.bfs(&g, &a, bfs_visit_str) == EC_OK);
	TASSERT(bfs_index == 6);
	g.clear(&g);
	printf("pass:test_graph_bfs_pos\n");
}

/* Test BFS - negative (NULL arguments, non-existent node) */
static void
test_graph_bfs_neg(void)
{
	printf("test_graph_bfs_neg\n");
	ec_graph_str_t g;
	ec_status_t st = ec_graph_str_init(&g, NULL, NULL);
	TASSERT(st == EC_OK);
	const char *a = "A", *b = "B", *z = "Z";
	TASSERT(add_dir_str(&g, a, b, 1) == EC_OK);
	/* Test NULL arguments */
	TASSERT(g.bfs(NULL, &a, bfs_visit_str) == EC_ERR_INVALID_ARG);
	TASSERT(g.bfs(&g, NULL, bfs_visit_str) == EC_ERR_INVALID_ARG);
	TASSERT(g.bfs(&g, &a, NULL) == EC_ERR_INVALID_ARG);
	/* Test non-existent node */
	TASSERT(g.bfs(&g, &z, bfs_visit_str) == EC_ERR_NOT_FOUND);
	g.clear(&g);
	printf("pass:test_graph_bfs_neg\n");
}

/* Test DFS - positive */
static void
test_graph_dfs_pos(void)
{
	printf("test_graph_dfs_pos\n");
	ec_graph_str_t g;
	ec_status_t st = ec_graph_str_init(&g, NULL, NULL);
	TASSERT(st == EC_OK);
	const char *a = "A", *b = "B", *c = "C", *d = "D", *e = "E", *f = "F";
	TASSERT(add_dir_str(&g, a, b, 1) == EC_OK);
	TASSERT(add_dir_str(&g, a, c, 1) == EC_OK);
	TASSERT(add_dir_str(&g, b, d, 1) == EC_OK);
	TASSERT(add_dir_str(&g, b, e, 1) == EC_OK);
	TASSERT(add_dir_str(&g, c, f, 1) == EC_OK);
	dfs_index = 0;
	TASSERT(g.dfs(&g, &a, dfs_visit_str) == EC_OK);
	TASSERT(dfs_index == 6);
	g.clear(&g);
	printf("pass:test_graph_dfs_pos\n");
}

/* Test DFS - negative (NULL arguments, non-existent node) */
static void
test_graph_dfs_neg(void)
{
	printf("test_graph_dfs_neg\n");
	ec_graph_str_t g;
	ec_status_t st = ec_graph_str_init(&g, NULL, NULL);
	TASSERT(st == EC_OK);
	const char *a = "A", *b = "B", *z = "Z";
	TASSERT(add_dir_str(&g, a, b, 1) == EC_OK);
	/* Test NULL arguments */
	TASSERT(g.dfs(NULL, &a, dfs_visit_str) == EC_ERR_INVALID_ARG);
	TASSERT(g.dfs(&g, NULL, dfs_visit_str) == EC_ERR_INVALID_ARG);
	TASSERT(g.dfs(&g, &a, NULL) == EC_ERR_INVALID_ARG);
	/* Test non-existent node */
	TASSERT(g.dfs(&g, &z, dfs_visit_str) == EC_ERR_NOT_FOUND);
	g.clear(&g);
	printf("pass:test_graph_dfs_neg\n");
}

/* Test Dijkstra - positive */
static void
test_graph_dijkstra_pos(void)
{
	printf("test_graph_dijkstra_pos\n");
	ec_graph_str_t g;
	ec_status_t st = ec_graph_str_init(&g, NULL, NULL);
	TASSERT(st == EC_OK);
	const char *a = "A", *b = "B", *c = "C", *d = "D";
	TASSERT(add_dir_str(&g, a, b, 1) == EC_OK);
	TASSERT(add_dir_str(&g, b, c, 2) == EC_OK);
	TASSERT(add_dir_str(&g, a, c, 5) == EC_OK);
	TASSERT(add_dir_str(&g, c, d, 1) == EC_OK);
	int* distances = NULL;
	TASSERT(g.dijkstra(&g, &a, &distances) == EC_OK);
	TASSERT(distances != NULL);
	TASSERT(distances[0] == 0); /* A to A */
	free(distances);
	g.clear(&g);
	printf("pass:test_graph_dijkstra_pos\n");
}

/* Test Dijkstra - negative (NULL arguments, non-existent node) */
static void
test_graph_dijkstra_neg(void)
{
	printf("test_graph_dijkstra_neg\n");
	ec_graph_str_t g;
	ec_status_t st = ec_graph_str_init(&g, NULL, NULL);
	TASSERT(st == EC_OK);
	const char *a = "A", *b = "B", *z = "Z";
	TASSERT(add_dir_str(&g, a, b, 1) == EC_OK);
	int* distances = NULL;
	/* Test NULL arguments */
	TASSERT(g.dijkstra(NULL, &a, &distances) == EC_ERR_INVALID_ARG);
	TASSERT(g.dijkstra(&g, NULL, &distances) == EC_ERR_INVALID_ARG);
	TASSERT(g.dijkstra(&g, &a, NULL) == EC_ERR_INVALID_ARG);
	/* Test non-existent node */
	TASSERT(g.dijkstra(&g, &z, &distances) == EC_ERR_NOT_FOUND);
	g.clear(&g);
	printf("pass:test_graph_dijkstra_neg\n");
}

/* Test remove_node - positive */
static void
test_graph_remove_node_pos(void)
{
	printf("test_graph_remove_node_pos\n");
	ec_graph_str_t g;
	ec_status_t st = ec_graph_str_init(&g, NULL, NULL);
	TASSERT(st == EC_OK);
	const char *a = "A", *b = "B", *c = "C";
	TASSERT(add_undir_str(&g, a, b, 1) == EC_OK);
	TASSERT(add_undir_str(&g, b, c, 1) == EC_OK);
	TASSERT(g.remove_node(&g, &b) == EC_OK);
	TASSERT(g.node_count == 2);
	TASSERT(!has_edge_str(&g, a, b));
	TASSERT(!has_edge_str(&g, c, b));
	g.clear(&g);
	printf("pass:test_graph_remove_node_pos\n");
}

/* Test remove_node - negative (NULL arguments, non-existent node) */
static void
test_graph_remove_node_neg(void)
{
	printf("test_graph_remove_node_neg\n");
	ec_graph_str_t g;
	ec_status_t st = ec_graph_str_init(&g, NULL, NULL);
	TASSERT(st == EC_OK);
	const char *a = "A", *b = "B", *z = "Z";
	TASSERT(add_dir_str(&g, a, b, 1) == EC_OK);
	/* Test NULL arguments */
	TASSERT(g.remove_node(NULL, &a) == EC_ERR_INVALID_ARG);
	TASSERT(g.remove_node(&g, NULL) == EC_ERR_INVALID_ARG);
	/* Test non-existent node */
	TASSERT(g.remove_node(&g, &z) == EC_ERR_NOT_FOUND);
	g.clear(&g);
	printf("pass:test_graph_remove_node_neg\n");
}

/* Test remove_edge - positive */
static void
test_graph_remove_edge_pos(void)
{
	printf("test_graph_remove_edge_pos\n");
	ec_graph_str_t g;
	ec_status_t st = ec_graph_str_init(&g, NULL, NULL);
	TASSERT(st == EC_OK);
	const char *a = "A", *b = "B", *c = "C";
	TASSERT(add_undir_str(&g, a, b, 1) == EC_OK);
	TASSERT(add_dir_str(&g, a, c, 1) == EC_OK);
	TASSERT(g.remove_edge(&g, &a, &b, 0) == EC_OK);
	TASSERT(!has_edge_str(&g, a, b) && !has_edge_str(&g, b, a));
	TASSERT(g.remove_edge(&g, &a, &c, 1) == EC_OK);
	TASSERT(!has_edge_str(&g, a, c));
	g.clear(&g);
	printf("pass:test_graph_remove_edge_pos\n");
}

/* Test remove_edge - negative (NULL arguments, non-existent nodes) */
static void
test_graph_remove_edge_neg(void)
{
	printf("test_graph_remove_edge_neg\n");
	ec_graph_str_t g;
	ec_status_t st = ec_graph_str_init(&g, NULL, NULL);
	TASSERT(st == EC_OK);
	const char *a = "A", *b = "B", *z = "Z";
	TASSERT(add_dir_str(&g, a, b, 1) == EC_OK);
	/* Test NULL arguments */
	TASSERT(g.remove_edge(NULL, &a, &b, 1) == EC_ERR_INVALID_ARG);
	TASSERT(g.remove_edge(&g, NULL, &b, 1) == EC_ERR_INVALID_ARG);
	TASSERT(g.remove_edge(&g, &a, NULL, 1) == EC_ERR_INVALID_ARG);
	/* Test non-existent nodes */
	TASSERT(g.remove_edge(&g, &z, &b, 1) == EC_ERR_NOT_FOUND);
	TASSERT(g.remove_edge(&g, &a, &z, 1) == EC_ERR_NOT_FOUND);
	g.clear(&g);
	printf("pass:test_graph_remove_edge_neg\n");
}

/* Test clear - positive */
static void
test_graph_clear_pos(void)
{
	printf("test_graph_clear_pos\n");
	ec_graph_str_t g;
	ec_status_t st = ec_graph_str_init(&g, NULL, NULL);
	TASSERT(st == EC_OK);
	const char *a = "A", *b = "B";
	TASSERT(add_undir_str(&g, a, b, 1) == EC_OK);
	TASSERT(g.node_count > 0);
	TASSERT(g.clear(&g) == EC_OK);
	TASSERT(g.node_count == 0);
	g.clear(&g);
	printf("pass:test_graph_clear_pos\n");
}

/* Test clear - negative (NULL argument) */
static void
test_graph_clear_neg(void)
{
	printf("test_graph_clear_neg\n");
	ec_graph_str_t g;
	ec_status_t st = ec_graph_str_init(&g, NULL, NULL);
	TASSERT(st == EC_OK);
	TASSERT(g.clear(NULL) == EC_ERR_INVALID_ARG);
	g.clear(&g);
	printf("pass:test_graph_clear_neg\n");
}

/* Main */
int
main(void)
{
	printf("Starting ec_graph_str tests...\n");
	test_graph_init_pos();
	test_graph_init_neg();
	test_graph_insert_node_pos();
	test_graph_insert_node_neg();
	test_graph_insert_edge_pos();
	test_graph_insert_edge_neg();
	test_graph_bfs_pos();
	test_graph_bfs_neg();
	test_graph_dfs_pos();
	test_graph_dfs_neg();
	test_graph_dijkstra_pos();
	test_graph_dijkstra_neg();
	test_graph_remove_node_pos();
	test_graph_remove_node_neg();
	test_graph_remove_edge_pos();
	test_graph_remove_edge_neg();
	test_graph_clear_pos();
	test_graph_clear_neg();
	printf("All ec_graph_str tests passed.\n");
	return 0;
}
