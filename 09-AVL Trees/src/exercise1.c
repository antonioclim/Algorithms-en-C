/**
 * =============================================================================
 * EXERCISE 1: AVL Tree Foundation
 * =============================================================================
 *
 * OBJECTIVE:
 *   Implement a basic AVL tree with insertion and all four rotation types.
 *   This exercise focuses on understanding the core balancing mechanism.
 *
 * REQUIREMENTS:
 *   1. Complete the AVLNode structure definition
 *   2. Implement height() and balance_factor() utility functions
 *   3. Implement right rotation (rotate_right)
 *   4. Implement left rotation (rotate_left)
 *   5. Implement the rebalance() function
 *   6. Implement avl_insert() with automatic rebalancing
 *   7. Implement inorder traversal to verify BST property
 *   8. Test with sequences that trigger all four rotation cases
 *
 * EXAMPLE INPUT:
 *   Insert sequence: 30, 20, 10 (triggers LL case)
 *   Insert sequence: 10, 20, 30 (triggers RR case)
 *   Insert sequence: 30, 10, 20 (triggers LR case)
 *   Insert sequence: 10, 30, 20 (triggers RL case)
 *
 * EXPECTED OUTPUT:
 *   After each insertion sequence, tree should be balanced with:
 *   - All balance factors in {-1, 0, +1}
 *   - Inorder traversal produces sorted output
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
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

/**
 * TODO 1: Complete the AVLNode structure
 *
 * An AVL node should contain:
 * - An integer key
 * - An integer height (height of subtree rooted at this node)
 * - A pointer to the left child
 * - A pointer to the right child
 *
 * Hint: Use 'struct AVLNode *' for the child pointers
 */
typedef struct AVLNode {
    /* YOUR CODE HERE */
    
} AVLNode;

/* =============================================================================
 * UTILITY FUNCTIONS
 * =============================================================================
 */

/**
 * TODO 2: Implement the height function
 *
 * Returns the height of the given node.
 * Convention: height(NULL) = -1, height(leaf) = 0
 *
 * @param node Pointer to the node (may be NULL)
 * @return Height of the node, or -1 if NULL
 *
 * Hint: This is a simple function - just handle the NULL case
 *       and return the stored height otherwise.
 */
int height(AVLNode *node) {
    /* YOUR CODE HERE */
    return 0;  /* Replace this */
}

/**
 * TODO 3: Implement the balance_factor function
 *
 * Calculates the balance factor of a node.
 * balance_factor = height(left) - height(right)
 *
 * @param node Pointer to the node (may be NULL)
 * @return Balance factor, or 0 if NULL
 *
 * Valid AVL balance factors: {-1, 0, +1}
 * +2 means left-heavy (needs right rotation)
 * -2 means right-heavy (needs left rotation)
 */
int balance_factor(AVLNode *node) {
    /* YOUR CODE HERE */
    return 0;  /* Replace this */
}

/**
 * Helper function to update the height of a node
 * (Provided for you)
 */
void update_height(AVLNode *node) {
    if (node == NULL) return;
    int left_h = height(node->left);
    int right_h = height(node->right);
    node->height = 1 + (left_h > right_h ? left_h : right_h);
}

/**
 * Helper function to create a new node
 * (Provided for you)
 */
AVLNode *create_node(int key) {
    AVLNode *node = (AVLNode *)malloc(sizeof(AVLNode));
    if (node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    node->key = key;
    node->height = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* =============================================================================
 * ROTATIONS
 * =============================================================================
 */

/**
 * TODO 4: Implement right rotation (for LL case)
 *
 * Performs a right rotation on the given node.
 * 
 *       y                x
 *      / \              / \
 *     x   C    --->    A   y
 *    / \                  / \
 *   A   B                B   C
 *
 * @param y The node to rotate (becomes right child)
 * @return The new root of the subtree (x)
 *
 * Steps:
 *   1. Save y's left child as x (x will become new root)
 *   2. Save x's right child as B (B will move)
 *   3. Make y the right child of x
 *   4. Make B the left child of y
 *   5. Update heights (y first, then x - order matters!)
 *   6. Return x (new root)
 */
AVLNode *rotate_right(AVLNode *y) {
    printf("  Performing RIGHT rotation on %d\n", y->key);
    
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace with new root */
}

/**
 * TODO 5: Implement left rotation (for RR case)
 *
 * Performs a left rotation on the given node.
 *
 *     x                    y
 *    / \                  / \
 *   A   y      --->      x   C
 *      / \              / \
 *     B   C            A   B
 *
 * @param x The node to rotate (becomes left child)
 * @return The new root of the subtree (y)
 *
 * Steps:
 *   1. Save x's right child as y (y will become new root)
 *   2. Save y's left child as B (B will move)
 *   3. Make x the left child of y
 *   4. Make B the right child of x
 *   5. Update heights (x first, then y - order matters!)
 *   6. Return y (new root)
 */
AVLNode *rotate_left(AVLNode *x) {
    printf("  Performing LEFT rotation on %d\n", x->key);
    
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace with new root */
}

/* =============================================================================
 * REBALANCING
 * =============================================================================
 */

/**
 * TODO 6: Implement the rebalance function
 *
 * Checks if a node is unbalanced and performs the appropriate rotation(s).
 *
 * @param node The node to check and potentially rebalance
 * @return The root of the (potentially rotated) subtree
 *
 * Cases:
 *   - bf > 1 (left-heavy):
 *       - If left child bf < 0: LR case (left rotate left child first)
 *       - Otherwise: LL case
 *       - Then right rotate the node
 *
 *   - bf < -1 (right-heavy):
 *       - If right child bf > 0: RL case (right rotate right child first)
 *       - Otherwise: RR case
 *       - Then left rotate the node
 *
 * Steps:
 *   1. Handle NULL case
 *   2. Update height of current node
 *   3. Calculate balance factor
 *   4. If bf > 1: handle left-heavy case
 *   5. If bf < -1: handle right-heavy case
 *   6. Return node (possibly rotated)
 */
AVLNode *rebalance(AVLNode *node) {
    /* YOUR CODE HERE */
    
    return node;  /* Replace with proper implementation */
}

/* =============================================================================
 * INSERTION
 * =============================================================================
 */

/**
 * TODO 7: Implement AVL insertion
 *
 * Inserts a key into the AVL tree, maintaining balance.
 *
 * @param node Current root of subtree
 * @param key Key to insert
 * @return New root of subtree (may change due to rotations)
 *
 * Steps:
 *   1. If node is NULL, create and return new node
 *   2. If key < node->key, recursively insert into left subtree
 *   3. If key > node->key, recursively insert into right subtree
 *   4. If key == node->key, it's a duplicate - just return node
 *   5. Rebalance and return
 *
 * IMPORTANT: Always capture the return value when recursing!
 *   node->left = avl_insert(node->left, key);  // Correct
 *   avl_insert(node->left, key);               // WRONG!
 */
AVLNode *avl_insert(AVLNode *node, int key) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace with proper implementation */
}

/* =============================================================================
 * TRAVERSAL
 * =============================================================================
 */

/**
 * TODO 8: Implement inorder traversal
 *
 * Prints all keys in sorted order (Left, Root, Right)
 *
 * @param node Root of subtree to traverse
 *
 * Hint: Recursively traverse left, print current, traverse right
 */
void avl_inorder(AVLNode *node) {
    /* YOUR CODE HERE */
}

/* =============================================================================
 * HELPER FUNCTIONS (PROVIDED)
 * =============================================================================
 */

/**
 * Print tree structure for debugging
 */
void print_tree_helper(AVLNode *node, int depth, char prefix) {
    if (node == NULL) return;
    
    print_tree_helper(node->right, depth + 1, '/');
    
    for (int i = 0; i < depth; i++) printf("    ");
    printf("%c--[%d](h=%d,bf=%d)\n", 
           prefix, node->key, node->height, balance_factor(node));
    
    print_tree_helper(node->left, depth + 1, '\\');
}

void print_tree(AVLNode *root) {
    printf("\nTree structure:\n");
    if (root == NULL) {
        printf("  (empty)\n");
    } else {
        print_tree_helper(root, 0, '-');
    }
    printf("\n");
}

/**
 * Free all nodes in the tree
 */
void avl_destroy(AVLNode *node) {
    if (node == NULL) return;
    avl_destroy(node->left);
    avl_destroy(node->right);
    free(node);
}

/**
 * Validate AVL properties
 */
bool validate_avl(AVLNode *node) {
    if (node == NULL) return true;
    
    int bf = balance_factor(node);
    if (bf < -1 || bf > 1) {
        printf("ERROR: Node %d has invalid balance factor %d\n", node->key, bf);
        return false;
    }
    
    if (node->left && node->left->key >= node->key) {
        printf("ERROR: BST property violated at node %d\n", node->key);
        return false;
    }
    
    if (node->right && node->right->key <= node->key) {
        printf("ERROR: BST property violated at node %d\n", node->key);
        return false;
    }
    
    return validate_avl(node->left) && validate_avl(node->right);
}

/* =============================================================================
 * MAIN PROGRAM - TEST CASES
 * =============================================================================
 */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           EXERCISE 1: AVL Tree Foundation                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    AVLNode *root = NULL;
    
    /* Test Case 1: LL Case (Right Rotation) */
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("TEST 1: LL Case - Insert 30, 20, 10\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    root = NULL;
    root = avl_insert(root, 30);
    root = avl_insert(root, 20);
    root = avl_insert(root, 10);
    
    print_tree(root);
    printf("Inorder: ");
    avl_inorder(root);
    printf("\nValid AVL: %s\n", validate_avl(root) ? "YES" : "NO");
    avl_destroy(root);
    
    /* Test Case 2: RR Case (Left Rotation) */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("TEST 2: RR Case - Insert 10, 20, 30\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    root = NULL;
    root = avl_insert(root, 10);
    root = avl_insert(root, 20);
    root = avl_insert(root, 30);
    
    print_tree(root);
    printf("Inorder: ");
    avl_inorder(root);
    printf("\nValid AVL: %s\n", validate_avl(root) ? "YES" : "NO");
    avl_destroy(root);
    
    /* Test Case 3: LR Case (Left-Right Double Rotation) */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("TEST 3: LR Case - Insert 30, 10, 20\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    root = NULL;
    root = avl_insert(root, 30);
    root = avl_insert(root, 10);
    root = avl_insert(root, 20);
    
    print_tree(root);
    printf("Inorder: ");
    avl_inorder(root);
    printf("\nValid AVL: %s\n", validate_avl(root) ? "YES" : "NO");
    avl_destroy(root);
    
    /* Test Case 4: RL Case (Right-Left Double Rotation) */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("TEST 4: RL Case - Insert 10, 30, 20\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    root = NULL;
    root = avl_insert(root, 10);
    root = avl_insert(root, 30);
    root = avl_insert(root, 20);
    
    print_tree(root);
    printf("Inorder: ");
    avl_inorder(root);
    printf("\nValid AVL: %s\n", validate_avl(root) ? "YES" : "NO");
    avl_destroy(root);
    
    /* Test Case 5: Larger tree */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("TEST 5: Larger tree - Insert 50, 30, 70, 20, 40, 60, 80\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    root = NULL;
    int values[] = {50, 30, 70, 20, 40, 60, 80};
    int n = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < n; i++) {
        printf("Inserting %d...\n", values[i]);
        root = avl_insert(root, values[i]);
    }
    
    print_tree(root);
    printf("Inorder: ");
    avl_inorder(root);
    printf("\nValid AVL: %s\n", validate_avl(root) ? "YES" : "NO");
    avl_destroy(root);
    
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    Exercise Complete                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    return 0;
}

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Add a counter to track the number of rotations performed
 *
 * 2. Implement a function to check if a specific key exists in the tree
 *
 * 3. Implement level-order (BFS) traversal using a queue
 *
 * 4. Add support for storing additional data with each key (key-value pairs)
 *
 * 5. Implement a function to find the kth smallest element
 *
 * =============================================================================
 */
