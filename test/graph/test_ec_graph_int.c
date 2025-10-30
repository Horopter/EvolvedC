#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#include "../../src/ec_graph.h"
#include <stdbool.h>
#ifdef EC_USE_MEMPOOL
#include "ec_mempool.h"
#endif

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

/* --- Global variables for traversal tests --- */

static int bfs_result[100];
static int bfs_index = 0;

static void
bfs_visit(int key)
{
	bfs_result[bfs_index++] = key;
}

static int dfs_result[100];
static int dfs_index = 0;

static void
dfs_visit(int key)
{
	dfs_result[dfs_index++] = key;
}

/* --- Test Functions --- */

/* Test that a newly initialized graph is empty. */
static void
test_graph_init(void* mp)
{
	stamp();
	printf("test_graph_init\n");
	ec_graph_int_t g;
	ec_status_t st = ec_graph_int_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	TASSERT(g.node_count == 0);
	g.clear(&g);
	printf("pass:test_graph_init\n");
}

/* Test node insertion (and duplicate avoidance). */
static void
test_graph_insert_node(void* mp)
{
	stamp();
	printf("test_graph_insert_node\n");
	ec_graph_int_t g;
	ec_status_t st = ec_graph_int_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	int n1 = 10;
	st = g.insert_node(&g, &n1);
	TASSERT(st == EC_OK);
	TASSERT(g.node_count == 1);
	ec_graph_int_node_t* node = ec_graph_int_find(&g, &n1);
	TASSERT(node != NULL);
	/* Inserting the same node again should not add a duplicate */
	st = g.insert_node(&g, &n1);
	TASSERT(st == EC_OK);
	TASSERT(g.node_count == 1);
	int n2 = 20;
	st = g.insert_node(&g, &n2);
	TASSERT(st == EC_OK);
	TASSERT(g.node_count == 2);
	g.clear(&g);
	printf("pass:test_graph_insert_node\n");
}

/* Test edge insertion in both directed and undirected mode. */
static void
test_graph_insert_edge(void* mp)
{
	stamp();
	printf("test_graph_insert_edge\n");
	ec_graph_int_t g;
	ec_status_t st = ec_graph_int_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	/* Insert a directed edge from 10 to 20 */
	int src = 10, dst = 20;
	st = g.insert_edge(&g, &src, &dst, 5, 1);
	TASSERT(st == EC_OK);
	TASSERT(g.node_count == 2);
	ec_graph_int_node_t* node_src = ec_graph_int_find(&g, &src);
	TASSERT(node_src != NULL);
	TASSERT(node_src->edge_count == 1);
	TASSERT(node_src->edges[0].dst == ec_graph_int_find(&g, &dst));

	/* Insert an undirected edge from 20 to 30 */
	int n30 = 30;
	st = g.insert_edge(&g, &dst, &n30, 2, 0); /* undirected: adds edge in both directions */
	TASSERT(st == EC_OK);
	ec_graph_int_node_t* node_dst = ec_graph_int_find(&g, &dst);
	TASSERT(node_dst != NULL);
	ec_graph_int_node_t* node_n30 = ec_graph_int_find(&g, &n30);
	TASSERT(node_n30 != NULL);
	{
		bool found_dn = false;
		for (size_t i = 0; i < node_dst->edge_count; i++)
			{
				if (node_dst->edges[i].dst == node_n30)
					{
						found_dn = true;
						break;
					}
			}
		TASSERT(found_dn);
	}
	{
		bool found_nd = false;
		for (size_t i = 0; i < node_n30->edge_count; i++)
			{
				if (node_n30->edges[i].dst == node_dst)
					{
						found_nd = true;
						break;
					}
			}
		TASSERT(found_nd);
	}
	g.clear(&g);
	printf("pass:test_graph_insert_edge\n");
}

/* Test Breadth-First Search (BFS) on a simple directed tree.
   The graph is:
         1
        / \
       2   3
      / \
     4   5
       \
        6 (from node 3)
*/
static void
test_graph_bfs(void* mp)
{
	stamp();
	printf("test_graph_bfs\n");
	ec_graph_int_t g;
	ec_status_t st = ec_graph_int_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	int n1 = 1, n2 = 2, n3 = 3, n4 = 4, n5 = 5, n6 = 6;
	st = g.insert_edge(&g, &n1, &n2, 1, 1); /* 1->2 */
	TASSERT(st == EC_OK);
	st = g.insert_edge(&g, &n1, &n3, 1, 1); /* 1->3 */
	TASSERT(st == EC_OK);
	st = g.insert_edge(&g, &n2, &n4, 1, 1); /* 2->4 */
	TASSERT(st == EC_OK);
	st = g.insert_edge(&g, &n2, &n5, 1, 1); /* 2->5 */
	TASSERT(st == EC_OK);
	st = g.insert_edge(&g, &n3, &n6, 1, 1); /* 3->6 */
	TASSERT(st == EC_OK);
	bfs_index = 0;
	st = g.bfs(&g, &n1, bfs_visit);
	TASSERT(st == EC_OK);
	/* Expected BFS order: 1, 2, 3, 4, 5, 6 */
	int expected[] = { 1, 2, 3, 4, 5, 6 };
	TASSERT(bfs_index == 6);
	for (int i = 0; i < 6; i++)
		{
			TASSERT(bfs_result[i] == expected[i]);
		}
	g.clear(&g);
	printf("pass:test_graph_bfs\n");
}

/* Test Depth-First Search (DFS) on the same tree.
   Expected DFS order (following insertion order): 1, 2, 4, 5, 3, 6
*/
static void
test_graph_dfs(void* mp)
{
	stamp();
	printf("test_graph_dfs\n");
	ec_graph_int_t g;
	ec_status_t st = ec_graph_int_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	int n1 = 1, n2 = 2, n3 = 3, n4 = 4, n5 = 5, n6 = 6;
	st = g.insert_edge(&g, &n1, &n2, 1, 1); /* 1->2 */
	TASSERT(st == EC_OK);
	st = g.insert_edge(&g, &n1, &n3, 1, 1); /* 1->3 */
	TASSERT(st == EC_OK);
	st = g.insert_edge(&g, &n2, &n4, 1, 1); /* 2->4 */
	TASSERT(st == EC_OK);
	st = g.insert_edge(&g, &n2, &n5, 1, 1); /* 2->5 */
	TASSERT(st == EC_OK);
	st = g.insert_edge(&g, &n3, &n6, 1, 1); /* 3->6 */
	TASSERT(st == EC_OK);
	dfs_index = 0;
	st = g.dfs(&g, &n1, dfs_visit);
	TASSERT(st == EC_OK);
	/* Expected DFS order: 1, 2, 4, 5, 3, 6 */
	int expected[] = { 1, 2, 4, 5, 3, 6 };
	TASSERT(dfs_index == 6);
	for (int i = 0; i < 6; i++)
		{
			TASSERT(dfs_result[i] == expected[i]);
		}
	g.clear(&g);
	printf("pass:test_graph_dfs\n");
}

/* Test Dijkstra’s algorithm on a simple weighted directed graph.
   Graph:
       1 -> 2 (weight 1)
       2 -> 3 (weight 2)
       1 -> 3 (weight 5)
       3 -> 4 (weight 1)
   Expected shortest distances from 1: {0, 1, 3, 4}
*/
static void
test_graph_dijkstra(void* mp)
{
	stamp();
	printf("test_graph_dijkstra\n");
	ec_graph_int_t g;
	ec_status_t st = ec_graph_int_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	int n1 = 1, n2 = 2, n3 = 3, n4 = 4;
	st = g.insert_edge(&g, &n1, &n2, 1, 1); /* 1->2 */
	TASSERT(st == EC_OK);
	st = g.insert_edge(&g, &n2, &n3, 2, 1); /* 2->3 */
	TASSERT(st == EC_OK);
	st = g.insert_edge(&g, &n1, &n3, 5, 1); /* 1->3 */
	TASSERT(st == EC_OK);
	st = g.insert_edge(&g, &n3, &n4, 1, 1); /* 3->4 */
	TASSERT(st == EC_OK);
	int* distances = NULL;
	st = g.dijkstra(&g, &n1, &distances);
	TASSERT(st == EC_OK);
	/* Assuming nodes are in order of first insertion: 1, 2, 3, 4 */
	TASSERT(distances[0] == 0);
	TASSERT(distances[1] == 1);
	TASSERT(distances[2] == 3);
	TASSERT(distances[3] == 4);
	free(distances);
	g.clear(&g);
	printf("pass:test_graph_dijkstra\n");
}

/* Test removal of a node.
   Create an undirected graph:
       1 -- 2 -- 3
   Remove node 2 and verify that no remaining node has an edge pointing to 2.
*/
static void
test_graph_remove_node(void* mp)
{
	stamp();
	printf("test_graph_remove_node\n");
	ec_graph_int_t g;
	ec_status_t st = ec_graph_int_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	int n1 = 1, n2 = 2, n3 = 3;
	st = g.insert_edge(&g, &n1, &n2, 1, 0); /* undirected edge 1<->2 */
	TASSERT(st == EC_OK);
	st = g.insert_edge(&g, &n2, &n3, 1, 0); /* undirected edge 2<->3 */
	TASSERT(st == EC_OK);
	st = g.remove_node(&g, &n2);
	TASSERT(st == EC_OK);
	TASSERT(g.node_count == 2);
	ec_graph_int_node_t* node1 = ec_graph_int_find(&g, &n1);
	TASSERT(node1 != NULL);
	for (size_t i = 0; i < node1->edge_count; i++)
		{
			TASSERT(node1->edges[i].dst->key != n2);
		}
	ec_graph_int_node_t* node3 = ec_graph_int_find(&g, &n3);
	TASSERT(node3 != NULL);
	for (size_t i = 0; i < node3->edge_count; i++)
		{
			TASSERT(node3->edges[i].dst->key != n2);
		}
	g.clear(&g);
	printf("pass:test_graph_remove_node\n");
}

/* Test removal of an edge.
   Build a graph with a undirected edge between 1 and 2 and a directed edge 1->3.
   Then remove them and verify that the edge lists are updated.
*/
static void
test_graph_remove_edge(void* mp)
{
	stamp();
	printf("test_graph_remove_edge\n");
	ec_graph_int_t g;
	ec_status_t st = ec_graph_int_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	int n1 = 1, n2 = 2, n3 = 3;
	st = g.insert_edge(&g, &n1, &n2, 1, 0); /* undirected edge 1<->2 */
	TASSERT(st == EC_OK);
	st = g.insert_edge(&g, &n1, &n3, 1, 1); /* directed edge 1->3 */
	TASSERT(st == EC_OK);
	st = g.remove_edge(&g, &n1, &n2, 0);
	TASSERT(st == EC_OK);
	ec_graph_int_node_t* node1 = ec_graph_int_find(&g, &n1);
	bool found = false;
	for (size_t i = 0; i < node1->edge_count; i++)
		{
			if (node1->edges[i].dst->key == n2)
				{
					found = true;
					break;
				}
		}
	TASSERT(!found);
	ec_graph_int_node_t* node2 = ec_graph_int_find(&g, &n2);
	found = false;
	for (size_t i = 0; i < node2->edge_count; i++)
		{
			if (node2->edges[i].dst->key == n1)
				{
					found = true;
					break;
				}
		}
	TASSERT(!found);
	st = g.remove_edge(&g, &n1, &n3, 1);
	TASSERT(st == EC_OK);
	found = false;
	for (size_t i = 0; i < node1->edge_count; i++)
		{
			if (node1->edges[i].dst->key == n3)
				{
					found = true;
					break;
				}
		}
	TASSERT(!found);
	g.clear(&g);
	printf("pass:test_graph_remove_edge\n");
}

/* Test clear: insert an edge then clear the graph. */
static void
test_graph_clear(void* mp)
{
	stamp();
	printf("test_graph_clear\n");
	ec_graph_int_t g;
	ec_status_t st = ec_graph_int_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	int n1 = 1, n2 = 2;
	st = g.insert_edge(&g, &n1, &n2, 1, 0);
	TASSERT(st == EC_OK);
	TASSERT(g.node_count > 0);
	st = g.clear(&g);
	TASSERT(st == EC_OK);
	TASSERT(g.node_count == 0);
	printf("pass:test_graph_clear\n");
}

/* Test using a custom compare function.
   Here we define a descending order compare so that duplicate “equal”
   values are detected properly.
*/
static int
cmp_desc(const int* a, const int* b)
{
	return (*a > *b) ? -1 : ((*a < *b) ? 1 : 0);
}

static void
test_graph_custom_compare(void* mp)
{
	stamp();
	printf("test_graph_custom_compare\n");
	ec_graph_int_t g;
	ec_status_t st = ec_graph_int_init(&g, mp, cmp_desc);
	TASSERT(st == EC_OK);
	int n1 = 5, n2 = 10, n3 = 5;
	st = g.insert_node(&g, &n1);
	TASSERT(st == EC_OK);
	TASSERT(g.node_count == 1);
	/* Inserting an equal value (5) should not add a new node */
	st = g.insert_node(&g, &n3);
	TASSERT(st == EC_OK);
	TASSERT(g.node_count == 1);
	st = g.insert_node(&g, &n2);
	TASSERT(st == EC_OK);
	TASSERT(g.node_count == 2);
	g.clear(&g);
	printf("pass:test_graph_custom_compare\n");
}

/* Test inserting many random nodes.
   (Since duplicate values are not added, the final node count should be
    less than or equal to the range of random values.)
*/
static void
test_graph_big_random(void* mp)
{
	stamp();
	printf("test_graph_big_random\n");
	ec_graph_int_t g;
	ec_status_t st = ec_graph_int_init(&g, mp, NULL);
	TASSERT(st == EC_OK);
	srand((unsigned)time(NULL));
	const int N = 1000;
	for (int i = 0; i < N; i++)
		{
			int val = rand() % 500; /* many duplicates expected */
			st = g.insert_node(&g, &val);
			TASSERT(st == EC_OK);
		}
	TASSERT(g.node_count <= 500);
	g.clear(&g);
	printf("pass:test_graph_big_random\n");
}

/* --- Main --- */
int
main(void)
{
	printf("Starting ec_graph_int tests...\n");
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

	test_graph_init(mp);
	test_graph_insert_node(mp);
	test_graph_insert_edge(mp);
	test_graph_bfs(mp);
	test_graph_dfs(mp);
	test_graph_dijkstra(mp);
	test_graph_remove_node(mp);
	test_graph_remove_edge(mp);
	test_graph_clear(mp);
	test_graph_custom_compare(mp);
	test_graph_big_random(mp);

#ifdef EC_USE_MEMPOOL
	ec_mempool_destroy((ec_mempool_t*)mp);
#endif

	printf("All ec_graph_int tests passed.\n");
	return 0;
}
