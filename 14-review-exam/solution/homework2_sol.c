/**
 * =============================================================================
 * HOMEWORK 2: Performance Analysis Framework - SOLUTION
 * =============================================================================
 *
 * A rigorous benchmarking framework that empirically analyses algorithm
 * complexity through statistical measurements and complexity estimation.
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c -lm
 * USAGE:
 *   ./homework2_sol --benchmark sorting --sizes 100,500,1000,5000 --runs 5
 *   ./homework2_sol --analyse quicksort --csv results.csv
 *   ./homework2_sol --full-analysis --output report.txt
 *
 * =============================================================================
 */

/* Feature test macros for POSIX functions */
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

/* =============================================================================
 * CONFIGURATION
 * =============================================================================
 */

#define MAX_SIZES           20
#define MAX_RUNS            100
#define MAX_NAME_LENGTH     32
#define DEFAULT_RUNS        5
#define CHART_WIDTH         40
#define MAX_ALGORITHMS      10

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * Timing result with statistical measures.
 */
typedef struct {
    double mean_ns;           /* Mean execution time in nanoseconds */
    double median_ns;         /* Median execution time */
    double std_dev;           /* Standard deviation */
    double min_ns;            /* Minimum time */
    double max_ns;            /* Maximum time */
    int    runs;              /* Number of runs performed */
    int    outliers_removed;  /* Number of outliers eliminated */
} TimingResult;

/**
 * Complexity class enumeration.
 */
typedef enum {
    COMPLEXITY_CONSTANT,      /* O(1)        */
    COMPLEXITY_LOGARITHMIC,   /* O(log n)    */
    COMPLEXITY_LINEAR,        /* O(n)        */
    COMPLEXITY_LINEARITHMIC,  /* O(n log n)  */
    COMPLEXITY_QUADRATIC,     /* O(n²)       */
    COMPLEXITY_CUBIC,         /* O(n³)       */
    COMPLEXITY_EXPONENTIAL,   /* O(2^n)      */
    COMPLEXITY_UNKNOWN
} ComplexityClass;

/**
 * Benchmark configuration.
 */
typedef struct {
    char             name[MAX_NAME_LENGTH];
    void           (*sort_func)(int *, int);
    size_t          *test_sizes;
    int              num_sizes;
    int              runs_per_size;
    ComplexityClass  expected;
} BenchmarkConfig;

/* =============================================================================
 * DATA GENERATION
 * =============================================================================
 */

/**
 * Generate array with random values.
 */
int *generate_random(size_t n, unsigned int seed) {
    int *arr = malloc(n * sizeof(int));
    if (!arr) return NULL;
    
    srand(seed);
    for (size_t i = 0; i < n; i++) {
        arr[i] = rand() % (int)(n * 10);
    }
    
    return arr;
}

/**
 * Generate sorted array (ascending or descending).
 */
int *generate_sorted(size_t n, int ascending) {
    int *arr = malloc(n * sizeof(int));
    if (!arr) return NULL;
    
    for (size_t i = 0; i < n; i++) {
        arr[i] = ascending ? (int)i : (int)(n - 1 - i);
    }
    
    return arr;
}

/**
 * Generate partially sorted array.
 * @param sorted_percent Percentage of array that is sorted (0-100)
 */
int *generate_partial(size_t n, int sorted_percent, unsigned int seed) {
    int *arr = malloc(n * sizeof(int));
    if (!arr) return NULL;
    
    /* First, create sorted array */
    for (size_t i = 0; i < n; i++) {
        arr[i] = (int)i;
    }
    
    /* Then shuffle (100 - sorted_percent)% of elements */
    srand(seed);
    size_t num_swaps = (n * (100 - sorted_percent)) / 100;
    for (size_t s = 0; s < num_swaps; s++) {
        size_t i = rand() % n;
        size_t j = rand() % n;
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
    
    return arr;
}

/**
 * Copy array.
 */
static void copy_array(const int *src, int *dest, size_t n) {
    memcpy(dest, src, n * sizeof(int));
}

/* =============================================================================
 * SORTING ALGORITHMS
 * =============================================================================
 */

/* Selection Sort - O(n²) */
void selection_sort(int *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) min_idx = j;
        }
        if (min_idx != i) {
            int temp = arr[i];
            arr[i] = arr[min_idx];
            arr[min_idx] = temp;
        }
    }
}

/* Quick Sort - O(n log n) average */
static int qs_partition(int *arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            int temp = arr[i]; arr[i] = arr[j]; arr[j] = temp;
        }
    }
    int temp = arr[i + 1]; arr[i + 1] = arr[high]; arr[high] = temp;
    return i + 1;
}

static void qs_recursive(int *arr, int low, int high) {
    if (low < high) {
        int pi = qs_partition(arr, low, high);
        qs_recursive(arr, low, pi - 1);
        qs_recursive(arr, pi + 1, high);
    }
}

void quick_sort(int *arr, int n) {
    if (n > 1) qs_recursive(arr, 0, n - 1);
}

/* Merge Sort - O(n log n) */
static void ms_merge(int *arr, int left, int mid, int right) {
    int n1 = mid - left + 1, n2 = right - mid;
    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));
    
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];
    
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    
    free(L); free(R);
}

static void ms_recursive(int *arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        ms_recursive(arr, left, mid);
        ms_recursive(arr, mid + 1, right);
        ms_merge(arr, left, mid, right);
    }
}

void merge_sort(int *arr, int n) {
    if (n > 1) ms_recursive(arr, 0, n - 1);
}

/* Heap Sort - O(n log n) */
static void heapify(int *arr, int n, int i) {
    int largest = i, left = 2 * i + 1, right = 2 * i + 2;
    if (left < n && arr[left] > arr[largest]) largest = left;
    if (right < n && arr[right] > arr[largest]) largest = right;
    if (largest != i) {
        int temp = arr[i]; arr[i] = arr[largest]; arr[largest] = temp;
        heapify(arr, n, largest);
    }
}

void heap_sort(int *arr, int n) {
    for (int i = n / 2 - 1; i >= 0; i--) heapify(arr, n, i);
    for (int i = n - 1; i > 0; i--) {
        int temp = arr[0]; arr[0] = arr[i]; arr[i] = temp;
        heapify(arr, i, 0);
    }
}

/* Linear Search - O(n) */
int linear_search(const int *arr, int n, int key) {
    for (int i = 0; i < n; i++) {
        if (arr[i] == key) return i;
    }
    return -1;
}

/* Binary Search - O(log n) */
int binary_search(const int *arr, int n, int key) {
    int left = 0, right = n - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] == key) return mid;
        if (arr[mid] < key) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

/* =============================================================================
 * TIMING INFRASTRUCTURE
 * =============================================================================
 */

/**
 * Get high-resolution time in nanoseconds.
 * Uses clock_gettime for precision on Linux.
 */
static double get_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e9 + ts.tv_nsec;
}

/**
 * Compare function for qsort (doubles).
 */
static int compare_double(const void *a, const void *b) {
    double diff = *(double *)a - *(double *)b;
    return (diff > 0) - (diff < 0);
}

/**
 * Benchmark a sorting function with statistical analysis.
 */
TimingResult benchmark_sort(void (*sort_func)(int *, int),
                            const int *original, size_t n, int runs) {
    TimingResult result = {0};
    
    double *times = malloc(runs * sizeof(double));
    int *copy = malloc(n * sizeof(int));
    
    if (!times || !copy) {
        free(times);
        free(copy);
        return result;
    }
    
    /* Collect timing data */
    for (int r = 0; r < runs; r++) {
        copy_array(original, copy, n);
        
        double start = get_time_ns();
        sort_func(copy, (int)n);
        double end = get_time_ns();
        
        times[r] = end - start;
    }
    
    /* Sort times for median calculation */
    qsort(times, runs, sizeof(double), compare_double);
    
    /* Remove outliers using IQR method */
    int start_idx = 0, end_idx = runs;
    if (runs >= 4) {
        double q1 = times[runs / 4];
        double q3 = times[(3 * runs) / 4];
        double iqr = q3 - q1;
        double lower = q1 - 1.5 * iqr;
        double upper = q3 + 1.5 * iqr;
        
        while (start_idx < runs && times[start_idx] < lower) start_idx++;
        while (end_idx > start_idx && times[end_idx - 1] > upper) end_idx--;
    }
    
    int valid_count = end_idx - start_idx;
    result.outliers_removed = runs - valid_count;
    result.runs = valid_count;
    
    /* Calculate statistics on cleaned data */
    double sum = 0;
    for (int i = start_idx; i < end_idx; i++) {
        sum += times[i];
    }
    result.mean_ns = sum / valid_count;
    
    /* Median */
    int mid = start_idx + valid_count / 2;
    if (valid_count % 2 == 0) {
        result.median_ns = (times[mid - 1] + times[mid]) / 2.0;
    } else {
        result.median_ns = times[mid];
    }
    
    /* Standard deviation */
    double sq_sum = 0;
    for (int i = start_idx; i < end_idx; i++) {
        double diff = times[i] - result.mean_ns;
        sq_sum += diff * diff;
    }
    result.std_dev = sqrt(sq_sum / valid_count);
    
    /* Min and max */
    result.min_ns = times[start_idx];
    result.max_ns = times[end_idx - 1];
    
    free(times);
    free(copy);
    
    return result;
}

/* =============================================================================
 * COMPLEXITY ESTIMATION
 * =============================================================================
 */

/**
 * Get complexity class name.
 */
const char *complexity_name(ComplexityClass c) {
    switch (c) {
        case COMPLEXITY_CONSTANT:     return "O(1)";
        case COMPLEXITY_LOGARITHMIC:  return "O(log n)";
        case COMPLEXITY_LINEAR:       return "O(n)";
        case COMPLEXITY_LINEARITHMIC: return "O(n log n)";
        case COMPLEXITY_QUADRATIC:    return "O(n²)";
        case COMPLEXITY_CUBIC:        return "O(n³)";
        case COMPLEXITY_EXPONENTIAL:  return "O(2^n)";
        default:                      return "Unknown";
    }
}

/**
 * Estimate complexity using ratio test.
 * Compares time ratio when doubling n.
 */
ComplexityClass estimate_complexity(const TimingResult *results,
                                    const size_t *sizes, int count) {
    if (count < 2) return COMPLEXITY_UNKNOWN;
    
    /* Find pairs where size doubles (approximately) */
    double total_ratio = 0;
    int ratio_count = 0;
    
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            double size_ratio = (double)sizes[j] / sizes[i];
            
            /* Look for approximately 2x increase */
            if (size_ratio >= 1.8 && size_ratio <= 2.2) {
                if (results[i].mean_ns > 0) {
                    double time_ratio = results[j].mean_ns / results[i].mean_ns;
                    total_ratio += time_ratio;
                    ratio_count++;
                }
            }
        }
    }
    
    if (ratio_count == 0) return COMPLEXITY_UNKNOWN;
    
    double avg_ratio = total_ratio / ratio_count;
    
    /* Classify based on ratio when doubling n:
     * O(1):       ratio ≈ 1
     * O(log n):   ratio ≈ 1.03 (slight increase)
     * O(n):       ratio ≈ 2
     * O(n log n): ratio ≈ 2.1-2.2
     * O(n²):      ratio ≈ 4
     * O(n³):      ratio ≈ 8
     * O(2^n):     ratio >> 8
     */
    
    if (avg_ratio < 1.2)       return COMPLEXITY_CONSTANT;
    if (avg_ratio < 1.5)       return COMPLEXITY_LOGARITHMIC;
    if (avg_ratio < 2.5)       return COMPLEXITY_LINEAR;
    if (avg_ratio < 3.0)       return COMPLEXITY_LINEARITHMIC;
    if (avg_ratio < 5.0)       return COMPLEXITY_QUADRATIC;
    if (avg_ratio < 10.0)      return COMPLEXITY_CUBIC;
    return COMPLEXITY_EXPONENTIAL;
}

/* =============================================================================
 * OUTPUT AND VISUALISATION
 * =============================================================================
 */

/**
 * Export results to CSV file.
 */
void export_csv(const char *filename, const BenchmarkConfig *cfg,
                const TimingResult *results) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Cannot create CSV file: %s\n", filename);
        return;
    }
    
    /* Header */
    fprintf(fp, "Algorithm,Size,Mean_ns,Median_ns,StdDev,Min_ns,Max_ns,Runs,Outliers\n");
    
    /* Data rows */
    for (int s = 0; s < cfg->num_sizes; s++) {
        fprintf(fp, "%s,%zu,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%d\n",
                cfg->name,
                cfg->test_sizes[s],
                results[s].mean_ns,
                results[s].median_ns,
                results[s].std_dev,
                results[s].min_ns,
                results[s].max_ns,
                results[s].runs,
                results[s].outliers_removed);
    }
    
    fclose(fp);
    printf("Results exported to: %s\n", filename);
}

/**
 * Print ASCII performance chart.
 */
void print_ascii_chart(const TimingResult *results,
                       const size_t *sizes, int count) {
    if (count == 0) return;
    
    /* Find max time for scaling */
    double max_time = 0;
    for (int i = 0; i < count; i++) {
        if (results[i].mean_ns > max_time) {
            max_time = results[i].mean_ns;
        }
    }
    
    printf("\nASCII Performance Chart (linear scale):\n");
    printf("────────────────────────────────────────────────\n");
    
    for (int i = 0; i < count; i++) {
        int bar_len = (int)((results[i].mean_ns / max_time) * CHART_WIDTH);
        if (bar_len < 1 && results[i].mean_ns > 0) bar_len = 1;
        
        printf("n=%-6zu |", sizes[i]);
        for (int b = 0; b < bar_len; b++) printf("█");
        for (int b = bar_len; b < CHART_WIDTH; b++) printf(" ");
        printf("| %8.2f ms\n", results[i].mean_ns / 1e6);
    }
}

/* =============================================================================
 * FULL ANALYSIS
 * =============================================================================
 */

void run_full_analysis(const char *output_file) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║         ALGORITHM COMPLEXITY ANALYSIS REPORT                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Default test sizes */
    size_t test_sizes[] = {100, 500, 1000, 2000, 5000};
    int num_sizes = 5;
    int runs = 5;
    
    /* Algorithm configurations */
    struct {
        const char *name;
        void (*func)(int *, int);
        ComplexityClass expected;
    } sort_algos[] = {
        {"SelectionSort", selection_sort, COMPLEXITY_QUADRATIC},
        {"QuickSort",     quick_sort,     COMPLEXITY_LINEARITHMIC},
        {"MergeSort",     merge_sort,     COMPLEXITY_LINEARITHMIC},
        {"HeapSort",      heap_sort,      COMPLEXITY_LINEARITHMIC},
        {NULL, NULL, COMPLEXITY_UNKNOWN}
    };
    
    printf("Generated: %s\n", __DATE__);
    printf("Test Parameters: sizes=[");
    for (int i = 0; i < num_sizes; i++) {
        printf("%zu%s", test_sizes[i], i < num_sizes - 1 ? "," : "");
    }
    printf("], runs=%d\n\n", runs);
    
    /* File output */
    FILE *fp = NULL;
    if (output_file) {
        fp = fopen(output_file, "w");
        if (fp) {
            fprintf(fp, "Algorithm Complexity Analysis Report\n");
            fprintf(fp, "=====================================\n\n");
        }
    }
    
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    printf("SORTING ALGORITHMS\n");
    printf("──────────────────────────────────────────────────────────────────\n");
    printf("%-16s %-12s %-12s %-8s %s\n", 
           "Algorithm", "Expected", "Measured", "Status", "Ratio (n→2n)");
    printf("──────────────────────────────────────────────────────────────────\n");
    
    int total_passed = 0;
    int total_tested = 0;
    
    for (int a = 0; sort_algos[a].name; a++) {
        TimingResult results[MAX_SIZES];
        
        /* Benchmark this algorithm */
        for (int s = 0; s < num_sizes; s++) {
            int *test_data = generate_random(test_sizes[s], 42);
            if (!test_data) continue;
            
            results[s] = benchmark_sort(sort_algos[a].func, test_data, 
                                        test_sizes[s], runs);
            free(test_data);
        }
        
        /* Estimate complexity */
        ComplexityClass measured = estimate_complexity(results, test_sizes, num_sizes);
        bool passed = (measured == sort_algos[a].expected);
        
        /* Calculate average ratio */
        double avg_ratio = 0;
        int ratio_count = 0;
        for (int i = 0; i < num_sizes - 1; i++) {
            double size_ratio = (double)test_sizes[i + 1] / test_sizes[i];
            if (size_ratio >= 1.8 && size_ratio <= 2.2 && results[i].mean_ns > 0) {
                avg_ratio += results[i + 1].mean_ns / results[i].mean_ns;
                ratio_count++;
            }
        }
        if (ratio_count > 0) avg_ratio /= ratio_count;
        
        printf("%-16s %-12s %-12s %s    %.2f ≈ %d\n",
               sort_algos[a].name,
               complexity_name(sort_algos[a].expected),
               complexity_name(measured),
               passed ? "✓ PASS" : "✗ FAIL",
               avg_ratio,
               (int)round(avg_ratio));
        
        if (passed) total_passed++;
        total_tested++;
        
        /* Write to file */
        if (fp) {
            fprintf(fp, "%s: %s → %s (%s)\n",
                    sort_algos[a].name,
                    complexity_name(sort_algos[a].expected),
                    complexity_name(measured),
                    passed ? "PASS" : "FAIL");
        }
    }
    
    /* Show chart for quicksort */
    printf("\n");
    {
        TimingResult results[MAX_SIZES];
        for (int s = 0; s < num_sizes; s++) {
            int *test_data = generate_random(test_sizes[s], 42);
            if (test_data) {
                results[s] = benchmark_sort(quick_sort, test_data, test_sizes[s], runs);
                free(test_data);
            }
        }
        print_ascii_chart(results, test_sizes, num_sizes);
    }
    
    /* Summary */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    printf("SUMMARY\n");
    printf("──────────────────────────────────────────────────────────────────\n");
    printf("Total algorithms tested: %d\n", total_tested);
    printf("Passed: %d\n", total_passed);
    printf("Failed: %d\n", total_tested - total_passed);
    printf("Coverage: %.0f%%\n", 100.0 * total_passed / total_tested);
    
    if (fp) {
        fprintf(fp, "\nSummary: %d/%d tests passed (%.0f%%)\n",
                total_passed, total_tested, 100.0 * total_passed / total_tested);
        fclose(fp);
        printf("\nResults exported to: %s\n", output_file);
    }
    
    printf("\n═══════════════════════════════════════════════════════════════\n\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

void print_usage(const char *prog) {
    printf("Usage: %s [options]\n\n", prog);
    printf("Options:\n");
    printf("  --full-analysis            Run complete analysis suite\n");
    printf("  --output <file>            Save report to file\n");
    printf("  --benchmark <type>         Benchmark type (sorting/searching)\n");
    printf("  --sizes <n1,n2,...>        Test sizes (comma-separated)\n");
    printf("  --runs <n>                 Runs per test (default: 5)\n");
    printf("  --csv <file>               Export results as CSV\n");
    printf("  --help                     Show this help message\n");
}

int main(int argc, char *argv[]) {
    const char *output_file = NULL;
    bool full_analysis = false;
    
    /* Parse command line */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--full-analysis") == 0) {
            full_analysis = true;
        } else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        } else if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
    }
    
    if (argc < 2 || full_analysis) {
        run_full_analysis(output_file);
        return 0;
    }
    
    print_usage(argv[0]);
    return 1;
}

/* =============================================================================
 * COMPLEXITY REFERENCE
 * =============================================================================
 *
 * Ratio Test Guide (when doubling n):
 *
 * | Complexity    | Expected Ratio | Example Algorithms           |
 * |---------------|----------------|------------------------------|
 * | O(1)          | 1.0            | Array access, hash lookup    |
 * | O(log n)      | 1.0 - 1.1      | Binary search                |
 * | O(n)          | 2.0            | Linear search, single loop   |
 * | O(n log n)    | 2.0 - 2.3      | Merge sort, heap sort        |
 * | O(n²)         | 4.0            | Selection sort, bubble sort  |
 * | O(n³)         | 8.0            | Matrix multiplication        |
 * | O(2^n)        | 2^n            | Naive Fibonacci, TSP         |
 *
 * Statistical Measures:
 * - Mean: Average of all measurements
 * - Median: Middle value (robust to outliers)
 * - Std Dev: Spread of measurements
 * - IQR: Interquartile range for outlier detection
 *
 * Outlier Removal:
 * - Uses IQR method: values outside [Q1 - 1.5×IQR, Q3 + 1.5×IQR] are excluded
 * - Improves measurement accuracy by removing JIT effects and cache warming
 *
 * =============================================================================
 */
