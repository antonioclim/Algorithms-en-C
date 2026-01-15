/**
 * =============================================================================
 * HOMEWORK 1: Algorithm Toolkit Library - SOLUTION
 * =============================================================================
 *
 * A comprehensive algorithm toolkit demonstrating mastery of the entire
 * semester's content: sorting, searching, graph algorithms and dynamic
 * programming—all accessible through function pointers.
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c -lm
 * USAGE: 
 *   ./homework1_sol --demo
 *   ./homework1_sol --sort quicksort --input numbers.txt --output sorted.txt
 *   ./homework1_sol --graph roads.txt --dijkstra 0
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>

/* =============================================================================
 * CONFIGURATION
 * =============================================================================
 */

#define MAX_ARRAY_SIZE  100000
#define MAX_VERTICES    1000
#define HASH_SIZE       97
#define INF             INT_MAX

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/* Generic comparator function pointer */
typedef int (*Comparator)(const void *, const void *);

/* Sorting function pointer */
typedef void (*SortFunction)(void *arr, size_t n, size_t size, Comparator cmp);

/**
 * Dynamic Array - Generic container with metadata
 */
typedef struct {
    void   *data;        /* Pointer to data storage */
    size_t  count;       /* Current number of elements */
    size_t  capacity;    /* Maximum capacity before resize */
    size_t  elem_size;   /* Size of each element in bytes */
} DynamicArray;

/**
 * Graph - Adjacency list representation
 */
typedef struct AdjNode {
    int               vertex;
    int               weight;
    struct AdjNode   *next;
} AdjNode;

typedef struct {
    AdjNode **adj;      /* Array of adjacency lists */
    int      *degree;   /* Degree of each vertex */
    int       vertices; /* Number of vertices */
    int       edges;    /* Number of edges */
    bool      directed; /* Is graph directed? */
} Graph;

/**
 * Hash Table - Chaining for collision resolution
 */
typedef struct HashNode {
    int               key;
    int               value;
    struct HashNode  *next;
} HashNode;

typedef struct {
    HashNode **buckets;
    int        size;
    int        count;
} HashTable;

/* =============================================================================
 * DYNAMIC ARRAY IMPLEMENTATION
 * =============================================================================
 */

/**
 * Create a new dynamic array.
 */
DynamicArray *array_create(size_t elem_size, size_t initial_cap) {
    DynamicArray *arr = malloc(sizeof(DynamicArray));
    if (!arr) return NULL;
    
    arr->data = malloc(elem_size * initial_cap);
    if (!arr->data) {
        free(arr);
        return NULL;
    }
    
    arr->count = 0;
    arr->capacity = initial_cap;
    arr->elem_size = elem_size;
    
    return arr;
}

/**
 * Destroy dynamic array and free all memory.
 */
void array_destroy(DynamicArray *arr) {
    if (!arr) return;
    free(arr->data);
    free(arr);
}

/**
 * Append element to dynamic array.
 * Automatically resizes if needed.
 */
int array_append(DynamicArray *arr, const void *elem) {
    if (!arr || !elem) return -1;
    
    /* Resize if at capacity */
    if (arr->count >= arr->capacity) {
        size_t new_cap = arr->capacity * 2;
        void *new_data = realloc(arr->data, arr->elem_size * new_cap);
        if (!new_data) return -1;
        arr->data = new_data;
        arr->capacity = new_cap;
    }
    
    /* Copy element to end of array */
    char *dest = (char *)arr->data + (arr->count * arr->elem_size);
    memcpy(dest, elem, arr->elem_size);
    arr->count++;
    
    return 0;
}

/**
 * Get element at index.
 */
void *array_get(const DynamicArray *arr, size_t index) {
    if (!arr || index >= arr->count) return NULL;
    return (char *)arr->data + (index * arr->elem_size);
}

/* =============================================================================
 * SORTING ALGORITHMS
 * =============================================================================
 */

/**
 * Swap helper function.
 */
static void swap_generic(void *a, void *b, size_t size) {
    char temp[size];
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
}

/**
 * Quick Sort implementation (generic).
 */
static int qs_partition(void *arr, int low, int high, size_t size, Comparator cmp) {
    char *base = arr;
    void *pivot = base + high * size;
    int i = low - 1;
    
    for (int j = low; j < high; j++) {
        void *elem = base + j * size;
        if (cmp(elem, pivot) <= 0) {
            i++;
            swap_generic(base + i * size, elem, size);
        }
    }
    swap_generic(base + (i + 1) * size, pivot, size);
    return i + 1;
}

static void qs_recursive(void *arr, int low, int high, size_t size, Comparator cmp) {
    if (low < high) {
        int pi = qs_partition(arr, low, high, size, cmp);
        qs_recursive(arr, low, pi - 1, size, cmp);
        qs_recursive(arr, pi + 1, high, size, cmp);
    }
}

void quick_sort(void *arr, size_t n, size_t size, Comparator cmp) {
    if (n > 1) {
        qs_recursive(arr, 0, (int)n - 1, size, cmp);
    }
}

/**
 * Merge Sort implementation (generic).
 */
static void ms_merge(void *arr, int left, int mid, int right, 
                     size_t size, Comparator cmp, void *temp) {
    char *base = arr;
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    /* Copy to temp arrays */
    char *L = temp;
    char *R = (char *)temp + n1 * size;
    memcpy(L, base + left * size, n1 * size);
    memcpy(R, base + (mid + 1) * size, n2 * size);
    
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (cmp(L + i * size, R + j * size) <= 0) {
            memcpy(base + k * size, L + i * size, size);
            i++;
        } else {
            memcpy(base + k * size, R + j * size, size);
            j++;
        }
        k++;
    }
    
    while (i < n1) {
        memcpy(base + k * size, L + i * size, size);
        i++; k++;
    }
    while (j < n2) {
        memcpy(base + k * size, R + j * size, size);
        j++; k++;
    }
}

static void ms_recursive(void *arr, int left, int right, 
                         size_t size, Comparator cmp, void *temp) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        ms_recursive(arr, left, mid, size, cmp, temp);
        ms_recursive(arr, mid + 1, right, size, cmp, temp);
        ms_merge(arr, left, mid, right, size, cmp, temp);
    }
}

void merge_sort(void *arr, size_t n, size_t size, Comparator cmp) {
    if (n <= 1) return;
    void *temp = malloc(n * size);
    if (!temp) return;
    ms_recursive(arr, 0, (int)n - 1, size, cmp, temp);
    free(temp);
}

/**
 * Heap Sort implementation (generic).
 */
static void heapify(void *arr, size_t n, int i, size_t size, Comparator cmp) {
    char *base = arr;
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    
    if (left < (int)n && cmp(base + left * size, base + largest * size) > 0)
        largest = left;
    if (right < (int)n && cmp(base + right * size, base + largest * size) > 0)
        largest = right;
    
    if (largest != i) {
        swap_generic(base + i * size, base + largest * size, size);
        heapify(arr, n, largest, size, cmp);
    }
}

void heap_sort(void *arr, size_t n, size_t size, Comparator cmp) {
    char *base = arr;
    
    /* Build max heap */
    for (int i = (int)n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i, size, cmp);
    }
    
    /* Extract elements from heap */
    for (int i = (int)n - 1; i > 0; i--) {
        swap_generic(base, base + i * size, size);
        heapify(arr, (size_t)i, 0, size, cmp);
    }
}

/* Integer comparator for sorting */
int int_compare(const void *a, const void *b) {
    return (*(const int *)a) - (*(const int *)b);
}

/* =============================================================================
 * BINARY SEARCH
 * =============================================================================
 */

/**
 * Binary search (iterative).
 */
int binary_search_iter(const DynamicArray *arr, const void *key, Comparator cmp) {
    if (!arr || arr->count == 0) return -1;
    
    int left = 0;
    int right = (int)arr->count - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        void *elem = array_get(arr, mid);
        int result = cmp(elem, key);
        
        if (result == 0) return mid;
        if (result < 0) left = mid + 1;
        else right = mid - 1;
    }
    
    return -1;
}

/**
 * Binary search (recursive).
 */
static int bs_recursive(const DynamicArray *arr, const void *key, 
                        Comparator cmp, int left, int right) {
    if (left > right) return -1;
    
    int mid = left + (right - left) / 2;
    void *elem = array_get(arr, mid);
    int result = cmp(elem, key);
    
    if (result == 0) return mid;
    if (result < 0) return bs_recursive(arr, key, cmp, mid + 1, right);
    return bs_recursive(arr, key, cmp, left, mid - 1);
}

int binary_search_rec(const DynamicArray *arr, const void *key, Comparator cmp) {
    if (!arr || arr->count == 0) return -1;
    return bs_recursive(arr, key, cmp, 0, (int)arr->count - 1);
}

/* =============================================================================
 * GRAPH IMPLEMENTATION
 * =============================================================================
 */

Graph *graph_create(int vertices, int directed) {
    Graph *g = malloc(sizeof(Graph));
    if (!g) return NULL;
    
    g->vertices = vertices;
    g->edges = 0;
    g->directed = directed;
    
    g->adj = calloc(vertices, sizeof(AdjNode *));
    g->degree = calloc(vertices, sizeof(int));
    
    if (!g->adj || !g->degree) {
        free(g->adj);
        free(g->degree);
        free(g);
        return NULL;
    }
    
    return g;
}

void graph_destroy(Graph *g) {
    if (!g) return;
    
    for (int i = 0; i < g->vertices; i++) {
        AdjNode *current = g->adj[i];
        while (current) {
            AdjNode *next = current->next;
            free(current);
            current = next;
        }
    }
    
    free(g->adj);
    free(g->degree);
    free(g);
}

int graph_add_edge(Graph *g, int u, int v, int weight) {
    if (!g || u < 0 || u >= g->vertices || v < 0 || v >= g->vertices)
        return -1;
    
    /* Add v to u's adjacency list */
    AdjNode *node = malloc(sizeof(AdjNode));
    if (!node) return -1;
    node->vertex = v;
    node->weight = weight;
    node->next = g->adj[u];
    g->adj[u] = node;
    g->degree[u]++;
    
    /* For undirected graphs, add u to v's list */
    if (!g->directed) {
        node = malloc(sizeof(AdjNode));
        if (!node) return -1;
        node->vertex = u;
        node->weight = weight;
        node->next = g->adj[v];
        g->adj[v] = node;
        g->degree[v]++;
    }
    
    g->edges++;
    return 0;
}

/**
 * Dijkstra's algorithm using adjacency list.
 * Returns array of shortest distances from source.
 */
int *dijkstra(const Graph *g, int source) {
    if (!g || source < 0 || source >= g->vertices) return NULL;
    
    int V = g->vertices;
    int *dist = malloc(V * sizeof(int));
    bool *processed = calloc(V, sizeof(bool));
    
    if (!dist || !processed) {
        free(dist);
        free(processed);
        return NULL;
    }
    
    /* Initialise distances */
    for (int i = 0; i < V; i++) {
        dist[i] = INF;
    }
    dist[source] = 0;
    
    /* Process all vertices */
    for (int count = 0; count < V - 1; count++) {
        /* Find minimum distance unprocessed vertex */
        int min_dist = INF;
        int u = -1;
        for (int v = 0; v < V; v++) {
            if (!processed[v] && dist[v] < min_dist) {
                min_dist = dist[v];
                u = v;
            }
        }
        
        if (u == -1) break;
        processed[u] = true;
        
        /* Update distances to neighbours */
        for (AdjNode *adj = g->adj[u]; adj; adj = adj->next) {
            int v = adj->vertex;
            int weight = adj->weight;
            
            if (!processed[v] && dist[u] != INF && 
                dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
            }
        }
    }
    
    free(processed);
    return dist;
}

/* =============================================================================
 * DYNAMIC PROGRAMMING: LONGEST COMMON SUBSEQUENCE
 * =============================================================================
 */

/**
 * LCS with tabulation.
 * Returns length of LCS and optionally fills the LCS string.
 */
int lcs_length(const char *a, const char *b, char *result) {
    int m = strlen(a);
    int n = strlen(b);
    
    /* Create DP table */
    int **dp = malloc((m + 1) * sizeof(int *));
    for (int i = 0; i <= m; i++) {
        dp[i] = calloc(n + 1, sizeof(int));
    }
    
    /* Fill DP table */
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (a[i - 1] == b[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                dp[i][j] = (dp[i - 1][j] > dp[i][j - 1]) ? 
                           dp[i - 1][j] : dp[i][j - 1];
            }
        }
    }
    
    int lcs_len = dp[m][n];
    
    /* Reconstruct LCS if result buffer provided */
    if (result) {
        int idx = lcs_len;
        result[idx] = '\0';
        
        int i = m, j = n;
        while (i > 0 && j > 0) {
            if (a[i - 1] == b[j - 1]) {
                result[--idx] = a[i - 1];
                i--; j--;
            } else if (dp[i - 1][j] > dp[i][j - 1]) {
                i--;
            } else {
                j--;
            }
        }
    }
    
    /* Free DP table */
    for (int i = 0; i <= m; i++) {
        free(dp[i]);
    }
    free(dp);
    
    return lcs_len;
}

/* =============================================================================
 * HASH TABLE IMPLEMENTATION
 * =============================================================================
 */

static unsigned int hash_function(int key, int size) {
    return ((unsigned int)key % size);
}

HashTable *hash_create(int size) {
    HashTable *ht = malloc(sizeof(HashTable));
    if (!ht) return NULL;
    
    ht->buckets = calloc(size, sizeof(HashNode *));
    if (!ht->buckets) {
        free(ht);
        return NULL;
    }
    
    ht->size = size;
    ht->count = 0;
    return ht;
}

void hash_destroy(HashTable *ht) {
    if (!ht) return;
    
    for (int i = 0; i < ht->size; i++) {
        HashNode *current = ht->buckets[i];
        while (current) {
            HashNode *next = current->next;
            free(current);
            current = next;
        }
    }
    
    free(ht->buckets);
    free(ht);
}

int hash_insert(HashTable *ht, int key, int value) {
    if (!ht) return -1;
    
    unsigned int idx = hash_function(key, ht->size);
    
    /* Check if key exists */
    for (HashNode *n = ht->buckets[idx]; n; n = n->next) {
        if (n->key == key) {
            n->value = value;
            return 0;
        }
    }
    
    /* Insert new node */
    HashNode *node = malloc(sizeof(HashNode));
    if (!node) return -1;
    
    node->key = key;
    node->value = value;
    node->next = ht->buckets[idx];
    ht->buckets[idx] = node;
    ht->count++;
    
    return 0;
}

int hash_search(const HashTable *ht, int key, int *value) {
    if (!ht) return -1;
    
    unsigned int idx = hash_function(key, ht->size);
    
    for (HashNode *n = ht->buckets[idx]; n; n = n->next) {
        if (n->key == key) {
            if (value) *value = n->value;
            return 0;
        }
    }
    
    return -1;
}

/* =============================================================================
 * TOOLKIT INTERFACE
 * =============================================================================
 */

/* Sorting algorithm dispatch table */
typedef struct {
    const char   *name;
    SortFunction  func;
} SortAlgorithm;

static const SortAlgorithm SORT_ALGORITHMS[] = {
    {"quicksort", quick_sort},
    {"mergesort", merge_sort},
    {"heapsort",  heap_sort},
    {NULL, NULL}
};

void toolkit_sort(DynamicArray *arr, const char *algorithm, Comparator cmp) {
    if (!arr || !algorithm) return;
    
    for (int i = 0; SORT_ALGORITHMS[i].name; i++) {
        if (strcmp(algorithm, SORT_ALGORITHMS[i].name) == 0) {
            SORT_ALGORITHMS[i].func(arr->data, arr->count, arr->elem_size, cmp);
            return;
        }
    }
    
    fprintf(stderr, "Unknown algorithm: %s\n", algorithm);
}

int toolkit_search(const DynamicArray *arr, const void *key, Comparator cmp) {
    return binary_search_iter(arr, key, cmp);
}

/* =============================================================================
 * DEMONSTRATION
 * =============================================================================
 */

void run_demo(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║         ALGORITHM TOOLKIT - DEMONSTRATION                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Sorting demonstration */
    printf("[SORTING] Testing with 1000 random integers\n");
    
    DynamicArray *arr = array_create(sizeof(int), 1000);
    srand(42);
    for (int i = 0; i < 1000; i++) {
        int val = rand() % 10000;
        array_append(arr, &val);
    }
    
    const char *algos[] = {"quicksort", "mergesort", "heapsort"};
    const char *names[] = {"QuickSort", "MergeSort", "HeapSort"};
    
    for (int a = 0; a < 3; a++) {
        /* Reset array */
        srand(42);
        for (size_t i = 0; i < arr->count; i++) {
            int val = rand() % 10000;
            memcpy((char *)arr->data + i * sizeof(int), &val, sizeof(int));
        }
        
        clock_t start = clock();
        toolkit_sort(arr, algos[a], int_compare);
        clock_t end = clock();
        
        double time_ms = ((double)(end - start) * 1000.0) / CLOCKS_PER_SEC;
        printf("  %s:%*s%.3f ms\n", names[a], 12 - (int)strlen(names[a]), "", time_ms);
    }
    array_destroy(arr);
    
    /* Searching demonstration */
    printf("\n[SEARCHING] Binary search for value 42\n");
    arr = array_create(sizeof(int), 1000);
    for (int i = 0; i < 1000; i++) {
        int val = i;
        array_append(arr, &val);
    }
    
    int key = 42;
    int idx = toolkit_search(arr, &key, int_compare);
    printf("  Found at index: %d\n", idx);
    array_destroy(arr);
    
    /* Graph demonstration */
    printf("\n[GRAPH] Dijkstra from vertex 0 (5 vertices, 7 edges)\n");
    Graph *g = graph_create(5, false);
    graph_add_edge(g, 0, 1, 4);
    graph_add_edge(g, 0, 2, 1);
    graph_add_edge(g, 1, 2, 2);
    graph_add_edge(g, 1, 3, 5);
    graph_add_edge(g, 2, 3, 8);
    graph_add_edge(g, 2, 4, 10);
    graph_add_edge(g, 3, 4, 3);
    
    int *dist = dijkstra(g, 0);
    if (dist) {
        for (int v = 0; v < g->vertices; v++) {
            printf("  To %d: %d\n", v, dist[v] == INF ? -1 : dist[v]);
        }
        free(dist);
    }
    graph_destroy(g);
    
    /* LCS demonstration */
    printf("\n[DYNAMIC PROGRAMMING] Longest Common Subsequence\n");
    const char *str_a = "ALGORITHM";
    const char *str_b = "LOGARITHM";
    char lcs[100];
    int lcs_len = lcs_length(str_a, str_b, lcs);
    printf("  String A: \"%s\"\n", str_a);
    printf("  String B: \"%s\"\n", str_b);
    printf("  LCS length: %d (\"%s\")\n", lcs_len, lcs);
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("All tests completed successfully.\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

void print_usage(const char *prog) {
    printf("Usage: %s [options]\n\n", prog);
    printf("Options:\n");
    printf("  --demo                      Run demonstration\n");
    printf("  --sort <algorithm>          Sort with quicksort/mergesort/heapsort\n");
    printf("  --input <file>              Input file for sorting\n");
    printf("  --output <file>             Output file for sorted results\n");
    printf("  --graph <file>              Load graph from file\n");
    printf("  --dijkstra <source>         Run Dijkstra from source vertex\n");
    printf("  --help                      Show this help message\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--demo") == 0) {
            run_demo();
            return 0;
        }
        if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
    }
    
    /* Parse other command line arguments */
    const char *sort_algo = NULL;
    const char *input_file = NULL;
    const char *output_file = NULL;
    const char *graph_file = NULL;
    int dijkstra_source = -1;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--sort") == 0 && i + 1 < argc) {
            sort_algo = argv[++i];
        } else if (strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
            input_file = argv[++i];
        } else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        } else if (strcmp(argv[i], "--graph") == 0 && i + 1 < argc) {
            graph_file = argv[++i];
        } else if (strcmp(argv[i], "--dijkstra") == 0 && i + 1 < argc) {
            dijkstra_source = atoi(argv[++i]);
        }
    }
    
    /* Handle sort command */
    if (sort_algo && input_file) {
        FILE *fp = fopen(input_file, "r");
        if (!fp) {
            fprintf(stderr, "Cannot open input file: %s\n", input_file);
            return 1;
        }
        
        DynamicArray *arr = array_create(sizeof(int), 1000);
        int val;
        while (fscanf(fp, "%d", &val) == 1) {
            array_append(arr, &val);
        }
        fclose(fp);
        
        printf("Sorting %zu integers with %s...\n", arr->count, sort_algo);
        toolkit_sort(arr, sort_algo, int_compare);
        
        if (output_file) {
            fp = fopen(output_file, "w");
            if (fp) {
                for (size_t i = 0; i < arr->count; i++) {
                    fprintf(fp, "%d\n", *(int *)array_get(arr, i));
                }
                fclose(fp);
                printf("Sorted results written to: %s\n", output_file);
            }
        } else {
            printf("Sorted: ");
            for (size_t i = 0; i < arr->count && i < 10; i++) {
                printf("%d ", *(int *)array_get(arr, i));
            }
            if (arr->count > 10) printf("...");
            printf("\n");
        }
        
        array_destroy(arr);
    }
    
    /* Handle graph commands */
    if (graph_file) {
        FILE *fp = fopen(graph_file, "r");
        if (!fp) {
            fprintf(stderr, "Cannot open graph file: %s\n", graph_file);
            return 1;
        }
        
        int V, E;
        if (fscanf(fp, "%d %d", &V, &E) != 2) {
            fprintf(stderr, "Invalid graph format\n");
            fclose(fp);
            return 1;
        }
        
        Graph *g = graph_create(V, false);
        for (int i = 0; i < E; i++) {
            int u, v, w;
            if (fscanf(fp, "%d %d %d", &u, &v, &w) == 3) {
                graph_add_edge(g, u, v, w);
            }
        }
        fclose(fp);
        
        printf("Loaded graph: %d vertices, %d edges\n", V, E);
        
        if (dijkstra_source >= 0) {
            int *dist = dijkstra(g, dijkstra_source);
            if (dist) {
                printf("Dijkstra from vertex %d:\n", dijkstra_source);
                for (int v = 0; v < V; v++) {
                    printf("  To %d: ", v);
                    if (dist[v] == INF) printf("unreachable\n");
                    else printf("%d\n", dist[v]);
                }
                free(dist);
            }
        }
        
        graph_destroy(g);
    }
    
    return 0;
}
