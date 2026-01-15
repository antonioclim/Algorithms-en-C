/**
 * =============================================================================
 * EXERCISE 1 SOLUTION: Sorting Algorithm Implementation and Analysis
 * =============================================================================
 *
 * Complete implementation of all sorting algorithms with statistics tracking.
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Global counters */
static long g_comparisons = 0;
static long g_swaps = 0;

typedef struct {
    long comparisons;
    long swaps;
    double time_ms;
} SortStats;

void reset_stats(void) {
    g_comparisons = 0;
    g_swaps = 0;
}

SortStats get_stats(void) {
    SortStats s = {g_comparisons, g_swaps, 0.0};
    return s;
}

/* SOLUTION TODO 1: swap_counted */
void swap_counted(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
    g_swaps++;
}

int compare_counted(int a, int b) {
    g_comparisons++;
    return a - b;
}

void copy_array(int dest[], int src[], int n) {
    for (int i = 0; i < n; i++) dest[i] = src[i];
}

void print_array(int arr[], int n) {
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
}

/* SOLUTION TODO 2: merge */
void merge(int arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));
    
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];
    
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (compare_counted(L[i], R[j]) <= 0) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    
    free(L);
    free(R);
}

/* SOLUTION TODO 3: merge_sort */
void merge_sort(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(arr, left, mid);
        merge_sort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

/* SOLUTION TODO 4: partition_last */
int partition_last(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    
    for (int j = low; j < high; j++) {
        if (compare_counted(arr[j], pivot) <= 0) {
            i++;
            swap_counted(&arr[i], &arr[j]);
        }
    }
    swap_counted(&arr[i + 1], &arr[high]);
    return i + 1;
}

/* SOLUTION TODO 5: partition_median3 */
int partition_median3(int arr[], int low, int high) {
    if (high - low < 2) return partition_last(arr, low, high);
    
    int mid = low + (high - low) / 2;
    
    /* Sort low, mid, high */
    if (compare_counted(arr[mid], arr[low]) < 0)
        swap_counted(&arr[mid], &arr[low]);
    if (compare_counted(arr[high], arr[low]) < 0)
        swap_counted(&arr[high], &arr[low]);
    if (compare_counted(arr[high], arr[mid]) < 0)
        swap_counted(&arr[high], &arr[mid]);
    
    /* Move median to high position */
    swap_counted(&arr[mid], &arr[high]);
    
    return partition_last(arr, low, high);
}

/* SOLUTION TODO 6: partition_random */
int partition_random(int arr[], int low, int high) {
    int rand_idx = low + rand() % (high - low + 1);
    swap_counted(&arr[rand_idx], &arr[high]);
    return partition_last(arr, low, high);
}

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

/* SOLUTION TODO 7: shell_sort with Hibbard gaps */
void shell_sort(int arr[], int n) {
    /* Generate Hibbard gaps: 2^k - 1 */
    int gaps[32];
    int num_gaps = 0;
    int gap = 1;
    while (gap < n) {
        gaps[num_gaps++] = gap;
        gap = 2 * gap + 1;  /* 1, 3, 7, 15, 31, ... */
    }
    
    /* Apply gaps from largest to smallest */
    for (int g = num_gaps - 1; g >= 0; g--) {
        gap = gaps[g];
        
        /* Gapped insertion sort */
        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j = i;
            
            while (j >= gap && compare_counted(arr[j - gap], temp) > 0) {
                arr[j] = arr[j - gap];
                g_swaps++;
                j -= gap;
            }
            if (j != i) {
                arr[j] = temp;
                g_swaps++;
            }
        }
    }
}

/* Wrappers */
void merge_sort_wrapper(int arr[], int n, int param) {
    (void)param;
    merge_sort(arr, 0, n - 1);
}

void shell_sort_wrapper(int arr[], int n, int param) {
    (void)param;
    shell_sort(arr, n);
}

SortStats run_sort(void (*sort_func)(int[], int, int), int arr[], int n, int param) {
    int *copy = (int *)malloc(n * sizeof(int));
    copy_array(copy, arr, n);
    
    reset_stats();
    clock_t start = clock();
    sort_func(copy, n, param);
    clock_t end = clock();
    
    SortStats stats = get_stats();
    stats.time_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
    
    int sorted = 1;
    for (int i = 1; i < n && sorted; i++) {
        if (copy[i] < copy[i-1]) sorted = 0;
    }
    
    if (!sorted) printf("  WARNING: Array not sorted correctly!\n");
    
    printf("  Sorted: ");
    if (n <= 20) print_array(copy, n);
    else printf("[%d elements]", n);
    printf("\n");
    
    free(copy);
    return stats;
}

int main(void) {
    int n;
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 1: Sorting Algorithm Comparison                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Enter array size: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        fprintf(stderr, "Invalid input\n");
        return 1;
    }
    
    int *arr = (int *)malloc(n * sizeof(int));
    printf("Enter %d integers: ", n);
    for (int i = 0; i < n; i++) scanf("%d", &arr[i]);
    
    printf("\nOriginal: ");
    print_array(arr, n);
    printf("\n\n");
    
    srand((unsigned int)time(NULL));
    SortStats stats;
    
    printf("─────────────────────────────────────────────────────────────\n");
    printf("MergeSort:\n");
    stats = run_sort(merge_sort_wrapper, arr, n, 0);
    printf("  Comparisons: %ld, Swaps: %ld, Time: %.3f ms\n\n", 
           stats.comparisons, stats.swaps, stats.time_ms);
    
    printf("─────────────────────────────────────────────────────────────\n");
    printf("QuickSort (last element pivot):\n");
    stats = run_sort(quick_sort, arr, n, 0);
    printf("  Comparisons: %ld, Swaps: %ld, Time: %.3f ms\n\n",
           stats.comparisons, stats.swaps, stats.time_ms);
    
    printf("─────────────────────────────────────────────────────────────\n");
    printf("QuickSort (median-of-three pivot):\n");
    stats = run_sort(quick_sort, arr, n, 1);
    printf("  Comparisons: %ld, Swaps: %ld, Time: %.3f ms\n\n",
           stats.comparisons, stats.swaps, stats.time_ms);
    
    printf("─────────────────────────────────────────────────────────────\n");
    printf("QuickSort (random pivot):\n");
    stats = run_sort(quick_sort, arr, n, 2);
    printf("  Comparisons: %ld, Swaps: %ld, Time: %.3f ms\n\n",
           stats.comparisons, stats.swaps, stats.time_ms);
    
    printf("─────────────────────────────────────────────────────────────\n");
    printf("ShellSort (Hibbard gaps):\n");
    stats = run_sort(shell_sort_wrapper, arr, n, 0);
    printf("  Comparisons: %ld, Swaps: %ld, Time: %.3f ms\n\n",
           stats.comparisons, stats.swaps, stats.time_ms);
    
    printf("═════════════════════════════════════════════════════════════\n");
    
    free(arr);
    return 0;
}
