/**
 * =============================================================================
 * EXERCISE 1: Binary Tree Construction and Traversal
 * =============================================================================
 *
 * OBJECTIVE:
 *   Implement a complete binary tree with traversals, statistics and
 *   memory management functions.
 *
 * REQUIREMENTS:
 *   1. Complete the TreeNode structure definition
 *   2. Implement create_node() for dynamic node allocation
 *   3. Implement all three traversal functions
 *   4. Implement tree statistics functions
 *   5. Implement proper memory deallocation
 *
 * EXAMPLE INPUT:
 *   Tree values: 50, 30, 70, 20, 40, 60, 80
 *
 * EXPECTED OUTPUT:
 *   Preorder:  50 30 20 40 70 60 80
 *   Inorder:   20 30 40 50 60 70 80
 *   Postorder: 20 40 30 60 80 70 50
 *   Height: 2
 *   Nodes: 7
 *   Leaves: 4
 *   Sum: 350
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
 *
 * =============================================================================
 * Algorithms and Programming Techniques
 * Week 07: Binary Trees
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * Binary Tree Node Structure
 *
 * A binary tree node contains:
 *   - an integer payload
 *   - a pointer to the left child (NULL if absent)
 *   - a pointer to the right child (NULL if absent)
 */
typedef struct TreeNode {
    int data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

/* =============================================================================
 * NODE CREATION
 * =============================================================================
 */

/**
 * Create and initialise a new node.
 */
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

/**
 * Preorder traversal: Node -> Left -> Right
 */
void preorder(TreeNode *node) {
    if (node == NULL) {
        return;
    }

    printf("%d ", node->data);
    preorder(node->left);
    preorder(node->right);
}

/**
 * Inorder traversal: Left -> Node -> Right
 */
void inorder(TreeNode *node) {
    if (node == NULL) {
        return;
    }

    inorder(node->left);
    printf("%d ", node->data);
    inorder(node->right);
}

/**
 * Postorder traversal: Left -> Right -> Node
 */
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

/**
 * Height of a (sub)tree.
 *
 * Convention: the empty tree has height -1.
 */
int tree_height(TreeNode *node) {
    if (node == NULL) {
        return -1;
    }

    int left_height = tree_height(node->left);
    int right_height = tree_height(node->right);

    return 1 + ((left_height > right_height) ? left_height : right_height);
}

/**
 * Count total nodes in the tree.
 */
int count_nodes(TreeNode *node) {
    if (node == NULL) {
        return 0;
    }

    return 1 + count_nodes(node->left) + count_nodes(node->right);
}

/**
 * Count leaf nodes (nodes with no children).
 */
int count_leaves(TreeNode *node) {
    if (node == NULL) {
        return 0;
    }

    if (node->left == NULL && node->right == NULL) {
        return 1;
    }

    return count_leaves(node->left) + count_leaves(node->right);
}

/**
 * Sum of all node values.
 */
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

/**
 * Free all nodes using postorder discipline.
 */
void free_tree(TreeNode *node) {
    if (node == NULL) {
        return;
    }

    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

/* =============================================================================
 * TREE CONSTRUCTION HELPER
 * =============================================================================
 */

/**
 * Build the sample tree required by the tests.
 *
 *              50
 *            /    \
 *           30     70
 *          / \    /  \
 *         20  40 60   80
 */
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

/**
 * Print tree statistics in a formatted table.
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

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Implement count_internal() to count nodes that have at least one child
 *
 * 2. Implement tree_search(root, value) to find a node by value
 *
 * 3. Implement tree_contains(root, value) returning true/false
 *
 * 4. Implement tree_min() and tree_max() to find extreme values
 *
 * 5. Implement print_tree_rotated(root, level) for visual tree display
 *
 * =============================================================================
 */
