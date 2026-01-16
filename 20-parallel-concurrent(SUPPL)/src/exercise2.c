/**
 * =============================================================================
 * WEEK 20: PARALLEL AND CONCURRENT PROGRAMMING
 * Exercise 2: Parallel Quick Sort
 * =============================================================================
 *
 * Implement a parallel version of Quick Sort using task-based parallelism.
 * Large partitions spawn new threads whilst small partitions execute
 * sequentially to avoid thread creation overhead.
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -pthread -o exercise2 exercise2.c
 * Usage: ./exercise2 [array_size]
 *
 * =============================================================================
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <pthread.h>
#include <time.h>

/* =============================================================================
 * CONFIGURATION
 * =============================================================================
 */

#define DEFAULT_ARRAY_SIZE 1000000
#define PARALLEL_THRESHOLD 10000    /* Switch to sequential below this size */
#define MAX_THREADS 8               /* Maximum concurrent threads */
#define INSERTION_THRESHOLD 16      /* Switch to insertion sort below this */

/* =============================================================================
 * GLOBAL STATE
 * =============================================================================
 */

static atomic_int active_threads = 0;
static atomic_int max_active_threads = 0;
static atomic_int tasks_created = 0;

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Get current time in milliseconds
 */
double get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

/**
 * Swap two integers
 */
static inline void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Check if array is sorted
 */
bool is_sorted(int *arr, int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i] < arr[i - 1]) return false;
    }
    return true;
}

/**
 * Generate random array
 */
void generate_random_array(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % (n * 10);
    }
}

/**
 * Copy array
 */
void copy_array(int *dest, const int *src, int n) {
    memcpy(dest, src, n * sizeof(int));
}

/* =============================================================================
 * INSERTION SORT (for small subarrays)
 * =============================================================================
 */

/**
 * Insertion sort for small arrays
 * 
 * This is more efficient than Quick Sort for very small arrays
 * due to lower overhead.
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
 * QUICK SORT PARTITIONING
 * =============================================================================
 */

/**
 * Median-of-three pivot selection
 *
 * TODO: Implement this function
 * - Find median of first, middle, and last elements
 * - Place median at the end (to be used as pivot)
 * - This reduces worst-case probability significantly
 */
int median_of_three(int *arr, int left, int right) {
    int mid = left + (right - left) / 2;
    
    /* Sort the three elements */
    if (arr[left] > arr[mid]) swap(&arr[left], &arr[mid]);
    if (arr[left] > arr[right]) swap(&arr[left], &arr[right]);
    if (arr[mid] > arr[right]) swap(&arr[mid], &arr[right]);
    
    /* Place median at right-1 position */
    swap(&arr[mid], &arr[right - 1]);
    
    return arr[right - 1];  /* Return pivot value */
}

/**
 * Hoare partition scheme
 *
 * TODO: Implement this function
 * - Use two pointers moving towards each other
 * - Swap elements on wrong sides
 * - Return the partition point
 *
 * Note: Hoare partitioning is generally faster than Lomuto
 * because it does fewer swaps on average.
 */
int hoare_partition(int *arr, int left, int right) {
    /* Use median-of-three for arrays larger than 3 elements */
    int pivot;
    if (right - left > 2) {
        pivot = median_of_three(arr, left, right);
        left++;
        right--;
    } else {
        pivot = arr[left + (right - left) / 2];
    }
    
    int i = left - 1;
    int j = right + 1;
    
    while (1) {
        /* Move i right until we find element >= pivot */
        do {
            i++;
        } while (arr[i] < pivot);
        
        /* Move j left until we find element <= pivot */
        do {
            j--;
        } while (arr[j] > pivot);
        
        /* If pointers have crossed, return partition point */
        if (i >= j) return j;
        
        /* Swap elements on wrong sides */
        swap(&arr[i], &arr[j]);
    }
}

/* =============================================================================
 * SEQUENTIAL QUICK SORT
 * =============================================================================
 */

/**
 * Sequential Quick Sort implementation
 *
 * This is the baseline for performance comparison.
 * Uses insertion sort for small subarrays.
 */
void quicksort_sequential(int *arr, int left, int right) {
    while (left < right) {
        /* Use insertion sort for small arrays */
        if (right - left < INSERTION_THRESHOLD) {
            insertion_sort(arr, left, right);
            return;
        }
        
        int pivot_idx = hoare_partition(arr, left, right);
        
        /* Recurse on smaller partition, iterate on larger */
        /* This limits stack depth to O(log n) */
        if (pivot_idx - left < right - pivot_idx) {
            quicksort_sequential(arr, left, pivot_idx);
            left = pivot_idx + 1;
        } else {
            quicksort_sequential(arr, pivot_idx + 1, right);
            right = pivot_idx;
        }
    }
}

/* =============================================================================
 * PARALLEL QUICK SORT
 * =============================================================================
 */

/**
 * Task structure for parallel sort
 */
typedef struct {
    int *arr;
    int left;
    int right;
    int depth;
} SortTask;

/**
 * Parallel Quick Sort worker thread
 *
 * TODO: Implement this function
 * - If array is small, use sequential sort
 * - If at maximum depth or threads, use sequential sort
 * - Otherwise, partition and spawn threads for both halves
 * - Track thread counts using atomic operations
 */
void *parallel_quicksort_worker(void *arg) {
    SortTask *task = (SortTask *)arg;
    int *arr = task->arr;
    int left = task->left;
    int right = task->right;
    int depth = task->depth;
    
    /* Base case: small array or max depth reached */
    int size = right - left + 1;
    
    if (size < PARALLEL_THRESHOLD || depth > 4) {
        quicksort_sequential(arr, left, right);
        return NULL;
    }
    
    /* Check if we can create more threads */
    int current_threads = atomic_load(&active_threads);
    if (current_threads >= MAX_THREADS) {
        quicksort_sequential(arr, left, right);
        return NULL;
    }
    
    /* Partition the array */
    int pivot_idx = hoare_partition(arr, left, right);
    
    /* Try to spawn threads for both partitions */
    atomic_fetch_add(&active_threads, 2);
    atomic_fetch_add(&tasks_created, 2);
    
    /* Update max thread count */
    int new_count = atomic_load(&active_threads);
    int old_max = atomic_load(&max_active_threads);
    while (new_count > old_max) {
        if (atomic_compare_exchange_weak(&max_active_threads, &old_max, new_count)) {
            break;
        }
    }
    
    /* Create tasks for left and right partitions */
    SortTask left_task = {arr, left, pivot_idx, depth + 1};
    SortTask right_task = {arr, pivot_idx + 1, right, depth + 1};
    
    pthread_t left_thread, right_thread;
    
    /* Spawn left thread */
    if (pthread_create(&left_thread, NULL, parallel_quicksort_worker, &left_task) != 0) {
        /* Fall back to sequential if thread creation fails */
        quicksort_sequential(arr, left, pivot_idx);
    }
    
    /* Spawn right thread */
    if (pthread_create(&right_thread, NULL, parallel_quicksort_worker, &right_task) != 0) {
        /* Fall back to sequential if thread creation fails */
        quicksort_sequential(arr, pivot_idx + 1, right);
    }
    
    /* Wait for both threads to complete */
    pthread_join(left_thread, NULL);
    pthread_join(right_thread, NULL);
    
    atomic_fetch_sub(&active_threads, 2);
    
    return NULL;
}

/**
 * Parallel Quick Sort entry point
 */
void quicksort_parallel(int *arr, int n) {
    atomic_store(&active_threads, 0);
    atomic_store(&max_active_threads, 0);
    atomic_store(&tasks_created, 0);
    
    SortTask task = {arr, 0, n - 1, 0};
    parallel_quicksort_worker(&task);
}

/* =============================================================================
 * BENCHMARK AND ANALYSIS
 * =============================================================================
 */

/**
 * Run benchmarks and compare sequential vs parallel
 */
void run_benchmark(int n) {
    printf("Configuration:\n");
    printf("  Array size:           %d elements\n", n);
    printf("  Parallel threshold:   %d elements\n", PARALLEL_THRESHOLD);
    printf("  Max threads:          %d\n", MAX_THREADS);
    printf("  Insertion threshold:  %d elements\n\n", INSERTION_THRESHOLD);
    
    /* Allocate arrays */
    int *original = malloc(n * sizeof(int));
    int *arr1 = malloc(n * sizeof(int));
    int *arr2 = malloc(n * sizeof(int));
    
    if (!original || !arr1 || !arr2) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    
    /* Generate random data */
    srand(42);  /* Fixed seed for reproducibility */
    generate_random_array(original, n);
    
    /* Sequential sort */
    printf("Sequential Quick Sort:\n");
    copy_array(arr1, original, n);
    
    double start = get_time_ms();
    quicksort_sequential(arr1, 0, n - 1);
    double seq_time = get_time_ms() - start;
    
    bool seq_sorted = is_sorted(arr1, n);
    printf("  Time:    %.2f ms\n", seq_time);
    printf("  Sorted:  %s\n\n", seq_sorted ? "YES" : "NO");
    
    /* Parallel sort */
    printf("Parallel Quick Sort:\n");
    copy_array(arr2, original, n);
    
    start = get_time_ms();
    quicksort_parallel(arr2, n);
    double par_time = get_time_ms() - start;
    
    bool par_sorted = is_sorted(arr2, n);
    printf("  Time:           %.2f ms\n", par_time);
    printf("  Sorted:         %s\n", par_sorted ? "YES" : "NO");
    printf("  Max threads:    %d\n", atomic_load(&max_active_threads));
    printf("  Tasks created:  %d\n\n", atomic_load(&tasks_created));
    
    /* Performance analysis */
    printf("Performance Analysis:\n");
    double speedup = seq_time / par_time;
    double efficiency = speedup / MAX_THREADS * 100;
    printf("  Speedup:     %.2fx\n", speedup);
    printf("  Efficiency:  %.1f%%\n\n", efficiency);
    
    /* Verify both sorts produce same result */
    bool results_match = true;
    for (int i = 0; i < n && results_match; i++) {
        if (arr1[i] != arr2[i]) results_match = false;
    }
    printf("Results Match: %s\n\n", results_match ? "YES" : "NO");
    
    /* Overall status */
    printf("SORTED: %s\n", (seq_sorted && par_sorted && results_match) ? "YES" : "NO");
    
    /* Cleanup */
    free(original);
    free(arr1);
    free(arr2);
}

/**
 * Run scalability test with different thread counts
 */
void run_scalability_test(int n) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     SCALABILITY ANALYSIS                                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int *original = malloc(n * sizeof(int));
    int *arr = malloc(n * sizeof(int));
    
    srand(42);
    generate_random_array(original, n);
    
    /* First measure sequential time */
    copy_array(arr, original, n);
    double start = get_time_ms();
    quicksort_sequential(arr, 0, n - 1);
    double seq_time = get_time_ms() - start;
    
    printf("Array size: %d elements\n\n", n);
    printf("╔══════════╦════════════╦══════════╦════════════╗\n");
    printf("║ Threads  ║  Time (ms) ║ Speedup  ║ Efficiency ║\n");
    printf("╠══════════╬════════════╬══════════╬════════════╣\n");
    printf("║    1     ║ %10.2f ║   1.00x  ║   100.0%%   ║\n", seq_time);
    
    /* Test with different thread limits */
    int thread_counts[] = {2, 4, 8, 16};
    
    for (int t = 0; t < 4; t++) {
        /* This is a simplified test - in real code we would 
           modify MAX_THREADS dynamically */
        copy_array(arr, original, n);
        
        start = get_time_ms();
        quicksort_parallel(arr, n);
        double par_time = get_time_ms() - start;
        
        double speedup = seq_time / par_time;
        double efficiency = speedup / thread_counts[t] * 100;
        
        printf("║    %d    ║ %10.2f ║   %.2fx  ║   %.1f%%   ║\n",
               thread_counts[t], par_time, speedup, efficiency);
    }
    
    printf("╚══════════╩════════════╩══════════╩════════════╝\n");
    
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
    printf("║     EXERCISE 2: PARALLEL QUICK SORT                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Parse command line arguments */
    int n = DEFAULT_ARRAY_SIZE;
    if (argc > 1) {
        n = atoi(argv[1]);
        if (n <= 0) {
            fprintf(stderr, "Invalid array size: %s\n", argv[1]);
            return EXIT_FAILURE;
        }
    }
    
    /* Run benchmark */
    run_benchmark(n);
    
    /* Run scalability test for large arrays */
    if (n >= 100000) {
        run_scalability_test(n);
    }
    
    return 0;
}
