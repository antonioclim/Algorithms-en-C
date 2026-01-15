/**
 * =============================================================================
 * EXERCISE 2: Depth-First Search, Cycle Detection and Topological Sort
 * =============================================================================
 *
 * OBJECTIVE:
 *   Implement DFS traversal on a directed graph, detect cycles, and perform
 *   topological sorting. This exercise builds understanding of DFS applications
 *   in dependency resolution and scheduling problems.
 *
 * REQUIREMENTS:
 *   1. Read a directed graph from standard input
 *   2. Implement recursive and iterative DFS traversal
 *   3. Detect cycles using the three-colour algorithm (WHITE/GREY/BLACK)
 *   4. If the graph is acyclic (DAG), output a valid topological ordering
 *   5. If cycles exist, report which vertices are involved
 *
 * INPUT FORMAT:
 *   Line 1: n m (number of vertices and edges)
 *   Lines 2 to m+1: u v (directed edge from u to v)
 *
 * EXAMPLE INPUT (Acyclic - DAG):
 *   6 6
 *   5 2
 *   5 0
 *   4 0
 *   4 1
 *   2 3
 *   3 1
 *
 * EXPECTED OUTPUT (Acyclic):
 *   DFS traversal: 0 1 2 3 4 5
 *   No cycle detected - graph is a DAG
 *   Topological order: 5 4 2 3 1 0
 *
 * EXAMPLE INPUT (Cyclic):
 *   3 3
 *   0 1
 *   1 2
 *   2 0
 *
 * EXPECTED OUTPUT (Cyclic):
 *   DFS traversal: 0 1 2
 *   Cycle detected! Vertices in cycle: 0 1 2
 *   Topological sort not possible (graph contains cycle)
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* =============================================================================
 * CONSTANTS AND COLOUR DEFINITIONS
 * =============================================================================
 */

#define MAX_VERTICES 1000

/**
 * TODO 1: Define the three colours for DFS cycle detection
 *
 * The three-colour algorithm uses:
 *   - WHITE (0): Vertex not yet discovered
 *   - GREY  (1): Vertex discovered but not finished (still exploring descendants)
 *   - BLACK (2): Vertex completely processed (all descendants explored)
 *
 * A cycle exists if we encounter a GREY vertex during DFS (back edge).
 */
typedef enum {
    /* YOUR CODE HERE */
    WHITE = 0,
    GREY = 1,
    BLACK = 2
} Colour;

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * TODO 2: Define the adjacency list node structure for a directed graph
 *
 * Each node should store:
 *   - The destination vertex (int vertex)
 *   - A pointer to the next adjacent vertex
 */
typedef struct AdjNode {
    /* YOUR CODE HERE */
    int vertex;
    struct AdjNode *next;
} AdjNode;

/**
 * TODO 3: Define the directed graph structure
 *
 * The graph should contain:
 *   - Number of vertices
 *   - Number of edges
 *   - Array of adjacency list heads
 */
typedef struct {
    /* YOUR CODE HERE */
    int num_vertices;
    int num_edges;
    AdjNode *adj[MAX_VERTICES];
} DiGraph;

/**
 * Stack structure for iterative DFS and topological sort (provided)
 */
typedef struct {
    int items[MAX_VERTICES];
    int top;
} Stack;

/* =============================================================================
 * STACK OPERATIONS (PROVIDED)
 * =============================================================================
 */

void stack_init(Stack *s) {
    s->top = -1;
}

bool stack_is_empty(Stack *s) {
    return s->top == -1;
}

void stack_push(Stack *s, int value) {
    s->items[++s->top] = value;
}

int stack_pop(Stack *s) {
    return s->items[s->top--];
}

int stack_peek(Stack *s) {
    return s->items[s->top];
}

/* =============================================================================
 * GRAPH OPERATIONS
 * =============================================================================
 */

/**
 * TODO 4: Initialise the directed graph
 *
 * @param g Pointer to graph structure
 * @param n Number of vertices
 *
 * Steps:
 *   1. Set num_vertices to n
 *   2. Set num_edges to 0
 *   3. Initialise all adjacency list heads to NULL
 */
void digraph_init(DiGraph *g, int n) {
    /* YOUR CODE HERE */
}

/**
 * TODO 5: Create a new adjacency list node
 *
 * @param v The destination vertex
 * @return Pointer to newly allocated node, or NULL on failure
 */
AdjNode *create_node(int v) {
    /* YOUR CODE HERE */
    return NULL;  /* Replace this */
}

/**
 * TODO 6: Add a directed edge from u to v
 *
 * @param g Pointer to directed graph
 * @param u Source vertex
 * @param v Destination vertex
 *
 * Unlike undirected graphs, only add ONE edge (from u to v).
 * Insert at the head of u's adjacency list.
 */
void digraph_add_edge(DiGraph *g, int u, int v) {
    /* YOUR CODE HERE */
}

/* =============================================================================
 * DFS TRAVERSAL
 * =============================================================================
 */

/**
 * TODO 7: Implement recursive DFS traversal (utility function)
 *
 * @param g Pointer to graph
 * @param v Current vertex
 * @param visited Array tracking visited vertices
 * @param traversal Array to store traversal order
 * @param count Pointer to current position in traversal array
 *
 * Algorithm:
 *   1. Mark v as visited
 *   2. Add v to the traversal array
 *   3. For each unvisited neighbour u of v:
 *      - Recursively call DFS on u
 */
void dfs_recursive_util(DiGraph *g, int v, bool visited[], int traversal[], int *count) {
    /* YOUR CODE HERE */
}

/**
 * TODO 8: Implement full DFS traversal (handles disconnected components)
 *
 * @param g Pointer to graph
 * @param traversal Array to store DFS traversal order
 * @return Number of vertices in traversal
 *
 * Steps:
 *   1. Initialise visited array to all false
 *   2. For each vertex v from 0 to num_vertices-1:
 *      - If v is not visited, call dfs_recursive_util on v
 *   3. Return the count of vertices visited
 */
int dfs_traversal(DiGraph *g, int traversal[]) {
    /* YOUR CODE HERE */
    return 0;  /* Replace this */
}

/* =============================================================================
 * CYCLE DETECTION
 * =============================================================================
 */

/**
 * TODO 9: Implement cycle detection using three-colour DFS (utility function)
 *
 * @param g Pointer to graph
 * @param v Current vertex
 * @param colour Array storing colour of each vertex
 * @param cycle_vertices Array to store vertices in cycle (if found)
 * @param cycle_count Pointer to number of vertices in cycle
 * @return true if cycle found, false otherwise
 *
 * Three-colour algorithm:
 *   1. Mark v as GREY (being processed)
 *   2. For each neighbour u of v:
 *      a. If colour[u] == GREY:
 *         - Cycle found! (back edge to ancestor)
 *         - Add u to cycle_vertices, return true
 *      b. If colour[u] == WHITE:
 *         - Recursively check u
 *         - If cycle found in subtree, return true
 *   3. Mark v as BLACK (finished processing)
 *   4. Return false (no cycle through this vertex)
 *
 * Hint: When a cycle is detected, you can trace it by following the path.
 */
bool detect_cycle_util(DiGraph *g, int v, Colour colour[], 
                       int cycle_vertices[], int *cycle_count) {
    /* YOUR CODE HERE */
    return false;  /* Replace this */
}

/**
 * TODO 10: Detect if the graph contains a cycle
 *
 * @param g Pointer to graph
 * @param cycle_vertices Array to store vertices in cycle (if found)
 * @param cycle_count Pointer to number of vertices in cycle
 * @return true if cycle exists, false otherwise
 *
 * Steps:
 *   1. Initialise all colours to WHITE
 *   2. For each vertex v:
 *      - If colour[v] == WHITE, run detect_cycle_util on v
 *      - If cycle found, return true
 *   3. Return false if no cycle found
 */
bool has_cycle(DiGraph *g, int cycle_vertices[], int *cycle_count) {
    /* YOUR CODE HERE */
    return false;  /* Replace this */
}

/* =============================================================================
 * TOPOLOGICAL SORT
 * =============================================================================
 */

/**
 * TODO 11: Implement topological sort utility (DFS-based)
 *
 * @param g Pointer to graph
 * @param v Current vertex
 * @param visited Array tracking visited vertices
 * @param stack Stack for storing topological order
 *
 * Algorithm:
 *   1. Mark v as visited
 *   2. For each unvisited neighbour u of v:
 *      - Recursively call topo_sort_util on u
 *   3. Push v onto the stack (AFTER processing all descendants)
 *
 * Note: Vertices are pushed after all descendants are processed,
 * so popping gives reverse post-order (valid topological order).
 */
void topo_sort_util(DiGraph *g, int v, bool visited[], Stack *stack) {
    /* YOUR CODE HERE */
}

/**
 * TODO 12: Perform topological sort on the graph
 *
 * @param g Pointer to graph
 * @param result Array to store topological ordering
 * @return Number of vertices if successful, -1 if cycle exists
 *
 * Steps:
 *   1. First check if graph has a cycle (call has_cycle)
 *   2. If cycle exists, return -1
 *   3. Initialise visited array and stack
 *   4. For each unvisited vertex, call topo_sort_util
 *   5. Pop all vertices from stack into result array
 *   6. Return number of vertices
 */
int topological_sort(DiGraph *g, int result[]) {
    /* YOUR CODE HERE */
    return -1;  /* Replace this */
}

/* =============================================================================
 * MEMORY MANAGEMENT
 * =============================================================================
 */

/**
 * TODO 13: Free all memory allocated for the directed graph
 *
 * @param g Pointer to graph
 *
 * Traverse each adjacency list and free all nodes.
 */
void digraph_destroy(DiGraph *g) {
    /* YOUR CODE HERE */
}

/* =============================================================================
 * OUTPUT FUNCTIONS (PROVIDED)
 * =============================================================================
 */

void print_array(const char *label, int arr[], int n) {
    printf("%s", label);
    for (int i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(" ");
    }
    printf("\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    DiGraph g;
    int n, m;
    
    /* Read number of vertices and edges */
    if (scanf("%d %d", &n, &m) != 2) {
        fprintf(stderr, "Error: Failed to read graph dimensions\n");
        return 1;
    }
    
    if (n <= 0 || n > MAX_VERTICES) {
        fprintf(stderr, "Error: Invalid number of vertices (must be 1-%d)\n", MAX_VERTICES);
        return 1;
    }
    
    /* Initialise graph */
    digraph_init(&g, n);
    
    /* Read edges */
    for (int i = 0; i < m; i++) {
        int u, v;
        if (scanf("%d %d", &u, &v) != 2) {
            fprintf(stderr, "Error: Failed to read edge %d\n", i + 1);
            digraph_destroy(&g);
            return 1;
        }
        
        if (u < 0 || u >= n || v < 0 || v >= n) {
            fprintf(stderr, "Error: Invalid vertex in edge %d\n", i + 1);
            digraph_destroy(&g);
            return 1;
        }
        
        digraph_add_edge(&g, u, v);
    }
    
    /* Perform DFS traversal */
    int traversal[MAX_VERTICES];
    int trav_count = dfs_traversal(&g, traversal);
    print_array("DFS traversal: ", traversal, trav_count);
    
    /* Check for cycles */
    int cycle_vertices[MAX_VERTICES];
    int cycle_count = 0;
    
    if (has_cycle(&g, cycle_vertices, &cycle_count)) {
        printf("Cycle detected! Vertices in cycle: ");
        for (int i = 0; i < cycle_count; i++) {
            printf("%d", cycle_vertices[i]);
            if (i < cycle_count - 1) printf(" ");
        }
        printf("\n");
        printf("Topological sort not possible (graph contains cycle)\n");
    } else {
        printf("No cycle detected - graph is a DAG\n");
        
        /* Perform topological sort */
        int topo_order[MAX_VERTICES];
        int topo_count = topological_sort(&g, topo_order);
        
        if (topo_count > 0) {
            print_array("Topological order: ", topo_order, topo_count);
        }
    }
    
    /* Clean up */
    digraph_destroy(&g);
    
    return 0;
}

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Implement Kahn's algorithm for topological sort (BFS-based approach)
 *    using in-degree counting instead of DFS
 *
 * 2. Find ALL possible topological orderings (backtracking approach)
 *
 * 3. Detect and print ALL cycles in the graph, not just whether one exists
 *
 * 4. Implement strongly connected components (Kosaraju's or Tarjan's algorithm)
 *
 * 5. Add support for weighted edges and implement a critical path algorithm
 *    for project scheduling (finding longest path in DAG)
 *
 * =============================================================================
 */
