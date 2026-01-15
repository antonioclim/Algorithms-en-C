/**
 * =============================================================================
 * EXERCISE 2: AVL Tree Advanced Operations
 * =============================================================================
 *
 * OBJECTIVE:
 *   Extend your AVL tree implementation with deletion, search operations
 *   and range queries. This exercise focuses on more complex tree manipulation.
 *
 * REQUIREMENTS:
 *   1. Implement avl_find_min() and avl_find_max()
 *   2. Implement avl_search() for key lookup
 *   3. Implement avl_delete() with proper rebalancing
 *   4. Implement avl_predecessor() and avl_successor()
 *   5. Implement avl_range_query() to find keys in [low, high]
 *   6. Implement avl_count_nodes() and avl_get_height()
 *   7. Implement avl_destroy() for memory cleanup
 *   8. Add validation to verify AVL property after operations
 *
 * EXAMPLE INPUT:
 *   Build tree: 50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45
 *   Delete: 70, then 50
 *   Range query: [25, 60]
 *
 * EXPECTED OUTPUT:
 *   Tree remains balanced after each deletion
 *   Range query returns: 25, 30, 35, 40, 45, 50, 60
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* =============================================================================
 * TYPE DEFINITIONS (PROVIDED)
 * =============================================================================
 */

typedef struct AVLNode {
    int key;
    int height;
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode;

/* =============================================================================
 * UTILITY FUNCTIONS (PROVIDED)
 * =============================================================================
 */

int height(AVLNode *node) {
    if (node == NULL) return -1;
    return node->height;
}

int balance_factor(AVLNode *node) {
    if (node == NULL) return 0;
    return height(node->left) - height(node->right);
}

void update_height(AVLNode *node) {
    if (node == NULL) return;
    int left_h = height(node->left);
    int right_h = height(node->right);
    node->height = 1 + (left_h > right_h ? left_h : right_h);
}

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
 * ROTATIONS (PROVIDED)
 * =============================================================================
 */

AVLNode *rotate_right(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *B = x->right;
    
    x->right = y;
    y->left = B;
    
    update_height(y);
    update_height(x);
    
    return x;
}

AVLNode *rotate_left(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *B = y->left;
    
    y->left = x;
    x->right = B;
    
    update_height(x);
    update_height(y);
    
    return y;
}

AVLNode *rebalance(AVLNode *node) {
    if (node == NULL) return NULL;
    
    update_height(node);
    int bf = balance_factor(node);
    
    if (bf > 1) {
        if (balance_factor(node->left) < 0) {
            node->left = rotate_left(node->left);
        }
        return rotate_right(node);
    }
    
    if (bf < -1) {
        if (balance_factor(node->right) > 0) {
            node->right = rotate_right(node->right);
        }
        return rotate_left(node);
    }
    
    return node;
}

AVLNode *avl_insert(AVLNode *node, int key) {
    if (node == NULL) {
        return create_node(key);
    }
    
    if (key < node->key) {
        node->left = avl_insert(node->left, key);
    } else if (key > node->key) {
        node->right = avl_insert(node->right, key);
    } else {
        return node;  /* Duplicate */
    }
    
    return rebalance(node);
}

/* =============================================================================
 * SEARCH OPERATIONS
 * =============================================================================
 */

/**
 * TODO 1: Implement avl_find_min
 *
 * Finds the node with the minimum key in the subtree.
 *
 * @param node Root of the subtree
 * @return Pointer to the node with minimum key, or NULL if tree is empty
 *
 * Hint: The minimum is always in the leftmost node.
 *       Keep going left until you can't go left anymore.
 */
AVLNode *avl_find_min(AVLNode *node) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace this */
}

/**
 * TODO 2: Implement avl_find_max
 *
 * Finds the node with the maximum key in the subtree.
 *
 * @param node Root of the subtree
 * @return Pointer to the node with maximum key, or NULL if tree is empty
 *
 * Hint: The maximum is always in the rightmost node.
 */
AVLNode *avl_find_max(AVLNode *node) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace this */
}

/**
 * TODO 3: Implement avl_search
 *
 * Searches for a key in the AVL tree.
 *
 * @param node Root of the tree
 * @param key Key to search for
 * @return Pointer to the node if found, NULL otherwise
 *
 * Steps:
 *   1. If node is NULL, return NULL (not found)
 *   2. If key equals node's key, return node (found)
 *   3. If key < node's key, search left subtree
 *   4. If key > node's key, search right subtree
 */
AVLNode *avl_search(AVLNode *node, int key) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Replace this */
}

/**
 * TODO 4: Implement avl_contains
 *
 * Checks if a key exists in the tree.
 *
 * @param node Root of the tree
 * @param key Key to check
 * @return true if key exists, false otherwise
 *
 * Hint: Use avl_search
 */
bool avl_contains(AVLNode *node, int key) {
    /* YOUR CODE HERE */
    
    return false;  /* Replace this */
}

/* =============================================================================
 * DELETION
 * =============================================================================
 */

/**
 * TODO 5: Implement avl_delete
 *
 * Deletes a key from the AVL tree, maintaining balance.
 *
 * @param node Root of the subtree
 * @param key Key to delete
 * @return New root of the subtree (may change due to deletion/rotation)
 *
 * Cases:
 *   1. Node not found: return NULL
 *   2. Leaf node (no children): free and return NULL
 *   3. One child: free node and return the child
 *   4. Two children: 
 *      a. Find inorder successor (min in right subtree)
 *      b. Copy successor's key to current node
 *      c. Delete successor from right subtree
 *
 * IMPORTANT: 
 *   - Always rebalance before returning
 *   - Free memory for deleted nodes
 *   - Handle the two-children case carefully
 *
 * Steps:
 *   1. Standard BST search for the key
 *   2. When found, handle the three cases above
 *   3. Rebalance and return
 */
AVLNode *avl_delete(AVLNode *node, int key) {
    /* Base case: node not found */
    if (node == NULL) {
        return NULL;
    }
    
    /* Search for the node */
    if (key < node->key) {
        /* TODO 5a: Recursively delete from left subtree */
        /* YOUR CODE HERE */
        
    } else if (key > node->key) {
        /* TODO 5b: Recursively delete from right subtree */
        /* YOUR CODE HERE */
        
    } else {
        /* Found the node to delete */
        
        /* TODO 5c: Handle case with 0 or 1 child */
        if (node->left == NULL || node->right == NULL) {
            /* YOUR CODE HERE */
            
        }
        
        /* TODO 5d: Handle case with 2 children */
        /* Find inorder successor, copy key, delete successor */
        /* YOUR CODE HERE */
        
    }
    
    /* TODO 5e: Rebalance and return */
    /* YOUR CODE HERE */
    
    return node;  /* Replace with proper return */
}

/* =============================================================================
 * PREDECESSOR AND SUCCESSOR
 * =============================================================================
 */

/**
 * TODO 6: Implement avl_successor
 *
 * Finds the inorder successor of the given key.
 * The successor is the smallest key greater than the given key.
 *
 * @param node Root of the tree
 * @param key The key to find successor for
 * @return Pointer to successor node, or NULL if no successor exists
 *
 * Algorithm:
 *   1. Search for the key, keeping track of the last node where we went left
 *   2. If found and has right child, successor is min of right subtree
 *   3. Otherwise, successor is the ancestor where we last went left
 *
 * Hint: Think about two cases:
 *   - Key has right subtree: successor is min of right subtree
 *   - Key has no right subtree: successor is nearest ancestor for which
 *     the key is in the left subtree
 */
AVLNode *avl_successor(AVLNode *root, int key) {
    AVLNode *successor = NULL;
    AVLNode *current = root;
    
    /* YOUR CODE HERE */
    
    return successor;
}

/**
 * TODO 7: Implement avl_predecessor
 *
 * Finds the inorder predecessor of the given key.
 * The predecessor is the largest key smaller than the given key.
 *
 * @param node Root of the tree
 * @param key The key to find predecessor for
 * @return Pointer to predecessor node, or NULL if no predecessor exists
 *
 * Hint: Mirror of successor logic
 */
AVLNode *avl_predecessor(AVLNode *root, int key) {
    AVLNode *predecessor = NULL;
    AVLNode *current = root;
    
    /* YOUR CODE HERE */
    
    return predecessor;
}

/* =============================================================================
 * RANGE QUERY
 * =============================================================================
 */

/**
 * TODO 8: Implement avl_range_query
 *
 * Finds all keys in the range [low, high] (inclusive).
 * Prints them in sorted order.
 *
 * @param node Root of the tree
 * @param low Lower bound of range
 * @param high Upper bound of range
 *
 * Algorithm (modified inorder traversal):
 *   1. If current key > low, recursively search left subtree
 *   2. If current key is in range [low, high], print it
 *   3. If current key < high, recursively search right subtree
 *
 * This is more efficient than full traversal + filtering!
 */
void avl_range_query(AVLNode *node, int low, int high) {
    /* YOUR CODE HERE */
}

/* =============================================================================
 * TREE STATISTICS
 * =============================================================================
 */

/**
 * TODO 9: Implement avl_count_nodes
 *
 * Counts the total number of nodes in the tree.
 *
 * @param node Root of the tree
 * @return Number of nodes
 */
int avl_count_nodes(AVLNode *node) {
    /* YOUR CODE HERE */
    
    return 0;  /* Replace this */
}

/**
 * TODO 10: Implement avl_get_height
 *
 * Returns the height of the tree.
 *
 * @param node Root of the tree
 * @return Height of tree (-1 for empty tree)
 */
int avl_get_height(AVLNode *node) {
    /* YOUR CODE HERE */
    
    return -1;  /* Replace this */
}

/* =============================================================================
 * MEMORY MANAGEMENT
 * =============================================================================
 */

/**
 * TODO 11: Implement avl_destroy
 *
 * Frees all memory used by the tree.
 * Use postorder traversal: delete children before parent.
 *
 * @param node Root of the tree
 *
 * Steps:
 *   1. If node is NULL, return
 *   2. Recursively destroy left subtree
 *   3. Recursively destroy right subtree
 *   4. Free current node
 */
void avl_destroy(AVLNode *node) {
    /* YOUR CODE HERE */
}

/* =============================================================================
 * VALIDATION
 * =============================================================================
 */

/**
 * TODO 12: Implement avl_validate
 *
 * Validates that the tree satisfies both BST and AVL properties.
 *
 * @param node Root of the tree
 * @return true if valid AVL tree, false otherwise
 *
 * Checks:
 *   1. BST property: left < node < right
 *   2. AVL property: |balance factor| <= 1 for all nodes
 *   3. Height values are correct
 */
bool avl_validate(AVLNode *node) {
    if (node == NULL) return true;
    
    /* Check balance factor */
    int bf = balance_factor(node);
    if (bf < -1 || bf > 1) {
        printf("AVL violation at node %d: bf = %d\n", node->key, bf);
        return false;
    }
    
    /* Check BST property */
    if (node->left && node->left->key >= node->key) {
        printf("BST violation: %d >= %d\n", node->left->key, node->key);
        return false;
    }
    if (node->right && node->right->key <= node->key) {
        printf("BST violation: %d <= %d\n", node->right->key, node->key);
        return false;
    }
    
    /* Recursively validate subtrees */
    return avl_validate(node->left) && avl_validate(node->right);
}

/* =============================================================================
 * HELPER FUNCTIONS (PROVIDED)
 * =============================================================================
 */

void avl_inorder(AVLNode *node) {
    if (node == NULL) return;
    avl_inorder(node->left);
    printf("%d ", node->key);
    avl_inorder(node->right);
}

void print_tree_helper(AVLNode *node, int depth, char prefix) {
    if (node == NULL) return;
    print_tree_helper(node->right, depth + 1, '/');
    for (int i = 0; i < depth; i++) printf("    ");
    printf("%c--[%d](h=%d,bf=%d)\n", prefix, node->key, node->height, balance_factor(node));
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

/* =============================================================================
 * MAIN PROGRAM - TEST CASES
 * =============================================================================
 */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║         EXERCISE 2: AVL Tree Advanced Operations              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    AVLNode *root = NULL;
    
    /* Build initial tree */
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("Building tree: 50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    int values[] = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
    int n = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < n; i++) {
        root = avl_insert(root, values[i]);
    }
    
    print_tree(root);
    printf("Inorder: ");
    avl_inorder(root);
    printf("\n");
    
    /* Test min/max */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("TEST 1: Find Min/Max\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    AVLNode *min_node = avl_find_min(root);
    AVLNode *max_node = avl_find_max(root);
    printf("Minimum: %s\n", min_node ? (char[20]){0} : "NULL");
    if (min_node) printf("Minimum: %d\n", min_node->key);
    printf("Maximum: %s\n", max_node ? (char[20]){0} : "NULL");
    if (max_node) printf("Maximum: %d\n", max_node->key);
    
    /* Test search */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("TEST 2: Search Operations\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    int search_keys[] = {35, 45, 100};
    for (int i = 0; i < 3; i++) {
        bool found = avl_contains(root, search_keys[i]);
        printf("Contains %d: %s\n", search_keys[i], found ? "YES" : "NO");
    }
    
    /* Test successor/predecessor */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("TEST 3: Successor and Predecessor\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    int test_key = 40;
    AVLNode *succ = avl_successor(root, test_key);
    AVLNode *pred = avl_predecessor(root, test_key);
    printf("Key: %d\n", test_key);
    printf("  Successor: %s\n", succ ? (char[20]){0} : "NULL");
    if (succ) printf("  Successor: %d\n", succ->key);
    printf("  Predecessor: %s\n", pred ? (char[20]){0} : "NULL");
    if (pred) printf("  Predecessor: %d\n", pred->key);
    
    /* Test range query */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("TEST 4: Range Query [25, 60]\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    printf("Keys in range: ");
    avl_range_query(root, 25, 60);
    printf("\n");
    
    /* Test deletion */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("TEST 5: Deletion\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    printf("\nDeleting 70 (leaf node):\n");
    root = avl_delete(root, 70);
    print_tree(root);
    printf("Valid: %s\n", avl_validate(root) ? "YES" : "NO");
    
    printf("\nDeleting 30 (node with two children):\n");
    root = avl_delete(root, 30);
    print_tree(root);
    printf("Valid: %s\n", avl_validate(root) ? "YES" : "NO");
    
    printf("\nDeleting 50 (root with two children):\n");
    root = avl_delete(root, 50);
    print_tree(root);
    printf("Valid: %s\n", avl_validate(root) ? "YES" : "NO");
    
    /* Test statistics */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("TEST 6: Tree Statistics\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    printf("Node count: %d\n", avl_count_nodes(root));
    printf("Tree height: %d\n", avl_get_height(root));
    
    /* Cleanup */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("Cleaning up...\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    avl_destroy(root);
    printf("Memory freed successfully.\n");
    
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    Exercise Complete                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    return 0;
}

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Implement avl_floor(key) - largest key <= given key
 *
 * 2. Implement avl_ceiling(key) - smallest key >= given key
 *
 * 3. Implement avl_rank(key) - number of keys less than given key
 *
 * 4. Implement avl_select(k) - kth smallest key
 *
 * 5. Implement avl_range_count(low, high) - count keys in range
 *
 * =============================================================================
 */
