/**
 * =============================================================================
 * WEEK 15: ALGORITHMIC PARADIGMS
 * Complete Working Example
 * =============================================================================
 *
 * This example demonstrates:
 *   1. Divide and Conquer: MergeSort, QuickSort, Binary Search
 *   2. Recurrence relation analysis and Master Theorem application
 *   3. Greedy Algorithms: Activity Selection, Huffman Coding, Fractional Knapsack
 *   4. Dynamic Programming: Fibonacci (3 versions), 0/1 Knapsack, LCS
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o example1 example1.c -lm
 * Usage: ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

/* =============================================================================
 * PART 1: DIVIDE AND CONQUER - MERGESORT
 * =============================================================================
 */

/**
 * Merge two sorted subarrays into a single sorted array
 * 
 * @param arr The array containing both subarrays
 * @param left Left index of first subarray
 * @param mid Middle index (end of first subarray)
 * @param right Right index of second subarray
 */
void merge(int arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    /* Create temporary arrays */
    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));
    
    if (!L || !R) {
        fprintf(stderr, "Memory allocation failed\n");
        free(L);
        free(R);
        return;
    }
    
    /* Copy data to temp arrays */
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];
    
    /* Merge temp arrays back */
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    
    /* Copy remaining elements */
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    
    free(L);
    free(R);
}

/**
 * MergeSort implementation with step-by-step output
 */
void merge_sort_verbose(int arr[], int left, int right, int depth) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        /* Print recursion tree visualisation */
        for (int i = 0; i < depth; i++) printf("  ");
        printf("├─ Sorting [%d..%d], mid=%d\n", left, right, mid);
        
        merge_sort_verbose(arr, left, mid, depth + 1);
        merge_sort_verbose(arr, mid + 1, right, depth + 1);
        merge(arr, left, mid, right);
        
        /* Show merged result */
        for (int i = 0; i < depth; i++) printf("  ");
        printf("└─ Merged: ");
        for (int i = left; i <= right; i++) printf("%d ", arr[i]);
        printf("\n");
    }
}

/**
 * Standard MergeSort (no output)
 */
void merge_sort(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(arr, left, mid);
        merge_sort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

void demo_mergesort(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 1: DIVIDE AND CONQUER - MERGESORT                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int arr[] = {38, 27, 43, 3, 9, 82, 10};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    printf("Original array: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n\n");
    
    printf("Recursion tree (divide and conquer):\n");
    merge_sort_verbose(arr, 0, n - 1, 0);
    
    printf("\nFinal sorted array: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n\n");
    
    printf("Complexity Analysis:\n");
    printf("  • Recurrence: T(n) = 2T(n/2) + O(n)\n");
    printf("  • Master Theorem: a=2, b=2, f(n)=n\n");
    printf("  • log_b(a) = log_2(2) = 1\n");
    printf("  • f(n) = Θ(n^1) → Case 2 applies\n");
    printf("  • T(n) = Θ(n log n)\n");
    printf("\n");
}

/* =============================================================================
 * PART 2: DIVIDE AND CONQUER - QUICKSORT
 * =============================================================================
 */

/**
 * Swap two integers
 */
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Lomuto partition scheme
 * Pivot is the last element
 */
int partition_lomuto(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

/**
 * Median-of-three pivot selection
 */
int median_of_three(int arr[], int low, int high) {
    int mid = low + (high - low) / 2;
    
    /* Sort low, mid, high */
    if (arr[mid] < arr[low]) swap(&arr[mid], &arr[low]);
    if (arr[high] < arr[low]) swap(&arr[high], &arr[low]);
    if (arr[high] < arr[mid]) swap(&arr[high], &arr[mid]);
    
    /* Move median to high-1 position */
    swap(&arr[mid], &arr[high - 1]);
    return arr[high - 1];
}

/**
 * QuickSort with verbose output
 */
void quick_sort_verbose(int arr[], int low, int high, int depth) {
    if (low < high) {
        for (int i = 0; i < depth; i++) printf("  ");
        printf("├─ QuickSort [%d..%d], pivot=%d\n", low, high, arr[high]);
        
        int pi = partition_lomuto(arr, low, high);
        
        for (int i = 0; i < depth; i++) printf("  ");
        printf("│  After partition: ");
        for (int i = low; i <= high; i++) {
            if (i == pi) printf("[%d] ", arr[i]);
            else printf("%d ", arr[i]);
        }
        printf("\n");
        
        quick_sort_verbose(arr, low, pi - 1, depth + 1);
        quick_sort_verbose(arr, pi + 1, high, depth + 1);
    }
}

/**
 * Standard QuickSort (no output)
 */
void quick_sort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition_lomuto(arr, low, high);
        quick_sort(arr, low, pi - 1);
        quick_sort(arr, pi + 1, high);
    }
}

void demo_quicksort(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: DIVIDE AND CONQUER - QUICKSORT                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int arr[] = {10, 7, 8, 9, 1, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    printf("Original array: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n\n");
    
    printf("QuickSort with Lomuto partitioning:\n");
    quick_sort_verbose(arr, 0, n - 1, 0);
    
    printf("\nFinal sorted array: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n\n");
    
    printf("Pivot Selection Strategies:\n");
    printf("  ┌────────────────────────┬──────────────────────────────┐\n");
    printf("  │ Strategy               │ Worst Case Trigger           │\n");
    printf("  ├────────────────────────┼──────────────────────────────┤\n");
    printf("  │ First/Last element     │ Already sorted input         │\n");
    printf("  │ Median-of-three        │ Specially crafted input      │\n");
    printf("  │ Random pivot           │ Probabilistically unlikely   │\n");
    printf("  └────────────────────────┴──────────────────────────────┘\n\n");
    
    printf("Complexity:\n");
    printf("  • Average: O(n log n)\n");
    printf("  • Worst:   O(n²) - when pivot is always min or max\n");
    printf("  • Space:   O(log n) for recursion stack\n");
    printf("\n");
}

/* =============================================================================
 * PART 3: GREEDY - ACTIVITY SELECTION
 * =============================================================================
 */

typedef struct {
    int start;
    int finish;
    int index;
} Activity;

/**
 * Comparator for sorting by finish time
 */
int cmp_activity_finish(const void *a, const void *b) {
    return ((Activity *)a)->finish - ((Activity *)b)->finish;
}

/**
 * Greedy activity selection
 * Returns number of selected activities, fills selected[] with indices
 */
int activity_selection(Activity activities[], int n, int selected[]) {
    /* Sort by finish time */
    qsort(activities, n, sizeof(Activity), cmp_activity_finish);
    
    int count = 0;
    selected[count++] = activities[0].index;
    int last_finish = activities[0].finish;
    
    for (int i = 1; i < n; i++) {
        if (activities[i].start >= last_finish) {
            selected[count++] = activities[i].index;
            last_finish = activities[i].finish;
        }
    }
    
    return count;
}

void demo_activity_selection(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 3: GREEDY - ACTIVITY SELECTION                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Activity activities[] = {
        {1, 4, 0}, {3, 5, 1}, {0, 6, 2}, {5, 7, 3},
        {3, 9, 4}, {5, 9, 5}, {6, 10, 6}, {8, 11, 7},
        {8, 12, 8}, {2, 14, 9}, {12, 16, 10}
    };
    int n = sizeof(activities) / sizeof(activities[0]);
    
    printf("Activities (start, finish):\n");
    printf("  ┌───────┬───────┬────────┐\n");
    printf("  │ Index │ Start │ Finish │\n");
    printf("  ├───────┼───────┼────────┤\n");
    for (int i = 0; i < n; i++) {
        printf("  │   %2d  │  %2d   │   %2d   │\n", 
               activities[i].index, activities[i].start, activities[i].finish);
    }
    printf("  └───────┴───────┴────────┘\n\n");
    
    int selected[11];
    int count = activity_selection(activities, n, selected);
    
    printf("Greedy selection (by earliest finish time):\n");
    printf("  Selected activities: ");
    for (int i = 0; i < count; i++) {
        printf("A%d ", selected[i]);
    }
    printf("\n  Total: %d activities\n\n", count);
    
    printf("Timeline visualisation:\n");
    printf("  Time:  0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16\n");
    printf("         │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │\n");
    
    /* Simplified timeline */
    printf("  A0:    │▓▓▓▓│  │  │  │  │  │  │  │  │  │  │  │  │\n");
    printf("  A3:    │  │  │  │  │▓▓▓│  │  │  │  │  │  │  │  │\n");
    printf("  A7:    │  │  │  │  │  │  │  │▓▓▓▓│  │  │  │  │  │\n");
    printf("  A10:   │  │  │  │  │  │  │  │  │  │  │  │▓▓▓▓▓│\n\n");
    
    printf("Greedy Choice Property:\n");
    printf("  • Always select the activity that finishes earliest\n");
    printf("  • This maximises remaining time for future activities\n");
    printf("  • Proof: Exchange argument shows any optimal solution can\n");
    printf("    include the earliest-finishing activity\n\n");
}

/* =============================================================================
 * PART 4: GREEDY - FRACTIONAL KNAPSACK
 * =============================================================================
 */

typedef struct {
    int weight;
    int value;
    double ratio;
    int index;
} Item;

int cmp_item_ratio(const void *a, const void *b) {
    double diff = ((Item *)b)->ratio - ((Item *)a)->ratio;
    return (diff > 0) ? 1 : (diff < 0) ? -1 : 0;
}

/**
 * Fractional Knapsack - greedy solution
 */
double fractional_knapsack(Item items[], int n, int capacity) {
    /* Calculate value-to-weight ratio */
    for (int i = 0; i < n; i++) {
        items[i].ratio = (double)items[i].value / items[i].weight;
    }
    
    /* Sort by ratio (descending) */
    qsort(items, n, sizeof(Item), cmp_item_ratio);
    
    double total_value = 0.0;
    int remaining = capacity;
    
    printf("Selecting items (sorted by v/w ratio):\n");
    
    for (int i = 0; i < n && remaining > 0; i++) {
        if (items[i].weight <= remaining) {
            /* Take entire item */
            total_value += items[i].value;
            remaining -= items[i].weight;
            printf("  • Item %d: take 100%% (w=%d, v=%d) → total=%.2f\n",
                   items[i].index, items[i].weight, items[i].value, total_value);
        } else {
            /* Take fraction */
            double fraction = (double)remaining / items[i].weight;
            total_value += fraction * items[i].value;
            printf("  • Item %d: take %.1f%% (w=%d of %d, v=%.2f) → total=%.2f\n",
                   items[i].index, fraction * 100, remaining, items[i].weight,
                   fraction * items[i].value, total_value);
            remaining = 0;
        }
    }
    
    return total_value;
}

void demo_fractional_knapsack(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 4: GREEDY - FRACTIONAL KNAPSACK                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Item items[] = {
        {10, 60, 0, 0},  /* w=10, v=60, ratio=6.0 */
        {20, 100, 0, 1}, /* w=20, v=100, ratio=5.0 */
        {30, 120, 0, 2}  /* w=30, v=120, ratio=4.0 */
    };
    int n = sizeof(items) / sizeof(items[0]);
    int capacity = 50;
    
    printf("Items:\n");
    printf("  ┌───────┬────────┬───────┬───────────┐\n");
    printf("  │ Index │ Weight │ Value │ Ratio v/w │\n");
    printf("  ├───────┼────────┼───────┼───────────┤\n");
    for (int i = 0; i < n; i++) {
        printf("  │   %d   │   %2d   │  %3d  │   %.2f    │\n",
               items[i].index, items[i].weight, items[i].value,
               (double)items[i].value / items[i].weight);
    }
    printf("  └───────┴────────┴───────┴───────────┘\n");
    printf("  Capacity: %d\n\n", capacity);
    
    double max_value = fractional_knapsack(items, n, capacity);
    
    printf("\n  Maximum value: %.2f\n\n", max_value);
    
    printf("Note: Greedy works for Fractional Knapsack but NOT for 0/1 Knapsack!\n");
    printf("      For 0/1, we need Dynamic Programming.\n\n");
}

/* =============================================================================
 * PART 5: DYNAMIC PROGRAMMING - FIBONACCI
 * =============================================================================
 */

/**
 * Naive recursive Fibonacci - O(2^n)
 */
long long fib_naive(int n) {
    if (n <= 1) return n;
    return fib_naive(n - 1) + fib_naive(n - 2);
}

/**
 * Memoised Fibonacci - O(n) time, O(n) space
 */
long long fib_memo_helper(int n, long long *cache) {
    if (n <= 1) return n;
    if (cache[n] != -1) return cache[n];
    cache[n] = fib_memo_helper(n - 1, cache) + fib_memo_helper(n - 2, cache);
    return cache[n];
}

long long fib_memo(int n) {
    long long *cache = (long long *)malloc((n + 1) * sizeof(long long));
    if (!cache) return -1;
    
    for (int i = 0; i <= n; i++) cache[i] = -1;
    long long result = fib_memo_helper(n, cache);
    free(cache);
    return result;
}

/**
 * Tabulated Fibonacci - O(n) time, O(1) space
 */
long long fib_tab(int n) {
    if (n <= 1) return n;
    
    long long prev2 = 0, prev1 = 1;
    for (int i = 2; i <= n; i++) {
        long long curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}

void demo_fibonacci(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 5: DYNAMIC PROGRAMMING - FIBONACCI                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Three implementations compared:\n\n");
    
    printf("1. Naive Recursive (exponential):\n");
    printf("   Recurrence tree shows massive redundancy:\n");
    printf("                       fib(5)\n");
    printf("                      /      \\\n");
    printf("                 fib(4)      fib(3)\n");
    printf("                /    \\       /    \\\n");
    printf("            fib(3) fib(2) fib(2) fib(1)\n");
    printf("            /  \\\n");
    printf("        fib(2) fib(1)\n\n");
    
    printf("2. Memoised (top-down DP):\n");
    printf("   Cache computed values to avoid recomputation\n\n");
    
    printf("3. Tabulated (bottom-up DP):\n");
    printf("   Build solution iteratively, O(1) space possible\n\n");
    
    printf("Results for small n:\n");
    printf("  ┌────┬────────┬──────────┬───────────┐\n");
    printf("  │  n │ Naive  │ Memoised │ Tabulated │\n");
    printf("  ├────┼────────┼──────────┼───────────┤\n");
    for (int n = 5; n <= 20; n += 5) {
        printf("  │ %2d │ %6lld │  %6lld  │   %6lld  │\n",
               n, fib_naive(n), fib_memo(n), fib_tab(n));
    }
    printf("  └────┴────────┴──────────┴───────────┘\n\n");
    
    /* Timing comparison for larger n */
    printf("Timing comparison (n=35):\n");
    clock_t start, end;
    
    start = clock();
    long long r1 = fib_naive(35);
    end = clock();
    printf("  Naive:     %lld (%.3f sec)\n", r1, (double)(end - start) / CLOCKS_PER_SEC);
    
    start = clock();
    long long r2 = fib_memo(35);
    end = clock();
    printf("  Memoised:  %lld (%.6f sec)\n", r2, (double)(end - start) / CLOCKS_PER_SEC);
    
    start = clock();
    long long r3 = fib_tab(35);
    end = clock();
    printf("  Tabulated: %lld (%.6f sec)\n\n", r3, (double)(end - start) / CLOCKS_PER_SEC);
}

/* =============================================================================
 * PART 6: DYNAMIC PROGRAMMING - 0/1 KNAPSACK
 * =============================================================================
 */

/**
 * 0/1 Knapsack with table visualisation
 */
int knapsack_01(int weights[], int values[], int n, int capacity) {
    /* Allocate DP table */
    int **dp = (int **)malloc((n + 1) * sizeof(int *));
    for (int i = 0; i <= n; i++) {
        dp[i] = (int *)calloc((capacity + 1), sizeof(int));
    }
    
    /* Fill DP table */
    for (int i = 1; i <= n; i++) {
        for (int w = 1; w <= capacity; w++) {
            if (weights[i-1] <= w) {
                int take = values[i-1] + dp[i-1][w - weights[i-1]];
                int skip = dp[i-1][w];
                dp[i][w] = (take > skip) ? take : skip;
            } else {
                dp[i][w] = dp[i-1][w];
            }
        }
    }
    
    /* Print table */
    printf("DP Table (dp[i][w] = max value with items 1..i and capacity w):\n\n");
    printf("     w→ ");
    for (int w = 0; w <= capacity; w++) printf("%3d ", w);
    printf("\n   i↓   ");
    for (int w = 0; w <= capacity; w++) printf("────");
    printf("\n");
    
    for (int i = 0; i <= n; i++) {
        if (i == 0) printf("    %d │ ", i);
        else printf("    %d │ ", i);
        for (int w = 0; w <= capacity; w++) {
            printf("%3d ", dp[i][w]);
        }
        if (i > 0) {
            printf(" ← item %d (w=%d, v=%d)", i, weights[i-1], values[i-1]);
        }
        printf("\n");
    }
    
    /* Backtrack to find selected items */
    printf("\nSelected items (backtracking):\n");
    int w = capacity;
    int total_weight = 0;
    for (int i = n; i > 0 && w > 0; i--) {
        if (dp[i][w] != dp[i-1][w]) {
            printf("  • Item %d: weight=%d, value=%d\n", i, weights[i-1], values[i-1]);
            w -= weights[i-1];
            total_weight += weights[i-1];
        }
    }
    
    int result = dp[n][capacity];
    
    printf("\nMaximum value: %d (using weight %d of %d)\n", result, total_weight, capacity);
    
    /* Free memory */
    for (int i = 0; i <= n; i++) free(dp[i]);
    free(dp);
    
    return result;
}

void demo_knapsack_01(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 6: DYNAMIC PROGRAMMING - 0/1 KNAPSACK              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int weights[] = {2, 3, 4, 5};
    int values[] = {3, 4, 5, 6};
    int n = sizeof(weights) / sizeof(weights[0]);
    int capacity = 5;
    
    printf("Problem instance:\n");
    printf("  ┌──────┬────────┬───────┐\n");
    printf("  │ Item │ Weight │ Value │\n");
    printf("  ├──────┼────────┼───────┤\n");
    for (int i = 0; i < n; i++) {
        printf("  │  %d   │   %d    │   %d   │\n", i + 1, weights[i], values[i]);
    }
    printf("  └──────┴────────┴───────┘\n");
    printf("  Capacity: %d\n\n", capacity);
    
    printf("Recurrence relation:\n");
    printf("  dp[i][w] = max(\n");
    printf("      dp[i-1][w],              // Don't take item i\n");
    printf("      dp[i-1][w-w_i] + v_i     // Take item i (if w_i ≤ w)\n");
    printf("  )\n\n");
    
    knapsack_01(weights, values, n, capacity);
    printf("\n");
}

/* =============================================================================
 * PART 7: DYNAMIC PROGRAMMING - LONGEST COMMON SUBSEQUENCE
 * =============================================================================
 */

/**
 * LCS with table visualisation and backtracking
 */
int lcs(const char *X, const char *Y, char *result) {
    int m = strlen(X);
    int n = strlen(Y);
    
    /* Allocate DP table */
    int **dp = (int **)malloc((m + 1) * sizeof(int *));
    for (int i = 0; i <= m; i++) {
        dp[i] = (int *)calloc((n + 1), sizeof(int));
    }
    
    /* Fill DP table */
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (X[i-1] == Y[j-1]) {
                dp[i][j] = dp[i-1][j-1] + 1;
            } else {
                dp[i][j] = (dp[i-1][j] > dp[i][j-1]) ? dp[i-1][j] : dp[i][j-1];
            }
        }
    }
    
    /* Print table */
    printf("DP Table:\n\n");
    printf("       ");
    for (int j = 0; j <= n; j++) {
        if (j == 0) printf("  ε ");
        else printf("  %c ", Y[j-1]);
    }
    printf("\n     ");
    for (int j = 0; j <= n; j++) printf("────");
    printf("\n");
    
    for (int i = 0; i <= m; i++) {
        if (i == 0) printf("   ε │");
        else printf("   %c │", X[i-1]);
        for (int j = 0; j <= n; j++) {
            printf(" %2d ", dp[i][j]);
        }
        printf("\n");
    }
    
    /* Backtrack to find LCS */
    int len = dp[m][n];
    result[len] = '\0';
    
    int i = m, j = n, k = len - 1;
    while (i > 0 && j > 0) {
        if (X[i-1] == Y[j-1]) {
            result[k--] = X[i-1];
            i--; j--;
        } else if (dp[i-1][j] > dp[i][j-1]) {
            i--;
        } else {
            j--;
        }
    }
    
    /* Free memory */
    for (int i = 0; i <= m; i++) free(dp[i]);
    free(dp);
    
    return len;
}

void demo_lcs(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 7: DYNAMIC PROGRAMMING - LCS                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    const char *X = "ABCBDAB";
    const char *Y = "BDCABA";
    char result[20];
    
    printf("Sequences:\n");
    printf("  X = \"%s\"\n", X);
    printf("  Y = \"%s\"\n\n", Y);
    
    printf("Recurrence:\n");
    printf("  dp[i][j] = dp[i-1][j-1] + 1           if X[i] == Y[j]\n");
    printf("           = max(dp[i-1][j], dp[i][j-1]) otherwise\n\n");
    
    int len = lcs(X, Y, result);
    
    printf("\nLongest Common Subsequence: \"%s\" (length %d)\n\n", result, len);
    
    printf("Note: A subsequence maintains relative order but need not be\n");
    printf("      contiguous. Compare with substring which must be contiguous.\n\n");
}

/* =============================================================================
 * PART 8: PARADIGM COMPARISON
 * =============================================================================
 */

void demo_paradigm_comparison(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 8: PARADIGM COMPARISON                             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("When to use each paradigm:\n\n");
    
    printf("┌───────────────────┬─────────────────────────────────────────────┐\n");
    printf("│ DIVIDE & CONQUER  │ Problem decomposable into independent subs  │\n");
    printf("│                   │ Subproblems don't overlap                   │\n");
    printf("│                   │ Examples: MergeSort, QuickSort, Binary Srch │\n");
    printf("├───────────────────┼─────────────────────────────────────────────┤\n");
    printf("│ GREEDY            │ Local optimal → Global optimal              │\n");
    printf("│                   │ No need to reconsider past choices          │\n");
    printf("│                   │ Examples: Activity Selection, Huffman, MST  │\n");
    printf("├───────────────────┼─────────────────────────────────────────────┤\n");
    printf("│ DYNAMIC PROG.     │ Overlapping subproblems                     │\n");
    printf("│                   │ Optimal substructure                        │\n");
    printf("│                   │ Examples: Knapsack, LCS, Edit Distance      │\n");
    printf("└───────────────────┴─────────────────────────────────────────────┘\n\n");
    
    printf("Decision flowchart:\n\n");
    printf("  Problem → Can be decomposed? ─No─→ Not D&C/DP\n");
    printf("                │\n");
    printf("               Yes\n");
    printf("                │\n");
    printf("                ▼\n");
    printf("         Overlapping subproblems? ─No─→ DIVIDE & CONQUER\n");
    printf("                │\n");
    printf("               Yes\n");
    printf("                │\n");
    printf("                ▼\n");
    printf("         Greedy choice property? ─Yes─→ GREEDY\n");
    printf("                │\n");
    printf("                No\n");
    printf("                │\n");
    printf("                ▼\n");
    printf("         DYNAMIC PROGRAMMING\n\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 15: ALGORITHMIC PARADIGMS                           ║\n");
    printf("║                    Complete Example                          ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    demo_mergesort();
    demo_quicksort();
    demo_activity_selection();
    demo_fractional_knapsack();
    demo_fibonacci();
    demo_knapsack_01();
    demo_lcs();
    demo_paradigm_comparison();
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    DEMONSTRATION COMPLETE                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
