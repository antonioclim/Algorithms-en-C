/**
 * =============================================================================
 * EXERCISE 1: Algorithm Benchmarking Suite
 * =============================================================================
 *
 * OBJECTIVE:
 *   Build a comprehensive benchmarking framework that compares different
 *   implementations of common algorithms, measuring execution time and
 *   analysing complexity through empirical observation.
 *
 * REQUIREMENTS:
 *   1. Implement three sorting algorithm variants
 *   2. Create a timing function using clock()
 *   3. Generate test data of varying sizes
 *   4. Compare theoretical vs empirical complexity
 *   5. Export results to a CSV file
 *
 * EXAMPLE INPUT:
 *   (No direct input - generates test data internally)
 *
 * EXPECTED OUTPUT:
 *   Benchmarking Algorithm Suite
 *   ============================
 *   
 *   Running benchmarks for n = 100, 500, 1000, 2000, 5000
 *   
 *   Results:
 *   Size  | SelectSort |  QuickSort |  MergeSort
 *   ------|------------|------------|------------
 *    100  |    0.05 ms |    0.01 ms |    0.02 ms
 *    500  |    1.20 ms |    0.08 ms |    0.12 ms
 *   1000  |    4.80 ms |    0.18 ms |    0.26 ms
 *   ...
 *   
 *   Results exported to benchmark_output.csv
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

/* =============================================================================
 * CONFIGURATION
 * =============================================================================
 */

#define MAX_ARRAY_SIZE      10000
#define NUM_TEST_SIZES      5
#define NUM_ALGORITHMS      3
#define BENCHMARK_RUNS      3
#define OUTPUT_FILENAME     "benchmark_output.csv"

/* Test sizes to benchmark */
static const int TEST_SIZES[NUM_TEST_SIZES] = {100, 500, 1000, 2000, 5000};

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * TODO 1: Define a function pointer type for sorting algorithms
 *
 * All sorting functions should take:
 *   - int *arr: pointer to array to sort
 *   - int n: number of elements
 * 
 * They should return void (sort in-place).
 *
 * Hint: typedef void (*TypeName)(int *, int);
 */

/* YOUR CODE HERE */


/**
 * Structure to hold benchmark results for one algorithm.
 */
typedef struct {
    char name[32];              /* Algorithm name */
    double times[NUM_TEST_SIZES]; /* Execution times in ms */
} BenchmarkResult;

/* =============================================================================
 * UTILITY FUNCTIONS (PROVIDED)
 * =============================================================================
 */

/**
 * Copy array contents.
 */
static void copy_array(const int *src, int *dest, int n) {
    memcpy(dest, src, n * sizeof(int));
}

/**
 * Check if array is sorted.
 */
static bool is_sorted(const int *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) return false;
    }
    return true;
}

/**
 * Print array (first 10 elements if large).
 */
static void print_array(const int *arr, int n) {
    int limit = (n > 10) ? 10 : n;
    printf("[");
    for (int i = 0; i < limit; i++) {
        printf("%d", arr[i]);
        if (i < limit - 1) printf(", ");
    }
    if (n > 10) printf(", ...");
    printf("]\n");
}

/* =============================================================================
 * SORTING ALGORITHMS
 * =============================================================================
 */

/**
 * TODO 2: Implement Selection Sort
 *
 * Selection Sort algorithm:
 *   1. Find the minimum element in unsorted portion
 *   2. Swap it with the first unsorted element
 *   3. Move boundary one element right
 *   4. Repeat until sorted
 *
 * Time Complexity: O(n²) for all cases
 * Space Complexity: O(1)
 *
 * @param arr Array to sort
 * @param n   Number of elements
 */
void selection_sort(int *arr, int n) {
    /* YOUR CODE HERE */
    
    /* Hint: Use nested loops
     *   - Outer loop: i from 0 to n-1
     *   - Find minimum index from i to n-1
     *   - Swap arr[i] with arr[min_idx]
     */
}

/**
 * TODO 3: Implement Quick Sort partition function
 *
 * Lomuto partition scheme:
 *   - Choose last element as pivot
 *   - Partition array so elements < pivot are left, >= pivot are right
 *   - Return final pivot position
 *
 * @param arr  Array to partition
 * @param low  Starting index
 * @param high Ending index (pivot location)
 * @return     Final position of pivot
 */
static int partition(int *arr, int low, int high) {
    /* YOUR CODE HERE */
    
    /* Hint:
     *   int pivot = arr[high];
     *   int i = low - 1;
     *   for j from low to high-1:
     *       if arr[j] <= pivot:
     *           i++
     *           swap arr[i] and arr[j]
     *   swap arr[i+1] and arr[high]
     *   return i + 1
     */
    
    return low;  /* Replace this */
}

/**
 * TODO 4: Implement Quick Sort recursive function
 *
 * Quick Sort algorithm:
 *   1. If low < high:
 *   2. Partition the array
 *   3. Recursively sort left partition
 *   4. Recursively sort right partition
 *
 * @param arr  Array to sort
 * @param low  Starting index
 * @param high Ending index
 */
static void quicksort_recursive(int *arr, int low, int high) {
    /* YOUR CODE HERE */
    
    /* Hint:
     *   if (low < high) {
     *       int pi = partition(arr, low, high);
     *       quicksort_recursive(arr, low, pi - 1);
     *       quicksort_recursive(arr, pi + 1, high);
     *   }
     */
}

/**
 * Quick Sort wrapper function (PROVIDED).
 */
void quick_sort(int *arr, int n) {
    if (n > 1) {
        quicksort_recursive(arr, 0, n - 1);
    }
}

/**
 * TODO 5: Implement Merge function for Merge Sort
 *
 * Merge two sorted subarrays:
 *   - arr[left..mid] and arr[mid+1..right]
 *   - Use temporary arrays for merging
 *   - Free temporary memory after use
 *
 * @param arr   Array containing both subarrays
 * @param left  Start of first subarray
 * @param mid   End of first subarray
 * @param right End of second subarray
 */
static void merge(int *arr, int left, int mid, int right) {
    /* YOUR CODE HERE */
    
    /* Hint:
     *   1. Calculate sizes: n1 = mid - left + 1, n2 = right - mid
     *   2. Allocate temp arrays L and R
     *   3. Copy data to temp arrays
     *   4. Merge back: compare L[i] and R[j], copy smaller
     *   5. Copy remaining elements
     *   6. Free temp arrays
     */
}

/**
 * TODO 6: Implement Merge Sort recursive function
 *
 * Merge Sort algorithm (divide and conquer):
 *   1. If left < right:
 *   2. Find middle point
 *   3. Recursively sort first half
 *   4. Recursively sort second half
 *   5. Merge the sorted halves
 *
 * @param arr   Array to sort
 * @param left  Starting index
 * @param right Ending index
 */
static void mergesort_recursive(int *arr, int left, int right) {
    /* YOUR CODE HERE */
    
    /* Hint:
     *   if (left < right) {
     *       int mid = left + (right - left) / 2;
     *       mergesort_recursive(arr, left, mid);
     *       mergesort_recursive(arr, mid + 1, right);
     *       merge(arr, left, mid, right);
     *   }
     */
}

/**
 * Merge Sort wrapper function (PROVIDED).
 */
void merge_sort(int *arr, int n) {
    if (n > 1) {
        mergesort_recursive(arr, 0, n - 1);
    }
}

/* =============================================================================
 * BENCHMARKING FUNCTIONS
 * =============================================================================
 */

/**
 * TODO 7: Implement array generator with random values
 *
 * Generate an array of n random integers in range [0, max_val).
 *
 * @param n       Number of elements
 * @param max_val Maximum value (exclusive)
 * @return        Pointer to allocated array, or NULL on failure
 *
 * Steps:
 *   1. Allocate memory for n integers
 *   2. Check for allocation failure
 *   3. Fill with random values using rand() % max_val
 *   4. Return pointer
 */
int *generate_random_array(int n, int max_val) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace this */
}

/**
 * TODO 8: Implement the timing function
 *
 * Measure execution time of a sorting algorithm.
 * Run the algorithm BENCHMARK_RUNS times and return average time.
 *
 * @param sort_func Function pointer to sorting algorithm
 * @param original  Original array (will be copied for each run)
 * @param n         Array size
 * @return          Average execution time in milliseconds
 *
 * Steps:
 *   1. Allocate a copy array
 *   2. For each run:
 *      a. Copy original to copy array
 *      b. Record start time with clock()
 *      c. Call sort_func(copy, n)
 *      d. Record end time
 *      e. Add elapsed time to total
 *   3. Free copy array
 *   4. Return average time in milliseconds
 *
 * Hint: time_ms = ((double)(end - start) * 1000.0) / CLOCKS_PER_SEC
 */
double measure_sort_time(void (*sort_func)(int *, int), 
                         const int *original, int n) {
    /* YOUR CODE HERE */
    
    return 0.0;  /* Replace this */
}

/* =============================================================================
 * FILE OUTPUT
 * =============================================================================
 */

/**
 * TODO 9: Export benchmark results to CSV file
 *
 * Create a CSV file with format:
 *   Size,SelectionSort,QuickSort,MergeSort
 *   100,0.05,0.01,0.02
 *   500,1.20,0.08,0.12
 *   ...
 *
 * @param results Array of BenchmarkResult structures
 * @param count   Number of results
 * @param filename Output filename
 * @return        0 on success, -1 on failure
 *
 * Steps:
 *   1. Open file for writing
 *   2. Write header row
 *   3. For each test size:
 *      a. Write size
 *      b. Write time for each algorithm (comma-separated)
 *   4. Close file
 */
int export_results_csv(const BenchmarkResult *results, int count, 
                       const char *filename) {
    /* YOUR CODE HERE */
    
    return -1;  /* Replace with 0 on success */
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  Algorithm Benchmarking Suite                                 ║\n");
    printf("║  Exercise 1 - Week 14                                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Seed random number generator */
    srand((unsigned int)time(NULL));
    
    /* TODO 10: Create array of function pointers to sorting algorithms
     *
     * Create an array that holds pointers to:
     *   - selection_sort
     *   - quick_sort
     *   - merge_sort
     *
     * Hint: SortFunction algorithms[] = {selection_sort, quick_sort, merge_sort};
     * (Requires TODO 1 to be completed first)
     */
    
    /* YOUR CODE HERE */
    
    /* Algorithm names for display */
    const char *algo_names[NUM_ALGORITHMS] = {
        "SelectSort", "QuickSort", "MergeSort"
    };
    
    /* Storage for benchmark results */
    BenchmarkResult results[NUM_ALGORITHMS];
    for (int i = 0; i < NUM_ALGORITHMS; i++) {
        strncpy(results[i].name, algo_names[i], 31);
        results[i].name[31] = '\0';
    }
    
    /* Print header */
    printf("Running benchmarks with %d runs per test...\n\n", BENCHMARK_RUNS);
    printf("%-6s", "Size");
    for (int a = 0; a < NUM_ALGORITHMS; a++) {
        printf(" | %12s", algo_names[a]);
    }
    printf("\n");
    printf("%-6s", "------");
    for (int a = 0; a < NUM_ALGORITHMS; a++) {
        printf("-+-%12s", "------------");
    }
    printf("\n");
    
    /* TODO 11: Run benchmarks for each test size
     *
     * For each size in TEST_SIZES:
     *   1. Generate random array using generate_random_array()
     *   2. For each algorithm:
     *      a. Measure time using measure_sort_time()
     *      b. Store result in results[algo].times[size_idx]
     *      c. Print formatted time
     *   3. Free the random array
     *
     * Expected output format per row:
     *   printf("%5d ", size);
     *   printf(" | %9.3f ms", time);
     */
    
    /* YOUR CODE HERE */
    
    /* Placeholder output (remove when implementing) */
    printf("(Benchmark implementation pending - complete TODOs 1-10)\n");
    
    /* TODO 12: Export results and verify sorting correctness
     *
     * Steps:
     *   1. Call export_results_csv() to save results
     *   2. Generate a small test array
     *   3. Sort with each algorithm
     *   4. Verify using is_sorted()
     *   5. Print verification results
     */
    
    /* YOUR CODE HERE */
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  Benchmarking complete                                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Add Heap Sort to the benchmark suite
 * 2. Implement worst-case input generation (sorted, reverse-sorted)
 * 3. Add memory usage tracking using mallinfo() on Linux
 * 4. Create a bar chart visualisation using ASCII art
 * 5. Implement adaptive sorting that chooses algorithm based on input size
 *
 * =============================================================================
 */
