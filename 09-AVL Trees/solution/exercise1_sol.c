/**
 * =============================================================================
 * EXERCISE 1: AVL Tree Foundation - SOLUTION
 * =============================================================================
 *
 * Complete solution with all TODOs implemented.
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* =============================================================================
 * TYPE DEFINITIONS - SOLUTION
 * =============================================================================
 */

/**
 * SOLUTION TODO 1: Complete AVLNode structure
 */
typedef struct AVLNode {
    int key;
    int height;
    struct AVLNode *left;
    struct AVLNode *right;
} AVLNode;

/* =============================================================================
 * UTILITY FUNCTIONS - SOLUTIONS
 * =============================================================================
 */

/**
 * SOLUTION TODO 2: height function
 */
int height(AVLNode *node) {
    if (node == NULL) return -1;
    return node->height;
}

/**
 * SOLUTION TODO 3: balance_factor function
 */
int balance_factor(AVLNode *node) {
    if (node == NULL) return 0;
    return height(node->left) - height(node->right);
}

/**
 * Update height helper (provided)
 */
void update_height(AVLNode *node) {
    if (node == NULL) return;
    int left_h = height(node->left);
    int right_h = height(node->right);
    node->height = 1 + (left_h > right_h ? left_h : right_h);
}

/**
 * Create node helper (provided)
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
 * ROTATIONS - SOLUTIONS
 * =============================================================================
 */

/**
 * SOLUTION TODO 4: Right rotation
 */
AVLNode *rotate_right(AVLNode *y) {
    printf("  Performing RIGHT rotation on %d\n", y->key);
    
    /* Step 1: Save y's left child as x */
    AVLNode *x = y->left;
    
    /* Step 2: Save x's right child as B */
    AVLNode *B = x->right;
    
    /* Step 3: Make y the right child of x */
    x->right = y;
    
    /* Step 4: Make B the left child of y */
    y->left = B;
    
    /* Step 5: Update heights (y first, then x) */
    update_height(y);
    update_height(x);
    
    /* Step 6: Return x (new root) */
    return x;
}

/**
 * SOLUTION TODO 5: Left rotation
 */
AVLNode *rotate_left(AVLNode *x) {
    printf("  Performing LEFT rotation on %d\n", x->key);
    
    /* Step 1: Save x's right child as y */
    AVLNode *y = x->right;
    
    /* Step 2: Save y's left child as B */
    AVLNode *B = y->left;
    
    /* Step 3: Make x the left child of y */
    y->left = x;
    
    /* Step 4: Make B the right child of x */
    x->right = B;
    
    /* Step 5: Update heights (x first, then y) */
    update_height(x);
    update_height(y);
    
    /* Step 6: Return y (new root) */
    return y;
}

/* =============================================================================
 * REBALANCING - SOLUTION
 * =============================================================================
 */

/**
 * SOLUTION TODO 6: Rebalance function
 */
AVLNode *rebalance(AVLNode *node) {
    /* Step 1: Handle NULL case */
    if (node == NULL) return NULL;
    
    /* Step 2: Update height of current node */
    update_height(node);
    
    /* Step 3: Calculate balance factor */
    int bf = balance_factor(node);
    
    /* Step 4: Left-heavy case (bf > 1) */
    if (bf > 1) {
        /* Check for LR case */
        if (balance_factor(node->left) < 0) {
            printf("  LR Case at node %d\n", node->key);
            node->left = rotate_left(node->left);
        } else {
            printf("  LL Case at node %d\n", node->key);
        }
        return rotate_right(node);
    }
    
    /* Step 5: Right-heavy case (bf < -1) */
    if (bf < -1) {
        /* Check for RL case */
        if (balance_factor(node->right) > 0) {
            printf("  RL Case at node %d\n", node->key);
            node->right = rotate_right(node->right);
        } else {
            printf("  RR Case at node %d\n", node->key);
        }
        return rotate_left(node);
    }
    
    /* Step 6: Already balanced, return node */
    return node;
}

/* =============================================================================
 * INSERTION - SOLUTION
 * =============================================================================
 */

/**
 * SOLUTION TODO 7: AVL insertion
 */
AVLNode *avl_insert(AVLNode *node, int key) {
    /* Step 1: If node is NULL, create and return new node */
    if (node == NULL) {
        return create_node(key);
    }
    
    /* Step 2-4: Recurse into appropriate subtree */
    if (key < node->key) {
        node->left = avl_insert(node->left, key);
    } else if (key > node->key) {
        node->right = avl_insert(node->right, key);
    } else {
        /* Duplicate key - no insertion */
        return node;
    }
    
    /* Step 5: Rebalance and return */
    return rebalance(node);
}

/* =============================================================================
 * TRAVERSAL - SOLUTION
 * =============================================================================
 */

/**
 * SOLUTION TODO 8: Inorder traversal
 */
void avl_inorder(AVLNode *node) {
    if (node == NULL) return;
    avl_inorder(node->left);
    printf("%d ", node->key);
    avl_inorder(node->right);
}

/* =============================================================================
 * HELPER FUNCTIONS (PROVIDED)
 * =============================================================================
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

void avl_destroy(AVLNode *node) {
    if (node == NULL) return;
    avl_destroy(node->left);
    avl_destroy(node->right);
    free(node);
}

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
    printf("║       EXERCISE 1: AVL Tree Foundation - SOLUTION              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    AVLNode *root = NULL;
    
    /* Test Case 1: LL Case */
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
    printf("\nValid AVL: %s\n", validate_avl(root) ? "YES ✓" : "NO ✗");
    avl_destroy(root);
    
    /* Test Case 2: RR Case */
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
    printf("\nValid AVL: %s\n", validate_avl(root) ? "YES ✓" : "NO ✗");
    avl_destroy(root);
    
    /* Test Case 3: LR Case */
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
    printf("\nValid AVL: %s\n", validate_avl(root) ? "YES ✓" : "NO ✗");
    avl_destroy(root);
    
    /* Test Case 4: RL Case */
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
    printf("\nValid AVL: %s\n", validate_avl(root) ? "YES ✓" : "NO ✗");
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
    printf("\nValid AVL: %s\n", validate_avl(root) ? "YES ✓" : "NO ✗");
    avl_destroy(root);
    
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    All Tests Passed! ✓                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    return 0;
}
