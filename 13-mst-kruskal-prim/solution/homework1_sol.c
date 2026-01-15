/**
 * =============================================================================
 * HOMEWORK 1 - SOLUTION: MULTI-SOURCE SHORTEST PATHS
 * =============================================================================
 *
 * This file contains the complete solution for Homework 1.
 * For instructor use only - do not distribute to students.
 *
 * Problem: Find shortest paths from multiple source vertices simultaneously.
 * Each vertex should know its distance to the nearest source and which
 * source that is.
 *
 * Algorithm: Modified Dijkstra that starts with all sources at distance 0.
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#define MAX_VERTICES 10000
#define INF INT_MAX

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct Edge {
    int dest;
    int weight;
    struct Edge *next;
} Edge;

typedef struct {
    int V;
    Edge **adj;
} Graph;

typedef struct {
    int vertex;
    int distance;
} HeapNode;

typedef struct {
    HeapNode *nodes;
    int *position;
    int size;
    int capacity;
} MinHeap;

/* =============================================================================
 * GRAPH FUNCTIONS
 * =============================================================================
 */

Graph *create_graph(int V) {
    Graph *g = (Graph *)malloc(sizeof(Graph));
    if (!g) return NULL;
    
    g->V = V;
    g->adj = (Edge **)calloc(V, sizeof(Edge *));
    if (!g->adj) {
        free(g);
        return NULL;
    }
    
    return g;
}

void add_edge(Graph *g, int u, int v, int weight) {
    Edge *edge = (Edge *)malloc(sizeof(Edge));
    if (!edge) return;
    
    edge->dest = v;
    edge->weight = weight;
    edge->next = g->adj[u];
    g->adj[u] = edge;
}

void free_graph(Graph *g) {
    if (!g) return;
    
    for (int i = 0; i < g->V; i++) {
        Edge *current = g->adj[i];
        while (current) {
            Edge *temp = current;
            current = current->next;
            free(temp);
        }
    }
    
    free(g->adj);
    free(g);
}

/* =============================================================================
 * MIN-HEAP FUNCTIONS
 * =============================================================================
 */

MinHeap *create_min_heap(int capacity) {
    MinHeap *heap = (MinHeap *)malloc(sizeof(MinHeap));
    if (!heap) return NULL;
    
    heap->nodes = (HeapNode *)malloc(capacity * sizeof(HeapNode));
    heap->position = (int *)malloc(capacity * sizeof(int));
    
    if (!heap->nodes || !heap->position) {
        free(heap->nodes);
        free(heap->position);
        free(heap);
        return NULL;
    }
    
    heap->size = 0;
    heap->capacity = capacity;
    
    return heap;
}

void swap_nodes(HeapNode *a, HeapNode *b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

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
        heap->position[heap->nodes[smallest].vertex] = idx;
        heap->position[heap->nodes[idx].vertex] = smallest;
        swap_nodes(&heap->nodes[smallest], &heap->nodes[idx]);
        min_heapify(heap, smallest);
    }
}

bool is_empty(MinHeap *heap) {
    return heap->size == 0;
}

HeapNode extract_min(MinHeap *heap) {
    HeapNode empty = {-1, INF};
    if (is_empty(heap)) return empty;
    
    HeapNode min_node = heap->nodes[0];
    HeapNode last_node = heap->nodes[heap->size - 1];
    
    heap->nodes[0] = last_node;
    heap->position[last_node.vertex] = 0;
    heap->size--;
    
    if (heap->size > 0) {
        min_heapify(heap, 0);
    }
    
    return min_node;
}

void decrease_key(MinHeap *heap, int vertex, int distance) {
    int idx = heap->position[vertex];
    heap->nodes[idx].distance = distance;
    
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (heap->nodes[idx].distance < heap->nodes[parent].distance) {
            heap->position[heap->nodes[idx].vertex] = parent;
            heap->position[heap->nodes[parent].vertex] = idx;
            swap_nodes(&heap->nodes[idx], &heap->nodes[parent]);
            idx = parent;
        } else {
            break;
        }
    }
}

bool is_in_heap(MinHeap *heap, int vertex) {
    return heap->position[vertex] < heap->size;
}

void free_heap(MinHeap *heap) {
    if (heap) {
        free(heap->nodes);
        free(heap->position);
        free(heap);
    }
}

/* =============================================================================
 * MULTI-SOURCE DIJKSTRA
 * =============================================================================
 */

/**
 * Multi-source Dijkstra's algorithm
 * 
 * This variant initialises all source vertices with distance 0, then runs
 * standard Dijkstra. The result is that each vertex has the shortest distance
 * to ANY source.
 *
 * @param g             The graph
 * @param sources       Array of source vertex indices
 * @param num_sources   Number of sources
 * @param dist          Output: distance to nearest source
 * @param nearest_src   Output: which source is nearest
 */
void multi_source_dijkstra(Graph *g, int sources[], int num_sources,
                           int dist[], int nearest_src[]) {
    int V = g->V;
    
    MinHeap *heap = create_min_heap(V);
    if (!heap) {
        fprintf(stderr, "Error: Failed to create heap\n");
        return;
    }
    
    /* Initialise all distances to INF */
    for (int v = 0; v < V; v++) {
        dist[v] = INF;
        nearest_src[v] = -1;
        heap->nodes[v].vertex = v;
        heap->nodes[v].distance = INF;
        heap->position[v] = v;
    }
    heap->size = V;
    
    /* Set all source vertices to distance 0 */
    for (int i = 0; i < num_sources; i++) {
        int src = sources[i];
        dist[src] = 0;
        nearest_src[src] = src;
        heap->nodes[src].distance = 0;
    }
    
    /* Build min-heap (reorder to maintain heap property) */
    for (int i = V / 2 - 1; i >= 0; i--) {
        min_heapify(heap, i);
    }
    
    /* Update position array after building heap */
    for (int i = 0; i < V; i++) {
        heap->position[heap->nodes[i].vertex] = i;
    }
    
    /* Standard Dijkstra loop */
    while (!is_empty(heap)) {
        HeapNode min_node = extract_min(heap);
        int u = min_node.vertex;
        
        if (dist[u] == INF) break;
        
        /* Relax adjacent edges */
        Edge *edge = g->adj[u];
        while (edge) {
            int v = edge->dest;
            int weight = edge->weight;
            
            if (is_in_heap(heap, v) && dist[u] != INF &&
                dist[u] + weight < dist[v]) {
                
                dist[v] = dist[u] + weight;
                nearest_src[v] = nearest_src[u];  /* Inherit nearest source */
                decrease_key(heap, v, dist[v]);
            }
            
            edge = edge->next;
        }
    }
    
    free_heap(heap);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    int V, E, K;
    
    /* Read graph parameters */
    if (scanf("%d %d %d", &V, &E, &K) != 3) {
        fprintf(stderr, "Error: Invalid input format\n");
        return EXIT_FAILURE;
    }
    
    if (V <= 0 || V > MAX_VERTICES || E < 0 || K <= 0 || K > V) {
        fprintf(stderr, "Error: Invalid parameters\n");
        return EXIT_FAILURE;
    }
    
    /* Read source vertices */
    int *sources = (int *)malloc(K * sizeof(int));
    if (!sources) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return EXIT_FAILURE;
    }
    
    for (int i = 0; i < K; i++) {
        if (scanf("%d", &sources[i]) != 1) {
            fprintf(stderr, "Error: Failed to read source vertex\n");
            free(sources);
            return EXIT_FAILURE;
        }
        if (sources[i] < 0 || sources[i] >= V) {
            fprintf(stderr, "Error: Invalid source vertex\n");
            free(sources);
            return EXIT_FAILURE;
        }
    }
    
    /* Create graph */
    Graph *g = create_graph(V);
    if (!g) {
        fprintf(stderr, "Error: Failed to create graph\n");
        free(sources);
        return EXIT_FAILURE;
    }
    
    /* Read edges */
    for (int i = 0; i < E; i++) {
        int u, v, w;
        if (scanf("%d %d %d", &u, &v, &w) != 3) {
            fprintf(stderr, "Error: Invalid edge format\n");
            free_graph(g);
            free(sources);
            return EXIT_FAILURE;
        }
        if (u < 0 || u >= V || v < 0 || v >= V || w < 0) {
            fprintf(stderr, "Error: Invalid edge parameters\n");
            free_graph(g);
            free(sources);
            return EXIT_FAILURE;
        }
        add_edge(g, u, v, w);
    }
    
    /* Allocate result arrays */
    int *dist = (int *)malloc(V * sizeof(int));
    int *nearest_src = (int *)malloc(V * sizeof(int));
    
    if (!dist || !nearest_src) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free_graph(g);
        free(sources);
        free(dist);
        free(nearest_src);
        return EXIT_FAILURE;
    }
    
    /* Run multi-source Dijkstra */
    multi_source_dijkstra(g, sources, K, dist, nearest_src);
    
    /* Print results */
    for (int v = 0; v < V; v++) {
        if (dist[v] == INF) {
            printf("Vertex %d: unreachable\n", v);
        } else {
            printf("Vertex %d: distance = %d, nearest source = %d\n",
                   v, dist[v], nearest_src[v]);
        }
    }
    
    /* Cleanup */
    free(dist);
    free(nearest_src);
    free(sources);
    free_graph(g);
    
    return EXIT_SUCCESS;
}

/* =============================================================================
 * END OF SOLUTION
 * =============================================================================
 */
