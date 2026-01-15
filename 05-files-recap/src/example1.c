/**
 * =============================================================================
 * WEEK 05: STACKS — THE LIFO DATA STRUCTURE
 * Complete Working Example
 * =============================================================================
 *
 * This example demonstrates:
 *   1. Array-based stack implementation with dynamic resizing
 *   2. Linked-list-based stack implementation
 *   3. Postfix expression evaluation
 *   4. Balanced bracket checking
 *   5. Real-world application: Undo/Redo system
 *   6. Generic stack using void pointers
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o example1 example1.c
 * Usage: ./example1
 *
 * =============================================================================
 */

#define _POSIX_C_SOURCE 200809L  /* Enable strdup and other POSIX functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/* =============================================================================
 * PART 1: ARRAY-BASED STACK IMPLEMENTATION
 * =============================================================================
 */

#define INITIAL_CAPACITY 4

/**
 * Array-based stack structure
 * Uses dynamic array that doubles in capacity when full
 */
typedef struct {
    int *data;      /* Pointer to dynamic array of integers */
    int top;        /* Index of top element (-1 if empty) */
    int capacity;   /* Current maximum capacity */
} ArrayStack;

/**
 * Create a new array-based stack
 * @param initial_cap Initial capacity (uses default if <= 0)
 * @return Pointer to new stack, or NULL on failure
 */
ArrayStack* array_stack_create(int initial_cap) {
    if (initial_cap <= 0) {
        initial_cap = INITIAL_CAPACITY;
    }
    
    ArrayStack *stack = malloc(sizeof(ArrayStack));
    if (!stack) {
        fprintf(stderr, "Error: Failed to allocate stack structure\n");
        return NULL;
    }
    
    stack->data = malloc(initial_cap * sizeof(int));
    if (!stack->data) {
        fprintf(stderr, "Error: Failed to allocate stack data array\n");
        free(stack);
        return NULL;
    }
    
    stack->top = -1;
    stack->capacity = initial_cap;
    
    return stack;
}

/**
 * Destroy an array-based stack and free all memory
 * @param stack Stack to destroy
 */
void array_stack_destroy(ArrayStack *stack) {
    if (stack) {
        free(stack->data);
        free(stack);
    }
}

/**
 * Check if stack is empty
 * @param stack Stack to check
 * @return true if empty, false otherwise
 */
bool array_stack_is_empty(const ArrayStack *stack) {
    return stack->top == -1;
}

/**
 * Check if stack is full (at current capacity)
 * @param stack Stack to check
 * @return true if full, false otherwise
 */
bool array_stack_is_full(const ArrayStack *stack) {
    return stack->top == stack->capacity - 1;
}

/**
 * Get current number of elements
 * @param stack Stack to query
 * @return Number of elements in stack
 */
int array_stack_size(const ArrayStack *stack) {
    return stack->top + 1;
}

/**
 * Push a value onto the stack
 * Automatically resizes if stack is full (doubles capacity)
 * @param stack Stack to push onto
 * @param value Value to push
 * @return 0 on success, -1 on failure
 */
int array_stack_push(ArrayStack *stack, int value) {
    /* Check if resize is needed */
    if (array_stack_is_full(stack)) {
        int new_capacity = stack->capacity * 2;
        int *new_data = realloc(stack->data, new_capacity * sizeof(int));
        
        if (!new_data) {
            fprintf(stderr, "Error: Failed to resize stack\n");
            return -1;
        }
        
        printf("    [Resized: %d -> %d]\n", stack->capacity, new_capacity);
        stack->data = new_data;
        stack->capacity = new_capacity;
    }
    
    /* Push the value */
    stack->top++;
    stack->data[stack->top] = value;
    
    return 0;
}

/**
 * Pop a value from the stack
 * @param stack Stack to pop from
 * @return The popped value
 * @note Exits program if stack is empty (for simplicity)
 */
int array_stack_pop(ArrayStack *stack) {
    if (array_stack_is_empty(stack)) {
        fprintf(stderr, "Error: Stack underflow - cannot pop from empty stack\n");
        exit(EXIT_FAILURE);
    }
    
    return stack->data[stack->top--];
}

/**
 * Peek at the top value without removing it
 * @param stack Stack to peek
 * @return The top value
 * @note Exits program if stack is empty
 */
int array_stack_peek(const ArrayStack *stack) {
    if (array_stack_is_empty(stack)) {
        fprintf(stderr, "Error: Cannot peek empty stack\n");
        exit(EXIT_FAILURE);
    }
    
    return stack->data[stack->top];
}

/**
 * Print stack contents for debugging
 * @param stack Stack to print
 */
void array_stack_print(const ArrayStack *stack) {
    printf("Stack [size=%d, capacity=%d]: ", 
           array_stack_size(stack), stack->capacity);
    
    if (array_stack_is_empty(stack)) {
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
 * PART 2: LINKED-LIST-BASED STACK IMPLEMENTATION
 * =============================================================================
 */

/**
 * Node structure for linked stack
 */
typedef struct Node {
    int data;
    struct Node *next;
} Node;

/**
 * Linked-list-based stack structure
 */
typedef struct {
    Node *top;   /* Pointer to top node (NULL if empty) */
    int size;    /* Current number of elements */
} LinkedStack;

/**
 * Create a new linked stack
 * @return Pointer to new stack, or NULL on failure
 */
LinkedStack* linked_stack_create(void) {
    LinkedStack *stack = malloc(sizeof(LinkedStack));
    if (!stack) {
        fprintf(stderr, "Error: Failed to allocate linked stack\n");
        return NULL;
    }
    
    stack->top = NULL;
    stack->size = 0;
    
    return stack;
}

/**
 * Destroy a linked stack and free all nodes
 * @param stack Stack to destroy
 */
void linked_stack_destroy(LinkedStack *stack) {
    if (!stack) return;
    
    /* Free all nodes */
    Node *current = stack->top;
    while (current) {
        Node *next = current->next;
        free(current);
        current = next;
    }
    
    free(stack);
}

/**
 * Check if linked stack is empty
 */
bool linked_stack_is_empty(const LinkedStack *stack) {
    return stack->top == NULL;
}

/**
 * Get size of linked stack
 */
int linked_stack_size(const LinkedStack *stack) {
    return stack->size;
}

/**
 * Push a value onto the linked stack
 * @param stack Stack to push onto
 * @param value Value to push
 * @return 0 on success, -1 on failure
 */
int linked_stack_push(LinkedStack *stack, int value) {
    Node *new_node = malloc(sizeof(Node));
    if (!new_node) {
        fprintf(stderr, "Error: Failed to allocate node\n");
        return -1;
    }
    
    new_node->data = value;
    new_node->next = stack->top;
    stack->top = new_node;
    stack->size++;
    
    return 0;
}

/**
 * Pop a value from the linked stack
 */
int linked_stack_pop(LinkedStack *stack) {
    if (linked_stack_is_empty(stack)) {
        fprintf(stderr, "Error: Stack underflow\n");
        exit(EXIT_FAILURE);
    }
    
    Node *old_top = stack->top;
    int value = old_top->data;
    
    stack->top = old_top->next;
    stack->size--;
    
    free(old_top);
    
    return value;
}

/**
 * Peek at the top of the linked stack
 */
int linked_stack_peek(const LinkedStack *stack) {
    if (linked_stack_is_empty(stack)) {
        fprintf(stderr, "Error: Cannot peek empty stack\n");
        exit(EXIT_FAILURE);
    }
    
    return stack->top->data;
}

/**
 * Print linked stack contents
 */
void linked_stack_print(const LinkedStack *stack) {
    printf("LinkedStack [size=%d]: ", stack->size);
    
    if (linked_stack_is_empty(stack)) {
        printf("(empty)");
    } else {
        printf("top -> ");
        Node *current = stack->top;
        while (current) {
            printf("[%d]", current->data);
            if (current->next) printf(" -> ");
            current = current->next;
        }
    }
    printf("\n");
}

/* =============================================================================
 * PART 3: POSTFIX EXPRESSION EVALUATION
 * =============================================================================
 */

/**
 * Evaluate a postfix expression
 * Supports single-digit operands and +, -, *, / operators
 * @param expression Postfix expression string (space-separated tokens)
 * @return Result of evaluation
 */
int evaluate_postfix(const char *expression) {
    ArrayStack *stack = array_stack_create(32);
    if (!stack) return 0;
    
    /* Make a copy since strtok modifies the string */
    char *expr_copy = strdup(expression);
    if (!expr_copy) {
        array_stack_destroy(stack);
        return 0;
    }
    
    char *token = strtok(expr_copy, " ");
    
    while (token != NULL) {
        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
            /* Token is a number */
            int num = atoi(token);
            array_stack_push(stack, num);
        } else {
            /* Token is an operator */
            if (array_stack_size(stack) < 2) {
                fprintf(stderr, "Error: Invalid expression\n");
                free(expr_copy);
                array_stack_destroy(stack);
                return 0;
            }
            
            int b = array_stack_pop(stack);
            int a = array_stack_pop(stack);
            int result = 0;
            
            switch (token[0]) {
                case '+': result = a + b; break;
                case '-': result = a - b; break;
                case '*': result = a * b; break;
                case '/': 
                    if (b == 0) {
                        fprintf(stderr, "Error: Division by zero\n");
                        free(expr_copy);
                        array_stack_destroy(stack);
                        return 0;
                    }
                    result = a / b; 
                    break;
                default:
                    fprintf(stderr, "Error: Unknown operator '%c'\n", token[0]);
                    break;
            }
            
            array_stack_push(stack, result);
        }
        
        token = strtok(NULL, " ");
    }
    
    int final_result = array_stack_pop(stack);
    
    free(expr_copy);
    array_stack_destroy(stack);
    
    return final_result;
}

/* =============================================================================
 * PART 4: BALANCED BRACKET CHECKING
 * =============================================================================
 */

/**
 * Check if two brackets match
 */
bool brackets_match(char open, char close) {
    return (open == '(' && close == ')') ||
           (open == '[' && close == ']') ||
           (open == '{' && close == '}');
}

/**
 * Check if an expression has balanced brackets
 * @param expression Expression to check
 * @return true if balanced, false otherwise
 */
bool check_balanced_brackets(const char *expression) {
    LinkedStack *stack = linked_stack_create();
    if (!stack) return false;
    
    for (int i = 0; expression[i] != '\0'; i++) {
        char c = expression[i];
        
        if (c == '(' || c == '[' || c == '{') {
            /* Opening bracket: push onto stack */
            linked_stack_push(stack, c);
        }
        else if (c == ')' || c == ']' || c == '}') {
            /* Closing bracket: check for match */
            if (linked_stack_is_empty(stack)) {
                printf("  Error at position %d: Unmatched '%c'\n", i, c);
                linked_stack_destroy(stack);
                return false;
            }
            
            char top = linked_stack_pop(stack);
            if (!brackets_match(top, c)) {
                printf("  Error at position %d: Expected match for '%c', got '%c'\n", 
                       i, top, c);
                linked_stack_destroy(stack);
                return false;
            }
        }
        /* Ignore other characters */
    }
    
    bool balanced = linked_stack_is_empty(stack);
    
    if (!balanced) {
        printf("  Error: Unclosed bracket(s) remain\n");
    }
    
    linked_stack_destroy(stack);
    return balanced;
}

/* =============================================================================
 * PART 5: UNDO/REDO SYSTEM (Real-World Application)
 * =============================================================================
 */

/**
 * Simple text editor state for demonstration
 */
typedef struct {
    char text[256];
} EditorState;

/**
 * Stack of editor states for undo/redo
 */
typedef struct StateNode {
    EditorState state;
    struct StateNode *next;
} StateNode;

typedef struct {
    StateNode *top;
    int count;
} StateStack;

StateStack* state_stack_create(void) {
    StateStack *stack = malloc(sizeof(StateStack));
    if (stack) {
        stack->top = NULL;
        stack->count = 0;
    }
    return stack;
}

void state_stack_destroy(StateStack *stack) {
    while (stack && stack->top) {
        StateNode *temp = stack->top;
        stack->top = temp->next;
        free(temp);
    }
    free(stack);
}

void state_stack_push(StateStack *stack, const EditorState *state) {
    StateNode *node = malloc(sizeof(StateNode));
    if (node) {
        node->state = *state;
        node->next = stack->top;
        stack->top = node;
        stack->count++;
    }
}

bool state_stack_pop(StateStack *stack, EditorState *state) {
    if (!stack->top) return false;
    
    StateNode *temp = stack->top;
    *state = temp->state;
    stack->top = temp->next;
    stack->count--;
    free(temp);
    return true;
}

bool state_stack_is_empty(const StateStack *stack) {
    return stack->top == NULL;
}

/**
 * Simple text editor with undo/redo
 */
typedef struct {
    EditorState current;
    StateStack *undo_stack;
    StateStack *redo_stack;
} TextEditor;

TextEditor* editor_create(void) {
    TextEditor *editor = malloc(sizeof(TextEditor));
    if (editor) {
        editor->current.text[0] = '\0';
        editor->undo_stack = state_stack_create();
        editor->redo_stack = state_stack_create();
    }
    return editor;
}

void editor_destroy(TextEditor *editor) {
    if (editor) {
        state_stack_destroy(editor->undo_stack);
        state_stack_destroy(editor->redo_stack);
        free(editor);
    }
}

void editor_type(TextEditor *editor, const char *text) {
    /* Save current state for undo */
    state_stack_push(editor->undo_stack, &editor->current);
    
    /* Clear redo stack (new action invalidates redo history) */
    while (!state_stack_is_empty(editor->redo_stack)) {
        EditorState temp;
        state_stack_pop(editor->redo_stack, &temp);
    }
    
    /* Append text */
    strncat(editor->current.text, text, 
            sizeof(editor->current.text) - strlen(editor->current.text) - 1);
}

bool editor_undo(TextEditor *editor) {
    if (state_stack_is_empty(editor->undo_stack)) {
        return false;
    }
    
    /* Save current state for redo */
    state_stack_push(editor->redo_stack, &editor->current);
    
    /* Restore previous state */
    state_stack_pop(editor->undo_stack, &editor->current);
    return true;
}

bool editor_redo(TextEditor *editor) {
    if (state_stack_is_empty(editor->redo_stack)) {
        return false;
    }
    
    /* Save current state for undo */
    state_stack_push(editor->undo_stack, &editor->current);
    
    /* Restore next state */
    state_stack_pop(editor->redo_stack, &editor->current);
    return true;
}

/* =============================================================================
 * DEMONSTRATION FUNCTIONS
 * =============================================================================
 */

void demo_part1_array_stack(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 1: Array-Based Stack Demonstration                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    ArrayStack *stack = array_stack_create(4);
    if (!stack) return;
    
    printf("Created stack with initial capacity 4\n\n");
    
    /* Push values to trigger resize */
    printf("Pushing values 10, 20, 30, 40, 50 (will trigger resize):\n");
    for (int i = 1; i <= 5; i++) {
        int value = i * 10;
        printf("  push(%d): ", value);
        array_stack_push(stack, value);
        array_stack_print(stack);
    }
    
    printf("\nPeek: %d\n", array_stack_peek(stack));
    printf("Size: %d\n\n", array_stack_size(stack));
    
    /* Pop all values */
    printf("Popping all values:\n");
    while (!array_stack_is_empty(stack)) {
        printf("  pop() = %d, ", array_stack_pop(stack));
        array_stack_print(stack);
    }
    
    array_stack_destroy(stack);
    printf("\nStack destroyed. Memory freed.\n");
}

void demo_part2_linked_stack(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: Linked-List Stack Demonstration                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    LinkedStack *stack = linked_stack_create();
    if (!stack) return;
    
    printf("Pushing values A=65, B=66, C=67 (as ASCII):\n");
    for (int i = 0; i < 3; i++) {
        int value = 'A' + i;
        printf("  push(%d='%c'): ", value, value);
        linked_stack_push(stack, value);
        linked_stack_print(stack);
    }
    
    printf("\nPeek: %d ('%c')\n", linked_stack_peek(stack), linked_stack_peek(stack));
    
    printf("\nPopping values:\n");
    while (!linked_stack_is_empty(stack)) {
        int value = linked_stack_pop(stack);
        printf("  pop() = %d ('%c'), ", value, value);
        linked_stack_print(stack);
    }
    
    linked_stack_destroy(stack);
    printf("\nLinked stack destroyed.\n");
}

void demo_part3_postfix(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 3: Postfix Expression Evaluation                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    const char *expressions[] = {
        "3 4 +",           /* 3 + 4 = 7 */
        "3 4 2 * +",       /* 3 + (4 * 2) = 11 */
        "5 1 2 + 4 * + 3 -", /* 5 + ((1 + 2) * 4) - 3 = 14 */
        "10 2 / 3 *",      /* (10 / 2) * 3 = 15 */
        "8 3 - 2 *"        /* (8 - 3) * 2 = 10 */
    };
    
    const char *infix[] = {
        "3 + 4",
        "3 + (4 * 2)",
        "5 + ((1 + 2) * 4) - 3",
        "(10 / 2) * 3",
        "(8 - 3) * 2"
    };
    
    int num_expr = sizeof(expressions) / sizeof(expressions[0]);
    
    for (int i = 0; i < num_expr; i++) {
        int result = evaluate_postfix(expressions[i]);
        printf("Infix:   %s\n", infix[i]);
        printf("Postfix: %s\n", expressions[i]);
        printf("Result:  %d\n\n", result);
    }
}

void demo_part4_brackets(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 4: Balanced Bracket Checking                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    const char *test_cases[] = {
        "{[()]}",
        "((()))",
        "{[}]",
        "((())",
        "int main() { if (x > 0) { return x; } }",
        "array[i] = func(a, b);",
        "((a + b) * (c - d))",
        "{[(])}"
    };
    
    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);
    
    for (int i = 0; i < num_tests; i++) {
        printf("Expression: \"%s\"\n", test_cases[i]);
        bool result = check_balanced_brackets(test_cases[i]);
        printf("Result: %s\n\n", result ? "✓ VALID" : "✗ INVALID");
    }
}

void demo_part5_undo_redo(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 5: Undo/Redo System (Text Editor)                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    TextEditor *editor = editor_create();
    if (!editor) return;
    
    printf("Simulating text editor operations:\n\n");
    
    printf("Type 'Hello': ");
    editor_type(editor, "Hello");
    printf("Text: \"%s\"\n", editor->current.text);
    
    printf("Type ' World': ");
    editor_type(editor, " World");
    printf("Text: \"%s\"\n", editor->current.text);
    
    printf("Type '!': ");
    editor_type(editor, "!");
    printf("Text: \"%s\"\n", editor->current.text);
    
    printf("\nUndo: ");
    editor_undo(editor);
    printf("Text: \"%s\"\n", editor->current.text);
    
    printf("Undo: ");
    editor_undo(editor);
    printf("Text: \"%s\"\n", editor->current.text);
    
    printf("Redo: ");
    editor_redo(editor);
    printf("Text: \"%s\"\n", editor->current.text);
    
    printf("\nType ' C!': ");
    editor_type(editor, " C!");
    printf("Text: \"%s\"\n", editor->current.text);
    
    printf("Redo (should fail - new action clears redo): ");
    if (!editor_redo(editor)) {
        printf("Nothing to redo\n");
    }
    printf("Text: \"%s\"\n", editor->current.text);
    
    editor_destroy(editor);
    printf("\nEditor destroyed.\n");
}

void demo_part6_complexity(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 6: Complexity Analysis Summary                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("┌─────────────┬────────────────┬────────────────┐\n");
    printf("│ Operation   │ Array Stack    │ Linked Stack   │\n");
    printf("├─────────────┼────────────────┼────────────────┤\n");
    printf("│ push        │ O(1) amortised │ O(1)           │\n");
    printf("│ pop         │ O(1)           │ O(1)           │\n");
    printf("│ peek        │ O(1)           │ O(1)           │\n");
    printf("│ isEmpty     │ O(1)           │ O(1)           │\n");
    printf("│ size        │ O(1)           │ O(1)           │\n");
    printf("├─────────────┼────────────────┼────────────────┤\n");
    printf("│ Space       │ O(capacity)    │ O(n) + ptrs    │\n");
    printf("└─────────────┴────────────────┴────────────────┘\n");
    printf("\n");
    printf("Key Trade-offs:\n");
    printf("  • Array: Better cache locality, occasional resize overhead\n");
    printf("  • Linked: No resize overhead, but pointer overhead per element\n");
    printf("  • Array typically preferred for most use cases\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     WEEK 05: STACKS — Complete Demonstration                  ║\n");
    printf("║     Algorithms and Programming Techniques                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    demo_part1_array_stack();
    demo_part2_linked_stack();
    demo_part3_postfix();
    demo_part4_brackets();
    demo_part5_undo_redo();
    demo_part6_complexity();
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  All demonstrations complete. No memory leaks (verify with Valgrind).\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    return 0;
}
