/**
 * =============================================================================
 * EXERCISE 2: Expression Tree Evaluator
 * =============================================================================
 *
 * OBJECTIVE:
 *   Build an expression tree from postfix notation and implement
 *   evaluation and notation conversion functions.
 *
 * REQUIREMENTS:
 *   1. Parse postfix expressions into tokens
 *   2. Build expression tree using a stack
 *   3. Evaluate expressions using postorder traversal
 *   4. Convert to infix notation with proper parentheses
 *   5. Convert to prefix notation
 *   6. Handle all four basic operators: +, -, *, /
 *
 * EXAMPLE INPUT:
 *   Postfix: "3 4 + 5 *"
 *
 * EXPECTED OUTPUT:
 *   Expression Tree Built
 *   Evaluation Result: 35
 *   Infix:   ((3 + 4) * 5)
 *   Prefix:  * + 3 4 5
 *   Postfix: 3 4 + 5 *
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 *
 * =============================================================================
 * Algorithms and Programming Techniques
 * Week 07: Binary Trees
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

/**
 * Expression tree node.
 *
 * In this representation, each node is either:
 *   - an operand node (leaf) with is_operator = 0 and a defined value
 *   - an operator node (internal) with is_operator = 1 and defined op
 */
typedef struct ExprNode {
    char op;              /* Operator: '+', '-', '*', '/' */
    int value;            /* Operand value (only for leaves) */
    int is_operator;      /* 1 if operator, 0 if operand */
    struct ExprNode *left;
    struct ExprNode *right;
} ExprNode;

/**
 * Stack for building the expression tree.
 * Stores pointers to ExprNode.
 */
typedef struct Stack {
    ExprNode *items[MAX_STACK_SIZE];
    int top;
} Stack;

/* =============================================================================
 * STACK OPERATIONS
 * =============================================================================
 */

Stack* stack_create(void) {
    Stack *s = malloc(sizeof(Stack));
    if (s == NULL) {
        fprintf(stderr, "Error: Stack allocation failed\n");
        exit(EXIT_FAILURE);
    }
    s->top = -1;
    return s;
}

bool stack_is_empty(Stack *s) {
    return s->top == -1;
}

void stack_push(Stack *s, ExprNode *node) {
    if (s->top >= MAX_STACK_SIZE - 1) {
        fprintf(stderr, "Error: Stack overflow\n");
        return;
    }
    s->top++;
    s->items[s->top] = node;
}

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

ExprNode* build_from_postfix(char *postfix) {
    Stack *stack = stack_create();

    /* strtok modifies its input, so copy into a local buffer */
    char expr[MAX_EXPR_LEN];
    strncpy(expr, postfix, MAX_EXPR_LEN - 1);
    expr[MAX_EXPR_LEN - 1] = '\0';

    char *token = strtok(expr, " ");
    while (token != NULL) {
        if (strlen(token) == 1 && is_operator(token[0])) {
            /* Operator: the first pop is the right operand */
            ExprNode *right = stack_pop(stack);
            ExprNode *left = stack_pop(stack);
            ExprNode *node = create_operator(token[0], left, right);
            stack_push(stack, node);
        } else {
            /* Operand: atoi handles multi-digit numbers, including negative */
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

int evaluate(ExprNode *node) {
    if (node == NULL) {
        return 0;
    }

    if (!node->is_operator) {
        return node->value;
    }

    int left_val = evaluate(node->left);
    int right_val = evaluate(node->right);

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
        default:
            return 0;
    }
}

/* =============================================================================
 * NOTATION CONVERSIONS
 * =============================================================================
 */

void to_infix(ExprNode *node) {
    if (node == NULL) {
        return;
    }

    if (!node->is_operator) {
        printf("%d", node->value);
        return;
    }

    printf("(");
    to_infix(node->left);
    printf(" %c ", node->op);
    to_infix(node->right);
    printf(")");
}

void to_prefix(ExprNode *node) {
    if (node == NULL) {
        return;
    }

    if (!node->is_operator) {
        printf("%d ", node->value);
        return;
    }

    printf("%c ", node->op);
    to_prefix(node->left);
    to_prefix(node->right);
}

void to_postfix(ExprNode *node) {
    if (node == NULL) {
        return;
    }

    if (!node->is_operator) {
        printf("%d ", node->value);
        return;
    }

    to_postfix(node->left);
    to_postfix(node->right);
    printf("%c ", node->op);
}

/* =============================================================================
 * MEMORY MANAGEMENT
 * =============================================================================
 */

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

/**
 * Print the expression tree structure (rotated 90 degrees).
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

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 2: Expression Tree Evaluator                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    /* Test expressions */
    char expr1[] = "3 4 + 5 *";
    char expr2[] = "10 2 / 3 +";
    char expr3[] = "5 1 2 + 4 * + 3 -";

    /* Expression 1 */
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Expression 1: %s\n", expr1);
    printf("═══════════════════════════════════════════════════════════════\n");

    ExprNode *tree1 = build_from_postfix(expr1);

    printf("\nTree Structure:\n");
    /* Start at level 1 to match the expected reference output */
    print_expr_tree(tree1, 1);

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
    print_expr_tree(tree2, 1);

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
    print_expr_tree(tree3, 1);

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

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Add support for multi-digit negative numbers (e.g., "-5", "123")
 *
 * 2. Implement error handling for malformed expressions
 *
 * 3. Add support for parentheses in infix input and convert to postfix
 *
 * 4. Implement expression simplification (e.g., "x * 0" -> "0")
 *
 * 5. Add support for variables (e.g., "x 2 *" with x=5 evaluates to 10)
 *
 * =============================================================================
 */
