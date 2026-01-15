/**
 * =============================================================================
 * EXERCISE 1 - SOLUTION: Binary Tree Construction and Traversal
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
#include <stdbool.h>

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/* TODO 1: SOLUTION */
typedef struct TreeNode {
    int data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

/* =============================================================================
 * NODE CREATION
 * =============================================================================
 */

/* TODO 2: SOLUTION */
TreeNode* create_node(int value) {
    TreeNode *node = malloc(sizeof(TreeNode));
    if (node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    node->data = value;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* =============================================================================
 * TRAVERSAL FUNCTIONS
 * =============================================================================
 */

/* TODO 3: SOLUTION - Preorder */
void preorder(TreeNode *node) {
    if (node == NULL) {
        return;
    }
    printf("%d ", node->data);
    preorder(node->left);
    preorder(node->right);
}

/* TODO 4: SOLUTION - Inorder */
void inorder(TreeNode *node) {
    if (node == NULL) {
        return;
    }
    inorder(node->left);
    printf("%d ", node->data);
    inorder(node->right);
}

/* TODO 5: SOLUTION - Postorder */
void postorder(TreeNode *node) {
    if (node == NULL) {
        return;
    }
    postorder(node->left);
    postorder(node->right);
    printf("%d ", node->data);
}

/* =============================================================================
 * TREE STATISTICS
 * =============================================================================
 */

/* TODO 6: SOLUTION - Height */
int tree_height(TreeNode *node) {
    if (node == NULL) {
        return -1;
    }
    
    int left_height = tree_height(node->left);
    int right_height = tree_height(node->right);
    
    if (left_height > right_height) {
        return 1 + left_height;
    } else {
        return 1 + right_height;
    }
}

/* TODO 7: SOLUTION - Count nodes */
int count_nodes(TreeNode *node) {
    if (node == NULL) {
        return 0;
    }
    return 1 + count_nodes(node->left) + count_nodes(node->right);
}

/* TODO 8: SOLUTION - Count leaves */
int count_leaves(TreeNode *node) {
    if (node == NULL) {
        return 0;
    }
    if (node->left == NULL && node->right == NULL) {
        return 1;
    }
    return count_leaves(node->left) + count_leaves(node->right);
}

/* TODO 9: SOLUTION - Tree sum */
int tree_sum(TreeNode *node) {
    if (node == NULL) {
        return 0;
    }
    return node->data + tree_sum(node->left) + tree_sum(node->right);
}

/* =============================================================================
 * MEMORY MANAGEMENT
 * =============================================================================
 */

/* TODO 10: SOLUTION - Free tree */
void free_tree(TreeNode *node) {
    if (node == NULL) {
        return;
    }
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

/* =============================================================================
 * TREE CONSTRUCTION
 * =============================================================================
 */

/* TODO 11: SOLUTION - Build sample tree */
TreeNode* build_sample_tree(void) {
    TreeNode *root = create_node(50);
    
    root->left = create_node(30);
    root->right = create_node(70);
    
    root->left->left = create_node(20);
    root->left->right = create_node(40);
    
    root->right->left = create_node(60);
    root->right->right = create_node(80);
    
    return root;
}

/* =============================================================================
 * DISPLAY HELPER
 * =============================================================================
 */

void print_statistics(TreeNode *root) {
    printf("\n┌─────────────────────────┬─────────────┐\n");
    printf("│ Statistic               │ Value       │\n");
    printf("├─────────────────────────┼─────────────┤\n");
    printf("│ Tree Height             │ %-11d │\n", tree_height(root));
    printf("│ Total Nodes             │ %-11d │\n", count_nodes(root));
    printf("│ Leaf Nodes              │ %-11d │\n", count_leaves(root));
    printf("│ Sum of Values           │ %-11d │\n", tree_sum(root));
    printf("└─────────────────────────┴─────────────┘\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

/* TODO 12: SOLUTION - Main function */
int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 1: Binary Tree Construction and Traversal        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Build the tree */
    TreeNode *root = build_sample_tree();
    
    /* Print traversals */
    printf("\nTraversal Results:\n");
    printf("─────────────────────────────────────────\n");
    
    printf("Preorder:  ");
    preorder(root);
    printf("\n");
    
    printf("Inorder:   ");
    inorder(root);
    printf("\n");
    
    printf("Postorder: ");
    postorder(root);
    printf("\n");
    
    /* Print statistics */
    printf("\nTree Statistics:");
    print_statistics(root);
    
    /* Clean up */
    printf("\nFreeing tree memory...\n");
    free_tree(root);
    root = NULL;
    
    printf("Done. Run with Valgrind to verify no memory leaks.\n\n");
    
    return 0;
}
