/**
 * =============================================================================
 * WEEK 13: GRAPH ALGORITHMS - SHORTEST PATH
 * Complete Working Example
 * =============================================================================
 *
 * This example demonstrates:
 *   1. Graph representation (adjacency matrix and adjacency list)
 *   2. Dijkstra's algorithm with O(V²) array implementation
 *   3. Dijkstra's algorithm with O((V+E) log V) binary heap
 *   4. Bellman-Ford algorithm with negative cycle detection
 *   5. Path reconstruction and printing
 *   6. Comparison of algorithm performance
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -g -O2 -o example1 example1.c
 * Usage: ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <time.h>

/* =============================================================================
 * CONSTANTS AND TYPE DEFINITIONS
 * =============================================================================
 */

#define MAX_VERTICES 100
#define INF INT_MAX

/* Edge structure for adjacency list representation */
typedef struct Edge {
    int dest;           /* Destination vertex */
    int weight;         /* Edge weight */
    struct Edge *next;  /* Pointer to next edge */
} Edge;

/* Graph structure supporting both representations */
typedef struct {
    int V;                          /* Number of vertices */
    int E;                          /* Number of edges */
    int adj_matrix[MAX_VERTICES][MAX_VERTICES];  /* Adjacency matrix */
    Edge *adj_list[MAX_VERTICES];   /* Adjacency list */
} Graph;

/* Priority queue node for heap-based Dijkstra */
typedef struct {
    int vertex;
    int distance;
} HeapNode;

/* Min-heap structure */
typedef struct {
    HeapNode *nodes;
    int *position;  /* Position of each vertex in heap */
    int size;
    int capacity;
} MinHeap;

/* =============================================================================
 * PART 1: GRAPH CREATION AND UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Initialise a new graph with V vertices.
 * Sets all matrix entries to INF (no edge) and list heads to NULL.
 */
Graph *create_graph(int V) {
    Graph *g = (Graph *)malloc(sizeof(Graph));
    if (!g) {
        fprintf(stderr, "Error: Memory allocation failed for graph\n");
        exit(EXIT_FAILURE);
    }
    
    g->V = V;
    g->E = 0;
    
    /* Initialise adjacency matrix with INF (no connection) */
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            g->adj_matrix[i][j] = (i == j) ? 0 : INF;
        }
        g->adj_list[i] = NULL;
    }
    
    return g;
}

/**
 * Add a directed edge from u to v with given weight.
 * Updates both adjacency matrix and adjacency list.
 */
void add_edge(Graph *g, int u, int v, int weight) {
    /* Update adjacency matrix */
    g->adj_matrix[u][v] = weight;
    
    /* Update adjacency list */
    Edge *new_edge = (Edge *)malloc(sizeof(Edge));
    if (!new_edge) {
        fprintf(stderr, "Error: Memory allocation failed for edge\n");
        exit(EXIT_FAILURE);
    }
    
    new_edge->dest = v;
    new_edge->weight = weight;
    new_edge->next = g->adj_list[u];
    g->adj_list[u] = new_edge;
    
    g->E++;
}

/**
 * Free all memory allocated for the graph.
 */
void free_graph(Graph *g) {
    if (!g) return;
    
    for (int i = 0; i < g->V; i++) {
        Edge *current = g->adj_list[i];
        while (current) {
            Edge *temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(g);
}

/**
 * Print the path from source to destination using parent array.
 */
void print_path(int parent[], int dest) {
    if (parent[dest] == -1) {
        printf("%d", dest);
        return;
    }
    print_path(parent, parent[dest]);
    printf(" -> %d", dest);
}

/**
 * Print all shortest paths and distances from source.
 */
void print_solution(int dist[], int parent[], int V, int src) {
    printf("\n  Vertex    Distance    Path from %d\n", src);
    printf("  ────────────────────────────────────────\n");
    
    for (int i = 0; i < V; i++) {
        printf("    %d    ", i);
        if (dist[i] == INF) {
            printf("     ∞        (unreachable)\n");
        } else {
            printf("  %4d        ", dist[i]);
            print_path(parent, i);
            printf("\n");
        }
    }
}

/* =============================================================================
 * PART 2: MIN-HEAP OPERATIONS FOR OPTIMISED DIJKSTRA
 * =============================================================================
 */

/**
 * Create a new min-heap with given capacity.
 */
MinHeap *create_min_heap(int capacity) {
    MinHeap *heap = (MinHeap *)malloc(sizeof(MinHeap));
    heap->nodes = (HeapNode *)malloc(capacity * sizeof(HeapNode));
    heap->position = (int *)malloc(capacity * sizeof(int));
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

/**
 * Free memory allocated for the heap.
 */
void free_min_heap(MinHeap *heap) {
    if (heap) {
        free(heap->nodes);
        free(heap->position);
        free(heap);
    }
}

/**
 * Swap two heap nodes.
 */
void swap_heap_nodes(HeapNode *a, HeapNode *b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Heapify at given index (restore min-heap property).
 */
void min_heapify(MinHeap *heap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    
    if (left < heap->size && 
        heap->nodes[left].distance < heap->nodes[smallest].distance) {
        smallest = left;
    }
    
    if (right < heap->size && 
        heap->nodes[right].distance < heap->nodes[smallest].distance) {
        smallest = right;
    }
    
    if (smallest != idx) {
        /* Update positions */
        heap->position[heap->nodes[smallest].vertex] = idx;
        heap->position[heap->nodes[idx].vertex] = smallest;
        
        swap_heap_nodes(&heap->nodes[smallest], &heap->nodes[idx]);
        min_heapify(heap, smallest);
    }
}

/**
 * Check if heap is empty.
 */
bool is_heap_empty(MinHeap *heap) {
    return heap->size == 0;
}

/**
 * Extract minimum element from heap.
 */
HeapNode extract_min(MinHeap *heap) {
    if (is_heap_empty(heap)) {
        HeapNode empty = {-1, INF};
        return empty;
    }
    
    HeapNode root = heap->nodes[0];
    HeapNode last = heap->nodes[heap->size - 1];
    heap->nodes[0] = last;
    
    heap->position[root.vertex] = heap->size - 1;
    heap->position[last.vertex] = 0;
    
    heap->size--;
    min_heapify(heap, 0);
    
    return root;
}

/**
 * Decrease key value (distance) for a vertex.
 */
void decrease_key(MinHeap *heap, int vertex, int distance) {
    int i = heap->position[vertex];
    heap->nodes[i].distance = distance;
    
    /* Bubble up to maintain heap property */
    while (i > 0 && heap->nodes[i].distance < heap->nodes[(i - 1) / 2].distance) {
        heap->position[heap->nodes[i].vertex] = (i - 1) / 2;
        heap->position[heap->nodes[(i - 1) / 2].vertex] = i;
        swap_heap_nodes(&heap->nodes[i], &heap->nodes[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

/**
 * Check if vertex is in the heap.
 */
bool is_in_heap(MinHeap *heap, int vertex) {
    return heap->position[vertex] < heap->size;
}

/* =============================================================================
 * PART 3: DIJKSTRA'S ALGORITHM - ARRAY IMPLEMENTATION O(V²)
 * =============================================================================
 */

void demo_dijkstra_array(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 3: DIJKSTRA'S ALGORITHM - O(V²) ARRAY               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Create example graph */
    int V = 6;
    Graph *g = create_graph(V);
    
    /* Add edges (directed weighted graph) */
    add_edge(g, 0, 1, 4);
    add_edge(g, 0, 2, 2);
    add_edge(g, 1, 2, 1);
    add_edge(g, 1, 3, 5);
    add_edge(g, 2, 3, 8);
    add_edge(g, 2, 4, 10);
    add_edge(g, 3, 4, 2);
    add_edge(g, 3, 5, 6);
    add_edge(g, 4, 5, 3);
    
    printf("  Graph structure:\n");
    printf("         (4)\n");
    printf("    0 ─────────► 1\n");
    printf("    │            │\n");
    printf("  (2)│         (1)│(5)\n");
    printf("    │            │\n");
    printf("    ▼   (8)      ▼   (6)\n");
    printf("    2 ─────────► 3 ─────────► 5\n");
    printf("    │            │            ▲\n");
    printf(" (10)│         (2)│          (3)│\n");
    printf("    │            │            │\n");
    printf("    └────────────► 4 ─────────┘\n\n");
    
    int src = 0;
    int dist[MAX_VERTICES];
    int parent[MAX_VERTICES];
    bool visited[MAX_VERTICES];
    
    /* Initialisation */
    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        parent[i] = -1;
        visited[i] = false;
    }
    dist[src] = 0;
    
    printf("  Running Dijkstra from vertex %d:\n", src);
    printf("  ─────────────────────────────────\n");
    
    /* Main loop: V iterations */
    for (int count = 0; count < V; count++) {
        /* Find minimum distance vertex not yet processed */
        int u = -1;
        int min_dist = INF;
        
        for (int v = 0; v < V; v++) {
            if (!visited[v] && dist[v] < min_dist) {
                min_dist = dist[v];
                u = v;
            }
        }
        
        if (u == -1) break;  /* All remaining vertices unreachable */
        
        visited[u] = true;
        printf("  Step %d: Select vertex %d (distance = %d)\n", count + 1, u, dist[u]);
        
        /* Relaxation: Update distances to adjacent vertices */
        for (int v = 0; v < V; v++) {
            if (!visited[v] && 
                g->adj_matrix[u][v] != INF &&
                dist[u] != INF &&
                dist[u] + g->adj_matrix[u][v] < dist[v]) {
                
                dist[v] = dist[u] + g->adj_matrix[u][v];
                parent[v] = u;
                printf("    └─ Relax edge (%d,%d): dist[%d] = %d\n", u, v, v, dist[v]);
            }
        }
    }
    
    print_solution(dist, parent, V, src);
    free_graph(g);
}

/* =============================================================================
 * PART 4: DIJKSTRA'S ALGORITHM - HEAP IMPLEMENTATION O((V+E) log V)
 * =============================================================================
 */

void demo_dijkstra_heap(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 4: DIJKSTRA'S ALGORITHM - O((V+E) log V) HEAP       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int V = 6;
    Graph *g = create_graph(V);
    
    /* Same graph as before */
    add_edge(g, 0, 1, 4);
    add_edge(g, 0, 2, 2);
    add_edge(g, 1, 2, 1);
    add_edge(g, 1, 3, 5);
    add_edge(g, 2, 3, 8);
    add_edge(g, 2, 4, 10);
    add_edge(g, 3, 4, 2);
    add_edge(g, 3, 5, 6);
    add_edge(g, 4, 5, 3);
    
    int src = 0;
    int dist[MAX_VERTICES];
    int parent[MAX_VERTICES];
    
    /* Create min-heap */
    MinHeap *heap = create_min_heap(V);
    
    /* Initialisation */
    for (int v = 0; v < V; v++) {
        dist[v] = INF;
        parent[v] = -1;
        heap->nodes[v].vertex = v;
        heap->nodes[v].distance = INF;
        heap->position[v] = v;
    }
    
    dist[src] = 0;
    heap->nodes[src].distance = 0;
    heap->size = V;
    
    /* Move source to front */
    decrease_key(heap, src, 0);
    
    printf("  Using binary min-heap for efficient extraction:\n");
    printf("  ─────────────────────────────────────────────────\n");
    
    int step = 0;
    while (!is_heap_empty(heap)) {
        HeapNode min_node = extract_min(heap);
        int u = min_node.vertex;
        
        if (dist[u] == INF) break;
        
        step++;
        printf("  Step %d: Extract vertex %d (distance = %d)\n", step, u, dist[u]);
        
        /* Traverse adjacency list */
        Edge *edge = g->adj_list[u];
        while (edge) {
            int v = edge->dest;
            
            if (is_in_heap(heap, v) && 
                dist[u] != INF &&
                dist[u] + edge->weight < dist[v]) {
                
                dist[v] = dist[u] + edge->weight;
                parent[v] = u;
                decrease_key(heap, v, dist[v]);
                printf("    └─ Decrease-key for vertex %d: new distance = %d\n", v, dist[v]);
            }
            edge = edge->next;
        }
    }
    
    print_solution(dist, parent, V, src);
    
    free_min_heap(heap);
    free_graph(g);
}

/* =============================================================================
 * PART 5: BELLMAN-FORD ALGORITHM O(VE)
 * =============================================================================
 */

void demo_bellman_ford(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 5: BELLMAN-FORD ALGORITHM O(VE)                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Create graph with negative edge weights */
    int V = 5;
    Graph *g = create_graph(V);
    
    add_edge(g, 0, 1, 6);
    add_edge(g, 0, 2, 7);
    add_edge(g, 1, 2, 8);
    add_edge(g, 1, 3, 5);
    add_edge(g, 1, 4, -4);  /* Negative weight! */
    add_edge(g, 2, 3, -3);  /* Negative weight! */
    add_edge(g, 2, 4, 9);
    add_edge(g, 3, 1, -2);  /* Negative weight! */
    add_edge(g, 4, 0, 2);
    add_edge(g, 4, 3, 7);
    
    printf("  Graph with negative edge weights:\n");
    printf("  (Dijkstra would fail here!)\n\n");
    printf("         (6)         (-4)\n");
    printf("    0 ─────────► 1 ─────────► 4\n");
    printf("    │ ▲         │ ▲          │\n");
    printf("  (7)│ │(2)   (8)│ │(-2)   (9)│(7)\n");
    printf("    │ │         │ │          │\n");
    printf("    ▼ │         ▼ │   (-3)   ▼\n");
    printf("    2 ─────────────────────► 3\n\n");
    
    int src = 0;
    int dist[MAX_VERTICES];
    int parent[MAX_VERTICES];
    
    /* Initialisation */
    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }
    dist[src] = 0;
    
    printf("  Running Bellman-Ford from vertex %d:\n", src);
    printf("  ─────────────────────────────────────\n");
    
    /* Main loop: V-1 iterations */
    for (int i = 0; i < V - 1; i++) {
        printf("  Iteration %d:\n", i + 1);
        bool changed = false;
        
        /* Relax all edges */
        for (int u = 0; u < V; u++) {
            Edge *edge = g->adj_list[u];
            while (edge) {
                int v = edge->dest;
                int weight = edge->weight;
                
                if (dist[u] != INF && dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    parent[v] = u;
                    printf("    Relax (%d -> %d, w=%d): dist[%d] = %d\n", 
                           u, v, weight, v, dist[v]);
                    changed = true;
                }
                edge = edge->next;
            }
        }
        
        if (!changed) {
            printf("    No changes - early termination possible\n");
            break;
        }
    }
    
    /* Check for negative cycles */
    printf("\n  Checking for negative cycles (iteration V):\n");
    bool has_negative_cycle = false;
    
    for (int u = 0; u < V; u++) {
        Edge *edge = g->adj_list[u];
        while (edge) {
            int v = edge->dest;
            int weight = edge->weight;
            
            if (dist[u] != INF && dist[u] + weight < dist[v]) {
                printf("    ⚠ Negative cycle detected via edge (%d -> %d)!\n", u, v);
                has_negative_cycle = true;
            }
            edge = edge->next;
        }
    }
    
    if (!has_negative_cycle) {
        printf("    ✓ No negative cycles found\n");
        print_solution(dist, parent, V, src);
    } else {
        printf("\n  ⚠ Shortest paths undefined due to negative cycle!\n");
    }
    
    free_graph(g);
}

/* =============================================================================
 * PART 6: NEGATIVE CYCLE DETECTION EXAMPLE
 * =============================================================================
 */

void demo_negative_cycle(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 6: NEGATIVE CYCLE DETECTION                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Create graph WITH a negative cycle */
    int V = 4;
    Graph *g = create_graph(V);
    
    add_edge(g, 0, 1, 1);
    add_edge(g, 1, 2, -1);
    add_edge(g, 2, 3, -1);
    add_edge(g, 3, 1, -1);  /* Creates cycle: 1 -> 2 -> 3 -> 1 with total weight -3 */
    
    printf("  Graph with NEGATIVE CYCLE:\n\n");
    printf("    0 ──(1)──► 1 ◄─────────┐\n");
    printf("               │           │\n");
    printf("             (-1)        (-1)\n");
    printf("               │           │\n");
    printf("               ▼           │\n");
    printf("               2 ──(-1)──► 3\n\n");
    printf("  Cycle: 1 → 2 → 3 → 1 with total weight = -1 + (-1) + (-1) = -3\n\n");
    
    int src = 0;
    int dist[MAX_VERTICES];
    
    for (int i = 0; i < V; i++) {
        dist[i] = INF;
    }
    dist[src] = 0;
    
    /* V-1 iterations of relaxation */
    for (int i = 0; i < V - 1; i++) {
        for (int u = 0; u < V; u++) {
            Edge *edge = g->adj_list[u];
            while (edge) {
                if (dist[u] != INF && dist[u] + edge->weight < dist[edge->dest]) {
                    dist[edge->dest] = dist[u] + edge->weight;
                }
                edge = edge->next;
            }
        }
    }
    
    /* Detect negative cycle */
    printf("  After V-1 iterations, checking for improvements:\n");
    for (int u = 0; u < V; u++) {
        Edge *edge = g->adj_list[u];
        while (edge) {
            if (dist[u] != INF && dist[u] + edge->weight < dist[edge->dest]) {
                printf("  ⚠ NEGATIVE CYCLE DETECTED!\n");
                printf("    Edge (%d → %d) can still improve distance.\n", u, edge->dest);
                printf("    This means we can keep reducing distances forever.\n");
                free_graph(g);
                return;
            }
            edge = edge->next;
        }
    }
    
    printf("  ✓ No negative cycle found.\n");
    free_graph(g);
}

/* =============================================================================
 * PART 7: ALGORITHM COMPARISON
 * =============================================================================
 */

void demo_comparison(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 7: ALGORITHM COMPARISON                             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("  ┌───────────────────────────────────────────────────────────────┐\n");
    printf("  │              Algorithm Comparison Summary                      │\n");
    printf("  ├──────────────────┬───────────────┬───────────────┬────────────┤\n");
    printf("  │ Property         │ Dijkstra O(V²)│ Dijkstra Heap │ Bellman-Ford│\n");
    printf("  ├──────────────────┼───────────────┼───────────────┼────────────┤\n");
    printf("  │ Time Complexity  │    O(V²)      │ O((V+E)log V) │   O(VE)    │\n");
    printf("  │ Space Complexity │    O(V)       │    O(V)       │   O(V)     │\n");
    printf("  │ Negative Weights │     No        │     No        │   Yes      │\n");
    printf("  │ Negative Cycles  │     No        │     No        │  Detects   │\n");
    printf("  │ Best For         │ Dense graphs  │ Sparse graphs │ Gen. graphs│\n");
    printf("  └──────────────────┴───────────────┴───────────────┴────────────┘\n\n");
    
    printf("  When to use each:\n");
    printf("  ─────────────────\n");
    printf("  • Dijkstra O(V²)  : Small dense graphs (E ≈ V²)\n");
    printf("  • Dijkstra Heap   : Large sparse graphs (E << V²), no negative weights\n");
    printf("  • Bellman-Ford    : Graphs with negative weights or cycle detection\n\n");
    
    printf("  Real-world applications:\n");
    printf("  ────────────────────────\n");
    printf("  • OSPF routing    → Dijkstra (non-negative link costs)\n");
    printf("  • GPS navigation  → Dijkstra with A* heuristic\n");
    printf("  • Currency arbitrage → Bellman-Ford (detect negative cycles)\n");
    printf("  • Network delay   → Bellman-Ford (can have negative adjustments)\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     WEEK 13: GRAPH ALGORITHMS - SHORTEST PATH                 ║\n");
    printf("║                Complete Example                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n  This demonstration covers:\n");
    printf("  ─────────────────────────\n");
    printf("  1. Graph creation and representation\n");
    printf("  2. Min-heap operations for priority queue\n");
    printf("  3. Dijkstra's algorithm - O(V²) array implementation\n");
    printf("  4. Dijkstra's algorithm - O((V+E) log V) heap implementation\n");
    printf("  5. Bellman-Ford algorithm with negative weights\n");
    printf("  6. Negative cycle detection\n");
    printf("  7. Algorithm comparison and selection\n");
    
    demo_dijkstra_array();
    demo_dijkstra_heap();
    demo_bellman_ford();
    demo_negative_cycle();
    demo_comparison();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                   End of Demonstration                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
