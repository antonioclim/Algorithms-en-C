/**
 * =============================================================================
 * EXERCISE 1: DIJKSTRA'S ALGORITHM WITH PATH RECONSTRUCTION
 * =============================================================================
 *
 * OBJECTIVE:
 *   Implement Dijkstra's shortest path algorithm using a priority queue
 *   (binary min-heap) to find shortest paths from a source vertex to all
 *   other vertices in a weighted directed graph.
 *
 * REQUIREMENTS:
 *   1. Implement the graph data structure using adjacency list
 *   2. Implement a min-heap priority queue
 *   3. Implement Dijkstra's algorithm
 *   4. Reconstruct and print the shortest paths
 *
 * INPUT FORMAT:
 *   Line 1: V E S (vertices, edges, source vertex)
 *   Lines 2 to E+1: u v w (edge from u to v with weight w)
 *
 * OUTPUT FORMAT:
 *   For each vertex v from 0 to V-1:
 *   "Vertex v: distance = d, path: source -> ... -> v"
 *   If unreachable: "Vertex v: unreachable"
 *
 * EXAMPLE INPUT:
 *   5 7 0
 *   0 1 4
 *   0 2 2
 *   1 2 1
 *   1 3 5
 *   2 3 8
 *   2 4 10
 *   3 4 2
 *
 * EXPECTED OUTPUT:
 *   Vertex 0: distance = 0, path: 0
 *   Vertex 1: distance = 4, path: 0 -> 1
 *   Vertex 2: distance = 2, path: 0 -> 2
 *   Vertex 3: distance = 9, path: 0 -> 1 -> 3
 *   Vertex 4: distance = 11, path: 0 -> 1 -> 3 -> 4
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
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
 * TODO 1: Define the Edge structure for adjacency list representation
 *
 * An edge should contain:
 *   - dest: the destination vertex (int)
 *   - weight: the edge weight (int)
 *   - next: pointer to the next edge in the list
 *
 * Hint: This is a standard linked list node for storing edges
 */

/* YOUR CODE HERE */
typedef struct Edge {
    int dest;
    int weight;
    struct Edge *next;  /* Replace with actual definition */
} Edge;

/**
 * TODO 2: Define the Graph structure
 *
 * The graph should contain:
 *   - V: number of vertices
 *   - adj: array of pointers to Edge (adjacency list heads)
 *
 * Hint: adj[i] points to the first edge from vertex i
 */

/* YOUR CODE HERE */
typedef struct {
    int V;
    Edge **adj;  /* Replace with actual definition */
} Graph;

/**
 * HeapNode for the priority queue
 * (Already implemented for you)
 */
typedef struct {
    int vertex;
    int distance;
} HeapNode;

/**
 * Min-heap priority queue structure
 * (Already implemented for you)
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
 * TODO 3: Implement create_graph
 *
 * @param V Number of vertices
 * @return Pointer to newly created graph
 *
 * Steps:
 *   1. Allocate memory for the Graph structure
 *   2. Set V to the number of vertices
 *   3. Allocate memory for the adjacency list array (V pointers)
 *   4. Initialise all adjacency list heads to NULL
 *   5. Return the graph pointer
 *
 * Hint: Use malloc for memory allocation
 */
Graph *create_graph(int V) {
    if (V <= 0) {
        return NULL;
    }

    Graph *g = (Graph *)malloc(sizeof(Graph));
    if (!g) {
        return NULL;
    }

    g->V = V;
    g->adj = (Edge **)calloc((size_t)V, sizeof(Edge *));
    if (!g->adj) {
        free(g);
        return NULL;
    }

    return g;

}

/**
 * TODO 4: Implement add_edge
 *
 * @param g Pointer to the graph
 * @param u Source vertex
 * @param v Destination vertex
 * @param weight Edge weight
 *
 * Steps:
 *   1. Allocate memory for a new Edge
 *   2. Set the destination and weight
 *   3. Insert at the head of adj[u] (prepend to list)
 *
 * Hint: New edge's next should point to current adj[u]
 */
void add_edge(Graph *g, int u, int v, int weight) {
    if (!g) {
        return;
    }
    if (u < 0 || u >= g->V || v < 0 || v >= g->V) {
        return;
    }

    Edge *e = (Edge *)malloc(sizeof(Edge));
    if (!e) {
        return;
    }

    e->dest = v;
    e->weight = weight;
    e->next = g->adj[u];
    g->adj[u] = e;

}

/**
 * Free all memory allocated for the graph
 * (Already implemented for you)
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
 * TODO 5: Implement create_min_heap
 *
 * @param capacity Maximum number of elements
 * @return Pointer to newly created min-heap
 *
 * Steps:
 *   1. Allocate memory for MinHeap structure
 *   2. Allocate memory for nodes array (capacity elements)
 *   3. Allocate memory for position array (capacity elements)
 *   4. Initialise size to 0 and capacity
 *   5. Return the heap pointer
 */
MinHeap *create_min_heap(int capacity) {
    if (capacity <= 0) {
        return NULL;
    }

    MinHeap *heap = (MinHeap *)malloc(sizeof(MinHeap));
    if (!heap) {
        return NULL;
    }

    heap->nodes = (HeapNode *)malloc((size_t)capacity * sizeof(HeapNode));
    heap->position = (int *)malloc((size_t)capacity * sizeof(int));
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
 * (Already implemented for you)
 */
void swap_nodes(HeapNode *a, HeapNode *b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * TODO 6: Implement min_heapify
 *
 * Restore min-heap property at index idx by sinking down.
 *
 * @param heap Pointer to the heap
 * @param idx Index to heapify from
 *
 * Steps:
 *   1. Calculate left child (2*idx + 1) and right child (2*idx + 2)
 *   2. Find the smallest among idx, left, right
 *   3. If smallest is not idx:
 *      a. Update position array for both vertices
 *      b. Swap the nodes
 *      c. Recursively heapify at the new position
 *
 * Hint: Compare distances to find minimum
 */
void min_heapify(MinHeap *heap, int idx) {
    if (!heap) {
        return;
    }

    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < heap->size && heap->nodes[left].distance < heap->nodes[smallest].distance) {
        smallest = left;
    }
    if (right < heap->size && heap->nodes[right].distance < heap->nodes[smallest].distance) {
        smallest = right;
    }

    if (smallest != idx) {
        HeapNode smallest_node = heap->nodes[smallest];
        HeapNode idx_node = heap->nodes[idx];

        heap->position[smallest_node.vertex] = idx;
        heap->position[idx_node.vertex] = smallest;

        swap_nodes(&heap->nodes[smallest], &heap->nodes[idx]);
        min_heapify(heap, smallest);
    }

}

/**
 * Check if heap is empty
 * (Already implemented for you)
 */
bool is_empty(MinHeap *heap) {
    return heap->size == 0;
}

/**
 * TODO 7: Implement extract_min
 *
 * Remove and return the minimum element from the heap.
 *
 * @param heap Pointer to the heap
 * @return The minimum HeapNode (smallest distance)
 *
 * Steps:
 *   1. Handle empty heap case
 *   2. Store the root (minimum)
 *   3. Replace root with last element
 *   4. Update position array
 *   5. Decrease size
 *   6. Heapify from root
 *   7. Return the stored minimum
 */
HeapNode extract_min(MinHeap *heap) {
    HeapNode empty = {-1, INF};

    if (!heap || heap->size <= 0) {
        return empty;
    }

    HeapNode root = heap->nodes[0];
    HeapNode last = heap->nodes[heap->size - 1];

    heap->nodes[0] = last;

    heap->position[last.vertex] = 0;

    heap->size--;

    /* Mark extracted vertex as outside the heap */
    heap->position[root.vertex] = heap->size;

    min_heapify(heap, 0);

    return root;

}

/**
 * TODO 8: Implement decrease_key
 *
 * Decrease the distance value of a vertex and bubble up.
 *
 * @param heap Pointer to the heap
 * @param vertex The vertex whose distance decreased
 * @param distance The new (smaller) distance
 *
 * Steps:
 *   1. Get the index of vertex in heap using position array
 *   2. Update the distance at that index
 *   3. Bubble up: while node's distance < parent's distance
 *      a. Update position array for both vertices
 *      b. Swap with parent
 *      c. Move to parent index
 *
 * Hint: Parent of index i is at (i-1)/2
 */
void decrease_key(MinHeap *heap, int vertex, int distance) {
    if (!heap) {
        return;
    }

    int i = heap->position[vertex];
    if (i < 0 || i >= heap->size) {
        return;
    }

    heap->nodes[i].distance = distance;

    while (i > 0) {
        int parent = (i - 1) / 2;
        if (heap->nodes[i].distance >= heap->nodes[parent].distance) {
            break;
        }

        heap->position[heap->nodes[i].vertex] = parent;
        heap->position[heap->nodes[parent].vertex] = i;

        swap_nodes(&heap->nodes[i], &heap->nodes[parent]);
        i = parent;
    }

}

/**
 * Check if vertex is in heap
 * (Already implemented for you)
 */
bool is_in_heap(MinHeap *heap, int vertex) {
    return heap->position[vertex] < heap->size;
}

/**
 * Free heap memory
 * (Already implemented for you)
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
 * Print path recursively
 * (Already implemented for you)
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
 * TODO 9: Implement dijkstra
 *
 * Find shortest paths from source to all other vertices.
 *
 * @param g Pointer to the graph
 * @param source Source vertex
 * @param dist Output array for distances
 * @param parent Output array for path reconstruction
 *
 * Steps:
 *   1. Create min-heap with capacity V
 *   2. Initialise dist[] to INF, parent[] to -1
 *   3. Set dist[source] = 0
 *   4. Add all vertices to heap with their distances
 *   5. While heap is not empty:
 *      a. Extract minimum distance vertex u
 *      b. For each edge (u, v) with weight w:
 *         - If v is in heap AND dist[u] + w < dist[v]:
 *           * Update dist[v] = dist[u] + w
 *           * Update parent[v] = u
 *           * Call decrease_key for v
 *   6. Free the heap
 *
 * Important: Check dist[u] != INF before relaxing
 */
void dijkstra(Graph *g, int source, int dist[], int parent[]) {
    if (!g || source < 0 || source >= g->V) {
        return;
    }

    MinHeap *heap = create_min_heap(g->V);
    if (!heap) {
        return;
    }

    for (int v = 0; v < g->V; v++) {
        dist[v] = INF;
        parent[v] = -1;
    }
    dist[source] = 0;

    heap->size = g->V;
    for (int v = 0; v < g->V; v++) {
        heap->nodes[v].vertex = v;
        heap->nodes[v].distance = dist[v];
        heap->position[v] = v;
    }

    /* Build heap bottom-up */
    for (int i = (heap->size - 1) / 2; i >= 0; i--) {
        min_heapify(heap, i);
        if (i == 0) {
            break;
        }
    }

    while (!is_empty(heap)) {
        HeapNode min_node = extract_min(heap);
        int u = min_node.vertex;

        if (u == -1 || min_node.distance == INF) {
            break;
        }

        for (Edge *e = g->adj[u]; e != NULL; e = e->next) {
            int v = e->dest;
            int w = e->weight;

            if (dist[u] != INF && is_in_heap(heap, v) && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                decrease_key(heap, v, dist[v]);
            }
        }
    }

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
    
    /* Arrays for results */
    int *dist = (int *)malloc(V * sizeof(int));
    int *parent = (int *)malloc(V * sizeof(int));
    
    if (!dist || !parent) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free_graph(g);
        free(dist);
        free(parent);
        return EXIT_FAILURE;
    }
    
    /* TODO 10: Call dijkstra and print results
     *
     * Steps:
     *   1. Call dijkstra(g, source, dist, parent)
     *   2. For each vertex v from 0 to V-1:
     *      - If dist[v] == INF: print "Vertex v: unreachable"
     *      - Else: print "Vertex v: distance = d, path: ..."
     *        Use print_path_recursive for the path
     */
    
    dijkstra(g, source, dist, parent);

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
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Modify the algorithm to return only the path to a specific destination
 *    vertex, not all vertices.
 *
 * 2. Implement bidirectional Dijkstra: run two searches simultaneously
 *    from source and destination, meeting in the middle.
 *
 * 3. Add support for reading the graph from a file instead of stdin.
 *
 * 4. Implement A* search with a heuristic for grid-based pathfinding.
 *
 * =============================================================================
 */
