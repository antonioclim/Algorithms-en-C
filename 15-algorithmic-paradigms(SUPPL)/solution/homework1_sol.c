/**
 * =============================================================================
 * WEEK 15: ALGORITHMIC PARADIGMS
 * Homework 1 Solution - Comprehensive Sorting Library
 * =============================================================================
 *
 * This solution implements:
 *   1. QuickSort with 3 pivot strategies
 *   2. ShellSort with 3 gap sequences
 *   3. Counting Sort (stable)
 *   4. Radix Sort (LSD and MSD)
 *   5. Bucket Sort for floating-point numbers
 *   6. Benchmarking framework
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c -lm
 * Usage: ./homework1_sol benchmark <n>
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
 * STATISTICS STRUCTURE
 * =============================================================================
 */

typedef struct {
    long long comparisons;
    long long swaps;
    double time_ms;
} SortStats;

/* Global statistics */
static SortStats g_stats;

static void reset_stats(void) {
    g_stats.comparisons = 0;
    g_stats.swaps = 0;
    g_stats.time_ms = 0.0;
}

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

static void swap_int(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
    g_stats.swaps++;
}

static int compare_int(int a, int b) {
    g_stats.comparisons++;
    return (a > b) - (a < b);
}

static int *copy_array(const int *arr, int n) {
    int *copy = malloc(n * sizeof(int));
    if (copy) {
        memcpy(copy, arr, n * sizeof(int));
    }
    return copy;
}

/* =============================================================================
 * SECTION 1: QUICKSORT WITH MULTIPLE PIVOT STRATEGIES
 * =============================================================================
 */

typedef enum {
    PIVOT_FIRST,
    PIVOT_MEDIAN3,
    PIVOT_RANDOM
} PivotStrategy;

static int choose_pivot_first(int *arr, int low, int high) {
    (void)arr; (void)high;
    return low;
}

static int choose_pivot_median3(int *arr, int low, int high) {
    int mid = low + (high - low) / 2;
    
    /* Sort low, mid, high */
    if (compare_int(arr[low], arr[mid]) > 0) {
        swap_int(&arr[low], &arr[mid]);
    }
    if (compare_int(arr[mid], arr[high]) > 0) {
        swap_int(&arr[mid], &arr[high]);
    }
    if (compare_int(arr[low], arr[mid]) > 0) {
        swap_int(&arr[low], &arr[mid]);
    }
    
    return mid;
}

static int choose_pivot_random(int *arr, int low, int high) {
    (void)arr;
    return low + rand() % (high - low + 1);
}

static int partition_lomuto(int *arr, int low, int high, PivotStrategy strategy) {
    int pivot_idx;
    
    switch (strategy) {
        case PIVOT_FIRST:
            pivot_idx = choose_pivot_first(arr, low, high);
            break;
        case PIVOT_MEDIAN3:
            pivot_idx = choose_pivot_median3(arr, low, high);
            break;
        case PIVOT_RANDOM:
            pivot_idx = choose_pivot_random(arr, low, high);
            break;
        default:
            pivot_idx = low;
    }
    
    /* Move pivot to end */
    swap_int(&arr[pivot_idx], &arr[high]);
    int pivot = arr[high];
    
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (compare_int(arr[j], pivot) <= 0) {
            i++;
            swap_int(&arr[i], &arr[j]);
        }
    }
    swap_int(&arr[i + 1], &arr[high]);
    
    return i + 1;
}

static void quicksort_impl(int *arr, int low, int high, PivotStrategy strategy) {
    if (low < high) {
        int pi = partition_lomuto(arr, low, high, strategy);
        quicksort_impl(arr, low, pi - 1, strategy);
        quicksort_impl(arr, pi + 1, high, strategy);
    }
}

void quicksort(int *arr, int n, PivotStrategy strategy) {
    reset_stats();
    clock_t start = clock();
    
    quicksort_impl(arr, 0, n - 1, strategy);
    
    clock_t end = clock();
    g_stats.time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
}

/* =============================================================================
 * SECTION 2: SHELLSORT WITH MULTIPLE GAP SEQUENCES
 * =============================================================================
 */

typedef enum {
    GAP_SHELL,      /* N/2, N/4, ..., 1 */
    GAP_HIBBARD,    /* 2^k - 1: 1, 3, 7, 15, 31, ... */
    GAP_KNUTH       /* (3^k - 1) / 2: 1, 4, 13, 40, ... */
} GapSequence;

static int *generate_shell_gaps(int n, int *num_gaps) {
    int *gaps = malloc((int)(log2(n) + 1) * sizeof(int));
    if (!gaps) return NULL;
    
    int count = 0;
    for (int gap = n / 2; gap > 0; gap /= 2) {
        gaps[count++] = gap;
    }
    *num_gaps = count;
    return gaps;
}

static int *generate_hibbard_gaps(int n, int *num_gaps) {
    int max_k = (int)log2(n) + 1;
    int *gaps = malloc(max_k * sizeof(int));
    if (!gaps) return NULL;
    
    int count = 0;
    for (int k = max_k; k >= 1; k--) {
        int gap = (1 << k) - 1;  /* 2^k - 1 */
        if (gap < n) {
            gaps[count++] = gap;
        }
    }
    *num_gaps = count;
    return gaps;
}

static int *generate_knuth_gaps(int n, int *num_gaps) {
    int *gaps = malloc(20 * sizeof(int));  /* More than enough */
    if (!gaps) return NULL;
    
    /* Generate in ascending order first */
    int temp[20];
    int count = 0;
    int gap = 1;
    while (gap < n) {
        temp[count++] = gap;
        gap = 3 * gap + 1;
    }
    
    /* Reverse to descending */
    for (int i = 0; i < count; i++) {
        gaps[i] = temp[count - 1 - i];
    }
    
    *num_gaps = count;
    return gaps;
}

void shellsort(int *arr, int n, GapSequence seq) {
    reset_stats();
    clock_t start = clock();
    
    int num_gaps;
    int *gaps;
    
    switch (seq) {
        case GAP_SHELL:
            gaps = generate_shell_gaps(n, &num_gaps);
            break;
        case GAP_HIBBARD:
            gaps = generate_hibbard_gaps(n, &num_gaps);
            break;
        case GAP_KNUTH:
            gaps = generate_knuth_gaps(n, &num_gaps);
            break;
        default:
            gaps = generate_shell_gaps(n, &num_gaps);
    }
    
    if (!gaps) return;
    
    for (int g = 0; g < num_gaps; g++) {
        int gap = gaps[g];
        
        /* Insertion sort with gap */
        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j = i;
            
            while (j >= gap && compare_int(arr[j - gap], temp) > 0) {
                arr[j] = arr[j - gap];
                g_stats.swaps++;
                j -= gap;
            }
            arr[j] = temp;
        }
    }
    
    free(gaps);
    
    clock_t end = clock();
    g_stats.time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
}

/* =============================================================================
 * SECTION 3: COUNTING SORT (STABLE)
 * =============================================================================
 */

void counting_sort(int *arr, int n, int max_val) {
    reset_stats();
    clock_t start = clock();
    
    int *count = calloc(max_val + 1, sizeof(int));
    int *output = malloc(n * sizeof(int));
    
    if (!count || !output) {
        free(count);
        free(output);
        return;
    }
    
    /* Count occurrences */
    for (int i = 0; i < n; i++) {
        count[arr[i]]++;
    }
    
    /* Cumulative count for stability */
    for (int i = 1; i <= max_val; i++) {
        count[i] += count[i - 1];
    }
    
    /* Build output array (traverse backwards for stability) */
    for (int i = n - 1; i >= 0; i--) {
        output[count[arr[i]] - 1] = arr[i];
        count[arr[i]]--;
        g_stats.swaps++;  /* Count as a move */
    }
    
    /* Copy back */
    memcpy(arr, output, n * sizeof(int));
    
    free(count);
    free(output);
    
    clock_t end = clock();
    g_stats.time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
}

/* =============================================================================
 * SECTION 4: RADIX SORT (LSD)
 * =============================================================================
 */

static int get_max(const int *arr, int n) {
    int max = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}

static void counting_sort_digit(int *arr, int n, int exp) {
    int *output = malloc(n * sizeof(int));
    int count[10] = {0};
    
    if (!output) return;
    
    /* Count occurrences of digits */
    for (int i = 0; i < n; i++) {
        count[(arr[i] / exp) % 10]++;
    }
    
    /* Cumulative count */
    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }
    
    /* Build output (backwards for stability) */
    for (int i = n - 1; i >= 0; i--) {
        int digit = (arr[i] / exp) % 10;
        output[count[digit] - 1] = arr[i];
        count[digit]--;
        g_stats.swaps++;
    }
    
    memcpy(arr, output, n * sizeof(int));
    free(output);
}

void radix_sort_lsd(int *arr, int n) {
    reset_stats();
    clock_t start = clock();
    
    int max = get_max(arr, n);
    
    /* Process each digit position */
    for (int exp = 1; max / exp > 0; exp *= 10) {
        counting_sort_digit(arr, n, exp);
    }
    
    clock_t end = clock();
    g_stats.time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
}

/* MSD Radix Sort for strings */
void radix_sort_msd_strings(char **arr, int n, int max_len) {
    reset_stats();
    clock_t start = clock();
    
    /* Implementation using recursive approach */
    /* Simplified: sort by character at each position */
    
    for (int pos = 0; pos < max_len; pos++) {
        /* Stable sort by character at position pos */
        int count[257] = {0};  /* 256 chars + empty */
        char **output = malloc(n * sizeof(char *));
        
        if (!output) return;
        
        /* Count */
        for (int i = 0; i < n; i++) {
            int c = (pos < (int)strlen(arr[i])) ? (unsigned char)arr[i][pos] + 1 : 0;
            count[c]++;
        }
        
        /* Cumulative */
        for (int i = 1; i < 257; i++) {
            count[i] += count[i - 1];
        }
        
        /* Build output */
        for (int i = n - 1; i >= 0; i--) {
            int c = (pos < (int)strlen(arr[i])) ? (unsigned char)arr[i][pos] + 1 : 0;
            output[count[c] - 1] = arr[i];
            count[c]--;
            g_stats.swaps++;
        }
        
        /* Copy back */
        for (int i = 0; i < n; i++) {
            arr[i] = output[i];
        }
        free(output);
    }
    
    clock_t end = clock();
    g_stats.time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
}

/* =============================================================================
 * SECTION 5: BUCKET SORT
 * =============================================================================
 */

typedef struct BucketNode {
    double value;
    struct BucketNode *next;
} BucketNode;

/* Insert into sorted bucket (insertion sort) */
static void insert_sorted(BucketNode **head, double value) {
    BucketNode *new_node = malloc(sizeof(BucketNode));
    if (!new_node) return;
    
    new_node->value = value;
    new_node->next = NULL;
    
    g_stats.swaps++;
    
    if (*head == NULL || (*head)->value >= value) {
        new_node->next = *head;
        *head = new_node;
        return;
    }
    
    BucketNode *current = *head;
    while (current->next != NULL && current->next->value < value) {
        g_stats.comparisons++;
        current = current->next;
    }
    g_stats.comparisons++;
    
    new_node->next = current->next;
    current->next = new_node;
}

void bucket_sort(double *arr, int n, int num_buckets) {
    reset_stats();
    clock_t start = clock();
    
    BucketNode **buckets = calloc(num_buckets, sizeof(BucketNode *));
    if (!buckets) return;
    
    /* Distribute into buckets */
    for (int i = 0; i < n; i++) {
        int bucket_idx = (int)(arr[i] * num_buckets);
        if (bucket_idx >= num_buckets) {
            bucket_idx = num_buckets - 1;
        }
        insert_sorted(&buckets[bucket_idx], arr[i]);
    }
    
    /* Concatenate buckets */
    int idx = 0;
    for (int i = 0; i < num_buckets; i++) {
        BucketNode *current = buckets[i];
        while (current) {
            arr[idx++] = current->value;
            BucketNode *temp = current;
            current = current->next;
            free(temp);
        }
    }
    
    free(buckets);
    
    clock_t end = clock();
    g_stats.time_ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
}

/* =============================================================================
 * SECTION 6: BENCHMARKING FRAMEWORK
 * =============================================================================
 */

typedef struct {
    const char *name;
    long long comparisons;
    long long swaps;
    double time_ms;
} BenchmarkResult;

static int *generate_random_array(int n) {
    int *arr = malloc(n * sizeof(int));
    if (!arr) return NULL;
    
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % (n * 10);
    }
    return arr;
}

static int *generate_sorted_array(int n) {
    int *arr = malloc(n * sizeof(int));
    if (!arr) return NULL;
    
    for (int i = 0; i < n; i++) {
        arr[i] = i;
    }
    return arr;
}

static int *generate_reverse_sorted_array(int n) {
    int *arr = malloc(n * sizeof(int));
    if (!arr) return NULL;
    
    for (int i = 0; i < n; i++) {
        arr[i] = n - i;
    }
    return arr;
}

static int *generate_nearly_sorted_array(int n) {
    int *arr = generate_sorted_array(n);
    if (!arr) return NULL;
    
    /* Swap about 5% of elements */
    int swaps = n / 20;
    for (int i = 0; i < swaps; i++) {
        int a = rand() % n;
        int b = rand() % n;
        int temp = arr[a];
        arr[a] = arr[b];
        arr[b] = temp;
    }
    return arr;
}

static void print_benchmark_table(const char *title, BenchmarkResult *results, int count) {
    printf("\n%s:\n", title);
    printf("┌──────────────────────────┬────────────────┬────────────┬────────────┐\n");
    printf("│ Algorithm                │ Comparisons    │ Swaps      │ Time(ms)   │\n");
    printf("├──────────────────────────┼────────────────┼────────────┼────────────┤\n");
    
    for (int i = 0; i < count; i++) {
        if (results[i].comparisons == -1) {
            printf("│ %-24s │ N/A            │ %-10lld │ %10.2f │\n",
                   results[i].name, results[i].swaps, results[i].time_ms);
        } else {
            printf("│ %-24s │ %-14lld │ %-10lld │ %10.2f │\n",
                   results[i].name, results[i].comparisons, results[i].swaps, results[i].time_ms);
        }
    }
    
    printf("└──────────────────────────┴────────────────┴────────────┴────────────┘\n");
}

void run_benchmark(int n) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("                    SORTING ALGORITHM BENCHMARK\n");
    printf("                         n = %d elements\n", n);
    printf("═══════════════════════════════════════════════════════════════════\n");
    
    srand(42);  /* Fixed seed for reproducibility */
    
    BenchmarkResult results[9];
    int num_results = 0;
    
    /* Test arrays */
    const char *array_types[] = {"Random Array", "Sorted Array", "Reverse-Sorted Array", "Nearly-Sorted Array"};
    int *(*generators[])(int) = {
        generate_random_array,
        generate_sorted_array,
        generate_reverse_sorted_array,
        generate_nearly_sorted_array
    };
    
    for (int t = 0; t < 4; t++) {
        int *base_arr = generators[t](n);
        if (!base_arr) continue;
        
        num_results = 0;
        
        /* QuickSort - First pivot */
        int *arr = copy_array(base_arr, n);
        quicksort(arr, n, PIVOT_FIRST);
        results[num_results++] = (BenchmarkResult){"QuickSort (first)", g_stats.comparisons, g_stats.swaps, g_stats.time_ms};
        free(arr);
        
        /* QuickSort - Median-of-3 */
        arr = copy_array(base_arr, n);
        quicksort(arr, n, PIVOT_MEDIAN3);
        results[num_results++] = (BenchmarkResult){"QuickSort (median3)", g_stats.comparisons, g_stats.swaps, g_stats.time_ms};
        free(arr);
        
        /* QuickSort - Random */
        arr = copy_array(base_arr, n);
        quicksort(arr, n, PIVOT_RANDOM);
        results[num_results++] = (BenchmarkResult){"QuickSort (random)", g_stats.comparisons, g_stats.swaps, g_stats.time_ms};
        free(arr);
        
        /* ShellSort - Shell gaps */
        arr = copy_array(base_arr, n);
        shellsort(arr, n, GAP_SHELL);
        results[num_results++] = (BenchmarkResult){"ShellSort (Shell)", g_stats.comparisons, g_stats.swaps, g_stats.time_ms};
        free(arr);
        
        /* ShellSort - Hibbard gaps */
        arr = copy_array(base_arr, n);
        shellsort(arr, n, GAP_HIBBARD);
        results[num_results++] = (BenchmarkResult){"ShellSort (Hibbard)", g_stats.comparisons, g_stats.swaps, g_stats.time_ms};
        free(arr);
        
        /* ShellSort - Knuth gaps */
        arr = copy_array(base_arr, n);
        shellsort(arr, n, GAP_KNUTH);
        results[num_results++] = (BenchmarkResult){"ShellSort (Knuth)", g_stats.comparisons, g_stats.swaps, g_stats.time_ms};
        free(arr);
        
        /* Counting Sort */
        arr = copy_array(base_arr, n);
        int max_val = get_max(arr, n);
        counting_sort(arr, n, max_val);
        results[num_results++] = (BenchmarkResult){"Counting Sort", -1, g_stats.swaps, g_stats.time_ms};
        free(arr);
        
        /* Radix Sort LSD */
        arr = copy_array(base_arr, n);
        radix_sort_lsd(arr, n);
        results[num_results++] = (BenchmarkResult){"Radix Sort (LSD)", -1, g_stats.swaps, g_stats.time_ms};
        free(arr);
        
        print_benchmark_table(array_types[t], results, num_results);
        free(base_arr);
    }
    
    /* Bucket sort test (special case for floats) */
    printf("\nBucket Sort (n=%d floats in [0,1)):\n", n);
    double *float_arr = malloc(n * sizeof(double));
    if (float_arr) {
        for (int i = 0; i < n; i++) {
            float_arr[i] = (double)rand() / RAND_MAX;
        }
        bucket_sort(float_arr, n, n / 10);
        printf("  Comparisons: %lld, Moves: %lld, Time: %.2f ms\n",
               g_stats.comparisons, g_stats.swaps, g_stats.time_ms);
        free(float_arr);
    }
    
    printf("\n═══════════════════════════════════════════════════════════════════\n");
}

/* =============================================================================
 * MAIN FUNCTION
 * =============================================================================
 */

void print_usage(const char *prog) {
    printf("Usage: %s <command> [args]\n\n", prog);
    printf("Commands:\n");
    printf("  benchmark <n>  Run benchmark with n elements\n");
    printf("  demo           Run demonstration with small arrays\n");
    printf("  help           Show this help message\n");
}

void run_demo(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║        HOMEWORK 1: SORTING LIBRARY DEMONSTRATION              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int arr[] = {64, 34, 25, 12, 22, 11, 90};
    int n = 7;
    
    printf("Original array: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n\n");
    
    /* QuickSort demo */
    int *test = copy_array(arr, n);
    quicksort(test, n, PIVOT_MEDIAN3);
    printf("QuickSort (median3): ");
    for (int i = 0; i < n; i++) printf("%d ", test[i]);
    printf("\n  Comparisons: %lld, Swaps: %lld\n\n", g_stats.comparisons, g_stats.swaps);
    free(test);
    
    /* ShellSort demo */
    test = copy_array(arr, n);
    shellsort(test, n, GAP_KNUTH);
    printf("ShellSort (Knuth): ");
    for (int i = 0; i < n; i++) printf("%d ", test[i]);
    printf("\n  Comparisons: %lld, Swaps: %lld\n\n", g_stats.comparisons, g_stats.swaps);
    free(test);
    
    /* Counting Sort demo */
    test = copy_array(arr, n);
    counting_sort(test, n, 100);
    printf("Counting Sort: ");
    for (int i = 0; i < n; i++) printf("%d ", test[i]);
    printf("\n  Moves: %lld\n\n", g_stats.swaps);
    free(test);
    
    /* Radix Sort demo */
    test = copy_array(arr, n);
    radix_sort_lsd(test, n);
    printf("Radix Sort (LSD): ");
    for (int i = 0; i < n; i++) printf("%d ", test[i]);
    printf("\n  Moves: %lld\n\n", g_stats.swaps);
    free(test);
    
    /* Bucket Sort demo */
    printf("Bucket Sort (floats in [0,1)):\n");
    double floats[] = {0.897, 0.565, 0.656, 0.123, 0.665, 0.343};
    int nf = 6;
    printf("  Original: ");
    for (int i = 0; i < nf; i++) printf("%.3f ", floats[i]);
    printf("\n");
    bucket_sort(floats, nf, 3);
    printf("  Sorted:   ");
    for (int i = 0; i < nf; i++) printf("%.3f ", floats[i]);
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    if (strcmp(argv[1], "benchmark") == 0) {
        int n = 10000;
        if (argc >= 3) {
            n = atoi(argv[2]);
        }
        run_benchmark(n);
    } else if (strcmp(argv[1], "demo") == 0) {
        run_demo();
    } else if (strcmp(argv[1], "help") == 0) {
        print_usage(argv[0]);
    } else {
        printf("Unknown command: %s\n", argv[1]);
        print_usage(argv[0]);
        return 1;
    }
    
    return 0;
}
