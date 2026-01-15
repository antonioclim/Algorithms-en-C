/**
 * =============================================================================
 * EXERCISE 2 SOLUTION: Balanced Brackets Validator (Linked Stack)
 * =============================================================================
 *
 * This is the complete solution for Exercise 2.
 * FOR INSTRUCTOR USE ONLY - Do not distribute to students.
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise2_sol exercise2_sol.c
 * Usage: ./exercise2_sol < input.txt
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 1024

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * SOLUTION TODO 1: Node structure
 */
typedef struct Node {
    char bracket;
    int position;
    struct Node *next;
} Node;

/**
 * SOLUTION TODO 2: LinkedStack structure
 */
typedef struct {
    Node *top;
    int size;
} LinkedStack;

/* =============================================================================
 * STACK IMPLEMENTATION
 * =============================================================================
 */

/**
 * SOLUTION TODO 3: stack_create
 */
LinkedStack* stack_create(void) {
    LinkedStack *stack = malloc(sizeof(LinkedStack));
    if (!stack) {
        fprintf(stderr, "Error: Failed to allocate stack\n");
        return NULL;
    }
    
    stack->top = NULL;
    stack->size = 0;
    
    return stack;
}

/**
 * SOLUTION TODO 4: stack_destroy
 */
void stack_destroy(LinkedStack *stack) {
    if (!stack) return;
    
    Node *current = stack->top;
    while (current != NULL) {
        Node *next = current->next;
        free(current);
        current = next;
    }
    
    free(stack);
}

/**
 * SOLUTION TODO 5: stack_is_empty
 */
bool stack_is_empty(const LinkedStack *stack) {
    return stack->top == NULL;
}

/**
 * SOLUTION TODO 6: stack_size
 */
int stack_size(const LinkedStack *stack) {
    return stack->size;
}

/**
 * SOLUTION TODO 7: stack_push
 */
bool stack_push(LinkedStack *stack, char bracket, int position) {
    Node *new_node = malloc(sizeof(Node));
    if (!new_node) {
        fprintf(stderr, "Error: Failed to allocate node\n");
        return false;
    }
    
    new_node->bracket = bracket;
    new_node->position = position;
    new_node->next = stack->top;
    
    stack->top = new_node;
    stack->size++;
    
    return true;
}

/**
 * SOLUTION TODO 8: stack_pop
 */
bool stack_pop(LinkedStack *stack, char *bracket, int *position) {
    if (stack_is_empty(stack)) {
        return false;
    }
    
    Node *old_top = stack->top;
    
    if (bracket) *bracket = old_top->bracket;
    if (position) *position = old_top->position;
    
    stack->top = old_top->next;
    stack->size--;
    
    free(old_top);
    
    return true;
}

/**
 * SOLUTION TODO 9: stack_peek
 */
bool stack_peek(const LinkedStack *stack, char *bracket, int *position) {
    if (stack_is_empty(stack)) {
        return false;
    }
    
    if (bracket) *bracket = stack->top->bracket;
    if (position) *position = stack->top->position;
    
    return true;
}

/* =============================================================================
 * BRACKET HELPER FUNCTIONS
 * =============================================================================
 */

/**
 * SOLUTION TODO 10: bracket helpers
 */
bool is_opening_bracket(char c) {
    return (c == '(' || c == '[' || c == '{');
}

bool is_closing_bracket(char c) {
    return (c == ')' || c == ']' || c == '}');
}

char get_matching_open(char closing) {
    switch (closing) {
        case ')': return '(';
        case ']': return '[';
        case '}': return '{';
        default:  return '\0';
    }
}

char get_matching_close(char opening) {
    switch (opening) {
        case '(': return ')';
        case '[': return ']';
        case '{': return '}';
        default:  return '\0';
    }
}

/* =============================================================================
 * MAIN VALIDATION LOGIC
 * =============================================================================
 */

/**
 * SOLUTION TODO 11 & 12: validate_expression
 */
void validate_expression(const char *expression) {
    printf("Expression: %s\n", expression);
    
    LinkedStack *stack = stack_create();
    if (!stack) {
        printf("Result: ERROR - Failed to create stack\n\n");
        return;
    }
    
    bool valid = true;
    int length = strlen(expression);
    
    for (int i = 0; i < length && valid; i++) {
        char c = expression[i];
        
        if (is_opening_bracket(c)) {
            stack_push(stack, c, i);
        }
        else if (is_closing_bracket(c)) {
            if (stack_is_empty(stack)) {
                printf("Result: INVALID - Unmatched closing bracket '%c' at position %d\n\n",
                       c, i);
                valid = false;
            }
            else {
                char top_bracket;
                int top_position;
                
                stack_pop(stack, &top_bracket, &top_position);
                
                char expected = get_matching_open(c);
                if (top_bracket != expected) {
                    printf("Result: INVALID - Mismatched bracket at position %d: "
                           "expected '%c', found '%c'\n\n",
                           i, get_matching_close(top_bracket), c);
                    valid = false;
                }
            }
        }
    }
    
    /* Check for unclosed brackets */
    if (valid && !stack_is_empty(stack)) {
        char unclosed;
        int position;
        stack_peek(stack, &unclosed, &position);
        printf("Result: INVALID - Unclosed bracket '%c' at position %d\n\n",
               unclosed, position);
        valid = false;
    }
    
    if (valid) {
        printf("Result: VALID\n\n");
    }
    
    stack_destroy(stack);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 2: Balanced Brackets Validator                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Enter expressions to validate (one per line).\n");
    printf("Type 'END' or press Ctrl+D to finish.\n\n");
    printf("───────────────────────────────────────────────────────────────\n\n");
    
    char line[MAX_LINE_LENGTH];
    
    while (fgets(line, sizeof(line), stdin) != NULL) {
        /* Remove trailing newline */
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        
        /* Check for termination */
        if (strcmp(line, "END") == 0) {
            break;
        }
        
        /* Skip empty lines */
        if (strlen(line) == 0) {
            continue;
        }
        
        /* Validate the expression */
        validate_expression(line);
    }
    
    printf("───────────────────────────────────────────────────────────────\n");
    printf("Validation complete. Verify memory with: make valgrind\n\n");
    
    return EXIT_SUCCESS;
}
