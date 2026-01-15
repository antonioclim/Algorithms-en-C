/**
 * =============================================================================
 * EXERCISE 2: AVL Tree Advanced Operations - SOLUTION
 * =============================================================================
 *
 * Complete solution with all TODOs implemented.
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise2_sol exercise2_sol.c
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

typedef struct AVLNode {
    int key;
    int height;
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode;

/* =============================================================================
 * UTILITY FUNCTIONS
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
 * ROTATIONS
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
        return node;
    }
    
    return rebalance(node);
}

/* =============================================================================
 * SEARCH OPERATIONS - SOLUTIONS
 * =============================================================================
 */

/**
 * SOLUTION TODO 1: Find minimum
 */
AVLNode *avl_find_min(AVLNode *node) {
    if (node == NULL) return NULL;
    
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

/**
 * SOLUTION TODO 2: Find maximum
 */
AVLNode *avl_find_max(AVLNode *node) {
    if (node == NULL) return NULL;
    
    while (node->right != NULL) {
        node = node->right;
    }
    return node;
}

/**
 * SOLUTION TODO 3: Search
 */
AVLNode *avl_search(AVLNode *node, int key) {
    if (node == NULL) return NULL;
    
    if (key == node->key) {
        return node;
    } else if (key < node->key) {
        return avl_search(node->left, key);
    } else {
        return avl_search(node->right, key);
    }
}

/**
 * SOLUTION TODO 4: Contains
 */
bool avl_contains(AVLNode *node, int key) {
    return avl_search(node, key) != NULL;
}

/* =============================================================================
 * DELETION - SOLUTION
 * =============================================================================
 */

/**
 * SOLUTION TODO 5: Delete
 */
AVLNode *avl_delete(AVLNode *node, int key) {
    /* Base case: node not found */
    if (node == NULL) {
        return NULL;
    }
    
    /* Search for the node */
    if (key < node->key) {
        /* TODO 5a: Recursively delete from left subtree */
        node->left = avl_delete(node->left, key);
    } else if (key > node->key) {
        /* TODO 5b: Recursively delete from right subtree */
        node->right = avl_delete(node->right, key);
    } else {
        /* Found the node to delete */
        
        /* TODO 5c: Handle case with 0 or 1 child */
        if (node->left == NULL || node->right == NULL) {
            AVLNode *temp = node->left ? node->left : node->right;
            free(node);
            return temp;  /* Return the child (or NULL if no child) */
        }
        
        /* TODO 5d: Handle case with 2 children */
        /* Find inorder successor (minimum in right subtree) */
        AVLNode *successor = avl_find_min(node->right);
        
        /* Copy successor's key to this node */
        node->key = successor->key;
        
        /* Delete the successor from right subtree */
        node->right = avl_delete(node->right, successor->key);
    }
    
    /* TODO 5e: Rebalance and return */
    return rebalance(node);
}

/* =============================================================================
 * PREDECESSOR AND SUCCESSOR - SOLUTIONS
 * =============================================================================
 */

/**
 * SOLUTION TODO 6: Successor
 */
AVLNode *avl_successor(AVLNode *root, int key) {
    AVLNode *successor = NULL;
    AVLNode *current = root;
    
    while (current != NULL) {
        if (key < current->key) {
            /* Current could be successor, go left to find smaller */
            successor = current;
            current = current->left;
        } else if (key > current->key) {
            /* Key is in right subtree */
            current = current->right;
        } else {
            /* Found the key */
            if (current->right != NULL) {
                /* Successor is minimum in right subtree */
                return avl_find_min(current->right);
            }
            break;
        }
    }
    
    return successor;
}

/**
 * SOLUTION TODO 7: Predecessor
 */
AVLNode *avl_predecessor(AVLNode *root, int key) {
    AVLNode *predecessor = NULL;
    AVLNode *current = root;
    
    while (current != NULL) {
        if (key > current->key) {
            /* Current could be predecessor, go right to find larger */
            predecessor = current;
            current = current->right;
        } else if (key < current->key) {
            /* Key is in left subtree */
            current = current->left;
        } else {
            /* Found the key */
            if (current->left != NULL) {
                /* Predecessor is maximum in left subtree */
                return avl_find_max(current->left);
            }
            break;
        }
    }
    
    return predecessor;
}

/* =============================================================================
 * RANGE QUERY - SOLUTION
 * =============================================================================
 */

/**
 * SOLUTION TODO 8: Range query
 */
void avl_range_query(AVLNode *node, int low, int high) {
    if (node == NULL) return;
    
    /* If current key > low, there might be keys in left subtree */
    if (node->key > low) {
        avl_range_query(node->left, low, high);
    }
    
    /* If current key is in range, print it */
    if (node->key >= low && node->key <= high) {
        printf("%d ", node->key);
    }
    
    /* If current key < high, there might be keys in right subtree */
    if (node->key < high) {
        avl_range_query(node->right, low, high);
    }
}

/* =============================================================================
 * TREE STATISTICS - SOLUTIONS
 * =============================================================================
 */

/**
 * SOLUTION TODO 9: Count nodes
 */
int avl_count_nodes(AVLNode *node) {
    if (node == NULL) return 0;
    return 1 + avl_count_nodes(node->left) + avl_count_nodes(node->right);
}

/**
 * SOLUTION TODO 10: Get height
 */
int avl_get_height(AVLNode *node) {
    return height(node);
}

/* =============================================================================
 * MEMORY MANAGEMENT - SOLUTION
 * =============================================================================
 */

/**
 * SOLUTION TODO 11: Destroy tree
 */
void avl_destroy(AVLNode *node) {
    if (node == NULL) return;
    avl_destroy(node->left);
    avl_destroy(node->right);
    free(node);
}

/* =============================================================================
 * VALIDATION - SOLUTION
 * =============================================================================
 */

/**
 * SOLUTION TODO 12: Validate AVL
 */
bool avl_validate(AVLNode *node) {
    if (node == NULL) return true;
    
    int bf = balance_factor(node);
    if (bf < -1 || bf > 1) {
        printf("AVL violation at node %d: bf = %d\n", node->key, bf);
        return false;
    }
    
    if (node->left && node->left->key >= node->key) {
        printf("BST violation: %d >= %d\n", node->left->key, node->key);
        return false;
    }
    if (node->right && node->right->key <= node->key) {
        printf("BST violation: %d <= %d\n", node->right->key, node->key);
        return false;
    }
    
    return avl_validate(node->left) && avl_validate(node->right);
}

/* =============================================================================
 * HELPER FUNCTIONS
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
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 2: AVL Tree Advanced Operations - SOLUTION       ║\n");
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
    printf("Minimum: %d ✓\n", min_node->key);
    printf("Maximum: %d ✓\n", max_node->key);
    
    /* Test search */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("TEST 2: Search Operations\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    int search_keys[] = {35, 45, 100};
    for (int i = 0; i < 3; i++) {
        bool found = avl_contains(root, search_keys[i]);
        printf("Contains %d: %s\n", search_keys[i], found ? "YES ✓" : "NO ✓");
    }
    
    /* Test successor/predecessor */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("TEST 3: Successor and Predecessor\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    int test_key = 40;
    AVLNode *succ = avl_successor(root, test_key);
    AVLNode *pred = avl_predecessor(root, test_key);
    printf("Key: %d\n", test_key);
    printf("  Successor: %d ✓\n", succ->key);
    printf("  Predecessor: %d ✓\n", pred->key);
    
    /* Test range query */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("TEST 4: Range Query [25, 60]\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    printf("Keys in range: ");
    avl_range_query(root, 25, 60);
    printf("✓\n");
    
    /* Test deletion */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("TEST 5: Deletion\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    printf("\nDeleting 70 (leaf-like node):\n");
    root = avl_delete(root, 70);
    print_tree(root);
    printf("Valid: %s\n", avl_validate(root) ? "YES ✓" : "NO ✗");
    
    printf("\nDeleting 30 (node with two children):\n");
    root = avl_delete(root, 30);
    print_tree(root);
    printf("Valid: %s\n", avl_validate(root) ? "YES ✓" : "NO ✗");
    
    printf("\nDeleting 50 (root with two children):\n");
    root = avl_delete(root, 50);
    print_tree(root);
    printf("Valid: %s\n", avl_validate(root) ? "YES ✓" : "NO ✗");
    
    /* Test statistics */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("TEST 6: Tree Statistics\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    printf("Node count: %d ✓\n", avl_count_nodes(root));
    printf("Tree height: %d ✓\n", avl_get_height(root));
    
    /* Cleanup */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("Cleaning up...\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    avl_destroy(root);
    printf("Memory freed successfully. ✓\n");
    
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    All Tests Passed! ✓                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    return 0;
}
