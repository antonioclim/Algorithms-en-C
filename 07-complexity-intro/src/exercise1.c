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
 * TODO 1: Complete the Binary Tree Node structure
 *
 * A binary tree node should contain:
 *   - An integer data field to store the value
 *   - A pointer to the left child node
 *   - A pointer to the right child node
 *
 * Hint: Use 'struct TreeNode' for the self-referential pointers
 */
typedef struct TreeNode {
    /* YOUR CODE HERE */
    
    
    
} TreeNode;

/* =============================================================================
 * NODE CREATION
 * =============================================================================
 */

/**
 * TODO 2: Implement node creation function
 *
 * This function should:
 *   1. Allocate memory for a new TreeNode using malloc
 *   2. Check if allocation succeeded (exit if not)
 *   3. Initialise data field with the given value
 *   4. Set both child pointers to NULL
 *   5. Return the pointer to the new node
 *
 * @param value The integer value to store in the node
 * @return Pointer to the newly created node
 *
 * Hint: sizeof(TreeNode) gives the size needed for allocation
 */
TreeNode* create_node(int value) {
    /* YOUR CODE HERE */
    
    
    
    
    
    return NULL;  /* Replace this */
}

/* =============================================================================
 * TRAVERSAL FUNCTIONS
 * =============================================================================
 */

/**
 * TODO 3: Implement Preorder traversal
 *
 * Preorder visits nodes in the order: Node -> Left -> Right
 *
 * Steps:
 *   1. Check if node is NULL (base case - just return)
 *   2. Print the current node's data followed by a space
 *   3. Recursively traverse the left subtree
 *   4. Recursively traverse the right subtree
 *
 * @param node The current node being visited
 */
void preorder(TreeNode *node) {
    /* YOUR CODE HERE */
    
    
    
    
}

/**
 * TODO 4: Implement Inorder traversal
 *
 * Inorder visits nodes in the order: Left -> Node -> Right
 *
 * Steps:
 *   1. Check if node is NULL (base case - just return)
 *   2. Recursively traverse the left subtree
 *   3. Print the current node's data followed by a space
 *   4. Recursively traverse the right subtree
 *
 * @param node The current node being visited
 */
void inorder(TreeNode *node) {
    /* YOUR CODE HERE */
    
    
    
    
}

/**
 * TODO 5: Implement Postorder traversal
 *
 * Postorder visits nodes in the order: Left -> Right -> Node
 *
 * Steps:
 *   1. Check if node is NULL (base case - just return)
 *   2. Recursively traverse the left subtree
 *   3. Recursively traverse the right subtree
 *   4. Print the current node's data followed by a space
 *
 * @param node The current node being visited
 */
void postorder(TreeNode *node) {
    /* YOUR CODE HERE */
    
    
    
    
}

/* =============================================================================
 * TREE STATISTICS
 * =============================================================================
 */

/**
 * TODO 6: Implement tree height calculation
 *
 * The height of a tree is the longest path from root to any leaf.
 * Convention: empty tree (NULL) has height -1
 *
 * Steps:
 *   1. Base case: if node is NULL, return -1
 *   2. Recursively calculate height of left subtree
 *   3. Recursively calculate height of right subtree
 *   4. Return 1 + maximum of the two heights
 *
 * @param node The root of the tree/subtree
 * @return Height of the tree
 *
 * Hint: Use a conditional expression or if-else to find maximum
 */
int tree_height(TreeNode *node) {
    /* YOUR CODE HERE */
    
    
    
    
    
    return 0;  /* Replace this */
}

/**
 * TODO 7: Implement total node counting
 *
 * Count all nodes in the tree.
 *
 * Steps:
 *   1. Base case: if node is NULL, return 0
 *   2. Return 1 (for current node) plus count of left subtree
 *      plus count of right subtree
 *
 * @param node The root of the tree/subtree
 * @return Total number of nodes
 */
int count_nodes(TreeNode *node) {
    /* YOUR CODE HERE */
    
    
    
    return 0;  /* Replace this */
}

/**
 * TODO 8: Implement leaf node counting
 *
 * A leaf is a node with NO children (both left and right are NULL).
 *
 * Steps:
 *   1. Base case: if node is NULL, return 0
 *   2. If both children are NULL, return 1 (it's a leaf)
 *   3. Otherwise, return sum of leaves in left and right subtrees
 *
 * @param node The root of the tree/subtree
 * @return Number of leaf nodes
 */
int count_leaves(TreeNode *node) {
    /* YOUR CODE HERE */
    
    
    
    
    
    return 0;  /* Replace this */
}

/**
 * TODO 9: Implement sum of all node values
 *
 * Calculate the sum of all data values in the tree.
 *
 * Steps:
 *   1. Base case: if node is NULL, return 0
 *   2. Return current node's data + sum of left subtree + sum of right subtree
 *
 * @param node The root of the tree/subtree
 * @return Sum of all values
 */
int tree_sum(TreeNode *node) {
    /* YOUR CODE HERE */
    
    
    
    return 0;  /* Replace this */
}

/* =============================================================================
 * MEMORY MANAGEMENT
 * =============================================================================
 */

/**
 * TODO 10: Implement tree deallocation (CRITICAL!)
 *
 * Free all memory allocated for the tree.
 * MUST use POSTORDER traversal - children must be freed before parent!
 *
 * Steps:
 *   1. Base case: if node is NULL, return
 *   2. Recursively free the left subtree
 *   3. Recursively free the right subtree
 *   4. Free the current node
 *
 * @param node The root of the tree to free
 *
 * WARNING: Freeing parent before children causes memory leaks!
 */
void free_tree(TreeNode *node) {
    /* YOUR CODE HERE */
    
    
    
    
}

/* =============================================================================
 * TREE CONSTRUCTION HELPER
 * =============================================================================
 */

/**
 * TODO 11: Build a sample tree for testing
 *
 * Construct the following tree structure:
 *
 *              50
 *            /    \
 *           30     70
 *          / \    /  \
 *         20  40 60   80
 *
 * Steps:
 *   1. Create the root node with value 50
 *   2. Create and attach left child (30) to root
 *   3. Create and attach right child (70) to root
 *   4. Create and attach children to node 30 (20 and 40)
 *   5. Create and attach children to node 70 (60 and 80)
 *   6. Return the root pointer
 *
 * @return Pointer to the root of the constructed tree
 */
TreeNode* build_sample_tree(void) {
    /* YOUR CODE HERE */
    
    
    
    
    
    
    
    
    return NULL;  /* Replace this */
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
    
    /**
     * TODO 12: Complete the main function
     *
     * Steps:
     *   1. Call build_sample_tree() to create the tree
     *   2. Print the traversal results (preorder, inorder, postorder)
     *   3. Print the statistics using print_statistics()
     *   4. Free the tree using free_tree()
     *   5. Set root to NULL after freeing
     */
    
    /* YOUR CODE HERE - Build the tree */
    TreeNode *root = NULL;  /* Replace NULL with build_sample_tree() call */
    
    
    /* Print traversals */
    printf("\nTraversal Results:\n");
    printf("─────────────────────────────────────────\n");
    
    printf("Preorder:  ");
    /* YOUR CODE HERE - Call preorder(root) */
    
    printf("\n");
    
    printf("Inorder:   ");
    /* YOUR CODE HERE - Call inorder(root) */
    
    printf("\n");
    
    printf("Postorder: ");
    /* YOUR CODE HERE - Call postorder(root) */
    
    printf("\n");
    
    /* Print statistics */
    printf("\nTree Statistics:");
    print_statistics(root);
    
    /* Clean up */
    printf("\nFreeing tree memory...\n");
    /* YOUR CODE HERE - Free the tree and set root to NULL */
    
    
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
