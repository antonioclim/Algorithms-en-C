/**
 * =============================================================================
 * EXERCISE 2 SOLUTION: Heapsort Implementation and Performance Analysis
 * =============================================================================
 *
 * INSTRUCTOR SOLUTION - Complete implementation with all TODOs filled
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -O2 -o exercise2_sol exercise2_sol.c -lm
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

/* TODO 1 SOLUTION: Heap navigation macros */
#define PARENT(i)       (((i) - 1) / 2)
#define LEFT_CHILD(i)   (2 * (i) + 1)
#define RIGHT_CHILD(i)  (2 * (i) + 2)

static unsigned long comparison_count = 0;

#define COMPARE(a, b) (comparison_count++, ((a) - (b)))

#define SWAP(arr, i, j) do { \
    int temp = (arr)[i]; \
    (arr)[i] = (arr)[j]; \
    (arr)[j] = temp; \
} while(0)


/* =============================================================================
 * HEAP OPERATIONS
 * =============================================================================
 */

/* TODO 2 SOLUTION: Sift-down with comparison counting */
void sift_down(int *arr, size_t n, size_t i) {
    while (true) {
        size_t largest = i;
        size_t left = LEFT_CHILD(i);
        size_t right = RIGHT_CHILD(i);
        
        if (left < n && COMPARE(arr[left], arr[largest]) > 0)
            largest = left;
        
        if (right < n && COMPARE(arr[right], arr[largest]) > 0)
            largest = right;
        
        if (largest == i)
            break;
        
        SWAP(arr, i, largest);
        i = largest;
    }
}

/* TODO 3 SOLUTION: Standard sift-down (no counting) */
void sift_down_standard(int *arr, size_t n, size_t i) {
    while (true) {
        size_t largest = i;
        size_t left = LEFT_CHILD(i);
        size_t right = RIGHT_CHILD(i);
        
        if (left < n && arr[left] > arr[largest])
            largest = left;
        
        if (right < n && arr[right] > arr[largest])
            largest = right;
        
        if (largest == i)
            break;
        
        SWAP(arr, i, largest);
        i = largest;
    }
}

/* TODO 4 SOLUTION: Floyd's build_max_heap */
void build_max_heap(int *arr, size_t n) {
    /* Start from the last internal node */
    for (int i = (int)(n / 2) - 1; i >= 0; i--) {
        sift_down(arr, n, (size_t)i);
    }
}

/* TODO 5 SOLUTION: Heapsort */
void heapsort(int *arr, size_t n) {
    if (n < 2) return;
    
    /* Phase 1: Build max-heap */
    build_max_heap(arr, n);
    
    /* Phase 2: Extract elements */
    for (size_t i = n - 1; i > 0; i--) {
        /* Move max to end */
        SWAP(arr, 0, i);
        /* Restore heap on reduced array */
        sift_down(arr, i, 0);
    }
}


/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

void reset_comparisons(void) {
    comparison_count = 0;
}

unsigned long get_comparisons(void) {
    return comparison_count;
}

void print_array(const int *arr, size_t n, const char *label) {
    printf("%s: ", label);
    if (n <= 20) {
        for (size_t i = 0; i < n; i++) {
            printf("%d ", arr[i]);
        }
    } else {
        for (size_t i = 0; i < 5; i++) printf("%d ", arr[i]);
        printf("... ");
        for (size_t i = n - 5; i < n; i++) printf("%d ", arr[i]);
    }
    printf("\n");
}

/* TODO 6 SOLUTION: is_sorted */
bool is_sorted(const int *arr, size_t n) {
    for (size_t i = 1; i < n; i++) {
        if (arr[i] < arr[i - 1])
            return false;
    }
    return true;
}

/* TODO 7 SOLUTION: verify_max_heap */
bool verify_max_heap(const int *arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        size_t left = LEFT_CHILD(i);
        size_t right = RIGHT_CHILD(i);
        
        if (left < n && arr[left] > arr[i])
            return false;
        if (right < n && arr[right] > arr[i])
            return false;
    }
    return true;
}

void copy_array(const int *src, int *dst, size_t n) {
    memcpy(dst, src, n * sizeof(int));
}


/* =============================================================================
 * TEST DATA GENERATORS
 * =============================================================================
 */

/* TODO 8 SOLUTION: Generate random */
void generate_random(int *arr, size_t n, int max_val) {
    for (size_t i = 0; i < n; i++) {
        arr[i] = rand() % (max_val + 1);
    }
}

/* TODO 9 SOLUTION: Generate sorted */
void generate_sorted(int *arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        arr[i] = (int)i;
    }
}

/* TODO 10 SOLUTION: Generate reverse sorted */
void generate_reverse_sorted(int *arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        arr[i] = (int)(n - 1 - i);
    }
}

void generate_duplicates(int *arr, size_t n, int num_unique) {
    for (size_t i = 0; i < n; i++) {
        arr[i] = rand() % num_unique;
    }
}


/* =============================================================================
 * COMPARISON SORTS
 * =============================================================================
 */

int qsort_compare(const void *a, const void *b) {
    comparison_count++;
    return *(const int *)a - *(const int *)b;
}

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

/* TODO 11 SOLUTION: Benchmark function */
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
        
        double theoretical = n * log2((double)n);
        
        /* Test random data */
        generate_random(arr, n, 1000000);
        copy_array(arr, backup, n);
        reset_comparisons();
        heapsort(arr, n);
        unsigned long random_comps = get_comparisons();
        if (!is_sorted(arr, n)) {
            printf("ERROR: Random not sorted!\n");
        }
        
        /* Test sorted data */
        generate_sorted(arr, n);
        reset_comparisons();
        heapsort(arr, n);
        unsigned long sorted_comps = get_comparisons();
        if (!is_sorted(arr, n)) {
            printf("ERROR: Sorted not sorted!\n");
        }
        
        /* Test reverse sorted data */
        generate_reverse_sorted(arr, n);
        reset_comparisons();
        heapsort(arr, n);
        unsigned long reverse_comps = get_comparisons();
        if (!is_sorted(arr, n)) {
            printf("ERROR: Reverse not sorted!\n");
        }
        
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

bool test_build_heap(void) {
    printf("Test 2: Build Max-Heap\n");
    printf("────────────────────────────────────\n");
    
    int arr[] = {4, 1, 3, 2, 16, 9, 10, 14, 8, 7};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    print_array(arr, n, "  Before");
    printf("  Is max-heap: %s\n", verify_max_heap(arr, n) ? "Yes" : "No");
    
    reset_comparisons();
    build_max_heap(arr, n);
    
    print_array(arr, n, "  After ");
    printf("  Is max-heap: %s\n", verify_max_heap(arr, n) ? "Yes" : "No");
    
    if (!verify_max_heap(arr, n)) {
        printf("  FAILED: Not a valid max-heap\n\n");
        return false;
    }
    
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

bool test_special_cases(void) {
    printf("Test 3: Special Cases\n");
    printf("────────────────────────────────────\n");
    
    /* Empty array */
    int empty[] = {0};
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

/* TODO 12 SOLUTION: Test large random */
bool test_large_random(void) {
    printf("Test 4: Large Random Array (10000 elements)\n");
    printf("────────────────────────────────────\n");
    
    size_t n = 10000;
    int *arr = malloc(n * sizeof(int));
    
    if (!arr) {
        printf("  FAILED: Memory allocation\n\n");
        return false;
    }
    
    /* Generate random array */
    generate_random(arr, n, 1000000);
    
    /* Sort */
    reset_comparisons();
    heapsort(arr, n);
    
    /* Verify sorted */
    if (!is_sorted(arr, n)) {
        printf("  FAILED: Not sorted\n\n");
        free(arr);
        return false;
    }
    
    
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
