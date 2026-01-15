/**
 * =============================================================================
 * Week 08 Homework 2: Expression Tree Evaluator - SOLUTION
 * =============================================================================
 * 
 * INSTRUCTOR ONLY - Do not distribute to students
 *
 * This implements an expression tree that represents arithmetic expressions.
 * Trees are built from postfix notation and can be evaluated/displayed.
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct ExprNode {
    bool is_operator;
    union {
        int operand;
        char operator;
    } value;
    struct ExprNode *left;
    struct ExprNode *right;
} ExprNode;

/* Stack for building expression tree */
#define MAX_STACK 100

typedef struct {
    ExprNode *items[MAX_STACK];
    int top;
} NodeStack;

/* Forward declarations */
void free_expression_tree(ExprNode *root);

/* =============================================================================
 * STACK OPERATIONS
 * =============================================================================
 */

void stack_init(NodeStack *s) {
    s->top = -1;
}

bool stack_is_empty(NodeStack *s) {
    return s->top == -1;
}

bool stack_push(NodeStack *s, ExprNode *node) {
    if (s->top >= MAX_STACK - 1) {
        return false;
    }
    s->items[++s->top] = node;
    return true;
}

ExprNode *stack_pop(NodeStack *s) {
    if (stack_is_empty(s)) {
        return NULL;
    }
    return s->items[s->top--];
}

/* =============================================================================
 * NODE CREATION
 * =============================================================================
 */

ExprNode *create_operand_node(int value) {
    ExprNode *node = (ExprNode *)malloc(sizeof(ExprNode));
    if (node == NULL) {
        return NULL;
    }
    
    node->is_operator = false;
    node->value.operand = value;
    node->left = NULL;
    node->right = NULL;
    
    return node;
}

ExprNode *create_operator_node(char op, ExprNode *left, ExprNode *right) {
    ExprNode *node = (ExprNode *)malloc(sizeof(ExprNode));
    if (node == NULL) {
        return NULL;
    }
    
    node->is_operator = true;
    node->value.operator = op;
    node->left = left;
    node->right = right;
    
    return node;
}

/* =============================================================================
 * BUILD EXPRESSION TREE FROM POSTFIX
 * =============================================================================
 */

bool is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

ExprNode *build_expression_tree(const char *postfix) {
    NodeStack stack;
    stack_init(&stack);
    
    const char *p = postfix;
    
    while (*p != '\0') {
        /* Skip whitespace */
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        
        if (*p == '\0') {
            break;
        }
        
        if (isdigit(*p) || (*p == '-' && isdigit(*(p + 1)))) {
            /* Parse number (possibly negative) */
            int num = 0;
            int sign = 1;
            
            if (*p == '-') {
                sign = -1;
                p++;
            }
            
            while (isdigit(*p)) {
                num = num * 10 + (*p - '0');
                p++;
            }
            
            num *= sign;
            
            ExprNode *operand = create_operand_node(num);
            if (operand == NULL) {
                /* Cleanup on error */
                while (!stack_is_empty(&stack)) {
                    free(stack_pop(&stack));
                }
                return NULL;
            }
            
            stack_push(&stack, operand);
        }
        else if (is_operator(*p)) {
            /* Pop two operands, create operator node */
            ExprNode *right = stack_pop(&stack);
            ExprNode *left = stack_pop(&stack);
            
            if (left == NULL || right == NULL) {
                fprintf(stderr, "Error: Invalid postfix expression\n");
                /* Cleanup */
                if (left) free(left);
                if (right) free(right);
                while (!stack_is_empty(&stack)) {
                    free(stack_pop(&stack));
                }
                return NULL;
            }
            
            ExprNode *op_node = create_operator_node(*p, left, right);
            if (op_node == NULL) {
                free(left);
                free(right);
                while (!stack_is_empty(&stack)) {
                    free(stack_pop(&stack));
                }
                return NULL;
            }
            
            stack_push(&stack, op_node);
            p++;
        }
        else {
            p++;  /* Skip unknown character */
        }
    }
    
    /* Result should be the only item on stack */
    ExprNode *result = stack_pop(&stack);
    
    /* Clean up any remaining items (shouldn't happen with valid input) */
    while (!stack_is_empty(&stack)) {
        ExprNode *extra = stack_pop(&stack);
        free_expression_tree(extra);
    }
    
    return result;
}

/* =============================================================================
 * TREE EVALUATION
 * =============================================================================
 */

int evaluate(ExprNode *root) {
    if (root == NULL) {
        return 0;
    }
    
    /* Leaf node: return operand */
    if (!root->is_operator) {
        return root->value.operand;
    }
    
    /* Internal node: evaluate children and apply operator */
    int left_val = evaluate(root->left);
    int right_val = evaluate(root->right);
    
    switch (root->value.operator) {
        case '+':
            return left_val + right_val;
        case '-':
            return left_val - right_val;
        case '*':
            return left_val * right_val;
        case '/':
            if (right_val == 0) {
                fprintf(stderr, "Warning: Division by zero\n");
                return 0;
            }
            return left_val / right_val;
        default:
            fprintf(stderr, "Error: Unknown operator '%c'\n", root->value.operator);
            return 0;
    }
}

/* =============================================================================
 * DISPLAY OPERATIONS
 * =============================================================================
 */

void display_infix(ExprNode *root) {
    if (root == NULL) {
        return;
    }
    
    if (!root->is_operator) {
        /* Operand: just print the value */
        printf("%d", root->value.operand);
    } else {
        /* Operator: print with parentheses */
        printf("(");
        display_infix(root->left);
        printf(" %c ", root->value.operator);
        display_infix(root->right);
        printf(")");
    }
}

void display_prefix(ExprNode *root) {
    if (root == NULL) {
        return;
    }
    
    if (!root->is_operator) {
        printf("%d ", root->value.operand);
    } else {
        printf("%c ", root->value.operator);
        display_prefix(root->left);
        display_prefix(root->right);
    }
}

void display_postfix(ExprNode *root) {
    if (root == NULL) {
        return;
    }
    
    if (!root->is_operator) {
        printf("%d ", root->value.operand);
    } else {
        display_postfix(root->left);
        display_postfix(root->right);
        printf("%c ", root->value.operator);
    }
}

/* =============================================================================
 * MEMORY CLEANUP
 * =============================================================================
 */

void free_expression_tree(ExprNode *root) {
    if (root == NULL) {
        return;
    }
    
    free_expression_tree(root->left);
    free_expression_tree(root->right);
    free(root);
}

/* =============================================================================
 * TREE HEIGHT (for display)
 * =============================================================================
 */

int tree_height(ExprNode *root) {
    if (root == NULL) {
        return -1;
    }
    
    int left_h = tree_height(root->left);
    int right_h = tree_height(root->right);
    
    return 1 + (left_h > right_h ? left_h : right_h);
}

/* =============================================================================
 * VISUAL TREE PRINTING
 * =============================================================================
 */

void print_tree_helper(ExprNode *root, int space, int indent) {
    if (root == NULL) {
        return;
    }
    
    space += indent;
    
    print_tree_helper(root->right, space, indent);
    
    printf("\n");
    for (int i = indent; i < space; i++) {
        printf(" ");
    }
    
    if (root->is_operator) {
        printf("%c", root->value.operator);
    } else {
        printf("%d", root->value.operand);
    }
    
    print_tree_helper(root->left, space, indent);
}

void print_expression_tree(ExprNode *root) {
    if (root == NULL) {
        printf("(empty tree)\n");
        return;
    }
    print_tree_helper(root, 0, 4);
    printf("\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    char postfix[256];
    
    /* Read postfix expression */
    if (fgets(postfix, sizeof(postfix), stdin) == NULL) {
        fprintf(stderr, "Error reading input\n");
        return 1;
    }
    
    /* Remove trailing newline */
    size_t len = strlen(postfix);
    if (len > 0 && postfix[len - 1] == '\n') {
        postfix[len - 1] = '\0';
    }
    
    /* Build expression tree */
    ExprNode *expr = build_expression_tree(postfix);
    
    if (expr == NULL) {
        fprintf(stderr, "Failed to build expression tree\n");
        return 1;
    }
    
    /* Display in various notations */
    printf("Infix: ");
    display_infix(expr);
    printf("\n");
    
    printf("Prefix: ");
    display_prefix(expr);
    printf("\n");
    
    printf("Postfix: ");
    display_postfix(expr);
    printf("\n");
    
    /* Evaluate */
    printf("Result: %d\n", evaluate(expr));
    
    /* Show tree structure */
    printf("\nTree structure:\n");
    print_expression_tree(expr);
    
    /* Cleanup */
    free_expression_tree(expr);
    
    return 0;
}
