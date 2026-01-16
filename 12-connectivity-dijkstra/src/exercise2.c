/**
 * =============================================================================
 * EXERCISE 2: Depth-First Search, Cycle Detection and Topological Sort
 * =============================================================================
 *
 * Objective
 *   Implement DFS traversal on a directed graph, detect directed cycles using
 *   the three-colour method and, for acyclic graphs, output a valid
 *   topological ordering.
 *
 * Input format
 *   Line 1: n m (number of vertices and directed edges)
 *   Next m lines: u v (directed edge u -> v)
 *
 * Output format
 *   DFS traversal: <v0 v1 ...>
 *   Either:
 *     Cycle detected! Vertices in cycle: <...>
 *     Topological sort not possible (graph contains cycle)
 *   Or:
 *     No cycle detected - graph is a DAG
 *     Topological order: <...>
 *
 * Compilation
 *   gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 *
 * =============================================================================
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* =============================================================================
 * CONSTANTS AND COLOUR DEFINITIONS
 * =============================================================================
 */

#define MAX_VERTICES 1000

typedef enum {
    WHITE = 0, /* Not discovered */
    GREY = 1,  /* Discovered and in current DFS stack */
    BLACK = 2  /* Fully processed */
} Colour;

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct AdjNode {
    int vertex;
    struct AdjNode *next;
} AdjNode;

typedef struct {
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

static void stack_init(Stack *s) {
    s->top = -1;
}

static bool stack_is_empty(const Stack *s) {
    return s->top == -1;
}

static void stack_push(Stack *s, int value) {
    s->items[++s->top] = value;
}

static int stack_pop(Stack *s) {
    return s->items[s->top--];
}

static int stack_peek(const Stack *s) {
    return s->items[s->top];
}

/* =============================================================================
 * GRAPH OPERATIONS
 * =============================================================================
 */

static void digraph_init(DiGraph *g, int n) {
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

static void digraph_add_edge(DiGraph *g, int u, int v) {
    AdjNode *node = create_node(v);
    if (node == NULL) {
        return;
    }
    node->next = g->adj[u];
    g->adj[u] = node;
    g->num_edges++;
}

/* =============================================================================
 * DFS TRAVERSAL
 * =============================================================================
 */

static void dfs_recursive_util(const DiGraph *g, int v, bool visited[], int traversal[],
                              int *count) {
    visited[v] = true;
    traversal[(*count)++] = v;

    for (AdjNode *curr = g->adj[v]; curr != NULL; curr = curr->next) {
        int u = curr->vertex;
        if (!visited[u]) {
            dfs_recursive_util(g, u, visited, traversal, count);
        }
    }
}

static int dfs_traversal(const DiGraph *g, int traversal[]) {
    bool visited[MAX_VERTICES];
    int count = 0;

    for (int i = 0; i < g->num_vertices; i++) {
        visited[i] = false;
    }

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

static int parent_vertex[MAX_VERTICES];

static bool detect_cycle_util(const DiGraph *g, int v, Colour colour[], int cycle_vertices[],
                             int *cycle_count) {
    colour[v] = GREY;

    for (AdjNode *curr = g->adj[v]; curr != NULL; curr = curr->next) {
        int u = curr->vertex;

        if (colour[u] == GREY) {
            /*
             * Back edge v -> u closes a directed cycle where u is an ancestor of v
             * in the current DFS recursion stack.
             */
            int tmp[MAX_VERTICES];
            int len = 0;

            int x = v;
            while (x != u && x != -1 && len < MAX_VERTICES) {
                tmp[len++] = x;
                x = parent_vertex[x];
            }
            tmp[len++] = u;

            /* Reverse to obtain u ... v order. */
            *cycle_count = 0;
            for (int i = len - 1; i >= 0; i--) {
                cycle_vertices[(*cycle_count)++] = tmp[i];
            }

            return true;
        }

        if (colour[u] == WHITE) {
            parent_vertex[u] = v;
            if (detect_cycle_util(g, u, colour, cycle_vertices, cycle_count)) {
                return true;
            }
        }
    }

    colour[v] = BLACK;
    return false;
}

static bool has_cycle(const DiGraph *g, int cycle_vertices[], int *cycle_count) {
    Colour colour[MAX_VERTICES];

    for (int i = 0; i < g->num_vertices; i++) {
        colour[i] = WHITE;
        parent_vertex[i] = -1;
    }

    *cycle_count = 0;

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

static void topo_sort_util(const DiGraph *g, int v, bool visited[], Stack *stack) {
    visited[v] = true;

    for (AdjNode *curr = g->adj[v]; curr != NULL; curr = curr->next) {
        int u = curr->vertex;
        if (!visited[u]) {
            topo_sort_util(g, u, visited, stack);
        }
    }

    /* Post-order push yields reverse post-order when popped. */
    stack_push(stack, v);
}

static int topological_sort(const DiGraph *g, int result[]) {
    int dummy_cycle[MAX_VERTICES];
    int dummy_count = 0;
    if (has_cycle(g, dummy_cycle, &dummy_count)) {
        return -1;
    }

    bool visited[MAX_VERTICES];
    Stack stack;
    stack_init(&stack);

    for (int i = 0; i < g->num_vertices; i++) {
        visited[i] = false;
    }

    for (int v = 0; v < g->num_vertices; v++) {
        if (!visited[v]) {
            topo_sort_util(g, v, visited, &stack);
        }
    }

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

static void digraph_destroy(DiGraph *g) {
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
 * OUTPUT FUNCTIONS (PROVIDED)
 * =============================================================================
 */

static void print_array(const char *label, const int arr[], int n) {
    printf("%s", label);
    for (int i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) {
            printf(" ");
        }
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

    if (scanf("%d %d", &n, &m) != 2) {
        fprintf(stderr, "Error: Failed to read graph dimensions\n");
        return 1;
    }

    if (n <= 0 || n > MAX_VERTICES) {
        fprintf(stderr, "Error: Invalid number of vertices (must be 1-%d)\n", MAX_VERTICES);
        return 1;
    }

    digraph_init(&g, n);

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

    int traversal[MAX_VERTICES];
    int trav_count = dfs_traversal(&g, traversal);
    print_array("DFS traversal: ", traversal, trav_count);

    int cycle_vertices[MAX_VERTICES];
    int cycle_count = 0;

    if (has_cycle(&g, cycle_vertices, &cycle_count)) {
        printf("Cycle detected! Vertices in cycle: ");
        for (int i = 0; i < cycle_count; i++) {
            printf("%d", cycle_vertices[i]);
            if (i < cycle_count - 1) {
                printf(" ");
            }
        }
        printf("\n");
        printf("Topological sort not possible (graph contains cycle)\n");
    } else {
        printf("No cycle detected - graph is a DAG\n");

        int topo_order[MAX_VERTICES];
        int topo_count = topological_sort(&g, topo_order);

        if (topo_count > 0) {
            print_array("Topological order: ", topo_order, topo_count);
        }
    }

    digraph_destroy(&g);
    (void)stack_peek; /* silence unused-function warnings in some builds */
    return 0;
}
