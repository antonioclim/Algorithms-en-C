/**
 * =============================================================================
 * EXERCISE 1 SOLUTION: Breadth-First Search and Shortest Paths
 * =============================================================================
 *
 * INSTRUCTOR SOLUTION - DO NOT DISTRIBUTE TO STUDENTS
 *
 * This file contains the complete, working solution for Exercise 1.
 * All TODOs have been implemented with proper error handling and
 * memory management.
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

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
 * TODO 1 SOLUTION: Adjacency list node structure
 */
typedef struct AdjNode {
    int vertex;
    struct AdjNode *next;
} AdjNode;

/**
 * TODO 2 SOLUTION: Graph structure
 */
typedef struct {
    int num_vertices;
    int num_edges;
    AdjNode *adj[MAX_VERTICES];
} Graph;

/**
 * Queue structure for BFS
 */
typedef struct {
    int items[MAX_VERTICES];
    int front;
    int rear;
} Queue;

/* =============================================================================
 * QUEUE OPERATIONS
 * =============================================================================
 */

void queue_init(Queue *q) {
    q->front = 0;
    q->rear = -1;
}

bool queue_is_empty(Queue *q) {
    return q->rear < q->front;
}

void queue_enqueue(Queue *q, int value) {
    q->items[++q->rear] = value;
}

int queue_dequeue(Queue *q) {
    return q->items[q->front++];
}

/* =============================================================================
 * GRAPH OPERATIONS
 * =============================================================================
 */

/**
 * TODO 3 SOLUTION: Initialise the graph
 */
void graph_init(Graph *g, int n) {
    g->num_vertices = n;
    g->num_edges = 0;
    
    for (int i = 0; i < n; i++) {
        g->adj[i] = NULL;
    }
}

/**
 * TODO 4 SOLUTION: Create a new adjacency list node
 */
AdjNode *create_node(int v) {
    AdjNode *node = (AdjNode *)malloc(sizeof(AdjNode));
    if (node == NULL) {
        return NULL;
    }
    
    node->vertex = v;
    node->next = NULL;
    
    return node;
}

/**
 * TODO 5 SOLUTION: Add an undirected edge to the graph
 */
void graph_add_edge(Graph *g, int u, int v) {
    /* Add v to u's adjacency list (insert at head) */
    AdjNode *node_v = create_node(v);
    if (node_v != NULL) {
        node_v->next = g->adj[u];
        g->adj[u] = node_v;
    }
    
    /* Add u to v's adjacency list (insert at head) */
    AdjNode *node_u = create_node(u);
    if (node_u != NULL) {
        node_u->next = g->adj[v];
        g->adj[v] = node_u;
    }
    
    g->num_edges++;
}

/**
 * TODO 6 SOLUTION: Perform BFS and compute distances from source
 */
void bfs(Graph *g, int source, int dist[], int parent[]) {
    /* Step 1: Initialise all distances to infinity */
    for (int i = 0; i < g->num_vertices; i++) {
        dist[i] = INFINITY_DIST;
        parent[i] = -1;
    }
    
    /* Step 2: Set source distance to 0 */
    dist[source] = 0;
    
    /* Step 3: Create and initialise queue */
    Queue q;
    queue_init(&q);
    queue_enqueue(&q, source);
    
    /* Step 4: BFS loop */
    while (!queue_is_empty(&q)) {
        int u = queue_dequeue(&q);
        
        /* Explore all neighbours of u */
        AdjNode *curr = g->adj[u];
        while (curr != NULL) {
            int v = curr->vertex;
            
            /* If v is unvisited */
            if (dist[v] == INFINITY_DIST) {
                dist[v] = dist[u] + 1;
                parent[v] = u;
                queue_enqueue(&q, v);
            }
            
            curr = curr->next;
        }
    }
}

/**
 * TODO 7 SOLUTION: Print the shortest path from source to target
 */
void print_path(int source, int target, int dist[], int parent[]) {
    /* Check if path exists */
    if (dist[target] == INFINITY_DIST) {
        printf("No path from %d to %d\n", source, target);
        return;
    }
    
    /* Reconstruct path by following parent pointers */
    int path[MAX_VERTICES];
    int path_length = 0;
    
    int current = target;
    while (current != -1) {
        path[path_length++] = current;
        current = parent[current];
    }
    
    /* Print path in correct order (reverse of what we collected) */
    printf("Path from %d to %d: ", source, target);
    for (int i = path_length - 1; i >= 0; i--) {
        printf("%d", path[i]);
        if (i > 0) {
            printf(" -> ");
        }
    }
    printf(" (distance: %d)\n", dist[target]);
}

/**
 * TODO 8 SOLUTION: Free all memory allocated for the graph
 */
void graph_destroy(Graph *g) {
    for (int i = 0; i < g->num_vertices; i++) {
        AdjNode *curr = g->adj[i];
        while (curr != NULL) {
            AdjNode *temp = curr;
            curr = curr->next;
            free(temp);
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
    
    /* Read number of vertices and edges */
    if (scanf("%d %d", &n, &m) != 2) {
        fprintf(stderr, "Error: Failed to read graph dimensions\n");
        return 1;
    }
    
    if (n <= 0 || n > MAX_VERTICES) {
        fprintf(stderr, "Error: Invalid number of vertices\n");
        return 1;
    }
    
    /* Initialise graph */
    graph_init(&g, n);
    
    /* Read edges */
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
    
    /* Read number of queries */
    int q;
    if (scanf("%d", &q) != 1) {
        fprintf(stderr, "Error: Failed to read number of queries\n");
        graph_destroy(&g);
        return 1;
    }
    
    /* Process queries */
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
        
        /* Only run BFS if source changed (optimisation) */
        if (s != last_source) {
            bfs(&g, s, dist, parent);
            last_source = s;
        }
        
        print_path(s, t, dist, parent);
    }
    
    /* Clean up */
    graph_destroy(&g);
    
    return 0;
}
