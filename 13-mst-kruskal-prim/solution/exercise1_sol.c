/**
 * =============================================================================
 * EXERCISE 1 - SOLUTION: DIJKSTRA'S ALGORITHM WITH PATH RECONSTRUCTION
 * =============================================================================
 *
 * This file contains the complete solution for Exercise 1.
 * For instructor use only - do not distribute to students.
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#define MAX_VERTICES 1000
#define INF INT_MAX

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * SOLUTION TODO 1: Edge structure for adjacency list representation
 */
typedef struct Edge {
    int dest;
    int weight;
    struct Edge *next;
} Edge;

/**
 * SOLUTION TODO 2: Graph structure
 */
typedef struct {
    int V;
    Edge **adj;
} Graph;

/**
 * HeapNode for the priority queue
 */
typedef struct {
    int vertex;
    int distance;
} HeapNode;

/**
 * Min-heap priority queue structure
 */
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

/**
 * SOLUTION TODO 3: Create a new graph with V vertices
 */
Graph *create_graph(int V) {
    Graph *g = (Graph *)malloc(sizeof(Graph));
    if (!g) {
        return NULL;
    }
    
    g->V = V;
    g->adj = (Edge **)calloc(V, sizeof(Edge *));
    
    if (!g->adj) {
        free(g);
        return NULL;
    }
    
    /* All adjacency lists initialised to NULL by calloc */
    return g;
}

/**
 * SOLUTION TODO 4: Add a directed edge from u to v with given weight
 */
void add_edge(Graph *g, int u, int v, int weight) {
    if (!g || u < 0 || u >= g->V || v < 0 || v >= g->V) {
        return;
    }
    
    /* Create new edge */
    Edge *edge = (Edge *)malloc(sizeof(Edge));
    if (!edge) {
        return;
    }
    
    edge->dest = v;
    edge->weight = weight;
    
    /* Prepend to adjacency list of u */
    edge->next = g->adj[u];
    g->adj[u] = edge;
}

/**
 * Free all memory allocated for the graph
 */
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

/**
 * SOLUTION TODO 5: Create a new min-heap with given capacity
 */
MinHeap *create_min_heap(int capacity) {
    MinHeap *heap = (MinHeap *)malloc(sizeof(MinHeap));
    if (!heap) {
        return NULL;
    }
    
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

/**
 * Swap two heap nodes
 */
void swap_nodes(HeapNode *a, HeapNode *b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * SOLUTION TODO 6: Restore min-heap property by sinking down from idx
 */
void min_heapify(MinHeap *heap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    
    /* Check if left child is smaller */
    if (left < heap->size && 
        heap->nodes[left].distance < heap->nodes[smallest].distance) {
        smallest = left;
    }
    
    /* Check if right child is smaller */
    if (right < heap->size && 
        heap->nodes[right].distance < heap->nodes[smallest].distance) {
        smallest = right;
    }
    
    /* If smallest is not the current node, swap and recurse */
    if (smallest != idx) {
        /* Update position array */
        heap->position[heap->nodes[smallest].vertex] = idx;
        heap->position[heap->nodes[idx].vertex] = smallest;
        
        /* Swap nodes */
        swap_nodes(&heap->nodes[smallest], &heap->nodes[idx]);
        
        /* Recursively heapify */
        min_heapify(heap, smallest);
    }
}

/**
 * Check if heap is empty
 */
bool is_empty(MinHeap *heap) {
    return heap->size == 0;
}

/**
 * SOLUTION TODO 7: Extract and return the minimum element
 */
HeapNode extract_min(MinHeap *heap) {
    HeapNode empty = {-1, INF};
    
    if (is_empty(heap)) {
        return empty;
    }
    
    /* Store the root (minimum) */
    HeapNode min_node = heap->nodes[0];
    
    /* Replace root with last element */
    HeapNode last_node = heap->nodes[heap->size - 1];
    heap->nodes[0] = last_node;
    
    /* Update position of moved node */
    heap->position[last_node.vertex] = 0;
    
    /* Decrease size */
    heap->size--;
    
    /* Heapify from root if heap is not empty */
    if (heap->size > 0) {
        min_heapify(heap, 0);
    }
    
    return min_node;
}

/**
 * SOLUTION TODO 8: Decrease the key (distance) of a vertex and bubble up
 */
void decrease_key(MinHeap *heap, int vertex, int distance) {
    /* Get index of vertex in heap */
    int idx = heap->position[vertex];
    
    /* Update distance */
    heap->nodes[idx].distance = distance;
    
    /* Bubble up while node is smaller than parent */
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        
        if (heap->nodes[idx].distance < heap->nodes[parent].distance) {
            /* Update positions */
            heap->position[heap->nodes[idx].vertex] = parent;
            heap->position[heap->nodes[parent].vertex] = idx;
            
            /* Swap with parent */
            swap_nodes(&heap->nodes[idx], &heap->nodes[parent]);
            
            /* Move to parent */
            idx = parent;
        } else {
            break;
        }
    }
}

/**
 * Check if vertex is in heap
 */
bool is_in_heap(MinHeap *heap, int vertex) {
    return heap->position[vertex] < heap->size;
}

/**
 * Free heap memory
 */
void free_heap(MinHeap *heap) {
    if (heap) {
        free(heap->nodes);
        free(heap->position);
        free(heap);
    }
}

/* =============================================================================
 * PATH RECONSTRUCTION
 * =============================================================================
 */

/**
 * Print path recursively from source to vertex v
 */
void print_path_recursive(int parent[], int v, int source) {
    if (v == source) {
        printf("%d", v);
        return;
    }
    if (parent[v] == -1) {
        printf("(no path)");
        return;
    }
    print_path_recursive(parent, parent[v], source);
    printf(" -> %d", v);
}

/* =============================================================================
 * DIJKSTRA'S ALGORITHM
 * =============================================================================
 */

/**
 * SOLUTION TODO 9: Dijkstra's algorithm using min-heap priority queue
 * 
 * Time complexity: O((V + E) log V)
 * Space complexity: O(V)
 */
void dijkstra(Graph *g, int source, int dist[], int parent[]) {
    int V = g->V;
    
    /* Create min-heap */
    MinHeap *heap = create_min_heap(V);
    if (!heap) {
        fprintf(stderr, "Error: Failed to create heap\n");
        return;
    }
    
    /* Initialise distances and parents */
    for (int v = 0; v < V; v++) {
        dist[v] = INF;
        parent[v] = -1;
        
        /* Add all vertices to heap */
        heap->nodes[v].vertex = v;
        heap->nodes[v].distance = INF;
        heap->position[v] = v;
    }
    
    /* Set source distance to 0 */
    dist[source] = 0;
    heap->nodes[source].distance = 0;
    heap->size = V;
    
    /* Move source to front of heap */
    heap->position[source] = 0;
    heap->position[0] = source;
    swap_nodes(&heap->nodes[0], &heap->nodes[source]);
    
    /* Process all vertices */
    while (!is_empty(heap)) {
        /* Extract vertex with minimum distance */
        HeapNode min_node = extract_min(heap);
        int u = min_node.vertex;
        
        /* If minimum distance is INF, remaining vertices are unreachable */
        if (dist[u] == INF) {
            break;
        }
        
        /* Relax all adjacent edges */
        Edge *edge = g->adj[u];
        while (edge) {
            int v = edge->dest;
            int weight = edge->weight;
            
            /* Relaxation step */
            if (is_in_heap(heap, v) && dist[u] != INF &&
                dist[u] + weight < dist[v]) {
                
                /* Update distance and parent */
                dist[v] = dist[u] + weight;
                parent[v] = u;
                
                /* Decrease key in heap */
                decrease_key(heap, v, dist[v]);
            }
            
            edge = edge->next;
        }
    }
    
    /* Cleanup */
    free_heap(heap);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    int V, E, source;
    
    /* Read number of vertices, edges and source */
    if (scanf("%d %d %d", &V, &E, &source) != 3) {
        fprintf(stderr, "Error: Invalid input format\n");
        return EXIT_FAILURE;
    }
    
    if (V <= 0 || V > MAX_VERTICES || E < 0 || source < 0 || source >= V) {
        fprintf(stderr, "Error: Invalid graph parameters\n");
        return EXIT_FAILURE;
    }
    
    /* Create graph */
    Graph *g = create_graph(V);
    if (!g) {
        fprintf(stderr, "Error: Failed to create graph\n");
        return EXIT_FAILURE;
    }
    
    /* Read edges */
    for (int i = 0; i < E; i++) {
        int u, v, w;
        if (scanf("%d %d %d", &u, &v, &w) != 3) {
            fprintf(stderr, "Error: Invalid edge format\n");
            free_graph(g);
            return EXIT_FAILURE;
        }
        if (u < 0 || u >= V || v < 0 || v >= V) {
            fprintf(stderr, "Error: Invalid vertex in edge\n");
            free_graph(g);
            return EXIT_FAILURE;
        }
        add_edge(g, u, v, w);
    }
    
    /* Allocate arrays for results */
    int *dist = (int *)malloc(V * sizeof(int));
    int *parent = (int *)malloc(V * sizeof(int));
    
    if (!dist || !parent) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free_graph(g);
        free(dist);
        free(parent);
        return EXIT_FAILURE;
    }
    
    /**
     * SOLUTION TODO 10: Run Dijkstra and print results
     */
    dijkstra(g, source, dist, parent);
    
    /* Print results for each vertex */
    for (int v = 0; v < V; v++) {
        if (dist[v] == INF) {
            printf("Vertex %d: unreachable\n", v);
        } else {
            printf("Vertex %d: distance = %d, path: ", v, dist[v]);
            print_path_recursive(parent, v, source);
            printf("\n");
        }
    }
    
    /* Cleanup */
    free(dist);
    free(parent);
    free_graph(g);
    
    return 0;
}

/* =============================================================================
 * END OF SOLUTION
 * =============================================================================
 */
