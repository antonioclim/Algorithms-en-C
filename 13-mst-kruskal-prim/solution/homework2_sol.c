/**
 * =============================================================================
 * HOMEWORK 2 - SOLUTION: NETWORK DELAY TIME
 * =============================================================================
 *
 * This file contains the complete solution for Homework 2.
 * For instructor use only - do not distribute to students.
 *
 * Problem: Simulate network packet propagation and find:
 *   1. Time for signal to reach all computers
 *   2. Which computers are unreachable
 *   3. The critical path (slowest path)
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#define MAX_COMPUTERS 10000
#define INF INT_MAX

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct Edge {
    int dest;
    int delay;
    struct Edge *next;
} Edge;

typedef struct {
    int N;
    Edge **adj;
} Network;

typedef struct {
    int computer;
    int time;
} HeapNode;

typedef struct {
    HeapNode *nodes;
    int *position;
    int size;
    int capacity;
} MinHeap;

/* Arrival event for sorted output */
typedef struct {
    int computer;
    int arrival_time;
    int hops;
} ArrivalEvent;

/* =============================================================================
 * NETWORK FUNCTIONS
 * =============================================================================
 */

Network *create_network(int N) {
    Network *net = (Network *)malloc(sizeof(Network));
    if (!net) return NULL;
    
    net->N = N;
    net->adj = (Edge **)calloc(N, sizeof(Edge *));
    if (!net->adj) {
        free(net);
        return NULL;
    }
    
    return net;
}

void add_link(Network *net, int from, int to, int delay) {
    Edge *edge = (Edge *)malloc(sizeof(Edge));
    if (!edge) return;
    
    edge->dest = to;
    edge->delay = delay;
    edge->next = net->adj[from];
    net->adj[from] = edge;
}

void free_network(Network *net) {
    if (!net) return;
    
    for (int i = 0; i < net->N; i++) {
        Edge *current = net->adj[i];
        while (current) {
            Edge *temp = current;
            current = current->next;
            free(temp);
        }
    }
    
    free(net->adj);
    free(net);
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
        heap->nodes[left].time < heap->nodes[smallest].time) {
        smallest = left;
    }
    
    if (right < heap->size && 
        heap->nodes[right].time < heap->nodes[smallest].time) {
        smallest = right;
    }
    
    if (smallest != idx) {
        heap->position[heap->nodes[smallest].computer] = idx;
        heap->position[heap->nodes[idx].computer] = smallest;
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
    heap->position[last_node.computer] = 0;
    heap->size--;
    
    if (heap->size > 0) {
        min_heapify(heap, 0);
    }
    
    return min_node;
}

void decrease_key(MinHeap *heap, int computer, int time) {
    int idx = heap->position[computer];
    heap->nodes[idx].time = time;
    
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (heap->nodes[idx].time < heap->nodes[parent].time) {
            heap->position[heap->nodes[idx].computer] = parent;
            heap->position[heap->nodes[parent].computer] = idx;
            swap_nodes(&heap->nodes[idx], &heap->nodes[parent]);
            idx = parent;
        } else {
            break;
        }
    }
}

bool is_in_heap(MinHeap *heap, int computer) {
    return heap->position[computer] < heap->size;
}

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
 * Count hops in path from source to dest
 */
int count_hops(int parent[], int dest, int source) {
    int hops = 0;
    int current = dest;
    
    while (current != source && parent[current] != -1) {
        hops++;
        current = parent[current];
    }
    
    return hops;
}

/**
 * Print path from source to dest
 */
void print_path(int parent[], int dest, int source) {
    if (dest == source) {
        printf("%d", source);
        return;
    }
    
    if (parent[dest] == -1) {
        printf("(no path)");
        return;
    }
    
    print_path(parent, parent[dest], source);
    printf(" -> %d", dest);
}

/* =============================================================================
 * NETWORK DELAY ANALYSIS
 * =============================================================================
 */

/**
 * Comparison function for sorting arrivals by time
 */
int compare_arrivals(const void *a, const void *b) {
    const ArrivalEvent *ea = (const ArrivalEvent *)a;
    const ArrivalEvent *eb = (const ArrivalEvent *)b;
    return ea->arrival_time - eb->arrival_time;
}

/**
 * Compute network delay times using Dijkstra
 */
void compute_delay_times(Network *net, int source, 
                         int delay[], int parent[]) {
    int N = net->N;
    
    MinHeap *heap = create_min_heap(N);
    if (!heap) {
        fprintf(stderr, "Error: Failed to create heap\n");
        return;
    }
    
    /* Initialisation */
    for (int v = 0; v < N; v++) {
        delay[v] = INF;
        parent[v] = -1;
        heap->nodes[v].computer = v;
        heap->nodes[v].time = INF;
        heap->position[v] = v;
    }
    
    delay[source] = 0;
    heap->nodes[source].time = 0;
    heap->size = N;
    
    /* Move source to front */
    heap->position[source] = 0;
    heap->position[0] = source;
    swap_nodes(&heap->nodes[0], &heap->nodes[source]);
    
    /* Dijkstra loop */
    while (!is_empty(heap)) {
        HeapNode min_node = extract_min(heap);
        int u = min_node.computer;
        
        if (delay[u] == INF) break;
        
        Edge *edge = net->adj[u];
        while (edge) {
            int v = edge->dest;
            int d = edge->delay;
            
            if (is_in_heap(heap, v) && delay[u] != INF &&
                delay[u] + d < delay[v]) {
                
                delay[v] = delay[u] + d;
                parent[v] = u;
                decrease_key(heap, v, delay[v]);
            }
            
            edge = edge->next;
        }
    }
    
    free_heap(heap);
}

/**
 * Generate complete network delay report
 */
void generate_report(Network *net, int source, int delay[], int parent[]) {
    int N = net->N;
    
    printf("Network Delay Analysis\n");
    printf("======================\n");
    printf("Source: Computer %d\n", source);
    
    /* Find maximum delay and check reachability */
    int max_delay = 0;
    int critical_computer = source;
    int unreachable_count = 0;
    
    for (int v = 0; v < N; v++) {
        if (delay[v] == INF) {
            unreachable_count++;
        } else if (delay[v] > max_delay) {
            max_delay = delay[v];
            critical_computer = v;
        }
    }
    
    /* Print total propagation time */
    if (unreachable_count > 0) {
        printf("Total propagation time: -1 (not all computers reachable)\n");
    } else {
        printf("Total propagation time: %d ms\n", max_delay);
    }
    
    printf("\n");
    
    /* Create and sort arrival events */
    ArrivalEvent *arrivals = (ArrivalEvent *)malloc(N * sizeof(ArrivalEvent));
    if (!arrivals) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return;
    }
    
    for (int v = 0; v < N; v++) {
        arrivals[v].computer = v;
        arrivals[v].arrival_time = delay[v];
        arrivals[v].hops = (delay[v] == INF) ? -1 : count_hops(parent, v, source);
    }
    
    qsort(arrivals, N, sizeof(ArrivalEvent), compare_arrivals);
    
    /* Print arrival order */
    printf("Arrival order:\n");
    for (int i = 0; i < N; i++) {
        int comp = arrivals[i].computer;
        
        if (delay[comp] == INF) {
            printf("  Computer %d: unreachable\n", comp);
        } else if (comp == source) {
            printf("  Computer %d: %d ms (source)\n", comp, delay[comp]);
        } else {
            printf("  Computer %d: %d ms via ", comp, delay[comp]);
            print_path(parent, comp, source);
            printf("\n");
        }
    }
    
    printf("\n");
    
    /* Print critical path */
    if (unreachable_count == 0 && N > 1) {
        printf("Critical path: ");
        print_path(parent, critical_computer, source);
        printf(" (%d ms)\n", max_delay);
    } else if (unreachable_count > 0) {
        printf("Critical path: Cannot determine (some computers unreachable)\n");
    }
    
    /* Print reachability status */
    printf("All computers reachable: %s\n", 
           unreachable_count == 0 ? "YES" : "NO");
    
    if (unreachable_count > 0) {
        printf("Unreachable computers: ");
        bool first = true;
        for (int v = 0; v < N; v++) {
            if (delay[v] == INF) {
                if (!first) printf(", ");
                printf("%d", v);
                first = false;
            }
        }
        printf("\n");
    }
    
    free(arrivals);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    int N, M, S;
    
    /* Read network parameters */
    if (scanf("%d %d %d", &N, &M, &S) != 3) {
        fprintf(stderr, "Error: Invalid input format\n");
        return EXIT_FAILURE;
    }
    
    if (N <= 0 || N > MAX_COMPUTERS || M < 0 || S < 0 || S >= N) {
        fprintf(stderr, "Error: Invalid parameters\n");
        return EXIT_FAILURE;
    }
    
    /* Create network */
    Network *net = create_network(N);
    if (!net) {
        fprintf(stderr, "Error: Failed to create network\n");
        return EXIT_FAILURE;
    }
    
    /* Read links */
    for (int i = 0; i < M; i++) {
        int a, b, t;
        if (scanf("%d %d %d", &a, &b, &t) != 3) {
            fprintf(stderr, "Error: Invalid link format\n");
            free_network(net);
            return EXIT_FAILURE;
        }
        if (a < 0 || a >= N || b < 0 || b >= N || t < 0) {
            fprintf(stderr, "Error: Invalid link parameters\n");
            free_network(net);
            return EXIT_FAILURE;
        }
        add_link(net, a, b, t);
    }
    
    /* Allocate result arrays */
    int *delay = (int *)malloc(N * sizeof(int));
    int *parent = (int *)malloc(N * sizeof(int));
    
    if (!delay || !parent) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free_network(net);
        free(delay);
        free(parent);
        return EXIT_FAILURE;
    }
    
    /* Compute delays */
    compute_delay_times(net, S, delay, parent);
    
    /* Generate report */
    generate_report(net, S, delay, parent);
    
    /* Cleanup */
    free(delay);
    free(parent);
    free_network(net);
    
    return EXIT_SUCCESS;
}

/* =============================================================================
 * END OF SOLUTION
 * =============================================================================
 */
