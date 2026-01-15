/**
 * =============================================================================
 * EXERCISE 2: Heapsort Implementation and Performance Analysis
 * =============================================================================
 *
 * OBJECTIVE:
 *   Implement heapsort algorithm with Floyd's build-heap and analyse its
 *   performance compared to other sorting algorithms.
 *
 * REQUIREMENTS:
 *   1. Implement sift_down for max-heap
 *   2. Implement Floyd's build_max_heap (O(n) time)
 *   3. Implement in-place heapsort
 *   4. Add comparison counter for empirical analysis
 *   5. Benchmark against different input distributions
 *
 * EXAMPLE INPUT:
 *   Array: 64 34 25 12 22 11 90 5
 *
 * EXPECTED OUTPUT:
 *   Sorted: 5 11 12 22 25 34 64 90
 *   Comparisons: ~24 (O(n log n))
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -O2 -o exercise2 exercise2.c -lm
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

/* =============================================================================
 * CONSTANTS AND MACROS
 * =============================================================================
 */

/**
 * TODO 1: Define the heap navigation macros
 *
 * Same as exercise 1 - define for 0-indexed array:
 *   PARENT(i)      - index of parent node
 *   LEFT_CHILD(i)  - index of left child
 *   RIGHT_CHILD(i) - index of right child
 */

/* YOUR CODE HERE */
#define PARENT(i)       (0)  /* Replace with correct formula */
#define LEFT_CHILD(i)   (0)  /* Replace with correct formula */
#define RIGHT_CHILD(i)  (0)  /* Replace with correct formula */


/* Global comparison counter for analysis */
static unsigned long comparison_count = 0;

/**
 * Comparison macro that increments counter
 */
#define COMPARE(a, b) (comparison_count++, ((a) - (b)))

/**
 * Swap macro
 */
#define SWAP(arr, i, j) do { \
    int temp = (arr)[i]; \
    (arr)[i] = (arr)[j]; \
    (arr)[j] = temp; \
} while(0)


/* =============================================================================
 * HEAP OPERATIONS
 * =============================================================================
 */

/**
 * TODO 2: Implement sift_down for max-heap
 *
 * Move element at index i downward to restore max-heap property.
 *
 * @param arr   Array representing the heap
 * @param n     Size of the heap (not array size - important for heapsort!)
 * @param i     Index of element to sift down
 *
 * Steps:
 *   1. Loop until heap property restored:
 *      a. Calculate left and right child indices
 *      b. Find largest among node i and its children
 *      c. If i is largest, done
 *      d. Swap with largest child and continue from that position
 *
 * Hint: Use COMPARE(arr[left], arr[largest]) > 0 for comparison counting
 *       Always check bounds (left < n) before comparing
 */
void sift_down(int *arr, size_t n, size_t i) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 3: Implement sift_down_standard (without comparison counting)
 *
 * Same as sift_down but uses direct comparison without counter.
 * Used for operations where we don't need to count comparisons.
 */
void sift_down_standard(int *arr, size_t n, size_t i) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 4: Implement Floyd's build_max_heap
 *
 * Convert an arbitrary array into a valid max-heap.
 * Uses bottom-up approach for O(n) time complexity.
 *
 * @param arr   Array to heapify
 * @param n     Number of elements
 *
 * Key insight:
 *   - Leaves (indices n/2 to n-1) are already valid 1-element heaps
 *   - Only need to heapify internal nodes from bottom to top
 *
 * Steps:
 *   1. Find the last internal node: (n / 2) - 1
 *   2. For each internal node from bottom to root:
 *      - Call sift_down to fix that subtree
 */
void build_max_heap(int *arr, size_t n) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 5: Implement heapsort
 *
 * Sort array in ascending order using heap sort algorithm.
 *
 * @param arr   Array to sort
 * @param n     Number of elements
 *
 * Algorithm:
 *   Phase 1 - Build max-heap: O(n)
 *   Phase 2 - Extract elements: O(n log n)
 *
 * Steps:
 *   1. Build max-heap from array
 *   2. For i from n-1 down to 1:
 *      a. Swap arr[0] (maximum) with arr[i]
 *      b. Reduce heap size (now i elements)
 *      c. Sift down new root to restore heap property
 *
 * Result: Array sorted in ascending order
 */
void heapsort(int *arr, size_t n) {
    /* YOUR CODE HERE */
    
}


/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Reset comparison counter
 */
void reset_comparisons(void) {
    comparison_count = 0;
}

/**
 * Get comparison count
 */
unsigned long get_comparisons(void) {
    return comparison_count;
}

/**
 * Print array
 */
void print_array(const int *arr, size_t n, const char *label) {
    printf("%s: ", label);
    if (n <= 20) {
        for (size_t i = 0; i < n; i++) {
            printf("%d ", arr[i]);
        }
    } else {
        /* Print first and last few elements */
        for (size_t i = 0; i < 5; i++) printf("%d ", arr[i]);
        printf("... ");
        for (size_t i = n - 5; i < n; i++) printf("%d ", arr[i]);
    }
    printf("\n");
}

/**
 * TODO 6: Implement is_sorted
 *
 * Check if array is sorted in ascending order.
 *
 * @param arr   Array to check
 * @param n     Number of elements
 * @return      true if sorted, false otherwise
 */
bool is_sorted(const int *arr, size_t n) {
    /* YOUR CODE HERE */
    
    return false;  /* Replace this */
}

/**
 * TODO 7: Implement verify_max_heap
 *
 * Check if array satisfies max-heap property.
 *
 * @param arr   Array to check
 * @param n     Number of elements
 * @return      true if valid max-heap, false otherwise
 *
 * Hint: For each node, check that parent >= children
 */
bool verify_max_heap(const int *arr, size_t n) {
    /* YOUR CODE HERE */
    
    return false;  /* Replace this */
}

/**
 * Copy array
 */
void copy_array(const int *src, int *dst, size_t n) {
    memcpy(dst, src, n * sizeof(int));
}


/* =============================================================================
 * TEST DATA GENERATORS
 * =============================================================================
 */

/**
 * TODO 8: Generate random array
 *
 * Fill array with random values between 0 and max_val.
 */
void generate_random(int *arr, size_t n, int max_val) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 9: Generate sorted array (ascending)
 */
void generate_sorted(int *arr, size_t n) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 10: Generate reverse sorted array (descending)
 */
void generate_reverse_sorted(int *arr, size_t n) {
    /* YOUR CODE HERE */
    
}

/**
 * Generate array with many duplicates
 */
void generate_duplicates(int *arr, size_t n, int num_unique) {
    for (size_t i = 0; i < n; i++) {
        arr[i] = rand() % num_unique;
    }
}


/* =============================================================================
 * COMPARISON SORTS (for benchmarking)
 * =============================================================================
 */

/**
 * Standard library qsort comparison function
 */
int qsort_compare(const void *a, const void *b) {
    comparison_count++;
    return *(const int *)a - *(const int *)b;
}

/**
 * Insertion sort (for small arrays and comparison)
 */
void insertion_sort(int *arr, size_t n) {
    for (size_t i = 1; i < n; i++) {
        int key = arr[i];
        size_t j = i;
        while (j > 0 && COMPARE(arr[j-1], key) > 0) {
            arr[j] = arr[j-1];
            j--;
        }
        arr[j] = key;
    }
}


/* =============================================================================
 * BENCHMARKING
 * =============================================================================
 */

/**
 * TODO 11: Implement benchmark function
 *
 * Run heapsort on different input sizes and distributions,
 * recording comparisons and verifying correctness.
 *
 * @param sizes     Array of input sizes to test
 * @param num_sizes Number of sizes
 *
 * For each size:
 *   1. Generate test data (random, sorted, reverse)
 *   2. Reset comparison counter
 *   3. Run heapsort
 *   4. Verify result is sorted
 *   5. Record comparisons
 *   6. Compare with theoretical O(n log n)
 */
void run_benchmark(const size_t *sizes, size_t num_sizes) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    Performance Benchmark                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("%-10s | %-12s | %-12s | %-12s | %-10s\n",
           "Size", "Random", "Sorted", "Reverse", "n*log(n)");
    printf("────────────────────────────────────────────────────────────────\n");
    
    for (size_t s = 0; s < num_sizes; s++) {
        size_t n = sizes[s];
        
        int *arr = malloc(n * sizeof(int));
        int *backup = malloc(n * sizeof(int));
        
        if (!arr || !backup) {
            fprintf(stderr, "Memory allocation failed for size %zu\n", n);
            free(arr);
            free(backup);
            continue;
        }
        
        /* Theoretical comparison count */
        double theoretical = n * log2((double)n);
        
        /* YOUR CODE HERE: Run benchmarks for each distribution */
        
        /* Example structure (implement the actual benchmarking): */
        unsigned long random_comps = 0;
        unsigned long sorted_comps = 0;
        unsigned long reverse_comps = 0;
        
        /* Test random data */
        /* generate_random(arr, n, 1000000); */
        /* copy_array(arr, backup, n); */
        /* reset_comparisons(); */
        /* heapsort(arr, n); */
        /* random_comps = get_comparisons(); */
        /* if (!is_sorted(arr, n)) printf("ERROR: Not sorted!\n"); */
        
        /* Similar for sorted and reverse sorted... */
        
        printf("%-10zu | %-12lu | %-12lu | %-12lu | %-10.0f\n",
               n, random_comps, sorted_comps, reverse_comps, theoretical);
        
        free(arr);
        free(backup);
    }
    
    printf("\n");
}


/* =============================================================================
 * TEST FUNCTIONS
 * =============================================================================
 */

/**
 * Test heapsort correctness
 */
bool test_heapsort_correctness(void) {
    printf("Test 1: Heapsort Correctness\n");
    printf("────────────────────────────────────\n");
    
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 5, 87, 43};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    print_array(arr, n, "  Before");
    
    reset_comparisons();
    heapsort(arr, n);
    
    print_array(arr, n, "  After ");
    printf("  Comparisons: %lu\n", get_comparisons());
    
    if (!is_sorted(arr, n)) {
        printf("  FAILED: Array not sorted\n\n");
        return false;
    }
    
    printf("  PASSED\n\n");
    return true;
}

/**
 * Test build heap
 */
bool test_build_heap(void) {
    printf("Test 2: Build Max-Heap\n");
    printf("────────────────────────────────────\n");
    
    int arr[] = {4, 1, 3, 2, 16, 9, 10, 14, 8, 7};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    print_array(arr, n, "  Before");
    printf("  Is max-heap: %s\n", verify_max_heap(arr, n) ? "Yes" : "No");
    
    build_max_heap(arr, n);
    
    print_array(arr, n, "  After ");
    printf("  Is max-heap: %s\n", verify_max_heap(arr, n) ? "Yes" : "No");
    
    if (!verify_max_heap(arr, n)) {
        printf("  FAILED: Not a valid max-heap\n\n");
        return false;
    }
    
    /* Check max element is at root */
    int max = arr[0];
    for (size_t i = 1; i < n; i++) {
        if (arr[i] > max) {
            printf("  FAILED: Root is not maximum\n\n");
            return false;
        }
    }
    
    printf("  PASSED\n\n");
    return true;
}

/**
 * Test with special cases
 */
bool test_special_cases(void) {
    printf("Test 3: Special Cases\n");
    printf("────────────────────────────────────\n");
    
    /* Empty array */
    int empty[] = {};
    heapsort(empty, 0);
    printf("  Empty array: PASSED\n");
    
    /* Single element */
    int single[] = {42};
    heapsort(single, 1);
    if (single[0] != 42) {
        printf("  Single element: FAILED\n\n");
        return false;
    }
    printf("  Single element: PASSED\n");
    
    /* Two elements */
    int two[] = {2, 1};
    heapsort(two, 2);
    if (!is_sorted(two, 2)) {
        printf("  Two elements: FAILED\n\n");
        return false;
    }
    printf("  Two elements: PASSED\n");
    
    /* Already sorted */
    int sorted[] = {1, 2, 3, 4, 5};
    heapsort(sorted, 5);
    if (!is_sorted(sorted, 5)) {
        printf("  Already sorted: FAILED\n\n");
        return false;
    }
    printf("  Already sorted: PASSED\n");
    
    /* Reverse sorted */
    int reverse[] = {5, 4, 3, 2, 1};
    heapsort(reverse, 5);
    if (!is_sorted(reverse, 5)) {
        printf("  Reverse sorted: FAILED\n\n");
        return false;
    }
    printf("  Reverse sorted: PASSED\n");
    
    /* All same elements */
    int same[] = {7, 7, 7, 7, 7};
    heapsort(same, 5);
    if (!is_sorted(same, 5)) {
        printf("  All same: FAILED\n\n");
        return false;
    }
    printf("  All same: PASSED\n");
    
    printf("\n");
    return true;
}

/**
 * TODO 12: Test with large random array
 */
bool test_large_random(void) {
    printf("Test 4: Large Random Array (10000 elements)\n");
    printf("────────────────────────────────────\n");
    
    size_t n = 10000;
    int *arr = malloc(n * sizeof(int));
    
    if (!arr) {
        printf("  FAILED: Memory allocation\n\n");
        return false;
    }
    
    /* YOUR CODE HERE */
    /* Generate random array, sort, verify */
    
    free(arr);
    printf("  PASSED\n\n");
    return true;
}


/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 2: Heapsort Implementation                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Seed random number generator */
    srand((unsigned int)time(NULL));
    
    int passed = 0;
    int total = 4;
    
    if (test_heapsort_correctness()) passed++;
    if (test_build_heap()) passed++;
    if (test_special_cases()) passed++;
    if (test_large_random()) passed++;
    
    printf("════════════════════════════════════════════════════════════════\n");
    printf("Results: %d/%d tests passed\n", passed, total);
    
    if (passed == total) {
        printf("\n✓ All tests passed!\n");
        
        /* Run benchmark if requested */
        if (argc > 1 && strcmp(argv[1], "--benchmark") == 0) {
            size_t sizes[] = {100, 500, 1000, 5000, 10000, 50000};
            run_benchmark(sizes, sizeof(sizes) / sizeof(sizes[0]));
        } else {
            printf("\nRun with --benchmark flag for performance analysis.\n");
        }
    } else {
        printf("\n✗ Some tests failed. Review your implementation.\n");
    }
    
    printf("\n");
    
    return (passed == total) ? 0 : 1;
}


/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Implement a hybrid sort that uses insertion sort for small subarrays
 *    (threshold ~16 elements)
 *
 * 2. Compare heapsort against quicksort and mergesort:
 *    - Implement or use qsort
 *    - Measure time and comparisons
 *    - Generate CSV output for graphing
 *
 * 3. Implement bottom-up heapsort (avoids half of comparisons)
 *
 * 4. Implement smoothsort (adaptive heapsort variant)
 *
 * 5. Analyse cache performance: count cache misses using perf tool
 *    Compare heapsort vs mergesort vs quicksort
 *
 * =============================================================================
 */
