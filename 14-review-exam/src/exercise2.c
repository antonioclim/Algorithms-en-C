/**
 * =============================================================================
 * EXERCISE 2: Unified Graph Analyser
 * =============================================================================
 *
 * OBJECTIVE:
 *   Build a comprehensive graph analysis tool that can load graphs from files,
 *   perform various traversals and algorithms, and export analysis reports.
 *   This exercise synthesises graph theory, file I/O and data structures.
 *
 * REQUIREMENTS:
 *   1. Load graph from adjacency list file format
 *   2. Implement BFS and DFS traversals
 *   3. Compute shortest paths using Dijkstra's algorithm
 *   4. Detect connected components
 *   5. Export analysis report to file
 *
 * INPUT FILE FORMAT (graph_sample.txt):
 *   6 8
 *   0 1 7
 *   0 3 10
 *   0 4 2
 *   1 2 9
 *   1 4 14
 *   2 5 15
 *   3 4 6
 *   4 5 11
 *
 *   First line: V E (vertices and edges)
 *   Following lines: src dest weight
 *
 * EXPECTED OUTPUT:
 *   Graph Analyser
 *   ==============
 *   Loaded graph: 6 vertices, 8 edges
 *   
 *   BFS from vertex 0: 0 1 3 4 2 5
 *   DFS from vertex 0: 0 1 2 5 4 3
 *   
 *   Dijkstra from vertex 0:
 *     To 0: 0
 *     To 1: 7
 *     To 2: 16
 *     ...
 *   
 *   Analysis exported to graph_analysis.txt
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
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
 * TODO 1: Define the Graph structure
 *
 * The structure should contain:
 *   - int vertices:     Number of vertices
 *   - int edges:        Number of edges
 *   - int **adj_matrix: 2D adjacency matrix (weights)
 *   - bool *visited:    Visited flags for traversals
 *
 * Hint: Use pointers for dynamic allocation
 */
typedef struct {
    /* YOUR CODE HERE */
    int placeholder;  /* Remove this when implementing */
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
 * TODO 2: Implement graph creation
 *
 * Allocate and initialise a graph with V vertices.
 *
 * @param vertices Number of vertices
 * @return         Pointer to new graph, or NULL on failure
 *
 * Steps:
 *   1. Allocate Graph structure
 *   2. Set vertices count, edges = 0
 *   3. Allocate adjacency matrix (V x V)
 *   4. Initialise all weights to 0
 *   5. Allocate visited array
 *   6. Return pointer
 *
 * Memory allocation pattern:
 *   g->adj_matrix = malloc(V * sizeof(int *));
 *   for each row: g->adj_matrix[i] = calloc(V, sizeof(int));
 */
Graph *graph_create(int vertices) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace this */
}

/**
 * TODO 3: Implement graph destruction
 *
 * Free all memory associated with the graph.
 *
 * @param g Graph to free
 *
 * Steps:
 *   1. Check if g is NULL
 *   2. Free each row of adjacency matrix
 *   3. Free adjacency matrix pointer
 *   4. Free visited array
 *   5. Free graph structure
 */
void graph_free(Graph *g) {
    /* YOUR CODE HERE */
}

/**
 * TODO 4: Add weighted edge to graph
 *
 * For undirected graph, add edge in both directions.
 *
 * @param g      Graph structure
 * @param src    Source vertex
 * @param dest   Destination vertex
 * @param weight Edge weight
 *
 * Validation:
 *   - Check g is not NULL
 *   - Check src and dest are valid (0 to vertices-1)
 *   - Add edge: g->adj_matrix[src][dest] = weight
 *   - For undirected: g->adj_matrix[dest][src] = weight
 *   - Increment edge count (only once for undirected)
 */
void graph_add_edge(Graph *g, int src, int dest, int weight) {
    /* YOUR CODE HERE */
}

/* =============================================================================
 * FILE I/O
 * =============================================================================
 */

/**
 * TODO 5: Load graph from file
 *
 * Read graph from file in format:
 *   Line 1: V E (vertices, edges)
 *   Lines 2-E+1: src dest weight
 *
 * @param filename Path to input file
 * @return         Loaded graph, or NULL on error
 *
 * Steps:
 *   1. Open file for reading
 *   2. Read V and E from first line
 *   3. Create graph with V vertices
 *   4. For each of E lines:
 *      a. Read src, dest, weight
 *      b. Add edge to graph
 *   5. Close file
 *   6. Return graph
 *
 * Error handling:
 *   - Check file opens successfully
 *   - Validate V and E are positive
 *   - Handle fscanf failures
 */
Graph *graph_load(const char *filename) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace this */
}

/* =============================================================================
 * GRAPH TRAVERSALS
 * =============================================================================
 */

/**
 * TODO 6: Reset visited flags
 *
 * Set all visited flags to false before traversal.
 *
 * @param g Graph structure
 */
void graph_reset_visited(Graph *g) {
    /* YOUR CODE HERE */
}

/**
 * TODO 7: Implement Breadth-First Search
 *
 * Perform BFS from starting vertex, storing traversal order.
 *
 * @param g      Graph structure
 * @param start  Starting vertex
 * @param order  Array to store traversal order
 * @param count  Pointer to store number of visited vertices
 *
 * Algorithm:
 *   1. Reset visited flags
 *   2. Create queue and enqueue start
 *   3. Mark start as visited
 *   4. While queue not empty:
 *      a. Dequeue vertex v
 *      b. Add v to order array
 *      c. For each unvisited neighbour:
 *         - Mark as visited
 *         - Enqueue neighbour
 *   5. Free queue
 */
void graph_bfs(Graph *g, int start, int *order, int *count) {
    /* YOUR CODE HERE */
    
    *count = 0;  /* Replace with actual implementation */
}

/**
 * TODO 8: Implement DFS helper (recursive)
 *
 * Recursive DFS from vertex v.
 *
 * @param g     Graph structure
 * @param v     Current vertex
 * @param order Array to store traversal order
 * @param idx   Pointer to current index in order array
 */
static void dfs_visit(Graph *g, int v, int *order, int *idx) {
    /* YOUR CODE HERE */
    
    /* Hint:
     *   1. Mark v as visited
     *   2. Add v to order[(*idx)++]
     *   3. For each neighbour i:
     *      if edge exists and not visited:
     *          dfs_visit(g, i, order, idx)
     */
}

/**
 * TODO 9: Implement Depth-First Search wrapper
 *
 * @param g      Graph structure
 * @param start  Starting vertex
 * @param order  Array to store traversal order
 * @param count  Pointer to store number of visited vertices
 */
void graph_dfs(Graph *g, int start, int *order, int *count) {
    /* YOUR CODE HERE */
    
    *count = 0;  /* Replace with actual implementation */
}

/* =============================================================================
 * SHORTEST PATH ALGORITHMS
 * =============================================================================
 */

/**
 * TODO 10: Find vertex with minimum distance (helper for Dijkstra)
 *
 * @param dist      Array of distances
 * @param processed Array of processed flags
 * @param V         Number of vertices
 * @return          Index of minimum distance vertex, or -1 if all processed
 */
static int find_min_distance(const int *dist, const bool *processed, int V) {
    /* YOUR CODE HERE */
    
    return -1;  /* Replace this */
}

/**
 * TODO 11: Implement Dijkstra's algorithm
 *
 * Find shortest paths from source to all vertices.
 *
 * @param g      Graph structure
 * @param src    Source vertex
 * @param dist   Array to store distances (caller allocates)
 * @param parent Array to store parent pointers (caller allocates, can be NULL)
 *
 * Algorithm:
 *   1. Initialise all distances to INF, source to 0
 *   2. Create processed array (all false)
 *   3. For V-1 iterations:
 *      a. Find unprocessed vertex u with min distance
 *      b. Mark u as processed
 *      c. For each neighbour v of u:
 *         if not processed and dist[u] + weight < dist[v]:
 *            dist[v] = dist[u] + weight
 *            parent[v] = u (if parent not NULL)
 *   4. Free processed array
 */
void dijkstra(Graph *g, int src, int *dist, int *parent) {
    /* YOUR CODE HERE */
}

/**
 * TODO 12: Reconstruct shortest path
 *
 * Build path from source to destination using parent array.
 *
 * @param parent Array of parent pointers
 * @param src    Source vertex
 * @param dest   Destination vertex
 * @param path   Array to store path (caller allocates)
 * @param length Pointer to store path length
 */
void reconstruct_path(const int *parent, int src, int dest, 
                      int *path, int *length) {
    /* YOUR CODE HERE */
    
    /* Hint:
     *   1. Start at dest, follow parent pointers to src
     *   2. Store vertices in reverse order
     *   3. Reverse the path array
     */
    
    *length = 0;  /* Replace this */
}

/* =============================================================================
 * GRAPH ANALYSIS
 * =============================================================================
 */

/**
 * TODO 13: Count connected components
 *
 * Use DFS to count number of connected components.
 *
 * @param g Graph structure
 * @return  Number of connected components
 *
 * Algorithm:
 *   1. Reset visited flags
 *   2. count = 0
 *   3. For each vertex v:
 *      if not visited:
 *          DFS from v (marks all reachable as visited)
 *          count++
 *   4. Return count
 */
int count_components(Graph *g) {
    /* YOUR CODE HERE */
    
    return 0;  /* Replace this */
}

/**
 * TODO 14: Calculate graph density
 *
 * Density = 2E / (V * (V-1)) for undirected graph
 *
 * @param g Graph structure
 * @return  Graph density (0.0 to 1.0)
 */
double graph_density(Graph *g) {
    /* YOUR CODE HERE */
    
    return 0.0;  /* Replace this */
}

/* =============================================================================
 * REPORT GENERATION
 * =============================================================================
 */

/**
 * TODO 15: Export analysis report to file
 *
 * Generate comprehensive analysis report including:
 *   - Graph statistics (vertices, edges, density)
 *   - BFS and DFS traversals from vertex 0
 *   - Shortest paths from vertex 0
 *   - Connected component count
 *
 * @param g        Graph structure
 * @param filename Output filename
 * @return         0 on success, -1 on failure
 */
int export_analysis(Graph *g, const char *filename) {
    /* YOUR CODE HERE */
    
    /* File content structure:
     *
     *   Graph Analysis Report
     *   =====================
     *   
     *   Statistics:
     *     Vertices: 6
     *     Edges: 8
     *     Density: 0.533
     *     Components: 1
     *   
     *   BFS Traversal (from 0): 0 1 3 4 2 5
     *   DFS Traversal (from 0): 0 1 2 5 4 3
     *   
     *   Shortest Paths from vertex 0:
     *     To 0: 0
     *     To 1: 7
     *     ...
     */
    
    return -1;  /* Replace with 0 on success */
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  Unified Graph Analyser                                       ║\n");
    printf("║  Exercise 2 - Week 14                                         ║\n");
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
    
    /* Display graph info (requires TODO 1-5 implementation) */
    /* printf("Loaded: %d vertices, %d edges\n", g->vertices, g->edges); */
    printf("(Graph loading implementation pending - complete TODOs 1-5)\n\n");
    
    /* Arrays for traversal results */
    int order[MAX_VERTICES];
    int count;
    
    /* BFS Traversal */
    printf("BFS Traversal:\n");
    graph_bfs(g, 0, order, &count);
    if (count > 0) {
        printf("  From vertex 0: ");
        for (int i = 0; i < count; i++) {
            printf("%d ", order[i]);
        }
        printf("\n");
    } else {
        printf("  (Implementation pending - complete TODO 7)\n");
    }
    
    /* DFS Traversal */
    printf("\nDFS Traversal:\n");
    graph_dfs(g, 0, order, &count);
    if (count > 0) {
        printf("  From vertex 0: ");
        for (int i = 0; i < count; i++) {
            printf("%d ", order[i]);
        }
        printf("\n");
    } else {
        printf("  (Implementation pending - complete TODOs 8-9)\n");
    }
    
    /* Dijkstra's Algorithm */
    printf("\nShortest Paths (Dijkstra from vertex 0):\n");
    /* TODO: Uncomment when graph structure is implemented
    int *dist = malloc(g->vertices * sizeof(int));
    int *parent = malloc(g->vertices * sizeof(int));
    if (dist && parent) {
        dijkstra(g, 0, dist, parent);
        for (int i = 0; i < g->vertices; i++) {
            if (dist[i] == INF) {
                printf("  To %d: unreachable\n", i);
            } else {
                printf("  To %d: %d\n", i, dist[i]);
            }
        }
        free(dist);
        free(parent);
    }
    */
    printf("  (Implementation pending - complete TODOs 10-12)\n");
    
    /* Graph Analysis */
    printf("\nGraph Analysis:\n");
    printf("  Components: %d\n", count_components(g));
    printf("  Density: %.3f\n", graph_density(g));
    
    /* Export Report */
    printf("\nExporting analysis report...\n");
    if (export_analysis(g, OUTPUT_FILE) == 0) {
        printf("  Report saved to %s\n", OUTPUT_FILE);
    } else {
        printf("  (Export pending - complete TODO 15)\n");
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
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Add Bellman-Ford algorithm for graphs with negative weights
 * 2. Implement Floyd-Warshall for all-pairs shortest paths
 * 3. Detect cycles using DFS (back edges)
 * 4. Implement topological sort for DAGs
 * 5. Add support for directed graphs (separate adjacency matrix values)
 * 6. Implement minimum spanning tree (Prim's or Kruskal's)
 *
 * =============================================================================
 */
