/**
 * =============================================================================
 * HOMEWORK 1 SOLUTION: Postfix Expression Calculator
 * =============================================================================
 *
 * Complete solution for the postfix expression calculator homework.
 * FOR INSTRUCTOR USE ONLY - Do not distribute to students.
 *
 * Features:
 *   - Multi-digit number support
 *   - Negative number support
 *   - Four basic operations (+, -, *, /)
 *   - Comprehensive error handling
 *   - Clean memory management
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c
 * Usage: ./homework1_sol < expressions.txt
 *
 * =============================================================================
 */

#define _POSIX_C_SOURCE 200809L  /* Enable strdup */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>

#define INITIAL_CAPACITY 32
#define MAX_LINE_LENGTH 1024
#define GROWTH_FACTOR 2

/* =============================================================================
 * STACK IMPLEMENTATION
 * =============================================================================
 */

typedef struct {
    long *data;
    int top;
    int capacity;
} Stack;

Stack* stack_create(int capacity) {
    Stack *s = malloc(sizeof(Stack));
    if (!s) return NULL;
    
    s->data = malloc(capacity * sizeof(long));
    if (!s->data) {
        free(s);
        return NULL;
    }
    
    s->top = -1;
    s->capacity = capacity;
    return s;
}

void stack_destroy(Stack *s) {
    if (s) {
        free(s->data);
        free(s);
    }
}

bool stack_is_empty(const Stack *s) {
    return s->top == -1;
}

int stack_size(const Stack *s) {
    return s->top + 1;
}

bool stack_push(Stack *s, long value) {
    if (s->top == s->capacity - 1) {
        int new_capacity = s->capacity * GROWTH_FACTOR;
        long *new_data = realloc(s->data, new_capacity * sizeof(long));
        if (!new_data) return false;
        s->data = new_data;
        s->capacity = new_capacity;
    }
    
    s->data[++s->top] = value;
    return true;
}

bool stack_pop(Stack *s, long *value) {
    if (stack_is_empty(s)) return false;
    *value = s->data[s->top--];
    return true;
}

/* =============================================================================
 * EXPRESSION EVALUATION
 * =============================================================================
 */

typedef enum {
    EVAL_OK,
    EVAL_DIVISION_BY_ZERO,
    EVAL_MALFORMED,
    EVAL_OVERFLOW,
    EVAL_MEMORY_ERROR
} EvalResult;

/**
 * Check if a string is a valid integer
 */
bool is_number(const char *token) {
    if (!token || !*token) return false;
    
    const char *p = token;
    if (*p == '-' || *p == '+') p++;  /* Skip sign */
    
    if (!*p) return false;  /* Just a sign is not a number */
    
    while (*p) {
        if (!isdigit(*p)) return false;
        p++;
    }
    return true;
}

/**
 * Check if a string is an operator
 */
bool is_operator(const char *token) {
    return token && strlen(token) == 1 && 
           (*token == '+' || *token == '-' || 
            *token == '*' || *token == '/');
}

/**
 * Evaluate a postfix expression
 */
EvalResult evaluate_postfix(const char *expression, long *result) {
    Stack *stack = stack_create(INITIAL_CAPACITY);
    if (!stack) return EVAL_MEMORY_ERROR;
    
    /* Make a copy for tokenisation */
    char *expr_copy = strdup(expression);
    if (!expr_copy) {
        stack_destroy(stack);
        return EVAL_MEMORY_ERROR;
    }
    
    EvalResult status = EVAL_OK;
    char *token = strtok(expr_copy, " \t");
    
    while (token && status == EVAL_OK) {
        if (is_number(token)) {
            /* Parse the number */
            errno = 0;
            char *endptr;
            long num = strtol(token, &endptr, 10);
            
            if (errno == ERANGE) {
                status = EVAL_OVERFLOW;
            } else {
                stack_push(stack, num);
            }
        }
        else if (is_operator(token)) {
            /* Need at least 2 operands */
            if (stack_size(stack) < 2) {
                status = EVAL_MALFORMED;
            } else {
                long b, a;
                stack_pop(stack, &b);
                stack_pop(stack, &a);
                
                long res = 0;
                switch (token[0]) {
                    case '+': res = a + b; break;
                    case '-': res = a - b; break;
                    case '*': res = a * b; break;
                    case '/':
                        if (b == 0) {
                            status = EVAL_DIVISION_BY_ZERO;
                        } else {
                            res = a / b;
                        }
                        break;
                }
                
                if (status == EVAL_OK) {
                    stack_push(stack, res);
                }
            }
        }
        else {
            /* Unknown token */
            status = EVAL_MALFORMED;
        }
        
        token = strtok(NULL, " \t");
    }
    
    /* Final check: should have exactly one value on stack */
    if (status == EVAL_OK) {
        if (stack_size(stack) != 1) {
            status = EVAL_MALFORMED;
        } else {
            stack_pop(stack, result);
        }
    }
    
    free(expr_copy);
    stack_destroy(stack);
    return status;
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    FILE *input = stdin;
    
    /* Check for file argument */
    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (!input) {
            fprintf(stderr, "Error: Cannot open file '%s'\n", argv[1]);
            return EXIT_FAILURE;
        }
    }
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     HOMEWORK 1: Postfix Expression Calculator                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    char line[MAX_LINE_LENGTH];
    int line_count = 0;
    int success_count = 0;
    int error_count = 0;
    
    while (fgets(line, sizeof(line), input)) {
        /* Remove trailing newline */
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }
        
        /* Skip empty lines and comments */
        if (len == 0 || line[0] == '#') {
            continue;
        }
        
        line_count++;
        long result;
        EvalResult status = evaluate_postfix(line, &result);
        
        switch (status) {
            case EVAL_OK:
                printf("Expression: %s = %ld\n", line, result);
                success_count++;
                break;
                
            case EVAL_DIVISION_BY_ZERO:
                printf("Error: Division by zero in expression \"%s\"\n", line);
                error_count++;
                break;
                
            case EVAL_MALFORMED:
                printf("Error: Malformed expression \"%s\"\n", line);
                error_count++;
                break;
                
            case EVAL_OVERFLOW:
                printf("Error: Integer overflow in expression \"%s\"\n", line);
                error_count++;
                break;
                
            case EVAL_MEMORY_ERROR:
                printf("Error: Memory allocation failed\n");
                error_count++;
                break;
        }
    }
    
    printf("\n───────────────────────────────────────────────────────────────\n");
    printf("Processed %d expressions: %d successful, %d errors\n\n",
           line_count, success_count, error_count);
    
    if (input != stdin) {
        fclose(input);
    }
    
    return EXIT_SUCCESS;
}
