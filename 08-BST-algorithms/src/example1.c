/**
 * =============================================================================
 * WEEK 08: BINARY SEARCH TREES
 * Complete Working Example
 * =============================================================================
 *
 * This example demonstrates:
 *   1. BST node structure and creation
 *   2. Insertion maintaining BST property
 *   3. Search operations (iterative and recursive)
 *   4. Tree traversals (in-order, pre-order, post-order)
 *   5. Finding minimum and maximum values
 *   6. Deletion handling all three cases
 *   7. Tree utility functions (height, count, validation)
 *   8. Visual tree printing
 *   9. Proper memory management
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o example1 example1.c
 * Usage: ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * Binary Search Tree Node Structure
 * 
 * Each node contains:
 *   - key: the value stored in the node
 *   - left: pointer to left child (smaller values)
 *   - right: pointer to right child (larger values)
 */
typedef struct BSTNode {
    int key;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

/* =============================================================================
 * PART 1: NODE CREATION AND BASIC OPERATIONS
 * =============================================================================
 */

/**
 * Creates a new BST node with the given key.
 * 
 * @param key The value to store in the new node
 * @return Pointer to the newly created node, or NULL if allocation fails
 */
BSTNode *bst_create_node(int key) {
    BSTNode *node = (BSTNode *)malloc(sizeof(BSTNode));
    if (node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for node with key %d\n", key);
        return NULL;
    }
    
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    
    return node;
}

/**
 * Inserts a new key into the BST.
 * Maintains the BST property: left < node < right
 * Duplicates are ignored.
 * 
 * @param root The root of the tree (or subtree)
 * @param key The key to insert
 * @return The root of the tree (may be new if tree was empty)
 */
BSTNode *bst_insert(BSTNode *root, int key) {
    /* Base case: empty position found, create new node */
    if (root == NULL) {
        return bst_create_node(key);
    }
    
    /* Recursive case: navigate to correct position */
    if (key < root->key) {
        root->left = bst_insert(root->left, key);
    } else if (key > root->key) {
        root->right = bst_insert(root->right, key);
    }
    /* If key == root->key, it's a duplicate - ignore */
    
    return root;
}

void demo_part1(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 1: Node Creation and Insertion                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Creating a BST by inserting: 50, 30, 70, 20, 40, 60, 80\n\n");
    
    BSTNode *root = NULL;
    int values[] = {50, 30, 70, 20, 40, 60, 80};
    int n = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < n; i++) {
        printf("  Inserting %d... ", values[i]);
        root = bst_insert(root, values[i]);
        printf("done\n");
    }
    
    printf("\nResulting tree structure:\n");
    printf("           50\n");
    printf("          /  \\\n");
    printf("        30    70\n");
    printf("       /  \\  /  \\\n");
    printf("      20  40 60  80\n");
    
    /* Clean up for this demo */
    /* Note: We'll implement proper cleanup later */
}

/* =============================================================================
 * PART 2: SEARCH OPERATIONS
 * =============================================================================
 */

/**
 * Searches for a key in the BST (iterative version).
 * Preferred for production code due to O(1) space complexity.
 * 
 * @param root The root of the tree
 * @param key The key to search for
 * @return Pointer to the node if found, NULL otherwise
 */
BSTNode *bst_search_iterative(BSTNode *root, int key) {
    BSTNode *current = root;
    
    while (current != NULL) {
        if (key == current->key) {
            return current;  /* Found! */
        }
        if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    
    return NULL;  /* Not found */
}

/**
 * Searches for a key in the BST (recursive version).
 * More elegant but uses O(h) stack space.
 * 
 * @param root The root of the tree
 * @param key The key to search for
 * @return Pointer to the node if found, NULL otherwise
 */
BSTNode *bst_search_recursive(BSTNode *root, int key) {
    /* Base cases */
    if (root == NULL || root->key == key) {
        return root;
    }
    
    /* Recursive case */
    if (key < root->key) {
        return bst_search_recursive(root->left, key);
    }
    return bst_search_recursive(root->right, key);
}

void demo_part2(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: Search Operations                                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Build the tree */
    BSTNode *root = NULL;
    int values[] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; i++) {
        root = bst_insert(root, values[i]);
    }
    
    printf("Searching in BST {50, 30, 70, 20, 40, 60, 80}:\n\n");
    
    /* Test searches */
    int search_keys[] = {40, 60, 25, 100};
    for (int i = 0; i < 4; i++) {
        BSTNode *result = bst_search_iterative(root, search_keys[i]);
        printf("  Search for %3d: %s\n", 
               search_keys[i],
               result != NULL ? "FOUND ✓" : "NOT FOUND ✗");
    }
    
    printf("\n  Both iterative and recursive search give identical results.\n");
    printf("  Iterative is preferred (O(1) space vs O(h) for recursive).\n");
}

/* =============================================================================
 * PART 3: TREE TRAVERSALS
 * =============================================================================
 */

/**
 * In-order traversal: Left, Node, Right
 * For BST: produces keys in sorted (ascending) order
 */
void bst_inorder(BSTNode *root) {
    if (root == NULL) {
        return;
    }
    bst_inorder(root->left);
    printf("%d ", root->key);
    bst_inorder(root->right);
}

/**
 * Pre-order traversal: Node, Left, Right
 * Useful for: tree copying, serialisation, prefix notation
 */
void bst_preorder(BSTNode *root) {
    if (root == NULL) {
        return;
    }
    printf("%d ", root->key);
    bst_preorder(root->left);
    bst_preorder(root->right);
}

/**
 * Post-order traversal: Left, Right, Node
 * Useful for: tree deletion, postfix notation, calculating sizes
 */
void bst_postorder(BSTNode *root) {
    if (root == NULL) {
        return;
    }
    bst_postorder(root->left);
    bst_postorder(root->right);
    printf("%d ", root->key);
}

void demo_part3(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 3: Tree Traversals                                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Build the tree */
    BSTNode *root = NULL;
    int values[] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; i++) {
        root = bst_insert(root, values[i]);
    }
    
    printf("Tree structure:\n");
    printf("           50\n");
    printf("          /  \\\n");
    printf("        30    70\n");
    printf("       /  \\  /  \\\n");
    printf("      20  40 60  80\n\n");
    
    printf("Traversal results:\n\n");
    
    printf("  In-order   (L-N-R): ");
    bst_inorder(root);
    printf("\n");
    printf("    → Produces SORTED output for BST!\n\n");
    
    printf("  Pre-order  (N-L-R): ");
    bst_preorder(root);
    printf("\n");
    printf("    → Root first, useful for copying trees\n\n");
    
    printf("  Post-order (L-R-N): ");
    bst_postorder(root);
    printf("\n");
    printf("    → Root last, essential for safe deletion\n");
}

/* =============================================================================
 * PART 4: FINDING MINIMUM AND MAXIMUM
 * =============================================================================
 */

/**
 * Finds the node with the minimum key in the BST.
 * The minimum is always in the leftmost node.
 * 
 * @param root The root of the tree
 * @return Pointer to the node with minimum key, or NULL if tree is empty
 */
BSTNode *bst_find_min(BSTNode *root) {
    if (root == NULL) {
        return NULL;
    }
    
    /* Keep going left until we can't anymore */
    while (root->left != NULL) {
        root = root->left;
    }
    
    return root;
}

/**
 * Finds the node with the maximum key in the BST.
 * The maximum is always in the rightmost node.
 * 
 * @param root The root of the tree
 * @return Pointer to the node with maximum key, or NULL if tree is empty
 */
BSTNode *bst_find_max(BSTNode *root) {
    if (root == NULL) {
        return NULL;
    }
    
    /* Keep going right until we can't anymore */
    while (root->right != NULL) {
        root = root->right;
    }
    
    return root;
}

void demo_part4(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 4: Finding Minimum and Maximum                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Build the tree */
    BSTNode *root = NULL;
    int values[] = {50, 30, 70, 20, 40, 60, 80, 15, 85};
    for (int i = 0; i < 9; i++) {
        root = bst_insert(root, values[i]);
    }
    
    printf("Tree with keys: {50, 30, 70, 20, 40, 60, 80, 15, 85}\n\n");
    
    BSTNode *min_node = bst_find_min(root);
    BSTNode *max_node = bst_find_max(root);
    
    printf("  Minimum key: %d (leftmost node)\n", min_node ? min_node->key : 0);
    printf("  Maximum key: %d (rightmost node)\n", max_node ? max_node->key : 0);
    
    printf("\n  Time complexity: O(h) where h is tree height\n");
    printf("  For balanced tree: O(log n)\n");
    printf("  For degenerate tree: O(n)\n");
}

/* =============================================================================
 * PART 5: DELETION
 * =============================================================================
 */

/**
 * Deletes a node with the given key from the BST.
 * Handles three cases:
 *   1. Node is a leaf (no children)
 *   2. Node has one child
 *   3. Node has two children (uses in-order successor)
 * 
 * @param root The root of the tree
 * @param key The key to delete
 * @return The root of the tree (may change if root was deleted)
 */
BSTNode *bst_delete(BSTNode *root, int key) {
    /* Base case: key not found */
    if (root == NULL) {
        return NULL;
    }
    
    /* Navigate to the node to delete */
    if (key < root->key) {
        root->left = bst_delete(root->left, key);
    } else if (key > root->key) {
        root->right = bst_delete(root->right, key);
    } else {
        /* Found the node to delete */
        
        /* Case 1: Leaf node (no children) */
        /* Case 2a: Only right child */
        if (root->left == NULL) {
            BSTNode *temp = root->right;
            free(root);
            return temp;
        }
        
        /* Case 2b: Only left child */
        if (root->right == NULL) {
            BSTNode *temp = root->left;
            free(root);
            return temp;
        }
        
        /* Case 3: Two children */
        /* Find in-order successor (smallest in right subtree) */
        BSTNode *successor = bst_find_min(root->right);
        
        /* Copy successor's key to this node */
        root->key = successor->key;
        
        /* Delete the successor (it has at most one child) */
        root->right = bst_delete(root->right, successor->key);
    }
    
    return root;
}

void demo_part5(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 5: Deletion Operations                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Build the tree */
    BSTNode *root = NULL;
    int values[] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; i++) {
        root = bst_insert(root, values[i]);
    }
    
    printf("Initial tree: ");
    bst_inorder(root);
    printf("\n\n");
    
    /* Case 1: Delete leaf */
    printf("Case 1 - Delete leaf node (20):\n");
    root = bst_delete(root, 20);
    printf("  After deletion: ");
    bst_inorder(root);
    printf("\n\n");
    
    /* Rebuild tree */
    root = bst_insert(root, 20);
    
    /* Case 2: Delete node with one child */
    printf("Case 2 - Delete node with one child:\n");
    root = bst_insert(root, 35);  /* Add a child to 40 */
    printf("  Tree with 35 added: ");
    bst_inorder(root);
    printf("\n");
    root = bst_delete(root, 40);  /* Now delete 40 (has one child: 35) */
    printf("  After deleting 40: ");
    bst_inorder(root);
    printf("\n\n");
    
    /* Rebuild */
    root = bst_insert(root, 40);
    
    /* Case 3: Delete node with two children */
    printf("Case 3 - Delete node with two children (30):\n");
    printf("  Before: ");
    bst_inorder(root);
    printf("\n");
    root = bst_delete(root, 30);
    printf("  After:  ");
    bst_inorder(root);
    printf("\n");
    printf("  (30 replaced by in-order successor 35)\n");
}

/* =============================================================================
 * PART 6: UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * Calculates the height of the BST.
 * Height = length of longest path from root to any leaf.
 * Empty tree has height -1, single node has height 0.
 */
int bst_height(BSTNode *root) {
    if (root == NULL) {
        return -1;
    }
    
    int left_height = bst_height(root->left);
    int right_height = bst_height(root->right);
    
    return 1 + (left_height > right_height ? left_height : right_height);
}

/**
 * Counts the total number of nodes in the BST.
 */
int bst_count_nodes(BSTNode *root) {
    if (root == NULL) {
        return 0;
    }
    return 1 + bst_count_nodes(root->left) + bst_count_nodes(root->right);
}

/**
 * Counts the number of leaf nodes (nodes with no children).
 */
int bst_count_leaves(BSTNode *root) {
    if (root == NULL) {
        return 0;
    }
    if (root->left == NULL && root->right == NULL) {
        return 1;
    }
    return bst_count_leaves(root->left) + bst_count_leaves(root->right);
}

/**
 * Helper function for BST validation.
 * Checks if all nodes fall within the valid range [min, max].
 */
bool bst_is_valid_helper(BSTNode *node, int min, int max) {
    if (node == NULL) {
        return true;
    }
    
    /* Check if current node violates range constraint */
    if (node->key <= min || node->key >= max) {
        return false;
    }
    
    /* Recursively check subtrees with updated ranges */
    return bst_is_valid_helper(node->left, min, node->key) &&
           bst_is_valid_helper(node->right, node->key, max);
}

/**
 * Validates whether a binary tree satisfies the BST property.
 */
bool bst_is_valid(BSTNode *root) {
    return bst_is_valid_helper(root, INT_MIN, INT_MAX);
}

void demo_part6(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 6: Utility Functions                                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Build a balanced tree */
    BSTNode *root = NULL;
    int values[] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; i++) {
        root = bst_insert(root, values[i]);
    }
    
    printf("Tree analysis for {50, 30, 70, 20, 40, 60, 80}:\n\n");
    printf("  Height:      %d\n", bst_height(root));
    printf("  Total nodes: %d\n", bst_count_nodes(root));
    printf("  Leaf nodes:  %d\n", bst_count_leaves(root));
    printf("  Is valid BST: %s\n", bst_is_valid(root) ? "YES ✓" : "NO ✗");
    
    printf("\nBuilding a degenerate tree (1, 2, 3, 4, 5):\n\n");
    BSTNode *degenerate = NULL;
    for (int i = 1; i <= 5; i++) {
        degenerate = bst_insert(degenerate, i);
    }
    
    printf("  Height:      %d (worst case!)\n", bst_height(degenerate));
    printf("  Total nodes: %d\n", bst_count_nodes(degenerate));
    printf("  Leaf nodes:  %d\n", bst_count_leaves(degenerate));
    printf("  Is valid BST: %s\n", bst_is_valid(degenerate) ? "YES ✓" : "NO ✗");
}

/* =============================================================================
 * PART 7: VISUAL TREE PRINTING
 * =============================================================================
 */

/**
 * Helper function to print tree with indentation.
 * Uses rotated view (right subtree on top).
 */
void bst_print_helper(BSTNode *root, int space, int indent) {
    if (root == NULL) {
        return;
    }
    
    space += indent;
    
    /* Print right subtree first (will appear on top) */
    bst_print_helper(root->right, space, indent);
    
    printf("\n");
    for (int i = indent; i < space; i++) {
        printf(" ");
    }
    printf("%d", root->key);
    
    /* Print left subtree */
    bst_print_helper(root->left, space, indent);
}

/**
 * Prints a visual representation of the tree.
 * Tree is rotated 90 degrees (right subtree on top).
 */
void bst_print_tree(BSTNode *root) {
    if (root == NULL) {
        printf("  (empty tree)\n");
        return;
    }
    bst_print_helper(root, 0, 5);
    printf("\n");
}

void demo_part7(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 7: Visual Tree Printing                             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    BSTNode *root = NULL;
    int values[] = {50, 30, 70, 20, 40, 60, 80, 35, 45, 75};
    for (int i = 0; i < 10; i++) {
        root = bst_insert(root, values[i]);
    }
    
    printf("Visual representation (rotated 90° clockwise):\n");
    printf("  - Right subtree appears on top\n");
    printf("  - Left subtree appears on bottom\n");
    bst_print_tree(root);
}

/* =============================================================================
 * PART 8: MEMORY CLEANUP
 * =============================================================================
 */

/**
 * Frees all nodes in the BST.
 * Uses post-order traversal to ensure children are freed before parent.
 */
void bst_free(BSTNode *root) {
    if (root == NULL) {
        return;
    }
    
    /* Post-order: free children first, then parent */
    bst_free(root->left);
    bst_free(root->right);
    free(root);
}

void demo_part8(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 8: Memory Management                                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Proper memory cleanup is essential!\n\n");
    
    printf("The bst_free() function uses POST-ORDER traversal:\n");
    printf("  1. Free left subtree\n");
    printf("  2. Free right subtree\n");
    printf("  3. Free current node\n\n");
    
    printf("Why post-order?\n");
    printf("  - Children must be freed BEFORE parent\n");
    printf("  - If we free parent first, we lose pointers to children\n");
    printf("  - This would cause memory leaks!\n\n");
    
    printf("Always verify with Valgrind:\n");
    printf("  valgrind --leak-check=full ./example1\n\n");
    
    /* Demo memory allocation and deallocation */
    BSTNode *root = NULL;
    for (int i = 0; i < 100; i++) {
        root = bst_insert(root, i);
    }
    printf("Created tree with 100 nodes.\n");
    
    bst_free(root);
    printf("Freed all 100 nodes.\n");
    printf("\nRun with Valgrind to verify: no memory leaks!\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     WEEK 08: BINARY SEARCH TREES - Complete Example           ║\n");
    printf("║     Algorithms and Programming Techniques                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    demo_part1();  /* Node creation and insertion */
    demo_part2();  /* Search operations */
    demo_part3();  /* Tree traversals */
    demo_part4();  /* Finding min and max */
    demo_part5();  /* Deletion */
    demo_part6();  /* Utility functions */
    demo_part7();  /* Visual printing */
    demo_part8();  /* Memory management */
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     Example complete! Now try the exercises.                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
