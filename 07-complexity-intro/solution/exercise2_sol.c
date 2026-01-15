/**
 * =============================================================================
 * EXERCISE 2 - SOLUTION: Expression Tree Evaluator
 * =============================================================================
 *
 * INSTRUCTOR COPY - Complete working solution
 *
 * All TODOs have been implemented correctly.
 * This solution passes all test cases and Valgrind memory check.
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/* =============================================================================
 * CONSTANTS
 * =============================================================================
 */

#define MAX_EXPR_LEN 256
#define MAX_STACK_SIZE 100

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/* TODO 1: SOLUTION */
typedef struct ExprNode {
    char op;
    int value;
    int is_operator;
    struct ExprNode *left;
    struct ExprNode *right;
} ExprNode;

typedef struct Stack {
    ExprNode *items[MAX_STACK_SIZE];
    int top;
} Stack;

/* =============================================================================
 * STACK OPERATIONS
 * =============================================================================
 */

/* TODO 2: SOLUTION */
Stack* stack_create(void) {
    Stack *s = malloc(sizeof(Stack));
    if (s == NULL) {
        fprintf(stderr, "Error: Stack allocation failed\n");
        exit(EXIT_FAILURE);
    }
    s->top = -1;
    return s;
}

/* TODO 3: SOLUTION */
bool stack_is_empty(Stack *s) {
    return s->top == -1;
}

/* TODO 4: SOLUTION */
void stack_push(Stack *s, ExprNode *node) {
    if (s->top >= MAX_STACK_SIZE - 1) {
        fprintf(stderr, "Error: Stack overflow\n");
        return;
    }
    s->top++;
    s->items[s->top] = node;
}

/* TODO 5: SOLUTION */
ExprNode* stack_pop(Stack *s) {
    if (stack_is_empty(s)) {
        return NULL;
    }
    ExprNode *node = s->items[s->top];
    s->top--;
    return node;
}

void stack_destroy(Stack *s) {
    free(s);
}

/* =============================================================================
 * EXPRESSION NODE CREATION
 * =============================================================================
 */

/* TODO 6: SOLUTION */
ExprNode* create_operand(int value) {
    ExprNode *node = malloc(sizeof(ExprNode));
    if (node == NULL) {
        fprintf(stderr, "Error: Node allocation failed\n");
        exit(EXIT_FAILURE);
    }
    node->is_operator = 0;
    node->value = value;
    node->op = '\0';
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* TODO 7: SOLUTION */
ExprNode* create_operator(char op, ExprNode *left, ExprNode *right) {
    ExprNode *node = malloc(sizeof(ExprNode));
    if (node == NULL) {
        fprintf(stderr, "Error: Node allocation failed\n");
        exit(EXIT_FAILURE);
    }
    node->is_operator = 1;
    node->op = op;
    node->value = 0;
    node->left = left;
    node->right = right;
    return node;
}

/* =============================================================================
 * EXPRESSION TREE CONSTRUCTION
 * =============================================================================
 */

bool is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

/* TODO 8: SOLUTION */
ExprNode* build_from_postfix(char *postfix) {
    Stack *stack = stack_create();
    
    char expr[MAX_EXPR_LEN];
    strncpy(expr, postfix, MAX_EXPR_LEN - 1);
    expr[MAX_EXPR_LEN - 1] = '\0';
    
    char *token = strtok(expr, " ");
    
    while (token != NULL) {
        if (strlen(token) == 1 && is_operator(token[0])) {
            /* Operator: pop two nodes, create operator node */
            ExprNode *right = stack_pop(stack);
            ExprNode *left = stack_pop(stack);
            ExprNode *node = create_operator(token[0], left, right);
            stack_push(stack, node);
        } else {
            /* Number: create operand node */
            int value = atoi(token);
            ExprNode *node = create_operand(value);
            stack_push(stack, node);
        }
        
        token = strtok(NULL, " ");
    }
    
    ExprNode *root = stack_pop(stack);
    stack_destroy(stack);
    
    return root;
}

/* =============================================================================
 * EXPRESSION EVALUATION
 * =============================================================================
 */

/* TODO 9: SOLUTION */
int evaluate(ExprNode *node) {
    if (node == NULL) {
        return 0;
    }
    
    /* Base case: operand node */
    if (!node->is_operator) {
        return node->value;
    }
    
    /* Recursive case: evaluate children first */
    int left_val = evaluate(node->left);
    int right_val = evaluate(node->right);
    
    /* Apply operator */
    switch (node->op) {
        case '+': return left_val + right_val;
        case '-': return left_val - right_val;
        case '*': return left_val * right_val;
        case '/': 
            if (right_val == 0) {
                fprintf(stderr, "Error: Division by zero\n");
                return 0;
            }
            return left_val / right_val;
        default: return 0;
    }
}

/* =============================================================================
 * NOTATION CONVERSIONS
 * =============================================================================
 */

/* TODO 10: SOLUTION */
void to_infix(ExprNode *node) {
    if (node == NULL) {
        return;
    }
    
    if (!node->is_operator) {
        printf("%d", node->value);
    } else {
        printf("(");
        to_infix(node->left);
        printf(" %c ", node->op);
        to_infix(node->right);
        printf(")");
    }
}

/* TODO 11: SOLUTION */
void to_prefix(ExprNode *node) {
    if (node == NULL) {
        return;
    }
    
    if (!node->is_operator) {
        printf("%d ", node->value);
    } else {
        printf("%c ", node->op);
        to_prefix(node->left);
        to_prefix(node->right);
    }
}

/* TODO 12: SOLUTION */
void to_postfix(ExprNode *node) {
    if (node == NULL) {
        return;
    }
    
    if (!node->is_operator) {
        printf("%d ", node->value);
    } else {
        to_postfix(node->left);
        to_postfix(node->right);
        printf("%c ", node->op);
    }
}

/* =============================================================================
 * MEMORY MANAGEMENT
 * =============================================================================
 */

/* TODO 13: SOLUTION */
void free_expr_tree(ExprNode *node) {
    if (node == NULL) {
        return;
    }
    free_expr_tree(node->left);
    free_expr_tree(node->right);
    free(node);
}

/* =============================================================================
 * HELPER FUNCTIONS
 * =============================================================================
 */

void print_expr_tree(ExprNode *node, int level) {
    if (node == NULL) return;
    
    print_expr_tree(node->right, level + 1);
    
    for (int i = 0; i < level; i++) {
        printf("    ");
    }
    
    if (node->is_operator) {
        printf("[%c]\n", node->op);
    } else {
        printf("%d\n", node->value);
    }
    
    print_expr_tree(node->left, level + 1);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

/* TODO 14: SOLUTION */
int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 2: Expression Tree Evaluator                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    char expr1[] = "3 4 + 5 *";
    char expr2[] = "10 2 / 3 +";
    char expr3[] = "5 1 2 + 4 * + 3 -";
    
    /* Expression 1 */
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Expression 1: %s\n", expr1);
    printf("═══════════════════════════════════════════════════════════════\n");
    
    ExprNode *tree1 = build_from_postfix(expr1);
    
    printf("\nTree Structure:\n");
    print_expr_tree(tree1, 0);
    
    printf("\nEvaluation: %d\n", evaluate(tree1));
    
    printf("\nNotations:\n");
    printf("  Infix:   ");
    to_infix(tree1);
    printf("\n");
    printf("  Prefix:  ");
    to_prefix(tree1);
    printf("\n");
    printf("  Postfix: ");
    to_postfix(tree1);
    printf("\n");
    
    free_expr_tree(tree1);
    
    /* Expression 2 */
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Expression 2: %s\n", expr2);
    printf("═══════════════════════════════════════════════════════════════\n");
    
    ExprNode *tree2 = build_from_postfix(expr2);
    
    printf("\nTree Structure:\n");
    print_expr_tree(tree2, 0);
    
    printf("\nEvaluation: %d\n", evaluate(tree2));
    
    printf("\nNotations:\n");
    printf("  Infix:   ");
    to_infix(tree2);
    printf("\n");
    printf("  Prefix:  ");
    to_prefix(tree2);
    printf("\n");
    printf("  Postfix: ");
    to_postfix(tree2);
    printf("\n");
    
    free_expr_tree(tree2);
    
    /* Expression 3 */
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Expression 3: %s\n", expr3);
    printf("═══════════════════════════════════════════════════════════════\n");
    
    ExprNode *tree3 = build_from_postfix(expr3);
    
    printf("\nTree Structure:\n");
    print_expr_tree(tree3, 0);
    
    printf("\nEvaluation: %d\n", evaluate(tree3));
    
    printf("\nNotations:\n");
    printf("  Infix:   ");
    to_infix(tree3);
    printf("\n");
    printf("  Prefix:  ");
    to_prefix(tree3);
    printf("\n");
    printf("  Postfix: ");
    to_postfix(tree3);
    printf("\n");
    
    free_expr_tree(tree3);
    
    printf("\nAll expression trees processed.\n");
    printf("Run with Valgrind to verify no memory leaks.\n\n");
    
    return 0;
}
