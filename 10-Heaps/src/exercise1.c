/**
 * =============================================================================
 * EXERCISE 1: Generic Priority Queue Implementation
 * =============================================================================
 *
 * OBJECTIVE:
 *   Implement a complete generic priority queue ADT using a binary heap.
 *   The priority queue should support arbitrary element types through void*
 *   pointers and custom comparator functions.
 *
 * REQUIREMENTS:
 *   1. Dynamic array storage with automatic resizing
 *   2. Support for both max-heap and min-heap via comparator
 *   3. Complete API: create, destroy, insert, extract, peek, size, is_empty
 *   4. Memory safety: no leaks, no buffer overflows
 *   5. Handle malloc failures gracefully
 *
 * EXAMPLE INPUT:
 *   Insert integers: 5, 3, 8, 1, 9, 4
 *   Extract all (max-heap): 9, 8, 5, 4, 3, 1
 *
 * EXPECTED OUTPUT:
 *   Priority Queue Operations Test
 *   Inserting: 5 3 8 1 9 4
 *   Extracting in priority order: 9 8 5 4 3 1
 *   All tests passed!
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* =============================================================================
 * CONSTANTS AND MACROS
 * =============================================================================
 */

#define INITIAL_CAPACITY 8
#define GROWTH_FACTOR 2
#define SHRINK_THRESHOLD 4  /* Shrink when size < capacity / SHRINK_THRESHOLD */

/**
 * TODO 1: Define the index navigation macros
 *
 * For a 0-indexed array representation of a complete binary tree:
 *   - Parent of node at index i
 *   - Left child of node at index i
 *   - Right child of node at index i
 *
 * Hint: 
 *   - Parent formula uses integer division
 *   - Child formulas involve multiplication by 2
 */

/* YOUR CODE HERE */
#define PARENT(i)       (0)  /* Replace with correct formula */
#define LEFT_CHILD(i)   (0)  /* Replace with correct formula */
#define RIGHT_CHILD(i)  (0)  /* Replace with correct formula */


/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * Comparator function type
 * Returns: positive if a > b, negative if a < b, zero if equal
 */
typedef int (*PQComparator)(const void *a, const void *b);

/**
 * TODO 2: Complete the PriorityQueue structure definition
 *
 * The structure should contain:
 *   - Array of void pointers to store elements
 *   - Current size (number of elements)
 *   - Allocated capacity
 *   - Size of each element in bytes
 *   - Comparator function pointer
 *
 * Hint: Think about what information you need to:
 *   - Navigate the heap
 *   - Know when to resize
 *   - Copy elements of arbitrary types
 *   - Compare elements
 */
typedef struct {
    /* YOUR CODE HERE */
    void **data;           /* Array of void pointers */
    size_t size;           /* Current number of elements */
    /* Add remaining fields */
    
} PriorityQueue;


/* =============================================================================
 * HELPER FUNCTIONS
 * =============================================================================
 */

/**
 * TODO 3: Implement the sift_up operation
 *
 * Move element at index i upward to restore heap property.
 * Used after inserting a new element at the end.
 *
 * @param pq    Pointer to priority queue
 * @param i     Index of element to sift up
 *
 * Steps:
 *   1. While not at root (i > 0)
 *   2. Compare current element with parent
 *   3. If heap property satisfied (parent >= current for max-heap), break
 *   4. Otherwise, swap with parent and continue upward
 *
 * Hint: Use pq->compare(pq->data[i], pq->data[parent]) to compare
 *       Swap pointers, not the data they point to
 */
static void sift_up(PriorityQueue *pq, size_t i) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 4: Implement the sift_down operation
 *
 * Move element at index i downward to restore heap property.
 * Used after extracting root and moving last element to root.
 *
 * @param pq    Pointer to priority queue
 * @param i     Index of element to sift down
 *
 * Steps:
 *   1. Loop:
 *      a. Find largest among node and its children
 *      b. If node is largest, heap property satisfied, break
 *      c. Swap with largest child
 *      d. Continue from the swapped position
 *
 * Hint: Check bounds before accessing children
 *       left < pq->size before comparing pq->data[left]
 */
static void sift_down(PriorityQueue *pq, size_t i) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 5: Implement resize function
 *
 * Resize the internal array to new_capacity.
 *
 * @param pq            Pointer to priority queue
 * @param new_capacity  New capacity for the array
 * @return              true on success, false on failure
 *
 * Steps:
 *   1. Use realloc to resize the data array
 *   2. Handle realloc failure (return false, don't lose original data)
 *   3. Update capacity field
 *
 * Hint: realloc returns NULL on failure but doesn't free original memory
 */
static bool pq_resize(PriorityQueue *pq, size_t new_capacity) {
    /* YOUR CODE HERE */
    
    return false;  /* Replace this */
}


/* =============================================================================
 * PRIORITY QUEUE API
 * =============================================================================
 */

/**
 * TODO 6: Implement pq_create
 *
 * Create and initialise a new priority queue.
 *
 * @param initial_capacity  Initial array capacity
 * @param elem_size         Size of each element in bytes
 * @param compare           Comparator function
 * @return                  Pointer to new priority queue, or NULL on failure
 *
 * Steps:
 *   1. Allocate PriorityQueue structure
 *   2. Allocate array of void pointers (initial_capacity)
 *   3. Initialise all fields
 *   4. Handle allocation failures (free already allocated memory)
 */
PriorityQueue *pq_create(size_t initial_capacity, size_t elem_size, 
                         PQComparator compare) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace this */
}

/**
 * TODO 7: Implement pq_destroy
 *
 * Free all memory associated with the priority queue.
 *
 * @param pq    Pointer to priority queue
 *
 * Steps:
 *   1. Check for NULL input
 *   2. Free each stored element (pq->data[i] for i < pq->size)
 *   3. Free the data array itself
 *   4. Free the PriorityQueue structure
 *
 * Hint: Remember that each element was malloc'd separately during insert
 */
void pq_destroy(PriorityQueue *pq) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 8: Implement pq_insert
 *
 * Insert a new element into the priority queue.
 *
 * @param pq        Pointer to priority queue
 * @param element   Pointer to element to insert
 * @return          true on success, false on failure
 *
 * Steps:
 *   1. Check if resize needed (size >= capacity)
 *   2. Allocate memory for new element (pq->elem_size bytes)
 *   3. Copy element data to new memory (use memcpy)
 *   4. Add pointer to end of array
 *   5. Increment size
 *   6. Sift up to restore heap property
 *
 * Hint: Handle both resize failure and malloc failure
 */
bool pq_insert(PriorityQueue *pq, const void *element) {
    /* YOUR CODE HERE */
    
    return false;  /* Replace this */
}

/**
 * TODO 9: Implement pq_extract
 *
 * Extract the highest-priority element from the queue.
 *
 * @param pq        Pointer to priority queue
 * @param element   Pointer to store extracted element
 * @return          true on success, false if queue is empty
 *
 * Steps:
 *   1. Check for empty queue
 *   2. Copy root element to output (use memcpy)
 *   3. Free the root element's memory
 *   4. Move last element to root position
 *   5. Decrement size
 *   6. Sift down to restore heap property (if size > 0)
 *
 * Hint: Set pq->data[pq->size] = NULL after moving to prevent double-free
 */
bool pq_extract(PriorityQueue *pq, void *element) {
    /* YOUR CODE HERE */
    
    return false;  /* Replace this */
}

/**
 * TODO 10: Implement pq_peek
 *
 * Get the highest-priority element without removing it.
 *
 * @param pq        Pointer to priority queue
 * @param element   Pointer to store peeked element
 * @return          true on success, false if queue is empty
 */
bool pq_peek(const PriorityQueue *pq, void *element) {
    /* YOUR CODE HERE */
    
    return false;  /* Replace this */
}

/**
 * Get the current size of the priority queue
 */
size_t pq_size(const PriorityQueue *pq) {
    return pq ? pq->size : 0;
}

/**
 * Check if the priority queue is empty
 */
bool pq_is_empty(const PriorityQueue *pq) {
    return pq == NULL || pq->size == 0;
}


/* =============================================================================
 * COMPARATOR FUNCTIONS
 * =============================================================================
 */

/**
 * TODO 11: Implement integer comparator for max-heap
 *
 * Compare two integers for max-heap ordering.
 * Return positive if a > b, negative if a < b, zero if equal.
 */
int int_compare_max(const void *a, const void *b) {
    /* YOUR CODE HERE */
    
    return 0;  /* Replace this */
}

/**
 * TODO 12: Implement integer comparator for min-heap
 *
 * Compare two integers for min-heap ordering.
 * Hint: Simply reverse the comparison from max-heap
 */
int int_compare_min(const void *a, const void *b) {
    /* YOUR CODE HERE */
    
    return 0;  /* Replace this */
}


/* =============================================================================
 * TEST FUNCTIONS
 * =============================================================================
 */

/**
 * Test basic operations
 */
bool test_basic_operations(void) {
    printf("Test 1: Basic operations (max-heap)\n");
    printf("────────────────────────────────────\n");
    
    PriorityQueue *pq = pq_create(INITIAL_CAPACITY, sizeof(int), int_compare_max);
    if (!pq) {
        printf("  FAILED: Could not create priority queue\n");
        return false;
    }
    
    /* Insert elements */
    int values[] = {5, 3, 8, 1, 9, 4, 7, 2, 6};
    size_t n = sizeof(values) / sizeof(values[0]);
    
    printf("  Inserting: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", values[i]);
        if (!pq_insert(pq, &values[i])) {
            printf("\n  FAILED: Insert failed\n");
            pq_destroy(pq);
            return false;
        }
    }
    printf("\n");
    
    /* Check size */
    if (pq_size(pq) != n) {
        printf("  FAILED: Size mismatch (expected %zu, got %zu)\n", n, pq_size(pq));
        pq_destroy(pq);
        return false;
    }
    
    /* Extract elements - should be in descending order */
    printf("  Extracting: ");
    int prev = 100;  /* Larger than any value */
    for (size_t i = 0; i < n; i++) {
        int val;
        if (!pq_extract(pq, &val)) {
            printf("\n  FAILED: Extract failed\n");
            pq_destroy(pq);
            return false;
        }
        printf("%d ", val);
        
        if (val > prev) {
            printf("\n  FAILED: Not in descending order\n");
            pq_destroy(pq);
            return false;
        }
        prev = val;
    }
    printf("\n");
    
    /* Should be empty now */
    if (!pq_is_empty(pq)) {
        printf("  FAILED: Queue should be empty\n");
        pq_destroy(pq);
        return false;
    }
    
    pq_destroy(pq);
    printf("  PASSED\n\n");
    return true;
}

/**
 * Test min-heap operations
 */
bool test_min_heap(void) {
    printf("Test 2: Min-heap operations\n");
    printf("────────────────────────────────────\n");
    
    PriorityQueue *pq = pq_create(INITIAL_CAPACITY, sizeof(int), int_compare_min);
    if (!pq) {
        printf("  FAILED: Could not create priority queue\n");
        return false;
    }
    
    int values[] = {5, 3, 8, 1, 9, 4};
    size_t n = sizeof(values) / sizeof(values[0]);
    
    printf("  Inserting: ");
    for (size_t i = 0; i < n; i++) {
        printf("%d ", values[i]);
        pq_insert(pq, &values[i]);
    }
    printf("\n");
    
    /* Extract - should be in ascending order */
    printf("  Extracting: ");
    int prev = -1;
    for (size_t i = 0; i < n; i++) {
        int val;
        pq_extract(pq, &val);
        printf("%d ", val);
        
        if (val < prev) {
            printf("\n  FAILED: Not in ascending order\n");
            pq_destroy(pq);
            return false;
        }
        prev = val;
    }
    printf("\n");
    
    pq_destroy(pq);
    printf("  PASSED\n\n");
    return true;
}

/**
 * Test dynamic resizing
 */
bool test_resizing(void) {
    printf("Test 3: Dynamic resizing\n");
    printf("────────────────────────────────────\n");
    
    PriorityQueue *pq = pq_create(4, sizeof(int), int_compare_max);
    if (!pq) {
        printf("  FAILED: Could not create priority queue\n");
        return false;
    }
    
    /* Insert more elements than initial capacity */
    printf("  Inserting 20 elements into capacity-4 queue...\n");
    for (int i = 1; i <= 20; i++) {
        if (!pq_insert(pq, &i)) {
            printf("  FAILED: Insert %d failed\n", i);
            pq_destroy(pq);
            return false;
        }
    }
    
    if (pq_size(pq) != 20) {
        printf("  FAILED: Size should be 20\n");
        pq_destroy(pq);
        return false;
    }
    
    /* Verify max element */
    int max;
    pq_peek(pq, &max);
    if (max != 20) {
        printf("  FAILED: Max should be 20, got %d\n", max);
        pq_destroy(pq);
        return false;
    }
    
    pq_destroy(pq);
    printf("  PASSED\n\n");
    return true;
}

/**
 * Test peek operation
 */
bool test_peek(void) {
    printf("Test 4: Peek operation\n");
    printf("────────────────────────────────────\n");
    
    PriorityQueue *pq = pq_create(INITIAL_CAPACITY, sizeof(int), int_compare_max);
    
    /* Peek on empty should fail */
    int val;
    if (pq_peek(pq, &val)) {
        printf("  FAILED: Peek on empty should return false\n");
        pq_destroy(pq);
        return false;
    }
    
    int x = 42;
    pq_insert(pq, &x);
    
    /* Peek should succeed and not remove */
    if (!pq_peek(pq, &val) || val != 42) {
        printf("  FAILED: Peek should return 42\n");
        pq_destroy(pq);
        return false;
    }
    
    if (pq_size(pq) != 1) {
        printf("  FAILED: Peek should not remove element\n");
        pq_destroy(pq);
        return false;
    }
    
    pq_destroy(pq);
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
    printf("║     EXERCISE 1: Generic Priority Queue Implementation         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int passed = 0;
    int total = 4;
    
    if (test_basic_operations()) passed++;
    if (test_min_heap()) passed++;
    if (test_resizing()) passed++;
    if (test_peek()) passed++;
    
    printf("════════════════════════════════════════════════════════════════\n");
    printf("Results: %d/%d tests passed\n", passed, total);
    
    if (passed == total) {
        printf("\n✓ All tests passed! Your priority queue implementation is correct.\n");
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
 * 1. Implement decrease_key operation for updating priorities
 *    (useful for Dijkstra's algorithm)
 *
 * 2. Add a pq_merge function to combine two priority queues
 *
 * 3. Implement a heap_verify function that checks heap property
 *    for debugging purposes
 *
 * 4. Add automatic shrinking when queue becomes too sparse
 *    (size < capacity / 4)
 *
 * 5. Implement a pq_from_array function that builds a heap
 *    from an array in O(n) time using Floyd's algorithm
 *
 * =============================================================================
 */
