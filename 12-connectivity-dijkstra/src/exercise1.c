/**
 * =============================================================================
 * EXERCISE 1: Breadth-First Search and Shortest Paths
 * =============================================================================
 *
 * OBJECTIVE:
 *   Implement BFS traversal and use it to find shortest paths in an unweighted
 *   graph. You will build a graph from file input and answer shortest path
 *   queries between vertex pairs.
 *
 * REQUIREMENTS:
 *   1. Read a graph from standard input (adjacency list format)
 *   2. Implement BFS traversal from a given source vertex
 *   3. Compute shortest path distances to all reachable vertices
 *   4. Reconstruct and print the actual shortest path between two vertices
 *   5. Handle disconnected components correctly
 *
 * INPUT FORMAT:
 *   Line 1: n m (number of vertices and edges)
 *   Lines 2 to m+1: u v (edge from u to v, undirected)
 *   Line m+2: q (number of queries)
 *   Next q lines: s t (find shortest path from s to t)
 *
 * EXAMPLE INPUT:
 *   6 7
 *   0 1
 *   0 2
 *   1 2
 *   1 3
 *   2 4
 *   3 4
 *   3 5
 *   3
 *   0 5
 *   0 4
 *   1 5
 *
 * EXPECTED OUTPUT:
 *   Path from 0 to 5: 0 -> 1 -> 3 -> 5 (distance: 3)
 *   Path from 0 to 4: 0 -> 2 -> 4 (distance: 2)
 *   Path from 1 to 5: 1 -> 3 -> 5 (distance: 2)
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
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
 * TODO 1: Define the adjacency list node structure
 *
 * Each node in the adjacency list should store:
 *   - The destination vertex (int)
 *   - A pointer to the next adjacent vertex (struct AdjNode*)
 *
 * Hint: Use 'struct AdjNode' as the type name for the self-referential pointer.
 */
typedef struct AdjNode {
    /* YOUR CODE HERE */
    int vertex;          /* Remove this line and implement properly */
    struct AdjNode *next;
} AdjNode;

/**
 * TODO 2: Define the graph structure
 *
 * The graph should contain:
 *   - Number of vertices (int num_vertices)
 *   - Number of edges (int num_edges)
 *   - An array of adjacency list heads (AdjNode* adj[MAX_VERTICES])
 *
 * Hint: The adjacency list array should be of size MAX_VERTICES.
 */
typedef struct {
    /* YOUR CODE HERE */
    int num_vertices;
    int num_edges;
    AdjNode *adj[MAX_VERTICES];
} Graph;

/**
 * Queue structure for BFS (provided)
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
 * TODO 3: Initialise the graph
 *
 * @param g Pointer to graph structure
 * @param n Number of vertices
 *
 * Steps:
 *   1. Set num_vertices to n
 *   2. Set num_edges to 0
 *   3. Initialise all adjacency list heads to NULL
 */
void graph_init(Graph *g, int n) {
    /* YOUR CODE HERE */
}

/**
 * TODO 4: Create a new adjacency list node
 *
 * @param v The destination vertex
 * @return Pointer to newly allocated AdjNode, or NULL on failure
 *
 * Steps:
 *   1. Allocate memory for a new AdjNode
 *   2. Check if allocation succeeded (return NULL if not)
 *   3. Set the vertex field to v
 *   4. Set the next pointer to NULL
 *   5. Return the new node
 */
AdjNode *create_node(int v) {
    /* YOUR CODE HERE */
    return NULL;  /* Replace this */
}

/**
 * TODO 5: Add an undirected edge to the graph
 *
 * @param g Pointer to graph
 * @param u First vertex
 * @param v Second vertex
 *
 * For an undirected graph, add edges in both directions:
 *   1. Add v to u's adjacency list (insert at head)
 *   2. Add u to v's adjacency list (insert at head)
 *   3. Increment num_edges by 1
 *
 * Hint: Inserting at head means: new_node->next = g->adj[u]; g->adj[u] = new_node;
 */
void graph_add_edge(Graph *g, int u, int v) {
    /* YOUR CODE HERE */
}

/**
 * TODO 6: Perform BFS and compute distances from source
 *
 * @param g Pointer to graph
 * @param source Starting vertex for BFS
 * @param dist Array to store distances (size: num_vertices)
 * @param parent Array to store parent pointers for path reconstruction
 *
 * Algorithm:
 *   1. Initialise all distances to INFINITY_DIST
 *   2. Initialise all parents to -1
 *   3. Set dist[source] = 0
 *   4. Create and initialise a queue
 *   5. Enqueue the source vertex
 *   6. While queue is not empty:
 *      a. Dequeue vertex u
 *      b. For each neighbour v of u:
 *         - If dist[v] == INFINITY_DIST (unvisited):
 *           * Set dist[v] = dist[u] + 1
 *           * Set parent[v] = u
 *           * Enqueue v
 *
 * Hint: Use the provided Queue operations.
 */
void bfs(Graph *g, int source, int dist[], int parent[]) {
    /* YOUR CODE HERE */
}

/**
 * TODO 7: Print the shortest path from source to target
 *
 * @param source Starting vertex
 * @param target Ending vertex
 * @param dist Distance array (from BFS)
 * @param parent Parent array (from BFS)
 *
 * Steps:
 *   1. If dist[target] == INFINITY_DIST, print "No path exists"
 *   2. Otherwise, reconstruct path by following parent pointers
 *      - Start from target
 *      - Follow parent[current] until you reach source
 *      - Store vertices in an array (they will be in reverse order)
 *   3. Print the path in correct order with " -> " between vertices
 *   4. Print the distance
 *
 * Hint: You can use a stack or array to reverse the path order.
 */
void print_path(int source, int target, int dist[], int parent[]) {
    /* YOUR CODE HERE */
}

/**
 * TODO 8: Free all memory allocated for the graph
 *
 * @param g Pointer to graph
 *
 * Steps:
 *   1. For each vertex i from 0 to num_vertices-1:
 *      a. Traverse the adjacency list g->adj[i]
 *      b. Free each node in the list
 *      c. Set g->adj[i] to NULL
 *   2. Reset num_vertices and num_edges to 0
 *
 * Hint: Use a temporary pointer to avoid losing the next node before freeing.
 */
void graph_destroy(Graph *g) {
    /* YOUR CODE HERE */
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

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Modify the program to handle directed graphs (add a flag for edge direction)
 *
 * 2. Implement multi-source BFS: find the vertex closest to all sources
 *
 * 3. Count the number of shortest paths between two vertices (not just find one)
 *
 * 4. Add a function to detect if the graph is bipartite using BFS
 *
 * =============================================================================
 */
