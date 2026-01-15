/**
 * =============================================================================
 * WEEK 14: ADVANCED TOPICS AND COMPREHENSIVE REVIEW
 * Complete Working Example - Algorithm Portfolio Manager
 * =============================================================================
 *
 * This example demonstrates a comprehensive synthesis of all semester topics:
 *   1. Function pointers and callbacks for algorithm selection
 *   2. File I/O for data persistence and logging
 *   3. Dynamic data structures (linked lists, hash tables)
 *   4. Graph algorithms (BFS, DFS, Dijkstra)
 *   5. Dynamic programming patterns (memoisation, tabulation)
 *   6. Performance benchmarking and optimisation techniques
 *
 * The program implements an "Algorithm Portfolio Manager" that:
 *   - Maintains a registry of algorithms using function pointers
 *   - Benchmarks and compares algorithm performance
 *   - Stores results using various data structures
 *   - Exports analysis reports to files
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -O2 -o example1 example1.c -lm
 * Usage: ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

/* =============================================================================
 * CONFIGURATION CONSTANTS
 * =============================================================================
 */

#define MAX_ALGORITHMS      20
#define MAX_NAME_LENGTH     64
#define HASH_TABLE_SIZE     31
#define MAX_VERTICES        100
#define INF                 999999
#define BENCHMARK_RUNS      5

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * Function pointer type for sorting algorithms.
 * All sorting algorithms must conform to this signature.
 */
typedef void (*SortFunction)(int *arr, int n);

/**
 * Function pointer type for search algorithms.
 * Returns index of element or -1 if not found.
 */
typedef int (*SearchFunction)(const int *arr, int n, int target);

/**
 * Function pointer type for dynamic programming algorithms.
 * Returns computed result as long long for large values.
 */
typedef long long (*DPFunction)(int n);

/**
 * Algorithm categories enumeration.
 */
typedef enum {
    ALGO_SORT,
    ALGO_SEARCH,
    ALGO_DP,
    ALGO_GRAPH
} AlgorithmCategory;

/**
 * Structure representing a registered algorithm.
 */
typedef struct {
    char name[MAX_NAME_LENGTH];
    AlgorithmCategory category;
    void *function;                 /* Generic function pointer */
    double avg_time_ms;             /* Average execution time */
    int call_count;                 /* Number of invocations */
} Algorithm;

/**
 * Node for linked list of benchmark results.
 */
typedef struct BenchmarkNode {
    char algorithm_name[MAX_NAME_LENGTH];
    int input_size;
    double time_ms;
    struct BenchmarkNode *next;
} BenchmarkNode;

/**
 * Hash table entry for algorithm lookup.
 */
typedef struct HashEntry {
    char key[MAX_NAME_LENGTH];
    int algorithm_index;
    struct HashEntry *next;         /* Chaining for collision resolution */
} HashEntry;

/**
 * Graph structure using adjacency list representation.
 */
typedef struct {
    int vertices;
    int **adj_matrix;               /* For weighted graphs */
    int *visited;                   /* Traversal markers */
} Graph;

/**
 * Priority queue node for Dijkstra's algorithm.
 */
typedef struct {
    int vertex;
    int distance;
} PQNode;

/* =============================================================================
 * GLOBAL STATE
 * =============================================================================
 */

static Algorithm g_algorithms[MAX_ALGORITHMS];
static int g_algorithm_count = 0;
static HashEntry *g_hash_table[HASH_TABLE_SIZE];
static BenchmarkNode *g_benchmark_head = NULL;

/* =============================================================================
 * PART 1: HASH TABLE OPERATIONS
 * Using division method with chaining for collision resolution
 * =============================================================================
 */

/**
 * Hash function using division method.
 * Produces values in range [0, HASH_TABLE_SIZE - 1].
 */
static unsigned int hash_function(const char *key) {
    unsigned int hash = 0;
    while (*key) {
        hash = hash * 31 + (unsigned char)(*key);
        key++;
    }
    return hash % HASH_TABLE_SIZE;
}

/**
 * Insert algorithm into hash table for O(1) average lookup.
 */
static void hash_insert(const char *name, int index) {
    unsigned int h = hash_function(name);
    
    HashEntry *entry = malloc(sizeof(HashEntry));
    if (!entry) {
        fprintf(stderr, "Memory allocation failed for hash entry\n");
        return;
    }
    
    strncpy(entry->key, name, MAX_NAME_LENGTH - 1);
    entry->key[MAX_NAME_LENGTH - 1] = '\0';
    entry->algorithm_index = index;
    entry->next = g_hash_table[h];
    g_hash_table[h] = entry;
}

/**
 * Look up algorithm index by name.
 * Returns -1 if not found.
 */
static int hash_lookup(const char *name) {
    unsigned int h = hash_function(name);
    HashEntry *entry = g_hash_table[h];
    
    while (entry) {
        if (strcmp(entry->key, name) == 0) {
            return entry->algorithm_index;
        }
        entry = entry->next;
    }
    return -1;
}

/**
 * Free all hash table entries.
 */
static void hash_cleanup(void) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashEntry *entry = g_hash_table[i];
        while (entry) {
            HashEntry *next = entry->next;
            free(entry);
            entry = next;
        }
        g_hash_table[i] = NULL;
    }
}

/* =============================================================================
 * PART 2: SORTING ALGORITHMS
 * Demonstrating function pointers with uniform interface
 * =============================================================================
 */

/**
 * Bubble Sort - O(n²) time, O(1) space
 * Simple but inefficient; used for educational purposes.
 */
static void bubble_sort(int *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = true;
            }
        }
        if (!swapped) break;    /* Early termination optimisation */
    }
}

/**
 * Insertion Sort - O(n²) average, O(n) best case
 * Efficient for small or nearly-sorted arrays.
 */
static void insertion_sort(int *arr, int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

/**
 * Quick Sort partition using Lomuto scheme.
 */
static int partition(int *arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    
    int temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    
    return i + 1;
}

/**
 * Quick Sort recursive implementation.
 */
static void quicksort_recursive(int *arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quicksort_recursive(arr, low, pi - 1);
        quicksort_recursive(arr, pi + 1, high);
    }
}

/**
 * Quick Sort wrapper - O(n log n) average, O(n²) worst case
 */
static void quick_sort(int *arr, int n) {
    quicksort_recursive(arr, 0, n - 1);
}

/**
 * Merge operation for merge sort.
 */
static void merge(int *arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));
    
    if (!L || !R) {
        free(L);
        free(R);
        return;
    }
    
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];
    
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    
    free(L);
    free(R);
}

/**
 * Merge Sort recursive implementation.
 */
static void mergesort_recursive(int *arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergesort_recursive(arr, left, mid);
        mergesort_recursive(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

/**
 * Merge Sort wrapper - O(n log n) guaranteed, O(n) space
 */
static void merge_sort(int *arr, int n) {
    mergesort_recursive(arr, 0, n - 1);
}

/* =============================================================================
 * PART 3: SEARCH ALGORITHMS
 * =============================================================================
 */

/**
 * Linear Search - O(n) time
 * Works on unsorted arrays.
 */
static int linear_search(const int *arr, int n, int target) {
    for (int i = 0; i < n; i++) {
        if (arr[i] == target) return i;
    }
    return -1;
}

/**
 * Binary Search - O(log n) time
 * Requires sorted array.
 */
static int binary_search(const int *arr, int n, int target) {
    int left = 0, right = n - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (arr[mid] == target) return mid;
        if (arr[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

/**
 * Interpolation Search - O(log log n) average for uniform distribution
 */
static int interpolation_search(const int *arr, int n, int target) {
    int low = 0, high = n - 1;
    
    while (low <= high && target >= arr[low] && target <= arr[high]) {
        if (low == high) {
            return (arr[low] == target) ? low : -1;
        }
        
        /* Interpolation formula */
        int pos = low + (((double)(high - low) / 
                  (arr[high] - arr[low])) * (target - arr[low]));
        
        if (arr[pos] == target) return pos;
        if (arr[pos] < target) low = pos + 1;
        else high = pos - 1;
    }
    return -1;
}

/* =============================================================================
 * PART 4: DYNAMIC PROGRAMMING ALGORITHMS
 * =============================================================================
 */

/**
 * Fibonacci - Naive recursive O(2^n)
 */
static long long fib_naive(int n) {
    if (n <= 1) return n;
    return fib_naive(n - 1) + fib_naive(n - 2);
}

/**
 * Fibonacci - Memoised O(n) time, O(n) space
 */
static long long fib_memo_helper(int n, long long *memo) {
    if (n <= 1) return n;
    if (memo[n] != -1) return memo[n];
    memo[n] = fib_memo_helper(n - 1, memo) + fib_memo_helper(n - 2, memo);
    return memo[n];
}

static long long fib_memoised(int n) {
    if (n <= 1) return n;
    long long *memo = malloc((n + 1) * sizeof(long long));
    if (!memo) return -1;
    
    for (int i = 0; i <= n; i++) memo[i] = -1;
    long long result = fib_memo_helper(n, memo);
    free(memo);
    return result;
}

/**
 * Fibonacci - Tabulation O(n) time, O(n) space
 */
static long long fib_tabulation(int n) {
    if (n <= 1) return n;
    
    long long *dp = malloc((n + 1) * sizeof(long long));
    if (!dp) return -1;
    
    dp[0] = 0;
    dp[1] = 1;
    for (int i = 2; i <= n; i++) {
        dp[i] = dp[i - 1] + dp[i - 2];
    }
    
    long long result = dp[n];
    free(dp);
    return result;
}

/**
 * Fibonacci - Space-optimised O(n) time, O(1) space
 */
static long long fib_optimised(int n) {
    if (n <= 1) return n;
    
    long long prev2 = 0, prev1 = 1;
    for (int i = 2; i <= n; i++) {
        long long curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}

/* =============================================================================
 * PART 5: GRAPH ALGORITHMS
 * =============================================================================
 */

/**
 * Create a new graph with given number of vertices.
 */
static Graph *graph_create(int vertices) {
    Graph *g = malloc(sizeof(Graph));
    if (!g) return NULL;
    
    g->vertices = vertices;
    g->adj_matrix = malloc(vertices * sizeof(int *));
    g->visited = calloc(vertices, sizeof(int));
    
    if (!g->adj_matrix || !g->visited) {
        free(g->adj_matrix);
        free(g->visited);
        free(g);
        return NULL;
    }
    
    for (int i = 0; i < vertices; i++) {
        g->adj_matrix[i] = calloc(vertices, sizeof(int));
        if (!g->adj_matrix[i]) {
            for (int j = 0; j < i; j++) free(g->adj_matrix[j]);
            free(g->adj_matrix);
            free(g->visited);
            free(g);
            return NULL;
        }
    }
    
    return g;
}

/**
 * Add weighted edge to graph.
 */
static void graph_add_edge(Graph *g, int src, int dest, int weight) {
    if (src >= 0 && src < g->vertices && dest >= 0 && dest < g->vertices) {
        g->adj_matrix[src][dest] = weight;
        g->adj_matrix[dest][src] = weight;    /* Undirected */
    }
}

/**
 * Depth-First Search traversal.
 */
static void dfs_visit(Graph *g, int v, int *order, int *idx) {
    g->visited[v] = 1;
    order[(*idx)++] = v;
    
    for (int i = 0; i < g->vertices; i++) {
        if (g->adj_matrix[v][i] > 0 && !g->visited[i]) {
            dfs_visit(g, i, order, idx);
        }
    }
}

/**
 * Breadth-First Search traversal.
 */
static void bfs_traversal(Graph *g, int start, int *order, int *count) {
    int *queue = malloc(g->vertices * sizeof(int));
    if (!queue) return;
    
    int front = 0, rear = 0;
    
    memset(g->visited, 0, g->vertices * sizeof(int));
    
    queue[rear++] = start;
    g->visited[start] = 1;
    
    while (front < rear) {
        int v = queue[front++];
        order[(*count)++] = v;
        
        for (int i = 0; i < g->vertices; i++) {
            if (g->adj_matrix[v][i] > 0 && !g->visited[i]) {
                queue[rear++] = i;
                g->visited[i] = 1;
            }
        }
    }
    
    free(queue);
}

/**
 * Dijkstra's shortest path algorithm.
 * Returns shortest distances from source to all vertices.
 */
static int *dijkstra(Graph *g, int src) {
    int *dist = malloc(g->vertices * sizeof(int));
    bool *processed = calloc(g->vertices, sizeof(bool));
    
    if (!dist || !processed) {
        free(dist);
        free(processed);
        return NULL;
    }
    
    /* Initialise distances */
    for (int i = 0; i < g->vertices; i++) {
        dist[i] = INF;
    }
    dist[src] = 0;
    
    /* Process all vertices */
    for (int count = 0; count < g->vertices - 1; count++) {
        /* Find minimum distance vertex not yet processed */
        int min_dist = INF, u = -1;
        for (int v = 0; v < g->vertices; v++) {
            if (!processed[v] && dist[v] < min_dist) {
                min_dist = dist[v];
                u = v;
            }
        }
        
        if (u == -1) break;
        processed[u] = true;
        
        /* Update distances of adjacent vertices */
        for (int v = 0; v < g->vertices; v++) {
            if (!processed[v] && g->adj_matrix[u][v] > 0 &&
                dist[u] != INF && 
                dist[u] + g->adj_matrix[u][v] < dist[v]) {
                dist[v] = dist[u] + g->adj_matrix[u][v];
            }
        }
    }
    
    free(processed);
    return dist;
}

/**
 * Free graph memory.
 */
static void graph_free(Graph *g) {
    if (g) {
        for (int i = 0; i < g->vertices; i++) {
            free(g->adj_matrix[i]);
        }
        free(g->adj_matrix);
        free(g->visited);
        free(g);
    }
}

/* =============================================================================
 * PART 6: BENCHMARK LINKED LIST OPERATIONS
 * =============================================================================
 */

/**
 * Add benchmark result to linked list (prepend for O(1) insertion).
 */
static void benchmark_add(const char *name, int size, double time_ms) {
    BenchmarkNode *node = malloc(sizeof(BenchmarkNode));
    if (!node) return;
    
    strncpy(node->algorithm_name, name, MAX_NAME_LENGTH - 1);
    node->algorithm_name[MAX_NAME_LENGTH - 1] = '\0';
    node->input_size = size;
    node->time_ms = time_ms;
    node->next = g_benchmark_head;
    g_benchmark_head = node;
}

/**
 * Free all benchmark nodes.
 */
static void benchmark_cleanup(void) {
    BenchmarkNode *current = g_benchmark_head;
    while (current) {
        BenchmarkNode *next = current->next;
        free(current);
        current = next;
    }
    g_benchmark_head = NULL;
}

/**
 * Export benchmark results to CSV file.
 */
static void benchmark_export(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Cannot open %s for writing\n", filename);
        return;
    }
    
    fprintf(fp, "Algorithm,Input Size,Time (ms)\n");
    
    BenchmarkNode *current = g_benchmark_head;
    while (current) {
        fprintf(fp, "%s,%d,%.4f\n", 
                current->algorithm_name,
                current->input_size,
                current->time_ms);
        current = current->next;
    }
    
    fclose(fp);
    printf("  Exported %s\n", filename);
}

/* =============================================================================
 * PART 7: ALGORITHM REGISTRY AND DISPATCHER
 * =============================================================================
 */

/**
 * Register an algorithm in the portfolio.
 */
static int register_algorithm(const char *name, AlgorithmCategory cat, void *func) {
    if (g_algorithm_count >= MAX_ALGORITHMS) {
        fprintf(stderr, "Algorithm registry full\n");
        return -1;
    }
    
    int idx = g_algorithm_count++;
    strncpy(g_algorithms[idx].name, name, MAX_NAME_LENGTH - 1);
    g_algorithms[idx].name[MAX_NAME_LENGTH - 1] = '\0';
    g_algorithms[idx].category = cat;
    g_algorithms[idx].function = func;
    g_algorithms[idx].avg_time_ms = 0.0;
    g_algorithms[idx].call_count = 0;
    
    hash_insert(name, idx);
    
    return idx;
}

/**
 * Get algorithm by name using hash table lookup.
 */
static Algorithm *get_algorithm(const char *name) {
    int idx = hash_lookup(name);
    if (idx >= 0 && idx < g_algorithm_count) {
        return &g_algorithms[idx];
    }
    return NULL;
}

/**
 * Benchmark a sorting algorithm.
 */
static double benchmark_sort(SortFunction sort_func, int *arr, int n) {
    /* Create copy for fair comparison */
    int *copy = malloc(n * sizeof(int));
    if (!copy) return -1.0;
    
    double total_time = 0.0;
    
    for (int run = 0; run < BENCHMARK_RUNS; run++) {
        memcpy(copy, arr, n * sizeof(int));
        
        clock_t start = clock();
        sort_func(copy, n);
        clock_t end = clock();
        
        total_time += ((double)(end - start) * 1000.0) / CLOCKS_PER_SEC;
    }
    
    free(copy);
    return total_time / BENCHMARK_RUNS;
}

/**
 * Generate random array for testing.
 */
static int *generate_random_array(int n, int max_val) {
    int *arr = malloc(n * sizeof(int));
    if (!arr) return NULL;
    
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % max_val;
    }
    return arr;
}

/* =============================================================================
 * DEMONSTRATION FUNCTIONS
 * =============================================================================
 */

static void demo_part1_hash_table(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  PART 1: Hash Table for Algorithm Registry                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("  Hash Function: division method (key %% %d)\n", HASH_TABLE_SIZE);
    printf("  Collision Resolution: separate chaining\n\n");
    
    /* Register all algorithms */
    register_algorithm("BubbleSort", ALGO_SORT, bubble_sort);
    register_algorithm("InsertionSort", ALGO_SORT, insertion_sort);
    register_algorithm("QuickSort", ALGO_SORT, quick_sort);
    register_algorithm("MergeSort", ALGO_SORT, merge_sort);
    register_algorithm("LinearSearch", ALGO_SEARCH, linear_search);
    register_algorithm("BinarySearch", ALGO_SEARCH, binary_search);
    register_algorithm("InterpolationSearch", ALGO_SEARCH, interpolation_search);
    register_algorithm("FibNaive", ALGO_DP, fib_naive);
    register_algorithm("FibMemoised", ALGO_DP, fib_memoised);
    register_algorithm("FibTabulation", ALGO_DP, fib_tabulation);
    register_algorithm("FibOptimised", ALGO_DP, fib_optimised);
    
    printf("  Registered %d algorithms in hash table\n\n", g_algorithm_count);
    
    /* Demonstrate lookup */
    const char *test_names[] = {"QuickSort", "BinarySearch", "FibOptimised", "Unknown"};
    printf("  Hash Table Lookups:\n");
    printf("  %-20s | %-10s | %s\n", "Algorithm", "Hash", "Found");
    printf("  %-20s-+-%-10s-+-%s\n", "--------------------", "----------", "------");
    
    for (int i = 0; i < 4; i++) {
        unsigned int h = hash_function(test_names[i]);
        Algorithm *a = get_algorithm(test_names[i]);
        printf("  %-20s | %10u | %s\n", 
               test_names[i], h, a ? "Yes" : "No");
    }
}

static void demo_part2_sorting(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  PART 2: Sorting Algorithm Benchmarks                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int sizes[] = {100, 500, 1000, 2000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    const char *sort_names[] = {"BubbleSort", "InsertionSort", "QuickSort", "MergeSort"};
    int num_sorts = sizeof(sort_names) / sizeof(sort_names[0]);
    
    printf("  Benchmarking with %d runs per test...\n\n", BENCHMARK_RUNS);
    printf("  %-15s", "Size");
    for (int s = 0; s < num_sorts; s++) {
        printf(" | %12s", sort_names[s]);
    }
    printf("\n");
    printf("  %-15s", "---------------");
    for (int s = 0; s < num_sorts; s++) {
        printf("-+-%12s", "------------");
    }
    printf("\n");
    
    for (int i = 0; i < num_sizes; i++) {
        int n = sizes[i];
        int *arr = generate_random_array(n, 10000);
        if (!arr) continue;
        
        printf("  n = %-10d", n);
        
        for (int s = 0; s < num_sorts; s++) {
            Algorithm *algo = get_algorithm(sort_names[s]);
            if (algo) {
                double time_ms = benchmark_sort((SortFunction)algo->function, arr, n);
                printf(" | %9.3f ms", time_ms);
                benchmark_add(sort_names[s], n, time_ms);
                algo->avg_time_ms = (algo->avg_time_ms * algo->call_count + time_ms) / 
                                    (algo->call_count + 1);
                algo->call_count++;
            }
        }
        printf("\n");
        
        free(arr);
    }
    
    printf("\n  Complexity Analysis:\n");
    printf("  • BubbleSort:    O(n²)      - Stable, simple, slow\n");
    printf("  • InsertionSort: O(n²)      - Stable, good for small/sorted data\n");
    printf("  • QuickSort:     O(n log n) - Unstable, fastest average case\n");
    printf("  • MergeSort:     O(n log n) - Stable, consistent, uses O(n) space\n");
}

static void demo_part3_dynamic_programming(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  PART 3: Dynamic Programming Comparison                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("  Computing Fibonacci numbers with different approaches:\n\n");
    
    int test_values[] = {10, 20, 30, 40};
    const char *dp_names[] = {"FibNaive", "FibMemoised", "FibTabulation", "FibOptimised"};
    
    printf("  %-5s", "n");
    for (int m = 0; m < 4; m++) {
        printf(" | %15s", dp_names[m]);
    }
    printf("\n");
    printf("  %-5s", "-----");
    for (int m = 0; m < 4; m++) {
        printf("-+-%-15s", "---------------");
    }
    printf("\n");
    
    for (int i = 0; i < 4; i++) {
        int n = test_values[i];
        printf("  %-5d", n);
        
        for (int m = 0; m < 4; m++) {
            Algorithm *algo = get_algorithm(dp_names[m]);
            if (algo) {
                clock_t start = clock();
                long long result = -1;
                
                /* Skip naive for large n (too slow) */
                if (m == 0 && n > 35) {
                    printf(" | %12s ms", "---");
                    continue;
                }
                
                DPFunction func = (DPFunction)algo->function;
                result = func(n);
                
                clock_t end = clock();
                double time_ms = ((double)(end - start) * 1000.0) / CLOCKS_PER_SEC;
                printf(" | %9.3f ms", time_ms);
            }
        }
        printf("\n");
    }
    
    printf("\n  Space Optimisation Demonstration:\n");
    printf("  • Naive:      O(2^n) time, O(n) stack space - exponential!\n");
    printf("  • Memoised:   O(n) time, O(n) space - top-down DP\n");
    printf("  • Tabulation: O(n) time, O(n) space - bottom-up DP\n");
    printf("  • Optimised:  O(n) time, O(1) space - rolling variables\n");
    printf("\n  Fib(45) = %lld\n", fib_optimised(45));
}

static void demo_part4_graphs(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  PART 4: Graph Algorithms                                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Create sample weighted graph */
    int V = 6;
    Graph *g = graph_create(V);
    if (!g) return;
    
    printf("  Creating weighted graph with %d vertices:\n\n", V);
    printf("       (0)----7----(1)----9----(2)\n");
    printf("        |\\          |          / \n");
    printf("        | \\        14         /  \n");
    printf("       10  \\        |       15   \n");
    printf("        |   2       |      /     \n");
    printf("        |    \\      |     /      \n");
    printf("       (3)----6----(4)---11----(5)\n\n");
    
    graph_add_edge(g, 0, 1, 7);
    graph_add_edge(g, 0, 3, 10);
    graph_add_edge(g, 0, 4, 2);
    graph_add_edge(g, 1, 2, 9);
    graph_add_edge(g, 1, 4, 14);
    graph_add_edge(g, 2, 5, 15);
    graph_add_edge(g, 3, 4, 6);
    graph_add_edge(g, 4, 5, 11);
    
    /* BFS traversal */
    int bfs_order[MAX_VERTICES];
    int bfs_count = 0;
    bfs_traversal(g, 0, bfs_order, &bfs_count);
    
    printf("  BFS from vertex 0: ");
    for (int i = 0; i < bfs_count; i++) {
        printf("%d ", bfs_order[i]);
    }
    printf("\n");
    
    /* DFS traversal */
    memset(g->visited, 0, g->vertices * sizeof(int));
    int dfs_order[MAX_VERTICES];
    int dfs_count = 0;
    dfs_visit(g, 0, dfs_order, &dfs_count);
    
    printf("  DFS from vertex 0: ");
    for (int i = 0; i < dfs_count; i++) {
        printf("%d ", dfs_order[i]);
    }
    printf("\n\n");
    
    /* Dijkstra's algorithm */
    int *distances = dijkstra(g, 0);
    if (distances) {
        printf("  Dijkstra's Shortest Paths from vertex 0:\n");
        printf("  %-10s | %-10s\n", "Vertex", "Distance");
        printf("  %-10s-+-%-10s\n", "----------", "----------");
        for (int i = 0; i < V; i++) {
            if (distances[i] == INF) {
                printf("  %-10d | %10s\n", i, "∞");
            } else {
                printf("  %-10d | %10d\n", i, distances[i]);
            }
        }
        free(distances);
    }
    
    graph_free(g);
}

static void demo_part5_file_export(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  PART 5: File I/O - Exporting Results                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Export benchmark results */
    printf("  Exporting benchmark results to files:\n");
    benchmark_export("benchmark_results.csv");
    
    /* Export algorithm statistics */
    FILE *stats = fopen("algorithm_stats.txt", "w");
    if (stats) {
        fprintf(stats, "Algorithm Statistics Report\n");
        fprintf(stats, "===========================\n\n");
        fprintf(stats, "%-20s | %-10s | %-10s | %s\n", 
                "Algorithm", "Category", "Calls", "Avg Time (ms)");
        fprintf(stats, "%-20s-+-%-10s-+-%-10s-+-%s\n",
                "--------------------", "----------", "----------", "-------------");
        
        const char *cat_names[] = {"Sort", "Search", "DP", "Graph"};
        
        for (int i = 0; i < g_algorithm_count; i++) {
            fprintf(stats, "%-20s | %-10s | %10d | %.4f\n",
                    g_algorithms[i].name,
                    cat_names[g_algorithms[i].category],
                    g_algorithms[i].call_count,
                    g_algorithms[i].avg_time_ms);
        }
        
        fclose(stats);
        printf("  Exported algorithm_stats.txt\n");
    }
    
    printf("\n  File I/O Summary:\n");
    printf("  • Used fopen/fclose for file management\n");
    printf("  • fprintf for formatted text output\n");
    printf("  • CSV format for data interchange\n");
    printf("  • Plain text for human-readable reports\n");
}

static void demo_part6_summary(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  PART 6: Semester Summary and Key Takeaways                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("  Data Structures Covered:\n");
    printf("  ┌─────────────────┬─────────────────┬─────────────────┐\n");
    printf("  │ Structure       │ Access          │ Insert/Delete   │\n");
    printf("  ├─────────────────┼─────────────────┼─────────────────┤\n");
    printf("  │ Array           │ O(1)            │ O(n)            │\n");
    printf("  │ Linked List     │ O(n)            │ O(1)            │\n");
    printf("  │ Hash Table      │ O(1) average    │ O(1) average    │\n");
    printf("  │ Binary Tree     │ O(log n)        │ O(log n)        │\n");
    printf("  │ Heap            │ O(1) for max    │ O(log n)        │\n");
    printf("  └─────────────────┴─────────────────┴─────────────────┘\n\n");
    
    printf("  Algorithm Paradigms:\n");
    printf("  • Divide and Conquer: MergeSort, QuickSort, Binary Search\n");
    printf("  • Dynamic Programming: Fibonacci, LCS, Shortest Paths\n");
    printf("  • Greedy: Dijkstra, Kruskal, Huffman Coding\n");
    printf("  • Backtracking: N-Queens, Sudoku, Graph Colouring\n\n");
    
    printf("  Key C Programming Concepts:\n");
    printf("  • Function pointers for algorithm abstraction\n");
    printf("  • Dynamic memory allocation (malloc/free)\n");
    printf("  • File I/O for data persistence\n");
    printf("  • Structures for complex data types\n");
    printf("  • Pointers for efficient data manipulation\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  WEEK 14: Advanced Topics and Comprehensive Review            ║\n");
    printf("║  Algorithm Portfolio Manager - Complete Example               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    srand((unsigned int)time(NULL));
    
    /* Initialise hash table */
    memset(g_hash_table, 0, sizeof(g_hash_table));
    
    /* Run all demonstrations */
    demo_part1_hash_table();
    demo_part2_sorting();
    demo_part3_dynamic_programming();
    demo_part4_graphs();
    demo_part5_file_export();
    demo_part6_summary();
    
    /* Cleanup */
    printf("\n  Cleaning up resources...\n");
    hash_cleanup();
    benchmark_cleanup();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  Program completed successfully. No memory leaks.             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

/* =============================================================================
 * END OF FILE
 * =============================================================================
 */
