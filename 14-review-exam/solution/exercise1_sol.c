/**
 * =============================================================================
 * EXERCISE 1: Algorithm Benchmarking Suite
 * =============================================================================
 *
 * This exercise integrates two complementary concerns:
 *
 * 1) Correctness under a strict, transcript-based regression harness
 *    - When stdin is not a TTY the programme reads: n followed by n integers
 *    - It sorts the same input with selection sort, quick sort and merge sort
 *    - It prints the sorted sequence once then prints PASS/FAIL per algorithm
 *
 * 2) Empirical benchmarking and reproducible performance observation
 *    - When stdin is a TTY the programme generates random inputs for a series
 *      of sizes and measures mean execution time over multiple runs
 *    - Results are exported in CSV form for external plotting
 *
 * Compilation:
 *   gcc -Wall -Wextra -std=c11 -g -o exercise1 src/exercise1.c
 *
 * Regression usage:
 *   ./exercise1 < tests/test1_input.txt
 *
 * Benchmark usage:
 *   ./exercise1 --benchmark
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

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
 * SOLUTION TODO 1: Function pointer type for sorting algorithms
 *
 * A function pointer type that can reference any sorting function
 * with signature: void function_name(int *array, int size)
 */
typedef void (*SortFunction)(int *, int);


/**
 * Structure to hold benchmark results for one algorithm.
 */
typedef struct {
    char name[32];                  /* Algorithm name */
    double times[NUM_TEST_SIZES];   /* Execution times in ms */
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
 * SOLUTION TODO 2: Selection Sort
 *
 * Time Complexity: O(n²) for all cases
 * Space Complexity: O(1) - in-place
 *
 * Selection sort works by repeatedly finding the minimum element from the
 * unsorted portion and placing it at the beginning of the unsorted portion.
 */
void selection_sort(int *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        /* Find index of minimum element in unsorted portion [i, n-1] */
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        
        /* Swap minimum element with first unsorted element */
        if (min_idx != i) {
            int temp = arr[i];
            arr[i] = arr[min_idx];
            arr[min_idx] = temp;
        }
    }
}

/**
 * SOLUTION TODO 3: Quick Sort partition function (Lomuto scheme)
 *
 * Partitions the array around the pivot (last element).
 * After partitioning:
 *   - All elements < pivot are to the left of pivot's final position
 *   - All elements >= pivot are to the right
 *
 * @return Final position of the pivot element
 */
static int partition(int *arr, int low, int high) {
    /* Choose last element as pivot */
    int pivot = arr[high];
    
    /* Index of smaller element - tracks boundary of left partition */
    int i = low - 1;
    
    /* Scan through array, moving elements smaller than pivot to left */
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            /* Swap arr[i] and arr[j] */
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    
    /* Place pivot in its correct position */
    int temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    
    return i + 1;  /* Return pivot's final position */
}

/**
 * SOLUTION TODO 4: Quick Sort recursive implementation
 *
 * Time Complexity: O(n log n) average, O(n²) worst case
 * Space Complexity: O(log n) average (call stack)
 *
 * Quick sort is a divide-and-conquer algorithm that:
 * 1. Selects a 'pivot' element
 * 2. Partitions array around pivot
 * 3. Recursively sorts sub-arrays
 */
static void quicksort_recursive(int *arr, int low, int high) {
    if (low < high) {
        /* Partition array and get pivot position */
        int pi = partition(arr, low, high);
        
        /* Recursively sort elements before and after partition */
        quicksort_recursive(arr, low, pi - 1);   /* Left of pivot */
        quicksort_recursive(arr, pi + 1, high);  /* Right of pivot */
    }
}

/**
 * Quick Sort wrapper function.
 */
void quick_sort(int *arr, int n) {
    if (n > 1) {
        quicksort_recursive(arr, 0, n - 1);
    }
}

/**
 * SOLUTION TODO 5: Merge function for Merge Sort
 *
 * Merges two sorted sub-arrays into one sorted array.
 * Uses temporary arrays to hold the two halves during merging.
 *
 * @param arr   The array containing both sub-arrays
 * @param left  Starting index of first sub-array
 * @param mid   Ending index of first sub-array
 * @param right Ending index of second sub-array
 */
static void merge(int *arr, int left, int mid, int right) {
    /* Calculate sizes of two sub-arrays */
    int n1 = mid - left + 1;    /* Size of left sub-array */
    int n2 = right - mid;       /* Size of right sub-array */
    
    /* Allocate temporary arrays */
    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));
    
    if (L == NULL || R == NULL) {
        fprintf(stderr, "Memory allocation failed in merge()\n");
        free(L);
        free(R);
        return;
    }
    
    /* Copy data to temporary arrays */
    for (int i = 0; i < n1; i++) {
        L[i] = arr[left + i];
    }
    for (int j = 0; j < n2; j++) {
        R[j] = arr[mid + 1 + j];
    }
    
    /* Merge temporary arrays back into arr[left..right] */
    int i = 0;      /* Index for left sub-array */
    int j = 0;      /* Index for right sub-array */
    int k = left;   /* Index for merged array */
    
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    
    /* Copy remaining elements of L[], if any */
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    
    /* Copy remaining elements of R[], if any */
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
    
    /* Free temporary arrays */
    free(L);
    free(R);
}

/**
 * SOLUTION TODO 6: Merge Sort recursive implementation
 *
 * Time Complexity: O(n log n) for all cases
 * Space Complexity: O(n) for temporary arrays
 *
 * Merge sort is a stable, divide-and-conquer algorithm that:
 * 1. Divides array into two halves
 * 2. Recursively sorts each half
 * 3. Merges the sorted halves
 */
static void mergesort_recursive(int *arr, int left, int right) {
    if (left < right) {
        /* Find middle point to divide array */
        /* Use left + (right - left) / 2 to avoid overflow */
        int mid = left + (right - left) / 2;
        
        /* Recursively sort first and second halves */
        mergesort_recursive(arr, left, mid);
        mergesort_recursive(arr, mid + 1, right);
        
        /* Merge the sorted halves */
        merge(arr, left, mid, right);
    }
}

/**
 * Merge Sort wrapper function.
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
 * SOLUTION TODO 7: Generate array with random values
 *
 * Allocates and fills an array with random integers.
 *
 * @param n       Number of elements to generate
 * @param max_val Maximum value (exclusive)
 * @return        Pointer to allocated array, or NULL on failure
 */
int *generate_random_array(int n, int max_val) {
    /* Allocate memory for n integers */
    int *arr = malloc(n * sizeof(int));
    
    /* Check for allocation failure */
    if (arr == NULL) {
        fprintf(stderr, "Memory allocation failed for array of size %d\n", n);
        return NULL;
    }
    
    /* Fill with random values in range [0, max_val) */
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % max_val;
    }
    
    return arr;
}

/**
 * SOLUTION TODO 8: Measure execution time of sorting algorithm
 *
 * Runs the algorithm multiple times and returns average execution time.
 * Uses a fresh copy of the original array for each run.
 *
 * @param sort_func Function pointer to sorting algorithm
 * @param original  Original array (copied for each run)
 * @param n         Array size
 * @return          Average execution time in milliseconds
 */
double measure_sort_time(SortFunction sort_func, const int *original, int n) {
    /* Allocate copy array for testing */
    int *copy = malloc(n * sizeof(int));
    if (copy == NULL) {
        fprintf(stderr, "Memory allocation failed in measure_sort_time()\n");
        return -1.0;
    }
    
    double total_time = 0.0;
    
    /* Run algorithm BENCHMARK_RUNS times */
    for (int run = 0; run < BENCHMARK_RUNS; run++) {
        /* Copy original array to preserve it */
        copy_array(original, copy, n);
        
        /* Record start time */
        clock_t start = clock();
        
        /* Execute sorting algorithm */
        sort_func(copy, n);
        
        /* Record end time */
        clock_t end = clock();
        
        /* Calculate elapsed time in milliseconds */
        double elapsed = ((double)(end - start) * 1000.0) / CLOCKS_PER_SEC;
        total_time += elapsed;
    }
    
    /* Free copy array */
    free(copy);
    
    /* Return average time */
    return total_time / BENCHMARK_RUNS;
}

/* =============================================================================
 * FILE OUTPUT
 * =============================================================================
 */

/**
 * SOLUTION TODO 9: Export benchmark results to CSV file
 *
 * Creates a CSV file suitable for importing into spreadsheet software
 * or plotting with tools like gnuplot, matplotlib, etc.
 *
 * @param results  Array of BenchmarkResult structures
 * @param count    Number of algorithms
 * @param filename Output filename
 * @return         0 on success, -1 on failure
 */
int export_results_csv(const BenchmarkResult *results, int count, 
                       const char *filename) {
    /* Open file for writing */
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open file '%s' for writing\n", filename);
        return -1;
    }
    
    /* Write header row */
    fprintf(fp, "Size");
    for (int a = 0; a < count; a++) {
        fprintf(fp, ",%s", results[a].name);
    }
    fprintf(fp, "\n");
    
    /* Write data rows */
    for (int s = 0; s < NUM_TEST_SIZES; s++) {
        fprintf(fp, "%d", TEST_SIZES[s]);
        for (int a = 0; a < count; a++) {
            fprintf(fp, ",%.4f", results[a].times[s]);
        }
        fprintf(fp, "\n");
    }
    
    /* Close file */
    fclose(fp);
    
    printf("Results exported to '%s'\n", filename);
    return 0;
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */


static void print_int_list(const int *arr, int n) {
    for (int i = 0; i < n; i++) {
        if (i) putchar(' ');
        printf("%d", arr[i]);
    }
    putchar('\n');
}

static int run_regression_mode(void) {
    int n = 0;
    if (scanf("%d", &n) != 1 || n < 0 || n > MAX_ARRAY_SIZE) {
        return 1;
    }

    int *input = NULL;
    if (n > 0) {
        input = (int *)malloc((size_t)n * sizeof(int));
        if (!input) {
            return 1;
        }
        for (int i = 0; i < n; i++) {
            if (scanf("%d", &input[i]) != 1) {
                free(input);
                return 1;
            }
        }
    }

    SortFunction algorithms[NUM_ALGORITHMS] = { selection_sort, quick_sort, merge_sort };
    const char *algo_labels[NUM_ALGORITHMS] = { "SelectionSort", "QuickSort", "MergeSort" };

    int *work = (n > 0) ? (int *)malloc((size_t)n * sizeof(int)) : NULL;
    int *reference = (n > 0) ? (int *)malloc((size_t)n * sizeof(int)) : NULL;
    if (n > 0 && (!work || !reference)) {
        free(input);
        free(work);
        free(reference);
        return 1;
    }

    bool passed[NUM_ALGORITHMS] = { true, true, true };

    for (int a = 0; a < NUM_ALGORITHMS; a++) {
        if (n > 0) {
            copy_array(input, work, n);
            algorithms[a](work, n);
        }

        if (!is_sorted(work, n)) {
            passed[a] = false;
        }

        if (a == 0) {
            if (n > 0) {
                copy_array(work, reference, n);
            }
        } else {
            if (n > 0 && memcmp(reference, work, (size_t)n * sizeof(int)) != 0) {
                passed[a] = false;
            }
        }
    }

    if (n > 0) {
        print_int_list(reference, n);
    } else {
        putchar('\n');
    }

    for (int a = 0; a < NUM_ALGORITHMS; a++) {
        printf("%s: %s\n", algo_labels[a], passed[a] ? "PASSED" : "FAILED");
    }

    free(input);
    free(work);
    free(reference);
    return 0;
}

static int run_benchmark_mode(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  Algorithm Benchmarking Suite                                 ║\n");
    printf("║  Exercise 1 - Week 14                                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    /* Fixed seed ensures reproducible benchmark tables across runs. */
    srand(42);

    SortFunction algorithms[NUM_ALGORITHMS] = { selection_sort, quick_sort, merge_sort };
    const char *algo_names[NUM_ALGORITHMS] = { "SelectSort", "QuickSort", "MergeSort" };

    BenchmarkResult results[NUM_ALGORITHMS];
    for (int i = 0; i < NUM_ALGORITHMS; i++) {
        strncpy(results[i].name, algo_names[i], 31);
        results[i].name[31] = '\0';
    }

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

    for (int s = 0; s < NUM_TEST_SIZES; s++) {
        int size = TEST_SIZES[s];
        int *test_array = generate_random_array(size, 10000);
        if (!test_array) {
            fprintf(stderr, "Failed to generate test array\n");
            return 1;
        }

        printf("%5d ", size);
        for (int a = 0; a < NUM_ALGORITHMS; a++) {
            double time_ms = measure_sort_time(algorithms[a], test_array, size);
            results[a].times[s] = time_ms;
            printf(" | %9.3f ms", time_ms);
        }
        printf("\n");

        free(test_array);
    }

    printf("\n");

    if (export_results_csv(results, NUM_ALGORITHMS, OUTPUT_FILENAME) == 0) {
        printf("Results exported to %s\n", OUTPUT_FILENAME);
    } else {
        printf("Failed to export results to %s\n", OUTPUT_FILENAME);
    }

    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  Verification                                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    int verify_size = 20;
    int *verify_array = generate_random_array(verify_size, 100);
    int *test_copy = (int *)malloc((size_t)verify_size * sizeof(int));

    if (verify_array && test_copy) {
        printf("Original array: ");
        print_array(verify_array, verify_size);
        printf("\n");

        for (int a = 0; a < NUM_ALGORITHMS; a++) {
            copy_array(verify_array, test_copy, verify_size);
            algorithms[a](test_copy, verify_size);

            bool sorted = is_sorted(test_copy, verify_size);
            printf("%-12s: ", algo_names[a]);
            printf("%s\n", sorted ? "PASSED" : "FAILED");
        }

        free(verify_array);
        free(test_copy);
    } else {
        free(verify_array);
        free(test_copy);
    }

    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  Benchmarking complete                                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    return 0;
}

int main(int argc, char **argv) {
    bool benchmark = false;
    bool force_stdin = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--benchmark") == 0 || strcmp(argv[i], "-b") == 0) {
            benchmark = true;
        } else if (strcmp(argv[i], "--stdin") == 0 || strcmp(argv[i], "-s") == 0) {
            force_stdin = true;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            fprintf(stderr,
                    "Usage: %s [--benchmark|-b] [--stdin|-s]\n\n"
                    "Default behaviour:\n"
                    "  - If stdin is not a TTY then run regression mode (read array from stdin)\n"
                    "  - Otherwise run benchmarking mode\n",
                    argv[0]);
            return 0;
        }
    }

    if (benchmark) {
        return run_benchmark_mode();
    }

    if (force_stdin || !isatty(STDIN_FILENO)) {
        return run_regression_mode();
    }

    return run_benchmark_mode();
}


/* =============================================================================
 * COMPLEXITY ANALYSIS
 * =============================================================================
 *
 * Algorithm       | Best       | Average    | Worst      | Space
 * ----------------|------------|------------|------------|--------
 * Selection Sort  | O(n²)      | O(n²)      | O(n²)      | O(1)
 * Quick Sort      | O(n log n) | O(n log n) | O(n²)      | O(log n)
 * Merge Sort      | O(n log n) | O(n log n) | O(n log n) | O(n)
 *
 * Expected benchmark ratios when doubling n:
 * - Selection Sort: ~4× (quadratic)
 * - Quick Sort: ~2× (linearithmic)
 * - Merge Sort: ~2× (linearithmic)
 *
 * =============================================================================
 */
