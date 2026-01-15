/**
 * =============================================================================
 * HOMEWORK 2 SOLUTION: Infix to Postfix Converter (Shunting-Yard Algorithm)
 * =============================================================================
 *
 * Complete solution for the Shunting-Yard algorithm homework.
 * FOR INSTRUCTOR USE ONLY - Do not distribute to students.
 *
 * Features:
 *   - Operator precedence handling
 *   - Left-to-right associativity
 *   - Parentheses support
 *   - Multi-digit number support
 *   - Error detection for mismatched parentheses
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c
 * Usage: ./homework2_sol
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 1024
#define MAX_OUTPUT_LENGTH 2048

/* =============================================================================
 * OPERATOR STACK (Linked List Implementation)
 * =============================================================================
 */

typedef struct OpNode {
    char op;
    struct OpNode *next;
} OpNode;

typedef struct {
    OpNode *top;
    int size;
} OpStack;

OpStack* op_stack_create(void) {
    OpStack *s = malloc(sizeof(OpStack));
    if (s) {
        s->top = NULL;
        s->size = 0;
    }
    return s;
}

void op_stack_destroy(OpStack *s) {
    if (!s) return;
    while (s->top) {
        OpNode *temp = s->top;
        s->top = temp->next;
        free(temp);
    }
    free(s);
}

bool op_stack_is_empty(const OpStack *s) {
    return s->top == NULL;
}

bool op_stack_push(OpStack *s, char op) {
    OpNode *node = malloc(sizeof(OpNode));
    if (!node) return false;
    
    node->op = op;
    node->next = s->top;
    s->top = node;
    s->size++;
    return true;
}

char op_stack_pop(OpStack *s) {
    if (op_stack_is_empty(s)) return '\0';
    
    OpNode *temp = s->top;
    char op = temp->op;
    s->top = temp->next;
    s->size--;
    free(temp);
    return op;
}

char op_stack_peek(const OpStack *s) {
    return op_stack_is_empty(s) ? '\0' : s->top->op;
}

/* =============================================================================
 * HELPER FUNCTIONS
 * =============================================================================
 */

/**
 * Get operator precedence
 * Higher value = higher precedence
 */
int get_precedence(char op) {
    switch (op) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
        default:
            return 0;
    }
}

/**
 * Check if character is an operator
 */
bool is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

/**
 * Append a string to output with space separator
 */
void append_output(char *output, const char *token) {
    if (strlen(output) > 0) {
        strcat(output, " ");
    }
    strcat(output, token);
}

/**
 * Append a single character to output
 */
void append_char(char *output, char c) {
    char token[2] = {c, '\0'};
    append_output(output, token);
}

/* =============================================================================
 * SHUNTING-YARD ALGORITHM
 * =============================================================================
 */

typedef enum {
    CONVERT_OK,
    CONVERT_MISMATCHED_PAREN,
    CONVERT_INVALID_CHAR,
    CONVERT_MEMORY_ERROR
} ConvertResult;

/**
 * Convert infix expression to postfix using Shunting-Yard algorithm
 */
ConvertResult infix_to_postfix(const char *infix, char *postfix) {
    OpStack *op_stack = op_stack_create();
    if (!op_stack) return CONVERT_MEMORY_ERROR;
    
    postfix[0] = '\0';
    char number_buffer[64];
    int num_idx = 0;
    
    ConvertResult result = CONVERT_OK;
    
    for (int i = 0; infix[i] && result == CONVERT_OK; i++) {
        char c = infix[i];
        
        /* Skip whitespace */
        if (isspace(c)) {
            /* Flush number buffer if we have one */
            if (num_idx > 0) {
                number_buffer[num_idx] = '\0';
                append_output(postfix, number_buffer);
                num_idx = 0;
            }
            continue;
        }
        
        /* Handle digits (build multi-digit numbers) */
        if (isdigit(c)) {
            number_buffer[num_idx++] = c;
            continue;
        }
        
        /* Flush number buffer before processing operator/parenthesis */
        if (num_idx > 0) {
            number_buffer[num_idx] = '\0';
            append_output(postfix, number_buffer);
            num_idx = 0;
        }
        
        /* Handle operators */
        if (is_operator(c)) {
            int curr_prec = get_precedence(c);
            
            /* Pop operators with greater or equal precedence */
            while (!op_stack_is_empty(op_stack)) {
                char top = op_stack_peek(op_stack);
                if (top == '(') break;
                
                int top_prec = get_precedence(top);
                if (top_prec < curr_prec) break;
                
                /* Left-to-right associativity: pop if equal precedence */
                append_char(postfix, op_stack_pop(op_stack));
            }
            
            op_stack_push(op_stack, c);
        }
        /* Handle left parenthesis */
        else if (c == '(') {
            op_stack_push(op_stack, c);
        }
        /* Handle right parenthesis */
        else if (c == ')') {
            /* Pop until matching left parenthesis */
            bool found_left = false;
            while (!op_stack_is_empty(op_stack)) {
                char top = op_stack_pop(op_stack);
                if (top == '(') {
                    found_left = true;
                    break;
                }
                append_char(postfix, top);
            }
            
            if (!found_left) {
                result = CONVERT_MISMATCHED_PAREN;
            }
        }
        /* Invalid character */
        else {
            result = CONVERT_INVALID_CHAR;
        }
    }
    
    /* Flush final number buffer */
    if (num_idx > 0 && result == CONVERT_OK) {
        number_buffer[num_idx] = '\0';
        append_output(postfix, number_buffer);
    }
    
    /* Pop remaining operators */
    while (!op_stack_is_empty(op_stack) && result == CONVERT_OK) {
        char top = op_stack_pop(op_stack);
        if (top == '(') {
            result = CONVERT_MISMATCHED_PAREN;
        } else {
            append_char(postfix, top);
        }
    }
    
    op_stack_destroy(op_stack);
    return result;
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     HOMEWORK 2: Infix to Postfix Converter                    ║\n");
    printf("║     (Shunting-Yard Algorithm)                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Enter infix expressions (one per line).\n");
    printf("Type 'quit' or press Ctrl+D to exit.\n\n");
    
    char line[MAX_LINE_LENGTH];
    char postfix[MAX_OUTPUT_LENGTH];
    
    while (1) {
        printf("Enter infix expression: ");
        fflush(stdout);
        
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }
        
        /* Remove trailing newline */
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }
        
        /* Check for quit */
        if (strcmp(line, "quit") == 0 || strcmp(line, "exit") == 0) {
            break;
        }
        
        /* Skip empty lines */
        if (len == 0) {
            continue;
        }
        
        /* Convert and display result */
        ConvertResult result = infix_to_postfix(line, postfix);
        
        switch (result) {
            case CONVERT_OK:
                printf("Postfix: %s\n\n", postfix);
                break;
                
            case CONVERT_MISMATCHED_PAREN:
                printf("Error: Mismatched parentheses\n\n");
                break;
                
            case CONVERT_INVALID_CHAR:
                printf("Error: Invalid character in expression\n\n");
                break;
                
            case CONVERT_MEMORY_ERROR:
                printf("Error: Memory allocation failed\n\n");
                break;
        }
    }
    
    printf("───────────────────────────────────────────────────────────────\n");
    printf("Goodbye!\n\n");
    
    return EXIT_SUCCESS;
}
