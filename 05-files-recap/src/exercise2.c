/**
 * =============================================================================
 * EXERCISE 2: Balanced Brackets Validator (Linked Stack)
 * =============================================================================
 *
 * OBJECTIVE:
 *   Implement a programme that validates whether expressions have properly
 *   balanced brackets using a linked-list-based stack. This exercise
 *   reinforces understanding of dynamic memory allocation with linked
 *   structures and a classic stack application.
 *
 * REQUIREMENTS:
 *   1. Implement a linked-list-based stack (Node and LinkedStack structures)
 *   2. Support three bracket types: (), [], {}
 *   3. Read expressions from standard input (one per line)
 *   4. Output "VALID" or "INVALID: [reason]" for each expression
 *   5. Report the position of mismatched or unclosed brackets
 *   6. Free all memory after processing each expression
 *
 * EXAMPLE INPUT:
 *   {[()]}
 *   {[(])}
 *   ((())
 *   int main() { return 0; }
 *   END
 *
 * EXPECTED OUTPUT:
 *   Expression: {[()]}
 *   Result: VALID
 *
 *   Expression: {[(])}
 *   Result: INVALID - Mismatched bracket at position 3: expected ']', found ')'
 *
 *   Expression: ((())
 *   Result: INVALID - Unclosed bracket '(' at position 0
 *
 *   Expression: int main() { return 0; }
 *   Result: VALID
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 * USAGE: ./exercise2 < input.txt   OR   ./exercise2 (then type expressions)
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
 * TODO 1: Define the Node structure for the linked stack
 *
 * Each node should contain:
 *   - A character value (the bracket)
 *   - An integer storing the position of this bracket in the expression
 *   - A pointer to the next node in the stack
 *
 * Hint: Use 'struct Node *next' for the self-referential pointer
 */
typedef struct Node {
    /* YOUR CODE HERE */
    char bracket;       /* The opening bracket character */
    int position;       /* Position in the expression (0-indexed) */
    struct Node *next;  /* Pointer to next node */
} Node;

/**
 * TODO 2: Define the LinkedStack structure
 *
 * The stack should contain:
 *   - A pointer to the top node (NULL if empty)
 *   - An integer tracking the current size
 */
typedef struct {
    /* YOUR CODE HERE */
    Node *top;    /* Pointer to top node */
    int size;     /* Current number of elements */
} LinkedStack;

/* =============================================================================
 * FUNCTION PROTOTYPES
 * =============================================================================
 */

LinkedStack* stack_create(void);
void         stack_destroy(LinkedStack *stack);
bool         stack_push(LinkedStack *stack, char bracket, int position);
bool         stack_pop(LinkedStack *stack, char *bracket, int *position);
bool         stack_peek(const LinkedStack *stack, char *bracket, int *position);
bool         stack_is_empty(const LinkedStack *stack);
int          stack_size(const LinkedStack *stack);

bool         is_opening_bracket(char c);
bool         is_closing_bracket(char c);
char         get_matching_open(char closing);
char         get_matching_close(char opening);

void         validate_expression(const char *expression);

/* =============================================================================
 * STACK IMPLEMENTATION
 * =============================================================================
 */

/**
 * TODO 3: Implement stack_create
 *
 * Creates and initialises an empty linked stack.
 *
 * @return Pointer to new stack, or NULL on failure
 *
 * Steps:
 *   1. Allocate memory for LinkedStack structure
 *   2. Check allocation; return NULL if failed
 *   3. Initialise top to NULL
 *   4. Initialise size to 0
 *   5. Return the stack pointer
 */
LinkedStack* stack_create(void) {
    /* YOUR CODE HERE */
    
    LinkedStack *stack = NULL;  /* TODO: Allocate with malloc */
    
    /* TODO: Check allocation */
    
    /* TODO: Initialise fields */
    
    return stack;
}

/**
 * TODO 4: Implement stack_destroy
 *
 * Frees all nodes and the stack structure.
 *
 * @param stack Stack to destroy (may be NULL)
 *
 * Steps:
 *   1. Check if stack is NULL; return if so
 *   2. Traverse the linked list, freeing each node
 *   3. Free the stack structure
 *
 * Hint: Save the next pointer before freeing current node
 */
void stack_destroy(LinkedStack *stack) {
    /* YOUR CODE HERE */
    
    if (!stack) return;
    
    /* TODO: Free all nodes */
    Node *current = stack->top;
    while (current != NULL) {
        /* TODO: Save next, free current, move to next */
    }
    
    /* TODO: Free the stack structure */
}

/**
 * TODO 5: Implement stack_is_empty
 *
 * @param stack Stack to check
 * @return true if empty (top == NULL), false otherwise
 */
bool stack_is_empty(const LinkedStack *stack) {
    /* YOUR CODE HERE */
    return true;  /* TODO: Replace with correct check */
}

/**
 * TODO 6: Implement stack_size
 *
 * @param stack Stack to query
 * @return Number of elements in stack
 */
int stack_size(const LinkedStack *stack) {
    /* YOUR CODE HERE */
    return 0;  /* TODO: Replace with correct value */
}

/**
 * TODO 7: Implement stack_push
 *
 * Pushes a bracket and its position onto the stack.
 *
 * @param stack Stack to push onto
 * @param bracket The bracket character
 * @param position Position in the expression
 * @return true on success, false on allocation failure
 *
 * Steps:
 *   1. Allocate a new Node
 *   2. Check allocation; return false if failed
 *   3. Set node's bracket and position
 *   4. Set node's next to current top
 *   5. Update stack's top to new node
 *   6. Increment size
 *   7. Return true
 */
bool stack_push(LinkedStack *stack, char bracket, int position) {
    /* YOUR CODE HERE */
    
    /* Step 1-2: Allocate node */
    Node *new_node = NULL;  /* TODO: Allocate with malloc */
    
    /* TODO: Check allocation */
    
    /* Step 3-6: Initialise and link */
    /* TODO: Set bracket */
    /* TODO: Set position */
    /* TODO: Link to current top */
    /* TODO: Update top */
    /* TODO: Increment size */
    
    return true;
}

/**
 * TODO 8: Implement stack_pop
 *
 * Removes the top element and returns its values through pointers.
 *
 * @param stack Stack to pop from
 * @param bracket Pointer to store the bracket (may be NULL)
 * @param position Pointer to store the position (may be NULL)
 * @return true if successful, false if stack was empty
 *
 * Steps:
 *   1. Check if stack is empty; return false if so
 *   2. Save pointer to top node
 *   3. If bracket pointer is not NULL, store the bracket
 *   4. If position pointer is not NULL, store the position
 *   5. Update top to next node
 *   6. Decrement size
 *   7. Free the old top node
 *   8. Return true
 */
bool stack_pop(LinkedStack *stack, char *bracket, int *position) {
    /* YOUR CODE HERE */
    
    /* Step 1: Check empty */
    if (stack_is_empty(stack)) {
        return false;
    }
    
    /* Step 2: Save top node */
    Node *old_top = stack->top;
    
    /* Step 3-4: Copy values if pointers provided */
    /* TODO: Copy bracket if bracket != NULL */
    /* TODO: Copy position if position != NULL */
    
    /* Step 5-7: Update stack and free node */
    /* TODO: Update top */
    /* TODO: Decrement size */
    /* TODO: Free old_top */
    
    return true;
}

/**
 * TODO 9: Implement stack_peek
 *
 * Returns the top element without removing it.
 *
 * @param stack Stack to peek
 * @param bracket Pointer to store the bracket (may be NULL)
 * @param position Pointer to store the position (may be NULL)
 * @return true if successful, false if stack was empty
 */
bool stack_peek(const LinkedStack *stack, char *bracket, int *position) {
    /* YOUR CODE HERE */
    
    if (stack_is_empty(stack)) {
        return false;
    }
    
    /* TODO: Copy values without removing */
    
    return true;
}

/* =============================================================================
 * BRACKET HELPER FUNCTIONS
 * =============================================================================
 */

/**
 * TODO 10: Implement bracket helper functions
 */

/**
 * Check if character is an opening bracket
 */
bool is_opening_bracket(char c) {
    /* YOUR CODE HERE */
    return (c == '(' || c == '[' || c == '{');
}

/**
 * Check if character is a closing bracket
 */
bool is_closing_bracket(char c) {
    /* YOUR CODE HERE */
    return false;  /* TODO: Check for ), ], } */
}

/**
 * Get the matching opening bracket for a closing bracket
 */
char get_matching_open(char closing) {
    switch (closing) {
        case ')': return '(';
        case ']': return '[';
        case '}': return '{';
        default:  return '\0';
    }
}

/**
 * Get the matching closing bracket for an opening bracket
 */
char get_matching_close(char opening) {
    /* YOUR CODE HERE */
    switch (opening) {
        /* TODO: Return matching close bracket */
        default: return '\0';
    }
}

/* =============================================================================
 * MAIN VALIDATION LOGIC
 * =============================================================================
 */

/**
 * TODO 11: Implement the bracket validation algorithm
 *
 * Algorithm:
 *   1. Create an empty stack
 *   2. For each character in the expression:
 *      a. If it's an opening bracket, push it with its position
 *      b. If it's a closing bracket:
 *         - If stack is empty: error (unmatched closing bracket)
 *         - Pop the top and check if it matches
 *         - If no match: error (mismatched brackets)
 *      c. If it's any other character, ignore it
 *   3. After processing all characters:
 *      - If stack is not empty: error (unclosed brackets)
 *      - Otherwise: valid
 *   4. Destroy the stack
 *
 * @param expression The expression to validate
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
    
    /* TODO: Implement the validation loop */
    for (int i = 0; i < length && valid; i++) {
        char c = expression[i];
        
        if (is_opening_bracket(c)) {
            /* TODO: Push opening bracket with position */
            /* YOUR CODE HERE */
        }
        else if (is_closing_bracket(c)) {
            /* TODO: Check for matching opening bracket */
            
            if (stack_is_empty(stack)) {
                /* Unmatched closing bracket */
                printf("Result: INVALID - Unmatched closing bracket '%c' at position %d\n\n",
                       c, i);
                valid = false;
            }
            else {
                char top_bracket;
                int top_position;
                
                /* TODO: Pop and check for match */
                stack_pop(stack, &top_bracket, &top_position);
                
                /* TODO: Compare top_bracket with expected match */
                char expected = get_matching_open(c);
                if (top_bracket != expected) {
                    printf("Result: INVALID - Mismatched bracket at position %d: "
                           "expected '%c', found '%c'\n\n",
                           i, get_matching_close(top_bracket), c);
                    valid = false;
                }
            }
        }
        /* Ignore non-bracket characters */
    }
    
    /* TODO 12: Check for unclosed brackets after loop */
    if (valid && !stack_is_empty(stack)) {
        /* YOUR CODE HERE */
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
    
    /* Clean up */
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

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Add support for angle brackets < > used in C++ templates
 *
 * 2. Implement a "fix suggestions" feature that shows where to add missing
 *    brackets to make the expression valid.
 *
 * 3. Add support for string literals - ignore brackets inside "quoted strings"
 *
 * 4. Count and report the nesting depth of the deepest bracket level.
 *
 * 5. Highlight the mismatched brackets by printing the expression with
 *    markers (^) under the problematic positions.
 *
 * =============================================================================
 */
