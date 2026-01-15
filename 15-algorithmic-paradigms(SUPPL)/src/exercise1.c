/**
 * =============================================================================
 * EXERCISE 1: Sorting Algorithm Implementation and Analysis
 * =============================================================================
 *
 * OBJECTIVE:
 *   Implement multiple sorting algorithms and compare their performance
 *   characteristics through instrumentation.
 *
 * REQUIREMENTS:
 *   1. Implement MergeSort with auxiliary array
 *   2. Implement QuickSort with three pivot selection strategies
 *   3. Implement ShellSort with Hibbard gaps (2^k - 1)
 *   4. Add comparison and swap counters to each algorithm
 *   5. Generate and display performance statistics
 *
 * EXAMPLE INPUT:
 *   7
 *   64 34 25 12 22 11 90
 *
 * EXPECTED OUTPUT:
 *   Original: 64 34 25 12 22 11 90
 *   
 *   MergeSort:    11 12 22 25 34 64 90  [Comparisons: 13, Swaps: 0]
 *   QuickSort:    11 12 22 25 34 64 90  [Comparisons: 14, Swaps: 8]
 *   ShellSort:    11 12 22 25 34 64 90  [Comparisons: 18, Swaps: 9]
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c -lm
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* =============================================================================
 * GLOBAL STATISTICS
 * =============================================================================
 */

/**
 * Structure to hold sorting statistics
 */
typedef struct {
    long comparisons;
    long swaps;
    double time_ms;
} SortStats;

/* Global counters (reset before each sort) */
static long g_comparisons = 0;
static long g_swaps = 0;

/**
 * Reset global counters
 */
void reset_stats(void) {
    g_comparisons = 0;
    g_swaps = 0;
}

/**
 * Get current stats
 */
SortStats get_stats(void) {
    SortStats s = {g_comparisons, g_swaps, 0.0};
    return s;
}

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Swap two integers and increment swap counter
 */
void swap_counted(int *a, int *b) {
    /* TODO 1: Implement swap and increment g_swaps
     *
     * Steps:
     *   1. Swap the values at *a and *b using a temporary variable
     *   2. Increment g_swaps
     */
    
    /* YOUR CODE HERE */
}

/**
 * Compare two integers and increment comparison counter
 * Returns: negative if a < b, zero if a == b, positive if a > b
 */
int compare_counted(int a, int b) {
    g_comparisons++;
    return a - b;
}

/**
 * Copy array
 */
void copy_array(int dest[], int src[], int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

/**
 * Print array
 */
void print_array(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
}

/* =============================================================================
 * MERGESORT IMPLEMENTATION
 * =============================================================================
 */

/**
 * TODO 2: Implement the merge function
 *
 * Merge two sorted subarrays arr[left..mid] and arr[mid+1..right]
 * Use compare_counted() for all comparisons.
 *
 * @param arr The array containing both subarrays
 * @param left Left index of first subarray
 * @param mid Middle index (end of first subarray)
 * @param right Right index (end of second subarray)
 *
 * Hint:
 *   1. Calculate sizes: n1 = mid - left + 1, n2 = right - mid
 *   2. Create temporary arrays L[n1] and R[n2]
 *   3. Copy data to temp arrays
 *   4. Merge using compare_counted() and direct assignment (no swap needed)
 *   5. Free temporary arrays
 */
void merge(int arr[], int left, int mid, int right) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 3: Implement MergeSort
 *
 * Recursively sort the array using divide and conquer.
 *
 * @param arr Array to sort
 * @param left Left index
 * @param right Right index
 *
 * Hint:
 *   1. Base case: left >= right (nothing to sort)
 *   2. Find mid = left + (right - left) / 2
 *   3. Recursively sort left half: merge_sort(arr, left, mid)
 *   4. Recursively sort right half: merge_sort(arr, mid + 1, right)
 *   5. Merge the sorted halves
 */
void merge_sort(int arr[], int left, int right) {
    /* YOUR CODE HERE */
    
}

/* =============================================================================
 * QUICKSORT IMPLEMENTATION
 * =============================================================================
 */

/**
 * TODO 4: Implement partition with last element as pivot
 *
 * Lomuto partition scheme: pivot is arr[high]
 * Use compare_counted() for comparisons and swap_counted() for swaps.
 *
 * @param arr Array to partition
 * @param low Start index
 * @param high End index (pivot position)
 * @return Final position of pivot
 *
 * Hint:
 *   1. Set pivot = arr[high]
 *   2. Initialize i = low - 1
 *   3. For j from low to high-1:
 *        If compare_counted(arr[j], pivot) <= 0:
 *          i++; swap_counted(&arr[i], &arr[j])
 *   4. swap_counted(&arr[i+1], &arr[high])
 *   5. Return i + 1
 */
int partition_last(int arr[], int low, int high) {
    /* YOUR CODE HERE */
    
    return low;  /* Replace with correct return */
}

/**
 * TODO 5: Implement median-of-three pivot selection
 *
 * Choose median of first, middle, and last elements as pivot.
 * Move the median to position high-1, then partition.
 *
 * @param arr Array to partition
 * @param low Start index
 * @param high End index
 * @return Final position of pivot
 *
 * Hint:
 *   1. Find mid = low + (high - low) / 2
 *   2. Sort arr[low], arr[mid], arr[high] using comparisons and swaps
 *   3. Move arr[mid] to arr[high] (so it becomes the pivot)
 *   4. Call partition_last(arr, low, high)
 */
int partition_median3(int arr[], int low, int high) {
    /* YOUR CODE HERE */
    
    return partition_last(arr, low, high);
}

/**
 * TODO 6: Implement random pivot selection
 *
 * Choose a random element as pivot.
 *
 * @param arr Array to partition
 * @param low Start index
 * @param high End index
 * @return Final position of pivot
 *
 * Hint:
 *   1. Generate random index: rand_idx = low + rand() % (high - low + 1)
 *   2. swap_counted(&arr[rand_idx], &arr[high])
 *   3. Call partition_last(arr, low, high)
 */
int partition_random(int arr[], int low, int high) {
    /* YOUR CODE HERE */
    
    return partition_last(arr, low, high);
}

/**
 * QuickSort with selectable partition strategy
 *
 * @param arr Array to sort
 * @param low Start index
 * @param high End index
 * @param strategy 0 = last element, 1 = median-of-3, 2 = random
 */
void quick_sort_impl(int arr[], int low, int high, int strategy) {
    if (low < high) {
        int pi;
        switch (strategy) {
            case 1:  pi = partition_median3(arr, low, high); break;
            case 2:  pi = partition_random(arr, low, high); break;
            default: pi = partition_last(arr, low, high); break;
        }
        quick_sort_impl(arr, low, pi - 1, strategy);
        quick_sort_impl(arr, pi + 1, high, strategy);
    }
}

void quick_sort(int arr[], int n, int strategy) {
    quick_sort_impl(arr, 0, n - 1, strategy);
}

/* =============================================================================
 * SHELLSORT IMPLEMENTATION
 * =============================================================================
 */

/**
 * TODO 7: Implement ShellSort with Hibbard gaps
 *
 * Hibbard gap sequence: 2^k - 1 (1, 3, 7, 15, 31, ...)
 *
 * @param arr Array to sort
 * @param n Size of array
 *
 * Hint:
 *   1. Generate Hibbard gaps: start with largest gap < n
 *      Find k such that 2^k - 1 < n, store gaps[k] = 2^k - 1
 *   2. For each gap from largest to smallest:
 *      - Do insertion sort for elements gap apart
 *      - Use compare_counted() and swap_counted()
 *
 * Gap generation example:
 *   For n = 100: gaps are 63, 31, 15, 7, 3, 1
 */
void shell_sort(int arr[], int n) {
    /* YOUR CODE HERE */
    
    /* Step 1: Generate Hibbard gaps (2^k - 1) */
    
    /* Step 2: For each gap, do gapped insertion sort */
    
}

/* =============================================================================
 * TESTING AND COMPARISON
 * =============================================================================
 */

/**
 * Run a sorting algorithm and record statistics
 */
SortStats run_sort(void (*sort_func)(int[], int, int), int arr[], int n, int param) {
    int *copy = (int *)malloc(n * sizeof(int));
    copy_array(copy, arr, n);
    
    reset_stats();
    clock_t start = clock();
    
    sort_func(copy, n, param);
    
    clock_t end = clock();
    SortStats stats = get_stats();
    stats.time_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
    
    /* Verify sort */
    int sorted = 1;
    for (int i = 1; i < n && sorted; i++) {
        if (copy[i] < copy[i-1]) sorted = 0;
    }
    
    if (!sorted) {
        printf("  WARNING: Array not sorted correctly!\n");
    }
    
    printf("  Sorted: ");
    if (n <= 20) {
        print_array(copy, n);
    } else {
        printf("[%d elements]", n);
    }
    printf("\n");
    
    free(copy);
    return stats;
}

/* Wrapper for merge_sort */
void merge_sort_wrapper(int arr[], int n, int param) {
    (void)param;
    merge_sort(arr, 0, n - 1);
}

/* Wrapper for shell_sort */
void shell_sort_wrapper(int arr[], int n, int param) {
    (void)param;
    shell_sort(arr, n);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    int n;
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 1: Sorting Algorithm Comparison                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Read input */
    printf("Enter array size: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        fprintf(stderr, "Invalid input\n");
        return 1;
    }
    
    int *arr = (int *)malloc(n * sizeof(int));
    if (!arr) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    
    printf("Enter %d integers: ", n);
    for (int i = 0; i < n; i++) {
        if (scanf("%d", &arr[i]) != 1) {
            fprintf(stderr, "Invalid input\n");
            free(arr);
            return 1;
        }
    }
    
    printf("\nOriginal: ");
    print_array(arr, n);
    printf("\n\n");
    
    /* Seed random number generator */
    srand((unsigned int)time(NULL));
    
    SortStats stats;
    
    /* MergeSort */
    printf("─────────────────────────────────────────────────────────────\n");
    printf("MergeSort:\n");
    stats = run_sort(merge_sort_wrapper, arr, n, 0);
    printf("  Comparisons: %ld, Swaps: %ld, Time: %.3f ms\n\n", 
           stats.comparisons, stats.swaps, stats.time_ms);
    
    /* QuickSort - last element pivot */
    printf("─────────────────────────────────────────────────────────────\n");
    printf("QuickSort (last element pivot):\n");
    stats = run_sort(quick_sort, arr, n, 0);
    printf("  Comparisons: %ld, Swaps: %ld, Time: %.3f ms\n\n",
           stats.comparisons, stats.swaps, stats.time_ms);
    
    /* QuickSort - median-of-three pivot */
    printf("─────────────────────────────────────────────────────────────\n");
    printf("QuickSort (median-of-three pivot):\n");
    stats = run_sort(quick_sort, arr, n, 1);
    printf("  Comparisons: %ld, Swaps: %ld, Time: %.3f ms\n\n",
           stats.comparisons, stats.swaps, stats.time_ms);
    
    /* QuickSort - random pivot */
    printf("─────────────────────────────────────────────────────────────\n");
    printf("QuickSort (random pivot):\n");
    stats = run_sort(quick_sort, arr, n, 2);
    printf("  Comparisons: %ld, Swaps: %ld, Time: %.3f ms\n\n",
           stats.comparisons, stats.swaps, stats.time_ms);
    
    /* ShellSort */
    printf("─────────────────────────────────────────────────────────────\n");
    printf("ShellSort (Hibbard gaps):\n");
    stats = run_sort(shell_sort_wrapper, arr, n, 0);
    printf("  Comparisons: %ld, Swaps: %ld, Time: %.3f ms\n\n",
           stats.comparisons, stats.swaps, stats.time_ms);
    
    printf("═════════════════════════════════════════════════════════════\n");
    
    free(arr);
    return 0;
}

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Implement HeapSort and add it to the comparison
 *
 * 2. Add Counting Sort for integer arrays with known range
 *
 * 3. Test with different input distributions:
 *    - Random
 *    - Already sorted
 *    - Reverse sorted
 *    - Nearly sorted (few elements out of place)
 *    - All equal elements
 *
 * 4. Implement adaptive sorting that detects sorted subarrays
 *
 * 5. Add visualization of recursion depth for divide-and-conquer algorithms
 *
 * =============================================================================
 */
