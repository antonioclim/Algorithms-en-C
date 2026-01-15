/**
 * =============================================================================
 * HOMEWORK 2 - SOLUTION: Lowest Common Ancestor (LCA)
 * =============================================================================
 *
 * INSTRUCTOR COPY - Complete working solution
 *
 * Implements:
 *   - find_lca(): Find lowest common ancestor of two nodes
 *   - path_to_node(): Find path from root to a node
 *   - print_path(): Print path between two nodes
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_PATH 100

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
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

void free_tree(TreeNode *node) {
    if (node == NULL) return;
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

/* =============================================================================
 * HELPER FUNCTIONS
 * =============================================================================
 */

/**
 * Check if a value exists in the tree.
 */
bool exists_in_tree(TreeNode *root, int value) {
    if (root == NULL) return false;
    if (root->data == value) return true;
    return exists_in_tree(root->left, value) || 
           exists_in_tree(root->right, value);
}

/* =============================================================================
 * LOWEST COMMON ANCESTOR
 * =============================================================================
 */

/**
 * Find the Lowest Common Ancestor of two nodes.
 *
 * Algorithm:
 *   - If current node is NULL, return NULL
 *   - If current node matches either p or q, return current node
 *   - Recursively search left and right subtrees
 *   - If both subtrees return non-NULL, current node is LCA
 *   - Otherwise, return whichever subtree result is non-NULL
 *
 * @param root Root of the tree
 * @param p First node value
 * @param q Second node value
 * @return Pointer to LCA node, or NULL if not found
 */
TreeNode* find_lca(TreeNode *root, int p, int q) {
    /* Base case: empty tree */
    if (root == NULL) {
        return NULL;
    }
    
    /* If current node is one of the targets, it could be LCA */
    if (root->data == p || root->data == q) {
        return root;
    }
    
    /* Recursively search left and right subtrees */
    TreeNode *left_lca = find_lca(root->left, p, q);
    TreeNode *right_lca = find_lca(root->right, p, q);
    
    /* If both subtrees found a target, current node is LCA */
    if (left_lca != NULL && right_lca != NULL) {
        return root;
    }
    
    /* Return whichever side found something (or NULL if neither) */
    return (left_lca != NULL) ? left_lca : right_lca;
}

/**
 * Safe LCA that verifies both nodes exist.
 */
TreeNode* find_lca_safe(TreeNode *root, int p, int q) {
    /* First verify both values exist in tree */
    if (!exists_in_tree(root, p) || !exists_in_tree(root, q)) {
        return NULL;
    }
    return find_lca(root, p, q);
}

/* =============================================================================
 * PATH FINDING
 * =============================================================================
 */

/**
 * Find the path from root to a node with given value.
 *
 * @param root Root of the tree
 * @param target Value to find
 * @param path Array to store path (must be pre-allocated)
 * @param len Pointer to store path length
 * @return true if found, false otherwise
 */
bool path_to_node(TreeNode *root, int target, int *path, int *len) {
    /* Base case: empty tree */
    if (root == NULL) {
        return false;
    }
    
    /* Add current node to path */
    path[*len] = root->data;
    (*len)++;
    
    /* Check if we found the target */
    if (root->data == target) {
        return true;
    }
    
    /* Search in left and right subtrees */
    if (path_to_node(root->left, target, path, len) ||
        path_to_node(root->right, target, path, len)) {
        return true;
    }
    
    /* Target not found in this path, backtrack */
    (*len)--;
    return false;
}

/* =============================================================================
 * PATH PRINTING
 * =============================================================================
 */

/**
 * Print the path from node p to node q through their LCA.
 *
 * @param root Root of the tree
 * @param p Start node value
 * @param q End node value
 */
void print_path(TreeNode *root, int p, int q) {
    /* Find paths to both nodes */
    int path_p[MAX_PATH], path_q[MAX_PATH];
    int len_p = 0, len_q = 0;
    
    if (!path_to_node(root, p, path_p, &len_p)) {
        printf("Node %d not found in tree\n", p);
        return;
    }
    
    /* Reset for second path (need to re-find) */
    len_q = 0;
    if (!path_to_node(root, q, path_q, &len_q)) {
        printf("Node %d not found in tree\n", q);
        return;
    }
    
    /* Find LCA index in both paths */
    int lca_idx_p = -1, lca_idx_q = -1;
    
    for (int i = 0; i < len_p && lca_idx_p < 0; i++) {
        for (int j = 0; j < len_q; j++) {
            if (path_p[i] == path_q[j]) {
                /* Found common ancestor, keep searching for deepest */
                lca_idx_p = i;
                lca_idx_q = j;
            }
        }
    }
    
    /* Print path: p to LCA (reversed), then LCA to q */
    printf("Path from %d to %d: ", p, q);
    
    /* Print from p up to LCA */
    for (int i = len_p - 1; i > lca_idx_p; i--) {
        printf("%d -> ", path_p[i]);
    }
    
    /* Print from LCA down to q */
    for (int i = lca_idx_q; i < len_q - 1; i++) {
        printf("%d -> ", path_q[i]);
    }
    printf("%d\n", path_q[len_q - 1]);
}

/* =============================================================================
 * TEST TREE CONSTRUCTION
 * =============================================================================
 */

/**
 * Build example tree:
 *         3
 *        / \
 *       5   1
 *      / \ / \
 *     6  2 0  8
 *       / \
 *      7   4
 */
TreeNode* build_example_tree(void) {
    TreeNode *root = create_node(3);
    
    root->left = create_node(5);
    root->right = create_node(1);
    
    root->left->left = create_node(6);
    root->left->right = create_node(2);
    root->right->left = create_node(0);
    root->right->right = create_node(8);
    
    root->left->right->left = create_node(7);
    root->left->right->right = create_node(4);
    
    return root;
}

void print_tree_structure(void) {
    printf("Tree Structure:\n");
    printf("        3\n");
    printf("       / \\\n");
    printf("      5   1\n");
    printf("     / \\ / \\\n");
    printf("    6  2 0  8\n");
    printf("      / \\\n");
    printf("     7   4\n\n");
}

/* =============================================================================
 * TEST CASES
 * =============================================================================
 */

void run_tests(void) {
    TreeNode *root = build_example_tree();
    print_tree_structure();
    
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("LCA Tests\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    /* Test 1: Nodes in different subtrees */
    TreeNode *lca1 = find_lca_safe(root, 5, 1);
    printf("LCA(5, 1) = %d (expected: 3) %s\n", 
           lca1 ? lca1->data : -1,
           (lca1 && lca1->data == 3) ? "✓" : "✗");
    
    /* Test 2: One node is ancestor of other */
    TreeNode *lca2 = find_lca_safe(root, 5, 4);
    printf("LCA(5, 4) = %d (expected: 5) %s\n", 
           lca2 ? lca2->data : -1,
           (lca2 && lca2->data == 5) ? "✓" : "✗");
    
    /* Test 3: Nodes in same subtree */
    TreeNode *lca3 = find_lca_safe(root, 6, 4);
    printf("LCA(6, 4) = %d (expected: 5) %s\n", 
           lca3 ? lca3->data : -1,
           (lca3 && lca3->data == 5) ? "✓" : "✗");
    
    /* Test 4: Same node */
    TreeNode *lca4 = find_lca_safe(root, 2, 2);
    printf("LCA(2, 2) = %d (expected: 2) %s\n", 
           lca4 ? lca4->data : -1,
           (lca4 && lca4->data == 2) ? "✓" : "✗");
    
    /* Test 5: Deep nodes */
    TreeNode *lca5 = find_lca_safe(root, 7, 4);
    printf("LCA(7, 4) = %d (expected: 2) %s\n", 
           lca5 ? lca5->data : -1,
           (lca5 && lca5->data == 2) ? "✓" : "✗");
    
    /* Test 6: Root with leaf */
    TreeNode *lca6 = find_lca_safe(root, 3, 8);
    printf("LCA(3, 8) = %d (expected: 3) %s\n", 
           lca6 ? lca6->data : -1,
           (lca6 && lca6->data == 3) ? "✓" : "✗");
    
    /* Test 7: Non-existent node */
    TreeNode *lca7 = find_lca_safe(root, 5, 99);
    printf("LCA(5, 99) = %s (expected: NULL) %s\n", 
           lca7 ? "non-NULL" : "NULL",
           (lca7 == NULL) ? "✓" : "✗");
    
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Path Tests\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    /* Path tests */
    print_path(root, 6, 4);    /* Expected: 6 -> 5 -> 2 -> 4 */
    print_path(root, 7, 8);    /* Expected: 7 -> 2 -> 5 -> 3 -> 1 -> 8 */
    print_path(root, 5, 1);    /* Expected: 5 -> 3 -> 1 */
    print_path(root, 0, 8);    /* Expected: 0 -> 1 -> 8 */
    
    printf("\n");
    
    free_tree(root);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     HOMEWORK 2 SOLUTION: Lowest Common Ancestor               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    run_tests();
    
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("All tests completed.\n");
    printf("Run with Valgrind to verify no memory leaks.\n\n");
    
    return 0;
}
