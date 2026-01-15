/**
 * =============================================================================
 * EXERCISE 2: Unified Graph Analyser - SOLUTION
 * =============================================================================
 *
 * This file contains the complete solution for Exercise 2.
 * All TODOs have been implemented with detailed explanations.
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise2_sol exercise2_sol.c
 * USAGE: ./exercise2_sol [graph_file]
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

/* =============================================================================
 * CONFIGURATION
 * =============================================================================
 */

#define MAX_VERTICES    100
#define MAX_LINE_LENGTH 256
#define INF             INT_MAX
#define OUTPUT_FILE     "graph_analysis.txt"

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * SOLUTION TODO 1: Graph structure definition
 *
 * Stores graph as an adjacency matrix with metadata.
 * Weight of 0 indicates no edge between vertices.
 */
typedef struct {
    int vertices;       /* Number of vertices in graph */
    int edges;          /* Number of edges */
    int **adj_matrix;   /* V x V adjacency matrix (weights) */
    bool *visited;      /* Visited flags for traversals */
} Graph;

/**
 * Queue node for BFS traversal.
 */
typedef struct QueueNode {
    int vertex;
    struct QueueNode *next;
} QueueNode;

/**
 * Queue structure for BFS.
 */
typedef struct {
    QueueNode *front;
    QueueNode *rear;
    int size;
} Queue;

/* =============================================================================
 * QUEUE OPERATIONS (PROVIDED)
 * =============================================================================
 */

static Queue *queue_create(void) {
    Queue *q = malloc(sizeof(Queue));
    if (q) {
        q->front = q->rear = NULL;
        q->size = 0;
    }
    return q;
}

static void queue_enqueue(Queue *q, int vertex) {
    QueueNode *node = malloc(sizeof(QueueNode));
    if (!node) return;
    node->vertex = vertex;
    node->next = NULL;
    if (q->rear) {
        q->rear->next = node;
    } else {
        q->front = node;
    }
    q->rear = node;
    q->size++;
}

static int queue_dequeue(Queue *q) {
    if (!q->front) return -1;
    QueueNode *temp = q->front;
    int vertex = temp->vertex;
    q->front = q->front->next;
    if (!q->front) q->rear = NULL;
    free(temp);
    q->size--;
    return vertex;
}

static bool queue_is_empty(Queue *q) {
    return q->size == 0;
}

static void queue_free(Queue *q) {
    while (!queue_is_empty(q)) {
        queue_dequeue(q);
    }
    free(q);
}

/* =============================================================================
 * GRAPH CREATION AND DESTRUCTION
 * =============================================================================
 */

/**
 * SOLUTION TODO 2: Graph creation
 *
 * Allocates all necessary memory for a graph with V vertices.
 * Initialises adjacency matrix to all zeros (no edges).
 *
 * Time Complexity: O(V²) for matrix initialisation
 * Space Complexity: O(V²) for adjacency matrix
 */
Graph *graph_create(int vertices) {
    /* Validate input */
    if (vertices <= 0 || vertices > MAX_VERTICES) {
        fprintf(stderr, "Invalid vertex count: %d\n", vertices);
        return NULL;
    }
    
    /* Allocate Graph structure */
    Graph *g = malloc(sizeof(Graph));
    if (!g) {
        fprintf(stderr, "Failed to allocate Graph structure\n");
        return NULL;
    }
    
    /* Initialise basic fields */
    g->vertices = vertices;
    g->edges = 0;
    
    /* Allocate adjacency matrix (array of row pointers) */
    g->adj_matrix = malloc(vertices * sizeof(int *));
    if (!g->adj_matrix) {
        fprintf(stderr, "Failed to allocate adjacency matrix\n");
        free(g);
        return NULL;
    }
    
    /* Allocate each row and initialise to zero */
    for (int i = 0; i < vertices; i++) {
        g->adj_matrix[i] = calloc(vertices, sizeof(int));
        if (!g->adj_matrix[i]) {
            fprintf(stderr, "Failed to allocate row %d\n", i);
            /* Free already allocated rows */
            for (int j = 0; j < i; j++) {
                free(g->adj_matrix[j]);
            }
            free(g->adj_matrix);
            free(g);
            return NULL;
        }
    }
    
    /* Allocate visited array */
    g->visited = calloc(vertices, sizeof(bool));
    if (!g->visited) {
        fprintf(stderr, "Failed to allocate visited array\n");
        for (int i = 0; i < vertices; i++) {
            free(g->adj_matrix[i]);
        }
        free(g->adj_matrix);
        free(g);
        return NULL;
    }
    
    return g;
}

/**
 * SOLUTION TODO 3: Graph destruction
 *
 * Properly frees all allocated memory in reverse order of allocation.
 * Safe to call with NULL pointer.
 */
void graph_free(Graph *g) {
    if (!g) return;
    
    /* Free each row of adjacency matrix */
    if (g->adj_matrix) {
        for (int i = 0; i < g->vertices; i++) {
            free(g->adj_matrix[i]);
        }
        free(g->adj_matrix);
    }
    
    /* Free visited array */
    free(g->visited);
    
    /* Free graph structure */
    free(g);
}

/**
 * SOLUTION TODO 4: Add weighted edge
 *
 * Adds an undirected edge between src and dest with given weight.
 * For undirected graphs, adds edge in both directions.
 */
void graph_add_edge(Graph *g, int src, int dest, int weight) {
    /* Validate parameters */
    if (!g) return;
    if (src < 0 || src >= g->vertices) {
        fprintf(stderr, "Invalid source vertex: %d\n", src);
        return;
    }
    if (dest < 0 || dest >= g->vertices) {
        fprintf(stderr, "Invalid destination vertex: %d\n", dest);
        return;
    }
    
    /* Add edge (undirected - add both directions) */
    g->adj_matrix[src][dest] = weight;
    g->adj_matrix[dest][src] = weight;
    
    /* Increment edge count (once for undirected edge) */
    g->edges++;
}

/* =============================================================================
 * FILE I/O
 * =============================================================================
 */

/**
 * SOLUTION TODO 5: Load graph from file
 *
 * Reads graph from standard format:
 *   Line 1: V E
 *   Lines 2+: src dest weight
 */
Graph *graph_load(const char *filename) {
    /* Open file */
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Cannot open file: %s\n", filename);
        return NULL;
    }
    
    /* Read header: V and E */
    int V, E;
    if (fscanf(fp, "%d %d", &V, &E) != 2) {
        fprintf(stderr, "Invalid file format: expected V E on first line\n");
        fclose(fp);
        return NULL;
    }
    
    /* Validate V and E */
    if (V <= 0 || E < 0) {
        fprintf(stderr, "Invalid V=%d or E=%d\n", V, E);
        fclose(fp);
        return NULL;
    }
    
    /* Create graph */
    Graph *g = graph_create(V);
    if (!g) {
        fclose(fp);
        return NULL;
    }
    
    /* Read edges */
    for (int i = 0; i < E; i++) {
        int src, dest, weight;
        if (fscanf(fp, "%d %d %d", &src, &dest, &weight) != 3) {
            fprintf(stderr, "Error reading edge %d\n", i + 1);
            graph_free(g);
            fclose(fp);
            return NULL;
        }
        graph_add_edge(g, src, dest, weight);
    }
    
    fclose(fp);
    return g;
}

/* =============================================================================
 * GRAPH TRAVERSALS
 * =============================================================================
 */

/**
 * SOLUTION TODO 6: Reset visited flags
 */
void graph_reset_visited(Graph *g) {
    if (!g || !g->visited) return;
    
    for (int i = 0; i < g->vertices; i++) {
        g->visited[i] = false;
    }
}

/**
 * SOLUTION TODO 7: Breadth-First Search
 *
 * BFS explores all vertices at distance k before visiting vertices at k+1.
 * Uses a queue to maintain the frontier of exploration.
 *
 * Time Complexity: O(V + E)
 * Space Complexity: O(V) for queue and visited array
 */
void graph_bfs(Graph *g, int start, int *order, int *count) {
    *count = 0;
    
    if (!g || start < 0 || start >= g->vertices) return;
    
    /* Reset all visited flags */
    graph_reset_visited(g);
    
    /* Create queue for BFS */
    Queue *q = queue_create();
    if (!q) return;
    
    /* Start BFS from starting vertex */
    queue_enqueue(q, start);
    g->visited[start] = true;
    
    while (!queue_is_empty(q)) {
        /* Dequeue front vertex */
        int v = queue_dequeue(q);
        
        /* Add to traversal order */
        order[(*count)++] = v;
        
        /* Visit all unvisited neighbours */
        for (int i = 0; i < g->vertices; i++) {
            /* Check if edge exists and neighbour not visited */
            if (g->adj_matrix[v][i] != 0 && !g->visited[i]) {
                g->visited[i] = true;
                queue_enqueue(q, i);
            }
        }
    }
    
    queue_free(q);
}

/**
 * SOLUTION TODO 8: DFS recursive helper
 *
 * Recursively explores as deep as possible before backtracking.
 */
static void dfs_visit(Graph *g, int v, int *order, int *idx) {
    /* Mark current vertex as visited */
    g->visited[v] = true;
    
    /* Add to traversal order */
    order[(*idx)++] = v;
    
    /* Recursively visit all unvisited neighbours */
    for (int i = 0; i < g->vertices; i++) {
        if (g->adj_matrix[v][i] != 0 && !g->visited[i]) {
            dfs_visit(g, i, order, idx);
        }
    }
}

/**
 * SOLUTION TODO 9: Depth-First Search wrapper
 *
 * Time Complexity: O(V + E)
 * Space Complexity: O(V) for recursion stack and visited array
 */
void graph_dfs(Graph *g, int start, int *order, int *count) {
    *count = 0;
    
    if (!g || start < 0 || start >= g->vertices) return;
    
    /* Reset visited flags */
    graph_reset_visited(g);
    
    /* Start DFS from starting vertex */
    dfs_visit(g, start, order, count);
}

/* =============================================================================
 * SHORTEST PATH ALGORITHMS
 * =============================================================================
 */

/**
 * SOLUTION TODO 10: Find minimum distance vertex
 *
 * Helper function for Dijkstra's algorithm.
 * Finds the unprocessed vertex with smallest distance.
 */
static int find_min_distance(const int *dist, const bool *processed, int V) {
    int min_dist = INF;
    int min_idx = -1;
    
    for (int v = 0; v < V; v++) {
        if (!processed[v] && dist[v] < min_dist) {
            min_dist = dist[v];
            min_idx = v;
        }
    }
    
    return min_idx;
}

/**
 * SOLUTION TODO 11: Dijkstra's algorithm
 *
 * Finds shortest paths from source to all other vertices.
 * Uses greedy approach: always process vertex with minimum distance.
 *
 * Time Complexity: O(V²) with adjacency matrix
 * Space Complexity: O(V) for distance and processed arrays
 *
 * Note: This implementation uses simple array-based min finding.
 * For better performance, use a priority queue (O(E log V)).
 */
void dijkstra(Graph *g, int src, int *dist, int *parent) {
    if (!g || src < 0 || src >= g->vertices || !dist) return;
    
    int V = g->vertices;
    
    /* Allocate processed array */
    bool *processed = calloc(V, sizeof(bool));
    if (!processed) {
        fprintf(stderr, "Memory allocation failed in dijkstra()\n");
        return;
    }
    
    /* Initialise distances to infinity, source to 0 */
    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        if (parent) parent[i] = -1;
    }
    dist[src] = 0;
    
    /* Process V-1 vertices (source already processed conceptually) */
    for (int count = 0; count < V - 1; count++) {
        /* Find unprocessed vertex with minimum distance */
        int u = find_min_distance(dist, processed, V);
        
        /* If no reachable unprocessed vertex, stop */
        if (u == -1 || dist[u] == INF) break;
        
        /* Mark as processed */
        processed[u] = true;
        
        /* Update distances to all neighbours */
        for (int v = 0; v < V; v++) {
            /* Check if:
             * 1. Edge exists (weight != 0)
             * 2. Vertex not processed
             * 3. Distance through u is shorter
             */
            int weight = g->adj_matrix[u][v];
            if (weight != 0 && 
                !processed[v] && 
                dist[u] != INF &&
                dist[u] + weight < dist[v]) {
                
                dist[v] = dist[u] + weight;
                if (parent) parent[v] = u;
            }
        }
    }
    
    free(processed);
}

/**
 * SOLUTION TODO 12: Reconstruct shortest path
 *
 * Traces back from destination to source using parent pointers.
 * Reverses the path to get correct order (source to destination).
 */
void reconstruct_path(const int *parent, int src, int dest, 
                      int *path, int *length) {
    *length = 0;
    
    if (!parent || !path) return;
    if (parent[dest] == -1 && dest != src) {
        /* No path exists */
        return;
    }
    
    /* Trace path backwards from dest to src */
    int temp_path[MAX_VERTICES];
    int temp_len = 0;
    int current = dest;
    
    while (current != -1) {
        temp_path[temp_len++] = current;
        if (current == src) break;
        current = parent[current];
    }
    
    /* Reverse to get path from src to dest */
    for (int i = 0; i < temp_len; i++) {
        path[i] = temp_path[temp_len - 1 - i];
    }
    *length = temp_len;
}

/* =============================================================================
 * GRAPH ANALYSIS
 * =============================================================================
 */

/**
 * SOLUTION TODO 13: Count connected components
 *
 * Uses DFS to identify all connected components in the graph.
 * Each DFS from an unvisited vertex explores one complete component.
 */
int count_components(Graph *g) {
    if (!g) return 0;
    
    /* Reset visited flags */
    graph_reset_visited(g);
    
    int count = 0;
    int order[MAX_VERTICES];  /* Dummy array for DFS */
    
    /* Try starting DFS from each vertex */
    for (int v = 0; v < g->vertices; v++) {
        if (!g->visited[v]) {
            /* Found new component - explore it with DFS */
            int dummy;
            dfs_visit(g, v, order, &dummy);
            count++;
        }
    }
    
    return count;
}

/**
 * SOLUTION TODO 14: Calculate graph density
 *
 * For undirected graph: density = 2E / (V × (V-1))
 * Density ranges from 0 (no edges) to 1 (complete graph)
 */
double graph_density(Graph *g) {
    if (!g || g->vertices < 2) return 0.0;
    
    int V = g->vertices;
    int E = g->edges;
    
    /* Maximum possible edges in undirected graph: V(V-1)/2 */
    double max_edges = (double)V * (V - 1) / 2.0;
    
    return (double)E / max_edges;
}

/* =============================================================================
 * REPORT GENERATION
 * =============================================================================
 */

/**
 * SOLUTION TODO 15: Export analysis report
 *
 * Generates a comprehensive text report with all graph analysis results.
 */
int export_analysis(Graph *g, const char *filename) {
    if (!g || !filename) return -1;
    
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Cannot create file: %s\n", filename);
        return -1;
    }
    
    /* Header */
    fprintf(fp, "╔═══════════════════════════════════════════════════════════════╗\n");
    fprintf(fp, "║              Graph Analysis Report                            ║\n");
    fprintf(fp, "╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Statistics */
    fprintf(fp, "Statistics\n");
    fprintf(fp, "══════════\n");
    fprintf(fp, "  Vertices:   %d\n", g->vertices);
    fprintf(fp, "  Edges:      %d\n", g->edges);
    fprintf(fp, "  Density:    %.3f\n", graph_density(g));
    fprintf(fp, "  Components: %d\n\n", count_components(g));
    
    /* BFS Traversal */
    int order[MAX_VERTICES];
    int count;
    
    graph_bfs(g, 0, order, &count);
    fprintf(fp, "BFS Traversal (from vertex 0)\n");
    fprintf(fp, "═════════════════════════════\n");
    fprintf(fp, "  ");
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d", order[i]);
        if (i < count - 1) fprintf(fp, " → ");
    }
    fprintf(fp, "\n\n");
    
    /* DFS Traversal */
    graph_dfs(g, 0, order, &count);
    fprintf(fp, "DFS Traversal (from vertex 0)\n");
    fprintf(fp, "═════════════════════════════\n");
    fprintf(fp, "  ");
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d", order[i]);
        if (i < count - 1) fprintf(fp, " → ");
    }
    fprintf(fp, "\n\n");
    
    /* Dijkstra's shortest paths */
    int *dist = malloc(g->vertices * sizeof(int));
    int *parent = malloc(g->vertices * sizeof(int));
    
    if (dist && parent) {
        dijkstra(g, 0, dist, parent);
        
        fprintf(fp, "Shortest Paths from vertex 0 (Dijkstra)\n");
        fprintf(fp, "═══════════════════════════════════════\n");
        
        for (int v = 0; v < g->vertices; v++) {
            fprintf(fp, "  To %d: ", v);
            if (dist[v] == INF) {
                fprintf(fp, "unreachable\n");
            } else {
                fprintf(fp, "%d", dist[v]);
                
                /* Show path */
                int path[MAX_VERTICES];
                int path_len;
                reconstruct_path(parent, 0, v, path, &path_len);
                
                if (path_len > 1) {
                    fprintf(fp, " (path: ");
                    for (int i = 0; i < path_len; i++) {
                        fprintf(fp, "%d", path[i]);
                        if (i < path_len - 1) fprintf(fp, "→");
                    }
                    fprintf(fp, ")");
                }
                fprintf(fp, "\n");
            }
        }
        
        free(dist);
        free(parent);
    }
    
    /* Footer */
    fprintf(fp, "\n═══════════════════════════════════════════════════════════════\n");
    fprintf(fp, "Report generated successfully.\n");
    
    fclose(fp);
    return 0;
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  Unified Graph Analyser                                       ║\n");
    printf("║  Exercise 2 - Week 14 (SOLUTION)                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Determine input file */
    const char *input_file = (argc > 1) ? argv[1] : "data/graph_sample.txt";
    
    printf("Loading graph from: %s\n\n", input_file);
    
    /* Load graph from file */
    Graph *g = graph_load(input_file);
    if (!g) {
        fprintf(stderr, "Error: Could not load graph from %s\n", input_file);
        fprintf(stderr, "Make sure the file exists and has correct format.\n");
        return 1;
    }
    
    /* Display graph info */
    printf("Graph loaded successfully!\n");
    printf("  Vertices: %d\n", g->vertices);
    printf("  Edges:    %d\n", g->edges);
    printf("  Density:  %.3f\n\n", graph_density(g));
    
    /* Arrays for traversal results */
    int order[MAX_VERTICES];
    int count;
    
    /* BFS Traversal */
    printf("BFS Traversal:\n");
    graph_bfs(g, 0, order, &count);
    printf("  From vertex 0: ");
    for (int i = 0; i < count; i++) {
        printf("%d ", order[i]);
    }
    printf("\n");
    
    /* DFS Traversal */
    printf("\nDFS Traversal:\n");
    graph_dfs(g, 0, order, &count);
    printf("  From vertex 0: ");
    for (int i = 0; i < count; i++) {
        printf("%d ", order[i]);
    }
    printf("\n");
    
    /* Dijkstra's algorithm */
    printf("\nShortest Paths (Dijkstra from vertex 0):\n");
    int *dist = malloc(g->vertices * sizeof(int));
    int *parent = malloc(g->vertices * sizeof(int));
    
    if (dist && parent) {
        dijkstra(g, 0, dist, parent);
        
        for (int v = 0; v < g->vertices; v++) {
            printf("  To %d: ", v);
            if (dist[v] == INF) {
                printf("unreachable\n");
            } else {
                printf("%d\n", dist[v]);
            }
        }
        
        free(dist);
        free(parent);
    }
    
    /* Connected components */
    printf("\nConnected Components: %d\n", count_components(g));
    
    /* Export analysis report */
    printf("\nExporting analysis report...\n");
    if (export_analysis(g, OUTPUT_FILE) == 0) {
        printf("Report saved to: %s\n", OUTPUT_FILE);
    }
    
    /* Cleanup */
    graph_free(g);
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  Analysis complete                                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

/* =============================================================================
 * COMPLEXITY SUMMARY
 * =============================================================================
 *
 * Operation           | Time        | Space
 * --------------------|-------------|--------
 * graph_create        | O(V²)       | O(V²)
 * graph_add_edge      | O(1)        | O(1)
 * graph_load          | O(V² + E)   | O(V²)
 * graph_bfs           | O(V²)       | O(V)
 * graph_dfs           | O(V²)       | O(V)
 * dijkstra            | O(V²)       | O(V)
 * count_components    | O(V²)       | O(V)
 *
 * Note: With adjacency list, BFS/DFS would be O(V + E)
 *       With priority queue, Dijkstra would be O((V + E) log V)
 *
 * =============================================================================
 */
