/**
 * =============================================================================
 * EXERCISE 1 SOLUTION: Array-Based Stack with Dynamic Resizing
 * =============================================================================
 *
 * This is the complete solution for Exercise 1.
 * FOR INSTRUCTOR USE ONLY - Do not distribute to students.
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* =============================================================================
 * CONSTANTS
 * =============================================================================
 */

#define INITIAL_CAPACITY 4
#define GROWTH_FACTOR 2

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * SOLUTION TODO 1: ArrayStack structure
 */
typedef struct {
    int *data;      /* Pointer to dynamic array */
    int top;        /* Index of top element (-1 if empty) */
    int capacity;   /* Current maximum capacity */
} ArrayStack;

/* =============================================================================
 * FUNCTION IMPLEMENTATIONS
 * =============================================================================
 */

/**
 * SOLUTION TODO 2: stack_create
 */
ArrayStack* stack_create(int initial_capacity) {
    /* Step 1: Handle default capacity */
    if (initial_capacity <= 0) {
        initial_capacity = INITIAL_CAPACITY;
    }
    
    /* Step 2-3: Allocate structure */
    ArrayStack *stack = malloc(sizeof(ArrayStack));
    if (!stack) {
        fprintf(stderr, "Error: Failed to allocate stack structure\n");
        return NULL;
    }
    
    /* Step 4-5: Allocate data array */
    stack->data = malloc(initial_capacity * sizeof(int));
    if (!stack->data) {
        fprintf(stderr, "Error: Failed to allocate stack data array\n");
        free(stack);
        return NULL;
    }
    
    /* Step 6-7: Initialise fields */
    stack->top = -1;
    stack->capacity = initial_capacity;
    
    /* Step 8: Return the stack */
    return stack;
}

/**
 * SOLUTION TODO 3: stack_destroy
 */
void stack_destroy(ArrayStack *stack) {
    if (!stack) return;
    
    free(stack->data);
    free(stack);
}

/**
 * SOLUTION TODO 4: stack_is_empty
 */
bool stack_is_empty(const ArrayStack *stack) {
    return stack->top == -1;
}

/**
 * SOLUTION TODO 5: stack_is_full
 */
bool stack_is_full(const ArrayStack *stack) {
    return stack->top == stack->capacity - 1;
}

/**
 * SOLUTION TODO 6: stack_size
 */
int stack_size(const ArrayStack *stack) {
    return stack->top + 1;
}

/**
 * SOLUTION TODO 7: stack_push with dynamic resizing
 */
bool stack_push(ArrayStack *stack, int value) {
    /* Defensive: if the stack is already full before pushing, grow first. */
    if (stack_is_full(stack)) {
        int new_capacity = stack->capacity * GROWTH_FACTOR;
        int *new_data = realloc(stack->data, (size_t)new_capacity * sizeof(int));
        if (!new_data) {
            fprintf(stderr, "Error: Failed to resize stack\n");
            return false;
        }
        printf("Stack resized: %d -> %d\n", stack->capacity, new_capacity);
        stack->data = new_data;
        stack->capacity = new_capacity;
    }

    /* Push the value. */
    stack->top++;
    stack->data[stack->top] = value;

    /* Grow immediately when this push fills the last available slot. */
    if (stack_is_full(stack)) {
        int new_capacity = stack->capacity * GROWTH_FACTOR;
        int *new_data = realloc(stack->data, (size_t)new_capacity * sizeof(int));
        if (!new_data) {
            fprintf(stderr, "Error: Failed to resize stack\n");
            return false;
        }
        printf("Stack resized: %d -> %d\n", stack->capacity, new_capacity);
        stack->data = new_data;
        stack->capacity = new_capacity;
    }

    return true;
}


/**
 * SOLUTION TODO 8: stack_pop
 */
int stack_pop(ArrayStack *stack) {
    if (stack_is_empty(stack)) {
        fprintf(stderr, "Error: Stack underflow - cannot pop from empty stack\n");
        exit(EXIT_FAILURE);
    }
    
    return stack->data[stack->top--];
}

/**
 * SOLUTION TODO 9: stack_peek
 */
int stack_peek(const ArrayStack *stack) {
    if (stack_is_empty(stack)) {
        fprintf(stderr, "Error: Cannot peek empty stack\n");
        exit(EXIT_FAILURE);
    }
    
    return stack->data[stack->top];
}

/**
 * Print stack contents for debugging
 */
void stack_print(const ArrayStack *stack) {
    printf("Stack [size=%d, cap=%d]: ", stack_size(stack), stack->capacity);
    if (stack_is_empty(stack)) {
        printf("(empty)");
    } else {
        printf("[ ");
        for (int i = 0; i <= stack->top; i++) {
            printf("%d ", stack->data[i]);
        }
        printf("] <- top");
    }
    printf("\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 1: Array-Based Stack with Dynamic Resizing       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Create a stack with initial capacity 4 */
    printf("Creating stack with initial capacity %d...\n", INITIAL_CAPACITY);
    ArrayStack *stack = stack_create(INITIAL_CAPACITY);
    
    if (!stack) {
        fprintf(stderr, "Failed to create stack!\n");
        return EXIT_FAILURE;
    }
    
    printf("Stack created successfully.\n\n");
    
    /* Push 20 values to trigger multiple resizes */
    printf("Pushing values 1 through 20:\n");
    for (int i = 1; i <= 20; i++) {
        printf("  push(%d) ", i);
        if (!stack_push(stack, i)) {
            fprintf(stderr, "Push failed!\n");
            stack_destroy(stack);
            return EXIT_FAILURE;
        }
        /* Print stack state every 5 pushes */
        if (i % 5 == 0) {
            stack_print(stack);
        }
    }
    
    printf("\nFinal state:\n");
    stack_print(stack);
    printf("Size: %d, Capacity: %d\n", stack_size(stack), stack->capacity);
    printf("Top element (peek): %d\n", stack_peek(stack));
    printf("Is empty: %s\n", stack_is_empty(stack) ? "Yes" : "No");
    
    /* Pop all values */
    printf("\nPopping all values: ");
    while (!stack_is_empty(stack)) {
        printf("%d ", stack_pop(stack));
    }
    printf("\n");
    
    printf("\nAfter popping all:\n");
    printf("Is empty: %s\n", stack_is_empty(stack) ? "Yes" : "No");
    printf("Size: %d\n", stack_size(stack));
    
    /* Clean up */
    stack_destroy(stack);
    printf("\nStack destroyed. Memory freed.\n");
    
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("  Exercise complete! Verify with: make valgrind\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    return EXIT_SUCCESS;
}
