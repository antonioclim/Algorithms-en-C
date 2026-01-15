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
 * TODO 1: Complete the Expression Node structure
 *
 * An expression tree node should contain:
 *   - A character 'op' for the operator (+, -, *, /)
 *   - An integer 'value' for operand nodes
 *   - A boolean 'is_operator' to distinguish operators from operands
 *   - Pointers to left and right children
 *
 * Hint: Operators have children, operands (leaves) don't
 */
typedef struct ExprNode {
    char op;              /* Operator: '+', '-', '*', '/' */
    int value;            /* Operand value (only for leaves) */
    int is_operator;      /* 1 if operator, 0 if operand */
    
    /* YOUR CODE HERE - Add left and right child pointers */
    
    
} ExprNode;

/**
 * Stack structure for building the expression tree.
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

/**
 * TODO 2: Implement stack initialisation
 *
 * Create a new empty stack.
 *
 * Steps:
 *   1. Allocate memory for Stack structure
 *   2. Check allocation success
 *   3. Set top to -1 (empty stack)
 *   4. Return stack pointer
 *
 * @return Pointer to new stack
 */
Stack* stack_create(void) {
    /* YOUR CODE HERE */
    
    
    
    
    return NULL;  /* Replace this */
}

/**
 * TODO 3: Implement stack empty check
 *
 * @param s Pointer to stack
 * @return true if stack is empty
 */
bool stack_is_empty(Stack *s) {
    /* YOUR CODE HERE */
    
    return true;  /* Replace this */
}

/**
 * TODO 4: Implement stack push operation
 *
 * Add a node to the top of the stack.
 *
 * Steps:
 *   1. Check for stack overflow
 *   2. Increment top
 *   3. Store node at items[top]
 *
 * @param s Pointer to stack
 * @param node Node to push
 */
void stack_push(Stack *s, ExprNode *node) {
    /* YOUR CODE HERE */
    
    
    
    
}

/**
 * TODO 5: Implement stack pop operation
 *
 * Remove and return the top node.
 *
 * Steps:
 *   1. Check for stack underflow (return NULL)
 *   2. Get node from items[top]
 *   3. Decrement top
 *   4. Return node
 *
 * @param s Pointer to stack
 * @return Top node (or NULL if empty)
 */
ExprNode* stack_pop(Stack *s) {
    /* YOUR CODE HERE */
    
    
    
    
    return NULL;  /* Replace this */
}

/**
 * Destroy the stack (free memory).
 */
void stack_destroy(Stack *s) {
    free(s);
}

/* =============================================================================
 * EXPRESSION NODE CREATION
 * =============================================================================
 */

/**
 * TODO 6: Create an operand (leaf) node
 *
 * Create a node representing a number.
 *
 * Steps:
 *   1. Allocate memory for ExprNode
 *   2. Set is_operator to 0 (false)
 *   3. Set value to the given number
 *   4. Set op to '\0' (no operator)
 *   5. Set left and right to NULL
 *   6. Return node pointer
 *
 * @param value The numeric value
 * @return Pointer to new operand node
 */
ExprNode* create_operand(int value) {
    /* YOUR CODE HERE */
    
    
    
    
    
    
    return NULL;  /* Replace this */
}

/**
 * TODO 7: Create an operator (internal) node
 *
 * Create a node representing an operator with children.
 *
 * Steps:
 *   1. Allocate memory for ExprNode
 *   2. Set is_operator to 1 (true)
 *   3. Set op to the given operator character
 *   4. Set value to 0 (not used)
 *   5. Set left and right to the given children
 *   6. Return node pointer
 *
 * @param op The operator character (+, -, *, /)
 * @param left Left child (first operand)
 * @param right Right child (second operand)
 * @return Pointer to new operator node
 */
ExprNode* create_operator(char op, ExprNode *left, ExprNode *right) {
    /* YOUR CODE HERE */
    
    
    
    
    
    
    
    return NULL;  /* Replace this */
}

/* =============================================================================
 * EXPRESSION TREE CONSTRUCTION
 * =============================================================================
 */

/**
 * Check if a character is an operator.
 */
bool is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

/**
 * TODO 8: Build expression tree from postfix notation
 *
 * Algorithm:
 *   For each token in the postfix expression:
 *     - If it's a number: create operand node, push to stack
 *     - If it's an operator: pop two nodes, create operator node
 *       with them as children, push result to stack
 *   At the end, stack contains the root of the expression tree.
 *
 * Example: "3 4 + 5 *"
 *   1. Push operand(3)
 *   2. Push operand(4)
 *   3. '+': pop 4 and 3, push operator('+', 3, 4)
 *   4. Push operand(5)
 *   5. '*': pop 5 and (+,3,4), push operator('*', (+,3,4), 5)
 *   6. Return root
 *
 * @param postfix The postfix expression string (space-separated)
 * @return Root of the expression tree
 *
 * Hint: Use strtok() to split by spaces, atoi() to convert numbers
 * Hint: When popping for operator, first pop is RIGHT, second is LEFT
 */
ExprNode* build_from_postfix(char *postfix) {
    Stack *stack = stack_create();
    
    /* Make a copy since strtok modifies the string */
    char expr[MAX_EXPR_LEN];
    strncpy(expr, postfix, MAX_EXPR_LEN - 1);
    expr[MAX_EXPR_LEN - 1] = '\0';
    
    /* Tokenise by spaces */
    char *token = strtok(expr, " ");
    
    while (token != NULL) {
        /* YOUR CODE HERE */
        
        /* Check if token is an operator or a number */
        /* If operator: pop two nodes, create operator node, push */
        /* If number: create operand node, push */
        
        
        
        
        
        
        
        
        token = strtok(NULL, " ");  /* Get next token */
    }
    
    /* The final item on stack is the root */
    ExprNode *root = stack_pop(stack);
    stack_destroy(stack);
    
    return root;
}

/* =============================================================================
 * EXPRESSION EVALUATION
 * =============================================================================
 */

/**
 * TODO 9: Evaluate the expression tree
 *
 * Use postorder traversal logic: evaluate children first, then apply operator.
 *
 * Steps:
 *   1. Base case: if node is an operand (not operator), return its value
 *   2. Recursively evaluate left subtree
 *   3. Recursively evaluate right subtree
 *   4. Apply the operator to the two results
 *   5. Return the result
 *
 * @param node Root of the expression tree
 * @return The computed result
 *
 * Hint: Use a switch statement for the four operators
 * Note: For division, use integer division (/)
 */
int evaluate(ExprNode *node) {
    /* YOUR CODE HERE */
    
    /* Base case: operand node */
    
    
    /* Recursive case: operator node */
    
    
    
    
    
    
    
    return 0;  /* Replace this */
}

/* =============================================================================
 * NOTATION CONVERSIONS
 * =============================================================================
 */

/**
 * TODO 10: Convert to infix notation with parentheses
 *
 * Inorder traversal with parentheses around each operator expression.
 *
 * Steps:
 *   1. If node is NULL, return
 *   2. If operand: print the value
 *   3. If operator:
 *      a. Print '('
 *      b. Recursively print left subtree (infix)
 *      c. Print ' ', operator, ' '
 *      d. Recursively print right subtree (infix)
 *      e. Print ')'
 *
 * @param node Root of the expression tree
 *
 * Example: Tree for "3 4 + 5 *" prints "((3 + 4) * 5)"
 */
void to_infix(ExprNode *node) {
    /* YOUR CODE HERE */
    
    
    
    
    
    
    
    
    
}

/**
 * TODO 11: Convert to prefix notation
 *
 * Preorder traversal: operator first, then operands.
 *
 * Steps:
 *   1. If node is NULL, return
 *   2. If operand: print value followed by space
 *   3. If operator:
 *      a. Print operator followed by space
 *      b. Recursively print left subtree (prefix)
 *      c. Recursively print right subtree (prefix)
 *
 * @param node Root of the expression tree
 *
 * Example: Tree for "3 4 + 5 *" prints "* + 3 4 5"
 */
void to_prefix(ExprNode *node) {
    /* YOUR CODE HERE */
    
    
    
    
    
    
}

/**
 * TODO 12: Convert to postfix notation
 *
 * Postorder traversal: operands first, then operator.
 *
 * Steps:
 *   1. If node is NULL, return
 *   2. If operand: print value followed by space
 *   3. If operator:
 *      a. Recursively print left subtree (postfix)
 *      b. Recursively print right subtree (postfix)
 *      c. Print operator followed by space
 *
 * @param node Root of the expression tree
 *
 * Example: Tree for "3 4 + 5 *" prints "3 4 + 5 *"
 */
void to_postfix(ExprNode *node) {
    /* YOUR CODE HERE */
    
    
    
    
    
    
}

/* =============================================================================
 * MEMORY MANAGEMENT
 * =============================================================================
 */

/**
 * TODO 13: Free the expression tree
 *
 * Use postorder traversal to free all nodes.
 *
 * Steps:
 *   1. If node is NULL, return
 *   2. Recursively free left subtree
 *   3. Recursively free right subtree
 *   4. Free current node
 *
 * @param node Root of the tree to free
 */
void free_expr_tree(ExprNode *node) {
    /* YOUR CODE HERE */
    
    
    
    
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
    
    /**
     * TODO 14: Complete the main function
     *
     * Test with multiple expressions:
     *   1. "3 4 + 5 *"      Expected: 35
     *   2. "10 2 / 3 +"     Expected: 8
     *   3. "5 1 2 + 4 * + 3 -"  Expected: 14
     */
    
    /* Test expressions */
    char expr1[] = "3 4 + 5 *";
    char expr2[] = "10 2 / 3 +";
    char expr3[] = "5 1 2 + 4 * + 3 -";
    
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Expression 1: %s\n", expr1);
    printf("═══════════════════════════════════════════════════════════════\n");
    
    /* YOUR CODE HERE */
    /* 1. Build tree: ExprNode *tree1 = build_from_postfix(expr1); */
    /* 2. Print tree structure */
    /* 3. Evaluate and print result */
    /* 4. Print all three notations */
    /* 5. Free the tree */
    
    ExprNode *tree1 = NULL;  /* Replace with build_from_postfix(expr1) */
    
    printf("\nTree Structure:\n");
    print_expr_tree(tree1, 0);
    
    printf("\nEvaluation: ");
    /* YOUR CODE HERE - Print evaluate(tree1) */
    
    printf("\n\nNotations:\n");
    printf("  Infix:   ");
    /* YOUR CODE HERE - Call to_infix(tree1) */
    
    printf("\n");
    printf("  Prefix:  ");
    /* YOUR CODE HERE - Call to_prefix(tree1) */
    
    printf("\n");
    printf("  Postfix: ");
    /* YOUR CODE HERE - Call to_postfix(tree1) */
    
    printf("\n");
    
    /* Free tree1 */
    /* YOUR CODE HERE */
    
    
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Expression 2: %s\n", expr2);
    printf("═══════════════════════════════════════════════════════════════\n");
    
    /* YOUR CODE HERE - Repeat for expr2 */
    
    
    
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Expression 3: %s\n", expr3);
    printf("═══════════════════════════════════════════════════════════════\n");
    
    /* YOUR CODE HERE - Repeat for expr3 */
    
    
    
    printf("\n");
    printf("All expression trees processed.\n");
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
