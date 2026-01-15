/**
 * =============================================================================
 * WEEK 12: GRAPH FUNDAMENTALS
 * Complete Working Example
 * =============================================================================
 *
 * This example demonstrates:
 *   1. Graph creation using adjacency list representation
 *   2. Graph creation using adjacency matrix representation
 *   3. Breadth-First Search (BFS) traversal
 *   4. Depth-First Search (DFS) - recursive and iterative
 *   5. Shortest path computation using BFS
 *   6. Connected components detection
 *   7. Cycle detection in directed graphs
 *   8. Topological sorting
 *   9. Proper memory management
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o example1 example1.c
 * Usage: ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =============================================================================
 * PART 1: DATA STRUCTURES
 * =============================================================================
 */

/**
 * Node for adjacency list representation
 */
typedef struct AdjNode {
    int vertex;
    struct AdjNode *next;
} AdjNode;

/**
 * Graph using adjacency list
 */
typedef struct {
    int num_vertices;
    AdjNode **adj_list;
    int is_directed;
} GraphList;

/**
 * Graph using adjacency matrix
 */
typedef struct {
    int num_vertices;
    int **matrix;
    int is_directed;
} GraphMatrix;

/**
 * Simple queue for BFS
 */
typedef struct {
    int *data;
    int front;
    int rear;
    int size;
    int capacity;
} Queue;

/**
 * Simple stack for iterative DFS
 */
typedef struct {
    int *data;
    int top;
    int capacity;
} Stack;

/* =============================================================================
 * PART 2: QUEUE OPERATIONS
 * =============================================================================
 */

Queue *queue_create(int capacity) {
    Queue *q = malloc(sizeof(Queue));
    if (!q) return NULL;
    
    q->data = malloc(capacity * sizeof(int));
    if (!q->data) {
        free(q);
        return NULL;
    }
    
    q->front = 0;
    q->rear = -1;
    q->size = 0;
    q->capacity = capacity;
    return q;
}

int queue_is_empty(Queue *q) {
    return q->size == 0;
}

void queue_enqueue(Queue *q, int item) {
    if (q->size == q->capacity) {
        fprintf(stderr, "Queue overflow!\n");
        return;
    }
    q->rear = (q->rear + 1) % q->capacity;
    q->data[q->rear] = item;
    q->size++;
}

int queue_dequeue(Queue *q) {
    if (queue_is_empty(q)) {
        fprintf(stderr, "Queue underflow!\n");
        return -1;
    }
    int item = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;
    return item;
}

void queue_destroy(Queue *q) {
    if (q) {
        free(q->data);
        free(q);
    }
}

/* =============================================================================
 * PART 3: STACK OPERATIONS
 * =============================================================================
 */

Stack *stack_create(int capacity) {
    Stack *s = malloc(sizeof(Stack));
    if (!s) return NULL;
    
    s->data = malloc(capacity * sizeof(int));
    if (!s->data) {
        free(s);
        return NULL;
    }
    
    s->top = -1;
    s->capacity = capacity;
    return s;
}

int stack_is_empty(Stack *s) {
    return s->top == -1;
}

void stack_push(Stack *s, int item) {
    if (s->top == s->capacity - 1) {
        fprintf(stderr, "Stack overflow!\n");
        return;
    }
    s->data[++s->top] = item;
}

int stack_pop(Stack *s) {
    if (stack_is_empty(s)) {
        fprintf(stderr, "Stack underflow!\n");
        return -1;
    }
    return s->data[s->top--];
}

void stack_destroy(Stack *s) {
    if (s) {
        free(s->data);
        free(s);
    }
}

/* =============================================================================
 * PART 4: ADJACENCY LIST GRAPH OPERATIONS
 * =============================================================================
 */

/**
 * Create a new adjacency list node
 */
AdjNode *create_adj_node(int vertex) {
    AdjNode *node = malloc(sizeof(AdjNode));
    if (!node) return NULL;
    
    node->vertex = vertex;
    node->next = NULL;
    return node;
}

/**
 * Create a graph with n vertices using adjacency list
 */
GraphList *graph_list_create(int n, int directed) {
    GraphList *g = malloc(sizeof(GraphList));
    if (!g) return NULL;
    
    g->num_vertices = n;
    g->is_directed = directed;
    g->adj_list = calloc(n, sizeof(AdjNode *));
    
    if (!g->adj_list) {
        free(g);
        return NULL;
    }
    
    return g;
}

/**
 * Add an edge to the adjacency list graph
 */
void graph_list_add_edge(GraphList *g, int src, int dest) {
    if (src < 0 || src >= g->num_vertices ||
        dest < 0 || dest >= g->num_vertices) {
        fprintf(stderr, "Invalid vertex index!\n");
        return;
    }
    
    /* Add edge from src to dest */
    AdjNode *node = create_adj_node(dest);
    node->next = g->adj_list[src];
    g->adj_list[src] = node;
    
    /* For undirected graphs, add reverse edge */
    if (!g->is_directed) {
        node = create_adj_node(src);
        node->next = g->adj_list[dest];
        g->adj_list[dest] = node;
    }
}

/**
 * Print the adjacency list representation
 */
void graph_list_print(GraphList *g) {
    printf("Adjacency List Representation:\n");
    for (int i = 0; i < g->num_vertices; i++) {
        printf("  Vertex %d:", i);
        for (AdjNode *adj = g->adj_list[i]; adj; adj = adj->next) {
            printf(" -> %d", adj->vertex);
        }
        printf(" -> NULL\n");
    }
}

/**
 * Destroy the adjacency list graph and free all memory
 */
void graph_list_destroy(GraphList *g) {
    if (!g) return;
    
    for (int i = 0; i < g->num_vertices; i++) {
        AdjNode *current = g->adj_list[i];
        while (current) {
            AdjNode *temp = current;
            current = current->next;
            free(temp);
        }
    }
    
    free(g->adj_list);
    free(g);
}

/* =============================================================================
 * PART 5: ADJACENCY MATRIX GRAPH OPERATIONS
 * =============================================================================
 */

/**
 * Create a graph with n vertices using adjacency matrix
 */
GraphMatrix *graph_matrix_create(int n, int directed) {
    GraphMatrix *g = malloc(sizeof(GraphMatrix));
    if (!g) return NULL;
    
    g->num_vertices = n;
    g->is_directed = directed;
    
    /* Allocate 2D matrix */
    g->matrix = malloc(n * sizeof(int *));
    if (!g->matrix) {
        free(g);
        return NULL;
    }
    
    for (int i = 0; i < n; i++) {
        g->matrix[i] = calloc(n, sizeof(int));
        if (!g->matrix[i]) {
            /* Clean up on failure */
            for (int j = 0; j < i; j++) {
                free(g->matrix[j]);
            }
            free(g->matrix);
            free(g);
            return NULL;
        }
    }
    
    return g;
}

/**
 * Add an edge to the adjacency matrix graph
 */
void graph_matrix_add_edge(GraphMatrix *g, int src, int dest) {
    if (src < 0 || src >= g->num_vertices ||
        dest < 0 || dest >= g->num_vertices) {
        fprintf(stderr, "Invalid vertex index!\n");
        return;
    }
    
    g->matrix[src][dest] = 1;
    
    if (!g->is_directed) {
        g->matrix[dest][src] = 1;
    }
}

/**
 * Print the adjacency matrix representation
 */
void graph_matrix_print(GraphMatrix *g) {
    printf("Adjacency Matrix Representation:\n");
    printf("     ");
    for (int i = 0; i < g->num_vertices; i++) {
        printf("%3d ", i);
    }
    printf("\n    +");
    for (int i = 0; i < g->num_vertices; i++) {
        printf("----");
    }
    printf("\n");
    
    for (int i = 0; i < g->num_vertices; i++) {
        printf("  %d |", i);
        for (int j = 0; j < g->num_vertices; j++) {
            printf("%3d ", g->matrix[i][j]);
        }
        printf("\n");
    }
}

/**
 * Destroy the adjacency matrix graph and free all memory
 */
void graph_matrix_destroy(GraphMatrix *g) {
    if (!g) return;
    
    for (int i = 0; i < g->num_vertices; i++) {
        free(g->matrix[i]);
    }
    free(g->matrix);
    free(g);
}

/* =============================================================================
 * PART 6: BREADTH-FIRST SEARCH (BFS)
 * =============================================================================
 */

/**
 * BFS traversal from a source vertex
 */
void bfs_traversal(GraphList *g, int source) {
    int *visited = calloc(g->num_vertices, sizeof(int));
    Queue *q = queue_create(g->num_vertices);
    
    printf("BFS traversal from vertex %d: ", source);
    
    visited[source] = 1;
    queue_enqueue(q, source);
    
    while (!queue_is_empty(q)) {
        int v = queue_dequeue(q);
        printf("%d ", v);
        
        /* Visit all adjacent vertices */
        for (AdjNode *adj = g->adj_list[v]; adj; adj = adj->next) {
            if (!visited[adj->vertex]) {
                visited[adj->vertex] = 1;
                queue_enqueue(q, adj->vertex);
            }
        }
    }
    printf("\n");
    
    free(visited);
    queue_destroy(q);
}

/**
 * BFS to compute shortest paths from source
 * Returns array of distances (-1 means unreachable)
 */
int *bfs_shortest_path(GraphList *g, int source) {
    int *distance = malloc(g->num_vertices * sizeof(int));
    
    /* Initialise all distances to -1 (unreachable) */
    for (int i = 0; i < g->num_vertices; i++) {
        distance[i] = -1;
    }
    
    Queue *q = queue_create(g->num_vertices);
    
    distance[source] = 0;
    queue_enqueue(q, source);
    
    while (!queue_is_empty(q)) {
        int v = queue_dequeue(q);
        
        for (AdjNode *adj = g->adj_list[v]; adj; adj = adj->next) {
            if (distance[adj->vertex] == -1) {
                distance[adj->vertex] = distance[v] + 1;
                queue_enqueue(q, adj->vertex);
            }
        }
    }
    
    queue_destroy(q);
    return distance;  /* Caller must free */
}

/* =============================================================================
 * PART 7: DEPTH-FIRST SEARCH (DFS)
 * =============================================================================
 */

/**
 * Recursive DFS helper function
 */
void dfs_recursive_helper(GraphList *g, int v, int *visited) {
    visited[v] = 1;
    printf("%d ", v);
    
    for (AdjNode *adj = g->adj_list[v]; adj; adj = adj->next) {
        if (!visited[adj->vertex]) {
            dfs_recursive_helper(g, adj->vertex, visited);
        }
    }
}

/**
 * DFS traversal (recursive version)
 */
void dfs_recursive(GraphList *g, int source) {
    int *visited = calloc(g->num_vertices, sizeof(int));
    
    printf("DFS (recursive) from vertex %d: ", source);
    dfs_recursive_helper(g, source, visited);
    printf("\n");
    
    free(visited);
}

/**
 * DFS traversal (iterative version using explicit stack)
 */
void dfs_iterative(GraphList *g, int source) {
    int *visited = calloc(g->num_vertices, sizeof(int));
    Stack *s = stack_create(g->num_vertices);
    
    printf("DFS (iterative) from vertex %d: ", source);
    
    stack_push(s, source);
    
    while (!stack_is_empty(s)) {
        int v = stack_pop(s);
        
        if (!visited[v]) {
            visited[v] = 1;
            printf("%d ", v);
            
            /* Push all unvisited neighbours onto stack */
            for (AdjNode *adj = g->adj_list[v]; adj; adj = adj->next) {
                if (!visited[adj->vertex]) {
                    stack_push(s, adj->vertex);
                }
            }
        }
    }
    printf("\n");
    
    free(visited);
    stack_destroy(s);
}

/* =============================================================================
 * PART 8: CONNECTED COMPONENTS
 * =============================================================================
 */

/**
 * BFS to mark all vertices in a component
 */
void bfs_component(GraphList *g, int source, int *visited, int component_id) {
    Queue *q = queue_create(g->num_vertices);
    
    visited[source] = component_id;
    queue_enqueue(q, source);
    
    while (!queue_is_empty(q)) {
        int v = queue_dequeue(q);
        
        for (AdjNode *adj = g->adj_list[v]; adj; adj = adj->next) {
            if (visited[adj->vertex] == 0) {
                visited[adj->vertex] = component_id;
                queue_enqueue(q, adj->vertex);
            }
        }
    }
    
    queue_destroy(q);
}

/**
 * Find all connected components in an undirected graph
 * Returns the number of components
 */
int find_connected_components(GraphList *g, int *component) {
    /* Initialise all vertices as unvisited (component 0) */
    for (int i = 0; i < g->num_vertices; i++) {
        component[i] = 0;
    }
    
    int num_components = 0;
    
    for (int v = 0; v < g->num_vertices; v++) {
        if (component[v] == 0) {
            num_components++;
            bfs_component(g, v, component, num_components);
        }
    }
    
    return num_components;
}

/* =============================================================================
 * PART 9: CYCLE DETECTION IN DIRECTED GRAPHS
 * =============================================================================
 */

#define WHITE 0
#define GREY  1
#define BLACK 2

/**
 * DFS helper for cycle detection
 * Returns 1 if cycle found, 0 otherwise
 */
int has_cycle_dfs(GraphList *g, int v, int *colour) {
    colour[v] = GREY;
    
    for (AdjNode *adj = g->adj_list[v]; adj; adj = adj->next) {
        /* Back edge to a vertex in current DFS path */
        if (colour[adj->vertex] == GREY) {
            return 1;
        }
        /* Recurse on unvisited vertices */
        if (colour[adj->vertex] == WHITE) {
            if (has_cycle_dfs(g, adj->vertex, colour)) {
                return 1;
            }
        }
    }
    
    colour[v] = BLACK;
    return 0;
}

/**
 * Detect if a directed graph has a cycle
 */
int graph_has_cycle(GraphList *g) {
    int *colour = calloc(g->num_vertices, sizeof(int));
    
    for (int v = 0; v < g->num_vertices; v++) {
        if (colour[v] == WHITE) {
            if (has_cycle_dfs(g, v, colour)) {
                free(colour);
                return 1;
            }
        }
    }
    
    free(colour);
    return 0;
}

/* =============================================================================
 * PART 10: TOPOLOGICAL SORT
 * =============================================================================
 */

/**
 * DFS helper for topological sort
 */
void topo_sort_dfs(GraphList *g, int v, int *visited, Stack *result) {
    visited[v] = 1;
    
    for (AdjNode *adj = g->adj_list[v]; adj; adj = adj->next) {
        if (!visited[adj->vertex]) {
            topo_sort_dfs(g, adj->vertex, visited, result);
        }
    }
    
    /* Push vertex after all descendants are processed */
    stack_push(result, v);
}

/**
 * Perform topological sort on a DAG
 * Returns array with topological order (caller must free)
 */
int *topological_sort(GraphList *g) {
    if (graph_has_cycle(g)) {
        fprintf(stderr, "Error: Graph has a cycle, topological sort not possible!\n");
        return NULL;
    }
    
    int *visited = calloc(g->num_vertices, sizeof(int));
    Stack *result = stack_create(g->num_vertices);
    
    for (int v = 0; v < g->num_vertices; v++) {
        if (!visited[v]) {
            topo_sort_dfs(g, v, visited, result);
        }
    }
    
    /* Extract topological order from stack */
    int *order = malloc(g->num_vertices * sizeof(int));
    for (int i = 0; i < g->num_vertices; i++) {
        order[i] = stack_pop(result);
    }
    
    free(visited);
    stack_destroy(result);
    return order;
}

/* =============================================================================
 * DEMONSTRATION FUNCTIONS
 * =============================================================================
 */

void demo_graph_representations(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     PART 1: GRAPH REPRESENTATIONS                             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Creating an undirected graph with 5 vertices:\n");
    printf("Edges: (0,1), (0,4), (1,2), (1,3), (1,4), (2,3), (3,4)\n\n");
    
    /* Adjacency List */
    GraphList *gl = graph_list_create(5, 0);
    graph_list_add_edge(gl, 0, 1);
    graph_list_add_edge(gl, 0, 4);
    graph_list_add_edge(gl, 1, 2);
    graph_list_add_edge(gl, 1, 3);
    graph_list_add_edge(gl, 1, 4);
    graph_list_add_edge(gl, 2, 3);
    graph_list_add_edge(gl, 3, 4);
    
    graph_list_print(gl);
    printf("\n");
    
    /* Adjacency Matrix */
    GraphMatrix *gm = graph_matrix_create(5, 0);
    graph_matrix_add_edge(gm, 0, 1);
    graph_matrix_add_edge(gm, 0, 4);
    graph_matrix_add_edge(gm, 1, 2);
    graph_matrix_add_edge(gm, 1, 3);
    graph_matrix_add_edge(gm, 1, 4);
    graph_matrix_add_edge(gm, 2, 3);
    graph_matrix_add_edge(gm, 3, 4);
    
    graph_matrix_print(gm);
    
    graph_list_destroy(gl);
    graph_matrix_destroy(gm);
}

void demo_bfs(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     PART 2: BREADTH-FIRST SEARCH (BFS)                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    GraphList *g = graph_list_create(6, 0);
    graph_list_add_edge(g, 0, 1);
    graph_list_add_edge(g, 0, 2);
    graph_list_add_edge(g, 1, 3);
    graph_list_add_edge(g, 2, 3);
    graph_list_add_edge(g, 3, 4);
    graph_list_add_edge(g, 4, 5);
    
    printf("Graph structure:\n");
    printf("    0 --- 1\n");
    printf("    |     |\n");
    printf("    2 --- 3 --- 4 --- 5\n\n");
    
    bfs_traversal(g, 0);
    
    printf("\nShortest paths from vertex 0:\n");
    int *dist = bfs_shortest_path(g, 0);
    for (int i = 0; i < 6; i++) {
        printf("  Distance to vertex %d: %d\n", i, dist[i]);
    }
    free(dist);
    
    graph_list_destroy(g);
}

void demo_dfs(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     PART 3: DEPTH-FIRST SEARCH (DFS)                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    GraphList *g = graph_list_create(6, 0);
    graph_list_add_edge(g, 0, 1);
    graph_list_add_edge(g, 0, 2);
    graph_list_add_edge(g, 1, 3);
    graph_list_add_edge(g, 2, 4);
    graph_list_add_edge(g, 3, 5);
    
    printf("Graph structure:\n");
    printf("    0\n");
    printf("   / \\\n");
    printf("  1   2\n");
    printf("  |   |\n");
    printf("  3   4\n");
    printf("  |\n");
    printf("  5\n\n");
    
    dfs_recursive(g, 0);
    dfs_iterative(g, 0);
    
    printf("\nNote: Order may differ between versions due to adjacency list order.\n");
    
    graph_list_destroy(g);
}

void demo_connected_components(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     PART 4: CONNECTED COMPONENTS                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    GraphList *g = graph_list_create(8, 0);
    /* Component 1: 0-1-2 */
    graph_list_add_edge(g, 0, 1);
    graph_list_add_edge(g, 1, 2);
    /* Component 2: 3-4-5 */
    graph_list_add_edge(g, 3, 4);
    graph_list_add_edge(g, 4, 5);
    graph_list_add_edge(g, 3, 5);
    /* Component 3: 6-7 */
    graph_list_add_edge(g, 6, 7);
    
    printf("Graph with 3 disconnected components:\n");
    printf("  Component 1: 0 -- 1 -- 2\n");
    printf("  Component 2: 3 -- 4 -- 5 (triangle)\n");
    printf("  Component 3: 6 -- 7\n\n");
    
    int *component = malloc(g->num_vertices * sizeof(int));
    int num_components = find_connected_components(g, component);
    
    printf("Number of connected components: %d\n\n", num_components);
    printf("Component assignment:\n");
    for (int i = 0; i < g->num_vertices; i++) {
        printf("  Vertex %d: Component %d\n", i, component[i]);
    }
    
    free(component);
    graph_list_destroy(g);
}

void demo_cycle_detection(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     PART 5: CYCLE DETECTION                                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Graph with cycle */
    printf("Test 1: Directed graph WITH cycle\n");
    printf("  0 -> 1 -> 2 -> 0 (cycle)\n\n");
    
    GraphList *g1 = graph_list_create(3, 1);
    graph_list_add_edge(g1, 0, 1);
    graph_list_add_edge(g1, 1, 2);
    graph_list_add_edge(g1, 2, 0);
    
    printf("  Has cycle: %s\n\n", graph_has_cycle(g1) ? "YES" : "NO");
    graph_list_destroy(g1);
    
    /* Graph without cycle (DAG) */
    printf("Test 2: Directed Acyclic Graph (DAG)\n");
    printf("  0 -> 1 -> 2\n");
    printf("  0 -> 2\n\n");
    
    GraphList *g2 = graph_list_create(3, 1);
    graph_list_add_edge(g2, 0, 1);
    graph_list_add_edge(g2, 1, 2);
    graph_list_add_edge(g2, 0, 2);
    
    printf("  Has cycle: %s\n", graph_has_cycle(g2) ? "YES" : "NO");
    graph_list_destroy(g2);
}

void demo_topological_sort(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     PART 6: TOPOLOGICAL SORT                                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Task dependency graph:\n");
    printf("  Task A (0) -> Task B (1) -> Task D (3)\n");
    printf("  Task A (0) -> Task C (2) -> Task D (3)\n");
    printf("  Task B (1) -> Task E (4)\n");
    printf("  Task D (3) -> Task E (4)\n\n");
    
    GraphList *g = graph_list_create(5, 1);
    graph_list_add_edge(g, 0, 1);  /* A -> B */
    graph_list_add_edge(g, 0, 2);  /* A -> C */
    graph_list_add_edge(g, 1, 3);  /* B -> D */
    graph_list_add_edge(g, 2, 3);  /* C -> D */
    graph_list_add_edge(g, 1, 4);  /* B -> E */
    graph_list_add_edge(g, 3, 4);  /* D -> E */
    
    int *order = topological_sort(g);
    if (order) {
        printf("Topological order (execution sequence):\n  ");
        for (int i = 0; i < 5; i++) {
            char task = 'A' + order[i];
            printf("%c (Task %d)", task, order[i]);
            if (i < 4) printf(" -> ");
        }
        printf("\n");
        free(order);
    }
    
    graph_list_destroy(g);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     WEEK 12: GRAPH FUNDAMENTALS - Complete Example            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    demo_graph_representations();
    demo_bfs();
    demo_dfs();
    demo_connected_components();
    demo_cycle_detection();
    demo_topological_sort();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     END OF DEMONSTRATION                                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
