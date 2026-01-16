/**
 * =============================================================================
 * EXERCISE 1: Breadth-First Search and Shortest Paths
 * =============================================================================
 *
 * Objective
 *   Implement Breadth-First Search (BFS) traversal and use it to answer
 *   shortest-path queries in an unweighted undirected graph.
 *
 * Input format
 *   Line 1: n m (number of vertices and edges)
 *   Next m lines: u v (undirected edge)
 *   Next line: q (number of queries)
 *   Next q lines: s t (shortest path from s to t)
 *
 * Output format
 *   For each query where a path exists:
 *     Path from s to t: s -> ... -> t (distance: d)
 *   If t is unreachable from s:
 *     No path from s to t
 *
 * Compilation
 *   gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
 *
 * =============================================================================
 */

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* =============================================================================
 * CONSTANTS
 * =============================================================================
 */

#define MAX_VERTICES 1000
#define INFINITY_DIST INT_MAX

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * Adjacency list node.
 */
typedef struct AdjNode {
    int vertex;
    struct AdjNode *next;
} AdjNode;

/**
 * Fixed-capacity adjacency-list graph.
 *
 * Note
 *   The adjacency array is sized to MAX_VERTICES for pedagogical simplicity.
 *   Only indices 0..num_vertices-1 are considered valid.
 */
typedef struct {
    int num_vertices;
    int num_edges;
    AdjNode *adj[MAX_VERTICES];
} Graph;

/**
 * Queue structure for BFS.
 *
 * This queue is sized to MAX_VERTICES. BFS enqueues each vertex at most once so
 * it will not overflow provided num_vertices <= MAX_VERTICES.
 */
typedef struct {
    int items[MAX_VERTICES];
    int front;
    int rear;
} Queue;

/* =============================================================================
 * QUEUE OPERATIONS (PROVIDED)
 * =============================================================================
 */

static void queue_init(Queue *q) {
    q->front = 0;
    q->rear = -1;
}

static bool queue_is_empty(const Queue *q) {
    return q->rear < q->front;
}

static void queue_enqueue(Queue *q, int value) {
    q->items[++q->rear] = value;
}

static int queue_dequeue(Queue *q) {
    return q->items[q->front++];
}

/* =============================================================================
 * GRAPH OPERATIONS
 * =============================================================================
 */

static void graph_init(Graph *g, int n) {
    g->num_vertices = n;
    g->num_edges = 0;
    for (int i = 0; i < n; i++) {
        g->adj[i] = NULL;
    }
}

static AdjNode *create_node(int v) {
    AdjNode *node = (AdjNode *)malloc(sizeof(AdjNode));
    if (node == NULL) {
        return NULL;
    }
    node->vertex = v;
    node->next = NULL;
    return node;
}

static void graph_add_edge(Graph *g, int u, int v) {
    /* Insert at head so neighbour visitation order is deterministic under tests. */
    AdjNode *node_v = create_node(v);
    if (node_v != NULL) {
        node_v->next = g->adj[u];
        g->adj[u] = node_v;
    }

    AdjNode *node_u = create_node(u);
    if (node_u != NULL) {
        node_u->next = g->adj[v];
        g->adj[v] = node_u;
    }

    g->num_edges++;
}

static void bfs(const Graph *g, int source, int dist[], int parent[]) {
    for (int i = 0; i < g->num_vertices; i++) {
        dist[i] = INFINITY_DIST;
        parent[i] = -1;
    }

    Queue q;
    queue_init(&q);

    dist[source] = 0;
    queue_enqueue(&q, source);

    while (!queue_is_empty(&q)) {
        int u = queue_dequeue(&q);

        for (AdjNode *curr = g->adj[u]; curr != NULL; curr = curr->next) {
            int v = curr->vertex;
            if (dist[v] == INFINITY_DIST) {
                dist[v] = dist[u] + 1;
                parent[v] = u;
                queue_enqueue(&q, v);
            }
        }
    }
}

static void print_path(int source, int target, const int dist[], const int parent[]) {
    if (dist[target] == INFINITY_DIST) {
        printf("No path from %d to %d\n", source, target);
        return;
    }

    int path[MAX_VERTICES];
    int len = 0;

    for (int cur = target; cur != -1; cur = parent[cur]) {
        path[len++] = cur;
        if (len >= MAX_VERTICES) {
            /* Defensive: should not occur in a valid BFS tree. */
            break;
        }
    }

    printf("Path from %d to %d: ", source, target);
    for (int i = len - 1; i >= 0; i--) {
        printf("%d", path[i]);
        if (i > 0) {
            printf(" -> ");
        }
    }
    printf(" (distance: %d)\n", dist[target]);
}

static void graph_destroy(Graph *g) {
    for (int i = 0; i < g->num_vertices; i++) {
        AdjNode *curr = g->adj[i];
        while (curr != NULL) {
            AdjNode *tmp = curr;
            curr = curr->next;
            free(tmp);
        }
        g->adj[i] = NULL;
    }
    g->num_vertices = 0;
    g->num_edges = 0;
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    Graph g;
    int n, m;

    if (scanf("%d %d", &n, &m) != 2) {
        fprintf(stderr, "Error: Failed to read graph dimensions\n");
        return 1;
    }

    if (n <= 0 || n > MAX_VERTICES) {
        fprintf(stderr, "Error: Invalid number of vertices\n");
        return 1;
    }

    graph_init(&g, n);

    for (int i = 0; i < m; i++) {
        int u, v;
        if (scanf("%d %d", &u, &v) != 2) {
            fprintf(stderr, "Error: Failed to read edge %d\n", i + 1);
            graph_destroy(&g);
            return 1;
        }

        if (u < 0 || u >= n || v < 0 || v >= n) {
            fprintf(stderr, "Error: Invalid vertex in edge %d\n", i + 1);
            graph_destroy(&g);
            return 1;
        }

        graph_add_edge(&g, u, v);
    }

    int q;
    if (scanf("%d", &q) != 1) {
        fprintf(stderr, "Error: Failed to read number of queries\n");
        graph_destroy(&g);
        return 1;
    }

    int dist[MAX_VERTICES];
    int parent[MAX_VERTICES];
    int last_source = -1;

    for (int i = 0; i < q; i++) {
        int s, t;
        if (scanf("%d %d", &s, &t) != 2) {
            fprintf(stderr, "Error: Failed to read query %d\n", i + 1);
            graph_destroy(&g);
            return 1;
        }

        if (s < 0 || s >= n || t < 0 || t >= n) {
            fprintf(stderr, "Error: Invalid vertex in query %d\n", i + 1);
            continue;
        }

        /* Recompute BFS tree only when the source changes. */
        if (s != last_source) {
            bfs(&g, s, dist, parent);
            last_source = s;
        }

        print_path(s, t, dist, parent);
    }

    graph_destroy(&g);
    return 0;
}
