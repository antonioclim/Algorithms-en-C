/**
 * =============================================================================
 * WEEK 10: HEAPS AND PRIORITY QUEUES
 * Complete Working Example
 * =============================================================================
 *
 * This example demonstrates:
 *   1. Binary heap structure and array representation
 *   2. Sift-up and sift-down operations
 *   3. Insert and extract operations
 *   4. Floyd's O(n) build-heap algorithm
 *   5. Heapsort implementation
 *   6. Generic priority queue with function pointers
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o example1 example1.c
 * Usage: ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* =============================================================================
 * SECTION 1: CONSTANTS AND MACROS
 * =============================================================================
 */

#define INITIAL_CAPACITY 16

/* Navigation macros for 0-indexed array */
#define PARENT(i)       (((i) - 1) / 2)
#define LEFT_CHILD(i)   (2 * (i) + 1)
#define RIGHT_CHILD(i)  (2 * (i) + 2)

/* Utility macros */
#define SWAP(a, b) do { int temp = (a); (a) = (b); (b) = temp; } while(0)

/* =============================================================================
 * SECTION 2: INTEGER HEAP STRUCTURE
 * =============================================================================
 */

/**
 * Simple integer max-heap structure
 */
typedef struct {
    int *data;          /* Dynamic array of integers */
    size_t size;        /* Current number of elements */
    size_t capacity;    /* Allocated capacity */
} IntHeap;

/**
 * Initialise an empty heap
 */
IntHeap *intheap_create(size_t initial_capacity) {
    IntHeap *h = malloc(sizeof(IntHeap));
    if (!h) return NULL;
    
    h->data = malloc(initial_capacity * sizeof(int));
    if (!h->data) {
        free(h);
        return NULL;
    }
    
    h->size = 0;
    h->capacity = initial_capacity;
    return h;
}

/**
 * Free all heap memory
 */
void intheap_destroy(IntHeap *h) {
    if (h) {
        free(h->data);
        free(h);
    }
}

/**
 * Resize the heap's internal array
 */
static bool intheap_resize(IntHeap *h, size_t new_capacity) {
    int *new_data = realloc(h->data, new_capacity * sizeof(int));
    if (!new_data) return false;
    
    h->data = new_data;
    h->capacity = new_capacity;
    return true;
}

/* =============================================================================
 * SECTION 3: SIFT OPERATIONS
 * =============================================================================
 */

/**
 * Sift-up: Move element at index i upward to restore heap property
 * Used after insertion at the end
 */
static void sift_up(int *arr, size_t i) {
    while (i > 0) {
        size_t parent = PARENT(i);
        
        /* If current element is not greater than parent, heap is valid */
        if (arr[i] <= arr[parent])
            break;
        
        /* Swap with parent and continue upward */
        SWAP(arr[i], arr[parent]);
        i = parent;
    }
}

/**
 * Sift-down: Move element at index i downward to restore heap property
 * Used after extraction (moving last element to root)
 */
static void sift_down(int *arr, size_t n, size_t i) {
    while (true) {
        size_t largest = i;
        size_t left = LEFT_CHILD(i);
        size_t right = RIGHT_CHILD(i);
        
        /* Find the largest among node and its children */
        if (left < n && arr[left] > arr[largest])
            largest = left;
        
        if (right < n && arr[right] > arr[largest])
            largest = right;
        
        /* If node is already the largest, heap property is satisfied */
        if (largest == i)
            break;
        
        /* Swap with largest child and continue downward */
        SWAP(arr[i], arr[largest]);
        i = largest;
    }
}

/* =============================================================================
 * SECTION 4: HEAP OPERATIONS
 * =============================================================================
 */

/**
 * Insert a value into the max-heap
 * Time complexity: O(log n)
 */
bool intheap_insert(IntHeap *h, int value) {
    /* Resize if at capacity */
    if (h->size >= h->capacity) {
        if (!intheap_resize(h, h->capacity * 2)) {
            fprintf(stderr, "Error: Failed to resize heap\n");
            return false;
        }
    }
    
    /* Add element at the end */
    h->data[h->size] = value;
    h->size++;
    
    /* Restore heap property by sifting up */
    sift_up(h->data, h->size - 1);
    
    return true;
}

/**
 * Extract the maximum value from the heap
 * Time complexity: O(log n)
 */
bool intheap_extract_max(IntHeap *h, int *value) {
    if (h->size == 0) {
        fprintf(stderr, "Error: Cannot extract from empty heap\n");
        return false;
    }
    
    /* Save the maximum (root) */
    *value = h->data[0];
    
    /* Move last element to root */
    h->data[0] = h->data[h->size - 1];
    h->size--;
    
    /* Restore heap property by sifting down */
    if (h->size > 0)
        sift_down(h->data, h->size, 0);
    
    return true;
}

/**
 * Peek at the maximum value without removing it
 * Time complexity: O(1)
 */
bool intheap_peek(const IntHeap *h, int *value) {
    if (h->size == 0)
        return false;
    
    *value = h->data[0];
    return true;
}

/**
 * Check if heap is empty
 */
bool intheap_is_empty(const IntHeap *h) {
    return h->size == 0;
}

/**
 * Get heap size
 */
size_t intheap_size(const IntHeap *h) {
    return h->size;
}

/* =============================================================================
 * SECTION 5: BUILD HEAP AND HEAPSORT
 * =============================================================================
 */

/**
 * Build a max-heap from an arbitrary array (Floyd's algorithm)
 * Time complexity: O(n) - NOT O(n log n)!
 */
void build_max_heap(int *arr, size_t n) {
    /* Start from the last internal node (parent of the last element) */
    /* All nodes from n/2 to n-1 are leaves and are trivially valid heaps */
    for (int i = (int)(n / 2) - 1; i >= 0; i--) {
        sift_down(arr, n, (size_t)i);
    }
}

/**
 * Heapsort: Sort an array in ascending order
 * Time complexity: O(n log n)
 * Space complexity: O(1) - in-place
 */
void heapsort(int *arr, size_t n) {
    /* Phase 1: Build max-heap - O(n) */
    build_max_heap(arr, n);
    
    /* Phase 2: Extract elements one by one - O(n log n) */
    for (size_t i = n - 1; i > 0; i--) {
        /* Move current maximum (root) to the end */
        SWAP(arr[0], arr[i]);
        
        /* Restore heap property on the reduced heap */
        sift_down(arr, i, 0);
    }
}

/* =============================================================================
 * SECTION 6: GENERIC PRIORITY QUEUE
 * =============================================================================
 */

/**
 * Comparator function type
 * Returns: positive if a > b, negative if a < b, zero if equal
 */
typedef int (*PQComparator)(const void *a, const void *b);

/**
 * Generic priority queue structure
 */
typedef struct {
    void **data;            /* Array of void pointers to elements */
    size_t size;            /* Current number of elements */
    size_t capacity;        /* Allocated capacity */
    size_t elem_size;       /* Size of each element */
    PQComparator compare;   /* Comparison function */
} PriorityQueue;

/**
 * Create a new priority queue
 */
PriorityQueue *pq_create(size_t initial_capacity, size_t elem_size, 
                         PQComparator compare) {
    PriorityQueue *pq = malloc(sizeof(PriorityQueue));
    if (!pq) return NULL;
    
    pq->data = malloc(initial_capacity * sizeof(void *));
    if (!pq->data) {
        free(pq);
        return NULL;
    }
    
    /* Initialise all pointers to NULL */
    for (size_t i = 0; i < initial_capacity; i++) {
        pq->data[i] = NULL;
    }
    
    pq->size = 0;
    pq->capacity = initial_capacity;
    pq->elem_size = elem_size;
    pq->compare = compare;
    
    return pq;
}

/**
 * Destroy the priority queue and free all memory
 */
void pq_destroy(PriorityQueue *pq) {
    if (!pq) return;
    
    /* Free all stored elements */
    for (size_t i = 0; i < pq->size; i++) {
        free(pq->data[i]);
    }
    
    free(pq->data);
    free(pq);
}

/**
 * Generic sift-up for priority queue
 */
static void pq_sift_up(PriorityQueue *pq, size_t i) {
    while (i > 0) {
        size_t parent = PARENT(i);
        
        if (pq->compare(pq->data[i], pq->data[parent]) <= 0)
            break;
        
        /* Swap pointers */
        void *temp = pq->data[i];
        pq->data[i] = pq->data[parent];
        pq->data[parent] = temp;
        
        i = parent;
    }
}

/**
 * Generic sift-down for priority queue
 */
static void pq_sift_down(PriorityQueue *pq, size_t i) {
    while (true) {
        size_t largest = i;
        size_t left = LEFT_CHILD(i);
        size_t right = RIGHT_CHILD(i);
        
        if (left < pq->size && 
            pq->compare(pq->data[left], pq->data[largest]) > 0)
            largest = left;
        
        if (right < pq->size && 
            pq->compare(pq->data[right], pq->data[largest]) > 0)
            largest = right;
        
        if (largest == i)
            break;
        
        void *temp = pq->data[i];
        pq->data[i] = pq->data[largest];
        pq->data[largest] = temp;
        
        i = largest;
    }
}

/**
 * Insert an element into the priority queue
 */
bool pq_insert(PriorityQueue *pq, const void *element) {
    /* Resize if needed */
    if (pq->size >= pq->capacity) {
        size_t new_capacity = pq->capacity * 2;
        void **new_data = realloc(pq->data, new_capacity * sizeof(void *));
        if (!new_data) return false;
        
        pq->data = new_data;
        pq->capacity = new_capacity;
    }
    
    /* Allocate and copy the element */
    void *new_elem = malloc(pq->elem_size);
    if (!new_elem) return false;
    
    memcpy(new_elem, element, pq->elem_size);
    
    /* Add at end and sift up */
    pq->data[pq->size] = new_elem;
    pq->size++;
    
    pq_sift_up(pq, pq->size - 1);
    
    return true;
}

/**
 * Extract the highest-priority element
 */
bool pq_extract(PriorityQueue *pq, void *element) {
    if (pq->size == 0) return false;
    
    /* Copy the root element */
    memcpy(element, pq->data[0], pq->elem_size);
    
    /* Free the root */
    free(pq->data[0]);
    
    /* Move last to root */
    pq->data[0] = pq->data[pq->size - 1];
    pq->data[pq->size - 1] = NULL;
    pq->size--;
    
    /* Sift down */
    if (pq->size > 0)
        pq_sift_down(pq, 0);
    
    return true;
}

/**
 * Peek at the highest-priority element
 */
bool pq_peek(const PriorityQueue *pq, void *element) {
    if (pq->size == 0) return false;
    
    memcpy(element, pq->data[0], pq->elem_size);
    return true;
}

/**
 * Get priority queue size
 */
size_t pq_size(const PriorityQueue *pq) {
    return pq->size;
}

/**
 * Check if priority queue is empty
 */
bool pq_is_empty(const PriorityQueue *pq) {
    return pq->size == 0;
}

/* =============================================================================
 * SECTION 7: EXAMPLE STRUCTURES AND COMPARATORS
 * =============================================================================
 */

/**
 * Task structure for priority queue demonstration
 */
typedef struct {
    int priority;
    int id;
    char description[64];
} Task;

/**
 * Comparator for tasks (by priority)
 */
int task_compare(const void *a, const void *b) {
    const Task *ta = (const Task *)a;
    const Task *tb = (const Task *)b;
    return ta->priority - tb->priority;
}

/* =============================================================================
 * SECTION 8: UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Print an array
 */
void print_array(const int *arr, size_t n, const char *label) {
    printf("%s: [", label);
    for (size_t i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

/**
 * Print heap as a tree structure
 */
void print_heap_tree(const int *arr, size_t n) {
    if (n == 0) {
        printf("  (empty heap)\n");
        return;
    }
    
    /* Calculate the height */
    size_t height = 0;
    size_t temp = n;
    while (temp > 0) {
        height++;
        temp /= 2;
    }
    
    /* Print level by level */
    size_t idx = 0;
    for (size_t level = 0; level < height && idx < n; level++) {
        size_t nodes_in_level = 1 << level;  /* 2^level */
        size_t spacing = (1 << (height - level)) - 1;
        
        /* Leading spaces */
        for (size_t s = 0; s < spacing; s++) printf("  ");
        
        /* Print nodes at this level */
        for (size_t i = 0; i < nodes_in_level && idx < n; i++, idx++) {
            printf("%2d", arr[idx]);
            /* Spacing between nodes */
            for (size_t s = 0; s < 2 * spacing + 1; s++) printf("  ");
        }
        printf("\n");
    }
}

/**
 * Verify max-heap property
 */
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

/* =============================================================================
 * DEMONSTRATION FUNCTIONS
 * =============================================================================
 */

void demo_part1_array_representation(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 1: Array Representation of Heaps                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int heap[] = {90, 85, 70, 50, 60, 65, 40};
    size_t n = sizeof(heap) / sizeof(heap[0]);
    
    printf("A max-heap stored in an array:\n\n");
    print_array(heap, n, "Array");
    printf("\nTree representation:\n");
    print_heap_tree(heap, n);
    
    printf("\nIndex navigation (0-indexed):\n");
    printf("  • Parent of index i:      (i - 1) / 2\n");
    printf("  • Left child of index i:  2*i + 1\n");
    printf("  • Right child of index i: 2*i + 2\n\n");
    
    printf("Examples:\n");
    for (size_t i = 0; i < n; i++) {
        printf("  Node[%zu] = %d", i, heap[i]);
        if (i > 0)
            printf(" | Parent[%zu] = %d", PARENT(i), heap[PARENT(i)]);
        if (LEFT_CHILD(i) < n)
            printf(" | Left[%zu] = %d", LEFT_CHILD(i), heap[LEFT_CHILD(i)]);
        if (RIGHT_CHILD(i) < n)
            printf(" | Right[%zu] = %d", RIGHT_CHILD(i), heap[RIGHT_CHILD(i)]);
        printf("\n");
    }
    
    printf("\nMax-heap property verified: %s\n", 
           verify_max_heap(heap, n) ? "✓ VALID" : "✗ INVALID");
}

void demo_part2_basic_operations(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: Basic Heap Operations                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    IntHeap *h = intheap_create(INITIAL_CAPACITY);
    if (!h) {
        fprintf(stderr, "Failed to create heap\n");
        return;
    }
    
    printf("Creating empty heap and inserting elements:\n\n");
    
    int values[] = {45, 20, 14, 12, 31, 7, 11, 13, 7};
    size_t n = sizeof(values) / sizeof(values[0]);
    
    for (size_t i = 0; i < n; i++) {
        printf("  Inserting %d...\n", values[i]);
        intheap_insert(h, values[i]);
        printf("  Heap after insert: ");
        print_array(h->data, h->size, "");
    }
    
    printf("\nFinal heap structure:\n");
    print_heap_tree(h->data, h->size);
    printf("Max-heap property: %s\n", 
           verify_max_heap(h->data, h->size) ? "✓ VALID" : "✗ INVALID");
    
    printf("\n--- Extracting elements in order ---\n\n");
    int value;
    while (!intheap_is_empty(h)) {
        intheap_peek(h, &value);
        printf("  Peek: %d, ", value);
        intheap_extract_max(h, &value);
        printf("Extracted: %d, Remaining size: %zu\n", value, intheap_size(h));
    }
    
    printf("\nElements extracted in descending order (max-heap property).\n");
    
    intheap_destroy(h);
}

void demo_part3_build_heap(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 3: Floyd's Build-Heap Algorithm                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int arr[] = {4, 1, 3, 2, 16, 9, 10, 14, 8, 7};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    printf("Converting arbitrary array to max-heap:\n\n");
    print_array(arr, n, "Before");
    printf("\nIs valid max-heap: %s\n", 
           verify_max_heap(arr, n) ? "Yes" : "No");
    
    printf("\n--- Applying Floyd's build_max_heap (O(n)) ---\n\n");
    
    /* Demonstrate step by step */
    printf("Starting from index %zu (last internal node):\n", n/2 - 1);
    for (int i = (int)(n / 2) - 1; i >= 0; i--) {
        printf("\n  Heapifying index %d (value %d):\n", i, arr[i]);
        sift_down(arr, n, (size_t)i);
        printf("    ");
        print_array(arr, n, "Array");
    }
    
    printf("\n");
    print_array(arr, n, "After");
    printf("\nTree representation:\n");
    print_heap_tree(arr, n);
    printf("Is valid max-heap: %s\n", 
           verify_max_heap(arr, n) ? "✓ Yes" : "✗ No");
    
    printf("\nNote: Floyd's algorithm runs in O(n) time, not O(n log n)!\n");
    printf("This is because most nodes are near the bottom where sift distances are small.\n");
}

void demo_part4_heapsort(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 4: Heapsort Algorithm                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 5};
    size_t n = sizeof(arr) / sizeof(arr[0]);
    
    printf("Sorting array using heapsort:\n\n");
    print_array(arr, n, "Unsorted");
    
    printf("\n--- Phase 1: Build max-heap ---\n");
    build_max_heap(arr, n);
    print_array(arr, n, "Max-heap");
    
    printf("\n--- Phase 2: Extract and place at end ---\n\n");
    
    /* Manual heapsort with trace */
    for (size_t i = n - 1; i > 0; i--) {
        printf("  Swap arr[0]=%d with arr[%zu]=%d\n", arr[0], i, arr[i]);
        SWAP(arr[0], arr[i]);
        printf("  After swap: ");
        print_array(arr, n, "");
        
        sift_down(arr, i, 0);
        printf("  After sift: ");
        print_array(arr, n, "");
        printf("  [heap: 0..%zu | sorted: %zu..%zu]\n\n", i-1, i, n-1);
    }
    
    print_array(arr, n, "Sorted");
    
    printf("\nHeapsort properties:\n");
    printf("  • Time complexity: O(n log n) worst case\n");
    printf("  • Space complexity: O(1) - in-place\n");
    printf("  • Not stable (relative order of equal elements may change)\n");
}

void demo_part5_generic_priority_queue(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 5: Generic Priority Queue                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Creating priority queue for Task scheduling:\n\n");
    
    PriorityQueue *pq = pq_create(10, sizeof(Task), task_compare);
    if (!pq) {
        fprintf(stderr, "Failed to create priority queue\n");
        return;
    }
    
    /* Create some tasks */
    Task tasks[] = {
        {3, 101, "Check emails"},
        {5, 102, "Attend meeting"},
        {1, 103, "Water plants"},
        {4, 104, "Review code"},
        {2, 105, "Lunch break"},
        {5, 106, "Submit report"}
    };
    size_t n = sizeof(tasks) / sizeof(tasks[0]);
    
    printf("Adding tasks to priority queue:\n\n");
    for (size_t i = 0; i < n; i++) {
        printf("  + [Priority %d] Task %d: %s\n", 
               tasks[i].priority, tasks[i].id, tasks[i].description);
        pq_insert(pq, &tasks[i]);
    }
    
    printf("\nQueue size: %zu\n", pq_size(pq));
    
    printf("\n--- Processing tasks by priority ---\n\n");
    Task current;
    int order = 1;
    while (!pq_is_empty(pq)) {
        pq_extract(pq, &current);
        printf("  %d. [Priority %d] Task %d: %s\n", 
               order++, current.priority, current.id, current.description);
    }
    
    printf("\nTasks processed in priority order (highest first).\n");
    
    pq_destroy(pq);
}

void demo_part6_applications(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 6: Practical Applications                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Application 1: Finding k largest elements */
    printf("Application 1: Finding top-k largest elements\n");
    printf("─────────────────────────────────────────────\n\n");
    
    int stream[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9};
    size_t stream_n = sizeof(stream) / sizeof(stream[0]);
    int k = 5;
    
    print_array(stream, stream_n, "Data stream");
    printf("Finding top %d elements using a min-heap of size %d:\n\n", k, k);
    
    /* Use a simple array for demonstration */
    int top_k[5] = {0};
    size_t heap_size = 0;
    
    for (size_t i = 0; i < stream_n; i++) {
        if (heap_size < (size_t)k) {
            /* Add to heap */
            top_k[heap_size] = stream[i];
            heap_size++;
            
            /* Maintain min-heap property (simplified) */
            for (int j = (int)heap_size - 1; j > 0; j--) {
                int parent = (j - 1) / 2;
                if (top_k[j] < top_k[parent]) {
                    SWAP(top_k[j], top_k[parent]);
                }
            }
        } else if (stream[i] > top_k[0]) {
            /* Replace minimum */
            top_k[0] = stream[i];
            
            /* Sift down in min-heap */
            size_t idx = 0;
            while (true) {
                size_t smallest = idx;
                size_t left = 2 * idx + 1;
                size_t right = 2 * idx + 2;
                
                if (left < heap_size && top_k[left] < top_k[smallest])
                    smallest = left;
                if (right < heap_size && top_k[right] < top_k[smallest])
                    smallest = right;
                
                if (smallest == idx) break;
                
                SWAP(top_k[idx], top_k[smallest]);
                idx = smallest;
            }
        }
    }
    
    print_array(top_k, k, "Top 5 elements (min-heap)");
    
    /* Sort for display */
    heapsort(top_k, k);
    print_array(top_k, k, "Sorted top 5");
    
    /* Application 2: Running median concept */
    printf("\n\nApplication 2: Running Median Concept\n");
    printf("─────────────────────────────────────────────\n\n");
    
    printf("To track the median of a stream:\n");
    printf("  1. Maintain two heaps:\n");
    printf("     • Max-heap for the lower half\n");
    printf("     • Min-heap for the upper half\n");
    printf("  2. Balance sizes after each insertion\n");
    printf("  3. Median is either:\n");
    printf("     • Root of the larger heap (odd count)\n");
    printf("     • Average of both roots (even count)\n");
    printf("\nTime: O(log n) per insert, O(1) to query median\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     WEEK 10: HEAPS AND PRIORITY QUEUES                        ║\n");
    printf("║     Complete Working Example                                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    demo_part1_array_representation();
    demo_part2_basic_operations();
    demo_part3_build_heap();
    demo_part4_heapsort();
    demo_part5_generic_priority_queue();
    demo_part6_applications();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     End of Demonstration                                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
