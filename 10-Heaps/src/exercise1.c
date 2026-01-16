/**
 * =============================================================================
 * EXERCISE 1 SOLUTION: Generic Priority Queue Implementation
 * =============================================================================
 *
 * INSTRUCTOR SOLUTION - Complete implementation with all TODOs filled
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c
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
#define SHRINK_THRESHOLD 4

/* TODO 1 SOLUTION: Index navigation macros */
#define PARENT(i)       (((i) - 1) / 2)
#define LEFT_CHILD(i)   (2 * (i) + 1)
#define RIGHT_CHILD(i)  (2 * (i) + 2)


/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef int (*PQComparator)(const void *a, const void *b);

/* TODO 2 SOLUTION: Complete PriorityQueue structure */
typedef struct {
    void **data;           /* Array of void pointers */
    size_t size;           /* Current number of elements */
    size_t capacity;       /* Allocated capacity */
    size_t elem_size;      /* Size of each element in bytes */
    PQComparator compare;  /* Comparator function */
} PriorityQueue;


/* =============================================================================
 * HELPER FUNCTIONS
 * =============================================================================
 */

/* TODO 3 SOLUTION: Sift-up operation */
static void sift_up(PriorityQueue *pq, size_t i) {
    while (i > 0) {
        size_t parent = PARENT(i);
        
        /* If heap property satisfied, stop */
        if (pq->compare(pq->data[i], pq->data[parent]) <= 0)
            break;
        
        /* Swap pointers with parent */
        void *temp = pq->data[i];
        pq->data[i] = pq->data[parent];
        pq->data[parent] = temp;
        
        i = parent;
    }
}

/* TODO 4 SOLUTION: Sift-down operation */
static void sift_down(PriorityQueue *pq, size_t i) {
    while (true) {
        size_t largest = i;
        size_t left = LEFT_CHILD(i);
        size_t right = RIGHT_CHILD(i);
        
        /* Find largest among node and children */
        if (left < pq->size && 
            pq->compare(pq->data[left], pq->data[largest]) > 0)
            largest = left;
        
        if (right < pq->size && 
            pq->compare(pq->data[right], pq->data[largest]) > 0)
            largest = right;
        
        /* If node is largest, heap property satisfied */
        if (largest == i)
            break;
        
        /* Swap with largest child */
        void *temp = pq->data[i];
        pq->data[i] = pq->data[largest];
        pq->data[largest] = temp;
        
        i = largest;
    }
}

/* TODO 5 SOLUTION: Resize function */
static bool pq_resize(PriorityQueue *pq, size_t new_capacity) {
    void **new_data = realloc(pq->data, new_capacity * sizeof(void *));
    if (!new_data)
        return false;
    
    pq->data = new_data;
    pq->capacity = new_capacity;
    return true;
}


/* =============================================================================
 * PRIORITY QUEUE API
 * =============================================================================
 */

/* TODO 6 SOLUTION: pq_create */
PriorityQueue *pq_create(size_t initial_capacity, size_t elem_size, 
                         PQComparator compare) {
    if (initial_capacity == 0)
        initial_capacity = INITIAL_CAPACITY;
    
    PriorityQueue *pq = malloc(sizeof(PriorityQueue));
    if (!pq)
        return NULL;
    
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

/* TODO 7 SOLUTION: pq_destroy */
void pq_destroy(PriorityQueue *pq) {
    if (!pq)
        return;
    
    /* Free each stored element */
    for (size_t i = 0; i < pq->size; i++) {
        free(pq->data[i]);
    }
    
    /* Free the array and structure */
    free(pq->data);
    free(pq);
}

/* TODO 8 SOLUTION: pq_insert */
bool pq_insert(PriorityQueue *pq, const void *element) {
    if (!pq || !element)
        return false;
    
    /* Resize if needed */
    if (pq->size >= pq->capacity) {
        if (!pq_resize(pq, pq->capacity * GROWTH_FACTOR))
            return false;
    }
    
    /* Allocate memory for new element */
    void *new_elem = malloc(pq->elem_size);
    if (!new_elem)
        return false;
    
    /* Copy element data */
    memcpy(new_elem, element, pq->elem_size);
    
    /* Add at end */
    pq->data[pq->size] = new_elem;
    pq->size++;
    
    /* Restore heap property */
    sift_up(pq, pq->size - 1);
    
    return true;
}

/* TODO 9 SOLUTION: pq_extract */
bool pq_extract(PriorityQueue *pq, void *element) {
    if (!pq || pq->size == 0)
        return false;
    
    /* Copy root element to output */
    memcpy(element, pq->data[0], pq->elem_size);
    
    /* Free root element */
    free(pq->data[0]);
    
    /* Move last element to root */
    pq->data[0] = pq->data[pq->size - 1];
    pq->data[pq->size - 1] = NULL;
    pq->size--;
    
    /* Restore heap property */
    if (pq->size > 0)
        sift_down(pq, 0);
    
    return true;
}

/* TODO 10 SOLUTION: pq_peek */
bool pq_peek(const PriorityQueue *pq, void *element) {
    if (!pq || pq->size == 0)
        return false;
    
    memcpy(element, pq->data[0], pq->elem_size);
    return true;
}

size_t pq_size(const PriorityQueue *pq) {
    return pq ? pq->size : 0;
}

bool pq_is_empty(const PriorityQueue *pq) {
    return pq == NULL || pq->size == 0;
}


/* =============================================================================
 * COMPARATOR FUNCTIONS
 * =============================================================================
 */

/* TODO 11 SOLUTION: Max-heap comparator */
int int_compare_max(const void *a, const void *b) {
    return *(const int *)a - *(const int *)b;
}

/* TODO 12 SOLUTION: Min-heap comparator */
int int_compare_min(const void *a, const void *b) {
    return *(const int *)b - *(const int *)a;
}


/* =============================================================================
 * TEST FUNCTIONS
 * =============================================================================
 */

bool test_basic_operations(void) {
    printf("Test 1: Basic operations (max-heap)\n");
    printf("────────────────────────────────────\n");
    
    PriorityQueue *pq = pq_create(INITIAL_CAPACITY, sizeof(int), int_compare_max);
    if (!pq) {
        printf("  FAILED: Could not create priority queue\n");
        return false;
    }
    
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
    
    if (pq_size(pq) != n) {
        printf("  FAILED: Size mismatch\n");
        pq_destroy(pq);
        return false;
    }
    
    printf("  Extracting: ");
    int prev = 100;
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
    
    if (!pq_is_empty(pq)) {
        printf("  FAILED: Queue should be empty\n");
        pq_destroy(pq);
        return false;
    }
    
    pq_destroy(pq);
    printf("  PASSED\n\n");
    return true;
}

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

bool test_resizing(void) {
    printf("Test 3: Dynamic resizing\n");
    printf("────────────────────────────────────\n");
    
    PriorityQueue *pq = pq_create(4, sizeof(int), int_compare_max);
    if (!pq) {
        printf("  FAILED: Could not create priority queue\n");
        return false;
    }
    
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

bool test_peek(void) {
    printf("Test 4: Peek operation\n");
    printf("────────────────────────────────────\n");
    
    PriorityQueue *pq = pq_create(INITIAL_CAPACITY, sizeof(int), int_compare_max);
    
    int val;
    if (pq_peek(pq, &val)) {
        printf("  FAILED: Peek on empty should return false\n");
        pq_destroy(pq);
        return false;
    }
    
    int x = 42;
    pq_insert(pq, &x);
    
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
