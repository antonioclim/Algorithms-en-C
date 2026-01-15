/**
 * =============================================================================
 * HOMEWORK 2 SOLUTION: K-Way Merge Using Min-Heap
 * =============================================================================
 *
 * INSTRUCTOR SOLUTION
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

/* =============================================================================
 * CONSTANTS AND MACROS
 * =============================================================================
 */

#define PARENT(i)       (((i) - 1) / 2)
#define LEFT_CHILD(i)   (2 * (i) + 1)
#define RIGHT_CHILD(i)  (2 * (i) + 2)

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * Heap node for K-way merge
 * Tracks which array and position the value came from
 */
typedef struct {
    int value;          /* Current value */
    int array_idx;      /* Which array (0 to k-1) */
    int elem_idx;       /* Index within that array */
} MergeNode;

/**
 * Min-heap for merge operation
 */
typedef struct {
    MergeNode *nodes;
    size_t size;
    size_t capacity;
} MinHeap;

/* =============================================================================
 * MIN-HEAP OPERATIONS
 * =============================================================================
 */

/**
 * Create min-heap
 */
MinHeap *minheap_create(size_t capacity) {
    MinHeap *h = malloc(sizeof(MinHeap));
    if (!h) return NULL;
    
    h->nodes = malloc(capacity * sizeof(MergeNode));
    if (!h->nodes) {
        free(h);
        return NULL;
    }
    
    h->size = 0;
    h->capacity = capacity;
    return h;
}

/**
 * Destroy min-heap
 */
void minheap_destroy(MinHeap *h) {
    if (h) {
        free(h->nodes);
        free(h);
    }
}

/**
 * Sift up for min-heap
 */
static void minheap_sift_up(MinHeap *h, size_t i) {
    while (i > 0) {
        size_t parent = PARENT(i);
        
        if (h->nodes[i].value >= h->nodes[parent].value)
            break;
        
        /* Swap */
        MergeNode temp = h->nodes[i];
        h->nodes[i] = h->nodes[parent];
        h->nodes[parent] = temp;
        
        i = parent;
    }
}

/**
 * Sift down for min-heap
 */
static void minheap_sift_down(MinHeap *h, size_t i) {
    while (true) {
        size_t smallest = i;
        size_t left = LEFT_CHILD(i);
        size_t right = RIGHT_CHILD(i);
        
        if (left < h->size && 
            h->nodes[left].value < h->nodes[smallest].value)
            smallest = left;
        
        if (right < h->size && 
            h->nodes[right].value < h->nodes[smallest].value)
            smallest = right;
        
        if (smallest == i)
            break;
        
        MergeNode temp = h->nodes[i];
        h->nodes[i] = h->nodes[smallest];
        h->nodes[smallest] = temp;
        
        i = smallest;
    }
}

/**
 * Insert into min-heap
 */
bool minheap_insert(MinHeap *h, MergeNode node) {
    if (h->size >= h->capacity)
        return false;
    
    h->nodes[h->size] = node;
    h->size++;
    
    minheap_sift_up(h, h->size - 1);
    return true;
}

/**
 * Extract minimum from heap
 */
bool minheap_extract_min(MinHeap *h, MergeNode *node) {
    if (h->size == 0)
        return false;
    
    *node = h->nodes[0];
    
    h->nodes[0] = h->nodes[h->size - 1];
    h->size--;
    
    if (h->size > 0)
        minheap_sift_down(h, 0);
    
    return true;
}

/**
 * Check if heap is empty
 */
bool minheap_is_empty(const MinHeap *h) {
    return h == NULL || h->size == 0;
}

/* =============================================================================
 * K-WAY MERGE ALGORITHM
 * =============================================================================
 */

/**
 * Merge K sorted arrays into a single sorted array
 *
 * @param arrays        Array of pointers to sorted arrays
 * @param sizes         Array of sizes for each input array
 * @param k             Number of arrays to merge
 * @param result        Output array (must be pre-allocated)
 * @param result_size   Output: total number of elements merged
 * @return              true on success, false on failure
 *
 * Time complexity: O(N log K) where N is total elements
 * Space complexity: O(K) for the heap
 */
bool merge_k_sorted(int **arrays, int *sizes, int k, 
                    int *result, int *result_size) {
    
    /* Handle edge cases */
    if (k == 0 || !arrays || !sizes || !result || !result_size) {
        if (result_size) *result_size = 0;
        return false;
    }
    
    /* Calculate total elements */
    int total = 0;
    for (int i = 0; i < k; i++) {
        if (sizes[i] < 0) return false;
        total += sizes[i];
    }
    
    /* Handle empty input */
    if (total == 0) {
        *result_size = 0;
        return true;
    }
    
    /* Handle single array case */
    if (k == 1) {
        memcpy(result, arrays[0], sizes[0] * sizeof(int));
        *result_size = sizes[0];
        return true;
    }
    
    /* Create min-heap with capacity K */
    MinHeap *heap = minheap_create((size_t)k);
    if (!heap) return false;
    
    /* Initialise heap with first element from each non-empty array */
    for (int i = 0; i < k; i++) {
        if (sizes[i] > 0) {
            MergeNode node = {
                .value = arrays[i][0],
                .array_idx = i,
                .elem_idx = 0
            };
            minheap_insert(heap, node);
        }
    }
    
    /* Merge process */
    int out_idx = 0;
    
    while (!minheap_is_empty(heap)) {
        /* Extract minimum */
        MergeNode min;
        minheap_extract_min(heap, &min);
        
        /* Add to result */
        result[out_idx++] = min.value;
        
        /* If more elements in this array, insert next one */
        int arr_idx = min.array_idx;
        int next_elem_idx = min.elem_idx + 1;
        
        if (next_elem_idx < sizes[arr_idx]) {
            MergeNode next = {
                .value = arrays[arr_idx][next_elem_idx],
                .array_idx = arr_idx,
                .elem_idx = next_elem_idx
            };
            minheap_insert(heap, next);
        }
    }
    
    *result_size = out_idx;
    
    minheap_destroy(heap);
    return true;
}

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Print array
 */
void print_array(const int *arr, int n, const char *label) {
    printf("%s: [", label);
    for (int i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

/**
 * Verify array is sorted
 */
bool is_sorted(const int *arr, int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i] < arr[i - 1])
            return false;
    }
    return true;
}

/**
 * Calculate sum of array (for verification)
 */
long long array_sum(const int *arr, int n) {
    long long sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

/* =============================================================================
 * TEST FUNCTIONS
 * =============================================================================
 */

bool test_basic_merge(void) {
    printf("Test 1: Basic K-Way Merge\n");
    printf("─────────────────────────────────\n");
    
    int arr1[] = {1, 4, 7, 10};
    int arr2[] = {2, 5, 8};
    int arr3[] = {3, 6, 9, 11, 12};
    
    int *arrays[] = {arr1, arr2, arr3};
    int sizes[] = {4, 3, 5};
    int k = 3;
    
    int total = sizes[0] + sizes[1] + sizes[2];
    int *result = malloc(total * sizeof(int));
    int result_size;
    
    printf("Input arrays:\n");
    print_array(arr1, sizes[0], "  Array 1");
    print_array(arr2, sizes[1], "  Array 2");
    print_array(arr3, sizes[2], "  Array 3");
    
    bool success = merge_k_sorted(arrays, sizes, k, result, &result_size);
    
    if (!success) {
        printf("  FAILED: merge_k_sorted returned false\n\n");
        free(result);
        return false;
    }
    
    printf("Output:\n");
    print_array(result, result_size, "  Merged");
    
    /* Verify sorted */
    if (!is_sorted(result, result_size)) {
        printf("  FAILED: Result not sorted\n\n");
        free(result);
        return false;
    }
    
    /* Verify count */
    if (result_size != total) {
        printf("  FAILED: Wrong size (expected %d, got %d)\n\n", total, result_size);
        free(result);
        return false;
    }
    
    free(result);
    printf("  PASSED\n\n");
    return true;
}

bool test_single_array(void) {
    printf("Test 2: Single Array (k=1)\n");
    printf("─────────────────────────────────\n");
    
    int arr1[] = {1, 3, 5, 7, 9};
    int *arrays[] = {arr1};
    int sizes[] = {5};
    
    int result[5];
    int result_size;
    
    merge_k_sorted(arrays, sizes, 1, result, &result_size);
    
    print_array(arr1, 5, "  Input");
    print_array(result, result_size, "  Output");
    
    if (result_size != 5 || memcmp(arr1, result, 5 * sizeof(int)) != 0) {
        printf("  FAILED\n\n");
        return false;
    }
    
    printf("  PASSED\n\n");
    return true;
}

bool test_empty_arrays(void) {
    printf("Test 3: Arrays with Empty Inputs\n");
    printf("─────────────────────────────────\n");
    
    int arr1[] = {1, 5, 9};
    int arr2[] = {};  /* Empty */
    int arr3[] = {2, 4};
    int arr4[] = {};  /* Empty */
    
    int *arrays[] = {arr1, arr2, arr3, arr4};
    int sizes[] = {3, 0, 2, 0};
    
    int result[10];
    int result_size;
    
    merge_k_sorted(arrays, sizes, 4, result, &result_size);
    
    printf("  Input: 2 non-empty arrays, 2 empty arrays\n");
    print_array(result, result_size, "  Output");
    
    if (result_size != 5) {
        printf("  FAILED: Expected 5 elements\n\n");
        return false;
    }
    
    if (!is_sorted(result, result_size)) {
        printf("  FAILED: Not sorted\n\n");
        return false;
    }
    
    printf("  PASSED\n\n");
    return true;
}

bool test_all_empty(void) {
    printf("Test 4: All Empty Arrays (k=0)\n");
    printf("─────────────────────────────────\n");
    
    int *arrays[] = {NULL};
    int sizes[] = {0};
    
    int result[10];
    int result_size = -1;
    
    merge_k_sorted(arrays, sizes, 0, result, &result_size);
    
    if (result_size != 0) {
        printf("  FAILED: Expected 0 elements\n\n");
        return false;
    }
    
    printf("  PASSED\n\n");
    return true;
}

bool test_large_k(void) {
    printf("Test 5: Large K (10 arrays)\n");
    printf("─────────────────────────────────\n");
    
    int k = 10;
    int arr_size = 100;
    
    /* Create k sorted arrays */
    int **arrays = malloc(k * sizeof(int *));
    int *sizes = malloc(k * sizeof(int));
    
    for (int i = 0; i < k; i++) {
        arrays[i] = malloc(arr_size * sizeof(int));
        sizes[i] = arr_size;
        
        /* Fill with sorted values: i, k+i, 2k+i, ... */
        for (int j = 0; j < arr_size; j++) {
            arrays[i][j] = i + j * k;
        }
    }
    
    int total = k * arr_size;
    int *result = malloc(total * sizeof(int));
    int result_size;
    
    printf("  Merging %d arrays of %d elements each\n", k, arr_size);
    
    merge_k_sorted(arrays, sizes, k, result, &result_size);
    
    printf("  Result size: %d\n", result_size);
    
    bool sorted = is_sorted(result, result_size);
    printf("  Is sorted: %s\n", sorted ? "Yes" : "No");
    
    /* Verify result is 0, 1, 2, ..., k*arr_size-1 */
    bool correct = true;
    for (int i = 0; i < result_size && correct; i++) {
        if (result[i] != i) correct = false;
    }
    printf("  Contains 0 to %d: %s\n", total - 1, correct ? "Yes" : "No");
    
    /* Cleanup */
    for (int i = 0; i < k; i++) {
        free(arrays[i]);
    }
    free(arrays);
    free(sizes);
    free(result);
    
    if (!sorted || !correct || result_size != total) {
        printf("  FAILED\n\n");
        return false;
    }
    
    printf("  PASSED\n\n");
    return true;
}

bool test_duplicates(void) {
    printf("Test 6: Arrays with Duplicates\n");
    printf("─────────────────────────────────\n");
    
    int arr1[] = {1, 1, 3, 5, 5};
    int arr2[] = {1, 2, 4, 5, 6};
    int arr3[] = {2, 3, 3, 5, 7};
    
    int *arrays[] = {arr1, arr2, arr3};
    int sizes[] = {5, 5, 5};
    
    int result[15];
    int result_size;
    
    merge_k_sorted(arrays, sizes, 3, result, &result_size);
    
    print_array(result, result_size, "  Merged");
    
    if (!is_sorted(result, result_size)) {
        printf("  FAILED: Not sorted\n\n");
        return false;
    }
    
    if (result_size != 15) {
        printf("  FAILED: Expected 15 elements\n\n");
        return false;
    }
    
    printf("  PASSED\n\n");
    return true;
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     HOMEWORK 2: K-Way Merge Using Min-Heap                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int passed = 0;
    int total = 6;
    
    if (test_basic_merge()) passed++;
    if (test_single_array()) passed++;
    if (test_empty_arrays()) passed++;
    if (test_all_empty()) passed++;
    if (test_large_k()) passed++;
    if (test_duplicates()) passed++;
    
    printf("════════════════════════════════════════════════════════════════\n");
    printf("Results: %d/%d tests passed\n", passed, total);
    
    if (passed == total) {
        printf("\n✓ All tests passed! K-Way merge implementation is correct.\n");
    } else {
        printf("\n✗ Some tests failed.\n");
    }
    
    printf("\n");
    printf("Complexity Analysis:\n");
    printf("  Time:  O(N log K) - each of N elements touches heap once\n");
    printf("  Space: O(K) - heap stores at most K nodes\n");
    printf("\n");
    
    return (passed == total) ? 0 : 1;
}
