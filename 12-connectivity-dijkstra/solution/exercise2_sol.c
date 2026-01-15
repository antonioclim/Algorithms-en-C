/**
 * =============================================================================
 * EXERCISE 2 SOLUTION: DFS, Cycle Detection and Topological Sort
 * =============================================================================
 *
 * INSTRUCTOR SOLUTION - DO NOT DISTRIBUTE TO STUDENTS
 *
 * This file contains the complete, working solution for Exercise 2.
 * All TODOs have been implemented with proper error handling and
 * memory management.
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise2_sol exercise2_sol.c
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
 * TODO 1 SOLUTION: Three colours for DFS cycle detection
 */
typedef enum {
    WHITE = 0,  /* Not yet discovered */
    GREY = 1,   /* Discovered but not finished (in progress) */
    BLACK = 2   /* Completely processed */
} Colour;

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * TODO 2 SOLUTION: Adjacency list node structure
 */
typedef struct AdjNode {
    int vertex;
    struct AdjNode *next;
} AdjNode;

/**
 * TODO 3 SOLUTION: Directed graph structure
 */
typedef struct {
    int num_vertices;
    int num_edges;
    AdjNode *adj[MAX_VERTICES];
} DiGraph;

/**
 * Stack structure for iterative DFS and topological sort
 */
typedef struct {
    int items[MAX_VERTICES];
    int top;
} Stack;

/* =============================================================================
 * STACK OPERATIONS
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
 * TODO 4 SOLUTION: Initialise the directed graph
 */
void digraph_init(DiGraph *g, int n) {
    g->num_vertices = n;
    g->num_edges = 0;
    
    for (int i = 0; i < n; i++) {
        g->adj[i] = NULL;
    }
}

/**
 * TODO 5 SOLUTION: Create a new adjacency list node
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
 * TODO 6 SOLUTION: Add a directed edge from u to v
 */
void digraph_add_edge(DiGraph *g, int u, int v) {
    /* For directed graph, only add edge u -> v */
    AdjNode *node = create_node(v);
    if (node != NULL) {
        node->next = g->adj[u];
        g->adj[u] = node;
        g->num_edges++;
    }
}

/* =============================================================================
 * DFS TRAVERSAL
 * =============================================================================
 */

/**
 * TODO 7 SOLUTION: Recursive DFS traversal utility
 */
void dfs_recursive_util(DiGraph *g, int v, bool visited[], int traversal[], int *count) {
    /* Mark vertex as visited */
    visited[v] = true;
    
    /* Add to traversal order */
    traversal[(*count)++] = v;
    
    /* Visit all unvisited neighbours */
    AdjNode *curr = g->adj[v];
    while (curr != NULL) {
        if (!visited[curr->vertex]) {
            dfs_recursive_util(g, curr->vertex, visited, traversal, count);
        }
        curr = curr->next;
    }
}

/**
 * TODO 8 SOLUTION: Full DFS traversal
 */
int dfs_traversal(DiGraph *g, int traversal[]) {
    bool visited[MAX_VERTICES];
    int count = 0;
    
    /* Initialise all vertices as unvisited */
    for (int i = 0; i < g->num_vertices; i++) {
        visited[i] = false;
    }
    
    /* Run DFS from each unvisited vertex (handles disconnected components) */
    for (int v = 0; v < g->num_vertices; v++) {
        if (!visited[v]) {
            dfs_recursive_util(g, v, visited, traversal, &count);
        }
    }
    
    return count;
}

/* =============================================================================
 * CYCLE DETECTION
 * =============================================================================
 */

/* Global arrays for cycle tracking */
static int parent_vertex[MAX_VERTICES];

/**
 * TODO 9 SOLUTION: Cycle detection using three-colour DFS
 */
bool detect_cycle_util(DiGraph *g, int v, Colour colour[], 
                       int cycle_vertices[], int *cycle_count) {
    /* Mark vertex as being processed (GREY) */
    colour[v] = GREY;
    
    /* Explore all neighbours */
    AdjNode *curr = g->adj[v];
    while (curr != NULL) {
        int u = curr->vertex;
        
        /* If neighbour is GREY, we found a back edge (cycle!) */
        if (colour[u] == GREY) {
            /* Reconstruct cycle by backtracking */
            cycle_vertices[(*cycle_count)++] = u;
            
            /* Trace back to find all vertices in the cycle */
            int trace = v;
            while (trace != u && *cycle_count < MAX_VERTICES) {
                cycle_vertices[(*cycle_count)++] = trace;
                
                /* Find parent of trace by looking at who called it */
                bool found = false;
                for (int i = 0; i < g->num_vertices && !found; i++) {
                    if (colour[i] == GREY) {
                        AdjNode *check = g->adj[i];
                        while (check != NULL) {
                            if (check->vertex == trace && parent_vertex[trace] == i) {
                                trace = i;
                                found = true;
                                break;
                            }
                            check = check->next;
                        }
                    }
                }
                if (!found) break;
            }
            
            return true;
        }
        
        /* If neighbour is WHITE, recursively explore it */
        if (colour[u] == WHITE) {
            parent_vertex[u] = v;
            if (detect_cycle_util(g, u, colour, cycle_vertices, cycle_count)) {
                return true;
            }
        }
        
        curr = curr->next;
    }
    
    /* Mark vertex as completely processed (BLACK) */
    colour[v] = BLACK;
    
    return false;
}

/**
 * TODO 10 SOLUTION: Detect if graph contains a cycle
 */
bool has_cycle(DiGraph *g, int cycle_vertices[], int *cycle_count) {
    Colour colour[MAX_VERTICES];
    
    /* Initialise all vertices as WHITE (unvisited) */
    for (int i = 0; i < g->num_vertices; i++) {
        colour[i] = WHITE;
        parent_vertex[i] = -1;
    }
    
    *cycle_count = 0;
    
    /* Check from each unvisited vertex */
    for (int v = 0; v < g->num_vertices; v++) {
        if (colour[v] == WHITE) {
            if (detect_cycle_util(g, v, colour, cycle_vertices, cycle_count)) {
                return true;
            }
        }
    }
    
    return false;
}

/* =============================================================================
 * TOPOLOGICAL SORT
 * =============================================================================
 */

/**
 * TODO 11 SOLUTION: Topological sort utility (DFS-based)
 */
void topo_sort_util(DiGraph *g, int v, bool visited[], Stack *stack) {
    /* Mark current vertex as visited */
    visited[v] = true;
    
    /* Recursively process all unvisited neighbours */
    AdjNode *curr = g->adj[v];
    while (curr != NULL) {
        if (!visited[curr->vertex]) {
            topo_sort_util(g, curr->vertex, visited, stack);
        }
        curr = curr->next;
    }
    
    /* Push vertex to stack AFTER all descendants are processed */
    /* This gives us reverse post-order (valid topological order) */
    stack_push(stack, v);
}

/**
 * TODO 12 SOLUTION: Perform topological sort
 */
int topological_sort(DiGraph *g, int result[]) {
    /* First check for cycles */
    int dummy_cycle[MAX_VERTICES];
    int dummy_count = 0;
    
    if (has_cycle(g, dummy_cycle, &dummy_count)) {
        return -1;  /* Cannot topologically sort a cyclic graph */
    }
    
    bool visited[MAX_VERTICES];
    Stack stack;
    
    /* Initialise */
    for (int i = 0; i < g->num_vertices; i++) {
        visited[i] = false;
    }
    stack_init(&stack);
    
    /* Run DFS from each unvisited vertex */
    for (int v = 0; v < g->num_vertices; v++) {
        if (!visited[v]) {
            topo_sort_util(g, v, visited, &stack);
        }
    }
    
    /* Pop all vertices from stack into result array */
    int count = 0;
    while (!stack_is_empty(&stack)) {
        result[count++] = stack_pop(&stack);
    }
    
    return count;
}

/* =============================================================================
 * MEMORY MANAGEMENT
 * =============================================================================
 */

/**
 * TODO 13 SOLUTION: Free all memory
 */
void digraph_destroy(DiGraph *g) {
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
 * OUTPUT FUNCTIONS
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
