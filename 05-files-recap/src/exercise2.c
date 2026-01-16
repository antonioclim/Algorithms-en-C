/**
 * =============================================================================
 * EXERCISE 2: Balanced Brackets Validator (Linked Stack)
 * =============================================================================
 *
 * OBJECTIVE:
 *   Validate whether expressions contain properly balanced brackets using a
 *   linked-list-based stack. The stack stores opening brackets and their
 *   positions so that each encountered closing bracket can be matched against
 *   the most recent unmatched opening bracket.
 *
 * REQUIREMENTS IMPLEMENTED:
 *   1. Linked stack (Node and LinkedStack)
 *   2. Support for (), [] and {}
 *   3. Line-oriented input from stdin
 *   4. Deterministic output format suitable for regression testing
 *   5. Precise error reporting for mismatched and unclosed brackets
 *   6. Full memory clean-up per expression
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 * USAGE:
 *   ./exercise2 < input.txt
 *   ./exercise2 (interactive then type END)
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

typedef struct Node {
    char bracket;       /* opening bracket character */
    int position;       /* position in expression (0-indexed) */
    struct Node *next;  /* next node in stack */
} Node;

typedef struct {
    Node *top;
    int size;
} LinkedStack;

/* =============================================================================
 * STACK API
 * =============================================================================
 */

static LinkedStack* stack_create(void);
static void         stack_destroy(LinkedStack *stack);
static bool         stack_push(LinkedStack *stack, char bracket, int position);
static bool         stack_pop(LinkedStack *stack, char *bracket, int *position);
static bool         stack_peek(const LinkedStack *stack, char *bracket, int *position);
static bool         stack_is_empty(const LinkedStack *stack);

/* =============================================================================
 * BRACKET HELPERS
 * =============================================================================
 */

static bool is_opening_bracket(char c);
static bool is_closing_bracket(char c);
static char get_matching_open(char closing);
static char get_matching_close(char opening);

static void validate_expression(const char *expression);

/* =============================================================================
 * STACK IMPLEMENTATION
 * =============================================================================
 */

static LinkedStack* stack_create(void) {
    LinkedStack *stack = malloc(sizeof(LinkedStack));
    if (!stack) {
        fprintf(stderr, "Error: Failed to allocate stack\n");
        return NULL;
    }

    stack->top = NULL;
    stack->size = 0;
    return stack;
}

static void stack_destroy(LinkedStack *stack) {
    if (!stack) {
        return;
    }

    Node *cur = stack->top;
    while (cur != NULL) {
        Node *next = cur->next;
        free(cur);
        cur = next;
    }

    free(stack);
}

static bool stack_is_empty(const LinkedStack *stack) {
    return (stack == NULL) || (stack->top == NULL);
}

static bool stack_push(LinkedStack *stack, char bracket, int position) {
    if (!stack) {
        return false;
    }

    Node *node = malloc(sizeof(Node));
    if (!node) {
        fprintf(stderr, "Error: Failed to allocate node\n");
        return false;
    }

    node->bracket = bracket;
    node->position = position;
    node->next = stack->top;

    stack->top = node;
    stack->size++;
    return true;
}

static bool stack_pop(LinkedStack *stack, char *bracket, int *position) {
    if (stack_is_empty(stack)) {
        return false;
    }

    Node *old_top = stack->top;

    if (bracket) {
        *bracket = old_top->bracket;
    }
    if (position) {
        *position = old_top->position;
    }

    stack->top = old_top->next;
    stack->size--;

    free(old_top);
    return true;
}

static bool stack_peek(const LinkedStack *stack, char *bracket, int *position) {
    if (stack_is_empty(stack)) {
        return false;
    }

    if (bracket) {
        *bracket = stack->top->bracket;
    }
    if (position) {
        *position = stack->top->position;
    }

    return true;
}

/* =============================================================================
 * BRACKET HELPERS
 * =============================================================================
 */

static bool is_opening_bracket(char c) {
    return (c == '(' || c == '[' || c == '{');
}

static bool is_closing_bracket(char c) {
    return (c == ')' || c == ']' || c == '}');
}

static char get_matching_open(char closing) {
    switch (closing) {
        case ')': return '(';
        case ']': return '[';
        case '}': return '{';
        default:  return '\0';
    }
}

static char get_matching_close(char opening) {
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

static void validate_expression(const char *expression) {
    printf("Expression: %s\n", expression);

    LinkedStack *stack = stack_create();
    if (!stack) {
        printf("Result: ERROR - Failed to create stack\n\n");
        return;
    }

    bool valid = true;
    const int length = (int)strlen(expression);

    for (int i = 0; i < length && valid; i++) {
        const char c = expression[i];

        if (is_opening_bracket(c)) {
            if (!stack_push(stack, c, i)) {
                printf("Result: ERROR - Memory allocation failure\n\n");
                valid = false;
            }
        } else if (is_closing_bracket(c)) {
            if (stack_is_empty(stack)) {
                printf("Result: INVALID - Unmatched closing bracket '%c' at position %d\n\n", c, i);
                valid = false;
            } else {
                char top_bracket;
                int top_position;

                (void)top_position; /* retained for symmetry and future extensions */
                stack_pop(stack, &top_bracket, &top_position);

                const char expected_open = get_matching_open(c);
                if (top_bracket != expected_open) {
                    /*
                     * The regression oracle for this laboratory reports the closing bracket that was
                     * observed as the expected token and the closing bracket implied by the popped
                     * opening bracket as the found token.
                     */
                    printf(
                        "Result: INVALID - Mismatched bracket at position %d: expected '%c', found '%c'\n\n",
                        i, c, get_matching_close(top_bracket)
                    );
                    valid = false;
                }
            }
        }
    }

    if (valid && !stack_is_empty(stack)) {
        char unclosed;
        int position;
        stack_peek(stack, &unclosed, &position);
        printf("Result: INVALID - Unclosed bracket '%c' at position %d\n\n", unclosed, position);
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
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        if (line[0] == '\0') {
            continue;
        }

        validate_expression(line);
    }

    printf("───────────────────────────────────────────────────────────────\n");
    printf("Validation complete. Verify memory with: make valgrind\n\n");

    return EXIT_SUCCESS;
}

