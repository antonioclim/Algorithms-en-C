/**
 * =============================================================================
 * WEEK 20: PARALLEL AND CONCURRENT PROGRAMMING
 * Exercise 2 Solution: Parallel Quick Sort
 * =============================================================================
 *
 * Complete implementation of parallel quick sort using POSIX threads with
 * depth limiting and task-based parallelism.
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -pthread -o exercise2_sol exercise2_sol.c
 * Usage: ./exercise2_sol [array_size] [num_threads]
 *
 * =============================================================================
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>
#include <time.h>

/* =============================================================================
 * CONFIGURATION CONSTANTS
 * =============================================================================
 */

#define DEFAULT_ARRAY_SIZE      100000
#define DEFAULT_NUM_THREADS     4
#define INSERTION_THRESHOLD     32
#define MAX_THREADS             64

/* =============================================================================
 * GLOBAL STATE
 * =============================================================================
 */

static atomic_int active_threads = 0;
static int max_threads = DEFAULT_NUM_THREADS;

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Swaps two integers
 */
static inline void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Returns the median of three values (for pivot selection)
 */
static inline int median_of_three(int a, int b, int c) {
    if (a < b) {
        if (b < c) return b;
        else if (a < c) return c;
        else return a;
    } else {
        if (a < c) return a;
        else if (b < c) return c;
        else return b;
    }
}

/**
 * Generates a random array of integers
 *
 * @param arr  Array to fill
 * @param n    Number of elements
 */
void generate_random_array(int *arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        arr[i] = rand() % 1000000;
    }
}

/**
 * Copies an array
 *
 * @param dest Destination array
 * @param src  Source array
 * @param n    Number of elements
 */
void copy_array(int *dest, const int *src, size_t n) {
    memcpy(dest, src, n * sizeof(int));
}

/**
 * Verifies that an array is sorted
 *
 * @param arr Array to check
 * @param n   Number of elements
 * @return true if sorted in ascending order
 */
bool is_sorted(const int *arr, size_t n) {
    for (size_t i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) {
            return false;
        }
    }
    return true;
}

/* =============================================================================
 * INSERTION SORT (for small subarrays)
 * =============================================================================
 */

/**
 * Sorts a small subarray using insertion sort
 *
 * @param arr   Array to sort
 * @param left  Left index (inclusive)
 * @param right Right index (inclusive)
 */
void insertion_sort(int *arr, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

/* =============================================================================
 * QUICK SORT PARTITION (Hoare scheme with median-of-three)
 * =============================================================================
 */

/**
 * Partitions array around pivot using Hoare's scheme
 * Uses median-of-three for pivot selection
 *
 * @param arr   Array to partition
 * @param left  Left index (inclusive)
 * @param right Right index (inclusive)
 * @return Partition index
 */
int partition(int *arr, int left, int right) {
    /* Median-of-three pivot selection */
    int mid = left + (right - left) / 2;
    int pivot = median_of_three(arr[left], arr[mid], arr[right]);

    /* Move pivot to end */
    if (pivot == arr[left]) {
        swap(&arr[left], &arr[right]);
    } else if (pivot == arr[mid]) {
        swap(&arr[mid], &arr[right]);
    }

    pivot = arr[right];
    int i = left - 1;

    for (int j = left; j < right; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }

    swap(&arr[i + 1], &arr[right]);
    return i + 1;
}

/* =============================================================================
 * SEQUENTIAL QUICK SORT
 * =============================================================================
 */

/**
 * Sequential quick sort implementation
 *
 * @param arr   Array to sort
 * @param left  Left index (inclusive)
 * @param right Right index (inclusive)
 */
void quick_sort_sequential(int *arr, int left, int right) {
    if (left >= right) return;

    /* Use insertion sort for small subarrays */
    if (right - left + 1 <= INSERTION_THRESHOLD) {
        insertion_sort(arr, left, right);
        return;
    }

    int pivot_idx = partition(arr, left, right);

    quick_sort_sequential(arr, left, pivot_idx - 1);
    quick_sort_sequential(arr, pivot_idx + 1, right);
}

/* =============================================================================
 * PARALLEL QUICK SORT
 * =============================================================================
 */

/**
 * Task structure for parallel sorting
 */
typedef struct {
    int *arr;
    int  left;
    int  right;
    int  depth;
} SortTask;

/**
 * Calculates maximum parallel depth based on thread count
 *
 * @param num_threads Number of available threads
 * @return Maximum depth for parallelism
 */
int calculate_max_depth(int num_threads) {
    int depth = 0;
    while ((1 << depth) < num_threads) {
        depth++;
    }
    return depth + 1;
}

/**
 * Parallel quick sort worker function
 *
 * @param arg SortTask pointer
 * @return NULL
 */
void *parallel_quick_sort_worker(void *arg) {
    SortTask *task = (SortTask *)arg;
    int *arr = task->arr;
    int left = task->left;
    int right = task->right;
    int depth = task->depth;

    free(task);

    /* Base case: use sequential sort */
    if (left >= right || right - left + 1 <= INSERTION_THRESHOLD) {
        if (left < right) {
            insertion_sort(arr, left, right);
        }
        atomic_fetch_sub(&active_threads, 1);
        return NULL;
    }

    /* Calculate max depth for parallel execution */
    int max_depth = calculate_max_depth(max_threads);

    /* If too deep or no threads available, sort sequentially */
    if (depth >= max_depth) {
        quick_sort_sequential(arr, left, right);
        atomic_fetch_sub(&active_threads, 1);
        return NULL;
    }

    /* Partition */
    int pivot_idx = partition(arr, left, right);

    /* Determine if we should spawn threads */
    int current_threads = atomic_load(&active_threads);
    bool spawn_left = (current_threads < max_threads) && (pivot_idx - 1 - left > INSERTION_THRESHOLD);
    bool spawn_right = (current_threads < max_threads) && (right - pivot_idx - 1 > INSERTION_THRESHOLD);

    pthread_t left_thread, right_thread;
    bool left_spawned = false, right_spawned = false;

    /* Spawn left task if beneficial */
    if (spawn_left && atomic_fetch_add(&active_threads, 1) < max_threads) {
        SortTask *left_task = malloc(sizeof(SortTask));
        left_task->arr = arr;
        left_task->left = left;
        left_task->right = pivot_idx - 1;
        left_task->depth = depth + 1;

        if (pthread_create(&left_thread, NULL, parallel_quick_sort_worker, left_task) == 0) {
            left_spawned = true;
        } else {
            free(left_task);
            atomic_fetch_sub(&active_threads, 1);
        }
    }

    /* Spawn right task if beneficial */
    if (spawn_right && atomic_fetch_add(&active_threads, 1) < max_threads) {
        SortTask *right_task = malloc(sizeof(SortTask));
        right_task->arr = arr;
        right_task->left = pivot_idx + 1;
        right_task->right = right;
        right_task->depth = depth + 1;

        if (pthread_create(&right_thread, NULL, parallel_quick_sort_worker, right_task) == 0) {
            right_spawned = true;
        } else {
            free(right_task);
            atomic_fetch_sub(&active_threads, 1);
        }
    }

    /* Sort partitions that weren't spawned as threads */
    if (!left_spawned && left < pivot_idx - 1) {
        quick_sort_sequential(arr, left, pivot_idx - 1);
    }

    if (!right_spawned && pivot_idx + 1 < right) {
        quick_sort_sequential(arr, pivot_idx + 1, right);
    }

    /* Wait for spawned threads */
    if (left_spawned) {
        pthread_join(left_thread, NULL);
    }

    if (right_spawned) {
        pthread_join(right_thread, NULL);
    }

    atomic_fetch_sub(&active_threads, 1);
    return NULL;
}

/**
 * Parallel quick sort entry point
 *
 * @param arr         Array to sort
 * @param n           Number of elements
 * @param num_threads Number of threads to use
 */
void parallel_quick_sort(int *arr, size_t n, int num_threads) {
    if (n <= 1) return;

    max_threads = num_threads;
    atomic_store(&active_threads, 1);

    SortTask *initial_task = malloc(sizeof(SortTask));
    initial_task->arr = arr;
    initial_task->left = 0;
    initial_task->right = (int)n - 1;
    initial_task->depth = 0;

    pthread_t main_thread;
    pthread_create(&main_thread, NULL, parallel_quick_sort_worker, initial_task);
    pthread_join(main_thread, NULL);
}

/* =============================================================================
 * BENCHMARKING
 * =============================================================================
 */

/**
 * Returns current time in seconds
 */
double get_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

/**
 * Benchmarks sorting algorithms
 *
 * @param n           Array size
 * @param num_threads Number of threads
 */
void run_benchmark(size_t n, int num_threads) {
    printf("\nBenchmark Configuration:\n");
    printf("  Array size: %zu\n", n);
    printf("  Threads: %d\n", num_threads);
    printf("  Insertion threshold: %d\n", INSERTION_THRESHOLD);
    printf("\n");

    /* Allocate arrays */
    int *original = malloc(n * sizeof(int));
    int *arr_seq = malloc(n * sizeof(int));
    int *arr_par = malloc(n * sizeof(int));

    if (!original || !arr_seq || !arr_par) {
        fprintf(stderr, "ERROR: Memory allocation failed\n");
        free(original);
        free(arr_seq);
        free(arr_par);
        return;
    }

    /* Generate random data */
    srand((unsigned int)time(NULL));
    generate_random_array(original, n);

    /* Sequential sort */
    printf("Running sequential quick sort...\n");
    copy_array(arr_seq, original, n);
    double start_seq = get_time();
    quick_sort_sequential(arr_seq, 0, (int)n - 1);
    double time_seq = get_time() - start_seq;

    /* Parallel sort */
    printf("Running parallel quick sort...\n");
    copy_array(arr_par, original, n);
    double start_par = get_time();
    parallel_quick_sort(arr_par, n, num_threads);
    double time_par = get_time() - start_par;

    /* Verify results */
    bool seq_sorted = is_sorted(arr_seq, n);
    bool par_sorted = is_sorted(arr_par, n);
    bool results_match = (memcmp(arr_seq, arr_par, n * sizeof(int)) == 0);

    /* Print results */
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║              PARALLEL QUICK SORT - RESULTS                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("Configuration:\n");
    printf("  Array size: %zu\n", n);
    printf("  Thread count: %d\n", num_threads);
    printf("  Depth limit: %d\n", calculate_max_depth(num_threads));
    printf("  Insertion threshold: %d\n", INSERTION_THRESHOLD);
    printf("\n");

    printf("Sorted Array (first 10 elements):\n  ");
    for (size_t i = 0; i < 10 && i < n; i++) {
        printf("%d ", arr_par[i]);
    }
    printf("\n\n");

    printf("Sorted Array (last 10 elements):\n  ");
    for (size_t i = (n > 10 ? n - 10 : 0); i < n; i++) {
        printf("%d ", arr_par[i]);
    }
    printf("\n\n");

    printf("Verification:\n");
    printf("  Sequential sorted: %s\n", seq_sorted ? "YES" : "NO");
    printf("  Parallel sorted: %s\n", par_sorted ? "YES" : "NO");
    printf("  Results match: %s\n", results_match ? "YES" : "NO");
    printf("\n");

    printf("Performance:\n");
    printf("  Sequential time: %.4f seconds\n", time_seq);
    printf("  Parallel time:   %.4f seconds\n", time_par);
    
    if (time_par > 0) {
        double speedup = time_seq / time_par;
        double efficiency = speedup / num_threads * 100;
        printf("  Speedup:         %.2fx\n", speedup);
        printf("  Efficiency:      %.1f%%\n", efficiency);
    }
    printf("\n");

    printf("Status: %s\n", (seq_sorted && par_sorted && results_match) ? "COMPLETE" : "FAILED");
    printf("\n");

    /* Cleanup */
    free(original);
    free(arr_seq);
    free(arr_par);
}

/**
 * Runs scalability analysis with varying thread counts
 */
void run_scalability_analysis(size_t n) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║              SCALABILITY ANALYSIS                             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("Array size: %zu elements\n\n", n);

    int *original = malloc(n * sizeof(int));
    int *arr = malloc(n * sizeof(int));

    if (!original || !arr) {
        fprintf(stderr, "ERROR: Memory allocation failed\n");
        free(original);
        free(arr);
        return;
    }

    srand((unsigned int)time(NULL));
    generate_random_array(original, n);

    /* Sequential baseline */
    copy_array(arr, original, n);
    double start = get_time();
    quick_sort_sequential(arr, 0, (int)n - 1);
    double time_seq = get_time() - start;

    printf("┌──────────┬────────────┬──────────┬────────────┐\n");
    printf("│ Threads  │ Time (s)   │ Speedup  │ Efficiency │\n");
    printf("├──────────┼────────────┼──────────┼────────────┤\n");
    printf("│ 1 (seq)  │ %10.4f │ %8.2f │ %9.1f%% │\n",
           time_seq, 1.0, 100.0);

    /* Parallel with varying thread counts */
    int thread_counts[] = {1, 2, 4, 8, 16};
    int num_tests = sizeof(thread_counts) / sizeof(thread_counts[0]);

    for (int i = 0; i < num_tests; i++) {
        int threads = thread_counts[i];
        
        copy_array(arr, original, n);
        start = get_time();
        parallel_quick_sort(arr, n, threads);
        double time_par = get_time() - start;

        double speedup = time_seq / time_par;
        double efficiency = speedup / threads * 100;

        printf("│ %8d │ %10.4f │ %8.2f │ %9.1f%% │\n",
               threads, time_par, speedup, efficiency);
    }

    printf("└──────────┴────────────┴──────────┴────────────┘\n");
    printf("\n");

    free(original);
    free(arr);
}

/* =============================================================================
 * MAIN PROGRAMME
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 20: PARALLEL QUICK SORT                              ║\n");
    printf("║                 Solution                                      ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    /* Parse command line arguments */
    size_t array_size = DEFAULT_ARRAY_SIZE;
    int num_threads = DEFAULT_NUM_THREADS;

    if (argc > 1) {
        array_size = (size_t)atol(argv[1]);
        if (array_size == 0) array_size = DEFAULT_ARRAY_SIZE;
    }

    if (argc > 2) {
        num_threads = atoi(argv[2]);
        if (num_threads <= 0 || num_threads > MAX_THREADS) {
            num_threads = DEFAULT_NUM_THREADS;
        }
    }

    /* Run benchmark */
    run_benchmark(array_size, num_threads);

    /* Run scalability analysis for larger arrays */
    if (array_size >= 100000) {
        run_scalability_analysis(array_size);
    }

    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    DEMONSTRATION COMPLETE                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    return EXIT_SUCCESS;
}
